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
#include <sstream>

#include "wx/filename.h"

#include "CMD_read.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "TextVariable.h"

CMD_read *CMD_read::cmd_read_ = 0;

CMD_read::CMD_read() : Command( wxT("READ") )
{
  AddQualifier( wxT("BINARY"),      false );
  AddQualifier( wxT("ASCII"),       false );
  AddQualifier( wxT("FORMAT"),      false );
  AddQualifier( wxT("ERRORSTOP"),   true  );
  AddQualifier( wxT("ERRORSKIP"),   false );
  AddQualifier( wxT("ERRORFILL"),   false );
  AddQualifier( wxT("FAST"),        false );
  AddQualifier( wxT("CLOSEFIRST"),  true  );
  AddQualifier( wxT("CLOSEAFTER"),  true  );
  AddQualifier( wxT("SCALARS"),     false );
  AddQualifier( wxT("MATRIX"),      false );
  AddQualifier( wxT("TENSOR"),      false );
  AddQualifier( wxT("VECTORS"),     true  );
  AddQualifier( wxT("TEXT"),        false );
  AddQualifier( wxT("MESSAGES"),    true  );
  AddQualifier( wxT("APPEND"),      false );
  AddQualifier( wxT("OVERLAY"),     false );
  AddQualifier( wxT("EXTEND"),      true  );
  AddQualifier( wxT("COMMAS"),      false ); // no longer used
  errorFill_ = 0.0;
}

void CMD_read::Execute( ParseLine const *p )
{
  //  ascii files:
  //
  // READ\MATRIX FILE{\range} matrix\(nc{,nr})
  // READ\MATRIX\FORMAT FILE{\range} (frmt) matrix/(nc{,nr})
  // READ\SCALAR FILE{\line} s1/{ c1 } ... sN/{ cN }
  // READ\SCALAR\FORMAT FILE{\line} (frmt) s1/{ c1 } ... sN/{ cN }
  // READ\VECTOR FILE{\range} x1/{ c1 } ... xN/{ cN }
  // READ\VECTOR\FORMAT FILE{\range} (frmt) x1/{ c1 } ... xN/{ cN }
  // READ\TEXT FILE{\range} tvar
  //
  //  unformatted binary files:
  //
  // READ\MATRIX FILE{\range} (frmt) matrix/(nc{,nr})
  // READ\SCALAR FILE{\line} (frmt) s1/{ c1 } ... sN/{ cN }
  // READ\VECTOR FILE{\range} (frmt) x1/{ c1 } ... xN/{ cN }
  //
  // where frmt is really a "prescription" for reading the unformatted file. If frmt
  // is of the form  nB  for n=1,2,4,8  then a stream read is done. The current
  // length of the input variables is used, since the records are variable length.
  // If frmt is of the form  R4,I2,R8, etc. then the file is read by record with the
  //record length implied by the specified number of bytes.
  //
  //   A file can be ASCII or UNFORMATTED (binary)
  //   ASCII:       no format --> my free format (parsed records)
  //                (*)       --> read each record with FORTRAN free format
  //                (format)  --> read each record with this format
  //   UNFORMATTED: (format)  --> if BINDEC format, e.g. (R4,I2), read by record
  //                (format)  --> if nB format, e.g. (4B), stream read
  //
  QualifierMap qualifiers, qualifierEntered;
  try
  {
    SetUp( p, qualifiers );
    SetUp( qualifierEntered );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  wxString command( Name()+wxT(": ") );
  startingLine_ = 1;               // starting line range value
  lineRange_.clear();
  //
  if( qualifierEntered[wxT("ASCII")] )
  {
    if( qualifiers[wxT("ASCII")] )qualifiers[wxT("BINARY")] = false;
    else                     qualifiers[wxT("BINARY")] = true;
  }
  if( qualifierEntered[wxT("ERRORSTOP")] )
  {
    if( qualifiers[wxT("ERRORSTOP")] )
    {
      qualifiers[wxT("ERRORSKIP")] = false;
      qualifiers[wxT("ERRORFILL")] = false;
    }
    else
    {
      qualifiers[wxT("ERRORSKIP")] = true;
      qualifiers[wxT("ERRORFILL")] = false;
    }
  }
  if( qualifierEntered[wxT("ERRORSKIP")] )
  {
    if( qualifiers[wxT("ERRORSKIP")] )
    {
      qualifiers[wxT("ERRORSTOP")] = false;
      qualifiers[wxT("ERRORFILL")] = false;
      qualifiers[wxT("FAST")]  = false;
    }
    else
    {
      qualifiers[wxT("ERRORSTOP")] = true;
      qualifiers[wxT("ERRORFILL")] = false;
      qualifiers[wxT("FAST")]  = false;
    }
  }
  if( qualifierEntered[wxT("ERRORFILL")] )
  {
    if( qualifiers[wxT("ERRORFILL")] )
    {
      qualifiers[wxT("ERRORSKIP")] = false;
      qualifiers[wxT("ERRORSTOP")] = false;
      qualifiers[wxT("FAST")]  = false;
    }
    else
    {
      qualifiers[wxT("ERRORSKIP")] = false;
      qualifiers[wxT("ERRORSTOP")] = true;
      qualifiers[wxT("FAST")]  = false;
    }
  }
  if( qualifiers[wxT("SCALARS")] )
  {
    qualifiers[wxT("MATRIX")]  = false;
    qualifiers[wxT("TENSOR")]  = false;
    qualifiers[wxT("VECTORS")] = false;
    qualifiers[wxT("TEXT")]    = false;
  }
  if( qualifiers[wxT("MATRIX")] )
  {
    qualifiers[wxT("SCALARS")] = false;
    qualifiers[wxT("TENSOR")]  = false;
    qualifiers[wxT("VECTORS")] = false;
    qualifiers[wxT("TEXT")]    = false;
  }
  if( qualifiers[wxT("TENSOR")] )
  {
    qualifiers[wxT("SCALARS")] = false;
    qualifiers[wxT("MATRIX")]  = false;
    qualifiers[wxT("VECTORS")] = false;
    qualifiers[wxT("TEXT")]    = false;
  }
  if( qualifiers[wxT("TEXT")] )
  {
    qualifiers[wxT("SCALARS")] = false;
    qualifiers[wxT("MATRIX")]  = false;
    qualifiers[wxT("TENSOR")]  = false;
    qualifiers[wxT("VECTORS")] = false;
  }
  if( qualifiers[wxT("VECTORS")] )
  {
    qualifiers[wxT("SCALARS")] = false;
    qualifiers[wxT("MATRIX")]  = false;
    qualifiers[wxT("TENSOR")]  = false;
    qualifiers[wxT("TEXT")]    = false;
  }
  bool output = qualifiers[wxT("MESSAGES")] || ExGlobals::NotInaScript() || ExGlobals::GetEcho();
  if( !qualifiers[wxT("MESSAGES")] )output = false;
  //
  if( !p->IsString(1) )throw ECommandError( command+wxT("filename not entered") );
  wxString fileName;
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, fileName );
  }
  catch (EVariableError &e)
  {
    throw;
  }
  AddToStackLine( wxString(wxT("'"))+fileName+wxString(wxT("'")) );
  //
  wxFileName wxfn( fileName );
  if( wxfn.GetPath().empty() )fileName = ExGlobals::GetCurrentPath() + wxfn.GetPathSeparator() + fileName;
  //
  bool newFile = (fileName!=readInFilename_);
  readInFilename_ = fileName;
  if( inStream_.is_open() && (qualifiers[wxT("CLOSEFIRST")] || newFile) )inStream_.close();
  //
  // for stream writen files, the user must create the ouput vectors before the read
  // command, thus specifying the record length for files written by record, we just
  // read till the end of file
  //
  if( !inStream_.is_open() )
  {
    inStream_.clear( std::ios::goodbit );
    if( qualifiers[wxT("BINARY")] )
      inStream_.open( fileName.mb_str(wxConvUTF8), std::ios_base::in | std::ios_base::binary );
    else
      inStream_.open( fileName.mb_str(wxConvUTF8), std::ios_base::in );
    if( !inStream_.is_open() )throw ECommandError( command+wxT("could not open file: ")+fileName );
  }
  wxString const backSlash(1,wxT('\\'));
  lineRangeFlag_ = -1;                   // -1 indicates no line range was entered
  ParseToken t( *p->GetToken(1) );
  if( t.GetNumberOfQualifiers() > 0 )      // a line range was entered
  {
    lineRangeFlag_ = 0;                  // 0 indicates a scalar line range was entered
    int ndim, dimSizes[3];
    double dum8;
    try
    {
      NumericVariable::GetVariable( *t.GetQualifier(0), ndim, dum8, lineRange_, dimSizes );
    }
    catch( EVariableError &e )
    {
      inStream_.close();
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( ndim == 0 )
    {
      startingLine_ = static_cast<int>(dum8);
      if( startingLine_ < 1 )
      {
        inStream_.close();
        throw ECommandError( command+wxT("record number < 1") );
      }
    }
    else if( ndim == 1 )
    {
      if( qualifiers[wxT("SCALARS")] )
      {
        inStream_.close();
        throw ECommandError( command+wxT("cannot specify vector line range for scalars") );
      }
      if( qualifiers[wxT("MATRIX")] )
      {
        inStream_.close();
        throw ECommandError( command+wxT("cannot specify vector line range for a matrix") );
      }
      startingLine_ = static_cast<int>(lineRange_[0]);
      if( startingLine_ < 1 )
      {
        inStream_.close();
        throw ECommandError( command+wxT("initial record number < 1") );
      }
      int idum = startingLine_;
      lineRangeFlag_ = 1;             // indicates a vector line range was entered
      for( int i=1; i<dimSizes[0]; ++i )
      {
        if( static_cast<int>(lineRange_[i]) <= idum )
        {
          inStream_.close();
          throw ECommandError( command+(*t.GetQualifier(0))+wxT(" is not monotonically increasing") );
        }
        idum = static_cast<int>(lineRange_[i]);
      }
    }
    else if( ndim == 2 )
    {
      inStream_.close();
      throw ECommandError( command+(*t.GetQualifier(0))+wxT(" is a matrix") );
    }
    else if( ndim == 3 )
    {
      inStream_.close();
      throw ECommandError( command+(*t.GetQualifier(0))+wxT(" is a tensor") );
    }
    AddToStackLineQualifier( *t.GetQualifier(0) );
  }
  wxString s( wxT("READ") );
  if( qualifiers[wxT("SCALARS")] )
  {
    s += backSlash + wxString(wxT("SCALARS"));
    if( qualifiers[wxT("ERRORSKIP")] )
    {
      ExGlobals::WarningMessage( s + wxT("ERRSKIP is meaningless when reading one record") );
      qualifiers[wxT("ERRORSKIP")] = false;
      qualifiers[wxT("ERRORSTOP")] = true;
    }
    if( qualifiers[wxT("ERRORFILL")] && qualifiers[wxT("FORMAT")] )
    {
      ExGlobals::WarningMessage( s + wxT("ERRORFILL cannot be used with a format")  );
      qualifiers[wxT("ERRORFILL")] = false;
      qualifiers[wxT("ERRORSTOP")] = true;
    }
    if( qualifiers[wxT("OVERLAY")] )
    {
      ExGlobals::WarningMessage( s + wxT("OVERLAY is meaningless when reading scalars") );
    }
    if( qualifiers[wxT("APPEND")] )
    {
      ExGlobals::WarningMessage( s + wxT("APPEND is meaningless when reading scalars") );
    }
    if( !qualifiers[wxT("EXTEND")] )
    {
      ExGlobals::WarningMessage( s + wxT("-EXTEND is meaningless when reading scalars") );
    }
    try
    {
      ReadScalars( qualifiers, output );
    }
    catch (ECommandError &e)
    {
      inStream_.close();
      throw;
    }
  }
  else if( qualifiers[wxT("TEXT")] )
  {
    s += backSlash + wxString(wxT("TEXT"));
    if( qualifiers[wxT("ERRORFILL")] )
      ExGlobals::WarningMessage( s + wxT("ERRORFILL is meaningless when reading text") );
    if( qualifiers[wxT("ERRORSKIP")] )
      ExGlobals::WarningMessage( s + wxT("ERRSKIP is meaningless when reading text") );
    if( qualifiers[wxT("OVERLAY")] )
      ExGlobals::WarningMessage( s + wxT("OVERLAY is meaningless when reading text") );
    if( qualifiers[wxT("APPEND")] )
      ExGlobals::WarningMessage( s + wxT("APPEND is meaningless when reading text") );
    if( !qualifiers[wxT("EXTEND")] )
      ExGlobals::WarningMessage( s + wxT("-EXTEND is meaningless when reading text") );
    try
    {
      ReadText( qualifiers, output );
    }
    catch (ECommandError &e)
    {
      inStream_.close();
      throw;
    }
  }
  else if( qualifiers[wxT("MATRIX")] )
  {
    s += backSlash + wxString(wxT("MATRIX"));
    if( qualifiers[wxT("ERRORFILL")] )
      ExGlobals::WarningMessage( s + wxT("ERRORFILL is meaningless when reading a matrix") );
    if( qualifiers[wxT("ERRORSKIP")] )
      ExGlobals::WarningMessage( s + wxT("ERRSKIP is meaningless when reading a matrix") );
    if( qualifiers[wxT("OVERLAY")] )
      ExGlobals::WarningMessage( s + wxT("OVERLAY is meaningless when reading a matrix") );
    if( qualifiers[wxT("APPEND")] )
      ExGlobals::WarningMessage( s + wxT("APPEND is meaningless when reading a matrix") );
    if( !qualifiers[wxT("EXTEND")] )
      ExGlobals::WarningMessage( s + wxT("-EXTEND is meaningless when reading a matrix") );
    try
    {
      ReadMatrix( qualifiers, output );
    }
    catch (ECommandError &e)
    {
      inStream_.close();
      throw;
    }
  }
  else if( qualifiers[wxT("TENSOR")] )
  {
    s += backSlash + wxString(wxT("TENSOR"));
    if( qualifiers[wxT("ERRORFILL")] )
      ExGlobals::WarningMessage( s + wxT("ERRORFILL is meaningless when reading a tensor") );
    if( qualifiers[wxT("ERRORSKIP")] )
      ExGlobals::WarningMessage( s + wxT("ERRSKIP is meaningless when reading a tensor") );
    if( qualifiers[wxT("OVERLAY")] )
      ExGlobals::WarningMessage( s + wxT("OVERLAY is meaningless when reading a tensor") );
    if( qualifiers[wxT("APPEND")] )
      ExGlobals::WarningMessage( s + wxT("APPEND is meaningless when reading a tensor") );
    if( !qualifiers[wxT("EXTEND")] )
      ExGlobals::WarningMessage( s + wxT("-EXTEND is meaningless when reading a tensor") );
    try
    {
      ReadTensor( qualifiers, output );
    }
    catch (ECommandError &e)
    {
      inStream_.close();
      throw;
    }
  }
  else if( qualifiers[wxT("VECTORS")] )                        // must be vectors
  {
    s += backSlash + wxString(wxT("VECTORS"));
    if( qualifiers[wxT("BINARY")] && qualifiers[wxT("ERRORFILL")] )
      ExGlobals::WarningMessage( s + wxT("ERRORFILL cannot be used with UNFORMATTED") );
    if( qualifiers[wxT("BINARY")] && qualifiers[wxT("ERRORSKIP")] )
      ExGlobals::WarningMessage( s + wxT("ERRSKIP cannot be used with UNFORMATTED") );
    if( !qualifiers[wxT("EXTEND")] && qualifiers[wxT("APPEND")] )
      ExGlobals::WarningMessage( s + wxT("-EXTEND cannot be used with APPEND") );
    try
    {
      ReadVectors( qualifiers, output );
    }
    catch (ECommandError &e)
    {
      inStream_.close();
      throw;
    }
  }
  if( qualifiers[wxT("CLOSEAFTER")] )inStream_.close();
}

void CMD_read::ReadScalars( QualifierMap &qualifiers, bool const output )
{
  // ascii files:
  //
  // READ\SCALARS FILE{\line} s1\{c1} ... sN\{cN}
  // READ\SCALARS\FORMAT FILE{\line} format s1 ... sN
  //
  wxString command( Name()+wxT(": ") );
  ParseLine const *p = GetParseLine();
  //
  int tcnt = 2;
  wxString format;
  if( qualifiers[wxT("FORMAT")] )
  {
    if( !p->IsString(2) )throw ECommandError( command+wxT("expecting format string") );
    format = p->GetString(2);
    format = format.substr( 1, format.length()-2 );
    ++tcnt;
    AddToStackLine( format );
  }
  std::vector<wxString> names;
  std::vector<int> columnNumber;
  for( int tokenCounter=tcnt; tokenCounter<p->GetNumberOfTokens(); ++tokenCounter )
  {
    if( !p->IsString(tokenCounter) )throw ECommandError( command+wxT("expecting output scalar names") );
    wxString name( p->GetString(tokenCounter) );
    AddToStackLine( name );
    names.push_back( name );
    //
    ParseToken t( *p->GetToken(tokenCounter) );
    if( t.GetNumberOfQualifiers() > 0 )       // a column number was specified
    {
      if( qualifiers[wxT("FORMAT")] )
        throw ECommandError( command+wxT("cannot have column numbers with a format") );
      wxString q( *t.GetQualifier(0) );
      int ndim, dimSizes[3];
      double dum8;
      std::vector<double> d;
      try
      {
        NumericVariable::GetVariable( *t.GetQualifier(0), ndim, dum8, d, dimSizes );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( ndim != 0 )
        throw ECommandError( command+wxT("column specifier: ")+(*t.GetQualifier(0))+
                             wxT(" is not a scalar") );
      if( static_cast<int>(dum8) < 1 )throw ECommandError( command+wxT("column number < 1") );
      columnNumber.push_back( static_cast<int>(dum8) );
      AddToStackLineQualifier( *t.GetQualifier(0) );
    }
    else             // column number NOT specified
    {
      columnNumber.push_back( names.size() );
    }
  }
  if( names.empty() )throw ECommandError( command+wxT("expecting output scalar names") );
  int maxColumnNumber = 0;
  std::size_t numberOfNames = names.size();
  for( std::size_t j=0; j<numberOfNames; ++j )
  {
    if( maxColumnNumber < columnNumber[j] )maxColumnNumber = columnNumber[j];
    TVariableTable::GetTable()->RemoveEntry( names[j] );
    NVariableTable::GetTable()->RemoveEntry( names[j] );
  }
  //
  // read in the initial dummy records
  //
  unsigned int recordNumber = 0;
  for( int i=1; i<startingLine_; ++i )
  {
    ++recordNumber;
    std::string buffer;
    if( std::getline(inStream_,buffer).fail() )
      throw ECommandError( command+
                           wxT("end of file reached reading initial dummy records, file: ")+
                           readInFilename_ );
  }
  ++recordNumber;
  //
  // finally ready to read some data
  //
  std::vector<double> data( maxColumnNumber );
  if( qualifiers[wxT("FORMAT")] )
  {
    std::string sc;
    sc.clear();
    if( std::getline(inStream_,sc).fail() || sc.empty() )
    {
      wxString s( wxT("record ") );
      throw ECommandError( command+(s<<recordNumber)+wxT(", file: ")+
                           readInFilename_+wxT(", is empty") );
    }
    double vd[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                   0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                   0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                   0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                   0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                   0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                   0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    int result = sscanf( sc.c_str(), format.mb_str(wxConvUTF8),
                         vd, vd+1, vd+2, vd+3, vd+4, vd+5,
                         vd+6, vd+7, vd+8, vd+9, vd+10, vd+11, vd+12, vd+13, vd+14,
                         vd+15, vd+16, vd+17, vd+18, vd+19, vd+20, vd+21, vd+22,
                         vd+23, vd+24, vd+25, vd+26, vd+27, vd+28, vd+29, vd+30,
                         vd+31, vd+32, vd+33, vd+34, vd+35, vd+36, vd+37, vd+38,
                         vd+39, vd+40, vd+41, vd+42, vd+43, vd+44, vd+45, vd+46,
                         vd+47, vd+48, vd+49, vd+50, vd+51, vd+52, vd+53, vd+54,
                         vd+55, vd+56, vd+57, vd+58, vd+59, vd+60, vd+61, vd+62,
                         vd+63, vd+64, vd+65, vd+66, vd+67, vd+68, vd+69, vd+70,
                         vd+71, vd+72, vd+73, vd+74, vd+75, vd+76, vd+77, vd+78,
                         vd+79, vd+80, vd+81, vd+81, vd+83, vd+84, vd+85, vd+86,
                         vd+87, vd+88, vd+89, vd+90, vd+91, vd+92, vd+93, vd+94,
                         vd+95, vd+96, vd+97, vd+98, vd+99 );
    if( result==EOF || result==0 )
    {
      wxString s( wxT("error reading record ") );
      throw ECommandError( command+(s << recordNumber)+wxT(", file: ")+readInFilename_ );
    }
    else if( result < maxColumnNumber )
    {
      wxString s( wxT("not enough values on record ") );
      throw ECommandError( command+(s << recordNumber)+wxT(", file: ")+readInFilename_ );
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
    if( std::getline(inStream_,sc).fail() || sc.empty() )
    {
      wxString c( wxT("record ") );
      throw ECommandError( command+(c << recordNumber)+wxT(", file: ")+
                           readInFilename_+wxT(", is empty") );
    }
    //
    // trim leading and trailing blanks
    //
    std::string const blank( " \t" );
    std::size_t rTemp = sc.find_last_not_of( blank );
    if( rTemp != sc.npos )
    {
      std::size_t lTemp = sc.find_first_not_of( blank );
      if( lTemp != sc.npos )sc = sc.substr( lTemp, rTemp-lTemp+1 );
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
    {   // blnk  ,   othr
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
    std::vector<wxString> strings( maxColumnNumber );
    int nf = -1;
    unsigned int newState = 1;
    unsigned int currentState = 0;
    bool flag = true;
    std::size_t scLen = sc.length();
    for( std::size_t k=0; k<scLen; ++k )
    {
      currentState = newState;
      int iascii = toascii( sc[k] );
      newState = StateTable[currentState-1][Classes[iascii]-1];
      if( newState == 2 )
      {
        if( ++nf == maxColumnNumber-1 )
        {
          flag = false;
          break;
        }
      }
      else if( newState == 3 )
      {
        strings[nf+1].append( wxString(sc.c_str(),wxConvUTF8).at(k) );
      }
      else if( newState == 4 || newState == 6 )
      {
        ++nf;
        double d;
        if( strings[nf].ToDouble(&d) )
        {
          itype[nf] = 1;
          reals[nf] = d;
        }
        else itype[nf] = 2;
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
      if( strings[nf].ToDouble(&d) )
      {
        itype[nf] = 1;
        reals[nf] = d;
      }
      else itype[nf] = 2;
    }
    if( nf > 0 )
    {
      for( int k=0; k<numberOfNames; ++k )
      {
        if( itype[columnNumber[k]-1] != 1 )
        {
          if( !qualifiers[wxT("ERRORFILL")] )
            throw ECommandError( command+wxT("error reading scalars") );
          data[k] = errorFill_;
          if( output )
          {
            wxChar c[200];
            ::wxSnprintf( c, 200, wxT("error in field %d, record %d, file: %s"),
                          k+1, recordNumber, readInFilename_.c_str() );
            ExGlobals::WarningMessage( command + c );
          }
        }
        else data[k] = reals[columnNumber[k]-1];
      }
    }
  }
  //
  // finished reading
  //
  for( std::size_t j=0; j<numberOfNames; ++j )
  {
    NumericVariable *nv = NumericVariable::PutVariable( names[j], data[j], p->GetInputLine() );
    nv->SetOrigin( readInFilename_ );
    if( output )ExGlobals::WriteOutput( wxString(wxT("scalar "))+names[j]+wxT(" has been created") );
  }
}

void CMD_read::ReadMatrix( QualifierMap &qualifiers, bool const output )
{
  // READ\MATRIX FILE{\range} matrix nr nc
  // READ\MATRIX\FORMAT FILE{\range} (frmt) matrix nr nc
  //
  wxString command( Name()+wxT(": ") );
  ParseLine const *p = GetParseLine();
  //
  int tcnt = 2;
  wxString format;
  if( qualifiers[wxT("FORMAT")] )
  {
    if( !p->IsString(tcnt) )throw ECommandError( command+wxT("expecting format string") );
    AddToStackLine( p->GetString(tcnt) );
    format = p->GetString(tcnt).substr( 1, format.length()-2 );
    ++tcnt;
  }
  if( !p->IsString(tcnt) )throw ECommandError( command+wxT("expecting output matrix name") );
  wxString matrixName( p->GetString(tcnt) );
  AddToStackLine( matrixName );
  //
  // get the number of rows of the output matrix
  //
  double d;
  NumericVariable::GetScalar( p->GetString(++tcnt), wxT("number of rows"), d );
  int nRow = static_cast<int>(d);
  if( nRow < 0 )throw ECommandError( command+wxT("number of rows < 0") );
  else if( nRow == 0 )throw ECommandError( command+wxT("number of rows = 0") );
  AddToStackLine( p->GetString(tcnt) );
  //
  // get the number of columns of the output matrix
  //
  bool numberOfColumnsGiven = ( p->GetNumberOfTokens() >= tcnt+2 );
  int nCol = 1000;
  if( numberOfColumnsGiven )
  {
    NumericVariable::GetScalar( p->GetString(++tcnt), wxT("number of columns"), d );
    nCol = static_cast<int>(d);
    if( nCol < 0 )throw ECommandError( command+wxT("number of columns < 0") );
    else if( nCol == 0 )throw ECommandError( command+wxT("number of columns = 0") );
    AddToStackLine( p->GetString(tcnt) );
  }
  //
  // Read in the initial dummy records
  //
  unsigned int recordNumber = 0;
  for( int i=1; i<startingLine_; ++i )
  {
    ++recordNumber;
    std::string buffer;
    if( std::getline(inStream_,buffer).fail() )
      throw ECommandError( command+
                           wxT("end of file reached reading initial dummy records, file: ")+
                           readInFilename_ );
  }
  std::vector<double> data;
  if( numberOfColumnsGiven )
  {
    data.resize( nRow*nCol );
    if( qualifiers[wxT("FORMAT")] )
    {
      for( int j = 0; j < nCol; ++j )
      {
        ++recordNumber;
        std::string sc;
        sc.clear();
        if( std::getline(inStream_,sc).fail() || sc.empty() )
        {
          wxString c( wxT("error reading record ") );
          throw ECommandError( command+(c << recordNumber)+wxT(", file: ")+readInFilename_ );
        }
        double vd[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0};
        int result = sscanf( sc.c_str(), format.mb_str(), vd, vd+1, vd+2, vd+3, vd+4, vd+5,
                             vd+6, vd+7, vd+8, vd+9, vd+10, vd+11, vd+12, vd+13, vd+14,
                             vd+15, vd+16, vd+17, vd+18, vd+19, vd+20, vd+21, vd+22,
                             vd+23, vd+24, vd+25, vd+26, vd+27, vd+28, vd+29, vd+30,
                             vd+31, vd+32, vd+33, vd+34 );
        if( result == EOF || result == 0 )
        {
          wxString cc( wxT("error reading record ") );
          throw ECommandError( command+(cc << recordNumber)+wxT(", file: ")+readInFilename_ );
        }
        else if( result < nRow )
        {
          wxString cc( wxT("not enough values on record ") );
          throw ECommandError( command+(cc << recordNumber)+wxT(", file: ")+readInFilename_ );
        }
        //
        // no errors in reading
        //
        for( int k=0; k<nRow; ++k )data[k+j*nRow] = vd[k];
      }
    }
    else  // no format given
    {
      for( int j=0; j<nCol; ++j )
      {
        ++recordNumber;
        std::string sc;
        sc.clear();
        if( std::getline(inStream_,sc).fail() || sc.empty() )
        {
          wxString c( wxT("error reading record ") );
          throw ECommandError( command+(c << recordNumber)+wxT(", file: ")+readInFilename_ );
        }
        //
        // trim leading and trailing blanks
        //
        std::string const blank( " \t" );
        std::size_t rTemp = sc.find_last_not_of( blank );
        if( rTemp != sc.npos )
        {
          std::size_t lTemp = sc.find_first_not_of( blank );
          if( lTemp != sc.npos )sc = sc.substr( lTemp, rTemp-lTemp+1 );
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
        std::vector<wxString> strings( nRow );
        int nf = -1;
        unsigned int newState = 1;
        unsigned int currentState = 0;
        bool flag = true;
        for( std::size_t k=0; k<sc.length(); ++k )
        {
          currentState = newState;
          int iascii = toascii( sc[k] );
          newState = StateTable[currentState-1][Classes[iascii]-1];
          if( newState == 2 )
          {
            if( ++nf == nRow )
            {
              flag = false;
              break;
            }
          }
          else if( newState == 3 )
          {
            strings[nf+1].append( (wxChar)sc[k] );
          }
          else if( newState == 4 || newState == 6 )
          {
            ++nf;
            double d;
            if( strings[nf].ToDouble(&d) )
            {
              itype[nf] = 1;
              reals[nf] = d;
            }
            else itype[nf] = 2;
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
          if( strings[nf].ToDouble(&d) )
          {
            itype[nf] = 1;
            reals[nf] = d;
          }
          else itype[nf] = 2;
        }
        if( nf > 0 )
        {
          for( int k=0; k<nRow; ++k )
          {
            if( itype[k] != 1 )
            {
              data[k+j*nRow] = errorFill_;
              if( output )
              {
                wxString c( wxT("error in field ") );
                c << k+1 << wxT(", record ") << recordNumber << wxT(", file: ") << readInFilename_;
                ExGlobals::WarningMessage( command+c );
              }
            }
            else data[k+j*nRow] = reals[k];
          }
        }
      }
    }
  }
  else  // number of columns not given
  {
    if( qualifiers[wxT("FORMAT")] )
    {
      for( int j=0;; ++j )
      {
        ++recordNumber;
        std::string sc;
        sc.clear();
        if( std::getline(inStream_,sc).fail() || sc.empty() )break;
        double vd[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0};
        int result = sscanf( sc.c_str(), format.mb_str(), vd, vd+1, vd+2, vd+3, vd+4, vd+5,
                             vd+6, vd+7, vd+8, vd+9, vd+10, vd+11, vd+12, vd+13, vd+14,
                             vd+15, vd+16, vd+17, vd+18, vd+19, vd+20, vd+21, vd+22,
                             vd+23, vd+24, vd+25, vd+26, vd+27, vd+28, vd+29, vd+30,
                             vd+31, vd+32, vd+33, vd+34 );
        if( result == EOF || result == 0 )
        {
          wxString c( wxT("error reading record ") );
          throw ECommandError( command+(c << recordNumber)+wxT(", file: ")+readInFilename_ );
        }
        else if( result < nRow )
        {
          wxString cc( wxT("not enough values on record ") );
          throw ECommandError( command+(cc << recordNumber)+wxT(", file: ")+readInFilename_ );
        }
        //
        // no errors in reading
        //
        data.resize( (j+1)*nRow );
        for( int k=0; k<nRow; ++k )data[k+j*nRow] = vd[k];
      }
    }
    else  // no format given
    {
      for( int j=0;; ++j )
      {
        ++recordNumber;
        std::string sc;
        sc.clear();
        if( std::getline(inStream_,sc).fail() || sc.empty() )break;
        //
        // trim leading and trailing blanks
        //
        std::string const blank( " \t" );
        std::size_t rTemp = sc.find_last_not_of( blank );
        if( rTemp != sc.npos )
        {
          std::size_t lTemp = sc.find_first_not_of( blank );
          if( lTemp != sc.npos )sc = sc.substr( lTemp, rTemp-lTemp+1 );
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
        std::vector<wxString> strings( nRow );
        int nf = -1;
        unsigned int newState = 1;
        unsigned int currentState = 0;
        bool flag = true;
        for( std::size_t k=0; k<sc.length(); ++k )
        {
          currentState = newState;
          int iascii = toascii( sc[k] );
          newState = StateTable[currentState-1][Classes[iascii]-1];
          if( newState == 2 )
          {
            if( ++nf == nRow )
            {
              flag = false;
              break;
            }
          }
          else if( newState == 3 )
          {
            strings[nf+1].append( wxString(sc.c_str(),wxConvUTF8).at(k) );
          }
          else if( newState == 4 || newState == 6 )
          {
            ++nf;
            double d;
            if( strings[nf].ToDouble(&d) )
            {
              itype[nf] = 1;
              reals[nf] = d;
            }
            else itype[nf] = 2;
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
          if( strings[nf].ToDouble(&d) )
          {
            itype[nf] = 1;
            reals[nf] = d;
          }
          else itype[nf] = 2;
        }
        if( nf > 0 )
        {
          data.resize( (j+1)*nRow );
          for( int k=0; k<nRow; ++k )
          {
            if( itype[k] != 1 )
            {
              data[k+j*nRow] = errorFill_;
              if( output )
              {
                wxString c( wxT("error in field ") );
                c << k+1 << wxT(", record ") << recordNumber << wxT(", file: ") << readInFilename_;
                ExGlobals::WarningMessage( command+c );
              }
            }
            else data[k+j*nRow] = reals[k];
          }
        }
      }
    }
  }
  //
  // Successful read
  //
  TVariableTable::GetTable()->RemoveEntry( matrixName );
  NVariableTable::GetTable()->RemoveEntry( matrixName );
  NumericVariable *nv = NumericVariable::PutVariable( matrixName, data,
                                                      static_cast<std::size_t>(nRow),
                                                      static_cast<std::size_t>(nCol),
                                                      p->GetInputLine() );
  nv->SetOrigin( readInFilename_ );
  if( output )
  {
    wxString s( wxT("matrix ") );
    ExGlobals::WriteOutput( s << matrixName << wxT(" has been created with ") << nRow
                              << wxT(" rows and ") << nCol << wxT(" columns") );
  }
}

void CMD_read::ReadText( QualifierMap &qualifiers, bool const output )
{
  //
  // READ\TEXT FILE{\range} tvar
  //
  wxString command( Name()+wxT(": ") );
  ParseLine const *p = GetParseLine();
  //
  int tcnt = 2;
  if( !p->IsString(tcnt) )throw ECommandError( command+wxT("expecting output text variable name") );
  wxString textName( p->GetString(tcnt) );
  AddToStackLine( textName );
  //
  // Read in the initial dummy records
  //
  int last = startingLine_+1; // last record read + 1
  //
  // there could only be initial dummy records if a line range was specified
  // and line ranges can only be on files with a record structure
  //
  unsigned int recordNumber = 0;
  for( int i=1; i<startingLine_; ++i )
  {
    ++recordNumber;
    std::string buffer;
    if( std::getline(inStream_,buffer).fail() )
      throw ECommandError( command+
                           wxT("end of file reached reading initial dummy records, file: ")+
                           readInFilename_ );
  }
  std::vector<wxString> data;
  for( std::size_t j=1;; ++j )
  {
    ++recordNumber;
    std::string sc;
    sc.clear();
    if( std::getline(inStream_,sc).fail() )break;
    if( sc.empty() )
    {
      sc = " ";
      if( lineRangeFlag_ != 1 )break;
      continue; // the record is empty, nothing on the line
    }
    data.push_back( wxString(sc.c_str(),wxConvUTF8) );
    if( lineRangeFlag_ != 1 )break;
    //
    // vector line range
    //
    bool stopReading = false;
    if( j >= lineRange_.size() )break; // stop reading
    int next = static_cast<int>(lineRange_[j]);
    for( int i=last; i<next; ++i )
    {
      ++recordNumber;
      std::string buffer;
      if( std::getline(inStream_,buffer).fail() )
      {
        stopReading = true;
        break;
      }
    }
    if( stopReading )break; // stop reading
    last = next+1;
  }
  TextVariable *tv = TextVariable::PutVariable( textName, data, p->GetInputLine() );
  tv->SetOrigin( readInFilename_ );
  if( output )ExGlobals::WriteOutput( wxString(wxT("text variable "))+textName+wxT(" has been created") );
}

void CMD_read::ReadVectors( QualifierMap &qualifiers, bool const output )
{
  // ascii files:
  //
  // READ\VECTORS FILE{\range} x1\{c1} ... xN\{cN}
  // READ\VECTORS\FORMAT FILE{\range} (format) x1 ... xN
  //
  wxString command( Name()+wxT(": ") );
  ParseLine const *p = GetParseLine();
  //
  int tcnt = 2;
  wxString format;
  if( qualifiers[wxT("FORMAT")] )
  {
    if( !p->IsString(2) )throw ECommandError( command+wxT("expecting format string") );
    format = p->GetString(2);
    format = format.substr( 1, format.length()-2 );
    ++tcnt;
  }
  std::vector<wxString> names;
  std::vector<int> id;
  std::vector<int> columnNumber;
  for( int tokenCounter=tcnt; tokenCounter<p->GetNumberOfTokens(); ++tokenCounter )
  {
    if( !p->IsString(tokenCounter) )throw ECommandError( command+wxT("expecting output vector names") );
    wxString name( p->GetString(tokenCounter) );
    AddToStackLine( name );
    //
    // determine if the vector name has a field count prefix
    //
    int numberOfDigits = 0;
    while( (toascii(name[numberOfDigits]) >= 48) &&
           (toascii(name[numberOfDigits]) <= 57) )++numberOfDigits;
    int fieldCount = 0;
    if( numberOfDigits > 0 ) // there is a field count, strip it off the name
    {
      double dum8;
      std::stringstream ss;
      wxString tmp( name.substr(0,numberOfDigits) );
      ss << tmp.mb_str(wxConvUTF8);
      ss >> dum8;
      fieldCount = static_cast<int>(dum8);
      if( fieldCount < 1 )
      {
        inStream_.close();
        throw ECommandError( command+wxT("field count < 1") );
      }
      name.assign( name.substr(numberOfDigits,name.length()-numberOfDigits) );
    }
    names.push_back( name );
    id.push_back( 0 );
    std::size_t namesSize = names.size();
    for( std::size_t k=0; k<namesSize; ++k )
    {
      if( name == names[k] )
      {
        id.back() = k;
        break;
      }
    }
    ParseToken t( *p->GetToken(tokenCounter) );
    if( t.GetNumberOfQualifiers() > 0 )       // a column number was specified
    {
      if( qualifiers[wxT("FORMAT")] )
        throw ECommandError( command+wxT("cannot have column numbers with a format") );
      double dum8;
      int ndim;
      int dimSizes[3];
      std::vector<double> d;
      wxString q( *t.GetQualifier(0) );
      try
      {
        NumericVariable::GetVariable( q, ndim, dum8, d, dimSizes );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( ndim != 0 )
        throw ECommandError( command+wxT("column specifier: ")+q+wxT(" is not a scalar") );
      if( static_cast<int>(dum8) < 1 )
        throw ECommandError( command+wxT("column number < 1") );
      columnNumber.push_back( static_cast<int>(dum8) );
      AddToStackLineQualifier( q );
    }
    else             // column number NOT specified
    {
      columnNumber.push_back( static_cast<int>(namesSize) );
    }
    for( int k=0; k<fieldCount-1; ++k )
    {
      id.push_back( id.back() );
      columnNumber.push_back( columnNumber.back()+1 );
    }
  }
  if( names.empty() )
  {
    inStream_.close();
    throw ECommandError( command+wxT("expecting output vector names") );
  }
  int maxColumnNumber = 0;
  std::vector<int> idd;   // indices of unique output vector names
  std::size_t size = id.size();
  for( std::size_t j=0; j<size; ++j )
  {
    if( maxColumnNumber < columnNumber[j] )maxColumnNumber = columnNumber[j];
    if( id[j] == static_cast<int>(j) )idd.push_back( j );
  }
  std::vector<unsigned int> nloc( id.size(), 0 );  // number of values to read
  unsigned int nlocMin = 0;
  if( !qualifiers[wxT("EXTEND")] || qualifiers[wxT("FAST")] ) // do not extend the current length
  {
    std::size_t iddsize = idd.size();
    for( std::size_t j=0; j<iddsize; ++j ) // loop over unique names
    {
      TextVariable *tv = TVariableTable::GetTable()->GetVariable( names[idd[j]] );
      if( tv ) // text variable exists already
      {
        if( qualifiers[wxT("FAST")] )
          throw ECommandError( command+names[idd[j]]+
             wxT(" is a text variable, output vectors must pre-exist with FAST qualifier") );
        TVariableTable::GetTable()->RemoveEntry( names[idd[j]] );
      }
      NumericVariable *nv = NVariableTable::GetTable()->GetVariable( names[idd[j]] );
      if( nv ) // variable already exists
      {
        if( nv->GetData().GetNumberOfDimensions() == 1 ) // it is a vector
        {
          nloc[idd[j]] = nv->GetData().GetDimMag(0);
        }
        else
        {
          if( qualifiers[wxT("FAST")] )
            throw ECommandError( command+names[idd[j]]+
                                 wxT(" is not a vector, output vectors must pre-exist with FAST qualifier") );
          NVariableTable::GetTable()->RemoveEntry( nv );
          nv = 0;
        }
      }
    }
    if( qualifiers[wxT("FAST")] )
    {
      nlocMin = nloc[idd[0]];
      std::size_t iddsize = idd.size();
      for( std::size_t j=1; j<iddsize; ++j )
      {
        if( nlocMin > nloc[idd[j]] )nlocMin = nloc[idd[j]];
      }
    }
  }
  else  // read as much as there is
  {
    if( qualifiers[wxT("OVERLAY")] || qualifiers[wxT("APPEND")] )  // use current vector
    {
      std::size_t iddsize = idd.size();
      for( std::size_t j=0; j<iddsize; ++j )
      {
        TVariableTable::GetTable()->RemoveEntry( names[idd[j]] );
        NumericVariable *nv = NVariableTable::GetTable()->GetVariable( names[idd[j]] );
        if( nv && nv->GetData().GetNumberOfDimensions()==1 )nloc[idd[j]] = nv->GetData().GetDimMag(0);
      }
    }
    else                                      // make new vector
    {
      std::size_t iddsize = idd.size();
      for( std::size_t j=0; j<iddsize; ++j )
      {
        TVariableTable::GetTable()->RemoveEntry( names[idd[j]] );
        NVariableTable::GetTable()->RemoveEntry( names[idd[j]] );
      }
    }
  }
  //
  std::vector<bool> fill( idd.size(), true );
  //
  // read in the initial dummy records
  //
  int last = startingLine_+1; // last record read + 1
  //
  // there could only be initial dummy records if a line range was specified
  // and line ranges can only be on files with a record structure
  //
  unsigned int recordNumber = 0;
  for( int i=1; i<startingLine_; ++i )
  {
    ++recordNumber;
    std::string buffer;
    if( std::getline(inStream_,buffer).fail() )
      throw ECommandError( command+
                           wxT("end of file reached reading initial dummy records, file: ")+
                           readInFilename_ );
  }
  //
  // finally ready to read some data
  //
  std::vector< std::vector<double> > data( maxColumnNumber );
  if( qualifiers[wxT("FAST")] )
  {
    if( maxColumnNumber > 100 )throw ECommandError(
      command+wxT("maximum number of vectors that can be read is 100, file: ")+
      readInFilename_ );
    std::size_t idsize = id.size();
    wxString formatSP( wxT("%lf") );
    wxString formatCM( wxT("%lf") );
    for( std::size_t j=1; j<maxColumnNumber; ++j )
    {
      formatCM += wxT(",%lf");
      formatSP += wxT(" %lf");
    }
    std::string sc;
    for( int j=1;; ++j )
    {
      ++recordNumber;
      sc.clear();
      if( std::getline(inStream_,sc).fail() )break;
      if( sc.empty() )continue; // the record is empty, nothing on the line
      format = sc.find(',')==sc.npos ? formatSP : formatCM;
      double vd[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                     0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
      int result = sscanf( sc.c_str(), format.mb_str(), vd, vd+1, vd+2, vd+3, vd+4, vd+5,
                           vd+6, vd+7, vd+8, vd+9, vd+10, vd+11, vd+12, vd+13, vd+14,
                           vd+15, vd+16, vd+17, vd+18, vd+19, vd+20, vd+21, vd+22,
                           vd+23, vd+24, vd+25, vd+26, vd+27, vd+28, vd+29, vd+30,
                           vd+31, vd+32, vd+33, vd+34, vd+35, vd+36, vd+37, vd+38,
                           vd+39, vd+40, vd+41, vd+42, vd+43, vd+44, vd+45, vd+46,
                           vd+47, vd+48, vd+49, vd+50, vd+51, vd+52, vd+53, vd+54,
                           vd+55, vd+56, vd+57, vd+58, vd+59, vd+60, vd+61, vd+62,
                           vd+63, vd+64, vd+65, vd+66, vd+67, vd+68, vd+69, vd+70,
                           vd+71, vd+72, vd+73, vd+74, vd+75, vd+76, vd+77, vd+78,
                           vd+79, vd+80, vd+81, vd+81, vd+83, vd+84, vd+85, vd+86,
                           vd+87, vd+88, vd+89, vd+90, vd+91, vd+92, vd+93, vd+94,
                           vd+95, vd+96, vd+97, vd+98, vd+99 );
      if( result == EOF )
      {
        wxString message( command+wxT("error reading record ") );
        message << recordNumber << wxT(", file: ") << readInFilename_;
        if( qualifiers[wxT("ERRORSTOP")] )throw ECommandError( message );
        if( qualifiers[wxT("MESSAGES")] )ExGlobals::WarningMessage( message );
      }
      else if( result == 0 )
      {
        wxString message( command+wxT("error reading record ") );
        message << recordNumber << wxT(", file: ") << readInFilename_;
        if( qualifiers[wxT("ERRORSTOP")] )throw ECommandError( message );
        if( qualifiers[wxT("MESSAGES")] )ExGlobals::WarningMessage( message );
      }
      else if( result < static_cast<int>(id.size()) )
      {
        wxString message( command+wxT("not enough values on record ") );
        message << recordNumber << wxT(", file: ") << readInFilename_;
        if( qualifiers[wxT("ERRORSTOP")] )throw ECommandError( message );
        if( qualifiers[wxT("MESSAGES")] )ExGlobals::WarningMessage( message );
      }
      else // no errors in reading
      {
        std::size_t idsize = id.size();
        for( std::size_t k=0; k<idsize; ++k )data[id[k]].push_back( vd[columnNumber[k]-1] );
        if( data[id[0]].size() == nlocMin )break;
      }
      if( lineRangeFlag_ == 1 ) // vector line range
      {
        bool stopReading = false;
        if( j >= static_cast<int>(lineRange_.size()) )break; // stop reading
        int next = static_cast<int>(lineRange_[j]);
        for( int i=last; i<next; ++i )
        {
          ++recordNumber;
          if( std::getline(inStream_,sc).fail() )
          {
            stopReading = true;
            break;
          }
        }
        if( stopReading )break; // stop reading
        last = next+1;
      }
    }
  }
  else // not fast
  {
    if( qualifiers[wxT("FORMAT")] )
    {
      for( int j=1;; ++j )  // loop over records in the file
      {
        ++recordNumber;
        std::string sc;
        sc.clear();
        if( std::getline(inStream_,sc).fail() )break;
        bool readError = false;
        if( sc.empty() )continue; // the record is empty, nothing on the line
        double vd[] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
                       0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
        int result = sscanf( sc.c_str(), format.mb_str(), vd, vd+1, vd+2, vd+3, vd+4, vd+5,
                             vd+6, vd+7, vd+8, vd+9, vd+10, vd+11, vd+12, vd+13, vd+14,
                             vd+15, vd+16, vd+17, vd+18, vd+19, vd+20, vd+21, vd+22,
                             vd+23, vd+24, vd+25, vd+26, vd+27, vd+28, vd+29, vd+30,
                             vd+31, vd+32, vd+33, vd+34, vd+35, vd+36, vd+37, vd+38,
                             vd+39, vd+40, vd+41, vd+42, vd+43, vd+44, vd+45, vd+46,
                             vd+47, vd+48, vd+49, vd+50, vd+51, vd+52, vd+53, vd+54,
                             vd+55, vd+56, vd+57, vd+58, vd+59, vd+60, vd+61, vd+62,
                             vd+63, vd+64, vd+65, vd+66, vd+67, vd+68, vd+69, vd+70,
                             vd+71, vd+72, vd+73, vd+74, vd+75, vd+76, vd+77, vd+78,
                             vd+79, vd+80, vd+81, vd+81, vd+83, vd+84, vd+85, vd+86,
                             vd+87, vd+88, vd+89, vd+90, vd+91, vd+92, vd+93, vd+94,
                             vd+95, vd+96, vd+97, vd+98, vd+99 );
        if( result == EOF )
        {
          wxString c( wxT("error reading record ") );
          throw ECommandError( command+(c << recordNumber)+wxT(", file: ")+readInFilename_);
        }
        else if( result == 0 )
        {
          readError = true;
          if( !qualifiers[wxT("ERRORSTOP")] )continue;
          wxString c( wxT("error reading record ") );
          throw ECommandError( command+(c << recordNumber)+wxT(", file: ")+readInFilename_);
        }
        else if( result < static_cast<int>(id.size()) )
        {
          readError = true;
          if( !qualifiers[wxT("ERRORSTOP")] )continue;
          wxString c( wxT("not enough values on record ") );
          throw ECommandError( command+(c << recordNumber)+wxT(", file: ")+readInFilename_);
        }
        //
        // no errors in reading
        //
        std::size_t idsize = id.size();
        for( std::size_t k=0; k<idsize; ++k )
        {
          if( fill[id[k]] )
          {
            if( readError )
            {
              data[id[k]].push_back( errorFill_ );
              if( output )
              {
                wxString c( wxT("field ") );
                c << k+1 << wxT(", record ") << recordNumber
                  << wxT(" set to Error Fill value, file: ") << readInFilename_;
                ExGlobals::WarningMessage( command+c );
              }
            }
            else
            {
              data[id[k]].push_back( vd[k] );
            }
            if( data[id[k]].size() == nloc[id[k]] && !qualifiers[wxT("EXTEND")] )
             fill[id[k]] = false;
          }
        }
        bool stopReading = false;
        if( lineRangeFlag_ == 1 ) // vector line range
        {
          if( j >= static_cast<int>(lineRange_.size()) )break;
          int next = static_cast<int>(lineRange_[j-1]);
          for( int i=last; i<next; ++i )
          {
            ++recordNumber;
            std::string buffer;
            if( std::getline(inStream_,buffer).fail() )
            {
              stopReading = true;
              break;
            }
          }
          if( stopReading )break;
          last = next+1;
        }
        stopReading = true;
        for( std::size_t k=0; k<idsize; ++k )
        {
          if( fill[id[k]] )
          {
            stopReading = false;
            break;
          }
        }
        if( stopReading )break;
      }
    }
    else  // no format entered
    {
      for( int j=1;; ++j )
      {
        ++recordNumber;
        std::string sc;
        sc.clear();
        if( std::getline(inStream_,sc).fail() )break;
        //
        // trim leading and trailing blanks
        //
        std::string const blank( " \t" );
        std::size_t rTemp = sc.find_last_not_of( blank );
        if( rTemp != sc.npos )
        {
          std::size_t lTemp = sc.find_first_not_of( blank );
          if( lTemp != sc.npos )sc = sc.substr( lTemp, rTemp-lTemp+1 );
        }
        if( sc.empty() )continue; // the record is empty, nothing on the line
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
        std::vector<wxString> strings( maxColumnNumber );
        int nf = -1;
        unsigned int newState = 1;
        unsigned int currentState = 0;
        bool flag = true;
        std::size_t scLen = sc.size();
        for( std::size_t k=0; k<scLen; ++k )
        {
          currentState = newState;
          int iascii = toascii( sc[k] );
          newState = StateTable[currentState-1][Classes[iascii]-1];
          if( newState == 2 )
          {
            if( ++nf == maxColumnNumber-1 )
            {
              flag = false;
              break;
            }
          }
          else if( newState == 3 )
          {
            strings[nf+1].append( wxString(sc.c_str(),wxConvUTF8).at(k) );
          }
          else if( newState == 4 || newState == 6 )
          {
            ++nf;
            double d;
            bool test = true;
            std::stringstream ss;
            try
            {
              ss << strings[nf].mb_str(wxConvUTF8);
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
            ss << strings[nf].mb_str(wxConvUTF8);
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
        bool stopReading = false;
        if( nf >= 0 )
        {
          std::size_t idsize = id.size();
          for( std::size_t k=0; k<idsize; ++k )
          {
            if( fill[id[k]] )
            {
              if( itype[columnNumber[k]-1] != 1 )
              {
                if( qualifiers[wxT("ERRORSTOP")] )
                {
                  stopReading = true;
                  break;
                }
                data[id[k]].push_back( errorFill_ );
                if( output )
                {
                  wxString c( wxT("field ") );
                  c << k+1 << wxT(", record ") << recordNumber
                    << wxT(" set to Error Fill value, file: ") << readInFilename_;
                  ExGlobals::WarningMessage( command+c );
                }
              }
              else
              {
                data[id[k]].push_back( reals[columnNumber[k]-1] );
              }
              if( data[id[k]].size()==nloc[id[k]] && !qualifiers[wxT("EXTEND")] )fill[id[k]] = false;
            }
          }
          if( stopReading )break;
        }
        if( lineRangeFlag_ == 1 ) // vector line range
        {
          stopReading = false;
          if( j >= static_cast<int>(lineRange_.size()) )break;
          int next = static_cast<int>(lineRange_[j-1]);
          for( int i=last; i<next; ++i )
          {
            ++recordNumber;
            if( std::getline(inStream_,sc).fail() )
            {
              stopReading = true;
              break;
            }
          }
          if( stopReading )break;
          last = next+1;
        }
        stopReading = true;
        std::size_t idsize( id.size() );
        for( std::size_t k=0; k<idsize; ++k )
        {
          if( fill[id[k]] )
          {
            stopReading = false;
            break;
          }
        }
        if( stopReading )break;
      }
    }
  }
  //
  // finished reading, one of: end of file, an error, or not extending vector
  //
  bool nothingWasRead = true;
  std::size_t iddsize = idd.size();
  for( std::size_t j=0; j<iddsize; ++j )
  {
    if( data[idd[j]].size() > 0 )
    {
      NumericVariable *nv = NumericVariable::PutVariable( names[j], data[idd[j]], 0, p->GetInputLine() );
      nv->SetOrigin( readInFilename_ );
      nothingWasRead = false;
      if( output )
      {
        wxString c;
        c << data[idd[j]].size();
        if( qualifiers[wxT("APPEND")] )
          ExGlobals::WriteOutput( c+wxT(" values have been appended to vector ")+names[idd[j]] );
        else
          ExGlobals::WriteOutput( wxString(wxT("vector "))+names[idd[j]]+wxT(" has been created with ")+c+wxT(" values") );
      }
    }
  }
  if( nothingWasRead )ExGlobals::WarningMessage( command + wxT("no vectors were modified") );
}

void CMD_read::ReadTensor( QualifierMap &qualifiers, bool const output )
{
  throw ECommandError( Name()+wxT(": ")+wxT("\\TENSOR not implemented yet") );
}

// end of file
