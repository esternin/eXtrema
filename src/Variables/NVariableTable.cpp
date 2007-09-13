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
#include "NVariableTable.h"
#include "EVariableError.h"
#include "NumericVariable.h"

NVariableTable *NVariableTable::variableTable_ = 0;

void NVariableTable::ClearTable()
{
  NVariableMap::iterator end = vMap_.end();
  for( NVariableMap::iterator i=vMap_.begin(); i!=end; ++i )
  {
    delete (*i).second;
    (*i).second = 0;
  }
  NVariableMap().swap( vMap_ );
  //vMap_.erase( vMap_.begin(), vMap_.end() );
}

void NVariableTable::AddEntry( NumericVariable *v )
{
  if( !v )throw EVariableError(wxT("attempt to add a null numeric variable"));
  wxString name( v->GetName() );
  if( NVariableTable::Contains( name ) )
   throw EVariableError( wxString(wxT("The variable "))+name+wxString(wxT(" is already in the table")) );
  vMap_.insert( entry_type(name,v) );
}

void NVariableTable::RemoveEntry( NumericVariable *v )
{
  if( !v )return;
  wxString name = v->GetName();
  delete v;
  vMap_.erase( name );
}

bool NVariableTable::RemoveVariables( std::size_t d )
{
  NVariableMap::const_iterator end = vMap_.end();
  for( NVariableMap::const_iterator i=vMap_.begin(); i!=end; ++i )
  {
    if( static_cast<std::size_t>((*i).second->GetData().GetNumberOfDimensions()) == d )
    {
      RemoveEntry( (*i).second );
      return true;
    }
  }
  return false;
}

bool NVariableTable::Contains( wxString const &name ) const
{ return ( vMap_.find(name.Upper())!=vMap_.end() ); }

NumericVariable *NVariableTable::GetVariable( wxString const &name ) const
{
  NVariableMap::const_iterator i = vMap_.find( name.Upper() );
  if( i != vMap_.end() )return (*i).second;
  return 0;
}

void NVariableTable::RemoveScalars()
{ if( RemoveVariables(0) )RemoveScalars(); }

void NVariableTable::RemoveVectors()
{ if( RemoveVariables(1) )RemoveVectors(); }

void NVariableTable::RemoveMatrices()
{ if( RemoveVariables(2) )RemoveMatrices(); }

void NVariableTable::RemoveTensors()
{ if( RemoveVariables(3) )RemoveTensors(); }

void NVariableTable::RemoveEntry( wxString const &name )
{ RemoveEntry( GetVariable(name) ); }

std::ostream &operator<<( std::ostream &out, NVariableTable const &table )
{
  out << "<numericvariables size=\"" << table.Entries() << "\">\n";
  NVariableTable::NVariableMap::const_iterator end = table.vMap_.end();
  for( NVariableTable::NVariableMap::const_iterator i=table.vMap_.begin(); i!=end; ++i )
    out << *((*i).second);
  return out << "</numericvariables>\n";
}

// end of file
