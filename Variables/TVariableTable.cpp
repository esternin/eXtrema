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

#include "EExpressionError.h"
#include "TextVariable.h"
#include "TVariableTable.h"

TVariableTable *TVariableTable::tVariableTable_ = 0;

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
  ExString name( v->GetName() );
  if( Contains(name) )
    throw EExpressionError(
      ExString("The variable ")+name+ExString(" is already in the table") );
  vMap_.insert( entry_type(name,v) );
}

void TVariableTable::RemoveEntry( TextVariable *v )
{
  if( !v )return;
  ExString name = v->GetName();
  delete v;
  v = 0;
  if( !vMap_.erase(name) )
    throw EExpressionError( ExString("The variable ")+name+" is not removed from the text variable table" );
}

TextVariable *TVariableTable::GetVariable( ExString const &name ) const
{
  if( !name.empty() )
  {
    TVariableMap::const_iterator i = vMap_.find( name.UpperCase() );
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
    for( TVariableTable::TVariableMap::const_iterator i=table.vMap_.begin(); i!=end; ++i )out << *((*i).second);
    out << "</textvariables>\n";
  }
  return out;
}

// end of file
