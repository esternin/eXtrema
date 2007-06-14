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
#include "TVariableTable.h"
#include "TextVariable.h"
#include "EVariableError.h"

TVariableTable *TVariableTable::tVariableTable_ = 0;

bool TVariableTable::Contains( wxString const &name ) const
{ return ( vMap_.find(name.Upper())!=vMap_.end() ); }

void TVariableTable::RemoveEntry( wxString const &name )
{ RemoveEntry( GetVariable(name) ); }

TextVariable *TVariableTable::GetEntry( int i )
{
  TVariableMap::iterator k = vMap_.begin();
  for( int j=0; j<i; ++j )++k;
  return (*k).second;
}

void TVariableTable::ClearTable()
{
  TVariableMap::iterator end = vMap_.end();
  for( TVariableMap::iterator i=vMap_.begin(); i!=end; ++i )
  {
    delete (*i).second;
    (*i).second = 0;
  }
  TVariableMap().swap( vMap_ );
  //vMap_.erase( vMap_.begin(), vMap_.end() );
}

void TVariableTable::AddEntry( TextVariable *v )
{
  if( !v )return;
  wxString name( v->GetName() );
  if( Contains(name) )
    throw EVariableError(
      wxString(wxT("The variable "))+name+wxString(wxT(" is already in the table")) );
  vMap_.insert( entry_type(name,v) );
}

void TVariableTable::RemoveEntry( TextVariable *v )
{
  if( !v )return;
  wxString name = v->GetName();
  delete v;
  v = 0;
  vMap_.erase( name );
}

TextVariable *TVariableTable::GetVariable( wxString const &name ) const
{
  if( !name.empty() )
  {
    TVariableMap::const_iterator i = vMap_.find( name.Upper() );
    if( i != vMap_.end() )return (*i).second;
  }
  return 0;
}

std::ostream &operator<<( std::ostream &out, TVariableTable const &table )
{
  if( table.vMap_.size() > 0 )
  {
    out << "<textvariables size=\"" << table.Entries() << "\">\n";
    TVariableTable::TVariableMap::const_iterator end = table.vMap_.end();
    for( TVariableTable::TVariableMap::const_iterator i=table.vMap_.begin(); i!=end; ++i )
      out << *((*i).second);
    out << "</textvariables>\n";
  }
  return out;
}

// end of file
