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
#ifndef EXTREMA_COMMAND_TEXT_CTRL
#define EXTREMA_COMMAND_TEXT_CTRL

#include <vector>

#include "wx/wx.h"

// The CommandTextCtrl is the command entry field in the analysis window.

class AnalysisCommandPanel;

class CommandTextCtrl : public wxTextCtrl
{
public:
  CommandTextCtrl( AnalysisCommandPanel * );
  ~CommandTextCtrl();

  void OnKeyDown( wxKeyEvent & );

  void DoACommand();

  void AddCommandString( wxString const & );

  std::vector<wxString> &GetCommandStrings()
  { return commandStrings_; }

private:
  std::vector<wxString> commandStrings_;
  int commandStringsIndex_;

  wxString GetACommand();
  wxString GetPreviousCommand();
  wxString GetNextCommand();
  
  DECLARE_EVENT_TABLE()
};

#endif
