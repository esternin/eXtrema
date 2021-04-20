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

#include "CurvePopup.h"
#include "BasicColors.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_window.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_page.h"
#include "EGraphicsError.h"
#include "GRA_cartesianCurve.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TCurvePopupForm *CurvePopupForm;

__fastcall TCurvePopupForm::TCurvePopupForm( TComponent* Owner )
  : TForm(Owner)
{
  setup_ = true;
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "CurvePopupForm", "Top", 220 );
  Left = ini->ReadInteger( "CurvePopupForm", "Left", 850 );
  Width = ini->ReadInteger( "CurvePopupForm", "Width", 245 );
  Height = ini->ReadInteger( "CurvePopupForm", "Height", 390 );
  delete ini;
}

__fastcall TCurvePopupForm::~TCurvePopupForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "CurvePopupForm", "Top", Top );
  ini->WriteInteger( "CurvePopupForm", "Left", Left );
  ini->WriteInteger( "CurvePopupForm", "Width", Width );
  ini->WriteInteger( "CurvePopupForm", "Height", Height );
  delete ini;
  if( curve_ )curve_->Disconnect();
}

void __fastcall TCurvePopupForm::OnCloseClick( TObject *Sender )
{ Close(); }

void __fastcall TCurvePopupForm::OnCloseForm( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TCurvePopupForm::OnCloseQueryForm( TObject *Sender, bool &CanClose )
{ CurvePopupForm = 0; }

void TCurvePopupForm::Disconnect()
{ curve_ = 0;}

void TCurvePopupForm::Setup( GRA_page *page, GRA_window *window, GRA_cartesianCurve *cc )
{
  setup_ = true;
  page_ = page;
  window_ = window;
  curve_ = cc;
  curve_->SetPopup();
  curveChars_ = window_->GetDataCurveCharacteristics();
  //
  // this will have problems if histType is other than 0, 1, or 2
  int histType = curve_->GetHistogramType();
  if( histType == 3 )histType = 1;
  if( histType == 4 )histType = 2;
  HistogramRG->ItemIndex = histType;
  if( HistogramRG->ItemIndex == 0 )
  {
    PSCodeLabel->Enabled = true;
    PSCodeEdit->Enabled = true;
    PSCodeUpDown->Enabled = true;
    PSConnectCheckBox->Enabled = true;
    PSColorLabel->Enabled = true;
    PSColorShape->Enabled = true;
    PSSizeLabel->Enabled = true;
    PSSizeEdit->Enabled = true;
    PSSizeUpDown->Enabled = true;
    PSAngleLabel->Enabled = true;
    PSAngleEdit->Enabled = true;
    PSAngleUpDown->Enabled = true;
  }
  else
  {
    PSCodeLabel->Enabled = false;
    PSCodeEdit->Enabled = false;
    PSCodeUpDown->Enabled = false;
    PSConnectCheckBox->Enabled = false;
    PSColorLabel->Enabled = false;
    PSColorShape->Enabled = false;
    PSSizeLabel->Enabled = false;
    PSSizeEdit->Enabled = false;
    PSSizeUpDown->Enabled = false;
    PSAngleLabel->Enabled = false;
    PSAngleEdit->Enabled = false;
    PSAngleUpDown->Enabled = false;
  }
  //
  int r, g, b;
  GRA_color *color = curve_->GetColor();
  GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
  CurveColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  //
  color = curve_->GetAreaFillColor();
  int code = GRA_colorControl::GetColorCode(color);
  GRA_colorControl::ColorCodeToRGB( code, r, g, b );
  AreaFillColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  //
  int lw = curve_->GetLineWidth();
  CurveLinewidthEdit->Text = AnsiString( lw );
  //
  int curveLineType = curve_->GetLineType();
  CurveLineTypeEdit->Text = AnsiString( curveLineType );
  //
  int ps = curve_->GetPlotsymbolCode();
  PSCodeEdit->Text = AnsiString( abs(ps) );
  //
  PSConnectCheckBox->Checked = (ps>0);
  //
  color = curve_->GetPlotsymbolColor();
  GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
  PSColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  //
  double heightW = curve_->GetPlotsymbolSize();
  double height, dummy;
  window_->WorldToPercent( 0.0, heightW, dummy, height );
  int hInt = static_cast<int>(height*10 + 0.5);
  PSSizeUpDown->Position = hInt;
  PSSizeEdit->Text = AnsiString( hInt/10. );
  //
  double angle = curve_->GetPlotsymbolAngle();
  PSAngleUpDown->Position = static_cast<int>(angle);
  //
  setup_ = false;
}

void __fastcall TCurvePopupForm::OnCurveColorMouseDown( TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y )
{
  if( setup_ || !curve_ )return;
  // set the colour of the curve
  //
  // this flag needs to be set false, to indicate if a name,
  // color, etc. have been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TBasicColorsForm> f( new TBasicColorsForm(this) );
  f->Top = Top + CurveColorShape->Top + 20;
  f->Left = Left + CurveColorShape->Left + CurveColorShape->Width + 10;
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )
  {
    GRA_color *color = ExGlobals::GetWorkingColor();
    int r, g, b;
    GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
    CurveColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
    static_cast<GRA_colorCharacteristic*>(curveChars_->Get("CURVECOLOR"))->Set( color );
    curve_->SetColor( color );
    ReDraw();
  }
}

void __fastcall TCurvePopupForm::OnAreaFillColorMouseDown( TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y )
{
  if( setup_ || !curve_ )return;
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TBasicColorsForm> f( new TBasicColorsForm(this) );
  f->Top = Top + AreaFillColorShape->Top + 20;
  f->Left = Left + AreaFillColorShape->Left + AreaFillColorShape->Width + 10;
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )
  {
    GRA_color *color = ExGlobals::GetWorkingColor();
    int r, g, b;
    GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
    AreaFillColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
    if( GRA_colorControl::GetColorCode(color) == 0 )color = 0;
    static_cast<GRA_colorCharacteristic*>(window_->GetGeneralCharacteristics()->Get("AREAFILLCOLOR"))->Set( color );
    curve_->SetAreaFillColor( color );
    ReDraw();
  }
}

void __fastcall TCurvePopupForm::OnCurveLinewidthChange( TObject *Sender )
{
  if( setup_ || !curve_ )return;
  AnsiString text = CurveLinewidthEdit->Text;
  if( !curveChars_ || text.IsEmpty() )return;
  int lw;
  try
  {
    lw = text.ToInt();
  }
  catch ( EConvertError const &e )
  {
    CurveLinewidthEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for curve line width", "Error", MB_OK );
    return;
  }
  if( lw < 0 )
  {
    CurveLinewidthEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for curve line width", "Error", MB_OK );
    return;
  }
  static_cast<GRA_intCharacteristic*>(curveChars_->Get("CURVELINEWIDTH"))->Set( lw );
  curve_->SetLineWidth( lw );
  ReDraw();
}

void __fastcall TCurvePopupForm::OnCurveLinewidthClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ || !curve_ )return;
  int lw;
  try
  {
    lw = CurveLinewidthEdit->Text.ToInt();
  }
  catch ( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for curve line width", "Error", MB_OK );
    return;
  }
  static_cast<GRA_intCharacteristic*>(curveChars_->Get("CURVELINEWIDTH"))->Set( lw );
  curve_->SetLineWidth( lw );
  ReDraw();
}

void __fastcall TCurvePopupForm::OnCurveLinetypeClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ || !curve_ )return;
  int n;
  try
  {
    n = CurveLineTypeEdit->Text.ToInt();
  }
  catch ( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for curve line type", "Error", MB_OK );
    return;
  }
  static_cast<GRA_intCharacteristic*>(curveChars_->Get("CURVELINETYPE"))->Set( n );
  curve_->SetLineType( n );
  ReDraw();
}

void __fastcall TCurvePopupForm::OnCurveLineTypeChange( TObject *Sender )
{
  if( setup_ || !curve_ )return;
  AnsiString text = CurveLineTypeEdit->Text;
  if( !curveChars_ || text.IsEmpty() )return;
  int n;
  try
  {
    n = text.ToInt();
  }
  catch ( EConvertError const &e )
  {
    CurveLineTypeEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for curve line type", "Error", MB_OK );
    return;
  }
  if( n <= 0 || n > 10 )
  {
    CurveLineTypeEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for curve line type", "Error", MB_OK );
    return;
  }
  static_cast<GRA_intCharacteristic*>(curveChars_->Get("CURVELINETYPE"))->Set( n );
  curve_->SetLineType( n );
  ReDraw();
}

void __fastcall TCurvePopupForm::OnCurveTypeClick( TObject *Sender )
{
  if( setup_ || !curve_ )return;
  int histType = HistogramRG->ItemIndex;
  static_cast<GRA_intCharacteristic*>(curveChars_->Get("HISTOGRAMTYPE"))->Set( histType );
  curve_->SetHistogramType( histType );
  if( HistogramRG->ItemIndex == 0 )
  {
    CurveLineTypeLabel->Enabled = true;
    CurveLineTypeEdit->Enabled = true;
    CurveLineTypeUpDown->Enabled = true;
    PSCodeLabel->Enabled = true;
    PSCodeEdit->Enabled = true;
    PSCodeUpDown->Enabled = true;
    PSConnectCheckBox->Enabled = true;
    PSColorLabel->Enabled = true;
    PSColorShape->Enabled = true;
    PSSizeLabel->Enabled = true;
    PSSizeEdit->Enabled = true;
    PSSizeUpDown->Enabled = true;
    PSAngleLabel->Enabled = true;
    PSAngleEdit->Enabled = true;
    PSAngleUpDown->Enabled = true;
  }
  else
  {
    CurveLineTypeLabel->Enabled = false;
    CurveLineTypeEdit->Enabled = false;
    CurveLineTypeUpDown->Enabled = false;
    PSCodeLabel->Enabled = false;
    PSCodeEdit->Enabled = false;
    PSCodeUpDown->Enabled = false;
    PSConnectCheckBox->Enabled = false;
    PSColorLabel->Enabled = false;
    PSColorShape->Enabled = false;
    PSSizeLabel->Enabled = false;
    PSSizeEdit->Enabled = false;
    PSSizeUpDown->Enabled = false;
    PSAngleLabel->Enabled = false;
    PSAngleEdit->Enabled = false;
    PSAngleUpDown->Enabled = false;
  }
  ReDraw();
}

void __fastcall TCurvePopupForm::OnPSCodeChange( TObject *Sender )
{
  if( setup_ || !curve_ )return;
  AnsiString text = PSCodeEdit->Text;
  if( !curveChars_ || text.IsEmpty() )return;
  int n;
  try
  {
    n = text.ToInt();
  }
  catch ( EConvertError const &e )
  {
    PSCodeEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for plotsymbol code", "Error", MB_OK );
    return;
  }
  if( n < 0 || n > 18 )
  {
    PSCodeEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for plotsymbol code", "Error", MB_OK );
    return;
  }
  if( !PSConnectCheckBox->Checked )n *= -1;
  static_cast<GRA_intCharacteristic*>(curveChars_->Get("PLOTSYMBOL"))->Set( n );
  curve_->SetPlotsymbolCode( n );
  ReDraw();
}

void __fastcall TCurvePopupForm::OnPSCodeClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ || !curve_ )return;
  int n;
  try
  {
    n = PSCodeEdit->Text.ToInt();
  }
  catch ( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for plotsymbol code", "Error", MB_OK );
    return;
  }
  if( !PSConnectCheckBox->Checked )n *= -1;
  static_cast<GRA_intCharacteristic*>(curveChars_->Get("PLOTSYMBOL"))->Set( n );
  curve_->SetPlotsymbolCode( n );
  ReDraw();
}

void __fastcall TCurvePopupForm::OnPSColorMouseDown( TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y )
{
  if( setup_ || !curve_ )return;
  // set the colour of the plotsymbols
  //
  // this flag needs to be set false, to indicate if a name,
  // color, etc. have been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TBasicColorsForm> f( new TBasicColorsForm(this) );
  f->Top = Top + PlotsymbolGroupBox->Top + PSColorShape->Top + 20;
  f->Left = Left + PlotsymbolGroupBox->Left + PSColorShape->Left + PSColorShape->Width + 10;
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )
  {
    GRA_color *color = ExGlobals::GetWorkingColor();
    int r, g, b;
    GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
    PSColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
    static_cast<GRA_colorCharacteristic*>(curveChars_->Get("PLOTSYMBOLCOLOR"))->Set( color );
    curve_->SetPlotsymbolColor( color );
    ReDraw();
  }
}

void __fastcall TCurvePopupForm::OnPSSizeChange(TObject *Sender)
{
  if( setup_ || !curve_ )return;
  AnsiString text = PSSizeEdit->Text;
  if( !curveChars_  || text.IsEmpty() )return;
  double size;
  try
  {
    size = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    PSSizeEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for plotsymbol size", "Error", MB_OK );
    return;
  }
  if( size < 0.0 || size > 100.0 )
  {
    PSSizeEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for plotsymbol size", "Error", MB_OK );
    return;
  }
  static_cast<GRA_sizeCharacteristic*>(curveChars_->Get("PLOTSYMBOLSIZE"))->SetAsPercent( size );
  PSSizeUpDown->Position = static_cast<int>(size*10 + 0.5);
  curve_->SetPlotsymbolSize(
   static_cast<GRA_sizeCharacteristic*>(curveChars_->Get("PLOTSYMBOLSIZE"))->GetAsWorld() );
  ReDraw();
}


void __fastcall TCurvePopupForm::OnPSSizeClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ || !curve_ )return;
  double size = PSSizeUpDown->Position/10.;
  PSSizeEdit->Text = AnsiString( size );
  static_cast<GRA_sizeCharacteristic*>(curveChars_->Get("PLOTSYMBOLSIZE"))->SetAsPercent( size );
  curve_->SetPlotsymbolSize(
   static_cast<GRA_sizeCharacteristic*>(curveChars_->Get("PLOTSYMBOLSIZE"))->GetAsWorld() );
  ReDraw();
}

void __fastcall TCurvePopupForm::OnPSAngleChange( TObject *Sender )
{
  if( setup_ || !curve_ )return;
  AnsiString text = PSAngleEdit->Text;
  if( !curveChars_ || text.IsEmpty() )return;
  double angle;
  try
  {
    angle = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    PSAngleEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for plotsymbol angle", "Error", MB_OK );
    return;
  }
  static_cast<GRA_angleCharacteristic*>(curveChars_->Get("PLOTSYMBOLANGLE"))->Set( angle );
  curve_->SetPlotsymbolAngle( angle );
  ReDraw();
}

void __fastcall TCurvePopupForm::OnPSAngleClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ || !curve_ )return;
  double angle;
  try
  {
    angle = PSAngleEdit->Text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for plotsymbol angle", "Error", MB_OK );
    return;
  }
  static_cast<GRA_angleCharacteristic*>(curveChars_->Get("PLOTSYMBOLANGLE"))->Set( angle );
  curve_->SetPlotsymbolAngle( angle );
  ReDraw();
}

void TCurvePopupForm::ReDraw()
{
  if( setup_ || !curve_ )return;
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

