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
#include "NumericVariable.h"
#include "NVariableTable.h"
#include "CMD_write.h"

#include "WriteMatrixForm.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TWriteMatForm *WriteMatForm;

__fastcall TWriteMatForm::TWriteMatForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "WriteMatForm", "Top", 236 );
  this->Left = ini->ReadInteger( "WriteMatForm", "Left", 546 );
  this->Height = ini->ReadInteger( "WriteMatForm", "Height", 280 );
  this->Width = ini->ReadInteger( "WriteMatForm", "Width", 323 );
  delete ini;
  //
  FillOutListBox();
}

__fastcall TWriteMatForm::~TWriteMatForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "WriteMatForm", "Top", this->Top );
  ini->WriteInteger( "WriteMatForm", "Left", this->Left );
  ini->WriteInteger( "WriteMatForm", "Height", this->Height );
  ini->WriteInteger( "WriteMatForm", "Width", this->Width );
  delete ini;
}

void __fastcall TWriteMatForm::FillOutListBox()
{
  NamesCheckListBox->Clear();
  NVariableTable *nvTable = NVariableTable::GetTable();
  int cntr = 0;
  int end = nvTable->Entries();
  for( int i=0; i<end; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry( i );
    if( nv->GetData().GetNumberOfDimensions() == 2 )
    {
      ++cntr;
      NamesCheckListBox->Items->Add( AnsiString(nv->GetName().c_str()) );
      NamesCheckListBox->Checked[cntr-1] = false;
    }
  }
  if( cntr == 0 )Application->MessageBox( "there are no matrix variables", "Warning", MB_OK );
}

void __fastcall TWriteMatForm::ApplyClick(TObject *Sender)
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
      Application->MessageBox( "only one matrix can be written at a time", "Warning", MB_OK );
      return;
    }
    nameChecked = true;
    iName = i;
  }
  if( !nameChecked )
  {
    outStream->close();
    Application->MessageBox( "no matrix was chosen", "Warning", MB_OK );
    return;
  }
  std::vector<double> data;
  int ndm;
  double value;
  int dimSizes[3];
  ExString name( NamesCheckListBox->Items->Strings[iName].c_str() );
  try
  {
    NumericVariable::GetVariable( name, ndm, value, data, dimSizes );
  }
  catch( EVariableError &e )
  {
    outStream->close();
    Application->MessageBox( e.what(), "Fatal Error", MB_OK );
    return;
  }
  for( int i=0; i<dimSizes[0]; ++i )
  {
    for( int j=0; j<dimSizes[1]; ++j )(*outStream) << data[i+j*dimSizes[0]] << " ";
    (*outStream) << "\n";
  }
  outStream->close();
}

void __fastcall TWriteMatForm::ChooseFileClick(TObject *Sender)
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
      if( FilenameComboBox->Text == FilenameComboBox->Items->Strings[i] )
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

void __fastcall TWriteMatForm::UpdateClick(TObject *Sender)
{ FillOutListBox(); }

void __fastcall TWriteMatForm::CloseClick(TObject *Sender)
{
  Close();
  WriteMatForm = 0;
}

void __fastcall TWriteMatForm::FormClose(TObject *Sender,TCloseAction &Action)
{ Action = caFree; }

void __fastcall TWriteMatForm::FormClose2(TObject *Sender, bool &CanClose)
{ WriteMatForm = 0; }

// end of file
