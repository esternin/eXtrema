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
#include "GRA_borlandW32.h"
#include "GRA_window.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_distanceCharacteristic.h"

#include "MainGraphicsWindow.h"

#include "GraphicsSubwindow.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TGraphicsSubwindowForm *GraphicsSubwindowForm;

__fastcall TGraphicsSubwindowForm::TGraphicsSubwindowForm(TComponent* Owner)
    : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "GraphicsSubwindowForm", "Top", 151 );
  this->Left = ini->ReadInteger( "GraphicsSubwindowForm", "Left", 347 );
  this->Height = ini->ReadInteger( "GraphicsSubwindowForm", "Height", 450 );
  this->Width = ini->ReadInteger( "GraphicsSubwindowForm", "Width", 414 );
  delete ini;
  //
  chosenWindow_ = ExGlobals::GetWindowNumber();
  FillStringGrid();
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
  GRA_distanceCharacteristic *xlowerwindow =
      static_cast<GRA_distanceCharacteristic*>(general->Get("XLOWERWINDOW"));
  GRA_distanceCharacteristic *ylowerwindow =
      static_cast<GRA_distanceCharacteristic*>(general->Get("YLOWERWINDOW"));
  GRA_distanceCharacteristic *xupperwindow =
      static_cast<GRA_distanceCharacteristic*>(general->Get("XUPPERWINDOW"));
  GRA_distanceCharacteristic *yupperwindow =
      static_cast<GRA_distanceCharacteristic*>(general->Get("YUPPERWINDOW"));
  double xlp = xlowerwindow->GetAsPercent();
  double ylp = ylowerwindow->GetAsPercent();
  double xup = xupperwindow->GetAsPercent();
  double yup = yupperwindow->GetAsPercent();
  WindowNumberEdit->Clear();
  WindowNumberEdit->Text = AnsiString( chosenWindow_ );
  xloEdit->Clear();
  xloEdit->Text = AnsiString( xlp );
  xupEdit->Clear();
  xupEdit->Text = AnsiString( xup );
  yloEdit->Clear();
  yloEdit->Text = AnsiString( ylp );
  yupEdit->Clear();
  yupEdit->Text = AnsiString( yup );
  NWHEdit->Clear();
  NWVEdit->Clear();
  TilingStartEdit->Clear();
  InheritEdit->Clear();
  DOWInheritEdit->Clear();
}

__fastcall TGraphicsSubwindowForm::~TGraphicsSubwindowForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "GraphicsSubwindowForm", "Top", this->Top );
  ini->WriteInteger( "GraphicsSubwindowForm", "Left", this->Left );
  ini->WriteInteger( "GraphicsSubwindowForm", "Height", this->Height );
  ini->WriteInteger( "GraphicsSubwindowForm", "Width", this->Width );
  delete ini;
}

void __fastcall TGraphicsSubwindowForm::RefreshStringGrid()
{
  for( std::size_t i=0; i<5; ++i )WindowCoordStringGrid->Cols[i]->Clear();
  FillStringGrid();
}

void __fastcall TGraphicsSubwindowForm::FillStringGrid()
{
  WindowCoordStringGrid->Cells[0][0] = "#";
  WindowCoordStringGrid->Cells[1][0] = "%xlo";
  WindowCoordStringGrid->Cells[2][0] = "%ylo";
  WindowCoordStringGrid->Cells[3][0] = "%xup";
  WindowCoordStringGrid->Cells[4][0] = "%yup";
  std::size_t size = ExGlobals::GetNumberOfWindows();
  WindowCoordStringGrid->RowCount = size + 1;
  for( std::size_t i=0; i<size; ++i )
  {
    WindowCoordStringGrid->Cells[0][i+1] = IntToStr(i);
    GRA_window *gw = ExGlobals::GetGraphWindow(i);
    GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
    GRA_distanceCharacteristic *xlowerwindow =
        static_cast<GRA_distanceCharacteristic*>(general->Get("XLOWERWINDOW"));
    GRA_distanceCharacteristic *ylowerwindow =
        static_cast<GRA_distanceCharacteristic*>(general->Get("YLOWERWINDOW"));
    GRA_distanceCharacteristic *xupperwindow =
        static_cast<GRA_distanceCharacteristic*>(general->Get("XUPPERWINDOW"));
    GRA_distanceCharacteristic *yupperwindow =
        static_cast<GRA_distanceCharacteristic*>(general->Get("YUPPERWINDOW"));
    double xlp = xlowerwindow->GetAsPercent();
    double ylp = ylowerwindow->GetAsPercent();
    double xup = xupperwindow->GetAsPercent();
    double yup = yupperwindow->GetAsPercent();
    WindowCoordStringGrid->Cells[1][i+1] = FloatToStr(xlp);
    WindowCoordStringGrid->Cells[2][i+1] = FloatToStr(ylp);
    WindowCoordStringGrid->Cells[3][i+1] = FloatToStr(xup);
    WindowCoordStringGrid->Cells[4][i+1] = FloatToStr(yup);
  }
  WindowCoordStringGrid->Repaint();
  WindowCoordStringGrid->Row = chosenWindow_ + 1;
}

void __fastcall TGraphicsSubwindowForm::InteractiveCheckBoxClicked(TObject *Sender)
{
  if( InteractiveCheckBox->Checked )
  {
    xloEdit->Enabled = false;
    xloLabel->Enabled = false;
    yloEdit->Enabled = false;
    yloLabel->Enabled = false;
    xupEdit->Enabled = false;
    xupLabel->Enabled = false;
    yupEdit->Enabled = false;
    yupLabel->Enabled = false;
  }
  else // interactive mode turned off
  {
    xloEdit->Enabled = true;
    xloLabel->Enabled = true;
    yloEdit->Enabled = true;
    yloLabel->Enabled = true;
    xupEdit->Enabled = true;
    xupLabel->Enabled = true;
    yupEdit->Enabled = true;
    yupLabel->Enabled = true;
  }
}

void __fastcall TGraphicsSubwindowForm::DOWApplyClick(TObject *Sender)
{
  AnsiString s( WindowNumberEdit->Text );
  if( s.IsEmpty() )
  {
    Application->MessageBox( "please enter a window number", "Warning", MB_OK );
    WindowNumberEdit->SetFocus();
    return;
  }
  int windowNumber = s.ToInt();
  if( windowNumber >= static_cast<int>(ExGlobals::GetNumberOfWindows()) )
  {
    int i = ExGlobals::GetNumberOfWindows() - 1;
    while ( ++i < windowNumber )
    {
      GRA_window *tmp = new GRA_window(i);
      ExGlobals::GetGraphWindows().push_back( tmp );
    }
  }
  chosenWindow_ = windowNumber;
  if( InteractiveCheckBox->Checked )
  {
    MainGraphicsForm->SetFocus();
    MainGraphicsForm->Show();
    MainGraphicsForm->Update();
    MainGraphicsForm->SetInteractiveWindowMode( true );
  }
  else
  {
    s = xloEdit->Text;
    if( s.IsEmpty() )
    {
      Application->MessageBox( "please enter a value for %xlo", "Warning", MB_OK );
      xloEdit->SetFocus();
      return;
    }
    double xlo = s.ToDouble();
    s = yloEdit->Text;
    if( s.IsEmpty() )
    {
      Application->MessageBox( "please enter a value for %ylo", "Warning", MB_OK );
      yloEdit->SetFocus();
      return;
    }
    double ylo = s.ToDouble();
    s = xupEdit->Text;
    if( s.IsEmpty() )
    {
      Application->MessageBox( "please enter a value for %xup", "Warning", MB_OK );
      xupEdit->SetFocus();
      return;
    }
    double xup = s.ToDouble();
    s = yupEdit->Text;
    if( s.IsEmpty() )
    {
      Application->MessageBox( "please enter a value for %yup", "Warning", MB_OK );
      yupEdit->SetFocus();
      return;
    }
    double yup = s.ToDouble();
    SetUp( xlo, ylo, xup, yup );
  }
}

void __fastcall TGraphicsSubwindowForm::SetUp( double lox, double loy, double upx, double upy )
{
  ExGlobals::AddGraphWindow( new GRA_window(chosenWindow_,lox,loy,upx,upy) );
  AnsiString s( DOWInheritEdit->Text );
  if( !s.IsEmpty() )
  {
    int i;
    try
    {
      i = s.ToInt();
    }
    catch (...)
    {
      s += AnsiString(" is not a valid integer value");
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      DOWInheritEdit->Clear();
      return;
    }
    GRA_window *targetWindow = ExGlobals::GetGraphWindow( chosenWindow_ );
    GRA_window *inheritWindow = ExGlobals::GetGraphWindow( i );
    targetWindow->InheritFrom( inheritWindow );
  }
  ExGlobals::SetWindowNumber( chosenWindow_ );
  RefreshStringGrid();
}

void __fastcall TGraphicsSubwindowForm::DMWApplyClick(TObject *Sender)
{
  AnsiString s( NWHEdit->Text );
  if( s.IsEmpty() )
  {
    Application->MessageBox( "please enter the number of windows horizontally", "Warning", MB_OK );
    NWHEdit->SetFocus();
    return;
  }
  int nx = s.ToInt();
  //
  s = NWVEdit->Text;
  if( s.IsEmpty() )
  {
    Application->MessageBox( "please enter the number of windows vertically", "Warning", MB_OK );
    NWVEdit->SetFocus();
    return;
  }
  int ny = s.ToInt();
  //
  s = TilingStartEdit->Text;
  if( s.IsEmpty() )
  {
    Application->MessageBox( "please enter the starting window number", "Warning", MB_OK );
    TilingStartEdit->SetFocus();
    return;
  }
  int start = s.ToInt();
  //
  double xinc = 100.0/nx;
  double yinc = 100.0/ny;
  for( int j=0; j<ny; ++j )
  {
    for( int i=0; i<nx; ++i )
    {
      int n = start+i+j*nx;
      double xl = i*xinc;
      double xu = (i+1)*xinc;
      double yl = 100.0-(j+1)*yinc;
      double yu = 100.0-j*yinc;
      ExGlobals::AddGraphWindow( new GRA_window(n,xl,yl,xu,yu) );
    }
  }
  chosenWindow_ = start;
  ExGlobals::SetWindowNumber( chosenWindow_ );
  RefreshStringGrid();
}

void __fastcall TGraphicsSubwindowForm::StringGridMouseUp( TObject *Sender, TMouseButton Button,
                                                           TShiftState Shift, int x, int y )
{
  int colm, row;
  WindowCoordStringGrid->MouseToCell( x, y, colm, row );
  if( row == 0 )return;
  chosenWindow_ = row-1;
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
  GRA_distanceCharacteristic *xlowerwindow =
      static_cast<GRA_distanceCharacteristic*>(general->Get("XLOWERWINDOW"));
  GRA_distanceCharacteristic *ylowerwindow =
      static_cast<GRA_distanceCharacteristic*>(general->Get("YLOWERWINDOW"));
  GRA_distanceCharacteristic *xupperwindow =
      static_cast<GRA_distanceCharacteristic*>(general->Get("XUPPERWINDOW"));
  GRA_distanceCharacteristic *yupperwindow =
      static_cast<GRA_distanceCharacteristic*>(general->Get("YUPPERWINDOW"));
  double xlp = xlowerwindow->GetAsPercent();
  double ylp = ylowerwindow->GetAsPercent();
  double xup = xupperwindow->GetAsPercent();
  double yup = yupperwindow->GetAsPercent();
  WindowNumberEdit->Clear();
  WindowNumberEdit->Text = AnsiString( chosenWindow_ );
  xloEdit->Clear();
  xloEdit->Text = AnsiString( xlp );
  yloEdit->Clear();
  yloEdit->Text = AnsiString( ylp );
  xupEdit->Clear();
  xupEdit->Text = AnsiString( xup );
  yupEdit->Clear();
  yupEdit->Text = AnsiString( yup );
  WindowCoordStringGrid->Row = chosenWindow_+1;
}

void __fastcall TGraphicsSubwindowForm::CloseClick(TObject *Sender)
{
  Close();
  GraphicsSubwindowForm = 0;
}

void __fastcall TGraphicsSubwindowForm::FormClose(TObject *Sender, TCloseAction &Action)
{ Action = caFree; }

void __fastcall TGraphicsSubwindowForm::FormClose2(TObject *Sender, bool &CanClose)
{ GraphicsSubwindowForm = 0; }

void __fastcall TGraphicsSubwindowForm::ApplyClick(TObject *Sender)
{
  AnsiString s( InheritEdit->Text );
  if( !s.IsEmpty() )
  {
    int i;
    try
    {
      i = s.ToInt();
    }
    catch (...)
    {
      s += AnsiString(" is not a valid integer value");
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      InheritEdit->Clear();
      return;
    }
    GRA_window *targetWindow = ExGlobals::GetGraphWindow( chosenWindow_ );
    GRA_window *inheritWindow = ExGlobals::GetGraphWindow( i );
    targetWindow->InheritFrom( inheritWindow );
  }
  ExGlobals::SetWindowNumber( chosenWindow_ );
}

void __fastcall TGraphicsSubwindowForm::WNEditExit(TObject *Sender)
{ GRA_borlandW32::TestEditExit( WindowNumberEdit, 0, "window number" ); }

void __fastcall TGraphicsSubwindowForm::xloEditExit(TObject *Sender)
{ GRA_borlandW32::TestEditExit( xloEdit, 0., 100., "%xlo" ); }

void __fastcall TGraphicsSubwindowForm::yloEditExit(TObject *Sender)
{ GRA_borlandW32::TestEditExit( yloEdit, 0., 100., "%ylo" ); }

void __fastcall TGraphicsSubwindowForm::xupEditExit(TObject *Sender)
{ GRA_borlandW32::TestEditExit( xupEdit, 0., 100., "%xup" ); }

void __fastcall TGraphicsSubwindowForm::yupEditExit(TObject *Sender)
{ GRA_borlandW32::TestEditExit( yupEdit, 0., 100., "%yup" ); }

void __fastcall TGraphicsSubwindowForm::NWHEditExit(TObject *Sender)
{ GRA_borlandW32::TestEditExit( NWHEdit, 1, "number of windows horizontally" ); }

void __fastcall TGraphicsSubwindowForm::NWVEditExit(TObject *Sender)
{ GRA_borlandW32::TestEditExit( NWVEdit, 1, "number of windows vertically" ); }

void __fastcall TGraphicsSubwindowForm::TilingStartEditExit(TObject *Sender)
{ GRA_borlandW32::TestEditExit( TilingStartEdit, 0, "window number" ); }

// end of file
