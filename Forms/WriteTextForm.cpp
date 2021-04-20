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
#include "EVariableError.h"
#include "TVariableTable.h"
#include "TextVariable.h"
#include "CMD_write.h"

#include "WriteTextForm.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TWriteTxtForm *WriteTxtForm;

__fastcall TWriteTxtForm::TWriteTxtForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "WriteTxtForm", "Top", 326 );
  this->Left = ini->ReadInteger( "WriteTxtForm", "Left", 543 );
  this->Height = ini->ReadInteger( "WriteTxtForm", "Height", 244 );
  this->Width = ini->ReadInteger( "WriteTxtForm", "Width", 320 );
  delete ini;
  //
  FillOutListBox();
}

__fastcall TWriteTxtForm::~TWriteTxtForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "WriteTxtForm", "Top", this->Top );
  ini->WriteInteger( "WriteTxtForm", "Left", this->Left );
  ini->WriteInteger( "WriteTxtForm", "Height", this->Height );
  ini->WriteInteger( "WriteTxtForm", "Width", this->Width );
  delete ini;
}

void __fastcall TWriteTxtForm::FillOutListBox()
{
  NamesCheckListBox->Clear();
  TVariableTable *tvTable = TVariableTable::GetTable();
  int cntr = 0;
  int end = tvTable->Entries();
  for( int i=0; i<end; ++i )
  {
    ++cntr;
    TextVariable *tv = tvTable->GetEntry( i );
    TextData td( tv->GetData() );
    AnsiString name( tv->GetName().c_str() );
    if( td.GetNumberOfDimensions() == 0 )
    {
      NamesCheckListBox->Items->Add( name );
      NamesCheckListBox->Checked[cntr-1] = false;
    }
    else // tv is a vector
    {
      std::size_t len = td.GetData().size();
      for( std::size_t j=0; j<len; ++j )
      {
        NamesCheckListBox->Items->Add( name+"["+(j+1)+"]" );
        NamesCheckListBox->Checked[cntr-1] = false;
      }
    }
  }
  if( cntr == 0 )Application->MessageBox( "there are no string variables", "Warning", MB_OK );
}

void __fastcall TWriteTxtForm::ChooseFileClick(TObject *Sender)
{
  if( WriteDialog->Execute() )
  {
    FilenameComboBox->Text = WriteDialog->FileName;
    int count = FilenameComboBox->Items->Count;
    //
    // filename is already in the list
    // exchange names to current filename is at the top
    //
    for( int i=1; i<count; ++i )
    {
      if( AnsiString(CMD_write::Definition()->GetFilename().c_str()) ==
          FilenameComboBox->Items->Strings[i] )
      {
        AnsiString temp( FilenameComboBox->Items->Strings[0] );
        FilenameComboBox->Items->Strings[0] = FilenameComboBox->Text;
        FilenameComboBox->Items->Strings[i] = temp;
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
      FilenameComboBox->Items->Strings[0] = FilenameComboBox->Text;
    }
    else
      FilenameComboBox->Items->Add( FilenameComboBox->Text );
  }
}

void __fastcall TWriteTxtForm::ApplyClick(TObject *Sender)
{
  if( FilenameComboBox->Text.IsEmpty() )
  {
    Application->MessageBox( "no file was chosen", "Warning", MB_OK );
    return;
  }
  CMD_write *writeCommand = CMD_write::Definition();
  writeCommand->SetFilename( ExString(FilenameComboBox->Text.c_str()) );
  std::ofstream *outStream = writeCommand->GetStream();
  if( outStream->is_open() )outStream->close();
  outStream->clear( std::ios::goodbit );
  if( AppendCheckBox->Checked )outStream->open( writeCommand->GetFilename().c_str(), std::ios::out|std::ios::app );
  else                         outStream->open( writeCommand->GetFilename().c_str(), std::ios::out );
  if( !outStream->is_open() )
  {
    ExString s( "could not open " );
    s += writeCommand->GetFilename();
    Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
    return;
  }
  bool nameChecked = false;
  int iName;
  for( int i=0; i<NamesCheckListBox->Items->Count; ++i )
  {
    if( !NamesCheckListBox->Checked[i] )continue;
    if( nameChecked )
    {
      outStream->close();
      Application->MessageBox("only one string variable can be written at a time","Warning",MB_OK);
      return;
    }
    nameChecked = true;
    iName = i;
  }
  if( !nameChecked )
  {
    outStream->close();
    Application->MessageBox( "no string variable was chosen", "Warning", MB_OK );
    return;
  }
  ExString name( NamesCheckListBox->Items->Strings[iName].c_str() );
  ExString line;
  try
  {
    TextVariable::GetVariable( name, false, line );
  }
  catch( EVariableError &e )
  {
    outStream->close();
    Application->MessageBox( e.what(), "Fatal Error", MB_OK );
    return;
  }
  (*outStream) << line.c_str() << "\n";
  outStream->close();
}

void __fastcall TWriteTxtForm::UpdateClick(TObject *Sender)
{ FillOutListBox(); }

void __fastcall TWriteTxtForm::CloseClick(TObject *Sender)
{
  Close();
  WriteTxtForm = 0;
}

void __fastcall TWriteTxtForm::FormClose(TObject *Sender,TCloseAction &Action)
{ Action = caFree; }

void __fastcall TWriteTxtForm::FormClose2(TObject *Sender,bool &CanClose)
{ WriteTxtForm = 0; }

// end of file
