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

#include "StackCommandsForm.h"
#include "ExGlobals.h"
#include "ExString.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TStackForm *StackForm;

__fastcall TStackForm::TStackForm( TComponent* Owner ) : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "StackForm", "Top", 90 );
  Left = ini->ReadInteger( "StackForm", "Left", 740 );
  Height = ini->ReadInteger( "StackForm", "Height", 190 );
  Width = ini->ReadInteger( "StackForm", "Width", 425 );
  int count = ini->ReadInteger( "StackForm", "FileCount", 0 );
  int currentFile = ini->ReadInteger( "StackForm", "CurrentFile", 0 );
  for( int i=0; i<count; ++i )
  {
    AnsiString ident( "file" );
    ident += AnsiString(i);
    FilenameComboBox->Items->Add( ini->ReadString("StackForm",ident,"") );
  }
  FilenameComboBox->ItemIndex = currentFile;
  delete ini;
  //
  if( ExGlobals::StackIsOn() )
  {
    OnOffRadioGroup->ItemIndex = 0;
    AddFileToList( AnsiString(ExGlobals::GetStackFile().c_str()) );
    BrowseButton->Enabled = true;
    FilenameComboBox->Enabled = true;
    EmptyListSpeedButton->Enabled = true;
    RemoveFileSpeedButton->Enabled = true;
    AppendCheckBox->Enabled = true;
  }
  else
  {
    OnOffRadioGroup->ItemIndex = 1;
    BrowseButton->Enabled = false;
    FilenameComboBox->Enabled = false;
    EmptyListSpeedButton->Enabled = false;
    RemoveFileSpeedButton->Enabled = false;
    AppendCheckBox->Enabled = false;
  }
}

__fastcall TStackForm::~TStackForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "StackForm", "Top", this->Top );
  ini->WriteInteger( "StackForm", "Left", this->Left );
  ini->WriteInteger( "StackForm", "Height", this->Height );
  ini->WriteInteger( "StackForm", "Width", this->Width );
  int count = FilenameComboBox->Items->Count;
  if( count == 0 )
  {
    int oldCount = ini->ReadInteger( "StackForm", "FileCount", 0 );
    for( int i=0; i<oldCount; ++i )
    {
      AnsiString ident( "file" );
      ident += AnsiString(i);
      ini->DeleteKey( "StackForm", ident );
    }
  }
  ini->WriteString( "StackForm", "FileCount", count );
  for( int i=0; i<count; ++i )
  {
    AnsiString ident( "file" );
    ident += AnsiString(i);
    ini->WriteString( "StackForm", ident, FilenameComboBox->Items->Strings[i] );
  }
  delete ini;
}

void __fastcall TStackForm::CloseButtonClick( TObject *Sender )
{
  Close();
  StackForm = 0;
}

void __fastcall TStackForm::ToggleOnOff( TObject *Sender )
{
  if( OnOffRadioGroup->ItemIndex == 1 )
  {
    BrowseButton->Enabled = false;
    FilenameComboBox->Enabled = false;
    EmptyListSpeedButton->Enabled = false;
    RemoveFileSpeedButton->Enabled = false;
    AppendCheckBox->Enabled = false;
  }
  else
  {
    BrowseButton->Enabled = true;
    FilenameComboBox->Enabled = true;
    EmptyListSpeedButton->Enabled = true;
    RemoveFileSpeedButton->Enabled = true;
    AppendCheckBox->Enabled = true;
  }
}

void __fastcall TStackForm::ApplyButtonClick( TObject *Sender )
{
  if( OnOffRadioGroup->ItemIndex == 1 )
  {
    ExGlobals::SetStackOff();
  }
  else
  {
    if( FilenameComboBox->Text.IsEmpty() )
    {
      Application->MessageBox( "no file was chosen", "Warning", MB_OK );
      return;
    }
    ExString filename( FilenameComboBox->Text.c_str() );
    if( !ExGlobals::SetStackOn(filename,AppendCheckBox->Checked) )
    {
      ExString s = ExString("could not open ") + filename;
      Application->MessageBox( s.c_str(), "Warning", MB_OK );
      return;
    }
  }
}

void __fastcall TStackForm::BrowseButtonClick( TObject *Sender )
{
  if( StackDialog->Execute() )AddFileToList( StackDialog->FileName );
}

void TStackForm::AddFileToList( AnsiString filename )
{
  int count = FilenameComboBox->Items->Count;
  //
  for( int i=0; i<count; ++i )
  {
    if( filename == FilenameComboBox->Items->Strings[i] ) // filename is already in the list
    {
      FilenameComboBox->ItemIndex = i;
      return;
    }
  }
  if( count >= 1 ) // shift all names down 1 and put current name at the top
  {
    AnsiString lastName = FilenameComboBox->Items->Strings[count-1];
    FilenameComboBox->Items->Add( lastName );
    for( int i=count-2; i>=0; --i )
      FilenameComboBox->Items->Strings[i+1] = FilenameComboBox->Items->Strings[i];
    FilenameComboBox->Items->Strings[0] = filename;
    FilenameComboBox->ItemIndex = 0;
  }
  else
  {
    FilenameComboBox->Items->Add( filename );
    FilenameComboBox->ItemIndex = 0;
  }
}

void __fastcall TStackForm::FormClose( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TStackForm::FormClose2( TObject *Sender, bool &CanClose )
{ StackForm = 0; }

void __fastcall TStackForm::ClearFileList( TObject *Sender )
{
  FilenameComboBox->Clear();
}

void __fastcall TStackForm::RemoveFilename( TObject *Sender )
{
  if( FilenameComboBox->Text.IsEmpty() )return;
  int i = FilenameComboBox->Items->IndexOf( FilenameComboBox->Text );
  FilenameComboBox->Items->Delete(i);
  if( FilenameComboBox->Items->Count == 0 )FilenameComboBox->Text = AnsiString("");
  else                                     FilenameComboBox->ItemIndex = 0;
}

// end of file

