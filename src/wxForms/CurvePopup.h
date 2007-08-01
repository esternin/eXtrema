/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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
#ifndef EXTREMA_CURVEPOPUP
#define EXTREMA_CURVEPOPUP

#include "wx/wx.h"

#include "ExSpinCtrlD.h"
#include "ExSpinCtrlI.h"
#include "ExColorCtrl.h"

class GraphicsPage;
class GRA_window;
class GRA_setOfCharacteristics;
class GRA_color;
class GRA_cartesianCurve;

class CurvePopup : public wxFrame
{
public:
  CurvePopup( GraphicsPage * );

  void Setup( GRA_window *, GRA_cartesianCurve * );

  void OnCurveColor( GRA_color * );
  void OnAreafillColor( GRA_color * );
  void OnPlotsymbolColor( GRA_color * );
  void OnPlotsymbolCode( int );
  void OnCurveLineWidth( int );
  void OnCurveLineType( int );
  void OnPlotsymbolAngle( int );
  void OnPlotsymbolSize( double );

private:
  void CreateForm();
  void ReDraw();

  // event handlers
  void OnCurveType( wxCommandEvent & );
  void OnConnect( wxCommandEvent & );
  void OnClose( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );

  bool setup_;
  GraphicsPage *page_;
  GRA_window *window_;
  GRA_cartesianCurve *curve_;
  GRA_setOfCharacteristics *curveChars_;

  wxRadioBox *histogramRB_;
  ExColorCtrl<CurvePopup> *curveColor_, *areafillColor_, *plotsymbolColor_;
  ExSpinCtrlI<CurvePopup> *lineWidthSC_, *lineTypeSC_;
  ExSpinCtrlI<CurvePopup> *plotsymbolCodeSC_, *plotsymbolAngleSC_;
  ExSpinCtrlD<CurvePopup> *plotsymbolSizeSC_;
  wxCheckBox *connectCB_;
  wxPanel *plotsymbolPanel_;
  wxButton *closeBTN_;

  enum {
      ID_curveType,
      ID_connect
  };
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
