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

#include <fstream>

#include "EGraphicsError.h"
#include "ExGlobals.h"

#include "GRA_borlandMetafile.h"

GRA_borlandMetafile::GRA_borlandMetafile( bool enhanced, ExString filename )
    : GRA_borlandW32(), metafile_(0), metafileCanvas_(0), filename_(filename)
{
  // the metafile canvas will be the same size as the monitor canvas
  //
  ExString extension( filename_.ExtractFileExt() );
  if( extension.empty() )filename_ += ExString(".wmf");
  //
  metafile_ = new TMetafile();
  metafile_->Enhanced = enhanced;
  //
  // set the metafile size before creating the canvas
  //
  metafile_->Height = static_cast<int>(yMax_-yMin_);
  metafile_->Width = static_cast<int>(xMax_-xMin_);
  metafileCanvas_ = new TMetafileCanvas( metafile_, 0 );
  SetCanvas( static_cast<TCanvas*>(metafileCanvas_) );
}

void GRA_borlandMetafile::DrawAndSave()
{
  Draw();
  delete metafileCanvas_;
  try
  {
    metafile_->SaveToFile( filename_.c_str() );
  }
  catch (...)
  {
    throw EGraphicsError( ExString("GRA_borlandMetafile:  unable to open ")+filename_ );
  }
}

// end of file
