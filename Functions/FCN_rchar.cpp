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

#include "Workspace.h"
#include "EExpressionError.h"

#include "FCN_rchar.h"

FCN_rchar FCN_rchar::fcn_rchar_;

void FCN_rchar::TextArrayEval( int const j, std::vector<Workspace*> &wStack,
                               std::vector<ExString> &sStack ) const
{
  double x;
  //
  if( static_cast<int>(wStack.size()) <= j )
    throw EExpressionError( "RCHAR function: wStack.size() <= j" );
  //
  Workspace *ws = wStack[j];
  if( ws->GetNumberOfDimensions() != 0 )
    throw EExpressionError( "first argument to RCHAR must be scalar" );
  x = ws->GetValue();
  if( GetArgumentCounter() == 1 )
  {
    sStack[j] = ExString( x );
  }
  else
  {
    ExString format( sStack[j+1] );
    char c[300];
    if( sprintf( c, format.c_str(), x ) < 0 )
    {
      throw EExpressionError( "error converting to character using format" );
    }
    sStack[j] = ExString( c );
  }
}

// end of file
