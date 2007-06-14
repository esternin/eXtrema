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
#ifndef EXTREMA_PEAKFINDFORM
#define EXTREMA_PEAKFINDFORM

#include <vector>

#include "wx/wx.h"
#include "wx/grid.h"

class GRA_arrow3;
class GRA_cartesianCurve;
class VisualizationWindow;

class PeakFindForm : public wxFrame
{
public:
  PeakFindForm( VisualizationWindow * );

private:
  void DrawArrow();
  void EraseLastArrow();
  void Initialize();
  void ShowValues();

  // event handlers
  void OnLeft( wxCommandEvent & );
  void OnRight( wxCommandEvent & );
  void OnSave( wxCommandEvent & );
  void OnInitialize( wxCommandEvent & );
  void OnClose( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );

  VisualizationWindow *visualizationWindow_;
  wxRadioBox *maxMinRB_;
  wxTextCtrl *pxTC_, *pyTC_, *xvTC_, *yvTC_;

  bool first_;
  double xCurrent_, yCurrent_;
  int jPeak_;
  GRA_arrow3 *currentArrow_;
  GRA_cartesianCurve *curve_;
  std::vector<double> xVec_, yVec_;
  std::vector<double> xData_, yData_;
  int npts_;
  double arrowLength_;
  double xMin_, xMax_, xLogBase_, yMin_, yMax_, yLogBase_, yRange_;

  enum {
      ID_left,
      ID_right,
      ID_save,
      ID_initialize
  };
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
