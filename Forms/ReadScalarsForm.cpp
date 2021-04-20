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

#include "ReadScalarsForm.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TReadScalForm *ReadScalForm;

__fastcall TReadScalForm::TReadScalForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "ReadScalForm", "Top", 195 );
  this->Left = ini->ReadInteger( "ReadScalForm", "Left", 451 );
  this->Height = ini->ReadInteger( "ReadScalForm", "Height", 353 );
  this->Width = ini->ReadInteger( "ReadScalForm", "Width", 488 );
  delete ini;
  //
  NameColumnStringGrid->Cells[0][0] = AnsiString("Name");
  NameColumnStringGrid->Cells[1][0] = AnsiString("Column");
}

__fastcall TReadScalForm::~TReadScalForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "ReadScalForm", "Top", this->Top );
  ini->WriteInteger( "ReadScalForm", "Left", this->Left );
  ini->WriteInteger( "ReadScalForm", "Height", this->Height );
  ini->WriteInteger( "ReadScalForm", "Width", this->Width );
  delete ini;
}

void __fastcall TReadScalForm::UseFormatClick(TObject *Sender)
{ FormatEdit->Enabled = FormatCheckBox->Checked; }

void __fastcall TReadScalForm::ChooseFileClick(TObject *Sender)
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
        std::string s( "could not open " );
        s += std::string(CMD_read::Definition()->GetFilename().c_str());
        Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
        return;
      }
    }
    int count = FilenameComboBox->Items->Count;
    //
    // filename is already in the list
    // exchange names so current filename is at the top
    //
    for( int i = 1; i < count; ++i )
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
      for( int i = count-2; i >= 0; --i )
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

void __fastcall TReadScalForm::ErrorFillClick(TObject *Sender)
{ ErrorFillEdit->Enabled = ErrorFillCheckBox->Checked; }

void __fastcall TReadScalForm::ApplyClick(TObject *Sender)
{
  if( FilenameComboBox->Text.IsEmpty() )
  {
    Application->MessageBox( "no file has been chosen", "Warning", MB_OK );
    return;
  }
  bool newFile = (AnsiString(CMD_read::Definition()->GetFilename().c_str()) != FilenameComboBox->Text );
  CMD_read::Definition()->SetReadInFilename( FilenameComboBox->Text.c_str() );
  if( CMD_read::Definition()->GetStream()->is_open() &&
      (CloseFirstCheckBox->Checked || newFile) )CMD_read::Definition()->GetStream()->close();
  if( !CMD_read::Definition()->GetStream()->is_open() )
  {
    CMD_read::Definition()->GetStream()->open( CMD_read::Definition()->GetFilename().c_str(), ios_base::in );
    if( !(*CMD_read::Definition()->GetStream()) )
    {
      std::string s( "could not open " );
      s += std::string(CMD_read::Definition()->GetFilename().c_str());
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      return;
    }
  }
  std::string format;
  if( FormatCheckBox->Checked )
  {
    int len = FormatEdit->GetTextLen();
    if( len == 0 )
    {
      Application->MessageBox( "expecting a format", "Fatal Error", MB_OK );
      return;
    }
    char *c = new char[++len];
    FormatEdit->GetTextBuf( c, len );
    format = c;
    delete [] c;
  }
  std::vector<ExString> names;
  std::vector<int> columnNumber;
  int numberOfNamesEntered = NameColumnStringGrid->RowCount - 1;
  for( int i = 0; i < numberOfNamesEntered; ++i )
  {
    ExString name( NameColumnStringGrid->Cells[0][i+1].c_str() );
    if( name.empty() )
    {
      numberOfNamesEntered = i;
      break;
    }
    names.push_back( name );
    double dum8;
    std::string colm( NameColumnStringGrid->Cells[1][i+1].c_str() );
    if( colm.empty() || FormatCheckBox->Checked )
    {
      columnNumber.push_back(i+1);
    }
    else
    {
      std::stringstream ss;
      try
      {
        ss << colm;
        ss >> dum8;
      }
      catch (...)
      {
        Application->MessageBox( "invalid column number", "Fatal Error", MB_OK );
        return;
      }
      int column = static_cast<int>(dum8);
      if( column < 1 )
      {
        Application->MessageBox( "column number < 1", "Fatal Error", MB_OK );
        return;
      }
      columnNumber.push_back( column );
    }
  }
  if( names.empty() )
  {
    Application->MessageBox( "no output scalar names were entered", "Fatal Error", MB_OK );
    return;
  }
  int maxColumnNumber = 0;
  std::size_t numberOfNames = names.size();
  for( std::size_t j=0; j<numberOfNames; ++j )
  {
    if( maxColumnNumber < columnNumber[j] )maxColumnNumber = columnNumber[j];
    TVariableTable::GetTable()->RemoveEntry( names[j] );
    NVariableTable::GetTable()->RemoveEntry( names[j] );
  }
  int startingLine = 1;
  int len = LineRangeEdit->GetTextLen();
  char *c = new char[++len];
  LineRangeEdit->GetTextBuf( c, len );
  std::string s( c );
  delete [] c;
  double d;
  std::stringstream ss;
  try
  {
    ss << s;
    ss >> d;
  }
  catch (...)
  {
    LineRangeEdit->Clear();
    LineRangeEdit->Text = AnsiString( "1" );
    s += " is an invalid value for line number";
    Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
    return;
  }
  startingLine = static_cast<int>(d);
  if( startingLine < 1 )
  {
    LineRangeEdit->Clear();
    LineRangeEdit->Text = AnsiString( "1" );
    s += " is an invalid value for line number";
    Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
    return;
  }
  //
  // read in the initial dummy records
  //
  unsigned int recordNumber = 0;
  std::ifstream *inStream = CMD_read::Definition()->GetStream();
  for( int i=1; i<startingLine; ++i )
  {
    ++recordNumber;
    std::string buffer;
    if( !std::getline(*inStream,buffer) )
    {
      std::string s("end of file reached reading initial dummy records, file: ");
      s += CMD_read::Definition()->GetFilename().c_str();
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      return;
    }
  }
  ++recordNumber;
  //
  // finally ready to read some data
  //
  std::vector<double> data( maxColumnNumber );
  double const theErrorFill = 0.0;
  if( FormatCheckBox->Checked )
  {
    std::string sc;
    sc.clear();
    if( !std::getline(*inStream,sc) || sc.empty() )
    {
      char c[200];
      sprintf( c, "error reading record %d,\r file: %s\n",
               recordNumber, CMD_read::Definition()->GetFilename().c_str() );
      Application->MessageBox( c, "Fatal Error", MB_OK );
      return;
    }
    double vd[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                   0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                   0.0,0.0,0.0,0.0,0.0};
    int result = sscanf( sc.c_str(), format.c_str(), vd, vd+1, vd+2, vd+3, vd+5,
                         vd+6, vd+7, vd+8, vd+9, vd+10, vd+11, vd+12, vd+13, vd+14,
                         vd+15, vd+16, vd+17, vd+18, vd+19, vd+20, vd+21, vd+22,
                         vd+23, vd+24, vd+25, vd+26, vd+27, vd+28, vd+29, vd+30,
                         vd+31, vd+32, vd+33, vd+34 );
    if( result == EOF )
    {
      char cc[200];
      sprintf( cc, "error reading record %d,\r file: %s\n",
               recordNumber, CMD_read::Definition()->GetFilename().c_str() );
      Application->MessageBox( cc, "Fatal Error", MB_OK );
      return;
    }
    else if( result == 0 )
    {
      char cc[200];
      sprintf( cc, "error reading record %d,\r file: %s\n",
               recordNumber, CMD_read::Definition()->GetFilename().c_str() );
      Application->MessageBox( cc, "Fatal Error", MB_OK );
      return;
    }
    else if( result < maxColumnNumber )
    {
      char cc[200];
      sprintf( cc, "not enough values on record %d,\r file: %s\n",
               recordNumber, CMD_read::Definition()->GetFilename().c_str() );
      Application->MessageBox( cc, "Fatal Error", MB_OK );
      return;
    }
    //
    // no errors in reading
    //
    for( int k=0; k<maxColumnNumber; ++k )data[k] = vd[k];
  }
  else  // no format entered
  {
    std::string sc;
    sc.clear();
    if( !std::getline(*inStream,sc) || sc.empty() )
    {
      char c[200];
      sprintf( c, "error reading record %d,\r file: %s\n",
               recordNumber, CMD_read::Definition()->GetFilename().c_str() );
      Application->MessageBox( c, "Fatal Error", MB_OK );
      return;
    }
    int const Classes[128] =
    {
        3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
    };
    int const StateTable[7][3] =
    { // blnk  ,   othr
        {   1,   2,   3   },
        {   1,   2,   3   },
        {   4,   6,   3   },
        {   5,   7,   3   },
        {   5,   7,   3   },
        {   1,   2,   3   },
        {   1,   2,   3   }
    };
    std::vector<double> reals( maxColumnNumber, 0.0 );
    std::vector<int> itype( maxColumnNumber, 0 );
    std::vector<std::string> strings( maxColumnNumber );
    int nf = -1;
    unsigned int newState = 1;
    unsigned int currentState = 0;
    bool flag = true;
    unsigned int scLen = sc.length();
    for( unsigned int k=0; k<scLen; ++k )
    {
      currentState = newState;
      int iascii = toascii( sc[k] );
      newState = StateTable[currentState-1][Classes[iascii]-1];
      if( newState == 2 )
      {
        if( ++nf == maxColumnNumber )
        {
          flag = false;
          break;
        }
      }
      else if( newState == 3 )
      {
        strings[nf+1].push_back( sc[k] );
      }
      else if( newState == 4 )
      {
        ++nf;
        double d;
        bool test = true;
        std::stringstream ss;
        try
        {
          ss << strings[nf];
          ss >> d;
        }
        catch (...)
        {
          test = false;
        }
        if( test )
        {
          itype[nf] = 1;
          reals[nf] = d;
        }
        else
        {
          itype[nf] = 2;
        }
        if( nf == maxColumnNumber-1 )
        {
          flag = false;
          break;
        }
      }
    }
    if( flag )
    {
      ++nf;
      double d;
      bool test = true;
      std::stringstream ss;
      try
      {
        ss << strings[nf];
        ss >> d;
      }
      catch (...)
      {
        test = false;
      }
      if( test )
      {
        itype[nf] = 1;
        reals[nf] = d;
      }
      else
      {
        itype[nf] = 2;
      }
    }
    if( nf > 0 )
    {
      for( unsigned int k=0; k<numberOfNames; ++k )
      {
        if( itype[columnNumber[k]-1] != 1 )
        {
          data[k] = theErrorFill;
          if( DisplayMessagesCheckBox->Checked )
          {
            char c[200];
            sprintf( c, "error in field %d, record %d, file: %s\n",
                     k+1, recordNumber, CMD_read::Definition()->GetFilename().c_str() );
            Application->MessageBox( c, "Warning", MB_OK );
          }
        }
        else
        {
          data[k] = reals[columnNumber[k]-1];
        }
      }
    }
  }
  //
  // finished reading
  //
  for( std::size_t j=0; j<numberOfNames; ++j )
  {
    ExString s( "READ\\SCALARS " );
    s += CMD_read::Definition()->GetFilename();
    try
    {
      NumericVariable *nv = NumericVariable::PutVariable( names[j], data[j], s );
      nv->SetOrigin( CMD_read::Definition()->GetFilename() );
    }
    catch ( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Fatal Error", MB_OK );
      return;
    }
    if( DisplayMessagesCheckBox->Checked )
    {
      ExString s("scalar ");
      s += names[j] + ExString(" has been created");
      Application->MessageBox( s.c_str(), "Information", MB_OK );
    }
  }
  if( CloseAfterCheckBox->Checked )CMD_read::Definition()->GetStream()->close();
}

void __fastcall TReadScalForm::FilenameComboBoxClick(TObject *Sender)
{
  bool newFile =
   (AnsiString(CMD_read::Definition()->GetFilename().c_str()) !=
    FilenameComboBox->Items->Strings[FilenameComboBox->ItemIndex] );
  //
  CMD_read::Definition()->SetReadInFilename(
   ExString(FilenameComboBox->Items->Strings[FilenameComboBox->ItemIndex].c_str()) );
  if( CMD_read::Definition()->GetStream()->is_open() &&
     (CloseFirstCheckBox->Checked || newFile) )CMD_read::Definition()->GetStream()->close();
  if( !CMD_read::Definition()->GetStream()->is_open() )
  {
    CMD_read::Definition()->GetStream()->open( CMD_read::Definition()->GetFilename().c_str(), ios_base::in );
    if( !(*CMD_read::Definition()->GetStream()) )
    {
      std::string s( "could not open " );
      s += CMD_read::Definition()->GetFilename().c_str();
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      return;
    }
  }
}

void __fastcall TReadScalForm::NameColumnSetEditText(TObject *Sender,
      int ACol, int ARow, const AnsiString Value)
{ if( ACol == 0 )NameColumnStringGrid->Cells[1][ARow] = IntToStr(ARow); }

void __fastcall TReadScalForm::CloseClick(TObject *Sender)
{
  Close();
  ReadScalForm = 0;
}

void __fastcall TReadScalForm::FormClose(TObject *Sender,TCloseAction &Action)
{ Action = caFree; }

void __fastcall TReadScalForm::FormClose2(TObject *Sender,bool &CanClose)
{ ReadScalForm = 0; }

// end of file
