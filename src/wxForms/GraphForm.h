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
#ifndef EXTREMA_GRAPHFORM
#define EXTREMA_GRAPHFORM

#include <vector>

#include "wx/wx.h"

class VisualizationWindow;

class GraphForm : public wxFrame
{
public:
  GraphForm( VisualizationWindow * );

  void Set( wxString const &, wxString const &, double, double,
            wxString const &, int, wxString const & );
  void ClearGraphics();

private:
  void CreateForm();
  void FillOutForm();
  void UpdateVectors( wxComboBox * );
  void Draw();

  // event handlers
  void OnPlotsymbolChange( wxCommandEvent & );
  void OnIndepChange( wxCommandEvent & );
  void OnDraw( wxCommandEvent & );
  void OnClear( wxCommandEvent & );
  void OnClose( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );

  VisualizationWindow *visualizationWindow_;
  wxStaticText *dataST_, *indepST_, *errorST_, *symbolST_, *minST_, *maxST_;
  wxComboBox *dataCB_, *indepCB_, *errorCB_, *symbolCB_;
  wxCheckBox *connectCkB_;
  wxTextCtrl *minTC_, *maxTC_;
  wxButton *drawButton_, *clearButton_;

  int color_;
  std::vector<int> symArray_;

  enum {
      ID_ivec,
      ID_symbol,
      ID_clear
  };
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
