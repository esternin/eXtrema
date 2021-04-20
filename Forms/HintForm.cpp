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

#include <vcl.h>
#pragma hdrstop

#include "HintForm.h"
#include "ExString.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TMainHintForm *MainHintForm;

__fastcall TMainHintForm::TMainHintForm( TComponent* Owner ) : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ClientWidth = ini->ReadInteger( "MainHintForm", "ClientWidth", 460 );
  ClientHeight = ini->ReadInteger( "MainHintForm", "ClientHeight", 60 );
  Top = ini->ReadInteger( "MainHintForm", "Top", (Screen->Width-ClientWidth)/2 );
  Left = ini->ReadInteger( "MainHintForm", "Left", (Screen->Height-ClientHeight)/2 );
  delete ini;
}

void TMainHintForm::ActivateHint( std::vector<ExString> &lines )
{
  MessageRichEdit->Lines->Clear();
  for( std::size_t i=0; i<lines.size(); ++i )MessageRichEdit->Lines->Add( lines[i].c_str() );
  Show();
}

void TMainHintForm::HideIt()
{
  Hide();
}

__fastcall TMainHintForm::~TMainHintForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "MainHintForm", "Top", Top );
  ini->WriteInteger( "MainHintForm", "Left", Left );
  ini->WriteInteger( "MainHintForm", "ClientWidth", ClientWidth );
  ini->WriteInteger( "MainHintForm", "ClientHeight", ClientHeight );
  delete ini;
}

void __fastcall TMainHintForm::FormClose( TObject *Sender, TCloseAction &Action )
{
  Action = caFree;
}

// end of file

