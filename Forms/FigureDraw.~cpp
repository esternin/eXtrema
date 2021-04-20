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

#include "FigureDraw.h"
#include "MainGraphicsWindow.h"
#include "GRA_color.h"
#include "GRA_window.h"
#include "SelectLineWidth.h"
#include "SelectColor.h"
#include "ExGlobals.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_colorControl.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TFigureDrawForm *FigureDrawForm;

__fastcall TFigureDrawForm::TFigureDrawForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "FigureDrawForm", "Top", 220 );
  this->Left = ini->ReadInteger( "FigureDrawForm", "Left", 850 );
  arrowType_ = ini->ReadInteger( "FigureDrawForm", "ArrowType", 1 );
  polygonType_ = ini->ReadInteger( "FigureDrawForm", "PolygonType", 1 );
  polygonAngle_ = ini->ReadFloat( "FigureDrawForm", "PolygonAngle", 0.0 );
  bool headsBothEnds = ini->ReadBool( "FigureDrawForm", "HeadsBothEnds", false );
  bool drawCircles = ini->ReadBool( "FigureDrawForm", "DrawCircles", false );
  delete ini;
  //
  switch( arrowType_ )
  {
    case 1:
      if( headsBothEnds )
      {
        PutImage( Image1b, Image1  );
        PutImage( Image2b, Image23 );
        PutImage( Image3b, Image33 );
      }
      else
      {
        PutImage( Image1b, Image1  );
        PutImage( Image2b, Image21 );
        PutImage( Image3b, Image31 );
      }
      break;
    case 2:
      if( headsBothEnds )
      {
        PutImage( Image1b, Image13 );
        PutImage( Image2b, Image22 );
        PutImage( Image3b, Image33 );
      }
      else
      {
        PutImage( Image1b, Image11 );
        PutImage( Image2b, Image2  );
        PutImage( Image3b, Image31 );
      }
      break;
    case 3:
      if( headsBothEnds )
      {
        PutImage( Image1b, Image13 );
        PutImage( Image2b, Image23 );
        PutImage( Image3b, Image32 );
      }
      else
      {
        PutImage( Image1b, Image11 );
        PutImage( Image2b, Image21 );
        PutImage( Image3b, Image3  );
      }
      break;
  }
  HeadsCheckBox->Checked = headsBothEnds;
  MainGraphicsForm->SetArrowType( arrowType_ );
  switch( polygonType_ )
  {
    case 1:  // rectangle
      PutImage( Rectangle1bImage, RectangleImage );
      PutImage( Square1bImage, Square1Image  );
      PutImage( Star1bImage, Star1Image  );
      break;
    case 2:  // square
      PutImage( Rectangle1bImage, Rectangle1Image  );
      PutImage( Square1bImage, SquareImage );
      PutImage( Star1bImage, Star1Image  );
      break;
    case 3:  // 5 pt star
      PutImage( Rectangle1bImage, Rectangle1Image  );
      PutImage( Square1bImage, Square1Image  );
      PutImage( Star1bImage, StarImage  );
      break;
  }
  MainGraphicsForm->SetPolygonType( polygonType_ );
  MainGraphicsForm->SetPolygonAngle( polygonAngle_ );
  CircleCheckBox->Checked = drawCircles;
}

__fastcall TFigureDrawForm::~TFigureDrawForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "FigureDrawForm", "Top", this->Top );
  ini->WriteInteger( "FigureDrawForm", "Left", this->Left );
  ini->WriteInteger( "FigureDrawForm", "ArrowType", arrowType_ );
  ini->WriteInteger( "FigureDrawForm", "PolygonType", polygonType_ );
  ini->WriteFloat( "FigureDrawForm", "PolygonAngle", polygonAngle_ );
  ini->WriteBool( "FigureDrawForm", "HeadsBothEnds", HeadsCheckBox->Checked );
  ini->WriteBool( "FigureDrawForm", "DrawCircles", CircleCheckBox->Checked );
  delete ini;
}

void __fastcall TFigureDrawForm::ClickImage1(TObject *Sender)
{
  switch( arrowType_ )
  {
    case 1:
      break;
    case 2:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image12 );
        PutImage( Image2b, Image23 );
      }
      else
      {
        PutImage( Image1b, Image1  );
        PutImage( Image2b, Image23 );
      }
      break;
    case 3:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image12 );
        PutImage( Image3b, Image33 );
      }
      else
      {
        PutImage( Image1b, Image1  );
        PutImage( Image3b, Image33 );
      }
      break;
  }
  arrowType_ = 1;
  MainGraphicsForm->SetArrowType( arrowType_ );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TFigureDrawForm::ClickImage2(TObject *Sender)
{
  switch( arrowType_ )
  {
    case 1:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image13 );
        PutImage( Image2b, Image22 );
      }
      else
      {
        PutImage( Image1b, Image11 );
        PutImage( Image2b, Image2  );
      }
      break;
    case 2:
      break;
    case 3:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image2b, Image22 );
        PutImage( Image3b, Image33 );
      }
      else
      {
        PutImage( Image2b, Image2  );
        PutImage( Image3b, Image31 );
      }
      break;
  }
  arrowType_ = 2;
  MainGraphicsForm->SetArrowType( arrowType_ );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TFigureDrawForm::ClickImage3(TObject *Sender)
{
  switch( arrowType_ )
  {
    case 1:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image13 );
        PutImage( Image3b, Image32 );
      }
      else
      {
        PutImage( Image1b, Image11 );
        PutImage( Image3b, Image3  );
      }
      break;
    case 2:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image2b, Image23 );
        PutImage( Image3b, Image32 );
      }
      else
      {
        PutImage( Image2b, Image21 );
        PutImage( Image3b, Image3  );
      }
      break;
    case 3:
      break;
  }
  arrowType_ = 3;
  MainGraphicsForm->SetArrowType( arrowType_ );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TFigureDrawForm::PutImage( TImage *i1, TImage *i2 )
{
  i1->Picture->Assign(i2->Picture);
}

void __fastcall TFigureDrawForm::CloseClick(TObject *Sender)
{
  CloseForm();
}

void TFigureDrawForm::CloseForm()
{
  Close();
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TFigureDrawForm::FormClose(TObject *Sender, TCloseAction &Action)
{
  Action = caFree;
}

void __fastcall TFigureDrawForm::FormClose2(TObject *Sender, bool &CanClose)
{
  FigureDrawForm = 0;
}

void __fastcall TFigureDrawForm::HeadBothEndsClick(TObject *Sender)
{
  switch( arrowType_ )
  {
    case 1:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image12 );
        PutImage( Image2b, Image23 );
        PutImage( Image3b, Image33 );
      }
      else
      {
        PutImage( Image1b, Image1  );
        PutImage( Image2b, Image21 );
        PutImage( Image3b, Image31 );
      }
      break;
    case 2:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image13 );
        PutImage( Image2b, Image22 );
        PutImage( Image3b, Image33 );
      }
      else
      {
        PutImage( Image1b, Image11 );
        PutImage( Image2b, Image2  );
        PutImage( Image3b, Image31 );
      }
      break;
    case 3:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image13 );
        PutImage( Image2b, Image23 );
        PutImage( Image3b, Image32 );
      }
      else
      {
        PutImage( Image1b, Image11 );
        PutImage( Image2b, Image21 );
        PutImage( Image3b, Image3  );
      }
      break;
  }
  MainGraphicsForm->SetHeadsBothEnds( HeadsCheckBox->Checked );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TFigureDrawForm::DrawEllipseClick(TObject *Sender)
{
  MainGraphicsForm->SetArrowPlacementMode( false );
  MainGraphicsForm->SetPolygonPlacementMode( false );
  MainGraphicsForm->SetEllipsePlacementMode( true );
  MainGraphicsForm->SetDrawCircles( CircleCheckBox->Checked );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TFigureDrawForm::DrawPolygonClick(TObject *Sender)
{
  double angle;
  try
  {
    angle = PolygonAngleEdit->Text.ToDouble();
  }
  catch (...)
  {
    Application->MessageBox("invalid angle","Error",MB_OK);
    return;
  }
  MainGraphicsForm->SetArrowPlacementMode( false );
  MainGraphicsForm->SetPolygonPlacementMode( true );
  MainGraphicsForm->SetEllipsePlacementMode( false );
  MainGraphicsForm->SetPolygonAngle( angle );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TFigureDrawForm::DrawArrowClick(TObject *Sender)
{
  MainGraphicsForm->SetArrowPlacementMode( true );
  MainGraphicsForm->SetPolygonPlacementMode( false );
  MainGraphicsForm->SetEllipsePlacementMode( false );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();

}

void __fastcall TFigureDrawForm::LineWidthClick(TObject *Sender)
{
  if( SelectLineWidthForm )
  {
    SelectLineWidthForm->WindowState = wsNormal;
    SelectLineWidthForm->Visible = true;
    SelectLineWidthForm->BringToFront();
  }
  else
  {
    SelectLineWidthForm = new TSelectLineWidthForm(MainGraphicsForm);
    SelectLineWidthForm->Show();
  }
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TFigureDrawForm::LineColorClick( TObject *Sender )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  // this flag needs to be set false, to indicate if a name,
  // color, etc. have been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TSelectColorForm> f( new TSelectColorForm(this) );
  f->Caption = "Select Line Color";
  f->NamedColorRadioGroup->Items->Insert(0,"no color");
  GRA_colorCharacteristic *colorChar =
    static_cast<GRA_colorCharacteristic*>(gw->GetDataCurveCharacteristics()->Get("CURVECOLOR"));
  f->SetColor( colorChar->IsVector() ? colorChar->Gets().front() : colorChar->Get() );
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )
  {
    GRA_color *fc = ExGlobals::GetWorkingColor();
    if( GRA_colorControl::GetColorCode(fc) == 0 )colorChar->Set( reinterpret_cast<GRA_color*>(0) );
    else colorChar->Set( fc );
  }
}

void __fastcall TFigureDrawForm::FillColorClick( TObject *Sender )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  // this flag needs to be set false, to indicate if a name,
  // color, etc. have been selected in the color selector form
  //
  ExGlobals::SetWorkingColorFlag( false );
  std::auto_ptr<TSelectColorForm> f( new TSelectColorForm(this) );
  f->Caption = "Select Fill Color";
  f->NamedColorRadioGroup->Items->Insert(0,"no color");
  GRA_colorCharacteristic *colorChar =
    static_cast<GRA_colorCharacteristic*>(gw->GetGeneralCharacteristics()->Get("AREAFILLCOLOR"));
  f->SetColor( colorChar->IsVector() ? colorChar->Gets().front() : colorChar->Get() );
  f->ShowModal();
  if( ExGlobals::GetWorkingColorFlag() )
  {
    GRA_color *fc = ExGlobals::GetWorkingColor();
    if( GRA_colorControl::GetColorCode(fc) == 0 )colorChar->Set( reinterpret_cast<GRA_color*>(0) );
    else colorChar->Set( fc );
  }
}

void __fastcall TFigureDrawForm::RectangleImageClick(TObject *Sender)
{
  switch( polygonType_ )
  {
    case 1:
      break;
    case 2:
      PutImage( Rectangle1bImage, RectangleImage );
      PutImage( Square1bImage, Square1Image  );
      break;
    case 3:
      PutImage( Rectangle1bImage, RectangleImage );
      PutImage( Star1bImage, Star1Image );
      break;
  }
  polygonType_ = 1;
  MainGraphicsForm->SetPolygonType( polygonType_ );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TFigureDrawForm::SquareImageClick(TObject *Sender)
{
  switch( polygonType_ )
  {
    case 1:
      PutImage( Rectangle1bImage, Rectangle1Image );
      PutImage( Square1bImage, SquareImage  );
      break;
    case 2:
      break;
    case 3:
      PutImage( Square1bImage, SquareImage  );
      PutImage( Star1bImage, Star1Image );
      break;
  }
  polygonType_ = 2;
  MainGraphicsForm->SetPolygonType( polygonType_ );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TFigureDrawForm::StarImageClick(TObject *Sender)
{
  switch( polygonType_ )
  {
    case 1:
      PutImage( Rectangle1bImage, Rectangle1Image );
      PutImage( Star1bImage, StarImage  );
      break;
    case 2:
      PutImage( Square1bImage, Square1Image );
      PutImage( Star1bImage, StarImage  );
      break;
    case 3:
      break;
  }
  polygonType_ = 3;
  MainGraphicsForm->SetPolygonType( polygonType_ );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

// end of file
