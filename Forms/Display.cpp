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
#include "extremaMain.h"

#include "Display.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TDisplayForm *DisplayForm;

__fastcall TDisplayForm::TDisplayForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "DisplayForm", "Top", 275 );
  this->Left= ini->ReadInteger( "DisplayForm", "Left", 489 );
  this->Width = ini->ReadInteger( "DisplayForm", "Width", 500 );
  delete ini;
}

__fastcall TDisplayForm::~TDisplayForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "DisplayForm", "Top", this->Top );
  ini->WriteInteger( "DisplayForm", "Left", this->Left );
  ini->WriteInteger( "DisplayForm", "Width", this->Width );
  delete ini;
  DisplayForm = 0;
}

void __fastcall TDisplayForm::CloseButtonClick(TObject *Sender)
{ Close(); }

void __fastcall TDisplayForm::FormClose(TObject *Sender, TCloseAction &Action)
{ Action = caFree; }

void __fastcall TDisplayForm::StopScriptsButtonClick(TObject *Sender)
{ ExGlobals::StopAllScripts(); Close(); }

void TDisplayForm::SetRichEdit( ExString &text, bool clear )
{
  if( clear )DisplayRichEdit->Lines->Clear();
  size_t n = text.find("\\n");
  while( n != text.npos() )
  {
    ExString s( text.substr(0,n) );
    DisplayRichEdit->Lines->Add( s.c_str() );
    text.erase( 0, n+2 );
    n = text.find("\\n");
  }
  DisplayRichEdit->Lines->Add( text.c_str() );
}

void SetUpDisplayForm( ExString &text, bool clear )
{
  if( DisplayForm )
  {
    DisplayForm->WindowState = wsNormal;
    DisplayForm->Visible = true;
    DisplayForm->BringToFront();
  }
  else
  {
    DisplayForm =  new TDisplayForm(dynamic_cast<TComponent*>(MainForm));;
    DisplayForm->Show();
  }
  DisplayForm->SetRichEdit( text, clear );
}

// end of file
