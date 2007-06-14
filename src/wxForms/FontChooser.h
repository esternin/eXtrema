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
#ifndef EXTREMA_FONTCHOOSER
#define EXTREMA_FONTCHOOSER

#include "wx/wx.h"
#include "wx/grid.h"
#include "wx/notebook.h"

class MySampleText;

class FontChooser : public wxDialog
{
public:
  FontChooser( wxWindow * );
  void SetFont( int, int, double, double );

private:
  void CreateForm();
  void SetFontSample();

  // event handlers
  void OnWhite( wxCommandEvent & );
  void OnPurple( wxCommandEvent & );
  void OnSalmon( wxCommandEvent & );
  void OnMaroon( wxCommandEvent & );
  void OnBlack( wxCommandEvent & );
  void OnYellow( wxCommandEvent & );
  void OnSienna( wxCommandEvent & );
  void OnNavy( wxCommandEvent & );
  void OnRed( wxCommandEvent & );
  void OnCyan( wxCommandEvent & );
  void OnTan( wxCommandEvent & );
  void OnOlive( wxCommandEvent & );
  void OnGreen( wxCommandEvent & );
  void OnBrown( wxCommandEvent & );
  void OnFuchsia( wxCommandEvent & );
  void OnSilver( wxCommandEvent & );
  void OnBlue( wxCommandEvent & );
  void OnCoral( wxCommandEvent & );
  void OnLime( wxCommandEvent & );
  void OnTeal( wxCommandEvent & );
  void OnFont( wxCommandEvent & );
  void OnMap( wxGridEvent & );
  void OnOK( wxCommandEvent & );
  void OnCancel( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );

  int colorCode_, fontCode_;
  
  wxWindow *parent_;
  MySampleText *sampleText_;
  wxRadioButton *namedRB_[20];
  wxTextCtrl *heightTC_, *angleTC_;
  wxComboBox *fontCB_;
  wxNotebook *colorNB_;
  wxNotebookPage *namedPage_, *mapPage_;
  wxGrid *mapGrid_;
  
  enum {
      ID_font,
      ID_map,
      ID_white,
      ID_purple,
      ID_salmon,
      ID_maroon,
      ID_black,
      ID_yellow,
      ID_sienna,
      ID_navy,
      ID_red,
      ID_cyan,
      ID_tan,
      ID_olive,
      ID_green,
      ID_brown,
      ID_fuchsia,
      ID_silver,
      ID_blue,
      ID_coral,
      ID_lime,
      ID_teal
  };

  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

class MySampleText : public wxWindow
{
public:
  MySampleText( wxWindow * );
  void SetText( wxFont &, wxColour );

private:
  wxFont font_;
  wxColour colour_;

  void OnPaint( wxPaintEvent & );
 
  DECLARE_EVENT_TABLE()
};


#endif
