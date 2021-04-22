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
#include "CMD_polygon.h"
#include "ExGlobals.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "UsefulFunctions.h"
#include "NumericVariable.h"

CMD_polygon *CMD_polygon::cmd_polygon_ = 0;

CMD_polygon::CMD_polygon() : Command( wxT("POLYGON") )
{
  AddQualifier( wxT("INSIDE"), true );
  AddQualifier( wxT("OUTSIDE"), false );
}

void CMD_polygon::Execute( ParseLine const *p )
{
  QualifierMap qualifiers;
  SetUp( p, qualifiers );
  wxString command( Name()+wxT(": ") );
  if( qualifiers[wxT("OUTSIDE")] )qualifiers[wxT("INSIDE")] = false;
  int counter = 1;
  std::vector<double> xP, yP;
  if( p->GetNumberOfTokens() < counter+1 )
    throw ECommandError( command+wxT("expecting polygon x-vector") );
  try
  {
    NumericVariable::GetVector( p->GetString(counter), wxT("polygon x-vector"), xP );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( p->GetString(counter++) );
  //
  if( p->GetNumberOfTokens() < counter+1 )
    throw ECommandError( command+wxT("expecting polygon y-vector") );
  try
  {
    NumericVariable::GetVector( p->GetString(counter), wxT("polygon y-vector"), yP );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( p->GetString(counter++) );
  //
  if( xP.size() != yP.size() )
    throw ECommandError( command+wxT("polygon vector lengths are not equal") );
  int numP = static_cast<int>(xP.size());
  if( numP <= 1 )throw ECommandError( command+wxT("polygon vector lengths are <= 1") );
  //
  std::vector<double> x, y;
  if( p->GetNumberOfTokens() < counter+1 )
    throw ECommandError( command+wxT("expecting data x-vector") );
  try
  {
    NumericVariable::GetVector( p->GetString(counter), wxT("data x-vector"), x );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( p->GetString(counter++) );
  //
  if( p->GetNumberOfTokens() < counter+1 )
    throw ECommandError( command+wxT("expecting data y-vector") );
  try
  {
    NumericVariable::GetVector( p->GetString(counter), wxT("data y-vector"), y );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( p->GetString(counter++) );
  
  if( x.size() != y.size() )
    throw ECommandError( command+wxT("data vector lengths are not equal") );
  int num = static_cast<int>(x.size());
  //
  if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
    throw ECommandError( command+wxT("expecting output index vector name") );
  wxString indexName( p->GetString(counter++) );
  AddToStackLine( indexName );
  //
  std::vector<double> idx(num);
  if( qualifiers[wxT("INSIDE")] )
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
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
}

// end of file
