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

#include "GRA_jpeg.h"

GRA_jpeg::GRA_jpeg( Graphics::TBitmap *b )
{
  jpeg_ = new TJPEGImage();
  jpeg_->Assign( b );
  percent_ = false;
}

GRA_jpeg::~GRA_jpeg()
{
  delete jpeg_;
}

GRA_jpeg::GRA_jpeg( GRA_jpeg const &rhs )
{ CopyStuff( rhs ); }

GRA_jpeg & GRA_jpeg::operator=( GRA_jpeg const &rhs )
{
  if( this != &rhs )CopyStuff( rhs );
  return *this;
}

void GRA_jpeg::CopyStuff( GRA_jpeg const &rhs )
{
  if( jpeg_ != NULL )delete jpeg_;
  jpeg_ = new TJPEGImage();
  jpeg_->Assign( rhs.jpeg_ );
}

Jpeg::TJPEGImage *GRA_jpeg::GetJPEG() const
{ return jpeg_; }

void GRA_jpeg::WriteOut( unsigned char c, std::ofstream &out )
{
  // output one byte
  // Beware of 2 percent characters at the start of a line.
  // This may cause trouble with post-processing software.
  //
  ++outBytes_;
  if( c == '%' )
  {
    if( outBytes_ == 1 )percent_ = true;
    else if( outBytes_ == 2 && percent_ )
    {
      out << std::endl;
      outBytes_ = 1;
      percent_ = false;
    }
  }
  out << c;
  if( outBytes_>63 )
  {
    out << std::endl;
    outBytes_ = 0;
  }
}

void GRA_jpeg::ASCII85Encode( std::ofstream &out )
{
  TMemoryStream *memStream = new TMemoryStream();
  jpeg_->SaveToStream( memStream );
  memStream->Position = 0;
  //jpeg_->SaveToFile( AnsiString("C:/extest/test.jpg") );
  //
  int count;
  outBytes_ = 0;
  //
  // 4 bytes read ==> output 5 bytes
  //
  unsigned char buff[4];
  unsigned long const pow85[] = { 1L, 85L, 85L*85L, 85L*85L*85L, 85L*85L*85L*85L };
  unsigned long word;
  while( (count=memStream->Read((void*)buff,4)) == 4)
  {
    word = (static_cast<unsigned long>((static_cast<unsigned int>(buff[0])<<8)+buff[1])<<16)+
           ((static_cast<unsigned int>(buff[2])<<8)+buff[3]);
    if( word == 0 )
    {
      WriteOut( 'z', out );     // shortcut for 0
    }
    else                        // calculate 5 ASCII85 bytes and output them
    {
      for( int i=4; i>=0; --i )
      {
        unsigned long v = word/pow85[i];
        WriteOut( static_cast<unsigned char>(v+'!'), out );
        word -= v*pow85[i];
      }
    }
  }
  word = 0;
  if( count != 0 )    // 1-3 bytes left
  {
    // accumulate bytes
    for( int i=count-1; i>=0; --i )word += static_cast<unsigned long>(buff[i]) << 8*(3-i);
    for( int i=4; i>=4-count; --i )
    {
      unsigned long v = word/pow85[i];
      WriteOut( static_cast<unsigned char>(v+'!'), out );
      word -= v*pow85[i];
    }
  }
  out << "~>" << std::endl;  // EOD marker
  delete memStream;
}

#pragma package(smart_init)
