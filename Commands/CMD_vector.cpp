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

//#include <memory>

#include "ExGlobals.h"
#include "ExString.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "EExpressionError.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "NVariableTable.h"
#include "TVariableTable.h"

#include "CMD_vector.h"

CMD_vector CMD_vector::cmd_vector_;

void CMD_vector::Execute( ParseLine const *p )
{
  SetUp( p );
  std::vector<ExString> names;
  for( int counter=1; counter<p->GetNumberOfTokens()-1; ++counter )
  {
    if( !p->IsString(counter) )throw ECommandError( "VECTOR", "expecting variable name(s)" );
    AddToStackLine( p->GetString(counter) );
    names.push_back( p->GetString(counter) );
  }
  if( names.empty() )throw ECommandError( "VECTOR", "expecting some vector names" );
  int counter = p->GetNumberOfTokens()-1;
  double v;
  try
  {
    NumericVariable::GetScalar( p->GetString(counter), "vector length", v );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "VECTOR", e.what() );
  }
  if( static_cast<int>(v+0.5) < 0 )throw ECommandError( "VECTOR", "vector length < 0" );
  else if( static_cast<int>(v+0.5) == 0 )throw ECommandError( "VECTOR", "vector length = 0" );
  std::size_t n = static_cast<std::size_t>(v+0.5);
  AddToStackLine( p->GetString(counter) );
  //
  std::size_t end = names.size();
  for( std::size_t j=0; j<end; ++j )
  {
    std::vector<double> x( n, 0.0 );
    NumericVariable *nv = NVariableTable::GetTable()->GetVariable( names[j] );
    if( nv )
    {
      if( nv->GetData().GetNumberOfDimensions() == 1 ) // it is a vector
      {
        std::vector<double> y( nv->GetData().GetData() );
        if( y.size() < n )
        {
          x.assign( y.begin(), y.end() );
          x.resize( n, 0.0 );
        }
        else if( y.size() == n )
        {
          x.assign( y.begin(), y.end() );
        }
        else
        {
          x.assign( y.begin(), y.begin()+n );
        }
      }
      else
      {
        NVariableTable::GetTable()->RemoveEntry( nv );
        nv = 0;
      }
    }
    else
      TVariableTable::GetTable()->RemoveEntry( names[j] );
    NumericVariable::PutVariable( names[j], x, 0, p->GetInputLine() );
  }
}

// end of file
