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
#ifndef EX_TVARIABLETABLE
#define EX_TVARIABLETABLE

#include <map>

#include "wx/wx.h"

class TextVariable;

class TVariableTable
{
private:
  typedef std::map< wxString const, TextVariable* > TVariableMap;
  typedef TVariableMap::value_type entry_type;

 // TVariableTable is the table of pointers to TextVariables
 // TVariableTable is a "singleton" (only one static object)
 // In TVariableTable, each TextVariable pointer is stored
 // with its name as a key to itself, so each variable
 // object must have a unique name

public:
  static TVariableTable *GetTable()
  {
    // returns the pointer to the TVariableTable object.
    // TVariableTable is a "singleton", and can get its pointer by this function. The
    // first time this function is called, the TVariableTable object is instantiated.
    //
    if( !tVariableTable_ )tVariableTable_ = new TVariableTable();
    return tVariableTable_;
  }

  virtual ~TVariableTable()
  { DeleteTable(); }

  static void DeleteTable()
  {
    if( tVariableTable_ )
    {
      tVariableTable_->ClearTable();
      delete tVariableTable_;
      tVariableTable_ = 0;
    }
  }

  int Entries() const
  { return vMap_.size(); }

  TextVariable *GetEntry( int );
  bool Contains( wxString const & ) const;
  TextVariable *GetVariable( wxString const & ) const;

  void AddEntry( TextVariable * );
  void RemoveEntry( TextVariable * );
  void RemoveEntry( wxString const & );
  void ClearTable();

  friend std::ostream &operator<<( std::ostream &, TVariableTable const & );

protected:
  TVariableTable()
  {}

  TVariableTable( TVariableTable const &rhs )
  { vMap_ = rhs.vMap_; }

private:
  TVariableMap vMap_;
  static TVariableTable *tVariableTable_;
};

#endif

