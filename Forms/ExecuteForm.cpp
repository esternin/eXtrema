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

#include "ExecuteForm.h"
#include "ExString.h"
#include "ExGlobals.h"
#include "extremaMain.h"
#include "ParseLine.h"
#include "CMD_execute.h"
#include "ESyntaxError.h"
#include "ECommandError.h"
#include "Script.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TExecForm *ExecForm;

__fastcall TExecForm::TExecForm( TComponent* Owner ) : TForm( Owner )
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "ExecForm", "Top", 103 );
  Left = ini->ReadInteger( "ExecForm", "Left", 632 );
  Width = ini->ReadInteger( "ExecForm", "Width", 463 );
  AnsiString defaultPath( ExGlobals::GetExecutablePath().c_str() );
  defaultPath += AnsiString("\Scripts");
  currentPath_ = ini->ReadString( "ExecForm", "Path", defaultPath );
  int count = ini->ReadInteger( "ExecForm", "FileCount", 0 );
  int currentFile = ini->ReadInteger( "ExecForm", "CurrentFile", 0 );
  for( int i=0; i<count; ++i )
  {
    AnsiString ident( "file" );
    ident += AnsiString(i);
    FilenameComboBox->Items->Add( ini->ReadString("ExecForm",ident,"") );
  }
  FilenameComboBox->ItemIndex = currentFile;
  ExecuteDialog->InitialDir = currentPath_;
  ParameterEdit->Text = ini->ReadString( "ExecForm", "Parameters", "" );
  delete ini;
}

__fastcall TExecForm::~TExecForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->EraseSection( "ExecForm" );
  ini->WriteInteger( "ExecForm", "Top", Top );
  ini->WriteInteger( "ExecForm", "Left", Left );
  ini->WriteInteger( "ExecForm", "Width", Width );
  ini->WriteString( "ExecForm", "Path", currentPath_ );
  ini->WriteInteger( "ExecForm", "CurrentFile", FilenameComboBox->ItemIndex );
  int count = FilenameComboBox->Items->Count;
  if( count == 0 )
  {
    int oldCount = ini->ReadInteger( "ExecForm", "FileCount", 0 );
    for( int i=0; i<oldCount; ++i )
      ini->DeleteKey( "ExecForm", AnsiString("file")+AnsiString(i) );
  }
  ini->WriteString( "ExecForm", "FileCount", count );
  for( int i=0; i<count; ++i )
    ini->WriteString( "ExecForm", AnsiString("file")+AnsiString(i),
                      FilenameComboBox->Items->Strings[i] );
  if( !ParameterEdit->Text.IsEmpty() )
    ini->WriteString( "ExecForm", "Parameters", ParameterEdit->Text );
  delete ini;
}

void __fastcall TExecForm::ChooseFile( TObject *Sender )
{
  if( ExecuteDialog->Execute() )SetFile( ExecuteDialog->FileName );
}

void TExecForm::SetFile( AnsiString fileName )
{
  if( fileName.IsEmpty() )return;
  std::ifstream f( fileName.c_str() );
  if( !f.is_open() )
  {
    std::string s( "could not open " );
    s += std::string(fileName.c_str());
    Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
    return;
  }
  int count = FilenameComboBox->Items->Count;
  //
  // filename is already in the list
  // exchange names so current filename is at the top
  //
  for( int i=0; i<count; ++i )
  {
    if( fileName == FilenameComboBox->Items->Strings[i] )
    {
      AnsiString temp( FilenameComboBox->Items->Strings[0] );
      FilenameComboBox->Items->Strings[0] = fileName;
      FilenameComboBox->Items->Strings[i] = temp;
      FilenameComboBox->ItemIndex = 0;
      f.close();
      return;
    }
  }
  //
  // shift all names down 1 and put current name at the top
  //
  if( count >= 1 )
  {
    AnsiString lastName = FilenameComboBox->Items->Strings[count-1];
    FilenameComboBox->Items->Add( lastName );
    for( int i=count-2; i>=0; --i )
      FilenameComboBox->Items->Strings[i+1] = FilenameComboBox->Items->Strings[i];
    FilenameComboBox->Items->Strings[0] = fileName;
  }
  else
  {
    FilenameComboBox->Items->Add( fileName );
  }
  FilenameComboBox->ItemIndex = 0;
  f.close();
}

void __fastcall TExecForm::ReturnKeyPress( TObject *Sender, char &Key )
{
  if( Key == VK_RETURN )ApplyClick( static_cast<TObject*>(0) );
}

void __fastcall TExecForm::ApplyClick( TObject *Sender )
{
  if( FilenameComboBox->Text.IsEmpty() )
  {
    Application->MessageBox( "no file has been chosen", "Warning", MB_OK );
    return;
  }
  ExString fileName( FilenameComboBox->Text.c_str() );
  currentPath_ = ExtractFilePath(FilenameComboBox->Text);
  ExGlobals::SetCurrentPath( ExString(currentPath_.c_str()) );
  ExString commandLine( "EXECUTE '" );
  commandLine += fileName + ExString("'");
  if( !ParameterEdit->Text.IsEmpty() )
   commandLine += ExString(" ") + ExString(ParameterEdit->Text.c_str());
  ParseLine p( commandLine );
  try
  {
    p.ParseIt();
  }
  catch( ESyntaxError &e )
  {
    Application->MessageBox( e.what(), "Fatal Error", MB_OK );
    return;
  }
  try
  {
    CMD_execute::Definition()->Execute( &p );
  }
  catch( ECommandError &e )
  {
    Application->MessageBox( e.what(), "Fatal Error", MB_OK );
    return;
  }
  if( ExGlobals::StackIsOn() )
  {
    ExString line( "FORM EXECUTE,'" );
    line += fileName;
    ExString p( ParameterEdit->Text.c_str() );
    if( p.empty() )line += "',,APPLY";
    else line += "','" + ExString(ParameterEdit->Text.c_str()) + "',APPLY";
    ExGlobals::WriteStack( line );
  }
  // this must be the top level script since it is run interactively
  //
  try
  {
    ExGlobals::RunScript();
  }
  catch ( runtime_error &e )
  {
    ExGlobals::ShowScriptError( e.what() );
    ExGlobals::StopAllScripts();
  }
}

void __fastcall TExecForm::ClearFileList( TObject *Sender )
{ FilenameComboBox->Clear(); }

void __fastcall TExecForm::CloseClick( TObject *Sender )
{
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( "FORM EXECUTE,CLOSE" );
  Close();
  ExecForm = NULL;
}

void __fastcall TExecForm::FormClose( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TExecForm::FormClose2( TObject *Sender, bool &CanClose )
{ ExecForm = NULL; }

void __fastcall TExecForm::RemoveFilename( TObject *Sender )
{
  if( FilenameComboBox->Text.IsEmpty() )return;
  int i = FilenameComboBox->Items->IndexOf( FilenameComboBox->Text );
  FilenameComboBox->Items->Delete(i);
  if( FilenameComboBox->Items->Count == 0 )FilenameComboBox->Text = AnsiString("");
  else                                     FilenameComboBox->ItemIndex = 0;
}

void __fastcall TExecForm::DeleteFilename( TObject *Sender, WORD &Key, TShiftState Shift )
{
  if( Key != VK_DELETE )return;
  if( FilenameComboBox->Text.IsEmpty() )return;
  int i = FilenameComboBox->Items->IndexOf( FilenameComboBox->Text );
  FilenameComboBox->Items->Delete(i);
  if( FilenameComboBox->Items->Count == 0 )FilenameComboBox->Text = AnsiString("");
  else                                     FilenameComboBox->ItemIndex = 0;
}

void TExecForm::Set( ExString const &filename, ExString const &parameters,
                     ExString const &action )
{
  SetFile( AnsiString(filename.c_str()) );
  ParameterEdit->Text = AnsiString( parameters.c_str() );
  if( action == "CLOSE" )
  {
    CloseClick( static_cast<TObject*>(0) );
  }
  else if( action == "APPLY" )
  {
    ApplyClick( static_cast<TObject*>(0) );
  }
}

// end of file

