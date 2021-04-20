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

#include "ReadVectorsForm.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TReadVecForm *ReadVecForm;

__fastcall TReadVecForm::TReadVecForm( TComponent* Owner )
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "ReadVecForm", "Top", 90 );
  Left = ini->ReadInteger( "ReadVecForm", "Left", 740 );
  Height = ini->ReadInteger( "ReadVecForm", "Height", 520 );
  Width = ini->ReadInteger( "ReadVecForm", "Width", 390 );
  //
  AnsiString defaultPath( ExGlobals::GetExecutablePath().c_str() );
  currentPath_ = ini->ReadString("ReadVecForm","Path",defaultPath);
  int count = ini->ReadInteger( "ReadVecForm", "FileCount", 0 );
  int currentFile = ini->ReadInteger( "ReadVecForm", "CurrentFile", 0 );
  for( int i=0; i<count; ++i )
  {
    AnsiString ident( "file" );
    ident += AnsiString(i);
    FilenameComboBox->Items->Add( ini->ReadString("ReadVecForm",ident,"") );
  }
  FilenameComboBox->ItemIndex = currentFile;
  //
  CloseFirstCheckBox->Checked = ini->ReadBool( "ReadVecForm", "CloseFirst", true );
  CloseAfterCheckBox->Checked = ini->ReadBool( "ReadVecForm", "CloseAfter", true );
  MakeNewCheckBox->Checked = ini->ReadBool( "ReadVecForm", "MakeNew", true );
  AppendCheckBox->Checked = ini->ReadBool( "ReadVecForm", "Append", false );
  ErrorStopCheckBox->Checked = ini->ReadBool( "ReadVecForm", "ErrorStop", true );
  ErrorFillCheckBox->Checked = ini->ReadBool( "ReadVecForm", "ErrorFill", false );
  DisplayMessagesCheckBox->Checked = ini->ReadBool( "ReadVecForm", "DisplayMessages", true );
  ErrorFillEdit->Text = AnsiString( ini->ReadFloat( "ReadVecForm", "ErrorFillValue", 0.0 ) );
  ErrorFillEdit->Enabled = ErrorFillCheckBox->Checked;
  //
  LineRangeCheckBox->Checked = ini->ReadBool( "ReadVecForm", "UseLineRange", false );

  delete ini;
  ReadDialog->InitialDir = currentPath_;
  //
  NameColumnStringGrid->Cells[0][0] = AnsiString("Name");
  NameColumnStringGrid->Cells[1][0] = AnsiString("Column");
}

__fastcall TReadVecForm::~TReadVecForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "ReadVecForm", "Top", Top );
  ini->WriteInteger( "ReadVecForm", "Left", Left );
  ini->WriteInteger( "ReadVecForm", "Height", Height );
  ini->WriteInteger( "ReadVecForm", "Width", Width );
  ini->WriteString( "ReadVecForm", "Path", currentPath_ );
  ini->WriteInteger( "ReadVecForm", "CurrentFile", FilenameComboBox->ItemIndex );
  int count = FilenameComboBox->Items->Count;
  if( count == 0 )
  {
    int oldCount = ini->ReadInteger( "ReadVecForm", "FileCount", 0 );
    for( int i=0; i<oldCount; ++i )
      ini->DeleteKey( "ReadVecForm", AnsiString("file")+AnsiString(i) );
  }
  ini->WriteString( "ReadVecForm", "FileCount", count );
  for( int i=0; i<count; ++i )
    ini->WriteString( "ReadVecForm", AnsiString("file")+AnsiString(i),
                      FilenameComboBox->Items->Strings[i] );
  delete ini;
}

void __fastcall TReadVecForm::ChooseFile( TObject *Sender )
{
  if( ReadDialog->Execute() )
  {
    bool newFile = (AnsiString(CMD_read::Definition()->GetFilename().c_str()) != ReadDialog->FileName );
    //
    CMD_read::Definition()->SetReadInFilename( ExString(ReadDialog->FileName.c_str()) );
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
      {
        FilenameComboBox->Items->Strings[i+1] = FilenameComboBox->Items->Strings[i];
      }
      FilenameComboBox->Items->Strings[0] = AnsiString(CMD_read::Definition()->GetFilename().c_str());
    }
    else
    {
      FilenameComboBox->Items->Add( AnsiString(CMD_read::Definition()->GetFilename().c_str()) );
    }
    FilenameComboBox->ItemIndex = 0;
  }
}

void __fastcall TReadVecForm::MakeNewClick( TObject *Sender )
{
  AppendCheckBox->Enabled = !MakeNewCheckBox->Checked;
}

void __fastcall TReadVecForm::StopOnErrorsClick( TObject *Sender )
{
  ErrorFillCheckBox->Enabled = !ErrorStopCheckBox->Checked;
  ErrorFillEdit->Enabled = ErrorFillCheckBox->Enabled;
}

void __fastcall TReadVecForm::ErrorFillClick( TObject *Sender )
{
  ErrorFillEdit->Enabled = ErrorFillCheckBox->Checked;
  FormatCheckBox->Enabled = !ErrorFillCheckBox->Enabled;
  FormatEdit->Enabled = !ErrorFillCheckBox->Enabled;
}

void __fastcall TReadVecForm::ApplyClick( TObject *Sender )
{
  if( FilenameComboBox->Text.IsEmpty() )
  {
    Application->MessageBox( "no file has been chosen", "Warning", MB_OK );
    return;
  }
  bool newFile = (AnsiString(CMD_read::Definition()->GetFilename().c_str()) != FilenameComboBox->Text );
  ExString fileName( FilenameComboBox->Text.c_str() );
  if( CMD_read::Definition()->GetStream()->is_open() &&
     (CloseFirstCheckBox->Checked || newFile) )CMD_read::Definition()->GetStream()->close();
  if( !CMD_read::Definition()->GetStream()->is_open() )
  {
    CMD_read::Definition()->GetStream()->open( fileName.c_str(), ios_base::in );
    if( !(*CMD_read::Definition()->GetStream()) )
    {
      ExString s( "could not open " );
      s += fileName;
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      return;
    }
  }
  CMD_read::Definition()->SetReadInFilename( fileName );
  currentPath_ = ExtractFilePath( AnsiString(fileName.c_str()) );
  ExGlobals::SetCurrentPath( ExString(currentPath_.c_str()) );
  //
  std::vector<ExString> names;
  std::vector<int> columnNumbers;
  std::size_t numberOfNames = NameColumnStringGrid->RowCount - 1;
  int maxColumnNumber = 0;
  for( std::size_t i=0; i<numberOfNames; ++i )
  {
    ExString name( NameColumnStringGrid->Cells[0][i+1].c_str() );
    if( name.empty() )
    {
      numberOfNames = i;
      break;
    }
    names.push_back( name );
    double dum8;
    std::string colm( NameColumnStringGrid->Cells[1][i+1].c_str() );
    if( colm.empty() || FormatCheckBox->Checked )
    {
      columnNumbers.push_back(i+1);
    }
    else
    {
      if( !(std::istringstream(colm) >> dum8) )
      {
        Application->MessageBox( "invalid column number", "Fatal Error", MB_OK );
        return;
      }
      int column = static_cast<int>(dum8);
      if( column < 1 )
      {
        Application->MessageBox( "column number must be >= 1", "Fatal Error", MB_OK );
        return;
      }
      columnNumbers.push_back( column );
      if( maxColumnNumber < column )maxColumnNumber = column;
    }
  }
  if( names.empty() )
  {
    Application->MessageBox( "no vector names were entered", "Fatal Error", MB_OK );
    return;
  }
  numberOfNames = names.size();
  if( numberOfNames > 35 )
  {
    Application->MessageBox( "maximum number of vectors that can be read is 35",
                             "Fatal Error", MB_OK );
    return;
  }
  std::vector< std::vector<double> > data( maxColumnNumber );
  std::vector<unsigned int> nloc( numberOfNames, 0 );  // number of values to read
  for( std::size_t j=0; j<numberOfNames; ++j )
  {
    TextVariable *tv = TVariableTable::GetTable()->GetVariable( names[j] );
    if( tv )
    {
      TVariableTable::GetTable()->RemoveEntry( tv );
      continue;
    }
    NumericVariable *nv = NVariableTable::GetTable()->GetVariable( names[j] );
    if( nv )
    {
      if( MakeNewCheckBox->Checked || nv->GetData().GetNumberOfDimensions()!=1 )
        NVariableTable::GetTable()->RemoveEntry( nv );
      else
      {
        nloc[j] = nv->GetData().GetDimMag(0);
        if( AppendCheckBox->Checked )
          data[j].assign( nv->GetData().GetData().begin(), nv->GetData().GetData().end() );
      }
    }
  }
  //
  std::vector<bool> fill( numberOfNames, true );
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
    if( !(std::istringstream(s) >> d) )
    {
      LineRange1Edit->Clear();
      LineRange1Edit->Text = AnsiString( "1" );
      s += " is an invalid value for starting line range";
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      return;
    }
    startingLine = int( d );
    if( startingLine < 1 )
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
      if( !(std::istringstream(s) >> d) )
      {
        LineRange2Edit->Clear();
        s += " is an invalid value for final line range";
        Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
        return;
      }
      lastLine = int( d );
      if( lastLine < startingLine )
      {
        LineRange2Edit->Clear();
        Application->MessageBox( "final < starting line range", "Fatal Error", MB_OK );
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
      if( !(std::istringstream(s) >> d) )
      {
        LineRange3Edit->Clear();
        LineRange3Edit->Text = AnsiString( "1" );
        s += " is an invalid value for line range increment";
        Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
        return;
      }
      increment = int( d );
      if( increment < 1 )
      {
        LineRange2Edit->Clear();
        LineRange3Edit->Text = AnsiString( "1" );
        Application->MessageBox( "line range increment < 1", "Fatal Error", MB_OK );
        return;
      }
    }
  }
  double errorFill( CMD_read::Definition()->GetErrorFill() );
  if( ErrorFillCheckBox->Checked )
  {
    int len = ErrorFillEdit->GetTextLen();
    char *c = new char[++len];
    ErrorFillEdit->GetTextBuf( c, len );
    std::string s(c);
    delete [] c;
    double d;
    if( !(std::istringstream(s) >> d) )
    {
      ErrorFillEdit->Clear();
      ErrorFillEdit->Text = AnsiString( "0.0" );
      s += " is an invalid value for error fill";
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      return;
    }
    errorFill = d;
    CMD_read::Definition()->SetErrorFill( d );
  }
  //
  // read in the initial dummy records
  //
  int last = startingLine+1; // last record read + 1
  //
  // there could only be initial dummy records if a line range was specified
  // and line ranges can only be on files with a record structure
  //
  std::ifstream *inStream = CMD_read::Definition()->GetStream();
  unsigned int recordNumber = 0;
  for( int i=1; i<startingLine; ++i )
  {
    ++recordNumber;
    std::string buffer;
    if( !std::getline(*inStream,buffer) )
    {
      Application->MessageBox("end of file reached while reading initial dummy records","Fatal Error",MB_OK);
      return;
    }
  }
  //
  // finally ready to read some data
  //
  std::string const delims(", \t");
  if( ErrorFillCheckBox->Checked )
  {
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
    for( int j=1;; ++j )
    {
      ++recordNumber;
      std::string sc;
      sc.clear();
      if( !std::getline(*inStream,sc) )break;
      if( sc.empty() )continue; // the record is empty, nothing on the line
      std::vector<double> reals( maxColumnNumber, 0.0 );
      std::vector<int> itype( maxColumnNumber, 0 );
      std::vector<std::string> strings( maxColumnNumber );
      int nf = -1;
      unsigned int newState = 1;
      unsigned int currentState = 0;
      bool flag = true;
      std::size_t length = sc.length();
      for( std::size_t k=0; k<length; ++k )
      {
        currentState = newState;
        int iascii = toascii( sc[k] );
        newState = StateTable[currentState-1][Classes[iascii]-1];
        if( newState == 2 )
        {
          if( nf++ == maxColumnNumber )
          {
            flag = false;
            break;
          }
        }
        else if( newState == 3 )
        {
          strings[nf+1].push_back( sc[k] );
        }
        else if( newState == 4 || newState == 6 )
        {
          itype[++nf] = 2;
          double d;
          if( std::istringstream(strings[nf]) >> d )
          {
            itype[nf] = 1;
            reals[nf] = d;
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
        itype[++nf] = 2;
        double d;
        if( std::istringstream(strings[nf]) >> d )
        {
          itype[nf] = 1;
          reals[nf] = d;
        }
      }
      bool stopReading = false;
      if( nf >= 0 )
      {
        for( std::size_t k=0; k<numberOfNames; ++k )
        {
          if( fill[k] )
          {
            if( itype[columnNumbers[k]-1] != 1 )
            {
              data[k].push_back( errorFill );
              if( DisplayMessagesCheckBox->Checked )
              {
                char c[200];
                sprintf( c, "field %d, record %d set to Error Fill value\r file: %s",
                         k+1, recordNumber, CMD_read::Definition()->GetFilename().c_str() );
                ExGlobals::WarningMessage( c );
              }
            }
            else data[k].push_back( reals[columnNumbers[k]-1] );
            if( data[k].size()==nloc[k] && !MakeNewCheckBox->Checked &&
                !AppendCheckBox->Checked )fill[k] = false;
          }
        }
        if( stopReading )break;
      }
      if( LineRangeCheckBox->Checked )
      {
        stopReading = false;
        int next = startingLine + increment*j;
        if( next > lastLine )break;
        for( int i=last; i<next; ++i )
        {
          std::string buffer;
          if( !std::getline(*inStream,buffer) )
          {
            stopReading = true;
            break;
          }
        }
        if( stopReading )break;
        last = next+1;
      }
      stopReading = true;
      for( std::size_t k=0; k<numberOfNames; ++k )
      {
        if( fill[k] )
        {
          stopReading = false;
          break;
        }
      }
      if( stopReading )break;
    }
  }
  else // not replacing errors with errorFill value
  {
    std::string format;
    std::string formatSP( "%lf" );
    std::string formatCM( "%lf" );
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
    else
    {
      for( int j=1; j<maxColumnNumber; ++j )
      {
        formatSP += " %lf";
        formatCM += ",%lf";
      }
    }
    for( int j=1;; ++j )  // loop over records in the file
    {
      ++recordNumber;
      std::string sc;
      sc.clear();
      if( !std::getline(*inStream,sc) )break;
      bool readError = false;
      if( sc.empty() )continue; // the record is empty, nothing on the line
      if( !FormatCheckBox->Checked )format = sc.find(',')==sc.npos ? formatSP : formatCM;
      double vd[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0};
      int result = sscanf( sc.c_str(), format.c_str(), vd, vd+1, vd+2, vd+3, vd+4, vd+5,
                           vd+6, vd+7, vd+8, vd+9, vd+10, vd+11, vd+12, vd+13, vd+14,
                           vd+15, vd+16, vd+17, vd+18, vd+19, vd+20, vd+21, vd+22,
                           vd+23, vd+24, vd+25, vd+26, vd+27, vd+28, vd+29, vd+30,
                           vd+31, vd+32, vd+33, vd+34 );
      if( result == EOF )
      {
        ExString c( "error reading record " );
        c += ExString(recordNumber);
        if( ErrorStopCheckBox->Checked )
        {
          Application->MessageBox( c.c_str(), "Fatal Error", MB_OK );
          return;  // stop reading
        }
        if( DisplayMessagesCheckBox->Checked )ExGlobals::WarningMessage( c );
      }
      else if( result == 0 )
      {
        ExString c( "error reading record " );
        c += ExString(recordNumber);
        if( ErrorStopCheckBox->Checked )
        {
          Application->MessageBox( c.c_str(), "Fatal Error", MB_OK );
          return;  // stop reading
        }
        if( DisplayMessagesCheckBox->Checked )ExGlobals::WarningMessage( c );
      }
      else if( result < maxColumnNumber )
      {
        ExString c( "not enough values on record " );
        c += ExString(recordNumber);
        if( ErrorStopCheckBox->Checked )
        {
          Application->MessageBox( c.c_str(), "Fatal Error", MB_OK );
          return;  // stop reading
        }
        if( DisplayMessagesCheckBox->Checked )ExGlobals::WarningMessage( c );
      }
      else // no errors in reading
      {
        for( std::size_t k=0; k<numberOfNames; ++k )data[k].push_back( vd[columnNumbers[k]-1] );
        if( LineRangeCheckBox->Checked )
        {
          bool stopReading = false;
          int next = startingLine + increment*j;
          if( lastLine>0 && next>lastLine )break;
          for( int i=last; i<next; ++i )
          {
            std::string buffer;
            if( !std::getline(*inStream,buffer) )
            {
              stopReading = true;
              break;
            }
          }
          if( stopReading )break;
          last = next+1;
        }
      }
    }
  }
  //
  // finished reading, one of: end of file, an error, or not extending vector
  //
  bool nothingWasRead = true;
  for( std::size_t j=0; j<numberOfNames; ++j )
  {
    if( data[j].size() > 0 )
    {
      ExString s("READ\\VECTORS ");
      s += CMD_read::Definition()->GetFilename();
      try
      {
        NumericVariable *nv = NumericVariable::PutVariable( names[j], data[j], 0, s );
        nv->SetOrigin( CMD_read::Definition()->GetFilename() );
      }
      catch ( EVariableError &e )
      {
        Application->MessageBox( e.what(), "Fatal Error", MB_OK );
        return;
      }
      nothingWasRead = false;
      if( DisplayMessagesCheckBox->Checked )
      {
        char c[10];
        sprintf( c, "%d", data[j].size()-nloc[j] );
        if( AppendCheckBox->Checked )
        {
          ExString s( c );
          s += ExString(" values have been appended to vector ") + names[j];
          Application->MessageBox( s.c_str(), "Information", MB_OK );
        }
        else
        {
          ExString s("vector ");
          s += names[j] + ExString(" has been created with ") + ExString( c ) +
              ExString(" values");
          Application->MessageBox( s.c_str(), "Information", MB_OK );
        }
      }
    }
  }
  if( nothingWasRead )
    Application->MessageBox( "nothing was read", "Information", MB_OK );

  if( CloseAfterCheckBox->Checked )CMD_read::Definition()->GetStream()->close();
}
/*
void __fastcall TReadVecForm::FilenameComboBoxClick(TObject *Sender)
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
      ExString s( "could not open " );
      s += CMD_read::Definition()->GetFilename();
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      return;
    }
  }
}
*/
void __fastcall TReadVecForm::NameColumnSetEditText( TObject *Sender,
                                                     int ACol, int ARow,
                                                     const AnsiString Value )
{ if( ACol == 0 )NameColumnStringGrid->Cells[1][ARow] = IntToStr(ARow); }

void __fastcall TReadVecForm::LineRangeCheckBoxClicked( TObject *Sender )
{
  bool const lrc = LineRangeCheckBox->Checked;
  LineRange1Edit->Enabled = lrc;
  LineRange2Edit->Enabled = lrc;
  LineRange3Edit->Enabled = lrc;
  LineRange1Label->Enabled = lrc;
  LineRange2Label->Enabled = lrc;
  LineRange3Label->Enabled = lrc;
}

void __fastcall TReadVecForm::FormatCheckBoxClicked( TObject *Sender )
{ FormatEdit->Enabled = FormatCheckBox->Checked; }

void __fastcall TReadVecForm::CloseClick(TObject *Sender)
{
  Close();
  ReadVecForm = 0;
}

void __fastcall TReadVecForm::FormClose( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TReadVecForm::FormClose2( TObject *Sender, bool &CanClose )
{ ReadVecForm = 0; }

void __fastcall TReadVecForm::ClearFileList( TObject *Sender )
{ FilenameComboBox->Clear(); }

void __fastcall TReadVecForm::RemoveFilename( TObject *Sender )
{
  if( FilenameComboBox->Text.IsEmpty() )return;
  int i = FilenameComboBox->Items->IndexOf( FilenameComboBox->Text );
  FilenameComboBox->Items->Delete(i);
  if( FilenameComboBox->Items->Count == 0 )FilenameComboBox->Text = AnsiString("");
  else                                     FilenameComboBox->ItemIndex = 0;
}

void __fastcall TReadVecForm::DeleteFilename( TObject *Sender, WORD &Key, TShiftState Shift )
{
  if( Key != VK_DELETE )return;
  if( FilenameComboBox->Text.IsEmpty() )return;
  int i = FilenameComboBox->Items->IndexOf( FilenameComboBox->Text );
  FilenameComboBox->Items->Delete(i);
  if( FilenameComboBox->Items->Count == 0 )FilenameComboBox->Text = AnsiString("");
  else                                     FilenameComboBox->ItemIndex = 0;
}

void __fastcall TReadVecForm::ReturnKeyPress( TObject *Sender, char &Key )
{
  if( Key == VK_RETURN )ApplyClick( static_cast<TObject*>(0) );
}

// end of file

