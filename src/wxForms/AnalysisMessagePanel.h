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
#ifndef EXTREMA_ANALYSIS_MESSAGE_PANEL
#define EXTREMA_ANALYSIS_MESSAGE_PANEL

#include "wx/wx.h"

// The AnalysisWindow is the main extrema window. It contains the command history
// and message text control and the command input text control, as well as various
// menus and speed buttons.

class AnalysisWindow;
class CommandHistoryWindow;

class AnalysisMessagePanel : public wxPanel
{
public:
  AnalysisMessagePanel( AnalysisWindow * );
  void WriteOutput( wxString const & );
  void ClearOutput();
  wxString GetValue();

private:
  void OnMouseLeftDown( wxMouseEvent & );

  CommandHistoryWindow *cmndHistoryWindow_;

  DECLARE_EVENT_TABLE()
};

class CommandHistoryWindow : public wxTextCtrl
{
public:
  CommandHistoryWindow( AnalysisMessagePanel * );
  void WriteOutput( wxString const & );

private:
  void OnMouseRightDown( wxMouseEvent & );

  enum {
      ID_commandHistory,
  };

  DECLARE_EVENT_TABLE()
};

#endif
