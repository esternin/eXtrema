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

#include "SetAspectRatio.h"
#include "ExGlobals.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TSetAspectRatioForm *SetAspectRatioForm;

__fastcall TSetAspectRatioForm::TSetAspectRatioForm( TComponent* Owner )
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "AspectRatioForm", "Top", 400 );
  Left = ini->ReadInteger( "AspectRatioForm", "Left", 700 );
  Width = ini->ReadInteger( "AspectRatioForm", "Width", 240 );
  Height = ini->ReadInteger( "AspectRatioForm", "Height", 190 );
  delete ini;
  //
  double ar = ExGlobals::GetAspectRatio();
  if( ar > 1.0 )
  {
    HeightEdit->Text = AnsiString(ar);
    WidthEdit->Text = AnsiString(1);
  }
  else
  {
    HeightEdit->Text = AnsiString(1);
    WidthEdit->Text = AnsiString(1.0/ar);
  }
}

__fastcall TSetAspectRatioForm::~TSetAspectRatioForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->EraseSection( "AspectRatioForm" );
  ini->WriteInteger( "AspectRatioForm", "Top", Top );
  ini->WriteInteger( "AspectRatioForm", "Left", Left );
  ini->WriteInteger( "AspectRatioForm", "Width", Width );
  ini->WriteInteger( "AspectRatioForm", "Height", Height );
  delete ini;
}

void __fastcall TSetAspectRatioForm::OKClick( TObject *Sender )
{
  double height, width;
  if( !GetValues(height,width) )return;
  ExGlobals::ChangeAspectRatio( height/width );
  Close();
  SetAspectRatioForm = 0;
}

void __fastcall TSetAspectRatioForm::CancelClick( TObject *Sender )
{
  Close();
  SetAspectRatioForm = 0;
}

bool TSetAspectRatioForm::GetValues( double &height, double &width )
{
  if( HeightEdit->Text.IsEmpty() )
  {
    Application->MessageBox("Please enter a height value","ERROR",MB_OK);
    return false;
  }
  try
  {
    height = HeightEdit->Text.ToDouble();
  }
  catch ( EConvertError &e )
  {
    Application->MessageBox("Invalid number entered as height","ERROR",MB_OK);
    HeightEdit->Text = AnsiString("");
    return false;
  }
  if( height <= 0.0 )
  {
    Application->MessageBox("Height must be > 0","ERROR",MB_OK);
    HeightEdit->Text = AnsiString("");
    return false;
  }
  if( WidthEdit->Text.IsEmpty() )
  {
    Application->MessageBox("Please enter a width value","ERROR",MB_OK);
    return false;
  }
  try
  {
    width = WidthEdit->Text.ToDouble();
  }
  catch (EConvertError &e)
  {
    Application->MessageBox("Invalid number entered as width","ERROR",MB_OK);
    WidthEdit->Text = AnsiString("");
    return false;
  }
  if( width <= 0.0 )
  {
    Application->MessageBox("Width must be > 0","ERROR",MB_OK);
    WidthEdit->Text = AnsiString("");
    return false;
  }
  return true;
}

// end of file
