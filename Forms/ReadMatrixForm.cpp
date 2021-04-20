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

#include "ReadMatrixForm.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TReadMatForm *ReadMatForm;

__fastcall TReadMatForm::TReadMatForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "ReadMatForm", "Top", 274 );
  this->Left = ini->ReadInteger( "ReadMatForm", "Left", 533 );
  this->Height = ini->ReadInteger( "ReadMatForm", "Height", 291 );
  this->Width = ini->ReadInteger( "ReadMatForm", "Width", 460 );
  delete ini;
}

__fastcall TReadMatForm::~TReadMatForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "ReadMatForm", "Top", this->Top );
  ini->WriteInteger( "ReadMatForm", "Left", this->Left );
  ini->WriteInteger( "ReadMatForm", "Height", this->Height );
  ini->WriteInteger( "ReadMatForm", "Width", this->Width );
  delete ini;
}

void __fastcall TReadMatForm::ApplyClick(TObject *Sender)
{
  if( FilenameComboBox->Text.IsEmpty() )
  {
    Application->MessageBox( "no file has been chosen", "Warning", MB_OK );
    return;
  }
  bool newFile =
      ( AnsiString(CMD_read::Definition()->GetFilename().c_str()) != FilenameComboBox->Text );
  CMD_read::Definition()->SetReadInFilename( FilenameComboBox->Text.c_str() );
  std::ifstream *inStream = CMD_read::Definition()->GetStream();
  if( inStream->is_open() && (CloseFirstCheckBox->Checked || newFile) )inStream->close();
  if( !inStream->is_open() )
  {
    inStream->open( CMD_read::Definition()->GetFilename().c_str(), ios_base::in );
    if( !(*inStream) )
    {
      ExString s( "could not open " );
      s += CMD_read::Definition()->GetFilename();
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      return;
    }
  }
  AnsiString format;
  if( FormatCheckBox->Checked )
  {
    if( FormatEdit->GetTextLen() == 0 )
    {
      Application->MessageBox( "expecting a format", "Fatal Error", MB_OK );
      return;
    }
    format = FormatEdit->Text;
  }
  AnsiString matrixName;
  if( MatrixNameEdit->GetTextLen() == 0 )
  {
    Application->MessageBox( "expecting output matrix name", "Fatal Error", MB_OK );
    return;
  }
  matrixName = MatrixNameEdit->Text;
  // get the number of rows of the output matrix
  //
  int nRow;
  if( NumRowsEdit->GetTextLen() == 0 )
  {
    Application->MessageBox( "expecting the number of rows", "Fatal Error", MB_OK );
    return;
  }
  try
  {
    nRow = StrToInt( NumRowsEdit->Text );
  }
  catch ( EConvertError &e )
  {
    Application->MessageBox( "invalid value entered for number of rows", "Fatal Error", MB_OK );
    return;
  }
  if( nRow < 0 )
  {
    Application->MessageBox( "number of rows < 0", "Fatal Error", MB_OK );
    return;
  }
  else if( nRow == 0 )
  {
    Application->MessageBox( "number of rows = 0", "Fatal Error", MB_OK );
    return;
  }
  // get the number of columns of the output matrix
  //
  bool numberOfColumnsGiven = ( NumRowsEdit->GetTextLen() > 0 );
  int nCol( 1000 );
  if( numberOfColumnsGiven )
  {
    try
    {
      nCol = StrToInt( NumColmsEdit->Text );
    }
    catch ( EConvertError &e )
    {
      Application->MessageBox( "invalid value entered for number of columns", "Fatal Error", MB_OK );
      return;
    }
    if( nCol < 0 )
    {
      Application->MessageBox( "number of columns < 0", "Fatal Error", MB_OK );
      return;
    }
    else if( nCol == 0 )
    {
      Application->MessageBox( "number of columns = 0", "Fatal Error", MB_OK );
      return;
    }
  }
  // Read in the initial dummy records
  //
  int startingLine;
  if( LineRangeEdit->GetTextLen() == 0 )
  {
    Application->MessageBox( "expecting a starting line number", "Fatal Error", MB_OK );
    return;
  }
  try
  {
    startingLine = StrToInt( LineRangeEdit->Text );
  }
  catch ( EConvertError &e )
  {
    Application->MessageBox( "invalid value entered for starting line", "Fatal Error", MB_OK );
    return;
  }
  unsigned int recordNumber=0;
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
  std::vector<double> data;
  double const theErrorFill = 0.0;
  if( numberOfColumnsGiven )
  {
    data.resize( nRow*nCol );
    if( FormatCheckBox->Checked )
    {
      for( int j = 0; j < nCol; ++j )
      {
        ++recordNumber;
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
        if( result==EOF || result==0 )
        {
          char cc[200];
          sprintf( cc, "error reading record %d,\r file: %s\n",
                   recordNumber, CMD_read::Definition()->GetFilename().c_str() );
          Application->MessageBox( cc, "Fatal Error", MB_OK );
          return;
        }
        else if( result < nRow )
        {
          char cc[200];
          sprintf( cc, "not enough values on record %d,\r file: %s\n",
                   recordNumber, CMD_read::Definition()->GetFilename().c_str() );
          Application->MessageBox( cc, "Fatal Error", MB_OK );
          return;
        }
        // no errors in reading
        //
        for( int k=0; k<nRow; ++k )data[k+j*nRow] = vd[k];
      }
    }
    else  // no format given
    {
      for( int j = 0; j < nCol; ++j )
      {
        ++recordNumber;
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
        std::vector<double> reals( nRow, 0.0 );
        std::vector<int> itype( nRow, 0 );
        std::vector<std::string> strings( nRow );
        int nf = -1;
        unsigned int newState = 1;
        unsigned int currentState = 0;
        bool flag = true;
        for( unsigned int k = 0; k < sc.length(); ++k )
        {
          currentState = newState;
          int iascii = toascii( sc[k] );
          newState = StateTable[currentState-1][Classes[iascii]-1];
          if( newState == 2 )
          {
            if( nf++ == nRow )
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
            if( nf == nRow-1 )
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
          for( int k = 0; k < nRow; ++k )
          {
            if( itype[k] != 1 )
            {
              data[k+j*nRow] = theErrorFill;
              if( DisplayMessagesCheckBox->Checked )
              {
                char c[200];
                sprintf( c, "error in field %d, record %d, file: %s\n",
                         k+1, recordNumber, CMD_read::Definition()->GetFilename().c_str() );
                Application->MessageBox( c, "Warning", MB_OK );
                return;
              }
            }
            else
            {
              data[k+j*nRow] = reals[k];
            }
          }
        }
      }
    }
  }
  else  // number of columns not given
  {
    if( FormatCheckBox->Checked )
    {
      for( int j = 0;; ++j )
      {
        ++recordNumber;
        std::string sc;
        sc.clear();
        if( !std::getline(*inStream,sc) || sc.empty() )break;
        double vd[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0};
        int result = sscanf( sc.c_str(), format.c_str(), vd, vd+1, vd+2, vd+3, vd+5,
                             vd+6, vd+7, vd+8, vd+9, vd+10, vd+11, vd+12, vd+13, vd+14,
                             vd+15, vd+16, vd+17, vd+18, vd+19, vd+20, vd+21, vd+22,
                             vd+23, vd+24, vd+25, vd+26, vd+27, vd+28, vd+29, vd+30,
                             vd+31, vd+32, vd+33, vd+34 );
        if( result == EOF || result == 0 )
        {
          char cc[200];
          sprintf( cc, "error reading record %d,\r file: %s\n",
                   recordNumber, CMD_read::Definition()->GetFilename().c_str() );
          Application->MessageBox( cc, "Fatal Error", MB_OK );
          return;
        }
        else if( result < nRow )
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
        data.resize( (j+1)*nRow );
        for( int k = 0; k < nRow; ++k )
        {
          data[k+j*nRow] = vd[k];
        }
      }
    }
    else  // no format given
    {
      for( int j = 0;; ++j )
      {
        ++recordNumber;
        std::string sc;
        sc.clear();
        if( !std::getline(*inStream,sc) || sc.empty() )break;
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
        std::vector<double> reals( nRow, 0.0 );
        std::vector<int> itype( nRow, 0 );
        std::vector<std::string> strings( nRow );
        int nf = -1;
        unsigned int newState = 1;
        unsigned int currentState = 0;
        bool flag = true;
        for( unsigned int k = 0; k < sc.length(); ++k )
        {
          currentState = newState;
          int iascii = toascii( sc[k] );
          newState = StateTable[currentState-1][Classes[iascii]-1];
          if( newState == 2 )
          {
            if( nf++ == nRow )
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
            if( nf == nRow-1 )
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
          data.resize( (j+1)*nRow );
          for( int k = 0; k < nRow; ++k )
          {
            if( itype[k] != 1 )
            {
              data[k+j*nRow] = theErrorFill;
              if( DisplayMessagesCheckBox->Checked )
              {
                char c[200];
                sprintf( c, "error in field %d, record %d, file: %s\n",
                         k+1, recordNumber, CMD_read::Definition()->GetFilename().c_str() );
                Application->MessageBox( c, "Warning", MB_OK );
                return;
              }
            }
            else
            {
              data[k+j*nRow] = reals[k];
            }
          }
        }
      }
    }
  }
  //
  // Successful read
  //
  TVariableTable::GetTable()->RemoveEntry( matrixName.c_str() );
  NVariableTable::GetTable()->RemoveEntry( matrixName.c_str() );
  //
  if( DisplayMessagesCheckBox->Checked )
  {
    std::string s( "matrix " );
    s += std::string(matrixName.c_str()) + std::string(" has been created with ");
    char c[100];
    sprintf( c, "%d rows and %d columns", nRow, nCol );
    s += c;
    Application->MessageBox( s.c_str(), "Information", MB_OK );
  }
  std::string s("READ\\SCALARS ");
  s += CMD_read::Definition()->GetFilename().c_str();
  try
  {
    NumericVariable *nv = NumericVariable::PutVariable( matrixName.c_str(), data, nRow, nCol, s );
    nv->SetOrigin( CMD_read::Definition()->GetFilename() );
  }
  catch ( EVariableError &e )
  {
    Application->MessageBox( e.what(), "Fatal Error", MB_OK );
    return;
  }
  if( CloseAfterCheckBox->Checked )inStream->close();
}

void __fastcall TReadMatForm::ChooseFileClick(TObject *Sender)
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

void __fastcall TReadMatForm::FilenameComboBoxClick(TObject *Sender)
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
      s += std::string( CMD_read::Definition()->GetFilename().c_str() );
      Application->MessageBox( s.c_str(), "Fatal Error", MB_OK );
      return;
    }
  }
}

void __fastcall TReadMatForm::CloseClick(TObject *Sender)
{
  Close();
  ReadMatForm = 0;
}

void __fastcall TReadMatForm::FormClose(TObject *Sender,TCloseAction &Action)
{ Action = caFree; }

void __fastcall TReadMatForm::FormClose2(TObject *Sender,bool &CanClose)
{ ReadMatForm = 0; }

// end of file
