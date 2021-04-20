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
#ifndef GRA_BORLANDPRINTER
#define GRA_BORLANDPRINTER

#include "ExGlobals.h"
#include "GRA_bitmap.h"
#include "GRA_borlandW32.h"

class GRA_borlandPrinter : public GRA_borlandW32
{
public:
  GRA_borlandPrinter( TPrinter *printer )
    : GRA_borlandW32(100,100,printer->PageWidth-100,printer->PageHeight-100)
  {
    double xminW, yminW, xmaxW, ymaxW;
    ExGlobals::GetWorldLimits( xminW, yminW, xmaxW, ymaxW );
    SetCanvas( printer->Canvas );
    xDotsPerInch_ = (printer->PageWidth-200)/(xmaxW-xminW);
    inc_ = 5;
    //
    isPrinter_ = true;
  }

  ~GRA_borlandPrinter()
  {}

  // this overrides the default method in GRA_borlandW32
  void SetLineWidth( int i )
  { canvas_->Pen->Width = std::max(1,static_cast<int>(i*xDotsPerInch_/72./4.)); }

protected:
  double xDotsPerInch_;
};

#endif
