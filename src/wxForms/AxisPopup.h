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
#ifndef EXTREMA_AXISPOPUP
#define EXTREMA_AXISPOPUP

#include "wx/wx.h"
#include "wx/notebook.h"

#include "ExSpinCtrlD.h"
#include "ExSpinCtrlI.h"
#include "ExColorCtrl.h"

class GraphicsPage;
class GRA_window;
class GRA_setOfCharacteristics;
class GRA_color;

class AxisPopup : public wxFrame
{
public:
  AxisPopup( GraphicsPage * );

  void Setup( GRA_window *, char const );

  void OnMajorTicLen( double );
  void OnMinorTicLen( double );
  void OnNumbersHeight( double );
  void OnLabelHeight( double );
  void OnLowAxis( double );
  void OnUpAxis( double );
  void OnLineWidth( int );
  void OnLineType( int );
  void OnMinorIncs( int );
  void OnNumbersAngle( int );
  void OnAxisColor( GRA_color * );
  void OnNumbersColor( GRA_color * );
  void OnLabelColor( GRA_color * );

private:
  void CreateForm();
  void MakeAxisPanel();
  void MakeNumbersPanel();
  void MakeLabelPanel();
  void MakeLocationPanel();
  void MakeAxisScalesPanel();
  void ReDraw();

  // event handlers
  void OnDrawAxis( wxCommandEvent & );
  void OnDrawGraphBox( wxCommandEvent & );
  void OnGridLines( wxCommandEvent & );
  void OnDrawTics( wxCommandEvent & );
  void OnTicDirection( wxCommandEvent & );
  void OnDrawNumbers( wxCommandEvent & );
  void OnDrawLabel( wxCommandEvent & );
  void OnNumbersFont( wxCommandEvent & );
  void OnLabel( wxCommandEvent & );
  void OnLabelEnter( wxCommandEvent & );
  void OnLabelFont( wxCommandEvent & );
  void OnApplyScales( wxCommandEvent & );
  void OnLogStyle( wxCommandEvent & );
  void OnLogBase( wxCommandEvent & );
  void OnClose( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );

  bool setup_;
  GraphicsPage *page_;
  GRA_window *window_;
  GRA_setOfCharacteristics *axisChars_;
  char which_;

  wxNotebook *notebook_;
  wxCheckBox *drawAxisCB_, *drawGraphBoxCB_, *drawTicsCB_;
  wxCheckBox *drawNumbersCB_, *drawLabelCB_, *logStyleCB_;
  ExSpinCtrlI<AxisPopup> *lineWidthSC_, *lineTypeSC_, *minorIncsSC_, *numbersAngleSC_;
  ExSpinCtrlD<AxisPopup> *majorTicLenSC_, *minorTicLenSC_, *numbersHeightSC_;
  ExSpinCtrlD<AxisPopup> *labelHeightSC_, *lowAxisSC_, *upAxisSC_;
  ExColorCtrl<AxisPopup> *axisColor_, *numbersColor_, *labelColor_;
  wxRadioBox *gridLinesRB_, *ticDirectionRB_, *logBaseRB_;
  wxComboBox *nFontCB_, *lFontCB_;  
  wxTextCtrl *labelTC_, *minTC_, *maxTC_;
  wxButton *applyScalesBTN_, *closeBTN_;

  enum {
      ID_drawAxis,
      ID_drawGraphBox,
      ID_gridLines,
      ID_drawTics,
      ID_ticDirection,
      ID_drawNumbers,
      ID_numbersFont,
      ID_drawLabel,
      ID_label,
      ID_label2,
      ID_labelFont,
      ID_applyScales,
      ID_logStyle,
      ID_logBase
  };
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
