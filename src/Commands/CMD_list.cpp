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
#include <cmath>

#include "wx/wx.h"

#include "CMD_list.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "EVariableError.h"
#include "TextVariable.h"
#include "NumericVariable.h"

CMD_list *CMD_list::cmd_list_ = 0;

CMD_list::CMD_list() : Command( wxT("LIST") )
{
  AddQualifier( wxT("MATRIX"), false );
  AddQualifier( wxT("FORMAT"), false );
  AddQualifier( wxT("COUNTER"), false );
}

void CMD_list::Execute( ParseLine const *p )
{
  //
  // LIST x1 { x2 ... x5 } { n1 { n2 { n3 }}}
  // LIST\COUNTER x1 { x2 ... x5 } { n1 { n2 { n3 }}}
  // LIST\MATRIX m
  // LIST\MATRIX\FORMAT m `format'
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
  wxString command( Name()+wxT(": ") );
  int nTokens = p->GetNumberOfTokens();
  if( qualifiers[wxT("MATRIX")] )
  {
    if( nTokens < 2 || !p->IsString(1) )
      throw ECommandError( command+wxT("expecting matrix to list") );
    std::vector<double> m;
    std::size_t nRow, nCol;
    try
    {
      NumericVariable::GetMatrix( p->GetString(1), wxT("matrix to list"), m, nRow, nCol );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(1) );
    if( qualifiers[wxT("FORMAT")] )
    {
      if( nTokens < 3 || !p->IsString(1) )
        throw ECommandError( command+wxT("expecting format") );
      //
      // no need for try-catch since GetVariable will never throw
      // with second parameter true
      //
      wxString format;
      TextVariable::GetVariable( p->GetString(2), true, format );
      for( std::size_t i=0; i<nRow; ++i )
      {
        wxString line;
        for( std::size_t j=0; j<nCol; ++j )
        {
          wxChar c[50];
          ::wxSnprintf( c, 50, format.c_str(), m[i+j*nRow] );
          line += c;
        }
        ExGlobals::WriteOutput( line );
      }
    }
    else
    {
      for( std::size_t i=0; i<nRow; ++i )
      {
        wxString line;
        for( std::size_t j=0; j<nCol; ++j )
        {
          wxChar c[50];
          ::wxSnprintf( c, 50, wxT("%12g"), m[i+j*nRow] );
          line += c;
        }
        ExGlobals::WriteOutput( line );
      }
    }
  }
  else // list vectors
  {
    std::size_t nVec = 0;
    int counter = 1;
    wxString name[5];
    std::vector<double> x[5];
    while( counter < std::min(6,nTokens) )
    {
      if( p->IsString(counter) )
      {
        int nDim;
        double value;
        int dimSizes[3];
        name[nVec] = p->GetString(counter);
        try
        {
          NumericVariable::GetVariable( name[nVec], nDim, value, x[nVec], dimSizes );
        }
        catch( EVariableError &e )
        {
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
        AddToStackLine( name[nVec] );
        if( nDim == 1 ) // encountered a vector
        {
          ++nVec;
        }
        else if( nDim == 2 )
        {
          wxString s( wxT("LIST: ") );
          s += name[nVec] + wxT(" is a matrix");
          ExGlobals::WarningMessage( s );
        }
        else if( nDim == 3 )
        {
          wxString s( wxT("LIST: ") );
          s += name[nVec] + wxT(" is a tensor");
          ExGlobals::WarningMessage( s );
        }
        else if( nDim == 0 ) //  encountered a scalar
        {
          break;
        }
      }
      else if( p->IsNumeric(counter) ) // encountered a number
      {
        break;
      }
      ++counter;
    }
    if( nVec == 0 )throw ECommandError( command+wxT("no vectors were entered") );
    int nMax = x[0].size();
    for( std::size_t i=1; i<nVec; ++i )
    {
      if( nMax < static_cast<int>(x[i].size()) )nMax = x[i].size();
    }
    int first = 1;
    int last = nMax;
    int inc = 1;
    if( p->IsNumeric(counter) )
    {
      first = static_cast<int>( p->GetNumeric(counter) );
      AddToStackLine( p->GetString(counter) );
    }
    else if( p->IsString(counter) )
    {
      double value;
      try
      {
        NumericVariable::GetScalar( p->GetString(counter), wxT("first index"), value );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      first = static_cast<int>( value );
    }
    if( p->IsNumeric(counter+1) )
    {
      last = static_cast<int>( p->GetNumeric(counter+1) );
      AddToStackLine( p->GetString(counter+1) );
    }
    else if( p->IsString(counter+1) )
    {
      double value;
      try
      {
        NumericVariable::GetScalar( p->GetString(counter+1), wxT("final index"), value );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      last = static_cast<int>( value );
    }
    if( p->IsNumeric(counter+2) )
    {
      inc = static_cast<int>( p->GetNumeric(counter+2) );
      AddToStackLine( p->GetString(counter+2) );
    }
    else if( p->IsString(counter+2) )
    {
      double value;
      try
      {
        NumericVariable::GetScalar( p->GetString(counter+2), wxT("increment"), value );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      inc = static_cast<int>( value );
    }
    first = std::min( nMax, std::max(1,first) );
    last = std::min( nMax, std::max(1,last) );
    if( inc == 0 )throw ECommandError( command+wxT("zero increment not allowed") );
    wxString s;
    int nDigits = 0;
    if( qualifiers[wxT("COUNTER")] )
    {
      nDigits = static_cast<int>( log10(static_cast<double>(last)) ) + 1;
      nDigits == 1 ? s=wxT('#') : s=wxString(nDigits-1,wxT(' '))+wxT('#');
    }
    wxChar c[20];
    ::wxSnprintf( c, 20, wxT(" %15s"), name[0].substr(0,15).c_str() );
    s += c;
    //
    for( std::size_t i=1; i<nVec; ++i )
    {
      ::wxSnprintf( c, 20, wxT(" %15s"), name[i].substr(0,15).c_str() );
      s += c;
    }
    ExGlobals::WriteOutput( s );
    wxString dashes( s.size(), '-' );
    ExGlobals::WriteOutput( dashes );
    int cntr = 1;
    for( int i=first; i<=last; i+=inc )
    {
      wxString s;
      if( qualifiers[wxT("COUNTER")] )
      {
        ::wxSnprintf( c, 20, wxT("%*d"), nDigits, cntr );
        s = c;
      }
      for( std::size_t j=0; j<nVec; ++j )
      {
        if( i <= static_cast<int>(x[j].size()) )
        {
          ::wxSnprintf( c, 20, wxT(" %15g"), x[j][i-1] );
          s += c;
        }
        else
        {
          s += wxT("                ");
        }
      }
      ExGlobals::WriteOutput( s );
      ++cntr;
    }
  }
}

// end of file
