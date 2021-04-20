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

#include "AxisPopup.h"
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

TAxisPopupForm *AxisPopupForm;

__fastcall TAxisPopupForm::TAxisPopupForm( TComponent* Owner ) : TForm(Owner)
{
  setup_ = true;
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "AxisPopupForm", "Top", 220 );
  Left = ini->ReadInteger( "AxisPopupForm", "Left", 850 );
  Width = ini->ReadInteger( "AxisPopupForm", "Width", 260 );
  Height = ini->ReadInteger( "AxisPopupForm", "Height", 375 );
  TopPageControl->ActivePageIndex =
   std::min(TopPageControl->PageCount-1,std::max(0,ini->ReadInteger("AxisPopupForm","Page",0)));
  delete ini;
  //
  xOffset_ = 30;
  yOffset_ = 40;
}

__fastcall TAxisPopupForm::~TAxisPopupForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "AxisPopupForm", "Top", Top );
  ini->WriteInteger( "AxisPopupForm", "Left", Left );
  ini->WriteInteger( "AxisPopupForm", "Width", Width );
  ini->WriteInteger( "AxisPopupForm", "Height", Height );
  ini->WriteInteger( "AxisPopupForm", "Page", TopPageControl->ActivePageIndex );
  delete ini;
}

void __fastcall TAxisPopupForm::OnCloseClick( TObject *Sender )
{ Close(); }

void __fastcall TAxisPopupForm::FormClose( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TAxisPopupForm::FormCloseQuery( TObject *Sender, bool &CanClose )
{ AxisPopupForm = 0; }

void TAxisPopupForm::Setup( GRA_page *page, GRA_window *window, char const which )
{
  setup_ = true;
  page_ = page;
  window_ = window;
  which_ = which;
  axisChars_ = which_=='X' ? window_->GetXAxisCharacteristics() : window_->GetYAxisCharacteristics();
  MinEdit->Text = AnsiString(static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("MIN"))->Get());
  MaxEdit->Text = AnsiString(static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("MAX"))->Get());
  double base = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("LOGBASE"))->Get();
  //
  // this will have problems if the log base is other than 2, e, or 10
  int idx = 0;
  if( fabs(base-2.0) < 0.01 )idx = 1;
  else if( fabs(base-exp(1.0)) < 0.01 )idx = 2;
  else if( fabs(base-10.0) < 0.01 )idx = 3;
  LogRadioGroup->ItemIndex = idx;
  //
  LogStyleCheckBox->Checked = static_cast<GRA_boolCharacteristic*>(axisChars_->Get("LOGSTYLE"))->Get();
  LogStyleCheckBox->Enabled = (base > 1.0);
  //
  AxisOnCheckBox->Checked = static_cast<GRA_boolCharacteristic*>(axisChars_->Get("AXISON"))->Get();
  DrawGraphBoxCheckBox->Checked = static_cast<GRA_boolCharacteristic*>(window_->GetGeneralCharacteristics()->Get("GRAPHBOX"))->Get();
  //
  GRA_color *color = static_cast<GRA_colorCharacteristic*>(axisChars_->Get("AXISCOLOR"))->Get();
  int r, g, b;
  GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
  AxisColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  color = static_cast<GRA_colorCharacteristic*>(axisChars_->Get("LABELCOLOR"))->Get();
  GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
  LabelColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
  //
  NumbersOnCheckBox->Checked = static_cast<GRA_boolCharacteristic*>(axisChars_->Get("NUMBERSON"))->Get();
  //
  color = static_cast<GRA_colorCharacteristic*>(axisChars_->Get("NUMBERSCOLOR"))->Get();
  GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
  NumbersColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
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
  GRA_font *numbersFont = static_cast<GRA_fontCharacteristic*>(axisChars_->Get("NUMBERSFONT"))->Get();
  GRA_font *labelFont = static_cast<GRA_fontCharacteristic*>(axisChars_->Get("LABELFONT"))->Get();
  int nFonts = static_cast<int>(fontNames.size());
  for( int i=0; i<nFonts; ++i )
  {
    NumbersFontComboBox->Items->Add( AnsiString(fontNames[i].c_str()) );
    if( numbersFont == GRA_fontControl::GetFont(fontNames[i]) )NumbersFontComboBox->ItemIndex = i;
    LabelFontComboBox->Items->Add( AnsiString(fontNames[i].c_str()) );
    if( labelFont == GRA_fontControl::GetFont(fontNames[i]) )LabelFontComboBox->ItemIndex = i;
  }
  double height = static_cast<GRA_sizeCharacteristic*>(axisChars_->Get("NUMBERSHEIGHT"))->GetAsPercent();
  int hInt = static_cast<int>(height*10 + 0.5);
  NumbersHeightUpDown->Position = hInt;
  NumbersHeightEdit->Text = AnsiString( hInt/10. );
  //
  double angle = static_cast<GRA_angleCharacteristic*>(axisChars_->Get("NUMBERSANGLE"))->Get();
  NumbersAngleUpDown->Position = static_cast<int>(angle);
  //
  int lw = static_cast<GRA_intCharacteristic*>(axisChars_->Get("LINEWIDTH"))->Get();
  AxisLinewidthEdit->Text = AnsiString( lw );
  //
  int nst = static_cast<GRA_intCharacteristic*>(axisChars_->Get("NSINCS"))->Get();
  NsincsEdit->Text = AnsiString( nst );
  //
  double length = static_cast<GRA_sizeCharacteristic*>(axisChars_->Get("LARGETICLENGTH"))->GetAsPercent();
  hInt = static_cast<int>(length*10 + 0.5);
  LTicLenUpDown->Position = hInt;
  LTicLenEdit->Text = AnsiString( hInt/10. );
  //
  length = static_cast<GRA_sizeCharacteristic*>(axisChars_->Get("SMALLTICLENGTH"))->GetAsPercent();
  hInt = static_cast<int>(length*10 + 0.5);
  STicLenUpDown->Position = hInt;
  STicLenEdit->Text = AnsiString( hInt/10. );
  //
  TicsOnCheckBox->Checked = static_cast<GRA_boolCharacteristic*>(axisChars_->Get("TICSON"))->Get();
  //
  double ticAngle = static_cast<GRA_angleCharacteristic*>(axisChars_->Get("TICANGLE"))->Get();
  bool ticsBothSides = static_cast<GRA_boolCharacteristic*>(axisChars_->Get("TICSBOTHSIDES"))->Get();
  if( ticsBothSides )TicRadioGroup->ItemIndex = 2;
  else
  {
    if( which_ == 'X' )TicRadioGroup->ItemIndex = (ticAngle>=0.0&&ticAngle<=180.0) ? 0 : 1;
    else               TicRadioGroup->ItemIndex = (ticAngle>=0.0&&ticAngle<=180.0) ? 1 : 0;
  }
  //
  LabelOnCheckBox->Checked = static_cast<GRA_boolCharacteristic*>(axisChars_->Get("LABELON"))->Get();
  //
  LabelEdit->Text = AnsiString(static_cast<GRA_stringCharacteristic*>(axisChars_->Get("LABEL"))->Get().c_str());
  //
  height = static_cast<GRA_sizeCharacteristic*>(axisChars_->Get("LABELHEIGHT"))->GetAsPercent();
  hInt = static_cast<int>(height*10 + 0.5);
  LabelHeightUpDown->Position = hInt;
  LabelHeightEdit->Text = AnsiString( hInt/10. );
  //
  double loc = static_cast<GRA_distanceCharacteristic*>(axisChars_->Get("LOWERAXIS"))->GetAsPercent();
  hInt = static_cast<int>(loc*10 + 0.5);
  LocationLowUpDown->Position = hInt;
  LocationLowEdit->Text = AnsiString( hInt/10. );
  //
  loc = static_cast<GRA_distanceCharacteristic*>(axisChars_->Get("UPPERAXIS"))->GetAsPercent();
  hInt = static_cast<int>(loc*10 + 0.5);
  LocationUpUpDown->Position = hInt;
  LocationUpEdit->Text = AnsiString( hInt/10. );
  //
  int grid = static_cast<GRA_intCharacteristic*>(axisChars_->Get("GRID"))->Get();
  GridNoneRB->Checked = (grid==0);
  GridMajorRB->Checked = (grid>0);
  GridAllRB->Checked = (grid<0);
  //
  int gridLineType = static_cast<GRA_intCharacteristic*>(window_->GetGeneralCharacteristics()->Get("GRIDLINETYPE"))->Get();
  GridLineTypeEdit->Text = AnsiString( gridLineType );
  //
  setup_ = false;
}

void __fastcall TAxisPopupForm::OnAxisColorMouseDown( TObject *Sender,
                     TMouseButton Button, TShiftState Shift, int X, int Y )
{
  if( setup_ )return;
  // set the colour of the axis
  //
  // this flag needs to be set false, to indicate if a name,
  // color, etc. have been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TBasicColorsForm> f( new TBasicColorsForm(this) );
  f->Top = Top + AxisTabSheet->Top + Y + yOffset_ + 50;
  f->Left = Left + AxisTabSheet->Left + X + xOffset_;
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )
  {
    GRA_color *color = ExGlobals::GetWorkingColor();
    int r, g, b;
    GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
    AxisColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
    static_cast<GRA_colorCharacteristic*>(axisChars_->Get("AXISCOLOR"))->Set( color );
    ReDraw();
  }
}

void __fastcall TAxisPopupForm::OnNumbersColorMouseDown(
      TObject *Sender, TMouseButton Button, TShiftState Shift, int X,
      int Y)
{
  if( setup_ )return;
  // set the colour of the axis numbering
  //
  // this flag needs to be set false, to indicate if a name,
  // color, etc. have been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TBasicColorsForm> f( new TBasicColorsForm(this) );
  f->Top = Top + NumbersTabSheet->Top + Y + yOffset_ + 50;
  f->Left = Left + NumbersTabSheet->Left + X + xOffset_;
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )
  {
    GRA_color *color = ExGlobals::GetWorkingColor();
    int r, g, b;
    GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
    NumbersColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
    static_cast<GRA_colorCharacteristic*>(axisChars_->Get("NUMBERSCOLOR"))->Set( color );
    ReDraw();
  }
}

void __fastcall TAxisPopupForm::OnLabelColorMouseDown( TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y )
{
  if( setup_ )return;
  // set the colour of the axis label
  //
  // this flag needs to be set false, to indicate if a name,
  // color, etc. have been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TBasicColorsForm> f( new TBasicColorsForm(this) );
  f->Top = Top + LabelTabSheet->Top + Y + yOffset_ + 90;
  f->Left = Left + LabelTabSheet->Left + X + xOffset_;
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )
  {
    GRA_color *color = ExGlobals::GetWorkingColor();
    int r, g, b;
    GRA_colorControl::ColorCodeToRGB( GRA_colorControl::GetColorCode(color), r, g, b );
    LabelColorShape->Brush->Color = StringToColor(AnsiString("0x00")+IntToHex(b,2)+IntToHex(g,2)+IntToHex(r,2));
    static_cast<GRA_colorCharacteristic*>(axisChars_->Get("LABELCOLOR"))->Set( color );
    ReDraw();
  }
}

void __fastcall TAxisPopupForm::OnApplyRescaling( TObject *Sender )
{
  if( setup_ )return;
  double min, max;
  try
  {
    min = MinEdit->Text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for min", "Error", MB_OK );
    return;
  }
  try
  {
    max = MaxEdit->Text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for max", "Error", MB_OK );
    return;
  }
  GRA_setOfCharacteristics *general = window_->GetGeneralCharacteristics();
  GRA_stringCharacteristic *autoscale = static_cast<GRA_stringCharacteristic*>(general->Get("AUTOSCALE"));
  GRA_doubleCharacteristic *axisMin = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("MIN"));
  GRA_doubleCharacteristic *axisMax = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("MAX"));
  GRA_doubleCharacteristic *axisVMin = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("VIRTUALMIN"));
  GRA_doubleCharacteristic *axisVMax = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("VIRTUALMAX"));
  GRA_intCharacteristic *axisNlincs = static_cast<GRA_intCharacteristic*>(axisChars_->Get("NLINCS"));
  autoscale->Set("OFF");
  axisMin->Set( min );
  axisMax->Set( max );
  axisVMin->Set( min );
  axisVMax->Set( max );
  axisNlincs->Set( 0 );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnSelectNumbersFont( TObject *Sender )
{
  if( setup_ )return;
  ExString fontName( NumbersFontComboBox->Items->Strings[NumbersFontComboBox->ItemIndex].c_str() );
  GRA_font *font = GRA_fontControl::GetFont( fontName );
  static_cast<GRA_fontCharacteristic*>(axisChars_->Get("NUMBERSFONT"))->Set( font );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnNumbersAngleUpDownClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ )return;
  double angle;
  try
  {
    angle = NumbersAngleEdit->Text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for numbers angle", "Error", MB_OK );
    return;
  }
  static_cast<GRA_angleCharacteristic*>(axisChars_->Get("NUMBERSANGLE"))->Set( angle );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnNumbersAngleChange( TObject *Sender )
{
  if( setup_ )return;
  AnsiString text = NumbersAngleEdit->Text;
  if( !axisChars_ || text.IsEmpty() )return;
  double angle;
  try
  {
    angle = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    NumbersAngleEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for numbers angle", "Error", MB_OK );
    return;
  }
  static_cast<GRA_angleCharacteristic*>(axisChars_->Get("NUMBERSANGLE"))->Set( angle );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnNumbersHeightUpDownClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ )return;
  double height = NumbersHeightUpDown->Position/10.;
  NumbersHeightEdit->Text = AnsiString( height );
  static_cast<GRA_sizeCharacteristic*>(axisChars_->Get("NUMBERSHEIGHT"))->SetAsPercent( height );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnNumbersHeightEditChange( TObject *Sender )
{
  if( setup_ )return;
  AnsiString text = NumbersHeightEdit->Text;
  if( !axisChars_  || text.IsEmpty() )return;
  double height;
  try
  {
    height = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    NumbersHeightEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for numbers height", "Error", MB_OK );
    return;
  }
  if( height < 0.0 || height > 100.0 )
  {
    NumbersHeightEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for numbers height", "Error", MB_OK );
    return;
  }
  static_cast<GRA_sizeCharacteristic*>(axisChars_->Get("NUMBERSHEIGHT"))->SetAsPercent( height );
  NumbersHeightUpDown->Position = static_cast<int>(height*10 + 0.5);
  ReDraw();
}

void __fastcall TAxisPopupForm::OnDisplayNumbersClick( TObject *Sender )
{
  if( setup_ )return;
  static_cast<GRA_boolCharacteristic*>(axisChars_->Get("NUMBERSON"))->Set( NumbersOnCheckBox->Checked );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnDrawAxisClick( TObject *Sender )
{
  if( setup_ )return;
  static_cast<GRA_boolCharacteristic*>(axisChars_->Get("AXISON"))->Set( AxisOnCheckBox->Checked );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnAxisLinewidthUpDownClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ )return;
  int lw;
  try
  {
    lw = AxisLinewidthEdit->Text.ToInt();
  }
  catch ( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for axis line width", "Error", MB_OK );
    return;
  }
  static_cast<GRA_intCharacteristic*>(axisChars_->Get("LINEWIDTH"))->Set( lw );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnAxisLinewidthChange( TObject *Sender )
{
  if( setup_ )return;
  AnsiString text = AxisLinewidthEdit->Text;
  if( !axisChars_ || text.IsEmpty() )return;
  int lw;
  try
  {
    lw = text.ToInt();
  }
  catch ( EConvertError const &e )
  {
    AxisLinewidthEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for axis line width", "Error", MB_OK );
    return;
  }
  if( lw < 0 )
  {
    AxisLinewidthEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for axis line width", "Error", MB_OK );
    return;
  }
  static_cast<GRA_intCharacteristic*>(axisChars_->Get("LINEWIDTH"))->Set( lw );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnNsincsChange( TObject *Sender )
{
  if( setup_ )return;
  AnsiString text = NsincsEdit->Text;
  if( !axisChars_ || text.IsEmpty() )return;
  int n;
  try
  {
    n = text.ToInt();
  }
  catch ( EConvertError const &e )
  {
    NsincsEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for number of small tic marks", "Error", MB_OK );
    return;
  }
  if( n < 0 )
  {
    NsincsEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for number of small tic marks", "Error", MB_OK );
    return;
  }
  static_cast<GRA_intCharacteristic*>(axisChars_->Get("NSINCS"))->Set( n );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnNsincsClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ )return;
  int n;
  try
  {
    n = NsincsEdit->Text.ToInt();
  }
  catch ( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for number of small tics", "Error", MB_OK );
    return;
  }
  static_cast<GRA_intCharacteristic*>(axisChars_->Get("NSINCS"))->Set( n );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnTicDirectionClick( TObject *Sender )
{
  if( setup_ || !axisChars_ )return;
  switch ( TicRadioGroup->ItemIndex )
  {
    case 0:
      if( which_ == 'X' )static_cast<GRA_angleCharacteristic*>(axisChars_->Get("TICANGLE"))->Set(90.0);
      else               static_cast<GRA_angleCharacteristic*>(axisChars_->Get("TICANGLE"))->Set(270.0);
      static_cast<GRA_boolCharacteristic*>(axisChars_->Get("TICSBOTHSIDES"))->Set( false );
      break;
    case 1:
      if( which_ == 'X' )static_cast<GRA_angleCharacteristic*>(axisChars_->Get("TICANGLE"))->Set(270.0);
      else               static_cast<GRA_angleCharacteristic*>(axisChars_->Get("TICANGLE"))->Set(90.0);
      static_cast<GRA_boolCharacteristic*>(axisChars_->Get("TICSBOTHSIDES"))->Set( false );
      break;
    case 2:
      static_cast<GRA_boolCharacteristic*>(axisChars_->Get("TICSBOTHSIDES"))->Set( true );
      break;
  }
  ReDraw();
}

void __fastcall TAxisPopupForm::OnLTicLenChange( TObject *Sender )
{
  if( setup_ )return;
  AnsiString text = LTicLenEdit->Text;
  if( !axisChars_  || text.IsEmpty() )return;
  double length;
  try
  {
    length = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    LTicLenEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for major tic length", "Error", MB_OK );
    return;
  }
  if( length < 0.0 || length > 100.0 )
  {
    LTicLenEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for major tic length", "Error", MB_OK );
    return;
  }
  static_cast<GRA_sizeCharacteristic*>(axisChars_->Get("LARGETICLENGTH"))->SetAsPercent( length );
  LTicLenUpDown->Position = static_cast<int>(length*10 + 0.5);
  ReDraw();
}

void __fastcall TAxisPopupForm::OnLTicLenClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ )return;
  double length = LTicLenUpDown->Position/10.;
  LTicLenEdit->Text = AnsiString( length );
  static_cast<GRA_sizeCharacteristic*>(axisChars_->Get("LARGETICLENGTH"))->SetAsPercent( length );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnSTicLenChange( TObject *Sender )
{
  if( setup_ )return;
  AnsiString text = STicLenEdit->Text;
  if( !axisChars_  || text.IsEmpty() )return;
  double length;
  try
  {
    length = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    STicLenEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for minor tic length", "Error", MB_OK );
    return;
  }
  if( length < 0.0 || length > 100.0 )
  {
    STicLenEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for minor tic length", "Error", MB_OK );
    return;
  }
  static_cast<GRA_sizeCharacteristic*>(axisChars_->Get("SMALLTICLENGTH"))->SetAsPercent( length );
  STicLenUpDown->Position = static_cast<int>(length*10 + 0.5);
  ReDraw();
}

void __fastcall TAxisPopupForm::OnSTicLenClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ )return;
  double length = STicLenUpDown->Position/10.;
  STicLenEdit->Text = AnsiString( length );
  static_cast<GRA_sizeCharacteristic*>(axisChars_->Get("SMALLTICLENGTH"))->SetAsPercent( length );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnDisplayTics( TObject *Sender )
{
  if( setup_ )return;
  static_cast<GRA_boolCharacteristic*>(axisChars_->Get("TICSON"))->Set( TicsOnCheckBox->Checked );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnDisplayLabelClick( TObject *Sender )
{
  if( setup_ )return;
  static_cast<GRA_boolCharacteristic*>(axisChars_->Get("LABELON"))->Set( LabelOnCheckBox->Checked );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnSelectLabelFont( TObject *Sender )
{
  if( setup_ )return;
  ExString fontName( LabelFontComboBox->Items->Strings[LabelFontComboBox->ItemIndex].c_str() );
  GRA_font *font = GRA_fontControl::GetFont( fontName );
  static_cast<GRA_fontCharacteristic*>(axisChars_->Get("LABELFONT"))->Set( font );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnLabelHeightEditChange( TObject *Sender )
{
  if( setup_ )return;
  AnsiString text = LabelHeightEdit->Text;
  if( !axisChars_  || text.IsEmpty() )return;
  double height;
  try
  {
    height = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    LabelHeightEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for label height", "Error", MB_OK );
    return;
  }
  if( height < 0.0 || height > 100.0 )
  {
    LabelHeightEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for label height", "Error", MB_OK );
    return;
  }
  static_cast<GRA_sizeCharacteristic*>(axisChars_->Get("LABELHEIGHT"))->SetAsPercent( height );
  LabelHeightUpDown->Position = static_cast<int>(height*10 + 0.5);
  ReDraw();
}

void __fastcall TAxisPopupForm::OnLabelHeightUpDownClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ )return;
  double height = LabelHeightUpDown->Position/10.;
  LabelHeightEdit->Text = AnsiString( height );
  static_cast<GRA_sizeCharacteristic*>(axisChars_->Get("LABELHEIGHT"))->SetAsPercent( height );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnLocationLowEditChange( TObject *Sender )
{
  if( setup_ )return;
  AnsiString text = LocationLowEdit->Text;
  if( !axisChars_  || text.IsEmpty() )return;
  double loc;
  try
  {
    loc = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    LocationLowEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for lower axis location", "Error", MB_OK );
    return;
  }
  if( loc < 0.0 || loc > 100.0 )
  {
    LocationLowEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for lower axis location", "Error", MB_OK );
    return;
  }
  static_cast<GRA_distanceCharacteristic*>(axisChars_->Get("LOWERAXIS"))->SetAsPercent( loc );
  LocationLowUpDown->Position = static_cast<int>(loc*10 + 0.5);
  ReDraw();
}

void __fastcall TAxisPopupForm::OnLocationLowUpDownClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ )return;
  double loc = LocationLowUpDown->Position/10.;
  LocationLowEdit->Text = AnsiString( loc );
  static_cast<GRA_distanceCharacteristic*>(axisChars_->Get("LOWERAXIS"))->SetAsPercent( loc );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnLocationUpEditChange( TObject *Sender )
{
  if( setup_ )return;
  AnsiString text = LocationUpEdit->Text;
  if( !axisChars_  || text.IsEmpty() )return;
  double loc;
  try
  {
    loc = text.ToDouble();
  }
  catch ( EConvertError const &e )
  {
    LocationUpEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for upper axis location", "Error", MB_OK );
    return;
  }
  if( loc < 0.0 || loc > 100.0 )
  {
    LocationUpEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for upper axis location", "Error", MB_OK );
    return;
  }
  static_cast<GRA_distanceCharacteristic*>(axisChars_->Get("UPPERAXIS"))->SetAsPercent( loc );
  LocationUpUpDown->Position = static_cast<int>(loc*10 + 0.5);
  ReDraw();
}

void __fastcall TAxisPopupForm::OnLocationUpUpDownClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ )return;
  double loc = LocationUpUpDown->Position/10.;
  LocationUpEdit->Text = AnsiString( loc );
  static_cast<GRA_distanceCharacteristic*>(axisChars_->Get("UPPERAXIS"))->SetAsPercent( loc );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnAxisLabelEditExit( TObject *Sender )
{
  if( setup_ )return;
  static_cast<GRA_stringCharacteristic*>(axisChars_->Get("LABEL"))->Set( ExString(LabelEdit->Text.c_str()) );
}

void __fastcall TAxisPopupForm::OnAxisLabelEditKeyPress( TObject *Sender, char &Key )
{
  if( setup_ )return;
  if( Key == VK_RETURN )
   static_cast<GRA_stringCharacteristic*>(axisChars_->Get("LABEL"))->Set( ExString(LabelEdit->Text.c_str()) );
  if( !LabelOnCheckBox->Checked )return;
  ReDraw();
}

void __fastcall TAxisPopupForm::OnDrawGraphBoxClick( TObject *Sender )
{
  if( setup_ )return;
  static_cast<GRA_boolCharacteristic*>(window_->GetGeneralCharacteristics()->Get("GRAPHBOX"))->
    Set( DrawGraphBoxCheckBox->Checked );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnNoGridLinesClick( TObject *Sender )
{
  if( setup_ )return;
  GridNoneRB->Checked = true;
  GridMajorRB->Checked = false;
  GridAllRB->Checked = false;
  static_cast<GRA_intCharacteristic*>(axisChars_->Get("GRID"))->Set( 0 );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnMajorTicGridLinesClick( TObject *Sender )
{
  if( setup_ )return;
  GridNoneRB->Checked = false;
  GridMajorRB->Checked = true;
  GridAllRB->Checked = false;
  static_cast<GRA_intCharacteristic*>(axisChars_->Get("GRID"))->Set( 1 );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnAllTicsGridLinesClick( TObject *Sender )
{
  if( setup_ )return;
  GridNoneRB->Checked = false;
  GridMajorRB->Checked = false;
  GridAllRB->Checked = true;
  static_cast<GRA_intCharacteristic*>(axisChars_->Get("GRID"))->Set( -1 );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnGridLineTypeEditChange( TObject *Sender )
{
  if( setup_ )return;
  AnsiString text = GridLineTypeEdit->Text;
  if( !axisChars_ || text.IsEmpty() )return;
  int n;
  try
  {
    n = text.ToInt();
  }
  catch ( EConvertError const &e )
  {
    GridLineTypeEdit->Text = text.SubString( 1, text.Length()-1 );
    Application->MessageBox( "invalid value entered for grid line type", "Error", MB_OK );
    return;
  }
  if( n <= 0 || n > 10 )
  {
    GridLineTypeEdit->Text = AnsiString();
    Application->MessageBox( "invalid value entered for grid line type", "Error", MB_OK );
    return;
  }
  static_cast<GRA_intCharacteristic*>(window_->GetGeneralCharacteristics()->Get("GRIDLINETYPE"))->Set( n );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnGridLineTypeUpDownClick( TObject *Sender, TUDBtnType Button )
{
  if( setup_ )return;
  int n;
  try
  {
    n = GridLineTypeEdit->Text.ToInt();
  }
  catch ( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for grid line type", "Error", MB_OK );
    return;
  }
  static_cast<GRA_intCharacteristic*>(window_->GetGeneralCharacteristics()->Get("GRIDLINETYPE"))->Set( n );
  ReDraw();
}

void __fastcall TAxisPopupForm::OnLogClick( TObject *Sender )
{
  if( setup_ )return;
  double currentBase = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("LOGBASE"))->Get();
  double currentMin = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("MIN"))->Get();
  double currentMax = static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("MAX"))->Get();
  double axisMin = currentMin;
  double axisMax = currentMax;
  if( currentBase > 1.)
  {
    axisMin = pow(currentBase,currentMin);
    axisMax = pow(currentBase,currentMax);
  }
  if( (LogRadioGroup->ItemIndex>0) && (axisMin<0.0 || axisMax<0.0) )
  {
    Application->MessageBox( "logarithmic axis not possible with negative values", "Error", MB_OK );
    LogRadioGroup->ItemIndex = 0;
    return;
  }
  if( LogRadioGroup->ItemIndex == 0 ) // linear axis
  {
    static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("LOGBASE"))->Set( 1.0 );
    MinEdit->Text = AnsiString( axisMin );
    MaxEdit->Text = AnsiString( axisMax );
    LogStyleCheckBox->Enabled = false;
  }
  else if( LogRadioGroup->ItemIndex == 1 )
  {
    static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("LOGBASE"))->Set( 2.0 );
    MinEdit->Text = axisMin==0.0 ? AnsiString(-1) : AnsiString( log(axisMin)/log(2.0) );
    MaxEdit->Text = AnsiString( log(axisMax)/log(2.0) );
    LogStyleCheckBox->Enabled = true;
  }
  else if( LogRadioGroup->ItemIndex == 2 )
  {
    static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("LOGBASE"))->Set( exp(1.0) );
    MinEdit->Text = axisMin==0.0 ? AnsiString(-1) : AnsiString( log(axisMin) );
    MaxEdit->Text = AnsiString( log(axisMax) );
    LogStyleCheckBox->Enabled = true;
  }
  else if( LogRadioGroup->ItemIndex == 3 )
  {
    static_cast<GRA_doubleCharacteristic*>(axisChars_->Get("LOGBASE"))->Set( 10.0 );
    MinEdit->Text = axisMin==0.0 ? AnsiString(-1) : AnsiString( log(axisMin)/log(10.0) );
    MaxEdit->Text = AnsiString( log(axisMax)/log(10.0) );
    LogStyleCheckBox->Enabled = true;
  }
}

void __fastcall TAxisPopupForm::OnLogStyleClick( TObject *Sender )
{
  if( setup_ )return;
  static_cast<GRA_boolCharacteristic*>(axisChars_->Get("LOGSTYLE"))->Set( LogStyleCheckBox->Checked );
  ReDraw();
}

void TAxisPopupForm::ReDraw()
{
  if( setup_ )return;
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

