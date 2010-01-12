/*
Copyright (C) 2010 Joseph L. Chuma

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
#ifndef EXTREMA_THREE_D_PLOT_FORM
#define EXTREMA_THREE_D_PLOT_FORM

#include <vector>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/textctrl.h>
#include <wx/frame.h>

class GRA_threeDPlot;
class VisualizationWindow;

class ThreeDPlotForm : public wxFrame 
{
public:
  ThreeDPlotForm( VisualizationWindow * );

  ~ThreeDPlotForm()
  {}
  
  void ZeroPlot()
  { threeDPlot_ = 0; }
  
private:
  void CreateForm();
  void UpdateVectors( wxComboBox * );
  void GetDataVectors( std::vector<double> &, std::vector<double> &, std::vector<double> & );
  void ReDraw();

  // event handlers
  void OnAngleIncChange( wxCommandEvent & );
  void OnEye2ObjChange( wxCommandEvent & );
  void OnLeft( wxCommandEvent & );
  void OnRight( wxCommandEvent & );
  void OnUp( wxCommandEvent & );
  void OnDown( wxCommandEvent & );
  void OnZoomIn( wxCommandEvent & );
  void OnZoomOut( wxCommandEvent & );
  void OnDraw( wxCommandEvent & );
  void OnClose( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );

  VisualizationWindow *visualizationWindow_;
  GRA_threeDPlot *threeDPlot_;
  wxComboBox *xVecComboBox_, *yVecComboBox_, *zVecComboBox_;
  wxTextCtrl *angleIncTextCtrl_, *eye2objTextCtrl_;
  double angleIncrement_, eye2obj_;

  enum 
  {
      ID_angleInc,
      ID_eye2obj,
      ID_draw,
      ID_left,
      ID_right,
      ID_up,
      ID_down,
      ID_zoomIn,
      ID_zoomOut
  };
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()

};

#endif
