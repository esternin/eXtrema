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
#ifndef EX_VARIABLE
#define EX_VARIABLE

#include <deque>

#include "ExString.h"

class Variable
{
public:
  Variable()
  {}

  Variable( ExString const &name ) : name_(name.UpperCase())
  {}

  virtual ~Variable()
  { std::deque<ExString>().swap( history_ ); }

  Variable( Variable const &rhs )
  { CopyStuff(rhs); }

  Variable &operator=( Variable const &rhs )
  {
    if( this != &rhs )CopyStuff(rhs);
    return *this;
  }

  void AddToHistory( ExString );

  std::deque<ExString> const &GetHistory()
  { return history_; }

  void SetHistory( std::deque<ExString> &s )
  { history_.swap( s ); }

  void SetName( ExString const &name )
  { name_ = name; }

  ExString &GetName()
  { return name_; }

  static ExString SimpleName( ExString const & );

  void SetOrigin( ExString const &origin )
  { origin_ = origin; }

  ExString const &GetOrigin() const
  { return origin_; }

protected:
  static void CheckReservedName( ExString const & );

  void CopyStuff( Variable const &rhs )
 {
    name_ = rhs.name_;
    history_.assign( rhs.history_.begin(), rhs.history_.end() );
  }

  ExString name_;
  std::deque<ExString> history_;
  ExString origin_;
};

#endif
