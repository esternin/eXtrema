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
#ifndef EXTREMA_FIGUREFORM
#define EXTREMA_FIGUREFORM

#include "wx/wx.h"

#include "ExSpinCtrlI.h"
#include "ExColorCtrl.h"

class VisualizationWindow;
class GRA_color;

class FigureForm : public wxFrame 
{
public:
  FigureForm( VisualizationWindow * );

private:
  void CreateForm();
  void FillOutForm( int, bool, bool, int, int );
  void OnArrow1( wxCommandEvent & );
  void OnArrow2( wxCommandEvent & );
  void OnArrow3( wxCommandEvent & );
  void OnRectangle( wxCommandEvent & );
  void OnSquare( wxCommandEvent & );
  void OnStar( wxCommandEvent & );
  void OnDrawArrow( wxCommandEvent & );
  void OnTwoHeads( wxCommandEvent & );
  void OnCircle( wxCommandEvent & );
  void OnPolygonAngle( int );
  void OnDrawPolygon( wxCommandEvent & );
  void OnDrawEllipse( wxCommandEvent & );
  void OnLineThickness( int );
  void OnLineColor( GRA_color * );
  void OnFillColor( GRA_color * );
  void OnClose( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );
  
  wxBitmapButton *arrow1_, *arrow2_, *arrow3_, *rectangle_, *square_, *star_;
  wxCheckBox *twoHeadsCheckBox_, *makeCircleCheckBox_;
  ExSpinCtrlI<FigureForm> *lineThicknessSC_, *polygonAngleSC_;
  ExColorCtrl<FigureForm> *lineColorCC_, *fillColorCC_;

  VisualizationWindow *visualizationWindow_;
  int arrowType_, polygonType_;
  double polygonAngle_;

  enum 
  {
      ID_arrow1,
      ID_arrow2,
      ID_arrow3,
      ID_rectangle,
      ID_square,
      ID_star,
      ID_twoHeads,
      ID_circle,
      ID_angle,
      ID_drawArrow,
      ID_drawPolygon,
      ID_drawEllipse,
      ID_linethickness,
      ID_linecolor,
      ID_fillcolor
  };
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
