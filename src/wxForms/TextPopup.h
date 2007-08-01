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
#ifndef EXTREMA_TEXTPOPUP
#define EXTREMA_TEXTPOPUP

#include "wx/wx.h"

#include "ExSpinCtrlD.h"
#include "ExSpinCtrlI.h"
#include "ExColorCtrl.h"

class GraphicsPage;
class GRA_window;
class GRA_setOfCharacteristics;
class GRA_color;
class GRA_drawableText;

class TextPopup : public wxFrame
{
public:
  TextPopup( GraphicsPage * );

  void Setup( GRA_window *, GRA_drawableText * );

  void OnColor( GRA_color * );
  void OnHeight( double );
  void OnAngle( int );
  void OnXLocation( double );
  void OnYLocation( double );

private:
  void CreateForm();
  void ReDraw();

  // event handlers
  void OnStringEnter( wxCommandEvent & );
  void OnFont( wxCommandEvent & );
  void OnClose( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );

  bool setup_;
  GraphicsPage *page_;
  GRA_window *window_;
  GRA_drawableText *drawableText_;
  GRA_setOfCharacteristics *textChars_;

  ExColorCtrl<TextPopup> *color_;
  ExSpinCtrlI<TextPopup> *angleSC_;
  ExSpinCtrlD<TextPopup> *heightSC_, *xLocSC_, *yLocSC_;
  wxComboBox *fontCB_;  
  wxTextCtrl *stringTC_;
  wxButton *closeBTN_;

  enum {
      ID_font,
      ID_string
  };
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
