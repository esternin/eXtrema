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

#include "FCN_time.h"

FCN_time FCN_time::fcn_time_;

void FCN_time::TextScalarEval( int j, std::vector<ExString> &sStack ) const
{
#ifdef __BORLANDC__
  if( GetArgumentCounter() == 1 )
  {
    ShortDateFormat = AnsiString( sStack[j].c_str() );
    AnsiString as = DateToStr( Time() );
    sStack[j] = ExString( as.c_str() );
  }
  else
  {
    ShortDateFormat = "hh:nn:ss";
    AnsiString as = DateToStr( Time() );
    sStack.push_back( ExString( as.c_str() ) );
  }
#endif
}

