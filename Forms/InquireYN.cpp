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
#include "CMD_inquire.h"
#include "ECommandError.h"

#include "InquireYN.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TInquireYNForm *InquireYNForm;

__fastcall TInquireYNForm::TInquireYNForm( TComponent* Owner )
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "InquireYNForm", "Top", 504 );
  this->Left = ini->ReadInteger( "InquireYNForm", "Left", 452 );
  this->Height = ini->ReadInteger( "InquireYNForm", "Height", 119 );
  this->Width = ini->ReadInteger( "InquireYNForm", "Width", 553 );
  delete ini;
}

__fastcall TInquireYNForm::~TInquireYNForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "InquireYNForm", "Top", this->Top );
  ini->WriteInteger( "InquireYNForm", "Left", this->Left );
  ini->WriteInteger( "InquireYNForm", "Height", this->Height );
  ini->WriteInteger( "InquireYNForm", "Width", this->Width );
  delete ini;
}

void TInquireYNForm::SetLabel( ExString const &label )
{ InquireLabel->Caption = AnsiString( label.c_str() ); }

void __fastcall TInquireYNForm::StopScriptsButtonClick(TObject *Sender)
{
  ExGlobals::StopAllScripts();
  Close();
}

void __fastcall TInquireYNForm::YesButtonClick(TObject *Sender)
{
  try
  {
    CMD_inquire::Definition()->ProcessYN( "YES" );
  }
  catch (ECommandError &e)
  {
    Application->MessageBox( e.what(), "Fatal Error", MB_OK );
    ExGlobals::StopAllScripts();
  }
  Close();
}

void __fastcall TInquireYNForm::NoButtonClick(TObject *Sender)
{
  try
  {
    CMD_inquire::Definition()->ProcessYN( "NO" );
  }
  catch (ECommandError &e)
  {
    Application->MessageBox( e.what(), "Fatal Error", MB_OK );
    ExGlobals::StopAllScripts();
  }
  Close();
}

void __fastcall TInquireYNForm::FormClose(TObject *Sender,TCloseAction &Action)
{ Action = caFree; }

// end of file
