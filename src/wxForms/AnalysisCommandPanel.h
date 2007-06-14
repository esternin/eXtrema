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
#ifndef EXTREMA_ANALYSIS_COMMAND_PANEL
#define EXTREMA_ANALYSIS_COMMAND_PANEL

#include <vector>

#include "wx/wx.h"

// IDs for the controls and the menu commands

// The AnalysisWindow is the main extrema window. It contains the command history
// and message text control and the command input text control, as well as various
// menus and speed buttons.

class AnalysisWindow;
class CommandTextCtrl;

class AnalysisCommandPanel : public wxPanel
{
public:
  AnalysisCommandPanel( AnalysisWindow * );
  void OnProcessCommand( wxCommandEvent & );
  void AddCommandString( wxString const & );
  std::vector<wxString> &GetCommandStrings();
  
private:
  CommandTextCtrl *commandTextCtrl_;

  enum {
      ID_processCommand
  };

  DECLARE_EVENT_TABLE()
};

#endif
