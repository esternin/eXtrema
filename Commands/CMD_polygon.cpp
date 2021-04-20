/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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

#include "CMD_polygon.h"
#include "ExGlobals.h"
#include "ExString.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "UsefulFunctions.h"
#include "NumericVariable.h"

CMD_polygon CMD_polygon::cmd_polygon_;

CMD_polygon::CMD_polygon() : Command( "POLYGON" )
{
  AddQualifier( "INSIDE", true );
  AddQualifier( "OUTSIDE", false );
}

void CMD_polygon::Execute( ParseLine const *p )
{
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError const &e)
  {
    throw;
  }
  if( qualifiers["OUTSIDE"] )qualifiers["INSIDE"] = false;
  int counter = 1;
  std::vector<double> xP, yP;
  if( p->GetNumberOfTokens() < counter+1 )
    throw ECommandError( "POLYGON", "expecting polygon x-vector" );
  try
  {
    NumericVariable::GetVector( p->GetString(counter), "polygon x-vector", xP );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( "POLYGON", e.what() );
  }
  AddToStackLine( p->GetString(counter++) );
  //
  if( p->GetNumberOfTokens() < counter+1 )
    throw ECommandError( "POLYGON", "expecting polygon y-vector" );
  try
  {
    NumericVariable::GetVector( p->GetString(counter), "polygon y-vector", yP );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( "POLYGON", e.what() );
  }
  AddToStackLine( p->GetString(counter++) );
  //
  if( xP.size() != yP.size() )
    throw ECommandError( "POLYGON", "polygon vector lengths are not equal" );
  int numP = static_cast<int>(xP.size());
  if( numP <= 1 )throw ECommandError( "POLYGON", "polygon vector lengths are <= 1" );
  //
  std::vector<double> x, y;
  if( p->GetNumberOfTokens() < counter+1 )
    throw ECommandError( "POLYGON", "expecting data x-vector" );
  try
  {
    NumericVariable::GetVector( p->GetString(counter), "data x-vector", x );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( "POLYGON", e.what() );
  }
  AddToStackLine( p->GetString(counter++) );
  //
  if( p->GetNumberOfTokens() < counter+1 )
    throw ECommandError( "POLYGON", "expecting data y-vector" );
  try
  {
    NumericVariable::GetVector( p->GetString(counter), "data y-vector", y );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( "POLYGON", e.what() );
  }
  AddToStackLine( p->GetString(counter++) );
  
  if( x.size() != y.size() )
    throw ECommandError( "POLYGON", "data vector lengths are not equal" );
  int num = static_cast<int>(x.size());
  //
  if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
    throw ECommandError( "POLYGON", "expecting output index vector name" );
  ExString indexName( p->GetString(counter++) );
  AddToStackLine( indexName );
  //
  std::vector<double> idx(num);
  if( qualifiers["INSIDE"] )
  {
    for( int i=0; i<num; ++i )
    {
      idx[i] = UsefulFunctions::InsidePolygon(x[i],y[i],xP,yP) ? 1.0 : 0.0;
    }
  }
  else
  {
    for( int i=0; i<num; ++i )
    {
      idx[i] = UsefulFunctions::InsidePolygon(x[i],y[i],xP,yP) ? 0.0 : 1.0;
    }
  }
  try
  {
    NumericVariable::PutVariable( indexName, idx, 0, p->GetInputLine() );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( "POLYGON", e.what() );
  }
}

// end of file
