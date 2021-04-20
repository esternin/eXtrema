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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ExGlobals.h"
#include "GRA_colorControl.h"
#include "GRA_window.h"

#include "SelectColor.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TSelectColorForm *SelectColorForm;

__fastcall TSelectColorForm::TSelectColorForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "SelectColorForm", "Top", 249 );
  this->Left = ini->ReadInteger( "SelectColorForm", "Left", 620 );
  delete ini;
  //
  gridClick_ = false;
  namedColorClick_ = false;
}

__fastcall TSelectColorForm::~TSelectColorForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "SelectColorForm", "Top", this->Top );
  ini->WriteInteger( "SelectColorForm", "Left", this->Left );
  delete ini;
}

void __fastcall TSelectColorForm::SetColor( GRA_color *color )
{
  NamedColorRadioGroup->ItemIndex = 0;
  int nColors = GRA_colorControl::GetColorMap()->GetSize();
  int ncols = static_cast<int>( sqrt(static_cast<double>(nColors)) + 0.5 );
  int nrows = nColors/ncols;
  while (ncols*nrows < nColors)++nrows;
  ColorDrawGrid->ColCount = ncols;
  ColorDrawGrid->RowCount = nrows;
  //
  if( color )
  {
    int colorCode = GRA_colorControl::GetColorCode(color);
    if( color->IsNamed() )
    {
      // if Count==19, black coincides with ItemIndex 0, not 1
      // but fill color has an extra entry, "no color", so in that
      // case, black coincides with ItemIndex 1, which is fine
      //
      if( NamedColorRadioGroup->Items->Count == 19 )++colorCode;
      NamedColorRadioGroup->ItemIndex = abs(colorCode);
    }
    else
    {
      int colm = colorCode%ncols;
      int row = (colorCode-colm)/ncols;
      ColorDrawGrid->Col = colm;
      ColorDrawGrid->Row = row;
    }
  }
}

void __fastcall TSelectColorForm::GridClick(TObject *Sender)
{
  gridClick_ = true;
  namedColorClick_ = false;
}

void __fastcall TSelectColorForm::OnDrawCell(TObject *Sender, int ACol,
                                             int ARow, TRect &Rect, TGridDrawState State)
{
  int color = ACol+ARow*ColorDrawGrid->ColCount;
  if( color < GRA_colorControl::GetColorMap()->GetSize() )
  {
    int r, g, b;
    GRA_colorControl::ColorCodeToRGB( color, r, g, b );
    ColorDrawGrid->Canvas->Brush->Color =
        StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
    ColorDrawGrid->Canvas->FillRect( Rect );
  }
}

void __fastcall TSelectColorForm::NamedColorRadioClick(TObject *Sender)
{
  gridClick_ = false;
  namedColorClick_ = true;
}

void __fastcall TSelectColorForm::OKButtonClick(TObject *Sender)
{
  if( gridClick_ )
  {
    int color = ColorDrawGrid->Col+ColorDrawGrid->Row*ColorDrawGrid->ColCount;
    ExGlobals::SetWorkingColorFlag( true );
    ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(color) );
  }
  if( namedColorClick_ )
  {
    int color = NamedColorRadioGroup->ItemIndex;
    //
    // if Count==19, black coincides with ItemIndex 0, not 1
    // but fill color has an extra entry, "no color", so in that
    // case, black coincides with ItemIndex 1, which is fine
    //
    if( NamedColorRadioGroup->Items->Count == 19 )++color;
    ExGlobals::SetWorkingColorFlag( true );
    ExGlobals::SetWorkingColor( GRA_colorControl::GetColor(-color) );
  }
  Close();
}

void __fastcall TSelectColorForm::CancelButtonClick(TObject *Sender)
{ Close(); }

// end of file
