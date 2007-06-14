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
#ifndef EXTREMA_TEXTFORM
#define EXTREMA_TEXTFORM

#include "wx/wx.h"

class VisualizationWindow;

class TextForm : public wxFrame
{
public:
  TextForm( VisualizationWindow * );
  void Erase();
  void Set( wxString const &, bool, double, double, int, wxString const & );

private:
  void CreateForm();
  void FillOutForm();
  void SetupInteractive();
  void Draw();
  
  // event handlers
  void OnUL( wxCommandEvent & );
  void OnUC( wxCommandEvent & );
  void OnUR( wxCommandEvent & );
  void OnCL( wxCommandEvent & );
  void OnCC( wxCommandEvent & );
  void OnCR( wxCommandEvent & );
  void OnLL( wxCommandEvent & );
  void OnLC( wxCommandEvent & );
  void OnLR( wxCommandEvent & );
  void OnInteractive( wxCommandEvent & );
  void OnFont( wxCommandEvent & );
  void OnErase( wxCommandEvent & );
  void OnDraw( wxCommandEvent & );
  void OnClose( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );

  VisualizationWindow *visualizationWindow_;
  wxTextCtrl *stringTC_, *locxTC_, *locyTC_;
  wxCheckBox *interactiveCkB_;
  wxStaticText *locxST_, *locyST_;
  wxRadioButton *alignmentRB_[9];

  enum {
      ID_interactive,
      ID_ul,
      ID_uc,
      ID_ur,
      ID_cl,
      ID_cc,
      ID_cr,
      ID_ll,
      ID_lc,
      ID_lr,
      ID_font,
      ID_erase
  };

  enum { SIZE = 9 };
  static int gwToRadio_[SIZE];

  bool fromSet_;

  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
