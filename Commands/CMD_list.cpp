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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ExGlobals.h"
#include "ExString.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "TextVariable.h"
#include "NumericVariable.h"

#include "CMD_list.h"

CMD_list CMD_list::cmd_list_;

CMD_list::CMD_list() : Command( "LIST" )
{
  AddQualifier( "MATRIX", false );
  AddQualifier( "FORMAT", false );
  AddQualifier( "COUNTER", false );
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
  int nTokens = p->GetNumberOfTokens();
  if( qualifiers["MATRIX"] )
  {
    if( nTokens < 2 || !p->IsString(1) )
      throw ECommandError( "LIST", "expecting matrix to list" );
    std::vector<double> m;
    std::size_t nRow, nCol;
    try
    {
      NumericVariable::GetMatrix( p->GetString(1), "matrix to list", m, nRow, nCol );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "LIST\\MATRIX", e.what() );
    }
    AddToStackLine( p->GetString(1) );
    if( qualifiers["FORMAT"] )
    {
      if( nTokens < 3 || !p->IsString(1) )throw ECommandError( "LIST", "expecting format" );
      //
      // no need for try-catch since GetVariable will never throw
      // with second parameter true
      //
      ExString format;
      TextVariable::GetVariable( p->GetString(2), true, format );
      for( std::size_t i=0; i<nRow; ++i )
      {
        ExString line;
        for( std::size_t j=0; j<nCol; ++j )
        {
          char c[50];
          sprintf( c, format.c_str(), m[i+j*nRow] );
          line += c;
        }
        ExGlobals::WriteOutput( line );
      }
    }
    else
    {
      for( std::size_t i=0; i<nRow; ++i )
      {
        ExString line;
        for( std::size_t j=0; j<nCol; ++j )
        {
          char c[50];
          sprintf( c, "%12g", m[i+j*nRow] );
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
    ExString name[5];
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
          throw ECommandError( "LIST", e.what() );
        }
        AddToStackLine( name[nVec] );
        if( nDim == 1 ) // encountered a vector
        {
          ++nVec;
        }
        else if( nDim == 2 )
        {
          ExString s( "LIST: " );
          s += name[nVec] + " is a matrix";
          ExGlobals::WarningMessage( s );
        }
        else if( nDim == 3 )
        {
          ExString s( "LIST: " );
          s += name[nVec] + " is a tensor";
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
    if( nVec == 0 )throw ECommandError( "LIST", "no vectors were entered" );
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
        NumericVariable::GetScalar( p->GetString(counter), "first index", value );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "LIST", e.what() );
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
        NumericVariable::GetScalar( p->GetString(counter+1), "final index", value );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "LIST", e.what() );
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
        NumericVariable::GetScalar( p->GetString(counter+2), "increment", value );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "LIST", e.what() );
      }
      inc = static_cast<int>( value );
    }
    first = std::min( nMax, std::max(1,first) );
    last = std::min( nMax, std::max(1,last) );
    if( inc == 0 )throw ECommandError( "LIST", "zero increment not allowed" );
    ExString s;
    int nDigits = 0;
    if( qualifiers["COUNTER"] )
    {
      nDigits = static_cast<int>( log10(static_cast<double>(last)) ) + 1;
      nDigits == 1 ? s=ExString("#") : s=ExString(nDigits-1,' ') + ExString("#");
    }
    char c[20];
    sprintf( c, " %15s", name[0].substr(0,15).c_str() );
    s += ExString( c );
    //
    for( std::size_t i=1; i<nVec; ++i )
    {
      sprintf( c, " %15s", name[i].substr(0,15).c_str() );
      s += ExString( c );
    }
    ExGlobals::WriteOutput( s );
    ExString dashes( s.size(), '-' );
    ExGlobals::WriteOutput( dashes );
    int cntr = 1;
    for( int i=first; i<=last; i+=inc )
    {
      ExString s;
      if( qualifiers["COUNTER"] )
      {
        sprintf( c, "%*d", nDigits, cntr );
        s = ExString( c );
      }
      for( std::size_t j=0; j<nVec; ++j )
      {
        if( i <= static_cast<int>(x[j].size()) )
        {
          sprintf( c, " %15g", x[j][i-1] );
          s += c;
        }
        else
        {
          s += ExString("                ");
        }
      }
      ExGlobals::WriteOutput( s );
      ++cntr;
    }
  }
}

// end of file
