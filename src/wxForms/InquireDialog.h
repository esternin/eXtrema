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
#ifndef EXTREMA_INQUIRE_DIALOG
#define EXTREMA_INQUIRE_DIALOG

#include "wx/wx.h"

class InquireDialog : public wxDialog
{
public:
  InquireDialog();
  void SetLabel( wxString const & );
  
  // event handlers
  void OnOK( wxCommandEvent & );
  void OnStopAll( wxCommandEvent& );
  void CloseEventHandler( wxCloseEvent & );

private:
  wxStaticText *messageCtrl_;
  wxTextCtrl *textCtrl_;

  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
