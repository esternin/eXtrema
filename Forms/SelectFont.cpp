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
#include "MainGraphicsWindow.h"
#include "GRA_window.h"
#include "GRA_colorControl.h"
#include "GRA_font.h"
#include "GRA_fontControl.h"
#include "GRA_borlandW32.h"

#include "SelectFont.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TSelectFontForm *SelectFontForm;

__fastcall TSelectFontForm::TSelectFontForm( TComponent* Owner )
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "SelectFontForm", "Top", 99 );
  Left = ini->ReadInteger( "SelectFontForm", "Left", 527 );
  Width = ini->ReadInteger( "SelectFontForm", "Width", 255 );
  Height = ini->ReadInteger( "SelectFontForm", "Height", 425 );
  colorCode_ = ini->ReadInteger( "SelectFontForm", "ColorCode", -1 );
  delete ini;
  //
  gridClick_ = false;
  namedColorClick_ = false;
}

__fastcall TSelectFontForm::~TSelectFontForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "SelectFontForm", "Top", Top );
  ini->WriteInteger( "SelectFontForm", "Left", Left );
  ini->WriteInteger( "SelectFontForm", "Width", Width );
  ini->WriteInteger( "SelectFontForm", "Height", Height );
  ini->WriteInteger( "SelectFontForm", "ColorCode", colorCode_ );
  delete ini;
}

void __fastcall TSelectFontForm::SetFont( GRA_font const *font )
{
  TCanvas *tc = MainGraphicsForm->GetCurrentImage()->Canvas;
  TFont *tfontSave = new TFont();
  tfontSave->Assign( tc->Font );
  std::size_t end = GRA_fontControl::GetCount();
  for( std::size_t i=0; i<end; ++i )
  {
    tc->Font->Name = GRA_fontControl::GetFont(i)->GetName().c_str();
    HDC hdc = tc->Font->Handle;
    TEXTMETRIC tm;
    GetTextMetrics( hdc, &tm );
    if( (tm.tmPitchAndFamily & TMPF_TRUETYPE) == tm.tmPitchAndFamily )
      FontsComboBox->Items->Add( tc->Font->Name );
  }
  tc->Font->Assign( tfontSave );
  delete tfontSave;
  //
  TFont *tfont = GRA_borlandW32::TranslateFont(font);
  FontsComboBox->Text = tfont->Name;
  FontsRichEdit->Font->Assign( tfont );
  FontsRichEdit->Text = AnsiString("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz");
  delete tfont;
  //
  int nColors = GRA_colorControl::GetColorMap()->GetSize();
  int ncols = static_cast<int>( sqrt(static_cast<double>(nColors)) + 0.5 );
  int nrows = nColors/ncols;
  while( ncols*nrows < nColors )++nrows;
  FontColorDrawGrid->ColCount = ncols;
  FontColorDrawGrid->RowCount = nrows;
  //
  if( colorCode_ < 0 )
  {
    NamedColorRadioGroup->ItemIndex = abs(colorCode_);
  }
  else
  {
    int Nc = FontColorDrawGrid->ColCount;
    int colm = colorCode_%Nc;
    int row = (colorCode_-colm)/Nc;
    FontColorDrawGrid->Col = colm;
    FontColorDrawGrid->Row = row;
  }
}

void __fastcall TSelectFontForm::SetHeight( double height )
{ HeightEdit->Text = AnsiString(height); }

void __fastcall TSelectFontForm::SetAngle( double angle )
{ AngleEdit->Text = AnsiString(angle); }

void __fastcall TSelectFontForm::DisableAngle()
{
  AngleEdit->Enabled = false;
  AngleLabel->Enabled = false;
}

void __fastcall TSelectFontForm::OKClick( TObject *Sender )
{
  if( FontsComboBox->ItemIndex > -1 )
    ExGlobals::SetWorkingFont(
     GRA_fontControl::GetFont(FontsComboBox->Items->Strings[FontsComboBox->ItemIndex].c_str()) );
  if( gridClick_ )
  {
    colorCode_ = FontColorDrawGrid->Col+FontColorDrawGrid->Row*FontColorDrawGrid->ColCount;
    ExGlobals::SetWorkingFontColor( GRA_colorControl::GetColor(colorCode_) );
  }
  if( namedColorClick_ )
  {
    colorCode_ = -1*NamedColorRadioGroup->ItemIndex;
    ExGlobals::SetWorkingFontColor( GRA_colorControl::GetColor(colorCode_) );
  }
  if( heightChanged_ )
  {
    double height;
    try
    {
      height = HeightEdit->Text.ToDouble();
    }
    catch (...)
    {
      Application->MessageBox( "invalid value entered for the height", "Error", MB_OK );
      return;
    }
    ExGlobals::SetWorkingFontHeight( height );
  }
  if( angleChanged_ )
  {
    double angle;
    try
    {
      angle = AngleEdit->Text.ToDouble();
    }
    catch (...)
    {
      Application->MessageBox( "invalid value entered for the angle", "Error", MB_OK );
      return;
    }
    ExGlobals::SetWorkingFontAngle( angle );
  }
  Close();
}

void __fastcall TSelectFontForm::FontsComboBoxClick( TObject *Sender )
{ FontsRichEdit->Font->Name = FontsComboBox->Items->Strings[FontsComboBox->ItemIndex]; }

void __fastcall TSelectFontForm::OnDrawCell( TObject *Sender, int ACol,
                                             int ARow, TRect &Rect, TGridDrawState State )
{
  int colorCode = ACol+ARow*FontColorDrawGrid->ColCount;
  int r, g, b;
  GRA_colorControl::ColorCodeToRGB( colorCode, r, g, b );
  FontColorDrawGrid->Canvas->Brush->Color =
      StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  FontColorDrawGrid->Canvas->FillRect( Rect );
}

void __fastcall TSelectFontForm::FontsGridClick( TObject *Sender )
{
  int colorCode = FontColorDrawGrid->Col+FontColorDrawGrid->Row*FontColorDrawGrid->ColCount;
  int r, g, b;
  GRA_colorControl::ColorCodeToRGB( colorCode, r, g, b );
  FontsRichEdit->Font->Color =
    StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  gridClick_ = true;
  namedColorClick_ = false;
}

void __fastcall TSelectFontForm::CancelClick( TObject *Sender )
{ Close(); }

void __fastcall TSelectFontForm::NamedColorRadioClick( TObject *Sender )
{
  int colorCode = -1*NamedColorRadioGroup->ItemIndex;
  int r, g, b;
  GRA_colorControl::ColorCodeToRGB( colorCode, r, g, b );
  FontsRichEdit->Font->Color =
    StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  gridClick_ = false;
  namedColorClick_ = true;
}

void __fastcall TSelectFontForm::FormClose( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TSelectFontForm::HeightChange( TObject *Sender )
{ heightChanged_ = true; }

void __fastcall TSelectFontForm::AngleChange( TObject *Sender )
{ angleChanged_ = true; }

// end of file
