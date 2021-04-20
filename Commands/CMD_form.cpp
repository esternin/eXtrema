/*
Copyright (C) 2006 Joseph L. Chuma, TRIUMF

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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "CMD_form.h"

#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "ExGlobals.h"
#include "NumericVariable.h"
#include "NVariableTable.h"
#include "TextVariable.h"
#include "extremaMain.h"
#include "FitNovice.h"
#include "GraphNovice.h"
#include "ExecuteForm.h"
#include "TextInteractive.h"
#include "GenerateCommandForm.h"

CMD_form CMD_form::cmd_form_;

CMD_form::CMD_form() : Command( "FORM" )
{
  AddQualifier( "NOVICE", true );
}

void CMD_form::Execute( ParseLine const *p )
{
  // open one of the gui forms
  //
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  if( p->GetNumberOfTokens()<2 || !p->IsString(1) )
    throw ECommandError("FORM","expecting a form name");
  ExString form( p->GetString(1) );
  form.ToUpper();
  form.RemoveQuotes();
  AddToStackLine( form );
  int icnt = 2;
  if( form == "FIT" )
  {
    // FORM FIT,CLOSE
    // FORM FIT,'CLEAR GRAPHICS'
    // FORM FIT,dataVec, indepVec, idmin, idmax, errorVec, fitType, nparam, expression,
    //          fixedVec, startVec, xloc, yloc, action
    //  where action can be one of: "CLOSE", "CLEAR GRAPHICS", "TEST THE FIT", "DO THE FIT"
    //
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError("FORM","expecting data vector name or CLOSE or 'CLEAR GRAPHICS'");
    ExString dataName( p->GetString(icnt) );
    dataName.ToUpper();
    dataName.RemoveQuotes();
    if( dataName == "CLOSE" )
    {
      if( FitNoviceForm )
      {
        FitNoviceForm->WindowState = wsNormal;
        FitNoviceForm->Visible = true;
        FitNoviceForm->BringToFront();
        FitNoviceForm->CloseButtonClick( static_cast<TObject*>(0) );
      }
      return;
    }
    else if( dataName == "CLEAR GRAPHICS" )
    {
      if( FitNoviceForm )
      {
        FitNoviceForm->WindowState = wsNormal;
        FitNoviceForm->Visible = true;
        FitNoviceForm->BringToFront();
      }
      else
      {
        FitNoviceForm = new TFitNoviceForm( MainForm );
        FitNoviceForm->Show();
      }
      FitNoviceForm->ClearButtonClick( static_cast<TObject*>(0) );
      return;
    }
    NumericVariable *dataVec = NVariableTable::GetTable()->GetVariable(dataName);
    if( !dataVec )
      throw ECommandError("FORM",ExString("data vector ")+dataName+" does not exist");
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError("FORM","expecting independent vector name");
    ExString indepName( p->GetString(icnt) );
    NumericVariable *indepVec = NVariableTable::GetTable()->GetVariable(indepName);
    if( !indepVec )
      throw ECommandError("FORM",ExString("independent vector ")+indepName+" does not exist");
    AddToStackLine( p->GetString(icnt) );
    double idmin, idmax;
    indepVec->GetData().GetMinMax( idmin, idmax );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt) && !p->IsNull(icnt)) )
      throw ECommandError("FORM","expecting indep. vector range minimum");
    if( p->IsNull(icnt) )
    {
      AddToStackLine( idmin );
    }
    else
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), "indep. vector range minimum", idmin );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( "FORM", e.what() );
      }
      AddToStackLine( p->GetString(icnt) );
    }
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt) && !p->IsNull(icnt)) )
      throw ECommandError("FORM","expecting indep. vector range maximum");
    if( p->IsNull(icnt) )
    {
      AddToStackLine( idmax );
    }
    else
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), "indep. vector range maximum", idmax );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( "FORM", e.what() );
      }
      AddToStackLine( p->GetString(icnt) );
    }
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNull(icnt)) )
      throw ECommandError("FORM","expecting error vector name or null");
    ExString errorName;
    NumericVariable *errorVec = 0;
    if( p->IsString(icnt) )
    {
      errorName = p->GetString(icnt);
      errorVec = NVariableTable::GetTable()->GetVariable(errorName);
      if( !errorVec )
        throw ECommandError("FORM",ExString("error vector")+errorName+" does not exist");
      AddToStackLine( p->GetString(icnt) );
    }
    else AddToStackLine();
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError("FORM","expecting type of fit");
    ExString typeName( p->GetString(icnt) );
    typeName.ToUpper();
    typeName.RemoveQuotes();
    if( typeName!="LEGENDRE POLYNOMIALS" && typeName!="POLYNOMIAL" && typeName!="GAUSSIAN" &&
        typeName!="LEAST SQUARES LINE" && typeName!="CONSTANT" && typeName!="CUSTOM" )
      throw ECommandError("FORM","invalid type of fit");
    AddToStackLine( p->GetString(icnt) );
    //
    int nParams = 0;
    ++icnt;
    if( typeName == "CUSTOM" )
    {
      if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt)) )
        throw ECommandError("FORM","expecting number of fit parameters");
      double fitp;
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), "number of fit parameters", fitp );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( "FORM", e.what() );
      }
      nParams = static_cast<int>(fitp);
      if( nParams > 8 || nParams < 1 )
        throw ECommandError("FORM","number of fit parameters must be > 0 and <= 8");
      AddToStackLine( p->GetString(icnt) );
      typeName = "Custom";
    }
    else if( typeName == "LEGENDRE POLYNOMIALS" )
    {
      if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt)) )
        throw ECommandError("FORM","expecting degree of polynomial");
      double deg;
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), "degree of polynomial", deg );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( "FORM", e.what() );
      }
      nParams = static_cast<int>(deg)+1;
      if( nParams > 8 || nParams < 1 )
        throw ECommandError("FORM","degree of polynomial must be > 0 and <= 7");
      AddToStackLine( p->GetString(icnt) );
      typeName = "Legendre polynomials";
    }
    else if( typeName == "POLYNOMIAL" )
    {
      if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt)) )
        throw ECommandError("FORM","expecting degree of polynomial");
      double deg;
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), "degree of polynomial", deg );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( "FORM", e.what() );
      }
      nParams = static_cast<int>(deg)+1;
      if( nParams > 8 || nParams < 1 )
        throw ECommandError("FORM","degree of polynomial must be > 0 and <= 7");
      AddToStackLine( p->GetString(icnt) );
      typeName = "Polynomial";
    }
    else if( typeName == "GAUSSIAN" )
    {
      nParams = 3;
      AddToStackLine();
      typeName = "Gaussian";
    }
    else if( typeName == "LEAST SQUARES LINE" )
    {
      nParams = 2;
      AddToStackLine();
      typeName = "Least squares line";
    }
    else if( typeName == "CONSTANT" )
    {
      nParams = 1;
      AddToStackLine();
      typeName = "Constant";
    }
    //
    ++icnt;
    ExString expression;
    if( typeName == "Custom" )
    {
      if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
        throw ECommandError("FORM","expecting fit expression");
      expression = p->GetString(icnt);
      expression.RemoveQuotes();
      AddToStackLine( p->GetString(icnt) );
    }
    else
    {
      AddToStackLine();
    }
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError("FORM","expecting fixed vector");
    std::vector<double> fixed;
    try
    {
      NumericVariable::GetVector( p->GetString(icnt), "fixed vector", fixed );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "FORM", e.what() );
    }
    if( static_cast<int>(fixed.size()) != nParams )
      throw ECommandError("FORM","fixed vector length must equal number of parameters");
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError("FORM","expecting starting values vector");
    std::vector<double> start;
    try
    {
      NumericVariable::GetVector( p->GetString(icnt), "fixed vector", start );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "FORM", e.what() );
    }
    if( static_cast<int>(start.size()) != nParams )
      throw ECommandError("FORM","starting values vector length must equal number of parameters");
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt)) )
      throw ECommandError("FORM","expecting %x legend location");
    double xloc;
    try
    {
      NumericVariable::GetScalar( p->GetString(icnt), "%x legend location", xloc );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FORM", e.what() );
    }
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt)) )
      throw ECommandError("FORM","expecting %y legend location");
    double yloc;
    try
    {
      NumericVariable::GetScalar( p->GetString(icnt), "%y legend location", yloc );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FORM", e.what() );
    }
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()>icnt && !p->IsString(icnt) && !p->IsNull(icnt) )
      throw ECommandError("FORM","expecting fit action");
    ExString action;
    if( p->GetNumberOfTokens()<=icnt || p->IsNull(icnt) )AddToStackLine();
    else
    {
      action = p->GetString(icnt).UpperCase();
      action.RemoveQuotes();
      if( action!="TEST THE FIT" && action!="CLEAR GRAPHICS" &&
          action!="DO THE FIT" && action!="CLOSE" )
        throw ECommandError("FORM","invalid type of action");
      AddToStackLine( p->GetString(icnt) );
    }
    //
    if( FitNoviceForm )
    {
      FitNoviceForm->WindowState = wsNormal;
      FitNoviceForm->Visible = true;
      FitNoviceForm->BringToFront();
    }
    else
    {
      FitNoviceForm = new TFitNoviceForm( MainForm );
      FitNoviceForm->Show();
    }
    FitNoviceForm->Set( dataName, indepName, idmin, idmax, errorName, typeName, nParams,
                        expression, fixed, start, xloc, yloc, action );
  }
  else if( form == "GRAPH" )
  {
    // FORM GRAPH,CLOSE
    // FROM GRAPH,CLEAR
    // FORM GRAPH, depVecName, indepVecName, idmin, idmax, errorVecName, plotsymbol, action
    //   where action can be one of: "CLOSE", "CLEAR", "DRAW"
    //
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError("FORM","expecting dependent vector name or CLOSE or CLEAR");
    ExString depName( p->GetString(icnt) );
    depName.ToUpper();
    if( depName == "CLOSE" )
    {
      if( GraphNoviceForm )
      {
        GraphNoviceForm->WindowState = wsNormal;
        GraphNoviceForm->Visible = true;
        GraphNoviceForm->BringToFront();
        GraphNoviceForm->CloseClick( static_cast<TObject*>(0) );
      }
      AddToStackLine( "CLOSE" );
      return;
    }
    else if( depName == "CLEAR" )
    {
      if( GraphNoviceForm )
      {
        GraphNoviceForm->WindowState = wsNormal;
        GraphNoviceForm->Visible = true;
        GraphNoviceForm->BringToFront();
      }
      else
      {
        GraphNoviceForm = new TGraphNoviceForm( MainForm );
        GraphNoviceForm->Show();
      }
      GraphNoviceForm->ClearClick( static_cast<TObject*>(0) );
      AddToStackLine( "CLEAR" );
      return;
    }
    NumericVariable *depVec = NVariableTable::GetTable()->GetVariable(depName);
    if( !depVec )
      throw ECommandError("FORM",ExString("dependent vector ")+depName+" does not exist");
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    double idmin, idmax;
    ExString indepName;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNull(icnt)) )
      throw ECommandError("FORM","expecting independent vector name");
    if( p->IsString(icnt) )
    {
      indepName = p->GetString(icnt);
      NumericVariable *indepVec = NVariableTable::GetTable()->GetVariable(indepName);
      if( !indepVec )
        throw ECommandError("FORM",ExString("independent vector ")+indepName+" does not exist");
      AddToStackLine( p->GetString(icnt) );
      indepVec->GetData().GetMinMax( idmin, idmax );
    }
    else AddToStackLine();
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt) && !p->IsNull(icnt)) )
      throw ECommandError("FORM","expecting indep. vector range minimum");
    if( p->IsNull(icnt) )
    {
      if( indepName.empty() )AddToStackLine();
      else                   AddToStackLine( idmin );
    }
    else
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), "indep. vector range minimum", idmin );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( "FORM", e.what() );
      }
      AddToStackLine( p->GetString(icnt) );
    }
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt) && !p->IsNull(icnt)) )
      throw ECommandError("FORM","expecting indep. vector range maximum");
    if( p->IsNull(icnt) )
    {
      if( indepName.empty() )AddToStackLine();
      else                   AddToStackLine( idmax );
    }
    else
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(icnt), "indep. vector range maximum", idmax );
      }
      catch ( EVariableError &e )
      {
        throw ECommandError( "FORM", e.what() );
      }
      AddToStackLine( p->GetString(icnt) );
    }
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNull(icnt)) )
      throw ECommandError("FORM","expecting error vector name or null");
    ExString errorName;
    if( p->IsString(icnt) )
    {
      errorName = p->GetString(icnt);
      NumericVariable *errorVec = NVariableTable::GetTable()->GetVariable(errorName);
      if( !errorVec )
        throw ECommandError("FORM",ExString("error vector")+errorName+" does not exist");
      AddToStackLine( p->GetString(icnt) );
    }
    else AddToStackLine();
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNumeric(icnt)) )
      throw ECommandError("FORM","expecting plotsymbol code");
    int ps;
    try
    {
      double tmp;
      NumericVariable::GetScalar( p->GetString(icnt), "plotsymbol code", tmp );
      ps = static_cast<int>(tmp);
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "FORM", e.what() );
    }
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()>icnt && !p->IsString(icnt) && !p->IsNull(icnt) )
      throw ECommandError("FORM","expecting graph action");
    ExString action;
    if( p->GetNumberOfTokens()<=icnt || p->IsNull(icnt) )AddToStackLine();
    else
    {
      action = p->GetString(icnt).UpperCase();
      action.RemoveQuotes();
      if( action!="DRAW" && action!="CLEAR" && action!="CLOSE" )
        throw ECommandError("FORM","invalid type of action");
      AddToStackLine( p->GetString(icnt) );
    }
    if( GraphNoviceForm )
    {
      GraphNoviceForm->WindowState = wsNormal;
      GraphNoviceForm->Visible = true;
      GraphNoviceForm->BringToFront();
    }
    else
    {
      GraphNoviceForm = new TGraphNoviceForm( MainForm );
      GraphNoviceForm->Show();
    }
    GraphNoviceForm->Set( depName, indepName, idmin, idmax, errorName, ps, action );
  }
  else if( form == "EXECUTE" )
  {
    // FORM EXECUTE,CLOSE
    // FROM EXECUTE,'filename','parameters',action
    //   where action can be one of: "CLOSE", "APPLY"
    //
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError("FORM","expecting filename or CLOSE");
    ExString fileName( p->GetString(icnt) );
    fileName.RemoveQuotes();
    if( fileName.UpperCase() == "CLOSE" )
    {
      AddToStackLine( "CLOSE" );
      if( ExecForm )
      {
        ExecForm->WindowState = wsNormal;
        ExecForm->Visible = true;
        ExecForm->BringToFront();
        ExecForm->CloseClick( static_cast<TObject*>(0) );
      }
      return;
    }
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    ExString parameters;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsString(icnt) && !p->IsNull(icnt)) )
      throw ECommandError("FORM","expecting parameters");
    if( p->IsString(icnt) )
    {
      parameters = p->GetString(icnt);
      AddToStackLine( p->GetString(icnt) );
    }
    else AddToStackLine();
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError("FORM","expecting execute action");
    ExString action;
    action = p->GetString(icnt).UpperCase();
    action.RemoveQuotes();
    if( action!="APPLY" && action!="CLOSE" )throw ECommandError("FORM","invalid type of action");
    AddToStackLine( p->GetString(icnt) );
    if( ExecForm )
    {
      ExecForm->WindowState = wsNormal;
      ExecForm->Visible = true;
      ExecForm->BringToFront();
    }
    else
    {
      ExecForm = new TExecForm( MainForm );
      ExecForm->Show();
    }
    ExecForm->Set( fileName, parameters, action );
  }
  else if( form == "TEXT" )
  {
    // FORM TEXT,CLOSE
    // FORM TEXT,ERASE
    // FROM TEXT,'text',psc,xloc,yloc,alignment,action
    //   where action can be one of: "CLOSE", "ERASE", "DRAW"
    //
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError("FORM","expecting 'text string' or CLOSE or ERASE");
    ExString text( p->GetString(icnt) );
    text.RemoveQuotes();
    if( text.UpperCase() == "CLOSE" )
    {
      AddToStackLine( "CLOSE" );
      if( TextCommandForm )
      {
        TextCommandForm->WindowState = wsNormal;
        TextCommandForm->Visible = true;
        TextCommandForm->BringToFront();
        TextCommandForm->CloseButtonClick(  static_cast<TObject*>(0) );
      }
      return;
    }
    else if( text.UpperCase() == "ERASE" )
    {
      AddToStackLine( "ERASE" );
      if( TextCommandForm )
      {
        TextCommandForm->WindowState = wsNormal;
        TextCommandForm->Visible = true;
        TextCommandForm->BringToFront();
      }
      else
      {
        TextCommandForm = new TTextCommandForm( MainForm );
        TextCommandForm->Show();
      }
      TextCommandForm->EraseButtonClick(  static_cast<TObject*>(0) );
      return;
    }
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
      throw ECommandError("FORM","expecting interactive flag");
    int psc = static_cast<int>( p->GetNumeric(icnt) );
    AddToStackLine( psc );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsNumeric(icnt) && !p->IsNull(icnt)) )
      throw ECommandError("FORM","expecting x location");
    double xloc = p->IsNumeric(icnt) ? p->GetNumeric(icnt) : 50.0;
    AddToStackLine( xloc );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || (!p->IsNumeric(icnt) && !p->IsNull(icnt)) )
      throw ECommandError("FORM","expecting y location");
    double yloc = p->IsNumeric(icnt) ? p->GetNumeric(icnt) : 50.0;
    AddToStackLine( yloc );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
      throw ECommandError("FORM","expecting alignment code");
    int alignment = static_cast<int>( p->GetNumeric(icnt) );
    AddToStackLine( alignment );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError("FORM","expecting text action");
    ExString action;
    action = p->GetString(icnt).UpperCase();
    action.RemoveQuotes();
    if( action!="CLOSE" && action!="ERASE" && action!="DRAW" )
      throw ECommandError("FORM","invalid type of action");
    AddToStackLine( p->GetString(icnt) );
    if( TextCommandForm )
    {
      TextCommandForm->WindowState = wsNormal;
      TextCommandForm->Visible = true;
      TextCommandForm->BringToFront();
    }
    else
    {
      TextCommandForm = new TTextCommandForm( MainForm );
      TextCommandForm->Show();
    }
    TextCommandForm->Set( text, psc, xloc, yloc, alignment, action );
  }
  else if( form == "GENERATE" )
  {
    // FORM GENERATE,CLOSE
    // FORM GENERATE,DEFAULTS
    // FROM EXECUTE,'vectorname',choice,min,inc,max,number,action
    //   where action can be one of: "CLOSE", "DEFAULTS", or "APPLY"
    //
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError("FORM","expecting vector name or CLOSE or DEFAULTS");
    ExString name( p->GetString(icnt) );
    if( name.UpperCase() == "CLOSE" )
    {
      AddToStackLine( "CLOSE" );
      if( GenerateCmndForm )
      {
        GenerateCmndForm->WindowState = wsNormal;
        GenerateCmndForm->Visible = true;
        GenerateCmndForm->BringToFront();
        GenerateCmndForm->CloseClick( static_cast<TObject*>(0) );
      }
      return;
    }
    else if( name.UpperCase() == "DEFAULTS" )
    {
      if( GenerateCmndForm )
      {
        GenerateCmndForm->WindowState = wsNormal;
        GenerateCmndForm->Visible = true;
        GenerateCmndForm->BringToFront();
      }
      else
      {
        GenerateCmndForm = new TGenerateCmndForm( MainForm );
        GenerateCmndForm->Show();
      }
      GenerateCmndForm->DefaultsClick( static_cast<TObject*>(0) );
      AddToStackLine( "DEFAULTS" );
      return;
    }
    AddToStackLine( p->GetString(icnt) );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
      throw ECommandError("FORM","expecting choice value");
    int choice = p->GetNumeric(icnt);
    AddToStackLine( choice );
    //
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
      throw ECommandError("FORM","expecting min value");
    double min = p->GetNumeric(icnt);
    AddToStackLine( min );
    //
    double inc = 0.0;
    double max = 0.0;
    int number = 0;
    switch ( choice )
    {
      case 0:  // min,,max,number  (random values)
      case 3:  // min,,max,number
      {
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || (!p->IsNumeric(icnt) && !p->IsNull(icnt)) )
          throw ECommandError("FORM","expecting increment value");
        if( p->IsNumeric(icnt) )inc = p->GetNumeric(icnt);
        AddToStackLine( inc );
        //
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
          throw ECommandError("FORM","expecting max value");
        max = p->GetNumeric(icnt);
        AddToStackLine( max );
        //
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
          throw ECommandError("FORM","expecting number value");
        number = p->GetNumeric(icnt);
        AddToStackLine( number );
        //
        break;
      }
      case 1: // min,inc,max,
      {
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
          throw ECommandError("FORM","expecting increment value");
        inc = p->GetNumeric(icnt);
        AddToStackLine( inc );
        //
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
          throw ECommandError("FORM","expecting max value");
        max = p->GetNumeric(icnt);
        AddToStackLine( max );
        //
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || (!p->IsNumeric(icnt) && !p->IsNull(icnt)) )
          throw ECommandError("FORM","expecting number value");
        if( p->IsNumeric(icnt) )number = p->GetNumeric(icnt);
        AddToStackLine( number );
        //
        break;
      }
      case 2: // min,inc,,number
      {
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
          throw ECommandError("FORM","expecting increment value");
        inc = p->GetNumeric(icnt);
        AddToStackLine( inc );
        //
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || (!p->IsNumeric(icnt) && !p->IsNull(icnt)) )
          throw ECommandError("FORM","expecting max value");
        if( p->IsNumeric(icnt) )max = p->GetNumeric(icnt);
        AddToStackLine( max );
        //
        ++icnt;
        if( p->GetNumberOfTokens()<=icnt || !p->IsNumeric(icnt) )
          throw ECommandError("FORM","expecting number value");
        number = p->GetNumeric(icnt);
        AddToStackLine( number );
        //
        break;
      }
      default:
        throw ECommandError("FORM","invalid choice, expecting 0, 1, 2, or 3");
    }
    ++icnt;
    if( p->GetNumberOfTokens()<=icnt || !p->IsString(icnt) )
      throw ECommandError("FORM","expecting generate action");
    ExString action;
    action = p->GetString(icnt).UpperCase();
    action.RemoveQuotes();
    if( action!="APPLY" && action!="CLOSE" && action!="DEFAULTS" )
      throw ECommandError("FORM","invalid type of action");
    AddToStackLine( p->GetString(icnt) );
    if( GenerateCmndForm )
    {
      GenerateCmndForm->WindowState = wsNormal;
      GenerateCmndForm->Visible = true;
      GenerateCmndForm->BringToFront();
    }
    else
    {
      GenerateCmndForm = new TGenerateCmndForm( MainForm );
      GenerateCmndForm->Show();
    }
    GenerateCmndForm->Set( name, choice, min, inc, max, number, action );
  }
  else throw ECommandError("FORM","unknown FORM type");
}

// end of file
