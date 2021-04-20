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

#include "WriteScalarsForm.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TWriteScalForm *WriteScalForm;

__fastcall TWriteScalForm::TWriteScalForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "WriteScalForm", "Top", 229 );
  this->Left = ini->ReadInteger( "WriteScalForm", "Left", 560 );
  this->Height = ini->ReadInteger( "WriteScalForm", "Height", 297 );
  this->Width = ini->ReadInteger( "WriteScalForm", "Width", 319 );
  delete ini;
  //
  FillOutListBox();
}

__fastcall TWriteScalForm::~TWriteScalForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "WriteScalForm", "Top", this->Top );
  ini->WriteInteger( "WriteScalForm", "Left", this->Left );
  ini->WriteInteger( "WriteScalForm", "Height", this->Height );
  ini->WriteInteger( "WriteScalForm", "Width", this->Width );
  delete ini;
}

void __fastcall TWriteScalForm::FillOutListBox()
{
  NamesCheckListBox->Clear();
  NVariableTable *nvTable = NVariableTable::GetTable();
  int cntr = 0;
  int end = nvTable->Entries();
  for( int i=0; i<end; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry( i );
    if( nv->GetData().GetNumberOfDimensions() == 0 )
    {
      ++cntr;
      NamesCheckListBox->Items->Add( AnsiString(nv->GetName().c_str()) );
      NamesCheckListBox->Checked[cntr-1] = false;
    }
  }
  if( cntr == 0 )
    Application->MessageBox( "there are no scalar variables", "Warning", MB_OK );
}

void __fastcall TWriteScalForm::ChooseFileClick(TObject *Sender)
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

void __fastcall TWriteScalForm::ApplyClick(TObject *Sender)
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
  std::vector<ExString> names;
  std::vector< std::vector<double> > data;
  std::vector<double> d;
  std::vector<double> values;
  //
  for( int i=0; i<NamesCheckListBox->Items->Count; ++i )
  {
    if( !NamesCheckListBox->Checked[i] )continue;
    int ndm;
    double value;
    int dimSizes[3];
    std::vector<double>().swap( d );
    ExString name( NamesCheckListBox->Items->Strings[i].c_str() );
    try
    {
      NumericVariable::GetVariable( name, ndm, value, d, dimSizes );
    }
    catch (EVariableError &e)
    {
      outStream->close();
      Application->MessageBox( e.what(), "Fatal Error", MB_OK );
      return;
    }
    names.push_back( name );
    values.push_back( value );
  }
  if( names.size() == 0 )
  {
    outStream->close();
    Application->MessageBox( "no scalars were chosen", "Warning", MB_OK );
    return;
  }
  std::size_t end = values.size();
  for( std::size_t j=0; j<end; ++j )(*outStream) << values[j] << " ";
  (*outStream) << "\n";
  outStream->close();
}

void __fastcall TWriteScalForm::UpdateClick(TObject *Sender)
{ FillOutListBox(); }

void __fastcall TWriteScalForm::CloseClick(TObject *Sender)
{
  Close();
  WriteScalForm = 0;
}

void __fastcall TWriteScalForm::FormClose(TObject *Sender,TCloseAction &Action)
{ Action = caFree; }

void __fastcall TWriteScalForm::FormClose2(TObject *Sender,bool &CanClose)
{ WriteScalForm = 0; }

// end of file
