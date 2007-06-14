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
#ifndef EXTREMA_READVECTORSFORM
#define EXTREMA_READVECTORSFORM

#include "wx/wx.h"
#include "wx/grid.h"

class AnalysisWindow;
class ChooseFilePanel;

class ReadVectorsForm : public wxFrame
{
public:
  ReadVectorsForm( AnalysisWindow * );
  
  // event handlers
  void OnApply( wxCommandEvent & );
  void OnClose( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );
  void OnCellChange( wxGridEvent & );
  void OnMakeNew( wxCommandEvent & );
  void OnErrorStop( wxCommandEvent & );
  void OnErrorFill( wxCommandEvent & );
  void OnLineRange( wxCommandEvent & );
  void OnFormat( wxCommandEvent & );

private:
  AnalysisWindow *analysisWindow_;
  ChooseFilePanel *topPanel_;
  wxGrid *vectorGrid_;
  wxCheckBox *closeBefore_, *closeAfter_, *makeNew_, *append_,
             *errorStop_, *errorFill_, *lineRange_, *format_, *messages_;
  wxTextCtrl *errorFillTextCtrl_, *formatTextCtrl_, *incrementTextCtrl_,
             *lastLineTextCtrl_, *firstLineTextCtrl_;
  wxStaticText *firstLineText_, *lastLineText_, *incrementText_;

  enum {
      ID_makeNew,
      ID_stopOnError,
      ID_errorFill,
      ID_lineRange,
      ID_format
  };
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
