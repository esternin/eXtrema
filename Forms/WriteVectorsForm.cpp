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

#include "WriteVectorsForm.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TWriteVecForm *WriteVecForm;

__fastcall TWriteVecForm::TWriteVecForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "WriteVecForm", "Top", 349 );
  this->Left = ini->ReadInteger( "WriteVecForm", "Left", 541 );
  this->Height = ini->ReadInteger( "WriteVecForm", "Height", 332 );
  this->Width = ini->ReadInteger( "WriteVecForm", "Width", 370 );
  delete ini;
  //
  FillOutListBox();
}

__fastcall TWriteVecForm::~TWriteVecForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "WriteVecForm", "Top", this->Top );
  ini->WriteInteger( "WriteVecForm", "Left", this->Left );
  ini->WriteInteger( "WriteVecForm", "Height", this->Height );
  ini->WriteInteger( "WriteVecForm", "Width", this->Width );
  delete ini;
}

void __fastcall TWriteVecForm::FillOutListBox()
{
  NamesCheckListBox->Clear();
  NVariableTable *nvTable = NVariableTable::GetTable();
  int cntr = 0;
  int end = nvTable->Entries();
  for( int i=0; i<end; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry( i );
    if( nv->GetData().GetNumberOfDimensions() == 1 )
    {
      ++cntr;
      AnsiString name( nv->GetName().c_str() );
      NamesCheckListBox->Items->Add( name );
      NamesCheckListBox->Checked[cntr-1] = false;
    }
  }
  if( cntr == 0 )Application->MessageBox( "there are no vector variables", "Warning", MB_OK );
}

void __fastcall TWriteVecForm::ChooseFileClick(TObject *Sender)
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
    {
      FilenameComboBox->Items->Add( FilenameComboBox->Text );
    }
    //FilenameComboBox->SelStart = 0;
    //FilenameComboBox->SelText = FilenameComboBox->Items->Strings[0];
  }
}

void __fastcall TWriteVecForm::ApplyClick(TObject *Sender)
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
  std::vector<int> nd1;
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
    data.push_back( d );
    nd1.push_back( dimSizes[0] );
  }
  if( names.size() == 0 )
  {
    outStream->close();
    Application->MessageBox( "no vectors were chosen", "Warning", MB_OK );
    return;
  }
  std::vector<int> mm( names.size(), 0 );
  for( ;; )
  {
    std::vector<double>().swap( d );
    std::size_t end = names.size();
    for( std::size_t i=0; i<end; ++i )
    {
      if( ++mm[i] <= nd1[i] )d.push_back( data[i][mm[i]-1] );
    }
    if( d.empty() )break;
    end = d.size();
    for( std::size_t j=0; j<end; ++j )(*outStream) << d[j] << " ";
    (*outStream) << "\n";
  }
  outStream->close();
}

void __fastcall TWriteVecForm::UpdateClick(TObject *Sender)
{ FillOutListBox(); }

void __fastcall TWriteVecForm::CloseClick(TObject *Sender)
{
  Close();
  WriteVecForm = 0;
}

void __fastcall TWriteVecForm::FormClose(TObject *Sender,TCloseAction &Action)
{ Action = caFree; }

void __fastcall TWriteVecForm::FormClose2(TObject *Sender,bool &CanClose)
{ WriteVecForm = 0; }

void __fastcall TWriteVecForm::SelectAllClick(TObject *Sender)
{
  for( int i=0; i<NamesCheckListBox->Items->Count; ++i )
    NamesCheckListBox->Checked[i] = true;
}

void __fastcall TWriteVecForm::ClearAllClick(TObject *Sender)
{
  for( int i=0; i<NamesCheckListBox->Items->Count; ++i )
    NamesCheckListBox->Checked[i] = false;
}
// end of file

