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

#include "extremaMain.h"
#include "MainGraphicsWindow.h"
#include "PNGDisplay.h"
#include "DigitizeInfo.h"
#include "NumericVariable.h"
#include "EVariableError.h"

#include "DigitizePNG.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TDigitizePNGForm *DigitizePNGForm;

__fastcall TDigitizePNGForm::TDigitizePNGForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "DigitizePNGForm", "Top", 250 );
  this->Left = ini->ReadInteger( "DigitizePNGForm", "Left", 500 );
  this->Height = ini->ReadInteger( "DigitizePNGForm", "Height", 305 );
  this->Width = ini->ReadInteger( "DigitizePNGForm", "Width", 405 );
  delete ini;
  StartStopButton->Caption = AnsiString("Start digitizing");
  digitizing_ = false;
}

__fastcall TDigitizePNGForm::~TDigitizePNGForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "DigitizePNGForm", "Top", this->Top );
  ini->WriteInteger( "DigitizePNGForm", "Left", this->Left );
  ini->WriteInteger( "DigitizePNGForm", "Height", this->Height );
  ini->WriteInteger( "DigitizePNGForm", "Width", this->Width );
  delete ini;
  DigitizePNGForm = NULL;
}

void __fastcall TDigitizePNGForm::CloseIt()
{
  MainForm->Enabled = true;
  MainGraphicsForm->Enabled = true;
  if( DigitizeInfoForm )
  {
    delete DigitizeInfoForm;
    DigitizeInfoForm = 0;
  }
}

void __fastcall TDigitizePNGForm::FormClose(TObject *Sender, TCloseAction &Action)
{ CloseIt(); Action=caFree; }

void __fastcall TDigitizePNGForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{ CloseIt(); }

void __fastcall TDigitizePNGForm::CloseClick(TObject *Sender)
{ CloseIt(); Close(); }

void __fastcall TDigitizePNGForm::SetPicked( int x, int y )
{ xImage_[nPick_]=x; yImage_[nPick_]=y; }

void __fastcall TDigitizePNGForm::GetUserUnits( int ix, int iy, double &x, double &y )
{
  x = a_ + b_*ix + c_*iy;
  y = d_ + e_*ix + f_*iy;
}

void __fastcall TDigitizePNGForm::StartStopClick(TObject *Sender)
{
  if( !digitizing_ )
  {
    MainForm->Enabled = false;
    MainGraphicsForm->Enabled = false;
    DigitizePNGForm->Enabled = false;
    //
    PointsLabel1->Visible = false;
    XYLabel1->Visible = false;
    PointsLabel2->Visible = false;
    XYLabel2->Visible = false;
    PointsLabel3->Visible = false;
    XYLabel3->Visible = false;
    NPointsLabel->Visible = false;
    NPLabel->Visible = false;
    LastPointLabel->Visible = false;
    LPLabel->Visible = false;
    //
    nPick_ = 0;
    //
    PNGForm->StartPicking();
    PNGForm->ClearStatusBar();
    //
    if( DigitizeInfoForm )
    {
      DigitizeInfoForm->Close();
      DigitizeInfoForm = 0;
      DigitizeInfoForm = new TDigitizeInfoForm(this);
      DigitizeInfoForm->Show();
    }
    else
    {
      DigitizeInfoForm = new TDigitizeInfoForm(this);
      DigitizeInfoForm->Show();
    }
    PNGForm->SetFocus();
  }
  else
  {
    MainForm->Enabled = true;
    MainGraphicsForm->Enabled = true;
    PNGForm->StopDigitizing();
    StartStopButton->Caption = "Start digitizing";
    digitizing_ = false;
    //
    if( !XoutEdit->Text.IsEmpty() )
    {
      try
      {
        NumericVariable::PutVariable( XoutEdit->Text.c_str(), xd_, 0, "DIGITIZE PNG" );
      }
      catch( EVariableError &e )
      {
        Application->MessageBox(e.what(),"ERROR",MB_OK);
        return;
      }
    }
    if( !YoutEdit->Text.IsEmpty() )
    {
      try
      {
        NumericVariable::PutVariable( YoutEdit->Text.c_str(), yd_, 0, "DIGITIZE PNG" );
      }
      catch( EVariableError &e )
      {
        Application->MessageBox(e.what(),"ERROR",MB_OK);
        return;
      }
    }
  }
}

void __fastcall TDigitizePNGForm::StartDigitizing()
{
  digitizing_ = true;
  StartStopButton->Caption = "Stop digitizing";
  SetupCoordinateTransform();
  PNGForm->StartDigitizing();
  nDigitized_ = 0;
  NPointsLabel->Visible = true;
  NPLabel->Visible = true;
  NPLabel->Caption = AnsiString(nDigitized_);
  LastPointLabel->Visible = true;
  std::vector<double>().swap( xd_ );
  std::vector<double>().swap( yd_ );
}

void __fastcall TDigitizePNGForm::AbortDigitizing()
{
  PointsLabel1->Visible = false;
  XYLabel1->Visible = false;
  PointsLabel2->Visible = false;
  XYLabel2->Visible = false;
  PointsLabel3->Visible = false;
  XYLabel3->Visible = false;
  NPointsLabel->Visible = false;
  NPLabel->Visible = false;
  LastPointLabel->Visible = false;
  LPLabel->Visible = false;
  StartStopButton->Caption = "Start digitizing";
  std::vector<double>().swap( xd_ );
  std::vector<double>().swap( yd_ );
}

void __fastcall TDigitizePNGForm::SetDigitized( int ix, int iy )
{
  NPLabel->Caption = AnsiString(++nDigitized_);
  double x = a_ + b_*ix + c_*iy;
  double y = d_ + e_*ix + f_*iy;
  char c[26];
  sprintf(c,"(%11.3lg,%11.3lg)",x,y);
  LPLabel->Visible = true;
  LPLabel->Caption = AnsiString(c);
  xd_.push_back(x);
  yd_.push_back(y);
}

void __fastcall TDigitizePNGForm::SetPoints( double x, double y )
{
  if( nPick_ == 3 )return;
  AnsiString s("x = ");
  s += AnsiString(x) + AnsiString(", y = ") + AnsiString(y);
  switch (nPick_)
  {
    case 0:
    {
      XYLabel1->Caption = s;
      XYLabel1->Visible = true;
      PointsLabel1->Visible = true;
      break;
    }
    case 1:
    {
      XYLabel2->Caption = s;
      XYLabel2->Visible = true;
      PointsLabel2->Visible = true;
      break;
    }
    case 2:
    {
      XYLabel3->Caption = s;
      XYLabel3->Visible = true;
      PointsLabel3->Visible = true;
    }
  }
  xUser_[nPick_] = x;
  yUser_[nPick_] = y;
  ++nPick_;
  DigitizeInfoForm->Next();
  DigitizeInfoForm->SetFocus();
}

void __fastcall TDigitizePNGForm::SetupCoordinateTransform()
{
  double denom = static_cast<double>(xImage_[0]*yImage_[1] - xImage_[1]*yImage_[0] -
                                     xImage_[2]*(yImage_[1]-yImage_[0]) +
                                     yImage_[2]*(xImage_[1]-xImage_[0]));
  if( denom == 0.0 )
  {
    Application->MessageBox("division by zero in CoordinateTransform","ERROR",MB_OK);
    DigitizePNGForm->Enabled = true;
    AbortDigitizing();
    return;
  }
  //
  a_ = (xImage_[2]*(yImage_[0]*xUser_[1] - yImage_[1]*xUser_[0]) -
        yImage_[2]*(xImage_[0]*xUser_[1] - xImage_[1]*xUser_[0]) +
        xUser_[2]*(xImage_[0]*yImage_[1] - xImage_[1]*yImage_[0]))/denom;
  //
  b_ = (-yImage_[0]*xUser_[1] + yImage_[1]*xUser_[0] +
        yImage_[2]*(xUser_[1]-xUser_[0]) - xUser_[2]*(yImage_[1]-yImage_[0]))/denom;
  //
  c_ = (xImage_[0]*xUser_[1] - xImage_[1]*xUser_[0] -
        xImage_[2]*(xUser_[1]-xUser_[0]) + xUser_[2]*(xImage_[1]-xImage_[0]))/denom;
  //
  d_ = (xImage_[2]*(yImage_[0]*yUser_[1]-yImage_[1]*yUser_[0]) -
        yImage_[2]*(xImage_[0]*yUser_[1]-xImage_[1]*yUser_[0]) +
        yUser_[2]*(xImage_[0]*yImage_[1]-xImage_[1]*yImage_[0]))/denom;
  //
  e_ = (-yImage_[0]*yUser_[1] + yImage_[1]*yUser_[0] +
        yImage_[2]*(yUser_[1]-yUser_[0]) - yUser_[2]*(yImage_[1]-yImage_[0]))/denom;
  //
  f_ = (xImage_[0]*yUser_[1] - xImage_[1]*yUser_[0] -
        xImage_[2]*(yUser_[1]-yUser_[0]) + yUser_[2]*(xImage_[1]-xImage_[0]))/denom;
}

// end of file
