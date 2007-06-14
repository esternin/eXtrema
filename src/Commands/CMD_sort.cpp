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
#include "wx/wx.h"

#include "CMD_sort.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "UsefulFunctions.h"
#include "EVariableError.h"
#include "NumericVariable.h"

CMD_sort *CMD_sort::cmd_sort_ = 0;

CMD_sort::CMD_sort() : Command( wxT("SORT") )
{
  AddQualifier( wxT("UP"), true );
  AddQualifier( wxT("DOWN"), false );
}

void CMD_sort::Execute( ParseLine const *p )
{
  //
  // Sort a vector into ascending or descending order.
  // Also rearrange other vectors along with the sorted one if their
  // names are included in the command parameter list.
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
  qualifiers[wxT("UP")] = !qualifiers[wxT("DOWN")];
  if( p->GetNumberOfTokens() < 2 )throw ECommandError( command+wxT("expecting a vector to sort") );
  std::vector<double> x;
  try
  {
    NumericVariable::GetVector( p->GetString(1), wxT("vector to sort"), x );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  wxString sortName( p->GetString(1) );
  AddToStackLine( sortName );
  std::size_t xSize = x.size();
  //
  int nTokens = p->GetNumberOfTokens();
  std::vector< std::vector<double> > otherVecs;
  std::vector<wxString> names;
  if( nTokens > 2 )otherVecs.resize( nTokens-2 );
  for( int i=2; i<nTokens; ++i )
  {
    if( !p->IsString(i) )
      throw ECommandError( command+wxT("expecting associated vector to re-arrange") );
    try
    {
      NumericVariable::GetVector( p->GetString(i), wxT("vector to re-arrange"), otherVecs[i-2] );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(i) );
    names.push_back( p->GetString(i) );
  }
  std::vector<double> dKey(xSize);
  for( std::size_t i=0; i<xSize; ++i )dKey[i] = static_cast<double>(i);
  UsefulFunctions::QuickSort( x, dKey, qualifiers[wxT("UP")] );
  std::vector<int> key(xSize);
  for( std::size_t i=0; i<xSize; ++i )key[i] = static_cast<int>(dKey[i]);
  //
  int order = qualifiers[wxT("UP")] ? 1 : 2;
  NumericVariable::PutVariable( sortName, x, order, p->GetInputLine() );
  //
  if( !names.empty() )
  {
    std::size_t nOthers = names.size();
    //
    // Create a temporary vector.
    // Sort the optional arrays using the KEYS vector which contains presorted indices.
    //
    std::vector<double> temp(xSize);
    for( std::size_t j=0; j<nOthers; ++j )
    {
      for( std::size_t i=0; i<xSize; ++i )temp[i] = otherVecs[j][key[i]];
      try
      {
        NumericVariable::PutVariable( names[j], temp, 0, p->GetInputLine() );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
    }
  }
}

// end of file
