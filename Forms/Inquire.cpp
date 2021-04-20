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
#include "ExString.h"
#include "ECommandError.h"
#include "CMD_inquire.h"

#include "Inquire.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TInquireForm *InquireForm;

__fastcall TInquireForm::TInquireForm( TComponent* Owner )
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt(Application->ExeName,".INI") );
  Top = ini->ReadInteger( "InquireForm", "Top", 359 );
  Left = ini->ReadInteger( "InquireForm", "Left", 449 );
  Height = ini->ReadInteger( "InquireForm", "Height", 146 );
  Width = ini->ReadInteger( "InquireForm", "Width", 528 );
  delete ini;
}

__fastcall TInquireForm::~TInquireForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "InquireForm", "Top", Top );
  ini->WriteInteger( "InquireForm", "Left", Left );
  ini->WriteInteger( "InquireForm", "Height", Height );
  ini->WriteInteger( "InquireForm", "Width", Width );
  delete ini;
}

void TInquireForm::SetLabel( ExString const &label )
{
  InquireLabel->Caption = AnsiString( label.c_str() );
  InquireEdit->Text = AnsiString();
}

void __fastcall TInquireForm::OKButtonClick( TObject *Sender )
{ ApplyIt(); }

void __fastcall TInquireForm::ReturnKeyPress(TObject *Sender, char &Key)
{ if( Key == VK_RETURN )ApplyIt(); }

void __fastcall TInquireForm::ApplyIt()
{
  try
  {
    CMD_inquire::Definition()->ProcessString( InquireEdit->Text.c_str() );
  }
  catch (ECommandError &e)
  {
    Application->MessageBox( e.what(), "Fatal Error: all scripts are stopped", MB_OK );
    ExGlobals::StopAllScripts();
  }
  Close();
}

void __fastcall TInquireForm::StopScriptsButtonClick( TObject *Sender )
{
  ExGlobals::StopAllScripts();
  Close();
}

void __fastcall TInquireForm::FormClose(TObject *Sender,TCloseAction &Action)
{ Action = caFree; }

// end of file
