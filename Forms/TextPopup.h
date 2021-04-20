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
#ifndef TextPopupH
#define TextPopupH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

class GRA_page;
class GRA_window;
class GRA_setOfCharacteristics;
class GRA_drawableText;

class TTextPopupForm : public TForm
{
__published:	// IDE-managed Components
  TLabel *StringLabel;
  TEdit *StringEdit;
  TLabel *AngleLabel;
  TEdit *AngleEdit;
  TUpDown *AngleUpDown;
  TLabel *ColorLabel;
  TShape *ColorShape;
  TLabel *HeightLabel;
  TEdit *HeightEdit;
  TUpDown *HeightUpDown;
  TLabel *FontLabel;
  TComboBox *FontComboBox;
  TButton *CloseButton;
  TLabel *XLocLabel;
  TEdit *XLocEdit;
  TUpDown *XLocUpDown;
  TLabel *YLocLabel;
  TEdit *YLocEdit;
  TUpDown *YLocUpDown;
  void __fastcall OnCloseClick(TObject *Sender);
  void __fastcall OnCloseForm(TObject *Sender, TCloseAction &Action);
  void __fastcall OnCloseQueryForm(TObject *Sender, bool &CanClose);
  void __fastcall OnStringEditKeyPress(TObject *Sender, char &Key);
  void __fastcall OnColorMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnHeightChange(TObject *Sender);
  void __fastcall OnHeightClick(TObject *Sender, TUDBtnType Button);
  void __fastcall OnAngleChange(TObject *Sender);
  void __fastcall OnAngleClick(TObject *Sender, TUDBtnType Button);
  void __fastcall OnFontChange(TObject *Sender);
  void __fastcall OnXLocChange(TObject *Sender);
  void __fastcall OnXLocClick(TObject *Sender, TUDBtnType Button);
  void __fastcall OnYLocChange(TObject *Sender);
  void __fastcall OnYLocClick(TObject *Sender, TUDBtnType Button);

private:	// User declarations
  GRA_page *page_;
  GRA_window *window_;
  GRA_setOfCharacteristics *textChars_;
  GRA_drawableText *drawableText_;
  bool setup_;

public:		// User declarations
  __fastcall TTextPopupForm(TComponent* Owner);
  __fastcall ~TTextPopupForm();

  void Disconnect();
  void Setup( GRA_page *, GRA_window *, GRA_drawableText * );
  void ReDraw();
};

extern PACKAGE TTextPopupForm *TextPopupForm;

#endif
