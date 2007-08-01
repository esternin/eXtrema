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
#ifndef EXTREMA_BASICCOLORS
#define EXTREMA_BASICCOLORS

#include "wx/wx.h"
#include "wx/dialog.h"

class BasicColors : public wxDialog
{
public:
  BasicColors( wxWindow * );

private:
  // event handlers
  void CloseEventHandler( wxCloseEvent & );
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

class ColorButton : public wxButton
{
public:
  ColorButton( wxPanel *, BasicColors *, int );

private:
  BasicColors *owner_;
  int colorCode_;

  // event handlers
  void OnClick( wxCommandEvent & );

  enum { ID_button };

  DECLARE_EVENT_TABLE()
};

#endif
