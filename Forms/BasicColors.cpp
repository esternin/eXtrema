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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "BasicColors.h"
#include "ExGlobals.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TBasicColorsForm *BasicColorsForm;

__fastcall TBasicColorsForm::TBasicColorsForm( TComponent* Owner ) : TForm(Owner)
{
  int r, g, b;
  int code = 0;
  GRA_colorControl::ColorCodeToRGB( code, r, g, b );
  Shape1->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape2->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape3->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape4->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape5->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape6->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape7->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape8->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape9->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape10->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape11->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape12->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape13->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape14->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape15->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape16->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape17->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape18->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape19->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  GRA_colorControl::ColorCodeToRGB( --code, r, g, b );
  Shape20->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
}

void __fastcall TBasicColorsForm::OnChooseColor0( TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y )
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(0) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor1(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-1) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor2(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-2) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor3(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-3) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor4(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-4) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor5(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-5) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor6(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-6) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor7(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-7) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor8(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-8) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor9(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-9) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor10(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-10) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor11(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-11) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor12(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-12) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor13(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-13) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor14(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-14) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor15(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-15) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor16(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-16) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor17(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-17) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor18(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-18) );
  Close();
}

void __fastcall TBasicColorsForm::OnChooseColor19(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  ExGlobals::SetWorkingColorFlag( true );
  ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-19) );
  Close();
}

