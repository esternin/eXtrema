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

#include "FCN_eltime.h"

FCN_eltime FCN_eltime::fcn_eltime_;

void FCN_eltime::ScalarEval( int j, std::vector<double> &rStack ) const
{
#ifdef __BORLANDC__
  if( rStack[j] == 0 )
  {
    time_ = Time(); // get the current time
  }
  else
  {
    unsigned short hour, min, sec, msec;
    TDateTime tmp( Time()-time_ );
    tmp.DecodeTime( &hour, &min, &sec, &msec );
    rStack[j] = hour*60*60 + min*60 + sec + msec/1000.0;
  }
#endif
}

