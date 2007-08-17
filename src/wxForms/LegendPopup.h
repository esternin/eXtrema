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
#ifndef EXTREMA_LEGENDPOPUP
#define EXTREMA_LEGENDPOPUP

#include "wx/wx.h"

#include "ExSlider.h"
#include "ExSpinCtrlD.h"
#include "ExColorCtrl.h"

class GraphicsPage;
class GRA_window;
class GRA_setOfCharacteristics;
class GRA_color;
class GRA_legend;

class LegendPopup : public wxFrame
{
public:
  LegendPopup( GraphicsPage * );

  void Setup( GRA_window *, GRA_legend * );

  void OnFrameColor( GRA_color * );
  void OnTitleColor( GRA_color * );
  void OnTitleHeight( double );
  void OnMoveH( double );
  void OnMoveV( double );
  void OnExpandH( double );
  void OnExpandV( double );

  void Disconnect();

private:
  void CreateForm();
  void ReDraw();

  // event handlers
  void OnTitleEnter( wxCommandEvent & );
  void OnTitleFont( wxCommandEvent & );
  void OnDrawLegend( wxCommandEvent & );
  void OnDrawFrame( wxCommandEvent & );
  void OnDrawTitle( wxCommandEvent & );

  void OnClose( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );

  bool setup_;
  GraphicsPage *page_;
  GRA_window *window_;
  GRA_legend *legend_;
  GRA_setOfCharacteristics *legChars_;

  ExColorCtrl<LegendPopup> *frameColor_, *titleColor_;
  ExSpinCtrlD<LegendPopup> *titleHeightSC_;
  ExSlider<LegendPopup> *moveH_, *moveV_, *expandH_, *expandV_;
  wxComboBox *titleFontCB_;
  wxTextCtrl *titleTC_;
  wxCheckBox *drawLegendCB_, *drawFrameCB_, *drawTitleCB_;
  wxButton *closeBTN_;

  enum {
      ID_font,
      ID_title,
      ID_drawLegend,
      ID_drawFrame,
      ID_drawTitle
  };
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
