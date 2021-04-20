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

#include "GRA_colorControl.h"

#include "GRA_point.h"
  
void GRA_point::CopyStuff( GRA_point const &rhs )
{
  x_ = rhs.x_;
  y_ = rhs.y_;
  color_ = rhs.color_;
}

std::ostream &operator<<( std::ostream &out, GRA_point const &p )
{
  return out << "<point x=\"" << p.x_ << "\" y=\"" << p.y_ << "\ color=\""
             << GRA_colorControl::GetColorCode(p.color_) << "\"/>\n";
}

// end of file
