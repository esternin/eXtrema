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
#ifndef EXTREMA_STACK_DIALOG
#define EXTREMA_STACK_DIALOG

#include "wx/wx.h"

class AnalysisWindow;
class ChooseFilePanel;

class StackDialog : public wxFrame
{
public:
  StackDialog( AnalysisWindow * );

private:
  void Toggle();

  // event handlers
  void OnOffToggle( wxCommandEvent & );
  void OnApply( wxCommandEvent & );
  void OnClose( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );
  
  AnalysisWindow *analysisWindow_;
  wxRadioBox *onOffRB_;
  ChooseFilePanel *chooseFilePanel_;
  wxCheckBox *appendCkB_;

  enum {
      ID_onoff 
  };
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
