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

#include "LegendPopup.h"
#include "GRA_page.h";
#include "GRA_window.h";
#include "GRA_setOfCharacteristics.h";
#include "GRA_legend.h";
#include "GRA_rectangle.h"
#include "GRA_colorControl.h"
#include "GRA_fontControl.h"
#include "BasicColors.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TLegendPopupForm *LegendPopupForm;

__fastcall TLegendPopupForm::TLegendPopupForm( TComponent* Owner ) : TForm(Owner)
{
  setup_ = true;
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "LegendPopupForm", "Top", 220 );
  Left = ini->ReadInteger( "LegendPopupForm", "Left", 850 );
  Width = ini->ReadInteger( "LegendPopupForm", "Width", 270 );
  Height = ini->ReadInteger( "LegendPopupForm", "Height", 515 );
  delete ini;
}

__fastcall TLegendPopupForm::~TLegendPopupForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "LegendPopupForm", "Top", Top );
  ini->WriteInteger( "LegendPopupForm", "Left", Left );
  ini->WriteInteger( "LegendPopupForm", "Width", Width );
  ini->WriteInteger( "LegendPopupForm", "Height", Height );
  delete ini;
  if( legend_ )legend_->Disconnect();
}

void TLegendPopupForm::Setup( GRA_page *page, GRA_window *window, GRA_legend *legend )
{
  setup_ = true;
  page_ = page;
  window_ = window;
  legChars_ = window_->GetGraphLegendCharacteristics();
  legend_ = legend;
  legend_->SetPopup();
  //
  DrawLegendCheckBox->Checked = static_cast<GRA_boolCharacteristic*>(legChars_->Get("ON"))->Get();
  //
  DrawFrameCheckBox->Checked = static_cast<GRA_boolCharacteristic*>(legChars_->Get("FRAMEON"))->Get();
  //
  int r, g, b;
  GRA_color *color = static_cast<GRA_colorCharacteristic*>(legChars_->Get("FRAMECOLOR"))->Get();
  GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
  FrameColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  //
  double xlo = static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEXLO"))->GetAsPercent();
  double ylo = static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEYLO"))->GetAsPercent();
  double xhi = static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEXHI"))->GetAsPercent();
  double yhi = static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEYHI"))->GetAsPercent();
  //
  MoveHorizontalTrackBar->Position = static_cast<int>(xlo+0.5);
  MoveHLabel->Caption = AnsiString(xlo)+AnsiString("%");
  MoveVerticalTrackBar->Position = static_cast<int>(ylo+0.5);
  MoveVLabel->Caption = AnsiString(ylo)+AnsiString("%");
  ExpandHorizontalTrackBar->Position = static_cast<int>(xhi-xlo+0.5);
  ExpandHLabel->Caption = AnsiString(xhi-xlo)+AnsiString("%");
  ExpandVerticalTrackBar->Position = static_cast<int>(yhi-ylo+0.5);
  ExpandVLabel->Caption = AnsiString(yhi-ylo)+AnsiString("%");
  //
  DrawTitleCheckBox->Checked = static_cast<GRA_boolCharacteristic*>(legChars_->Get("TITLEON"))->Get();
  //
  TitleEdit->Text = AnsiString( static_cast<GRA_stringCharacteristic*>(legChars_->Get("TITLE"))->Get().c_str() );
  //
  color = static_cast<GRA_colorCharacteristic*>(legChars_->Get("TITLECOLOR"))->Get();
  GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
  TitleColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  //
  double titleHeight = static_cast<GRA_sizeCharacteristic*>(legChars_->Get("TITLEHEIGHT"))->GetAsPercent();
  int tmp = static_cast<int>(titleHeight*10 + 0.5);
  TitleHeightUpDown->Position = tmp;
  TitleHeightEdit->Text = AnsiString( tmp/10. );
  //
  // first create a vector containing the names of valid true type fonts
  std::vector<ExString> fontNames;
  TCanvas *tc = page_->GetImage()->Canvas;
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
      fontNames.push_back( ExString(tc->Font->Name.c_str()) );
  }
  tc->Font->Assign( tfontSave );
  delete tfontSave;
  //
  // now fill the combo boxes with the font names
  GRA_font *titleFont = static_cast<GRA_fontCharacteristic*>(legChars_->Get("TITLEFONT"))->Get();
  int nFonts = static_cast<int>(fontNames.size());
  for( int i=0; i<nFonts; ++i )
  {
    TitleFontComboBox->Items->Add( AnsiString(fontNames[i].c_str()) );
    if( titleFont == GRA_fontControl::GetFont(fontNames[i]) )TitleFontComboBox->ItemIndex = i;
  }
  //
  setup_ = false;
}

void TLegendPopupForm::ReDraw()
{
  if( setup_ || !legend_ )return;
  try
  {
    page_->SetGraphWindow( window_ );
    page_->ReplotCurrentWindow();
  }
  catch ( EGraphicsError const &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
  }
}

void __fastcall TLegendPopupForm::OnDrawLegendClick( TObject *Sender )
{
  if( setup_ || !legend_ )return;
  static_cast<GRA_boolCharacteristic*>(legChars_->Get("ON"))->Set( DrawLegendCheckBox->Checked );
  ReDraw();
}

void __fastcall TLegendPopupForm::OnDrawFrameClick( TObject *Sender )
{
  if( setup_ || !legend_ )return;
  static_cast<GRA_boolCharacteristic*>(legChars_->Get("FRAMEON"))->Set( DrawFrameCheckBox->Checked );
  ReDraw();
}

void __fastcall TLegendPopupForm::OnFrameColorMouseDown( TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y )
{
  if( setup_ || !legend_ )return;
  // set the colour of the frame
  //
  // this flag needs to be set false, to indicate if a name,
  // color, etc. have been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TBasicColorsForm> f( new TBasicColorsForm(this) );
  f->Top = Top + FrameColorShape->Top + 20;
  f->Left = Left + FrameColorShape->Left + FrameColorShape->Width + 10;
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )
  {
    GRA_color *color = ExGlobals::GetWorkingColor();
    int r, g, b;
    GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
    FrameColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
    static_cast<GRA_colorCharacteristic*>(legChars_->Get("FRAMECOLOR"))->Set( color );
    legend_->MakeFrame();
    if( DrawFrameCheckBox->Checked && DrawLegendCheckBox->Checked )ReDraw();
  }
}

void __fastcall TLegendPopupForm::OnDrawTitleClick( TObject *Sender )
{
  if( setup_ || !legend_ )return;
  static_cast<GRA_boolCharacteristic*>(legChars_->Get("TITLEON"))->Set( DrawTitleCheckBox->Checked );
  ReDraw();
}

void __fastcall TLegendPopupForm::OnTitleKeyPress( TObject *Sender, char &Key )
{
  if( setup_ || Key!=VK_RETURN )return;
  static_cast<GRA_stringCharacteristic*>(legChars_->Get("TITLE"))->Set( ExString(TitleEdit->Text.c_str()) );
  legend_->MakeTitle();
  if( DrawTitleCheckBox->Checked )ReDraw();
}

void __fastcall TLegendPopupForm::OnTitleColorMouseDown( TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y )
{
  if( setup_ || !legend_ )return;
  // set the colour of the title
  //
  // this flag needs to be set false, to indicate if a name,
  // color, etc. have been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TBasicColorsForm> f( new TBasicColorsForm(this) );
  f->Top = Top + TitleGroupBox->Top + TitleColorShape->Top + 20;
  f->Left = Left + TitleGroupBox->Left + TitleColorShape->Left + TitleColorShape->Width + 10;
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )
  {
    GRA_color *color = ExGlobals::GetWorkingColor();
    int r, g, b;
    GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
    TitleColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
    static_cast<GRA_colorCharacteristic*>(legChars_->Get("TITLECOLOR"))->Set( color );
    legend_->MakeTitle();
    if( DrawTitleCheckBox->Checked )ReDraw();
  }
}

void __fastcall TLegendPopupForm::OnTitleHeightChange( TObject *Sender )
{
  if( setup_ || !legend_ )return;
  AnsiString text = TitleHeightEdit->Text;
  if( !legChars_  || text.IsEmpty() )return;
  double y;
  try
  {
    y = text.ToDouble();
  }
  catch (EConvertError const &e)
  {
    TitleHeightEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for legend title height", "Error", MB_OK );
    return;
  }
  if( y < 0.0 || y > 100.0 )
  {
    TitleHeightEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for legend title height", "Error", MB_OK );
    return;
  }
  static_cast<GRA_sizeCharacteristic*>(legChars_->Get("TITLEHEIGHT"))->SetAsPercent( y );
  TitleHeightUpDown->Position = static_cast<int>(y*10 + 0.5);
  legend_->MakeTitle();
  if( DrawTitleCheckBox->Checked )ReDraw();
}

void __fastcall TLegendPopupForm::OnTitleHeightUpDownClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ || !legend_ )return;
  double y = TitleHeightUpDown->Position/10.;
  TitleHeightEdit->Text = AnsiString( y );
  static_cast<GRA_sizeCharacteristic*>(legChars_->Get("TITLEHEIGHT"))->SetAsPercent( y );
  legend_->MakeTitle();
  if( DrawTitleCheckBox->Checked )ReDraw();
}

void __fastcall TLegendPopupForm::OnTitleFontChange( TObject *Sender )
{
  if( setup_ || !legend_ )return;
  ExString fontName( TitleFontComboBox->Items->Strings[TitleFontComboBox->ItemIndex].c_str() );
  GRA_font *font = GRA_fontControl::GetFont( fontName );
  static_cast<GRA_fontCharacteristic*>(legChars_->Get("TITLEFONT"))->Set( font );
  legend_->MakeTitle();
  if( DrawTitleCheckBox->Checked )ReDraw();
}

void __fastcall TLegendPopupForm::OnMoveHorizontally( TObject *Sender )
{
  if( setup_ || !legend_ )return;
  double xlo = static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEXLO"))->GetAsPercent();
  double xhi = static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEXHI"))->GetAsPercent();
  double dx = xhi - xlo;
  double x = static_cast<double>(MoveHorizontalTrackBar->Position);
  static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEXLO"))->SetAsPercent( x );
  static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEXHI"))->SetAsPercent( x+dx );
  MoveHLabel->Caption = AnsiString(x)+AnsiString("%");
  legend_->Initialize();
  ReDraw();
}

void __fastcall TLegendPopupForm::OnMoveVertically( TObject *Sender )
{
  if( setup_ || !legend_ )return;
  double ylo = static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEYLO"))->GetAsPercent();
  double yhi = static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEYHI"))->GetAsPercent();
  double dy = yhi - ylo;
  double y = static_cast<double>(MoveVerticalTrackBar->Position);
  static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEYLO"))->SetAsPercent( y );
  static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEYHI"))->SetAsPercent( y+dy );
  MoveVLabel->Caption = AnsiString(y)+AnsiString("%");
  legend_->Initialize();
  ReDraw();
}

void __fastcall TLegendPopupForm::OnExpandHorizontally( TObject *Sender )
{
  if( setup_ || !legend_ )return;
  double xlo = static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEXLO"))->GetAsPercent();
  double dx = static_cast<double>(ExpandHorizontalTrackBar->Position);
  static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEXHI"))->SetAsPercent( xlo+dx );
  ExpandHLabel->Caption = AnsiString(dx)+AnsiString("%");
  legend_->Initialize();
  ReDraw();
}

void __fastcall TLegendPopupForm::OnExpandVertically( TObject *Sender )
{
  if( setup_ || !legend_ )return;
  double ylo = static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEYLO"))->GetAsPercent();
  double dy = static_cast<double>(ExpandVerticalTrackBar->Position);
  static_cast<GRA_distanceCharacteristic*>(legChars_->Get("FRAMEYHI"))->SetAsPercent( ylo+dy );
  ExpandVLabel->Caption = AnsiString(dy)+AnsiString("%");
  legend_->Initialize();
  ReDraw();
}

void __fastcall TLegendPopupForm::OnCloseClick( TObject *Sender )
{ Close(); }

void __fastcall TLegendPopupForm::OnCloseForm( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TLegendPopupForm::OnCloseQuery( TObject *Sender, bool &CanClose )
{ LegendPopupForm = 0; }

void TLegendPopupForm::Disconnect()
{ legend_ = 0;}

