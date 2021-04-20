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
#ifndef GRA_JPEG
#define GRA_JPEG

#include "jpeg.hpp"

class GRA_jpeg
{
public:
  GRA_jpeg( Graphics::TBitmap * );
  virtual ~GRA_jpeg();
  GRA_jpeg( GRA_jpeg const & );
  GRA_jpeg &operator=( GRA_jpeg const & );
  void CopyStuff( GRA_jpeg const & );
  Jpeg::TJPEGImage *GetJPEG() const;
  void ASCII85Encode( std::ofstream & );

private:
  void WriteOut( unsigned char, std::ofstream & );

  Jpeg::TJPEGImage *jpeg_;
  int outBytes_;
  bool percent_;
};

#endif
