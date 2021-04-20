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
#include "EExpressionError.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "NVariableTable.h"
#include "TVariableTable.h"

#include "CMD_matrix.h"

CMD_matrix CMD_matrix::cmd_matrix_;

void CMD_matrix::Execute( ParseLine const *p )
{
  SetUp( p );
  std::vector<ExString> names;
  for( int counter=1; counter<p->GetNumberOfTokens()-2; ++counter )
  {
    if( !p->IsString(counter) )throw ECommandError( "MATRIX", "expecting matrix name(s)" );
    ExString name( p->GetString(counter) );
    AddToStackLine( name );
    names.push_back( name );
  }
  if( names.empty() )throw ECommandError( "MATRIX", "expecting some matrix names" );
  int counter = p->GetNumberOfTokens()-2;
  double v;
  try
  {
    NumericVariable::GetScalar( p->GetString(counter), "number of rows", v );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "MATRIX", e.what() );
  }
  if( static_cast<int>(v+0.5) < 0 )throw ECommandError( "MATRIX", "number of rows < 0" );
  else if( static_cast<int>(v+0.5) == 0 )throw ECommandError( "MATRIX", "number of rows = 0" );
  std::size_t nRows = static_cast<std::size_t>(v+0.5);
  AddToStackLine( p->GetString(counter++) );
  //
  try
  {
    NumericVariable::GetScalar( p->GetString(counter), "number of columns", v );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "MATRIX", e.what() );
  }
  if( static_cast<int>(v+0.5) < 0 )throw ECommandError( "MATRIX", "number of columns < 0" );
  else if( static_cast<int>(v+0.5) == 0 )throw ECommandError( "MATRIX", "number of columns = 0" );
  std::size_t nCols = static_cast<std::size_t>(v+0.5);
  AddToStackLine( p->GetString(counter) );
  //
  std::size_t numNames = names.size();
  for( std::size_t j=0; j<numNames; ++j )
  {
    std::vector<double> x( nRows*nCols, 0.0 );
    NumericVariable *nv = NVariableTable::GetTable()->GetVariable(names[j]);
    if( nv )
    {
      NumericData nd( nv->GetData() );
      if( nd.GetNumberOfDimensions() == 2 )
      {
        std::vector<double> y( nd.GetData() );
        std::size_t nr = nd.GetDimMag(0);
        std::size_t nc = nd.GetDimMag(1);
        std::size_t kc = 0;
        while ( kc < std::min(nCols,nc) )
        {
          std::size_t kr = 0;
          while ( kr < std::min(nRows,nr) )
          {
            x[kr+kc*nRows] = y[kr+kc*nr];
            ++kr;
          }
          ++kc;
        }
      }
      else
      {
        NVariableTable::GetTable()->RemoveEntry( nv );
        nv = 0;
      }
    }
    else
    {
      TextVariable *tv = TVariableTable::GetTable()->GetVariable(names[j]);
      if( tv )TVariableTable::GetTable()->RemoveEntry( tv );
      tv = 0;
    }
    NumericVariable::PutVariable( names[j], x, nRows, nCols, p->GetInputLine() );
  }
}
 
// end of file
