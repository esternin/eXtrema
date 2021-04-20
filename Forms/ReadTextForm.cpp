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
#include "ECommandError.h"
#include "EExpressionError.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "NVariableTable.h"
#include "TextVariable.h"
#include "TVariableTable.h"
#include "CMD_read.h"

#include "ReadTextForm.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TReadTxtForm *ReadTxtForm;

__fastcall TReadTxtForm::TReadTxtForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "ReadTxtForm", "Top", 373 );
  this->Left = ini->ReadInteger( "ReadTxtForm", "Left", 395 );
  this->Height = ini->ReadInteger( "ReadTxtForm", "Height", 258 );
  this->Width = ini->ReadInteger( "ReadTxtForm", "Width", 440 );
  delete ini;
}

__fastcall TReadTxtForm::~TReadTxtForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "ReadTxtForm", "Top", this->Top );
  ini->WriteInteger( "ReadTxtForm", "Left", this->Left );
  ini->WriteInteger( "ReadTxtForm", "Height", this->Height );
  ini->WriteInteger( "ReadTxtForm", "Width", this->Width );
  delete ini;
}

void __fastcall TReadTxtForm::ChooseFile(TObject *Sender)
{
  if( ReadDialog->Execute() )
  {
    bool newFile = (AnsiString(CMD_read::Definition()->GetFilename().c_str()) != ReadDialog->FileName );
    //
    CMD_read::Definition()->SetReadInFilename( ReadDialog->FileName.c_str() );
    if( CMD_read::Definition()->GetStream()->is_open() &&
       (CloseFirstCheckBox->Checked || newFile) )CMD_read::Definition()->GetStream()->close();
    if( !CMD_read::Definition()->GetStream()->is_open() )
    {
      CMD_read::Definition()->GetStream()->open( CMD_read::Definition()->GetFilename().c_str(), ios_base::in );
      if( !(*CMD_read::Definition()->GetStream()) )
      {
        ExString s( "could not open " );
        s += CMD_read::Definition()->GetFilename();
        Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
        return;
      }
    }
    int count = FilenameComboBox->Items->Count;
    //
    // filename is already in the list
    // exchange names so current filename is at the top
    //
    for( int i=1; i<count; ++i )
    {
      if( AnsiString(CMD_read::Definition()->GetFilename().c_str()) == FilenameComboBox->Items->Strings[i] )
      {
        AnsiString temp( FilenameComboBox->Items->Strings[0] );
        FilenameComboBox->Items->Strings[0] = AnsiString(CMD_read::Definition()->GetFilename().c_str());
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
      FilenameComboBox->Items->Strings[0] = AnsiString(CMD_read::Definition()->GetFilename().c_str());
    }
    else
    {
      FilenameComboBox->Items->Add( AnsiString(CMD_read::Definition()->GetFilename().c_str()) );
    }
    FilenameComboBox->ItemIndex = 0;
  }
}

void __fastcall TReadTxtForm::FilenameComboBoxClick(TObject *Sender)
{
  bool newFile =
   (AnsiString(CMD_read::Definition()->GetFilename().c_str()) !=
    FilenameComboBox->Items->Strings[FilenameComboBox->ItemIndex] );
  //
  CMD_read::Definition()->SetReadInFilename(
    FilenameComboBox->Items->Strings[FilenameComboBox->ItemIndex].c_str() );
  if( CMD_read::Definition()->GetStream()->is_open() &&
     (CloseFirstCheckBox->Checked || newFile) )CMD_read::Definition()->GetStream()->close();
  if( !CMD_read::Definition()->GetStream()->is_open() )
  {
    CMD_read::Definition()->GetStream()->open( CMD_read::Definition()->GetFilename().c_str(), ios_base::in );
    if( !(*CMD_read::Definition()->GetStream()) )
    {
      ExString s( "could not open " );
      s += CMD_read::Definition()->GetFilename();
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      return;
    }
  }
}

void __fastcall TReadTxtForm::ApplyClick(TObject *Sender)
{
  if( FilenameComboBox->Text.IsEmpty() )
  {
    Application->MessageBox( "no file has been chosen", "Warning", MB_OK );
    return;
  }
  bool newFile = (AnsiString(CMD_read::Definition()->GetFilename().c_str()) != FilenameComboBox->Text );
  CMD_read::Definition()->SetReadInFilename( ExString(FilenameComboBox->Text.c_str()) );
  if( CMD_read::Definition()->GetStream()->is_open() &&
      (CloseFirstCheckBox->Checked || newFile) )CMD_read::Definition()->GetStream()->close();
  if( !CMD_read::Definition()->GetStream()->is_open() )
  {
    CMD_read::Definition()->GetStream()->open( CMD_read::Definition()->GetFilename().c_str(), ios_base::in );
    if( !(*CMD_read::Definition()->GetStream()) )
    {
      ExString s( "could not open " );
      s += CMD_read::Definition()->GetFilename();
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      return;
    }
  }
  //
  AnsiString textName;
  if( TextNameEdit->GetTextLen() == 0 )
  {
    Application->MessageBox( "expecting output string variable name", "Fatal Error", MB_OK );
    return;
  }
  textName = TextNameEdit->Text;
  //
  int startingLine = 1;
  int lastLine = -1;
  int increment = 1;
  if( LineRangeCheckBox->Checked )
  {
    int len = LineRange1Edit->GetTextLen();
    char *c = new char[++len];
    LineRange1Edit->GetTextBuf( c, len );
    std::string s( c );
    delete [] c;
    double d;
    bool test = true;
    std::stringstream ss;
    try
    {
      ss << s;
      ss >> d;
    }
    catch (...)
    {
      test = false;
    }
    if( test )
    {
      startingLine = static_cast<int>(d);
      if( startingLine < 1 )
      {
        LineRange1Edit->Clear();
        LineRange1Edit->Text = AnsiString( "1" );
        s += " is an invalid value for starting line range";
        Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
        return;
      }
    }
    else
    {
      LineRange1Edit->Clear();
      LineRange1Edit->Text = AnsiString( "1" );
      s += " is an invalid value for starting line range";
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      return;
    }
    len = LineRange2Edit->GetTextLen();
    if( len > 0 )
    {
      c = new char[++len];
      LineRange2Edit->GetTextBuf( c, len );
      s.assign( c );
      delete [] c;
      bool test = true;
      std::stringstream ss;
      try
      {
        ss << s;
        ss >> d;
      }
      catch (...)
      {
        test = false;
      }
      if( test )
      {
        lastLine = static_cast<int>(d);
        if( lastLine < startingLine )
        {
          LineRange2Edit->Clear();
          Application->MessageBox( "final < starting line range", "Fatal Error", MB_OK );
          return;
        }
      }
      else
      {
        LineRange2Edit->Clear();
        s += " is an invalid value for final line range";
        Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
        return;
      }
    }
    len = LineRange3Edit->GetTextLen();
    if( len > 0 )
    {
      c = new char[++len];
      LineRange3Edit->GetTextBuf( c, len );
      s.assign( c );
      delete [] c;
      bool test = true;
      std::stringstream ss;
      try
      {
        ss << s;
        ss >> d;
      }
      catch (...)
      {
        test = false;
      }
      if( test )
      {
        increment = static_cast<int>(d);
        if( increment < 1 )
        {
          LineRange2Edit->Clear();
          LineRange3Edit->Text = AnsiString("1");
          Application->MessageBox( "line range increment < 1", "Fatal Error", MB_OK );
          return;
        }
      }
      else
      {
        LineRange3Edit->Clear();
        LineRange3Edit->Text = AnsiString("1");
        s += " is an invalid value for line range increment";
        Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
        return;
      }
    }
  }
  bool vecRng = (lastLine != -1) && (lastLine != startingLine) &&
                ((lastLine-startingLine)/increment+1 > 1);
  int numRng = 0;
  if( vecRng )numRng = (lastLine-startingLine)/increment + 1;
  //
  // Read in the initial dummy records
  //
  int last = startingLine+1; // last record read + 1
  //
  // there could only be initial dummy records if a line range was specified
  // and line ranges can only be on files with a record structure
  //
  unsigned int recordNumber = 0;
  std::ifstream *inStream = CMD_read::Definition()->GetStream();
  for( int i=1; i<startingLine; ++i )
  {
    ++recordNumber;
    std::string buffer;
    if( !std::getline(*inStream,buffer) )
    {
      Application->MessageBox(
       "end of file reached while reading initial dummy records", "Fatal Error", MB_OK );
      return;
    }
  }
  std::vector<ExString> data;
  for( int j = 1;; ++j )
  {
    ++recordNumber;
    std::string sc;
    sc.clear();
    if( !std::getline(*inStream,sc) )break;
    if( sc.empty() )
    {
      sc = " ";
      if( !vecRng )break;
      continue; // the record is empty, nothing on the line
    }
    data.push_back( ExString(sc) );
    if( !vecRng )break;
    //
    // vector line range
    //
    bool stopReading = false;
    if( j >= numRng )break; // stop reading
    int next = startingLine + increment*j;
    for( int i = last; i < next; ++i )
    {
      ++recordNumber;
      std::string buffer;
      if( !std::getline(*inStream,buffer) )
      {
        stopReading = true;
        break;
      }
    }
    if( stopReading )break; // stop reading
    last = next+1;
  }
  ExString s( "READ\\TEXT " );
  s += CMD_read::Definition()->GetFilename();
  try
  {
    TextVariable *tv = TextVariable::PutVariable( textName.c_str(), data, s );
    tv->SetOrigin( CMD_read::Definition()->GetFilename() );
  }
  catch ( EVariableError &e )
  {
    Application->MessageBox( e.what(), "Fatal Error", MB_OK );
    return;
  }
  //
  if( DisplayMessagesCheckBox->Checked )
  {
    ExString s( "text variable " );
    s += ExString(textName.c_str()) + ExString(" has been created");
    Application->MessageBox( s.c_str(), "Information", MB_OK );
  }
  if( CloseAfterCheckBox->Checked )CMD_read::Definition()->GetStream()->close();
}

void __fastcall TReadTxtForm::LineRangeCheckBoxClicked(TObject *Sender)
{
  if( LineRangeCheckBox->Checked )
  {
    LineRange1Edit->Enabled = true;
    LineRange2Edit->Enabled = true;
    LineRange3Edit->Enabled = true;
    LineRange1Label->Enabled = true;
    LineRange2Label->Enabled = true;
    LineRange3Label->Enabled = true;
  }
  else
  {
    LineRange1Edit->Enabled = false;
    LineRange2Edit->Enabled = false;
    LineRange3Edit->Enabled = false;
    LineRange1Label->Enabled = false;
    LineRange2Label->Enabled = false;
    LineRange3Label->Enabled = false;
  }
}

void __fastcall TReadTxtForm::CloseClick(TObject *Sender)
{
  Close();
  ReadTxtForm = 0;
}

void __fastcall TReadTxtForm::FormClose(TObject *Sender,TCloseAction &Action)
{ Action = caFree; }

void __fastcall TReadTxtForm::FormClose2(TObject *Sender,bool &CanClose)
{ ReadTxtForm = 0; }

// end of file
