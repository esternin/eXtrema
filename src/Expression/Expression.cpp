/*
Copyright (C) 2005 Joseph L. Chuma, TRIUMF
  
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
  
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <algorithm>
#include <memory>

#include "Expression.h"
#include "EExpressionError.h"
#include "ExprCodes.h"
#include "Workspace.h"
#include "EVariableError.h"
#include "NVariableTable.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "OperatorDefinition.h"
#include "OperatorTable.h"
#include "FunctionDefinition.h"

Expression::Expression( wxString const &s )
    : string_( s ), highestLevel_( 0 )
{
  //
  // trim leading and trailing blanks
  //
  wxChar const blank( wxT(' ') );
  std::size_t rTemp = string_.find_last_not_of( blank );
  std::size_t lTemp = string_.find_first_not_of( blank );

  //std::cout << "rTemp= " << rTemp << ", lTemp= " << lTemp << "\n";

  string_ = string_.substr( lTemp, rTemp-lTemp+1 );
  
  Workspace *ws = new Workspace(this);
  ws->SetLevel( highestLevel_ );
  levels_.insert( WSEntryType(highestLevel_,ws) );
  isaFit_ = false;
  OperatorTable::GetTable()->GetOperator(wxT("//"))->SetCharacter();

  //std::cout << "1: s= |" << s.mb_str(wxConvUTF8) << "|\n";

}
  
Expression::Expression( Expression const &rhs )
{ CopyStuff(rhs); }

Expression & Expression::operator=( Expression const &rhs )
{
  if( &rhs != this )CopyStuff(rhs);
  return *this;
}

Expression::~Expression()
{
  WSLevel::const_iterator end( levels_.end() );
  for( WSLevel::const_iterator i(levels_.begin()); i != end; ++i )
  {
    Workspace *ws = (*i).second;
    if( ws != 0 )delete ws;
  }
}

void Expression::CopyStuff( Expression const &rhs )
{
  levels_ = rhs.levels_;
  highestLevel_ = rhs.highestLevel_;
  string_ = rhs.string_;
  type_ = rhs.type_;
  finalData_ = rhs.finalData_;
  finalString_ = rhs.finalString_;
  isaFit_ = rhs.isaFit_;
  fitParameters_ = rhs.fitParameters_;
}
  
NumericData &Expression::GetFinalAnswer()
{ return finalData_; }

void Expression::AddWorkspace( Workspace *w )
{
  int level = w->GetLevel();
  levels_.insert( WSEntryType(level,w) );
  if( level > highestLevel_ )highestLevel_ = level;
}

void Expression::RemoveWorkspace( Workspace *w )
{
  WSLevelValuePair lvp = std::make_pair( w->GetLevel(), w );
  levels_.erase( find( levels_.begin(), levels_.end(), lvp ) );
}

Workspace *Expression::GetFinalWorkspace()
{ return (*levels_.find(0)).second; }

void Expression::GetWorkspaces( int level, std::vector<Workspace*> &wsv )
{
  std::vector<Workspace*>().swap( wsv );
  WSLevelPair lp = levels_.equal_range( level );
  WSLevel::const_iterator end( lp.second );
  for( WSLevel::const_iterator k(lp.first); k!=end; ++k )
  {
    Workspace *ws = (*k).second;
    wsv.push_back( ws );
  }
}

void Expression::TestFitParameterName( wxString &name )
{
  std::vector<wxString>::const_iterator end( fitParameters_.end() );
  for( std::vector<wxString>::const_iterator i = fitParameters_.begin(); i != end; ++i )
  {
    if( *i == name )return; // same fitting parameter used more than once
  }
  fitParameters_.push_back( name );
}

double Expression::GetFitParameterValue( std::size_t i ) const
{
  return NVariableTable::GetTable()->
      GetVariable(fitParameters_[i])->GetData().GetScalarValue();
}

void Expression::SetFitParameterValue( std::size_t i, double v )
{
  NVariableTable::GetTable()->
      GetVariable(fitParameters_[i])->GetData().SetScalarValue( v );
}

void Expression::SetFitParameterValue( std::size_t i, double v, wxString const &s )
{
  NumericVariable *nv =
      NVariableTable::GetTable()->GetVariable( fitParameters_[i] );
  nv->GetData().SetScalarValue( v );
  nv->AddToHistory( s );
}

void Expression::Evaluate()
{
  Workspace *w0 = (*levels_.find(0)).second;
  w0->ParseAndCheck( false );
  w0->HandleFunctionsAndText();
  bool doOver = true;
  while( doOver )
  {
    doOver = false;
    for( int level=highestLevel_; level>=1; --level )
    {
      std::vector<Workspace*> wsv;
      GetWorkspaces( level, wsv );
      std::vector<Workspace*>::const_iterator end( wsv.end() );
      for( std::vector<Workspace*>::const_iterator i=wsv.begin(); i!=end; ++i )
      {
        Workspace *ws = *i;
        if( ws->IsFinished() )continue;
        if( ws->HandleFunctionsAndText() )
        {
          doOver = true;
          break;
        }
      }
      if( doOver )break;
    }
  }
  bool newWorkspaceMade;
  do
  {
    for( int level=highestLevel_; level>=0; --level )
    {
      newWorkspaceMade = false;
      //
      // loop over all Workspaces at each level
      //
      std::vector<Workspace*> wsv;
      GetWorkspaces( level, wsv );
      std::vector<Workspace*>::const_iterator end( wsv.end() );
      for( std::vector<Workspace*>::const_iterator i=wsv.begin(); i!=end; ++i )
      {
        Workspace *s = *i;
        if( s->IsFinished() )continue;     // go to next Workspace at this level
        std::deque<ExprCodes*> &sCodes = s->GetCodes();
        std::size_t sCodesSize = sCodes.size();
        for( std::size_t j=0; j<sCodesSize; ++j )
        {
          ExprCodes *sjCode = sCodes[j];
          if( sjCode->IsaWorkspace() )
          {
            Workspace *t = sjCode->GetWorkspacePtr();
            if( t->IsNumeric() )
            {
              if( !t->EvaluateNow() )
              {
                ExprCodes *etmp = sjCode;
                std::deque<ExprCodes*> &tCodes = t->GetCodes();
                sCodes.insert( sCodes.begin()+j, tCodes.begin(), tCodes.end() );
                j += tCodes.size();
                sCodesSize += tCodes.size() - 1;
                sCodes.erase( sCodes.begin()+j );
                std::deque<ExprCodes*>().swap( tCodes );
                RemoveWorkspace( t );
                delete t;
                delete etmp;
              }
            }
            else if( t->IsCharacter() )
            {
              sjCode->SetTString( t->GetFinalString() );
            }
          }
          else if( sjCode->IsaTVariable() )
          {
            TextVariable *tv = sjCode->GetTVarPtr();
            if( s->IsNumeric() )
            {
              NumericData *ia = 0;
              Workspace *iaw = sjCode->GetTAIndexPtr();
              if( iaw )ia = &iaw->GetFinalData();
              NumericData *ic = 0;
              Workspace *icw = sjCode->GetTCIndexPtr();
              if( icw )ic = &icw->GetFinalData();
              wxString str( wxT('(') );
              wxString tmp;
              tmp = tv->GetString(ia,ic);
              //wxChar *c = new wxChar[tmp.length()+1];
              //strcpy( c, tmp.c_str() );
              //str.append( c );
              //delete [] c;
              str += tmp + wxT(')');
              Workspace *ws = new Workspace(str);
              ws->SetExpression( this );
              ws->SetLevel( level+1 );
              if( level+1 > highestLevel_ )highestLevel_ = level+1;
              ws->SetNumeric();
              if( IsaFit() )ws->SetEvaluateNow( false );
              else ws->SetEvaluateNow( true );
              AddWorkspace( ws );
              sjCode->SetWorkspacePtr( ws );
              newWorkspaceMade = true;
            }
          }
          else if( sjCode->IsaTString() )
          {
            wxString tmp( sjCode->GetTString() );
            if( s->IsNumeric() )
            {
              wxString str( wxT('(') );
              str += tmp + wxT(')');
              std::unique_ptr<Workspace> aws( new Workspace(str) );
              Workspace *ws = aws.release();
              ws->SetExpression( this );
              ws->SetLevel( level+1 );
              if( level+1 > highestLevel_ )highestLevel_ = level+1;
              ws->SetNumeric();
              if( IsaFit() )ws->SetEvaluateNow( false );
              else ws->SetEvaluateNow( true );
              AddWorkspace( ws );
              sjCode->SetWorkspacePtr( ws );
              newWorkspaceMade = true;
            }
          }   // end of tests for code
        }   // end of loop over codes
        if( level == 0 || newWorkspaceMade )continue;
        if( s->IsNumeric() && s->EvaluateNow() )
        {
          try
          {
            if( !s->IsRPNdone() )
            {
              //s->HandleFunctionsAndText();
              s->RPNconvert();
              s->SetHierarchy();
              s->SixthPass();
            }
            SeventhPass( s->GetLevel() );
          }
          catch( EExpressionError &e )
          {
            throw;
          }
          s->SetFinished();
        }
        else if( s->IsCharacter() )
        {
          //s->HandleFunctionsAndText();
          s->RPNTconvert();
          CalcText( s );
          s->SetFinished();
        }
      }    // end of loop over Workspaces at a level
      if( newWorkspaceMade && (level != highestLevel_) )
      {
        std::vector<Workspace*> wsv;
        GetWorkspaces( level+1, wsv );
        std::vector<Workspace*>::const_iterator end( wsv.end() );
        for( std::vector<Workspace*>::const_iterator i=wsv.begin(); i!=end; ++i )
        {
          (*i)->ParseAndCheck( false );
          (*i)->HandleFunctionsAndText();
        }
        break;  // break out of loop over levels
      }
      else
      {
        highestLevel_ = level;
      }
    }   // end of loop over levels
  } while( newWorkspaceMade );
  //
  // check that all Workspaces are finished
  //

  //std::cout << "highestLevel_=" << highestLevel_ << "\n";

  for( int level=1; level<=highestLevel_; ++level )
  {
    std::vector<Workspace*> wsv;
    GetWorkspaces( level, wsv );
    std::vector<Workspace*>::const_iterator end( wsv.end() );
    for( std::vector<Workspace*>::const_iterator i=wsv.begin(); i!=end; ++i )
    {
      Workspace *ws = *i;
      ws->HandleFunctionsAndText();
      if( ws->IsCharacter() )
      {
        ws->RPNTconvert();
        CalcText( ws );
        finalString_ = ws->GetFinalString();
      }
      else
      {
        ws->RPNconvert();
        ws->SetHierarchy();
        ws->SixthPass();
        SeventhPass( 0 );
        finalData_ = ws->GetFinalData();
      }
    }
  }
  Workspace *ws = GetFinalWorkspace();
  if( ws->IsCharacter() )
  {
    ws->RPNTconvert();
    CalcText( ws );
    finalString_ = ws->GetFinalString();
    type_ = CHARACTER;
  }
  else
  {
    ws->RPNconvert();
    ws->SetHierarchy();
    ws->SixthPass();
    SeventhPass( 0 );
    finalData_ = ws->GetFinalData();
    type_ = NUMERIC;
  }
}

void Expression::SeventhPass( int minLevel )
{
  for( int level=highestLevel_; level>=minLevel; --level )
  {
    std::vector<Workspace*> wsv;
    GetWorkspaces( level, wsv );
    std::vector<Workspace*>::const_iterator end( wsv.end() );
    for( std::vector<Workspace*>::const_iterator i(wsv.begin()); i!=end; ++i )
    {
      if( level==0 || ((*i)->IsRPNdone() && !(*i)->IsFinished()) )
      {
        (*i)->SetType();
        (*i)->SetUp();
        (*i)->Calculate();
      }
    }
  }
}

void Expression::FinalPass()
{
  for( int level=highestLevel_; level>=0; --level )
  {
    std::vector<Workspace*> wsv;
    GetWorkspaces( level, wsv );
    std::vector<Workspace*>::const_iterator end( wsv.end() );
    for( std::vector<Workspace*>::const_iterator i(wsv.begin()); i!=end; ++i )
    {
      if( level==0 || ((*i)->IsRPNdone() && !(*i)->IsFinished()) )(*i)->Calculate();
    }
  }
  finalData_ = GetFinalWorkspace()->GetFinalData();
  type_ = NUMERIC;
}

void Expression::CalcText( Workspace *ws )
{
  std::vector<wxString> sStack;
  std::vector<Workspace*> wStack;
  int j = -1;
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  std::deque<ExprCodes*>::const_iterator end = codes.end();
  for( std::deque<ExprCodes*>::const_iterator i=codes.begin(); i!=end; ++i )
  {
    ExprCodes *code = *i;
    if( code->IsaFunction() )
    {
      FunctionDefinition *fp = code->GetFcnPtr();
      int narg = code->GetArgCntr();
      fp->SetArgumentCounter( narg );
      j -= narg-1;
      if( fp->IsMixed2Char() )
      {
        fp->TextArrayEval( j, wStack, sStack );
      }
      else
      {
        fp->TextScalarEval( j, sStack );
      }
      for( int i=0; i<narg-1; ++i )
      {
        wStack.pop_back();
        sStack.pop_back();
      }
    }
    else if( code->IsaOperator() )
    {
      int narg = code->GetOpPtr()->OperandCounter();
      j -= narg-1;
      wxString s;
      code->GetOpPtr()->TextScalarEval( j, sStack );
      wStack.pop_back();
      sStack.pop_back();
    }
    else if( code->IsaTVariable() )
    {
      ++j;
      NumericData *ia = 0;
      Workspace *iaw = code->GetTAIndexPtr(); // array index workspace pointer
      if( iaw )ia = &iaw->GetFinalData();
      NumericData *ic = 0;
      Workspace *icw = code->GetTCIndexPtr(); // character index workspace pointer
      if( icw )ic = &icw->GetFinalData();
      try
      {
        sStack.push_back( code->GetTVarPtr()->GetString(ia,ic) );
      }
      catch ( EVariableError &e )
      {
        throw EExpressionError( wxString(e.what(),wxConvUTF8) );
      }
      wStack.push_back( reinterpret_cast<Workspace*>(0) );
    }
    else if( code->IsaWorkspace() )
    {
      ++j;
      Workspace *ws = code->GetWorkspacePtr();
      if( ws->IsCharacter() )
      {
        wStack.push_back( reinterpret_cast<Workspace*>(0) );
        sStack.push_back( ws->GetFinalString() );
      }
      else
      {
        wStack.push_back( ws );
        sStack.push_back( code->GetTString() );
      }
    }
    else if( code->IsaTString() )
    {
      ++j;
      sStack.push_back( code->GetTString() );
      wStack.push_back( reinterpret_cast<Workspace*>(0) );
    }
  }
  ws->SetFinalString( sStack.front() );
}

// end of file
