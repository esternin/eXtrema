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

#include "wx/wx.h"

class Variable
{
public:
  Variable();
  Variable( wxString const & );
  virtual ~Variable();
  Variable( Variable const & );
  Variable &operator=( Variable const & );

  void AddToHistory( wxString );
  std::deque<wxString> const &GetHistory();
  void SetHistory( std::deque<wxString> & );
  void SetName( wxString const & );
  wxString &GetName();
  static wxString SimpleName( wxString const & );
  void SetOrigin( wxString const & );
  wxString const &GetOrigin() const;

protected:
  static void CheckReservedName( wxString const & );
  void CopyStuff( Variable const & );

  wxString name_;
  std::deque<wxString> history_;
  wxString origin_;
};

#endif
