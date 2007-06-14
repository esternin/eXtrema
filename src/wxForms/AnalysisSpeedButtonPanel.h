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
#ifndef EXTREMA_ANALYSIS_SPEED_BUTTON_PANEL
#define EXTREMA_ANALYSIS_SPEED_BUTTON_PANEL

#include <iostream>

#include "wx/wx.h"
#include "wx/html/helpctrl.h"

#include "extrema.h"

DECLARE_APP(extrema)

// The AnalysisWindow is the main extrema window. It contains the command history
// and message text control and the command input text control, as well as various
// menus and speed buttons.

class AnalysisWindow;

class AnalysisSpeedButtonPanel : public wxPanel
{
public:
  AnalysisSpeedButtonPanel( AnalysisWindow * );
  
  void OnConstants( wxCommandEvent & );
  void OnClearMessages( wxCommandEvent & );
  void OnExit( wxCommandEvent & );
  void OnShowVariables( wxCommandEvent & );
  void OnExecute( wxCommandEvent & );
  void OnGenerateVector( wxCommandEvent & );
  void OnFit( wxCommandEvent & );
  void OnHelp( wxCommandEvent & );
  void OnStackToggle( wxCommandEvent & );
  void OnSaveSession( wxCommandEvent & );
  void OnRestoreSession( wxCommandEvent & );

private:
  AnalysisWindow *analysisWindow_;
  
  enum {
      ID_execute,
      ID_show,
      ID_fit,
      ID_generate,
      ID_constants,
      ID_saveSession,
      ID_restoreSession,
      ID_clearMessages,
      ID_stackToggle
  };

  DECLARE_EVENT_TABLE()
};

#endif
