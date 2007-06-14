/*
Copyright (C) 2006 Joseph L. Chuma, TRIUMF

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
#ifndef EXTREMA_WXPLPLOTSTREAM
#define EXTREMA_WXPLPLOTSTREAM

#include "plplotP.h"
#include "plstream.h"
#include "wx/dc.h"

#define PL_DOUBLE

class wxPLplotstream : public plstream
{
public:
  wxPLplotstream( wxDC *, int, int );
  ~wxPLplotstream() {}
  
  void set_stream();          // calls some code before every PLplot command
  void SetSize( int, int );   // set new size of plot area
  void RenewPlot();           // redo plot

private:
  wxDC* dc_;            // pointer to wxDC to plot into
  int width_, height_;  // width, height of dc/plot area
};

#endif
