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
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "TextVariable.h"
#include "NumericVariable.h"

#include "CMD_write.h"

CMD_write CMD_write::cmd_write_;

CMD_write::CMD_write() : Command( "WRITE" )
{
  AddQualifier( "SCALARS", false );
  AddQualifier( "VECTORS", true  );
  AddQualifier( "MATRIX",  false );
  AddQualifier( "TENSOR",  false );
  AddQualifier( "TEXT",    false );
  AddQualifier( "APPEND",  false );
  AddQualifier( "FORMAT",  false );
  AddQualifier( "BINARY",  false );
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
  if( qualifiers["FORMAT"] && qualifiers["BINARY"] )
    throw ECommandError( "WRITE", "BINARY and FORMAT qualifiers are not compatible" );
  //
  if( !p->IsString(1) )throw ECommandError( "WRITE", "no filename was entered" );
  try
  {
    TextVariable::GetVariable( p->GetString(1), true, filename_ );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "WRITE", e.what() );
  }
  //
  if( filename_.ExtractFilePath().empty() )filename_ = ExGlobals::GetCurrentPath() + filename_;
  //
  if( ofStream_.is_open() )ofStream_.close();
  if( qualifiers["BINARY"] )
  {
    if( qualifiers["APPEND"] )
      ofStream_.open( filename_.c_str(),std::ios::out|std::ios::binary|std::ios::app );
    else
      ofStream_.open( filename_.c_str(), std::ios::out|std::ios::binary );
  }
  else
  {
    if( qualifiers["APPEND"] )
      ofStream_.open( filename_.c_str(), std::ios::out|std::ios::app );
    else
      ofStream_.open( filename_.c_str(), std::ios::out );
  }
  AddToStackLine( p->GetString(1) );
  int icnt = 2;
  ExString format;
  if( qualifiers["FORMAT"] )
  {
    if( !p->IsString(2) )throw ECommandError( "WRITE", "expecting format" );
    try
    {
      TextVariable::GetVariable( p->GetString(2), true, format );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "WRITE", e.what() );
    }
    AddToStackLine( p->GetString(2) );
    icnt = 3;
  }
  if( qualifiers["TEXT"] )
  {
    if( !p->IsString(icnt) )
      throw ECommandError( "WRITE\\TEXT", "expecting text to write" );
    ExString line;
    try
    {
      TextVariable::GetVariable( p->GetString(icnt), false, line );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "WRITE", e.what() );
    }
    AddToStackLine( p->GetString(icnt) );
    if( qualifiers["FORMAT"] )
    {
      char c[32000];
      sprintf( c, format.c_str(), line.c_str() );
      ofStream_ << c << std::endl;
    }
    else
    {
      ofStream_ << line.c_str() << std::endl;
    }
    ofStream_.close();
    return;
  }
  //
  // write scalars, vectors, matrix, or tensor
  //
  if( qualifiers["MATRIX"] || qualifiers["SCALARS"] || qualifiers["TENSOR"] )
    qualifiers["VECTORS"] = false;
  //
  std::vector<ExString> names;
  std::vector<double> values;
  std::vector<int> id;
  std::vector< std::vector<double> > data;
  std::vector<double> d;
  std::vector<int> nd1, nd2, nd3;
  //
  while( icnt < p->GetNumberOfTokens() )
  {
    if( !p->IsString(icnt) )
    {
      ExString c("parameter ");
      c += ExString(icnt+1) + ExString(" is invalid");
      throw ECommandError( "WRITE", c );
    }
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
      throw ECommandError( "WRITE", e.what() );
    }
    names.push_back( p->GetString(icnt) );
    values.push_back( value );
    data.push_back( d );
    id.push_back( 0 );
    nd1.push_back( dimSizes[0] );
    nd2.push_back( dimSizes[1] );
    nd3.push_back( dimSizes[2] );
    //
    if( qualifiers["TENSOR"] && ndm!=3 )
    {
      ExString s( p->GetString(icnt) );
      s += ExString(" is not a tensor");
      throw ECommandError( "WRITE\\TENSOR", s );
    }
    if( qualifiers["MATRIX"] && ndm!=2 )
    {
      ExString s( p->GetString(icnt) );
      s += ExString(" is not a matrix");
      throw ECommandError( "WRITE\\MATRIX", s );
    }
    if( qualifiers["VECTORS"] )
    {
      if( ndm != 1 )
      {
        ExString s( p->GetString(icnt) );
        s += ExString(" is not a vector");
        throw ECommandError( "WRITE\\VECTOR", s );
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
    if( qualifiers["SCALARS"] && ndm!=0 )
    {
      ExString s( p->GetString(icnt) );
      s += ExString(" is not a scalar");
      throw ECommandError( "WRITE\\SCALAR", s );
    }
    AddToStackLine( p->GetString(icnt) );
    ++icnt;
  }
  if( names.empty() )throw ECommandError( "WRITE", "no variables entered" );
  //
  if( qualifiers["SCALARS"] )
  {
    if( qualifiers["BINARY"] )
    {
      for( std::size_t j=0; j<names.size(); ++j )ofStream_ << values[j];
    }
    else
    {
      if( qualifiers["FORMAT"] )
      {
        throw ECommandError( "WRITE\\SCALARS", "formats are not supported as yet" );
      }
      else
      {
        for( std::size_t j=0; j<names.size(); ++j )ofStream_ << values[j] << " ";
        ofStream_ << std::endl;
      }
    }
  }
  else if( qualifiers["MATRIX"] )
  {
    if( qualifiers["BINARY"] )
    {
      for( int i=0; i<nd1[0]; ++i )
      {
        for( int j=0; j<nd2[0]; ++j )ofStream_ << data[0][i+j*nd1[0]];
      }
    }
    else
    {
      if( qualifiers["FORMAT"] )
      {
        throw ECommandError( "WRITE\\MATRIX", "formats are not supported as yet" );
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
  else if( qualifiers["TENSOR"] )
  {
    if( qualifiers["BINARY"] )
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
      if( qualifiers["FORMAT"] )
      {
        throw ECommandError( "WRITE\\TENSOR", "formats are not supported as yet" );
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
      if( qualifiers["BINARY"] )
      {
        for( std::size_t j=0; j<d.size(); ++j )ofStream_ << d[j];
      }
      else
      {
        if( qualifiers["FORMAT"] )
        {
          throw ECommandError( "WRITE\\VECTORS", "formats are not supported as yet" );
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
