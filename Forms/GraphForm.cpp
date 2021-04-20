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
#include "GRA_window.h"
#include "GRA_cartesianAxes.h"
#include "GRA_cartesianCurve.h"
#include "EGraphicsError.h"
#include "EVariableError.h"
#include "NVariableTable.h"
#include "NumericVariable.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_font.h"
#include "GRA_borlandMonitor.h"

// forms
#include "SelectColor.h"
#include "SelectFont.h"
#include "MainGraphicsWindow.h"

#include "GraphForm.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TGraphCommandForm *GraphCommandForm;

__fastcall TGraphCommandForm::TGraphCommandForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "GraphCommandForm", "Top", 25 );
  this->Left = ini->ReadInteger( "GraphCommandForm", "Left", 600 );
  this->Height = ini->ReadInteger( "GraphCommandForm", "Height", 521 );
  this->Width = ini->ReadInteger( "GraphCommandForm", "Width", 573 );
  delete ini;
  //
  wasCommensurate_ = false;
  FillOutForm();
}

__fastcall TGraphCommandForm::~TGraphCommandForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "GraphCommandForm", "Top", this->Top );
  ini->WriteInteger( "GraphCommandForm", "Left", this->Left );
  ini->WriteInteger( "GraphCommandForm", "Height", this->Height );
  ini->WriteInteger( "GraphCommandForm", "Width", this->Width );
  delete ini;
}

void __fastcall TGraphCommandForm::FormClose(TObject *Sender, TCloseAction &Action)
{ Action = caFree; }

void __fastcall TGraphCommandForm::FormClose2(TObject *Sender, bool &CanClose)
{ GraphCommandForm = 0; }

void __fastcall TGraphCommandForm::FillOutForm()
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *dataCurveChars = gw->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *generalChars = gw->GetGeneralCharacteristics();
  //
  GRA_intCharacteristic *histogramType =
    static_cast<GRA_intCharacteristic*>(dataCurveChars->Get("HISTOGRAMTYPE"));
  GRA_stringCharacteristic *autoscale =
    static_cast<GRA_stringCharacteristic*>(generalChars->Get("AUTOSCALE"));
  //
  histogramType_ = histogramType->Get();
  switch( histogramType_ )
  {
    case 0:
      PutImage( HT0Image, HT0aImage );
      PutImage( HT1Image, HT1bImage );
      PutImage( HT2Image, HT2bImage );
      PutImage( HT3Image, HT3bImage );
      PutImage( HT4Image, HT4bImage );
      break;
    case 1:
      PutImage( HT0Image, HT0bImage );
      PutImage( HT1Image, HT1aImage );
      PutImage( HT2Image, HT2bImage );
      PutImage( HT3Image, HT3bImage );
      PutImage( HT4Image, HT4bImage );
      break;
    case 2:
      PutImage( HT0Image, HT0bImage );
      PutImage( HT1Image, HT1bImage );
      PutImage( HT2Image, HT2aImage );
      PutImage( HT3Image, HT3bImage );
      PutImage( HT4Image, HT4bImage );
      break;
    case 3:
      PutImage( HT0Image, HT0bImage );
      PutImage( HT1Image, HT1bImage );
      PutImage( HT2Image, HT2bImage );
      PutImage( HT3Image, HT3aImage );
      PutImage( HT4Image, HT4bImage );
      break;
    case 4:
      PutImage( HT0Image, HT0bImage );
      PutImage( HT1Image, HT1bImage );
      PutImage( HT2Image, HT2bImage );
      PutImage( HT3Image, HT3bImage );
      PutImage( HT4Image, HT4aImage );
      break;
  }
  //
  AxesOnlyCheckBox->Checked = false;
  OverlayCheckBox->Checked = false;
  YOnRightCheckBox->Checked = false;
  XOnTopCheckBox->Checked = false;
  SmoothCheckBox->Checked = false;
  //
  if( autoscale->Get() == "COMMENSURATE" )
  {
    AutoscaleRadioGroup->ItemIndex = 2;
    wasCommensurate_ = true;
    VirtualCheckBox->Checked = false;
    XVirtualCheckBox->Checked = false;
    YVirtualCheckBox->Checked = false;
    VirtualCheckBox->Enabled = false;
    XVirtualCheckBox->Enabled = false;
    YVirtualCheckBox->Enabled = false;
    //
    XVMinEdit->Enabled = false;
    XVMinLabel->Enabled = false;
    XVMaxEdit->Enabled = false;
    XVMaxLabel->Enabled = false;
    //
    NLXincEdit->Enabled = false;
    NLXincLabel->Enabled = false;
    NSXincEdit->Enabled = false;
    NSXincLabel->Enabled = false;
    //
    YVMinEdit->Enabled = false;
    YVMinLabel->Enabled = false;
    YVMaxEdit->Enabled = false;
    YVMaxLabel->Enabled = false;
    //
    NLYincEdit->Enabled = false;
    NLYincLabel->Enabled = false;
    NSYincEdit->Enabled = false;
    NSYincLabel->Enabled = false;
    //
    XNDigEdit->Enabled = false;
    XNDigLabel->Enabled = false;
    XNDecEdit->Enabled = false;
    XNDecLabel->Enabled = false;
    //
    YNDigEdit->Enabled = false;
    YNDigLabel->Enabled = false;
    YNDecEdit->Enabled = false;
    YNDecLabel->Enabled = false;
  }
  else if( autoscale->Get() == "OFF" )
  {
    AutoscaleRadioGroup->ItemIndex = 0;
    VirtualCheckBox->Checked = false;
    XVirtualCheckBox->Checked = false;
    YVirtualCheckBox->Checked = false;
    VirtualCheckBox->Enabled = false;
    XVirtualCheckBox->Enabled = false;
    YVirtualCheckBox->Enabled = false;
    //
    XVMinEdit->Enabled = true;
    XVMinLabel->Enabled = true;
    XVMaxEdit->Enabled = true;
    XVMaxLabel->Enabled = true;
    //
    NLXincEdit->Enabled = true;
    NLXincLabel->Enabled = true;
    NSXincEdit->Enabled = true;
    NSXincLabel->Enabled = true;
    //
    YVMinEdit->Enabled = true;
    YVMinLabel->Enabled = true;
    YVMaxEdit->Enabled = true;
    YVMaxLabel->Enabled = true;
    //
    NLYincEdit->Enabled = true;
    NLYincLabel->Enabled = true;
    NSYincEdit->Enabled = true;
    NSYincLabel->Enabled = true;
    //
    XNDigEdit->Enabled = true;
    XNDigLabel->Enabled = true;
    XNDecEdit->Enabled = true;
    XNDecLabel->Enabled = true;
    //
    YNDigEdit->Enabled = true;
    YNDigLabel->Enabled = true;
    YNDecEdit->Enabled = true;
    YNDecLabel->Enabled = true;
  }
  else if( autoscale->Get() == "VIRTUAL" )
  {
    AutoscaleRadioGroup->ItemIndex = 1;
    VirtualCheckBox->Checked = true;
    XVirtualCheckBox->Checked = false;
    YVirtualCheckBox->Checked = false;
    VirtualCheckBox->Enabled = true;
    XVirtualCheckBox->Enabled = false;
    YVirtualCheckBox->Enabled = false;
    //
    XVMinEdit->Enabled = false;
    XVMinLabel->Enabled = false;
    XVMaxEdit->Enabled = false;
    XVMaxLabel->Enabled = false;
    //
    NLXincEdit->Enabled = false;
    NLXincLabel->Enabled = false;
    NSXincEdit->Enabled = false;
    NSXincLabel->Enabled = false;
    //
    YVMinEdit->Enabled = false;
    YVMinLabel->Enabled = false;
    YVMaxEdit->Enabled = false;
    YVMaxLabel->Enabled = false;
    //
    NLYincEdit->Enabled = false;
    NLYincLabel->Enabled = false;
    NSYincEdit->Enabled = false;
    NSYincLabel->Enabled = false;
    //
    XNDigEdit->Enabled = true;
    XNDigLabel->Enabled = true;
    XNDecEdit->Enabled = true;
    XNDecLabel->Enabled = true;
    //
    YNDigEdit->Enabled = true;
    YNDigLabel->Enabled = true;
    YNDecEdit->Enabled = true;
    YNDecLabel->Enabled = true;
  }
  else if( autoscale->Get() == "ON" )
  {
    AutoscaleRadioGroup->ItemIndex = 1;
    VirtualCheckBox->Checked = false;
    XVirtualCheckBox->Checked = false;
    YVirtualCheckBox->Checked = false;
    VirtualCheckBox->Enabled = true;
    XVirtualCheckBox->Enabled = false;
    YVirtualCheckBox->Enabled = false;
    //
    XVMinEdit->Enabled = false;
    XVMinLabel->Enabled = false;
    XVMaxEdit->Enabled = false;
    XVMaxLabel->Enabled = false;
    //
    NLXincEdit->Enabled = false;
    NLXincLabel->Enabled = false;
    NSXincEdit->Enabled = false;
    NSXincLabel->Enabled = false;
    //
    YVMinEdit->Enabled = false;
    YVMinLabel->Enabled = false;
    YVMaxEdit->Enabled = false;
    YVMaxLabel->Enabled = false;
    //
    NLYincEdit->Enabled = false;
    NLYincLabel->Enabled = false;
    NSYincEdit->Enabled = false;
    NSYincLabel->Enabled = false;
    //
    XNDigEdit->Enabled = false;
    XNDigLabel->Enabled = false;
    XNDecEdit->Enabled = false;
    XNDecLabel->Enabled = false;
    //
    YNDigEdit->Enabled = false;
    YNDigLabel->Enabled = false;
    YNDecEdit->Enabled = false;
    YNDecLabel->Enabled = false;
  }
  else if( autoscale->Get() == "XVIRTUAL" )
  {
    AutoscaleRadioGroup->ItemIndex = 3;
    VirtualCheckBox->Checked = false;
    XVirtualCheckBox->Checked = true;
    YVirtualCheckBox->Checked = false;
    VirtualCheckBox->Enabled = false;
    XVirtualCheckBox->Enabled = true;
    YVirtualCheckBox->Enabled = false;
    //
    XVMinEdit->Enabled = false;
    XVMinLabel->Enabled = false;
    XVMaxEdit->Enabled = false;
    XVMaxLabel->Enabled = false;
    //
    NLXincEdit->Enabled = false;
    NLXincLabel->Enabled = false;
    NSXincEdit->Enabled = false;
    NSXincLabel->Enabled = false;
    //
    YVMinEdit->Enabled = true;
    YVMinLabel->Enabled = true;
    YVMaxEdit->Enabled = true;
    YVMaxLabel->Enabled = true;
    //
    NLYincEdit->Enabled = true;
    NLYincLabel->Enabled = true;
    NSYincEdit->Enabled = true;
    NSYincLabel->Enabled = true;
    //
    XNDigEdit->Enabled = false;
    XNDigLabel->Enabled = false;
    XNDecEdit->Enabled = false;
    XNDecLabel->Enabled = false;
    //
    YNDigEdit->Enabled = false;
    YNDigLabel->Enabled = false;
    YNDecEdit->Enabled = false;
    YNDecLabel->Enabled = false;
  }
  else if( autoscale->Get() == "X" )
  {
    AutoscaleRadioGroup->ItemIndex = 3;
    VirtualCheckBox->Checked = false;
    XVirtualCheckBox->Checked = false;
    YVirtualCheckBox->Checked = false;
    VirtualCheckBox->Enabled = false;
    XVirtualCheckBox->Enabled = true;
    YVirtualCheckBox->Enabled = false;
    //
    XVMinEdit->Enabled = false;
    XVMinLabel->Enabled = false;
    XVMaxEdit->Enabled = false;
    XVMaxLabel->Enabled = false;
    //
    NLXincEdit->Enabled = false;
    NLXincLabel->Enabled = false;
    NSXincEdit->Enabled = false;
    NSXincLabel->Enabled = false;
    //
    YVMinEdit->Enabled = true;
    YVMinLabel->Enabled = true;
    YVMaxEdit->Enabled = true;
    YVMaxLabel->Enabled = true;
    //
    NLYincEdit->Enabled = true;
    NLYincLabel->Enabled = true;
    NSYincEdit->Enabled = true;
    NSYincLabel->Enabled = true;
    //
    XNDigEdit->Enabled = false;
    XNDigLabel->Enabled = false;
    XNDecEdit->Enabled = false;
    XNDecLabel->Enabled = false;
    //
    YNDigEdit->Enabled = false;
    YNDigLabel->Enabled = false;
    YNDecEdit->Enabled = false;
    YNDecLabel->Enabled = false;
  }
  else if( autoscale->Get() == "YVIRTUAL" )
  {
    AutoscaleRadioGroup->ItemIndex = 4;
    VirtualCheckBox->Checked = false;
    XVirtualCheckBox->Checked = false;
    YVirtualCheckBox->Checked = true;
    VirtualCheckBox->Enabled = false;
    XVirtualCheckBox->Enabled = false;
    YVirtualCheckBox->Enabled = true;
    //
    XVMinEdit->Enabled = true;
    XVMinLabel->Enabled = true;
    XVMaxEdit->Enabled = true;
    XVMaxLabel->Enabled = true;
    //
    NLXincEdit->Enabled = true;
    NLXincLabel->Enabled = true;
    NSXincEdit->Enabled = true;
    NSXincLabel->Enabled = true;
    //
    YVMinEdit->Enabled = false;
    YVMinLabel->Enabled = false;
    YVMaxEdit->Enabled = false;
    YVMaxLabel->Enabled = false;
    //
    NLYincEdit->Enabled = false;
    NLYincLabel->Enabled = false;
    NSYincEdit->Enabled = false;
    NSYincLabel->Enabled = false;
    //
    XNDigEdit->Enabled = true;
    XNDigLabel->Enabled = true;
    XNDecEdit->Enabled = true;
    XNDecLabel->Enabled = true;
    //
    YNDigEdit->Enabled = true;
    YNDigLabel->Enabled = true;
    YNDecEdit->Enabled = true;
    YNDecLabel->Enabled = true;
  }
  else if( autoscale->Get() == "Y" )
  {
    AutoscaleRadioGroup->ItemIndex = 4;
    VirtualCheckBox->Checked = false;
    XVirtualCheckBox->Checked = false;
    YVirtualCheckBox->Checked = false;
    VirtualCheckBox->Enabled = false;
    XVirtualCheckBox->Enabled = false;
    YVirtualCheckBox->Enabled = true;
    //
    XVMinEdit->Enabled = true;
    XVMinLabel->Enabled = true;
    XVMaxEdit->Enabled = true;
    XVMaxLabel->Enabled = true;
    //
    NLXincEdit->Enabled = true;
    NLXincLabel->Enabled = true;
    NSXincEdit->Enabled = true;
    NSXincLabel->Enabled = true;
    //
    YVMinEdit->Enabled = false;
    YVMinLabel->Enabled = false;
    YVMaxEdit->Enabled = false;
    YVMaxLabel->Enabled = false;
    //
    NLYincEdit->Enabled = false;
    NLYincLabel->Enabled = false;
    NSYincEdit->Enabled = false;
    NSYincLabel->Enabled = false;
    //
    XNDigEdit->Enabled = true;
    XNDigLabel->Enabled = true;
    XNDecEdit->Enabled = true;
    XNDecLabel->Enabled = true;
    //
    YNDigEdit->Enabled = true;
    YNDigLabel->Enabled = true;
    YNDecEdit->Enabled = true;
    YNDecLabel->Enabled = true;
  }
  GRA_intCharacteristic *plotsymbol =
    static_cast<GRA_intCharacteristic*>(dataCurveChars->Get("PLOTSYMBOL"));
  int i = plotsymbol->IsVector() ? plotsymbol->Gets().front() : plotsymbol->Get();
  int const symArray[] = {0,1,3,4,5,6,7,9,10,12,14,16,17,18,2,8,11,13,15};
  PlotSymbolComboBox->ItemIndex = symArray[abs(i)];
  PlotSymbolConnectCheckBox->Checked = (i>=0);
  //
  GRA_sizeCharacteristic *pss =
    static_cast<GRA_sizeCharacteristic*>(dataCurveChars->Get("PLOTSYMBOLSIZE"));
  double x = pss->IsVector() ? pss->Gets(true).front() : pss->Get(true);
  PlotSymbolSizeEdit->Text = AnsiString(x);
  //
  GRA_angleCharacteristic *psa =
    static_cast<GRA_angleCharacteristic*>(dataCurveChars->Get("PLOTSYMBOLANGLE"));
  double a = psa->IsVector() ? psa->Gets().front() : psa->Get();
  PlotSymbolAngleEdit->Text = AnsiString(a);
  //
  GRA_setOfCharacteristics *xAxisChars = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisChars = gw->GetYAxisCharacteristics();
  GRA_doubleCharacteristic *xmin =
    static_cast<GRA_doubleCharacteristic*>(xAxisChars->Get("MIN"));
  GRA_doubleCharacteristic *xmax =
    static_cast<GRA_doubleCharacteristic*>(xAxisChars->Get("MAX"));
  GRA_doubleCharacteristic *ymin =
    static_cast<GRA_doubleCharacteristic*>(yAxisChars->Get("MIN"));
  GRA_doubleCharacteristic *ymax =
    static_cast<GRA_doubleCharacteristic*>(yAxisChars->Get("MAX"));
  GRA_doubleCharacteristic *xvmin =
    static_cast<GRA_doubleCharacteristic*>(xAxisChars->Get("VIRTUALMIN"));
  GRA_doubleCharacteristic *xvmax =
    static_cast<GRA_doubleCharacteristic*>(xAxisChars->Get("VIRTUALMAX"));
  GRA_doubleCharacteristic *yvmin =
    static_cast<GRA_doubleCharacteristic*>(yAxisChars->Get("VIRTUALMIN"));
  GRA_doubleCharacteristic *yvmax =
    static_cast<GRA_doubleCharacteristic*>(yAxisChars->Get("VIRTUALMAX"));
  GRA_doubleCharacteristic *xlogbase =
    static_cast<GRA_doubleCharacteristic*>(xAxisChars->Get("LOGBASE"));
  GRA_doubleCharacteristic *ylogbase =
    static_cast<GRA_doubleCharacteristic*>(yAxisChars->Get("LOGBASE"));
  GRA_boolCharacteristic *xlogstyle =
    static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("LOGSTYLE"));
  GRA_boolCharacteristic *ylogstyle =
    static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("LOGSTYLE"));
  GRA_distanceCharacteristic *xloweraxis =
    static_cast<GRA_distanceCharacteristic*>(xAxisChars->Get("LOWERAXIS"));
  GRA_distanceCharacteristic *yloweraxis =
    static_cast<GRA_distanceCharacteristic*>(yAxisChars->Get("LOWERAXIS"));
  GRA_distanceCharacteristic *xupperaxis =
    static_cast<GRA_distanceCharacteristic*>(xAxisChars->Get("UPPERAXIS"));
  GRA_distanceCharacteristic *yupperaxis =
    static_cast<GRA_distanceCharacteristic*>(yAxisChars->Get("UPPERAXIS"));
  GRA_boolCharacteristic *xaxison =
    static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("AXISON"));
  GRA_boolCharacteristic *yaxison =
    static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("AXISON"));
  GRA_angleCharacteristic *xaxisangle =
    static_cast<GRA_angleCharacteristic*>(xAxisChars->Get("AXISANGLE"));
  GRA_angleCharacteristic *yaxisangle =
    static_cast<GRA_angleCharacteristic*>(yAxisChars->Get("AXISANGLE"));
  GRA_boolCharacteristic *xforcecross =
    static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("FORCECROSS"));
  GRA_boolCharacteristic *yforcecross =
    static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("FORCECROSS"));
  GRA_boolCharacteristic *xticson =
    static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("TICSON"));
  GRA_boolCharacteristic *yticson =
    static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("TICSON"));

  XMinEdit->Text = AnsiString( xmin->Get() );
  XMaxEdit->Text = AnsiString( xmax->Get() );
  XVMinEdit->Text = AnsiString( xvmin->Get() );
  XVMaxEdit->Text = AnsiString( xvmax->Get() );
  XLogBaseEdit->Text = AnsiString( xlogbase->Get() );
  XLogStyleCheckBox->Checked = xlogstyle->Get();
  XLAxisEdit->Text = AnsiString( xloweraxis->GetAsPercent() );
  XUAxisEdit->Text = AnsiString( xupperaxis->GetAsPercent() );
  XAxisOnCheckBox->Checked = xaxison->Get();
  XAxisAngleEdit->Text = AnsiString( xaxisangle->Get() );
  XForceCrossCheckBox->Checked = xforcecross->Get();
  //
  XTicsCheckBox->Checked = xticson->Get();
  if( XTicsCheckBox->Checked )
  {
    XTicsBothSidesCheckBox->Enabled = true;
    XLTicLenLabel->Enabled = true;
    XLTicLenEdit->Enabled = true;
    XSTicLenLabel->Enabled = true;
    XSTicLenEdit->Enabled = true;
    XTicAngleLabel->Enabled = true;
    XTicAngleEdit->Enabled = true;
    if( autoscale->Get()=="OFF" || autoscale->Get()=="Y" || autoscale->Get()=="YVIRTUAL" )
    {
      NLXincLabel->Enabled = true;
      NLXincEdit->Enabled = true;
      NSXincLabel->Enabled = true;
      NSXincEdit->Enabled = true;
    }
  }
  else
  {
    XTicsBothSidesCheckBox->Enabled = false;
    XLTicLenLabel->Enabled = false;
    XLTicLenEdit->Enabled = false;
    XSTicLenLabel->Enabled = false;
    XSTicLenEdit->Enabled = false;
    XTicAngleLabel->Enabled = false;
    XTicAngleEdit->Enabled = false;
    NLXincLabel->Enabled = false;
    NLXincEdit->Enabled = false;
    NSXincLabel->Enabled = false;
    NSXincEdit->Enabled = false;
  }
  GRA_boolCharacteristic *xticsbothsides =
    static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("TICSBOTHSIDES"));
  GRA_boolCharacteristic *yticsbothsides =
    static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("TICSBOTHSIDES"));
  GRA_sizeCharacteristic *xlargeticlength =
    static_cast<GRA_sizeCharacteristic*>(xAxisChars->Get("LARGETICLENGTH"));
  GRA_sizeCharacteristic *ylargeticlength =
    static_cast<GRA_sizeCharacteristic*>(yAxisChars->Get("LARGETICLENGTH"));
  GRA_sizeCharacteristic *xsmallticlength =
    static_cast<GRA_sizeCharacteristic*>(xAxisChars->Get("SMALLTICLENGTH"));
  GRA_sizeCharacteristic *ysmallticlength =
    static_cast<GRA_sizeCharacteristic*>(yAxisChars->Get("SMALLTICLENGTH"));
  GRA_angleCharacteristic *xticangle =
    static_cast<GRA_angleCharacteristic*>(xAxisChars->Get("TICANGLE"));
  GRA_angleCharacteristic *yticangle =
    static_cast<GRA_angleCharacteristic*>(yAxisChars->Get("TICANGLE"));
  GRA_intCharacteristic *xnlincs =
    static_cast<GRA_intCharacteristic*>(xAxisChars->Get("NLINCS"));
  GRA_intCharacteristic *ynlincs =
    static_cast<GRA_intCharacteristic*>(yAxisChars->Get("NLINCS"));
  GRA_intCharacteristic *xnsincs =
    static_cast<GRA_intCharacteristic*>(xAxisChars->Get("NSINCS"));
  GRA_intCharacteristic *ynsincs =
    static_cast<GRA_intCharacteristic*>(yAxisChars->Get("NSINCS"));
  GRA_boolCharacteristic *xnumberson =
    static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("NUMBERSON"));
  GRA_boolCharacteristic *ynumberson =
    static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("NUMBERSON"));
  GRA_intCharacteristic *xnumberofdigits =
    static_cast<GRA_intCharacteristic*>(xAxisChars->Get("NUMBEROFDIGITS"));
  GRA_intCharacteristic *ynumberofdigits =
    static_cast<GRA_intCharacteristic*>(yAxisChars->Get("NUMBEROFDIGITS"));
  GRA_intCharacteristic *xnumberofdecimals =
    static_cast<GRA_intCharacteristic*>(xAxisChars->Get("NUMBEROFDECIMALS"));
  GRA_intCharacteristic *ynumberofdecimals =
    static_cast<GRA_intCharacteristic*>(yAxisChars->Get("NUMBEROFDECIMALS"));
  GRA_sizeCharacteristic *ximagticlength =
    static_cast<GRA_sizeCharacteristic*>(xAxisChars->Get("IMAGTICLENGTH"));
  GRA_sizeCharacteristic *yimagticlength =
    static_cast<GRA_sizeCharacteristic*>(yAxisChars->Get("IMAGTICLENGTH"));
  GRA_angleCharacteristic *ximagticangle =
    static_cast<GRA_angleCharacteristic*>(xAxisChars->Get("IMAGTICANGLE"));
  GRA_angleCharacteristic *yimagticangle =
    static_cast<GRA_angleCharacteristic*>(yAxisChars->Get("IMAGTICANGLE"));
  GRA_stringCharacteristic *xlabel =
    static_cast<GRA_stringCharacteristic*>(xAxisChars->Get("LABEL"));
  GRA_stringCharacteristic *ylabel =
    static_cast<GRA_stringCharacteristic*>(yAxisChars->Get("LABEL"));
  GRA_boolCharacteristic *xlabelon =
    static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("LABELON"));
  GRA_boolCharacteristic *ylabelon =
    static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("LABELON"));

  XTicsBothSidesCheckBox->Checked = xticsbothsides->Get();
  XLTicLenEdit->Text = AnsiString( xlargeticlength->GetAsPercent() );
  XSTicLenEdit->Text = AnsiString( xsmallticlength->GetAsPercent() );
  XTicAngleEdit->Text = AnsiString( xticangle->Get() );
  NLXincEdit->Text = AnsiString( xnlincs->Get() );
  NSXincEdit->Text = AnsiString( xnsincs->Get() );
  XNumbersCheckBox->Checked = xnumberson->Get();
  XNDigEdit->Text = AnsiString( xnumberofdigits->Get() );
  XNDecEdit->Text = AnsiString( xnumberofdecimals->Get() );
  XNumDistEdit->Text = AnsiString( ximagticlength->GetAsPercent() );
  XNumAxisAngleEdit->Text = AnsiString( ximagticangle->Get() );
  XTextLabelEdit->Text = AnsiString( xlabel->Get().c_str() );
  XTextLabelCheckBox->Checked = xlabelon->Get();
  //
  YMinEdit->Text = AnsiString( ymin->Get() );
  YMaxEdit->Text = AnsiString( ymax->Get() );
  YVMinEdit->Text = AnsiString( yvmin->Get() );
  YVMaxEdit->Text = AnsiString( yvmax->Get() );
  YLogBaseEdit->Text = AnsiString( ylogbase->Get() );
  YLogStyleCheckBox->Checked = ylogstyle->Get();
  YLAxisEdit->Text = AnsiString( yloweraxis->GetAsPercent() );
  YUAxisEdit->Text = AnsiString( yupperaxis->GetAsPercent() );
  YAxisOnCheckBox->Checked = yaxison->Get();
  YAxisAngleEdit->Text = AnsiString( yaxisangle->Get() );
  YForceCrossCheckBox->Checked = yforcecross->Get();
  //
  YTicsCheckBox->Checked = yticson->Get();
  if( YTicsCheckBox->Checked )
  {
    YTicsBothSidesCheckBox->Enabled = true;
    YLTicLenLabel->Enabled = true;
    YLTicLenEdit->Enabled = true;
    YSTicLenLabel->Enabled = true;
    YSTicLenEdit->Enabled = true;
    YTicAngleLabel->Enabled = true;
    YTicAngleEdit->Enabled = true;
    if( autoscale->Get()=="OFF" || autoscale->Get()=="X" || autoscale->Get()=="XVIRTUAL" )
    {
      NLYincLabel->Enabled = true;
      NLYincEdit->Enabled = true;
      NSYincLabel->Enabled = true;
      NSYincEdit->Enabled = true;
    }
  }
  else
  {
    YTicsBothSidesCheckBox->Enabled = false;
    YLTicLenLabel->Enabled = false;
    YLTicLenEdit->Enabled = false;
    YSTicLenLabel->Enabled = false;
    YSTicLenEdit->Enabled = false;
    YTicAngleLabel->Enabled = false;
    YTicAngleEdit->Enabled = false;
    NLYincLabel->Enabled = false;
    NLYincEdit->Enabled = false;
    NSYincLabel->Enabled = false;
    NSYincEdit->Enabled = false;
  }
  YTicsBothSidesCheckBox->Checked = yticsbothsides->Get();
  YLTicLenEdit->Text = AnsiString( ylargeticlength->GetAsPercent() );
  YSTicLenEdit->Text = AnsiString( ysmallticlength->GetAsPercent() );
  YTicAngleEdit->Text = AnsiString( yticangle->Get() );
  NLYincEdit->Text = AnsiString( ynlincs->Get() );
  NSYincEdit->Text = AnsiString( ynsincs->Get() );
  YNumbersCheckBox->Checked = ynumberson->Get();
  YNDigEdit->Text = AnsiString( ynumberofdigits->Get() );
  YNDecEdit->Text = AnsiString( ynumberofdecimals->Get() );
  YNumDistEdit->Text = AnsiString( yimagticlength->GetAsPercent() );
  YNumAxisAngleEdit->Text = AnsiString( yimagticangle->Get() );
  YTextLabelEdit->Text = AnsiString( ylabel->Get().c_str() );
  YTextLabelCheckBox->Checked = ylabelon->Get();
  //
  NVariableTable *nvTable = NVariableTable::GetTable();
  int entries = nvTable->Entries();
  for( int i=0; i<entries; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry(i);
    if( nv->GetData().GetNumberOfDimensions() == 1 )
    {
      AnsiString name( nv->GetName().c_str() );
      DepVarComboBox->Items->Add( name );
      DepVarErrorComboBox->Items->Add( name );
      DepVarError2ComboBox->Items->Add( name );
      IndepVarComboBox->Items->Add( name );
      IndepVarErrorComboBox->Items->Add( name );
      IndepVarError2ComboBox->Items->Add( name );
    }
  }
  DepVarComboBox->ItemIndex = -1;
  DepVarErrorComboBox->ItemIndex = -1;
  DepVarError2ComboBox->ItemIndex = -1;
  IndepVarComboBox->ItemIndex = -1;
  IndepVarErrorComboBox->ItemIndex = -1;
  IndepVarError2ComboBox->ItemIndex = -1;
}

void __fastcall TGraphCommandForm::AutoscaleClick(TObject *Sender)
{
  switch ( AutoscaleRadioGroup->ItemIndex )
  {
   case 0:           // autoscale off
    VirtualCheckBox->Enabled = false;
    XVirtualCheckBox->Enabled = false;
    YVirtualCheckBox->Enabled = false;
    //
    XVMinEdit->Enabled = true;
    XVMinLabel->Enabled = true;
    XVMaxEdit->Enabled = true;
    XVMaxLabel->Enabled = true;
    //
    if( XTicsCheckBox->Checked )
    {
      NLXincEdit->Enabled = true;
      NLXincLabel->Enabled = true;
      NSXincEdit->Enabled = true;
      NSXincLabel->Enabled = true;
    }
    //
    YVMinEdit->Enabled = true;
    YVMinLabel->Enabled = true;
    YVMaxEdit->Enabled = true;
    YVMaxLabel->Enabled = true;
    //
    if( YTicsCheckBox->Checked )
    {
      NLYincEdit->Enabled = true;
      NLYincLabel->Enabled = true;
      NSYincEdit->Enabled = true;
      NSYincLabel->Enabled = true;
    }
    //
    XNDigEdit->Enabled = true;
    XNDigLabel->Enabled = true;
    XNDecEdit->Enabled = true;
    XNDecLabel->Enabled = true;
    //
    YNDigEdit->Enabled = true;
    YNDigLabel->Enabled = true;
    YNDecEdit->Enabled = true;
    YNDecLabel->Enabled = true;
    break;
   case 1:           // autoscale on
    VirtualCheckBox->Enabled = true;
    XVirtualCheckBox->Enabled = false;
    YVirtualCheckBox->Enabled = false;
    //
    XVMinEdit->Enabled = false;
    XVMinLabel->Enabled = false;
    XVMaxEdit->Enabled = false;
    XVMaxLabel->Enabled = false;
    //
    NLXincEdit->Enabled = false;
    NLXincLabel->Enabled = false;
    NSXincEdit->Enabled = false;
    NSXincLabel->Enabled = false;
    //
    YVMinEdit->Enabled = false;
    YVMinLabel->Enabled = false;
    YVMaxEdit->Enabled = false;
    YVMaxLabel->Enabled = false;
    //
    NLYincEdit->Enabled = false;
    NLYincLabel->Enabled = false;
    NSYincEdit->Enabled = false;
    NSYincLabel->Enabled = false;
    //
    XNDigEdit->Enabled = false;
    XNDigLabel->Enabled = false;
    XNDecEdit->Enabled = false;
    XNDecLabel->Enabled = false;
    //
    YNDigEdit->Enabled = false;
    YNDigLabel->Enabled = false;
    YNDecEdit->Enabled = false;
    YNDecLabel->Enabled = false;
    break;
   case 2:           // commensurate
    VirtualCheckBox->Enabled = false;
    XVirtualCheckBox->Enabled = false;
    YVirtualCheckBox->Enabled = false;
    //
    XVMinEdit->Enabled = false;
    XVMinLabel->Enabled = false;
    XVMaxEdit->Enabled = false;
    XVMaxLabel->Enabled = false;
    //
    NLXincEdit->Enabled = false;
    NLXincLabel->Enabled = false;
    NSXincEdit->Enabled = false;
    NSXincLabel->Enabled = false;
    //
    YVMinEdit->Enabled = false;
    YVMinLabel->Enabled = false;
    YVMaxEdit->Enabled = false;
    YVMaxLabel->Enabled = false;
    //
    NLYincEdit->Enabled = false;
    NLYincLabel->Enabled = false;
    NSYincEdit->Enabled = false;
    NSYincLabel->Enabled = false;
    //
    XNDigEdit->Enabled = false;
    XNDigLabel->Enabled = false;
    XNDecEdit->Enabled = false;
    XNDecLabel->Enabled = false;
    //
    YNDigEdit->Enabled = false;
    YNDigLabel->Enabled = false;
    YNDecEdit->Enabled = false;
    YNDecLabel->Enabled = false;
    break;
   case 3:           // x only
    VirtualCheckBox->Enabled = false;
    XVirtualCheckBox->Enabled = true;
    YVirtualCheckBox->Enabled = false;
    //
    XVMinEdit->Enabled = false;
    XVMinLabel->Enabled = false;
    XVMaxEdit->Enabled = false;
    XVMaxLabel->Enabled = false;
    //
    NLXincEdit->Enabled = false;
    NLXincLabel->Enabled = false;
    NSXincEdit->Enabled = false;
    NSXincLabel->Enabled = false;
    //
    YVMinEdit->Enabled = true;
    YVMinLabel->Enabled = true;
    YVMaxEdit->Enabled = true;
    YVMaxLabel->Enabled = true;
    //
    if( YTicsCheckBox->Checked )
    {
      NLYincEdit->Enabled = true;
      NLYincLabel->Enabled = true;
      NSYincEdit->Enabled = true;
      NSYincLabel->Enabled = true;
    }
    //
    XNDigEdit->Enabled = false;
    XNDigLabel->Enabled = false;
    XNDecEdit->Enabled = false;
    XNDecLabel->Enabled = false;
    //
    YNDigEdit->Enabled = false;
    YNDigLabel->Enabled = false;
    YNDecEdit->Enabled = false;
    YNDecLabel->Enabled = false;
    break;
   case 4:           // y only
    VirtualCheckBox->Enabled = false;
    XVirtualCheckBox->Enabled = false;
    YVirtualCheckBox->Enabled = true;
    //
    XVMinEdit->Enabled = true;
    XVMinLabel->Enabled = true;
    XVMaxEdit->Enabled = true;
    XVMaxLabel->Enabled = true;
    //
    if( XTicsCheckBox->Checked )
    {
      NLXincEdit->Enabled = true;
      NLXincLabel->Enabled = true;
      NSXincEdit->Enabled = true;
      NSXincLabel->Enabled = true;
    }
    //
    YVMinEdit->Enabled = false;
    YVMinLabel->Enabled = false;
    YVMaxEdit->Enabled = false;
    YVMaxLabel->Enabled = false;
    //
    NLYincEdit->Enabled = false;
    NLYincLabel->Enabled = false;
    NSYincEdit->Enabled = false;
    NSYincLabel->Enabled = false;
    //
    XNDigEdit->Enabled = true;
    XNDigLabel->Enabled = true;
    XNDecEdit->Enabled = true;
    XNDecLabel->Enabled = true;
    //
    YNDigEdit->Enabled = true;
    YNDigLabel->Enabled = true;
    YNDecEdit->Enabled = true;
    YNDecLabel->Enabled = true;
    break;
  }
}

void __fastcall TGraphCommandForm::PlotSymbolColorButtonClick(TObject *Sender)
{
  // this flag needs to be set false, to indicate if a name,
  // color, etc. has been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  TSelectColorForm *f = new TSelectColorForm(this);
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_colorCharacteristic *psc =
    static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("PLOTSYMBOLCOLOR"));
  GRA_color *color = psc->IsVector() ? psc->Gets().front() : psc->Get();
  f->SetColor( color );
  f->Caption = AnsiString("Select Plot Symbol Color");
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )psc->Set( ExGlobals::GetWorkingColor() );
}

void __fastcall TGraphCommandForm::AxesOnlyClick(TObject *Sender)
{
  if( AxesOnlyCheckBox->Checked )
  {
    OverlayCheckBox->Checked = false;
    SmoothCheckBox->Enabled = false;
  }
  else
  {
    SmoothCheckBox->Enabled = true;
  }
}

void __fastcall TGraphCommandForm::OverlayCurveClick(TObject *Sender)
{
  if( OverlayCheckBox->Checked )
  {
    AxesOnlyCheckBox->Checked = false;
    YOnRightCheckBox->Checked = false;
    XOnTopCheckBox->Checked = false;
    YOnRightCheckBox->Enabled = false;
    XOnTopCheckBox->Enabled = false;
    SmoothCheckBox->Enabled = true;
  }
  else
  {
    YOnRightCheckBox->Enabled = true;
    XOnTopCheckBox->Enabled = true;
  }
}

void __fastcall TGraphCommandForm::XAxisColorButtonClick(TObject *Sender)
{
  // this flag needs to be set false, to indicate if a name,
  // color, etc. has been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TSelectColorForm> f( new TSelectColorForm(this) );
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_colorCharacteristic *xac =
    static_cast<GRA_colorCharacteristic*>(gw->GetXAxisCharacteristics()->Get("AXISCOLOR"));
  f->SetColor( xac->Get() );
  f->Caption = AnsiString("X Axis Color");
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )xac->Set( ExGlobals::GetWorkingColor() );
}

void __fastcall TGraphCommandForm::YAxisColorButtonClick(TObject *Sender)
{
  // this flag needs to be set false, to indicate if a name,
  // color, etc. has been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TSelectColorForm> f( new TSelectColorForm(this) );
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_colorCharacteristic *yac =
    static_cast<GRA_colorCharacteristic*>(gw->GetYAxisCharacteristics()->Get("AXISCOLOR"));
  f->SetColor( yac->Get() );
  f->Caption = AnsiString("Y Axis Color");
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )yac->Set( ExGlobals::GetWorkingColor() );
}

void __fastcall TGraphCommandForm::XNumericFontButtonClick(TObject *Sender)
{
  // these flags need to be set false, to indicate if a name,
  // color, etc. has been selected in the font selector form
  //
  ExGlobals::SetWorkingFontFlags( false );
  std::auto_ptr<TSelectFontForm> f( new TSelectFontForm(this) );
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *axisChars = gw->GetXAxisCharacteristics();
  GRA_fontCharacteristic *numbersfont =
    static_cast<GRA_fontCharacteristic*>(axisChars->Get("NUMBERSFONT"));
  GRA_sizeCharacteristic *numbersheight =
    static_cast<GRA_sizeCharacteristic*>(axisChars->Get("NUMBERSHEIGHT"));
  GRA_angleCharacteristic *numbersangle =
    static_cast<GRA_angleCharacteristic*>(axisChars->Get("NUMBERSANGLE"));
  GRA_colorCharacteristic *numberscolor =
    static_cast<GRA_colorCharacteristic*>(axisChars->Get("NUMBERSCOLOR"));
  //
  f->SetFont( numbersfont->Get() );
  f->Caption = AnsiString("X Axis Numeric Labels Font");
  f->SetHeight( numbersheight->GetAsPercent() );
  f->SetAngle( numbersangle->Get() );
  f->ShowModal();
  if( ExGlobals::GetWorkingFontFlag() )
    numbersfont->Set( ExGlobals::GetWorkingFont() );
  if( ExGlobals::GetWorkingFontColorFlag() )
    numberscolor->Set( ExGlobals::GetWorkingFontColor() );
  if( ExGlobals::GetWorkingFontHeightFlag() )
    numbersheight->SetAsPercent( ExGlobals::GetWorkingFontHeight() );
  if( ExGlobals::GetWorkingFontAngleFlag() )
    numbersangle->Set( ExGlobals::GetWorkingFontAngle() );
}

void __fastcall TGraphCommandForm::YNumericFontButtonClick(TObject *Sender)
{
  // these flags need to be set false, to indicate if a name,
  // color, etc. have been selected in the font selector form
  //
  ExGlobals::SetWorkingFontFlags( false );
  std::auto_ptr<TSelectFontForm> f( new TSelectFontForm(this) );
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *axisChars = gw->GetYAxisCharacteristics();
  GRA_fontCharacteristic *numbersfont =
    static_cast<GRA_fontCharacteristic*>(axisChars->Get("NUMBERSFONT"));
  GRA_sizeCharacteristic *numbersheight =
    static_cast<GRA_sizeCharacteristic*>(axisChars->Get("NUMBERSHEIGHT"));
  GRA_angleCharacteristic *numbersangle =
    static_cast<GRA_angleCharacteristic*>(axisChars->Get("NUMBERSANGLE"));
  GRA_colorCharacteristic *numberscolor =
    static_cast<GRA_colorCharacteristic*>(axisChars->Get("NUMBERSCOLOR"));
  //
  f->SetFont( numbersfont->Get() );
  f->Caption = AnsiString("Y Axis Numeric Labels Font");
  f->SetHeight( numbersheight->GetAsPercent() );
  f->SetAngle( numbersangle->Get() );
  f->ShowModal();
  if( ExGlobals::GetWorkingFontFlag() )
    numbersfont->Set( ExGlobals::GetWorkingFont() );
  if( ExGlobals::GetWorkingFontColorFlag() )
    numberscolor->Set( ExGlobals::GetWorkingFontColor() );
  if( ExGlobals::GetWorkingFontHeightFlag() )
    numbersheight->SetAsPercent( ExGlobals::GetWorkingFontHeight() );
  if( ExGlobals::GetWorkingFontAngleFlag() )
    numbersangle->Set( ExGlobals::GetWorkingFontAngle() );
}

void __fastcall TGraphCommandForm::XTextFontButtonClick(TObject *Sender)
{
  // these flags need to be set false, to indicate if a name,
  // color, etc. has been selected in the font selector form
  //
  ExGlobals::SetWorkingFontFlags( false );
  std::auto_ptr<TSelectFontForm> f( new TSelectFontForm(this) );
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *axisChars = gw->GetXAxisCharacteristics();
  GRA_fontCharacteristic *labelfont =
    static_cast<GRA_fontCharacteristic*>(axisChars->Get("LABELFONT"));
  GRA_sizeCharacteristic *labelheight =
    static_cast<GRA_sizeCharacteristic*>(axisChars->Get("LABELHEIGHT"));
  GRA_colorCharacteristic *labelcolor =
    static_cast<GRA_colorCharacteristic*>(axisChars->Get("LABELCOLOR"));
  //
  f->SetFont( labelfont->Get() );
  f->Caption = AnsiString("X Axis Label Font");
  f->SetHeight( labelheight->GetAsPercent() );
  f->ShowModal();
  if( ExGlobals::GetWorkingFontFlag() )
    labelfont->Set( ExGlobals::GetWorkingFont() );
  if( ExGlobals::GetWorkingFontColorFlag() )
    labelcolor->Set( ExGlobals::GetWorkingFontColor() );
  if( ExGlobals::GetWorkingFontHeightFlag() )
    labelheight->SetAsPercent( ExGlobals::GetWorkingFontHeight() );
}

void __fastcall TGraphCommandForm::YTextFontButtonClick(TObject *Sender)
{
  // these flags need to be set false, to indicate if a name,
  // color, etc. has been selected in the font selector form
  //
  ExGlobals::SetWorkingFontFlags( false );
  std::auto_ptr<TSelectFontForm> f( new TSelectFontForm(this) );
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *axisChars = gw->GetYAxisCharacteristics();
  GRA_fontCharacteristic *labelfont =
    static_cast<GRA_fontCharacteristic*>(axisChars->Get("LABELFONT"));
  GRA_sizeCharacteristic *labelheight =
    static_cast<GRA_sizeCharacteristic*>(axisChars->Get("LABELHEIGHT"));
  GRA_colorCharacteristic *labelcolor =
    static_cast<GRA_colorCharacteristic*>(axisChars->Get("LABELCOLOR"));
  //
  f->SetFont( labelfont->Get() );
  f->Caption = AnsiString("Y Axis Label Font");
  f->SetHeight( labelheight->GetAsPercent() );
  f->ShowModal();
  if( ExGlobals::GetWorkingFontFlag() )
    labelfont->Set( ExGlobals::GetWorkingFont() );
  if( ExGlobals::GetWorkingFontColorFlag() )
    labelcolor->Set( ExGlobals::GetWorkingFontColor() );
  if( ExGlobals::GetWorkingFontHeightFlag() )
    labelheight->SetAsPercent( ExGlobals::GetWorkingFontHeight() );
}

void __fastcall TGraphCommandForm::ClearGraphicsButtonClick(TObject *Sender)
{
  ExGlobals::ClearGraphicsMonitor();
  ExGlobals::ClearWindows();
}

void __fastcall TGraphCommandForm::ReplotButtonClick(TObject *Sender)
{ ExGlobals::ReplotCurrentWindow(); }

void __fastcall TGraphCommandForm::DrawGraphButtonClick(TObject *Sender)
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisChars = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisChars = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *dataCurveChars = gw->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *generalChars = gw->GetGeneralCharacteristics();
  GRA_intCharacteristic *histogramtype =
    static_cast<GRA_intCharacteristic*>(dataCurveChars->Get("HISTOGRAMTYPE"));
  GRA_intCharacteristic *plotsymbol =
    static_cast<GRA_intCharacteristic*>(dataCurveChars->Get("PLOTSYMBOL"));
  GRA_sizeCharacteristic *plotsymbolsize =
    static_cast<GRA_sizeCharacteristic*>(dataCurveChars->Get("PLOTSYMBOLSIZE"));
  GRA_angleCharacteristic *plotsymbolangle =
    static_cast<GRA_angleCharacteristic*>(dataCurveChars->Get("PLOTSYMBOLANGLE"));
  //
  histogramtype->Set( histogramType_ );
  //
  int i = PlotSymbolConnectCheckBox->Checked ? 1 : -1;
  int const symArray[] = {0,1,14,2,3,4,5,6,15,7,8,16,9,17,10,18,11,12,13};
  plotsymbol->Set( i*symArray[PlotSymbolComboBox->ItemIndex] );
  //
  try
  {
    plotsymbolsize->SetAsPercent( PlotSymbolSizeEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid plot symbol size", "Error", MB_OK );
    return;
  }
  try
  {
    plotsymbolangle->Set( PlotSymbolAngleEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid plot symbol angle", "Error", MB_OK );
    return;
  }
  GRA_stringCharacteristic *autoscale =
      static_cast<GRA_stringCharacteristic*>(generalChars->Get("AUTOSCALE"));
  GRA_distanceCharacteristic *xLowerAxis =
      static_cast<GRA_distanceCharacteristic*>(xAxisChars->Get("LOWERAXIS"));
  GRA_distanceCharacteristic *yLowerAxis =
      static_cast<GRA_distanceCharacteristic*>(yAxisChars->Get("LOWERAXIS"));
  GRA_distanceCharacteristic *xUpperAxis =
      static_cast<GRA_distanceCharacteristic*>(xAxisChars->Get("UPPERAXIS"));
  GRA_distanceCharacteristic *yUpperAxis =
      static_cast<GRA_distanceCharacteristic*>(yAxisChars->Get("UPPERAXIS"));
  GRA_intCharacteristic *xnlincs =
      static_cast<GRA_intCharacteristic*>(xAxisChars->Get("NLINCS"));
  GRA_intCharacteristic *ynlincs =
      static_cast<GRA_intCharacteristic*>(yAxisChars->Get("NLINCS"));
  GRA_intCharacteristic *xnsincs =
      static_cast<GRA_intCharacteristic*>(xAxisChars->Get("NSINCS"));
  GRA_intCharacteristic *ynsincs =
      static_cast<GRA_intCharacteristic*>(yAxisChars->Get("NSINCS"));
  GRA_doubleCharacteristic *xlogbase =
      static_cast<GRA_doubleCharacteristic*>(xAxisChars->Get("LOGBASE"));
  GRA_doubleCharacteristic *ylogbase =
      static_cast<GRA_doubleCharacteristic*>(yAxisChars->Get("LOGBASE"));
  GRA_boolCharacteristic *xlogstyle =
      static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("LOGSTYLE"));
  GRA_boolCharacteristic *ylogstyle =
      static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("LOGSTYLE"));
  GRA_doubleCharacteristic *xmin = static_cast<GRA_doubleCharacteristic*>(xAxisChars->Get("MIN"));
  GRA_doubleCharacteristic *ymin = static_cast<GRA_doubleCharacteristic*>(yAxisChars->Get("MIN"));
  GRA_doubleCharacteristic *xmax = static_cast<GRA_doubleCharacteristic*>(xAxisChars->Get("MAX"));
  GRA_doubleCharacteristic *ymax = static_cast<GRA_doubleCharacteristic*>(yAxisChars->Get("MAX"));
  GRA_doubleCharacteristic *xvmin =
      static_cast<GRA_doubleCharacteristic*>(xAxisChars->Get("VIRTUALMIN"));
  GRA_doubleCharacteristic *yvmin =
      static_cast<GRA_doubleCharacteristic*>(yAxisChars->Get("VIRTUALMIN"));
  GRA_doubleCharacteristic *xvmax =
      static_cast<GRA_doubleCharacteristic*>(xAxisChars->Get("VIRTUALMAX"));
  GRA_doubleCharacteristic *yvmax =
      static_cast<GRA_doubleCharacteristic*>(yAxisChars->Get("VIRTUALMAX"));
  GRA_boolCharacteristic *xAxisOn =
      static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("AXISON"));
  GRA_boolCharacteristic *yAxisOn =
      static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("AXISON"));
  GRA_angleCharacteristic *xAxisAngle =
      static_cast<GRA_angleCharacteristic*>(xAxisChars->Get("AXISANGLE"));
  GRA_angleCharacteristic *yAxisAngle =
      static_cast<GRA_angleCharacteristic*>(yAxisChars->Get("AXISANGLE"));
  GRA_boolCharacteristic *xForceCross =
      static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("FORCECROSS"));
  GRA_boolCharacteristic *yForceCross =
      static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("FORCECROSS"));
  GRA_boolCharacteristic *xTicsOn =
      static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("TICSON"));
  GRA_boolCharacteristic *yTicsOn =
      static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("TICSON"));
  GRA_boolCharacteristic *xTicsBothSides =
      static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("TICSBOTHSIDES"));
  GRA_boolCharacteristic *yTicsBothSides =
      static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("TICSBOTHSIDES"));
  GRA_sizeCharacteristic *xLargeTicLength =
      static_cast<GRA_sizeCharacteristic*>(xAxisChars->Get("LARGETICLENGTH"));
  GRA_sizeCharacteristic *yLargeTicLength =
      static_cast<GRA_sizeCharacteristic*>(yAxisChars->Get("LARGETICLENGTH"));
  GRA_sizeCharacteristic *xSmallTicLength =
      static_cast<GRA_sizeCharacteristic*>(xAxisChars->Get("SMALLTICLENGTH"));
  GRA_sizeCharacteristic *ySmallTicLength =
      static_cast<GRA_sizeCharacteristic*>(yAxisChars->Get("SMALLTICLENGTH"));
  GRA_angleCharacteristic *xTicAngle =
      static_cast<GRA_angleCharacteristic*>(xAxisChars->Get("TICANGLE"));
  GRA_angleCharacteristic *yTicAngle =
      static_cast<GRA_angleCharacteristic*>(yAxisChars->Get("TICANGLE"));
  GRA_sizeCharacteristic *xImagTicLength =
      static_cast<GRA_sizeCharacteristic*>(xAxisChars->Get("IMAGTICLENGTH"));
  GRA_sizeCharacteristic *yImagTicLength =
      static_cast<GRA_sizeCharacteristic*>(yAxisChars->Get("IMAGTICLENGTH"));
  GRA_angleCharacteristic *xImagTicAngle =
      static_cast<GRA_angleCharacteristic*>(xAxisChars->Get("IMAGTICANGLE"));
  GRA_angleCharacteristic *yImagTicAngle =
      static_cast<GRA_angleCharacteristic*>(yAxisChars->Get("IMAGTICANGLE"));
  GRA_boolCharacteristic *xNumbersOn =
      static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("NUMBERSON"));
  GRA_boolCharacteristic *yNumbersOn =
      static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("NUMBERSON"));
  GRA_intCharacteristic *xNumberOfDigits =
      static_cast<GRA_intCharacteristic*>(xAxisChars->Get("NUMBEROFDIGITS"));
  GRA_intCharacteristic *yNumberOfDigits =
      static_cast<GRA_intCharacteristic*>(yAxisChars->Get("NUMBEROFDIGITS"));
  GRA_intCharacteristic *xNumberOfDecimals =
      static_cast<GRA_intCharacteristic*>(xAxisChars->Get("NUMBEROFDECIMALS"));
  GRA_intCharacteristic *yNumberOfDecimals =
      static_cast<GRA_intCharacteristic*>(yAxisChars->Get("NUMBEROFDECIMALS"));
  GRA_boolCharacteristic *xlabelon = static_cast<GRA_boolCharacteristic*>(xAxisChars->Get("LABELON"));
  GRA_boolCharacteristic *ylabelon = static_cast<GRA_boolCharacteristic*>(yAxisChars->Get("LABELON"));
  GRA_stringCharacteristic *xlabel = static_cast<GRA_stringCharacteristic*>(xAxisChars->Get("LABEL"));
  GRA_stringCharacteristic *ylabel = static_cast<GRA_stringCharacteristic*>(yAxisChars->Get("LABEL"));
  //
  switch ( AutoscaleRadioGroup->ItemIndex )
  {
    case 0:
      autoscale->Set("OFF");
      if( wasCommensurate_ )
      {
        xLowerAxis->SetAsPercent( 10.0 );
        xUpperAxis->SetAsPercent( 90.0 );
        yLowerAxis->SetAsPercent( 10.0 );
        yUpperAxis->SetAsPercent( 90.0 );
        wasCommensurate_ = false;
      }
      xnlincs->Set( 0 );
      ynlincs->Set( 0 );
      break;
    case 1:
      VirtualCheckBox->Checked ? autoscale->Set("VIRTUAL") : autoscale->Set("ON");
      if( wasCommensurate_ )
      {
        xLowerAxis->SetAsPercent( 10.0 );
        xUpperAxis->SetAsPercent( 90.0 );
        yLowerAxis->SetAsPercent( 10.0 );
        yUpperAxis->SetAsPercent( 90.0 );
        wasCommensurate_ = false;
      }
      break;
    case 2:
      autoscale->Set("COMMENSURATE");
      wasCommensurate_ = true;
      break;
    case 3:
      XVirtualCheckBox->Checked ? autoscale->Set("XVIRTUAL") : autoscale->Set("X");
      if( wasCommensurate_ )
      {
        xLowerAxis->SetAsPercent( 10.0 );
        xUpperAxis->SetAsPercent( 90.0 );
        yLowerAxis->SetAsPercent( 10.0 );
        yUpperAxis->SetAsPercent( 90.0 );
        wasCommensurate_ = false;
      }
      xnlincs->Set( 0 );
      break;
    case 4:
      YVirtualCheckBox->Checked ? autoscale->Set("YVIRTUAL") : autoscale->Set("Y");
      if( wasCommensurate_ )
      {
        xLowerAxis->SetAsPercent( 10.0 );
        xUpperAxis->SetAsPercent( 90.0 );
        yLowerAxis->SetAsPercent( 10.0 );
        yUpperAxis->SetAsPercent( 90.0 );
        wasCommensurate_ = false;
      }
      ynlincs->Set( 0 );
      break;
  }
  try
  {
    xlogbase->Set( XLogBaseEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid x-axis log base", "Error", MB_OK );
    return;
  }
  xlogstyle->Set( XLogStyleCheckBox->Checked );
  try
  {
    xmin->Set( XMinEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid x-axis min", "Error", MB_OK );
    return;
  }
  try
  {
    xmax->Set( XMaxEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid x-axis max", "Error", MB_OK );
    return;
  }
  if( XVMinEdit->Enabled )
  {
    try
    {
      xvmin->Set( XVMinEdit->Text.ToDouble() );
    }
    catch (...)
    {
      Application->MessageBox( "invalid x-axis virtual min", "Error", MB_OK );
      return;
    }
    try
    {
      xvmax->Set( XVMaxEdit->Text.ToDouble() );
    }
    catch (...)
    {
      Application->MessageBox( "invalid x-axis virtual max", "Error", MB_OK );
      return;
    }
  }
  if( NLXincEdit->Enabled )
  {
    try
    {
      xnlincs->Set( NLXincEdit->Text.ToInt() );
    }
    catch (...)
    {
      Application->MessageBox( "invalid number of x-axis large increments", "Error", MB_OK );
      return;
    }
  }
  if( NSXincEdit->Enabled )
  {
    try
    {
      xnsincs->Set( NSXincEdit->Text.ToInt() );
    }
    catch (...)
    {
      Application->MessageBox( "invalid number of x-axis small increments", "Error", MB_OK );
      return;
    }
  }
  try
  {
    ylogbase->Set( YLogBaseEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid y-axis log base", "Error", MB_OK );
    return;
  }
  ylogstyle->Set( YLogStyleCheckBox->Checked );
  try
  {
    ymin->Set( YMinEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid y-axis min", "Error", MB_OK );
    return;
  }
  try
  {
    ymax->Set( YMaxEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid y-axis max", "Error", MB_OK );
    return;
  }
  if( YVMinEdit->Enabled )
  {
    try
    {
      yvmin->Set( YVMinEdit->Text.ToDouble() );
    }
    catch (...)
    {
      Application->MessageBox( "invalid y-axis virtual min", "Error", MB_OK );
      return;
    }
    try
    {
      yvmax->Set( YVMaxEdit->Text.ToDouble() );
    }
    catch (...)
    {
      Application->MessageBox( "invalid y-axis virtual max", "Error", MB_OK );
      return;
    }
  }
  if( NLYincEdit->Enabled )
  {
    try
    {
      ynlincs->Set( NLYincEdit->Text.ToInt() );
    }
    catch (...)
    {
      Application->MessageBox( "invalid number of y-axis large increments", "Error", MB_OK );
      return;
    }
  }
  if( NSYincEdit->Enabled )
  {
    try
    {
      ynsincs->Set( NSYincEdit->Text.ToInt() );
    }
    catch (...)
    {
      Application->MessageBox( "invalid number of y-axis small increments", "Error", MB_OK );
      return;
    }
  }
  xAxisOn->Set( XAxisOnCheckBox->Checked );
  try
  {
    xLowerAxis->SetAsPercent( XLAxisEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid lower x-axis percentage", "Error", MB_OK );
    return;
  }
  try
  {
    xUpperAxis->SetAsPercent( XUAxisEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid upper x-axis percentage", "Error", MB_OK );
    return;
  }
  try
  {
    xAxisAngle->Set( XAxisAngleEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid x-axis angle", "Error", MB_OK );
    return;
  }
  xForceCross->Set( XForceCrossCheckBox->Checked );
  //
  xTicsOn->Set( XTicsCheckBox->Checked );
  xTicsBothSides->Set( XTicsBothSidesCheckBox->Checked );
  try
  {
    xLargeTicLength->SetAsPercent( XLTicLenEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid x-axis large tic length percentage", "Error", MB_OK );
    return;
  }
  try
  {
    xSmallTicLength->SetAsPercent( XSTicLenEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid x-axis small tic length percentage", "Error", MB_OK );
    return;
  }
  try
  {
    xTicAngle->Set( XTicAngleEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid x-axis tic angle", "Error", MB_OK );
    return;
  }
  xNumbersOn->Set( XNumbersCheckBox->Checked );
  try
  {
    xNumberOfDigits->Set( XNDigEdit->Text.ToInt() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid x-axis number of digits", "Error", MB_OK );
    return;
  }
  try
  {
    xNumberOfDecimals->Set( XNDecEdit->Text.ToInt() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid x-axis number of decimals", "Error", MB_OK );
    return;
  }
  try
  {
    xImagTicLength->SetAsPercent( XNumDistEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid x-axis number distance percentage", "Error", MB_OK );
    return;
  }
  try
  {
    xImagTicAngle->Set( XNumAxisAngleEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid number x-axis angle", "Error", MB_OK );
    return;
  }
  xlabelon->Set( XTextLabelCheckBox->Checked );
  xlabel->Set( XTextLabelEdit->Text.c_str() );
  //
  yAxisOn->Set( YAxisOnCheckBox->Checked );
  try
  {
    yLowerAxis->SetAsPercent( YLAxisEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid lower y-axis percentage", "Error", MB_OK );
    return;
  }
  try
  {
    yUpperAxis->SetAsPercent( YUAxisEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid upper y-axis percentage", "Error", MB_OK );
    return;
  }
  try
  {
    yAxisAngle->Set( YAxisAngleEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid y-axis angle", "Error", MB_OK );
    return;
  }
  yForceCross->Set( YForceCrossCheckBox->Checked );
  //
  yTicsOn->Set( YTicsCheckBox->Checked );
  yTicsBothSides->Set( YTicsBothSidesCheckBox->Checked );
  try
  {
    yLargeTicLength->SetAsPercent( YLTicLenEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid y-axis large tic length percentage", "Error", MB_OK );
    return;
  }
  try
  {
    ySmallTicLength->SetAsPercent( YSTicLenEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid y-axis small tic length percentage", "Error", MB_OK );
    return;
  }
  try
  {
    yTicAngle->Set( YTicAngleEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid y-axis tic angle", "Error", MB_OK );
    return;
  }
  yNumbersOn->Set( YNumbersCheckBox->Checked );
  try
  {
    yNumberOfDigits->Set( YNDigEdit->Text.ToInt() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid y-axis number of digits", "Error", MB_OK );
    return;
  }
  try
  {
    yNumberOfDecimals->Set( YNDecEdit->Text.ToInt() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid y-axis number of decimals", "Error", MB_OK );
    return;
  }
  try
  {
    yImagTicLength->SetAsPercent( YNumDistEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid y-axis number distance percentage", "Error", MB_OK );
    return;
  }
  try
  {
    yImagTicAngle->Set( YNumAxisAngleEdit->Text.ToDouble() );
  }
  catch (...)
  {
    Application->MessageBox( "invalid number y-axis angle", "Error", MB_OK );
    return;
  }
  ylabelon->Set( YTextLabelCheckBox->Checked );
  ylabel->Set( YTextLabelEdit->Text.c_str() );
  //
  DrawGraph();
}

void __fastcall TGraphCommandForm::DrawGraph()
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxisChars = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisChars = gw->GetYAxisCharacteristics();
  GRA_stringCharacteristic *xAutoscale =
      static_cast<GRA_stringCharacteristic*>(xAxisChars->Get("AUTOSCALE"));
  GRA_stringCharacteristic *yAutoscale =
      static_cast<GRA_stringCharacteristic*>(yAxisChars->Get("AUTOSCALE"));
  //
  bool xE1 = false;
  bool yE1 = false;
  bool xE2 = false;
  bool yE2 = false;
  std::vector<double> x, y, xe1, ye1, xe2, ye2;
  if( AxesOnlyCheckBox->Checked ) // just plotting axes
  {
    if( IndepVarComboBox->Text.IsEmpty() ) // no x vector
    {
      if( DepVarComboBox->Text.IsEmpty() ) // no x or y vectors
      {
        xAutoscale->Set("OFF");
        yAutoscale->Set("OFF");
        x.push_back( 0.0 );
        y.push_back( 0.0 );
      }
      else                           // no x, but y is there
      {
        try
        {
          NumericVariable::GetVector( IndepVarComboBox->Text.c_str(), "dependent variable", y );
        }
        catch( EVariableError &e )
        {
          Application->MessageBox( e.what(), "Error", MB_OK );
          return;
        }
        std::size_t size = y.size();
        for( std::size_t i=0; i<size; ++i )x.push_back( static_cast<double>(i) );
      }
    }
    else                           // x is there
    {
      try
      {
        NumericVariable::GetVector( IndepVarComboBox->Text.c_str(), "independent variable", x );
        NumericVariable::GetVector( DepVarComboBox->Text.c_str(), "dependent variable", y );
      }
      catch( EVariableError &e )
      {
        Application->MessageBox( e.what(), "Error", MB_OK );
        return;
      }
      if( x.size() != y.size() )
      {
        Application->MessageBox( "input vectors have different lengths, using minimum",
         "Warning", MB_OK );
      }
    }
    std::size_t num = std::min(x.size(),y.size());
    if( num < x.size() )x.resize( num );
    if( num < y.size() )y.resize( num );
    GRA_cartesianAxes *cartesianAxes =
        new GRA_cartesianAxes( x, y, XOnTopCheckBox->Checked, YOnRightCheckBox->Checked );
    try
    {
      cartesianAxes->Make();
    }
    catch (EGraphicsError &e)
    {
      delete cartesianAxes;
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    cartesianAxes->Draw( ExGlobals::GetScreenOutput() );
    return;
  }
  if( IndepVarComboBox->Text.IsEmpty() ) // no x vector
  {
    if( DepVarComboBox->Text.IsEmpty() ) // no x or y vectors
    {
      Application->MessageBox( "no data vectors entered", "Error", MB_OK );
      return;
    }
    try
    {
      NumericVariable::GetVector( DepVarComboBox->Text.c_str(), "dependent variable", y );
    }
    catch( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    std::size_t size = y.size();
    for( std::size_t i=0; i<size; ++i )x.push_back( static_cast<double>(i) );
  }
  else                           // x is there
  {
    try
    {
      NumericVariable::GetVector( IndepVarComboBox->Text.c_str(), "independent variable", x );
      NumericVariable::GetVector( DepVarComboBox->Text.c_str(), "dependent variable", y );
    }
    catch( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    if( x.size() != y.size() )Application->MessageBox(
       "input vectors have different lengths, using minimum", "Warning", MB_OK );
  }
  std::size_t num = std::min(x.size(),y.size());
  if( num < x.size() )x.resize( num );
  if( num < y.size() )y.resize( num );
  if( !DepVarErrorComboBox->Text.IsEmpty() ) // there is a y error vector
  {
    yE1 = true;
    try
    {
      NumericVariable::GetVector( DepVarErrorComboBox->Text.c_str(), "y error vector", ye1 );
    }
    catch( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
  if( !IndepVarErrorComboBox->Text.IsEmpty() ) // there is an x error vector
  {
    xE1 = true;
    try
    {
      NumericVariable::GetVector( IndepVarErrorComboBox->Text.c_str(), "x error vector", xe1 );
    }
    catch( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
  if( !DepVarError2ComboBox->Text.IsEmpty() ) // there is a second y error vector
  {
    yE2 = true;
    try
    {
      NumericVariable::GetVector( DepVarError2ComboBox->Text.c_str(), "second y error vector", ye2 );
    }
    catch( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
  if( !IndepVarError2ComboBox->Text.IsEmpty() ) // there is a second x error vector
  {
    xE2 = true;
    try
    {
      NumericVariable::GetVector( IndepVarError2ComboBox->Text.c_str(), "second x error vector", xe2 );
    }
    catch( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
  }
  if( yE1 )num = min(num,ye1.size());
  if( xE1 )num = min(num,xe1.size());
  if( yE2 )num = min(num,ye2.size());
  if( xE2 )num = min(num,xe2.size());
  if( num < x.size() )x.resize( num );
  if( num < y.size() )y.resize( num );
  if( xE1 && num < xe1.size() )xe1.resize( num );
  if( yE1 && num < ye1.size() )ye1.resize( num );
  if( xE2 && num < xe2.size() )xe2.resize( num );
  if( yE2 && num < ye2.size() )ye2.resize( num );
  //
  GRA_cartesianAxes *cartesianAxes =
      new GRA_cartesianAxes( x, y, XOnTopCheckBox->Checked, YOnRightCheckBox->Checked );
  try
  {
    cartesianAxes->Make();
  }
  catch (EGraphicsError &e)
  {
    delete cartesianAxes;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  cartesianAxes->Draw( ExGlobals::GetScreenOutput() );
  //
  GRA_cartesianCurve *cartesianCurve =
      new GRA_cartesianCurve(x,y,xe1,ye1,xe2,ye2,SmoothCheckBox->Checked);
  try
  {
    cartesianCurve->Make();
  }
  catch (EGraphicsError &e)
  {
    delete cartesianCurve;
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  cartesianCurve->Draw( ExGlobals::GetScreenOutput() );
  gw->AddDrawableObject( cartesianAxes );
  gw->AddDrawableObject( cartesianCurve );
  return;
}

void __fastcall TGraphCommandForm::DefaultsButtonClick(TObject *Sender)
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *generalChars = gw->GetGeneralCharacteristics();
  gw->SetDataCurveDefaults();
  static_cast<GRA_stringCharacteristic*>(generalChars->Get("AUTOSCALE"))->Set("ON");
  gw->SetXAxisDefaults();
  gw->SetYAxisDefaults();
  FillOutForm();
}

void __fastcall TGraphCommandForm::CloseButtonClick( TObject *Sender )
{
  Close();
  GraphCommandForm = 0;
}

void __fastcall TGraphCommandForm::H0Click( TObject *Sender )
{
  switch ( histogramType_ )
  {
    case 1:
      PutImage( HT0Image, HT0aImage );
      PutImage( HT1Image, HT1bImage );
      break;
    case 2:
      PutImage( HT0Image, HT0aImage );
      PutImage( HT2Image, HT2bImage );
      break;
    case 3:
      PutImage( HT0Image, HT0aImage );
      PutImage( HT3Image, HT3bImage );
      break;
    case 4:
      PutImage( HT0Image, HT0aImage );
      PutImage( HT4Image, HT4bImage );
      break;
  }
  GRA_setOfCharacteristics *dataCurveC =
    ExGlobals::GetGraphWindow()->GetDataCurveCharacteristics();
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get("HISTOGRAMTYPE"))->Set( 0 );
  histogramType_ = 0;
}

void __fastcall TGraphCommandForm::H1Click( TObject *Sender )
{
  switch ( histogramType_ )
  {
    case 0:
      PutImage( HT1Image, HT1aImage );
      PutImage( HT0Image, HT0bImage );
      break;
    case 2:
      PutImage( HT1Image, HT1aImage );
      PutImage( HT2Image, HT2bImage );
      break;
    case 3:
      PutImage( HT1Image, HT1aImage );
      PutImage( HT3Image, HT3bImage );
      break;
    case 4:
      PutImage( HT1Image, HT1aImage );
      PutImage( HT4Image, HT4bImage );
      break;
  }
  GRA_setOfCharacteristics *dataCurveC =
    ExGlobals::GetGraphWindow()->GetDataCurveCharacteristics();
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get("HISTOGRAMTYPE"))->Set( 0 );
  histogramType_ = 1;
}

void __fastcall TGraphCommandForm::H2Click( TObject *Sender )
{
  switch ( histogramType_ )
  {
    case 0:
      PutImage( HT2Image, HT2aImage );
      PutImage( HT0Image, HT0bImage );
      break;
    case 1:
      PutImage( HT2Image, HT2aImage );
      PutImage( HT1Image, HT1bImage );
      break;
    case 3:
      PutImage( HT2Image, HT2aImage );
      PutImage( HT3Image, HT3bImage );
      break;
    case 4:
      PutImage( HT2Image, HT2aImage );
      PutImage( HT4Image, HT4bImage );
      break;
  }
  GRA_setOfCharacteristics *dataCurveC =
    ExGlobals::GetGraphWindow()->GetDataCurveCharacteristics();
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get("HISTOGRAMTYPE"))->Set( 0 );
  histogramType_ = 2;
}

void __fastcall TGraphCommandForm::H3Click(TObject *Sender)
{
  switch ( histogramType_ )
  {
    case 0:
      PutImage( HT3Image, HT3aImage );
      PutImage( HT0Image, HT0bImage );
      break;
    case 1:
      PutImage( HT3Image, HT3aImage );
      PutImage( HT1Image, HT1bImage );
      break;
    case 2:
      PutImage( HT3Image, HT3aImage );
      PutImage( HT2Image, HT2bImage );
      break;
    case 4:
      PutImage( HT3Image, HT3aImage );
      PutImage( HT4Image, HT4bImage );
      break;
  }
  GRA_setOfCharacteristics *dataCurveC =
    ExGlobals::GetGraphWindow()->GetDataCurveCharacteristics();
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get("HISTOGRAMTYPE"))->Set( 0 );
  histogramType_ = 3;
}

void __fastcall TGraphCommandForm::H4Click(TObject *Sender)
{
  switch ( histogramType_ )
  {
    case 0:
      PutImage( HT4Image, HT4aImage );
      PutImage( HT0Image, HT0bImage );
      break;
    case 1:
      PutImage( HT4Image, HT4aImage );
      PutImage( HT1Image, HT1bImage );
      break;
    case 2:
      PutImage( HT4Image, HT4aImage );
      PutImage( HT2Image, HT2bImage );
      break;
    case 3:
      PutImage( HT4Image, HT4aImage );
      PutImage( HT3Image, HT3bImage );
      break;
  }
  GRA_setOfCharacteristics *dataCurveC =
    ExGlobals::GetGraphWindow()->GetDataCurveCharacteristics();
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get("HISTOGRAMTYPE"))->Set( 0 );
  histogramType_ = 4;
}

void __fastcall TGraphCommandForm::PutImage( TImage *i1, TImage *i2 )
{ i1->Picture->Assign(i2->Picture); }

//end if file

