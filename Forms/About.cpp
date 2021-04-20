/*
Copyright (C) 2005,...,2009 Joseph L. Chuma

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

#include "About.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TAboutBox *AboutBox;

__fastcall TAboutBox::TAboutBox( TComponent* Owner ) : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "AboutBox", "Top", 347 );
  this->Left = ini->ReadInteger( "AboutBox", "Left", 539 );
  delete ini;
}

__fastcall TAboutBox::~TAboutBox()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "AboutBox", "Top", this->Top );
  ini->WriteInteger( "AboutBox", "Left", this->Left );
  delete ini;
}

void __fastcall TAboutBox::WebClick( TObject *Sender )
{ ShellExecute( 0,"open",WebLabel->Caption.c_str(),"","",SW_SHOWNORMAL ); }

//---------------------------------------------------------------------------
