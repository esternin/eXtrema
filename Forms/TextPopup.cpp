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

#include "TextPopup.h"
#include "GRA_drawableText.h"
#include "BasicColors.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_window.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_font.h"
#include "GRA_fontControl.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_page.h"
#include "EGraphicsError.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TTextPopupForm *TextPopupForm;

__fastcall TTextPopupForm::TTextPopupForm( TComponent* Owner )
  : TForm(Owner)
{
  setup_ = true;
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "TextPopupForm", "Top", 220 );
  Left = ini->ReadInteger( "TextPopupForm", "Left", 850 );
  Width = ini->ReadInteger( "TextPopupForm", "Width", 255 );
  Height = ini->ReadInteger( "TextPopupForm", "Height", 275 );
  delete ini;
}

__fastcall TTextPopupForm::~TTextPopupForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "TextPopupForm", "Top", Top );
  ini->WriteInteger( "TextPopupForm", "Left", Left );
  ini->WriteInteger( "TextPopupForm", "Width", Width );
  ini->WriteInteger( "TextPopupForm", "Height", Height );
  delete ini;
  if( drawableText_ )drawableText_->Disconnect();
}

void TTextPopupForm::Setup( GRA_page *page, GRA_window *window, GRA_drawableText *dt )
{
  setup_ = true;
  page_ = page;
  window_ = window;
  textChars_ = window_->GetTextCharacteristics();
  drawableText_ = dt;
  drawableText_->SetPopup();
  //
  StringEdit->Text = AnsiString( drawableText_->GetString().c_str() );
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
  GRA_font *textFont = drawableText_->GetFont();
  int nFonts = static_cast<int>(fontNames.size());
  for( int i=0; i<nFonts; ++i )
  {
    FontComboBox->Items->Add( AnsiString(fontNames[i].c_str()) );
    if( textFont == GRA_fontControl::GetFont(fontNames[i]) )FontComboBox->ItemIndex = i;
  }
  //
  GRA_color *color = drawableText_->GetColor();
  int r, g, b;
  GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
  ColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  //
  double heightW = drawableText_->GetHeight();
  double height, dummy;
  window_->WorldToPercent( 0.0, heightW, dummy, height );
  int hInt = static_cast<int>(height*10 + 0.5);
  HeightUpDown->Position = hInt;
  HeightEdit->Text = AnsiString( hInt/10. );
  //
  double angle = drawableText_->GetAngle();
  AngleUpDown->Position = static_cast<int>(angle);
  //
  double xlocW = drawableText_->GetX();
  double ylocW = drawableText_->GetY();
  double xloc, yloc;
  window_->WorldToPercent( xlocW, ylocW, xloc, yloc );
  hInt = static_cast<int>(xloc*10+0.5);
  XLocUpDown->Position = hInt;
  XLocEdit->Text = AnsiString( hInt/10. );
  hInt = static_cast<int>(yloc*10+0.5);
  YLocUpDown->Position = hInt;
  YLocEdit->Text = AnsiString( hInt/10. );
  //
  setup_ = false;
}

void __fastcall TTextPopupForm::OnStringEditKeyPress( TObject *Sender, char &Key )
{
  if( setup_ || !drawableText_ )return;
  if( Key == VK_RETURN )
  {
    drawableText_->SetString( ExString(StringEdit->Text.c_str()) );
  }
  ReDraw();
}

void TTextPopupForm::ReDraw()
{
  if( setup_ || !drawableText_ )return;
  try
  {
    drawableText_->Parse();
    page_->SetGraphWindow( window_ );
    page_->ReplotCurrentWindow();
  }
  catch ( EGraphicsError const &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
  }
}

void __fastcall TTextPopupForm::OnColorMouseDown( TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y )
{
  if( setup_ || !drawableText_ )return;
  // set the colour of the text string
  //
  // this flag needs to be set false, to indicate if a name,
  // color, etc. have been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TBasicColorsForm> f( new TBasicColorsForm(this) );
  f->Top = Top + ColorShape->Top + 20;
  f->Left = Left + ColorShape->Left + ColorShape->Width + 10;
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )
  {
    GRA_color *color = ExGlobals::GetWorkingColor();
    int r, g, b;
    GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
    ColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
    static_cast<GRA_colorCharacteristic*>(textChars_->Get("COLOR"))->Set( color );
    drawableText_->SetColor( color );
    ReDraw();
  }
}

void __fastcall TTextPopupForm::OnHeightChange( TObject *Sender )
{
  if( setup_ || !drawableText_ )return;
  AnsiString text = HeightEdit->Text;
  if( !textChars_  || text.IsEmpty() )return;
  double height;
  try
  {
    height = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    HeightEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for text height", "Error", MB_OK );
    return;
  }
  if( height < 0.0 || height > 100.0 )
  {
    HeightEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for text height", "Error", MB_OK );
    return;
  }
  static_cast<GRA_sizeCharacteristic*>(textChars_->Get("HEIGHT"))->SetAsPercent( height );
  HeightUpDown->Position = static_cast<int>(height*10 + 0.5);
  drawableText_->SetHeight(
   static_cast<GRA_sizeCharacteristic*>(textChars_->Get("HEIGHT"))->GetAsWorld() );
  ReDraw();
}

void __fastcall TTextPopupForm::OnHeightClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ || !drawableText_ )return;
  double height = HeightUpDown->Position/10.;
  HeightEdit->Text = AnsiString( height );
  static_cast<GRA_sizeCharacteristic*>(textChars_->Get("HEIGHT"))->SetAsPercent( height );
  drawableText_->SetHeight(
   static_cast<GRA_sizeCharacteristic*>(textChars_->Get("HEIGHT"))->GetAsWorld() );
  ReDraw();
}

void __fastcall TTextPopupForm::OnAngleChange(TObject *Sender)
{
  if( setup_ || !drawableText_ )return;
  AnsiString text = AngleEdit->Text;
  if( !textChars_ || text.IsEmpty() )return;
  double angle;
  try
  {
    angle = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    AngleEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for text angle", "Error", MB_OK );
    return;
  }
  static_cast<GRA_angleCharacteristic*>(textChars_->Get("ANGLE"))->Set( angle );
  drawableText_->SetAngle( angle );
  ReDraw();
}

void __fastcall TTextPopupForm::OnAngleClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ || !drawableText_ )return;
  double angle;
  try
  {
    angle = AngleEdit->Text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for text angle", "Error", MB_OK );
    return;
  }
  static_cast<GRA_angleCharacteristic*>(textChars_->Get("ANGLE"))->Set( angle );
  drawableText_->SetAngle( angle );
  ReDraw();
}

void __fastcall TTextPopupForm::OnFontChange( TObject *Sender )
{
  if( setup_ || !drawableText_ )return;
  ExString fontName( FontComboBox->Items->Strings[FontComboBox->ItemIndex].c_str() );
  GRA_font *font = GRA_fontControl::GetFont( fontName );
  static_cast<GRA_fontCharacteristic*>(textChars_->Get("FONT"))->Set( font );
  drawableText_->SetFont( font );
  ReDraw();
}

void __fastcall TTextPopupForm::OnXLocChange( TObject *Sender )
{
  if( setup_ || !drawableText_ )return;
  AnsiString text = XLocEdit->Text;
  if( !textChars_  || text.IsEmpty() )return;
  double loc;
  try
  {
    loc = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    XLocEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for text x-location", "Error", MB_OK );
    return;
  }
  if( loc < 0.0 || loc > 100.0 )
  {
    XLocEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for text x-location", "Error", MB_OK );
    return;
  }
  static_cast<GRA_distanceCharacteristic*>(textChars_->Get("XLOCATION"))->SetAsPercent( loc );
  XLocUpDown->Position = static_cast<int>(loc*10 + 0.5);
  drawableText_->SetX(
    static_cast<GRA_distanceCharacteristic*>(textChars_->Get("XLOCATION"))->GetAsWorld() );
  ReDraw();
}

void __fastcall TTextPopupForm::OnXLocClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ || !drawableText_ )return;
  double loc = XLocUpDown->Position/10.;
  XLocEdit->Text = AnsiString( loc );
  static_cast<GRA_distanceCharacteristic*>(textChars_->Get("XLOCATION"))->SetAsPercent( loc );
  drawableText_->SetX(
    static_cast<GRA_distanceCharacteristic*>(textChars_->Get("XLOCATION"))->GetAsWorld() );
  ReDraw();
}

void __fastcall TTextPopupForm::OnYLocChange( TObject *Sender )
{
  if( setup_ || !drawableText_ )return;
  AnsiString text = YLocEdit->Text;
  if( !textChars_  || text.IsEmpty() )return;
  double loc;
  try
  {
    loc = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    YLocEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for text y-location", "Error", MB_OK );
    return;
  }
  if( loc < 0.0 || loc > 100.0 )
  {
    YLocEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for text y-location", "Error", MB_OK );
    return;
  }
  static_cast<GRA_distanceCharacteristic*>(textChars_->Get("YLOCATION"))->SetAsPercent( loc );
  YLocUpDown->Position = static_cast<int>(loc*10 + 0.5);
  drawableText_->SetY(
    static_cast<GRA_distanceCharacteristic*>(textChars_->Get("YLOCATION"))->GetAsWorld() );
  ReDraw();
}

void __fastcall TTextPopupForm::OnYLocClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ || !drawableText_ )return;
  double loc = YLocUpDown->Position/10.;
  YLocEdit->Text = AnsiString( loc );
  static_cast<GRA_distanceCharacteristic*>(textChars_->Get("YLOCATION"))->SetAsPercent( loc );
  drawableText_->SetY(
    static_cast<GRA_distanceCharacteristic*>(textChars_->Get("YLOCATION"))->GetAsWorld() );
  ReDraw();
}

void __fastcall TTextPopupForm::OnCloseClick( TObject *Sender )
{ Close(); }

void __fastcall TTextPopupForm::OnCloseForm( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TTextPopupForm::OnCloseQueryForm( TObject *Sender, bool &CanClose )
{ TextPopupForm = 0; }

void TTextPopupForm::Disconnect()
{ drawableText_ = 0;}

