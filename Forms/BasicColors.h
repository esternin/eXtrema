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
#ifndef BasicColorsH
#define BasicColorsH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

class TBasicColorsForm : public TForm
{
__published:	// IDE-managed Components
  TShape *Shape1;
  TShape *Shape2;
  TShape *Shape3;
  TShape *Shape4;
  TShape *Shape5;
  TShape *Shape6;
  TShape *Shape7;
  TShape *Shape8;
  TShape *Shape9;
  TShape *Shape10;
  TShape *Shape11;
  TShape *Shape12;
  TShape *Shape13;
  TShape *Shape14;
  TShape *Shape15;
  TShape *Shape16;
  TShape *Shape17;
  TShape *Shape18;
  TShape *Shape19;
  TShape *Shape20;
  void __fastcall OnChooseColor0(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor1(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor2(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor3(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor4(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor5(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor6(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor7(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor8(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor9(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor10(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor11(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor12(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor13(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor14(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor15(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor16(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor17(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor18(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnChooseColor19(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
private:	// User declarations
public:		// User declarations
  __fastcall TBasicColorsForm(TComponent* Owner);
};

extern PACKAGE TBasicColorsForm *BasicColorsForm;

#endif
