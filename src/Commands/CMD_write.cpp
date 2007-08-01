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
#include "wx/filename.h"

#include "CMD_write.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "EVariableError.h"
#include "TextVariable.h"
#include "NumericVariable.h"

CMD_write *CMD_write::cmd_write_ = 0;

CMD_write::CMD_write() : Command( wxT("WRITE") )
{
  AddQualifier( wxT("SCALARS"), false );
  AddQualifier( wxT("VECTORS"), true  );
  AddQualifier( wxT("MATRIX"),  false );
  AddQualifier( wxT("TENSOR"),  false );
  AddQualifier( wxT("TEXT"),    false );
  AddQualifier( wxT("APPEND"),  false );
  AddQualifier( wxT("FORMAT"),  false );
  AddQualifier( wxT("BINARY"),  false );
}

void CMD_write::Execute( ParseLine const *p )
{
  // WRITE                  file          x1 { x2 ... }
  // WRITE\FORMAT           file  format  x1 { x2 ... }
  // WRITE\SCALARS          file          s1 { s2 ... }
  // WRITE\SCALARS\FORMAT   file  format  s1 { s2 ... }
  // WRITE\MATRIX           file          m1 { m2 ... }
  // WRITE\MATRIX\FORMAT    file  format  m1 { m2 ... }
  // WRITE\TENSOR         file          m1 { m2 ... }
  // WRITE\TENSOR\FORMAT  file  format  m1 { m2 ... }
  // WRITE\TEXT             file          txtvar
  // WRITE\TEXT\FORMAT      file  format  txtvar
  //
  /*
    #include <algorithm>
    #include <fstream>
    #include <iostream>

    int main()
    {
    double array[10];
    using std::fill;
    fill(array,array+10,1.0f);

    std::ofstream out("data",std::ios::binary);
    out.write(reinterpret_cast<char *>(array), sizeof array);
    out.close();

    fill(array,array+10,2.0f);

    std::ifstream in("data",std::ios::binary);
    in.read(reinterpret_cast<char *>(array), sizeof array);
    in.close();

    using std::copy;
    copy(array,array+10,std::ostream_iterator<double>(std::cout," "));

    std::cout << '\n';
    }

    writes

    1 1 1 1 1 1 1 1 1 1
  */
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  wxString command( Name()+wxT(": ") );
  if( qualifiers[wxT("FORMAT")] && qualifiers[wxT("BINARY")] )
    throw ECommandError( command+wxT("BINARY and FORMAT qualifiers are not compatible") );
  if( qualifiers[wxT("SCALARS")] || qualifiers[wxT("MATRIX")] ||
      qualifiers[wxT("TENSOR")] || qualifiers[wxT("TEXT")] )qualifiers[wxT("VECTORS")] = false;
  //
  if( !p->IsString(1) )throw ECommandError( command+wxT("no filename was entered") );
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, filename_ );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  //
  wxFileName wxfn( filename_ );
  if( wxfn.GetPath().empty() )filename_ = ExGlobals::GetCurrentPath() + wxfn.GetPathSeparator() + filename_;
  //
  if( ofStream_.is_open() )ofStream_.close();
  if( qualifiers[wxT("BINARY")] )
  {
    if( qualifiers[wxT("APPEND")] )
    {
      ofStream_.open( filename_.mb_str(),
                      std::ios_base::in | std::ios_base::out | std::ios_base::binary );
      ofStream_.seekp( 0, std::ios_base::end );
    }
    else
    {
      ofStream_.open( filename_.mb_str(), std::ios_base::out | std::ios_base::binary );
    }
  }
  else
  {
    if( qualifiers[wxT("APPEND")] )
    {
      ofStream_.open( filename_.mb_str(), std::ios_base::in | std::ios_base::out );
      ofStream_.seekp( 0, std::ios_base::end );
    }
    else
    {
      ofStream_.open( filename_.mb_str(), std::ios_base::out );
    }
  }
  AddToStackLine( p->GetString(1) );
  int icnt = 2;
  wxString format;
  if( qualifiers[wxT("FORMAT")] )
  {
    if( !p->IsString(2) )throw ECommandError( command+wxT("expecting format") );
    try
    {
      TextVariable::GetVariable( p->GetString(2), true, format );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(2) );
    icnt = 3;
  }
  if( qualifiers[wxT("TEXT")] )
  {
    if( !p->IsString(icnt) )
      throw ECommandError( command+wxT("expecting text to write") );
    wxString line;
    try
    {
      TextVariable::GetVariable( p->GetString(icnt), false, line );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(icnt) );
    if( qualifiers[wxT("FORMAT")] )
    {
      wxChar c[32000];
      ::wxSnprintf( c, 32000, format.c_str(), line.c_str() );
      ofStream_ << c << std::endl;
    }
    else
    {
      ofStream_ << line.mb_str(wxConvUTF8) << std::endl;
    }
    ofStream_.close();
    return;
  }
  //
  // write scalars, vectors, matrix, or tensor
  //
  std::vector<wxString> names;
  std::vector<double> values;
  std::vector<int> id;
  std::vector< std::vector<double> > data;
  std::vector<double> d;
  std::vector<int> nd1, nd2, nd3;
  //
  while( icnt < p->GetNumberOfTokens() )
  {
    if( !p->IsString(icnt) )
      throw ECommandError( command+( wxString(wxT("parameter ")) << icnt+1 << wxT(" is invalid")) );
    int ndm;
    double value;
    int dimSizes[3];
    std::vector<double>().swap( d );
    try
    {
      NumericVariable::GetVariable( p->GetString(icnt), ndm, value, d, dimSizes );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    names.push_back( p->GetString(icnt) );
    values.push_back( value );
    data.push_back( d );
    id.push_back( 0 );
    nd1.push_back( dimSizes[0] );
    nd2.push_back( dimSizes[1] );
    nd3.push_back( dimSizes[2] );
    //
    if( qualifiers[wxT("TENSOR")] && ndm!=3 )
    {
      wxString s( p->GetString(icnt) );
      s += wxString(wxT(" is not a tensor"));
      throw ECommandError( command+s );
    }
    if( qualifiers[wxT("MATRIX")] && ndm!=2 )
    {
      wxString s( p->GetString(icnt) );
      s += wxString(wxT(" is not a matrix"));
      throw ECommandError( command+s );
    }
    if( qualifiers[wxT("VECTORS")] )
    {
      if( ndm != 1 )
      {
        wxString s( p->GetString(icnt) );
        s += wxString(wxT(" is not a vector"));
        throw ECommandError( command+s );
      }
      for( std::size_t j=0; j<names.size(); ++j )
      {
        if( names.back() == names[j] )
        {
          id.back() = static_cast<int>(j);
          break;
        }
      }
    }
    if( qualifiers[wxT("SCALARS")] && ndm!=0 )
    {
      wxString s( p->GetString(icnt) );
      s += wxString(wxT(" is not a scalar"));
      throw ECommandError( command+s );
    }
    AddToStackLine( p->GetString(icnt) );
    ++icnt;
  }
  if( names.empty() )throw ECommandError( command+wxT("no variables entered") );
  //
  if( qualifiers[wxT("SCALARS")] )
  {
    if( qualifiers[wxT("BINARY")] )
    {
      for( std::size_t j=0; j<names.size(); ++j )ofStream_ << values[j];
    }
    else
    {
      if( qualifiers[wxT("FORMAT")] )
      {
        throw ECommandError( command+wxT("formats are not supported as yet") );
      }
      else
      {
        for( std::size_t j=0; j<names.size(); ++j )ofStream_ << values[j] << " ";
        ofStream_ << std::endl;
      }
    }
  }
  else if( qualifiers[wxT("MATRIX")] )
  {
    if( qualifiers[wxT("BINARY")] )
    {
      for( int i=0; i<nd1[0]; ++i )
      {
        for( int j=0; j<nd2[0]; ++j )ofStream_ << data[0][i+j*nd1[0]];
      }
    }
    else
    {
      if( qualifiers[wxT("FORMAT")] )
      {
        throw ECommandError( command+wxT("formats are not supported as yet") );
      }
      else
      {
        for( int i=0; i<nd1[0]; ++i )
        {
          for( int j=0; j<nd2[0]; ++j )ofStream_ << data[0][i+j*nd1[0]] << " ";
          ofStream_ << std::endl;
        }
      }
    }
  }
  else if( qualifiers[wxT("TENSOR")] )
  {
    if( qualifiers[wxT("BINARY")] )
    {
      for( int i=0; i<nd1[0]; ++i )
      {
        for( int j=0; j<nd2[0]; ++j )
        {
          for( int k=0; k<nd2[0]; ++k )ofStream_ << data[0][i+j*nd1[0]+k*nd2[0]];
        }
      }
    }
    else
    {
      if( qualifiers[wxT("FORMAT")] )
      {
        throw ECommandError( command+wxT("formats are not supported as yet") );
      }
      else
      {
        for( int i=0; i<nd1[0]; ++i )
        {
          for( int j=0; j<nd2[0]; ++j )
          {
            for( int k=0; k<nd2[0]; ++k )ofStream_ << data[0][i+j*nd1[0]+k*nd2[0]] << " ";
            ofStream_ << std::endl;
          }
        }
      }
    }
  }
  else  // vectors
  {
    std::vector<int> mm( names.size(), 0 );
    for( ;; )
    {
      std::vector<double> d;
      d.clear();
      for( std::size_t i=0; i<names.size(); ++i )
      {
        ++mm[id[i]];
        if( mm[id[i]] <= nd1[id[i]] )d.push_back( data[id[i]][mm[id[i]]-1] );
      }
      if( d.empty() )break;
      if( qualifiers[wxT("BINARY")] )
      {
        for( std::size_t j=0; j<d.size(); ++j )ofStream_ << d[j];
      }
      else
      {
        if( qualifiers[wxT("FORMAT")] )
        {
          throw ECommandError( command+wxT("formats are not supported as yet") );
        }
        else
        {
          for( std::size_t j=0; j<d.size(); ++j )ofStream_ << d[j] << " ";
          ofStream_ << std::endl;
        }
      }
    }
  }
  ofStream_.close();
}

// end of file
