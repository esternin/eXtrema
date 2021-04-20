/*
Copyright (C) 2010 Joseph L. Chuma

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

#include "ThreeDPlot.h"
#include "GRA_threeDPlot.h"
#include "EVariableError.h"
#include "EGraphicsError.h"
#include "NVariableTable.h"
#include "NumericVariable.h"
#include "ExGlobals.h"
#include "GRA_window.h"
#include "ExString.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TThreeDPlotForm *ThreeDPlotForm;

__fastcall TThreeDPlotForm::TThreeDPlotForm( TComponent *Owner )
        : TForm(Owner)
{
  threeDPlot_ = 0;

  UpdateVectors( static_cast<TObject*>(XVecComboBox) );
  UpdateVectors( static_cast<TObject*>(YVecComboBox) );
  UpdateVectors( static_cast<TObject*>(ZVecComboBox) );

  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "3DPlotForm", "Top", 25 );
  Left = ini->ReadInteger( "3DPlotForm", "Left", 600 );

  XVecComboBox->ItemIndex = XVecComboBox->Items->IndexOf( ini->ReadString("3DPlotForm","XVector","") );
  YVecComboBox->ItemIndex = YVecComboBox->Items->IndexOf( ini->ReadString("3DPlotForm","YVector","") );
  ZVecComboBox->ItemIndex = ZVecComboBox->Items->IndexOf( ini->ReadString("3DPlotForm","ZVector","") );

  angleIncrement_ = ini->ReadFloat( "3DPlotForm", "AngleInc", 5. );
  AngleIncE->Text = AnsiString( angleIncrement_ );
  EyeE->Text = AnsiString( ini->ReadFloat("3DPlotForm","Eye2Obj",100.) );
}

__fastcall TThreeDPlotForm::~TThreeDPlotForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "3DPlotForm", "Top", Top );
  ini->WriteInteger( "3DPlotForm", "Left", Left );
  if( XVecComboBox->ItemIndex >= 0 )
    ini->WriteString( "3DPlotForm", "XVector", XVecComboBox->Items->Strings[XVecComboBox->ItemIndex] );
  if( YVecComboBox->ItemIndex >= 0 )
    ini->WriteString( "3DPlotForm", "YVector", YVecComboBox->Items->Strings[YVecComboBox->ItemIndex] );
  if( ZVecComboBox->ItemIndex >= 0 )
    ini->WriteString( "3DPlotForm", "ZVector", ZVecComboBox->Items->Strings[ZVecComboBox->ItemIndex] );
  ini->WriteFloat( "3DPlotForm", "AngleInc", AngleIncE->Text.ToDouble() );
  ini->WriteFloat( "3DPlotForm", "Eye2Obj", EyeE->Text.ToDouble() );
  delete ini;
}

void __fastcall TThreeDPlotForm::UpdateVectors( TObject *Sender )
{
  TComboBox *cBox = static_cast<TComboBox*>(Sender);
  cBox->Clear();
  NVariableTable *nvTable = NVariableTable::GetTable();
  int entries = nvTable->Entries();
  for( int i=0; i<entries; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry(i);
    if( nv->GetData().GetNumberOfDimensions() == 1 )cBox->Items->Add( nv->GetName().c_str() );
  }
  cBox->ItemIndex = -1;
}

void __fastcall TThreeDPlotForm::DrawClick( TObject *Sender )
{
  std::vector<double> x, y, z;
  try
  {
    GetDataVectors( x, y, z );
  }
  catch( EVariableError const &e )
  {
    Application->MessageBox( e.what(), "Error", MB_OK );
    return;
  }
  GRA_window *gw = ExGlobals::GetGraphWindow();
  gw->Clear();
  gw->Erase();
  threeDPlot_ = new GRA_threeDPlot( this, x, y, z );
  threeDPlot_->Draw( ExGlobals::GetScreenOutput() );
  ExGlobals::GetGraphWindow()->AddDrawableObject( threeDPlot_ );
}

void TThreeDPlotForm::GetDataVectors( std::vector<double> &x, std::vector<double> &y, std::vector<double> &z )
{
  ExString xVecName( XVecComboBox->Items->Strings[XVecComboBox->ItemIndex].c_str() );
  ExString yVecName( YVecComboBox->Items->Strings[YVecComboBox->ItemIndex].c_str() );
  ExString zVecName( ZVecComboBox->Items->Strings[ZVecComboBox->ItemIndex].c_str() );
  try
  {
    NumericVariable::GetVector( xVecName, "x variable", x );
    NumericVariable::GetVector( yVecName, "y variable", y );
    NumericVariable::GetVector( zVecName, "z variable", z );
  }
  catch( EVariableError const &e )
  {
    throw;
  }
  std::size_t npt = std::min( std::min(x.size(),y.size()), z.size() );
  x.resize( npt );
  y.resize( npt );
  z.resize( npt );
}

void __fastcall TThreeDPlotForm::CloseButtonClick( TObject *Sender )
{
  Close();
  ThreeDPlotForm = 0;
}

void TThreeDPlotForm::ReDraw()
{
  if( !threeDPlot_ )return;
  GRA_threeDPlot *tmp = new GRA_threeDPlot( *threeDPlot_ );
  GRA_window *gw = ExGlobals::GetGraphWindow();
  gw->Clear();
  gw->Erase();
  threeDPlot_ = tmp;
  threeDPlot_->Draw( ExGlobals::GetScreenOutput() );
  gw->AddDrawableObject( threeDPlot_ );
}

void __fastcall TThreeDPlotForm::AngleIncChange( TObject *Sender )
{
  if( AngleIncE->Text.IsEmpty() )return;
  double ai;
  try
  {
    ai = AngleIncE->Text.ToDouble();
  }
  catch( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for angle incrment", "Error", MB_OK );
    AngleIncE->Text = AnsiString(angleIncrement_);
    return;
  }
  angleIncrement_ = ai;
}

void __fastcall TThreeDPlotForm::Eye2ObjectChange( TObject *Sender )
{
  if( EyeE->Text.IsEmpty() )return;
  double e2o;
  try
  {
    e2o = EyeE->Text.ToDouble();
  }
  catch( EConvertError const &e )
  {
    Application->MessageBox( "invalid value entered for eye to object distance factor", "Error", MB_OK );
    EyeE->Text = AnsiString( 100. );
    return;
  }
  if( threeDPlot_ )
  {
    threeDPlot_->SetEye2Object( e2o );
    ReDraw();
  }
}

void __fastcall TThreeDPlotForm::RotateLeftClick( TObject *Sender )
{
  if( threeDPlot_ )
  {
    threeDPlot_->RotateY( -angleIncrement_ );
    ReDraw();
  }
}

void __fastcall TThreeDPlotForm::RotateRightClick( TObject *Sender )
{
  if( threeDPlot_ )
  {
    threeDPlot_->RotateY( angleIncrement_ );
    ReDraw();
  }
}

void __fastcall TThreeDPlotForm::RotateUpClick( TObject *Sender )
{
  if( threeDPlot_ )
  {
    threeDPlot_->RotateX( -angleIncrement_ );
    ReDraw();
  }
}

void __fastcall TThreeDPlotForm::RotateDownClick( TObject *Sender )
{
  if( threeDPlot_ )
  {
    threeDPlot_->RotateX( angleIncrement_ );
    ReDraw();
  }
}

void __fastcall TThreeDPlotForm::ZoomInClick( TObject *Sender )
{
  if( threeDPlot_ )
  {
    threeDPlot_->ScaleDS( 1.05 );
    ReDraw();
  }
}

void __fastcall TThreeDPlotForm::ZoomOutClick( TObject *Sender )
{
  if( threeDPlot_ )
  {
    threeDPlot_->ScaleDS( 0.95 );
    ReDraw();
  }
}

