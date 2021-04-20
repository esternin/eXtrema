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

#include "FCN_char.h"

FCN_char FCN_char::fcn_char_;

void FCN_char::TextArrayEval( int const j, std::vector<Workspace*> &wStack,
                              std::vector<ExString> &sStack ) const
{
  //
  if( static_cast<int>(wStack.size()) <= j )
    throw EExpressionError( "CHAR function: wStack.size() <= j" );
  //
  Workspace *n = wStack[j];
  if( n->IsEmpty() )throw EExpressionError( "CHAR: argument workspace is empty" );
  ExString str;
  if( n->GetNumberOfDimensions() == 0 )
  {
    int x = static_cast<int>( n->GetValue() );
    if( x < 0 )       throw EExpressionError( "CHAR: argument < 0" );
    else if( x > 127 )throw EExpressionError( "CHAR: argument > 127" );
    str = ExString( 1, static_cast<char>(x) );
  }
  else if( n->GetNumberOfDimensions() == 1 )
  {
    std::size_t end = n->GetDimMag(0);
    for( std::size_t i=0; i<end; ++i )
    {
      int x = static_cast<int>( n->GetData(i) );
      if( x < 0 )       throw EExpressionError( "CHAR: argument < 0" );
      else if( x > 127 )throw EExpressionError( "CHAR: argument > 127" );
      str += ExString( 1, static_cast<char>(x) );
    }
  }
  else throw EExpressionError( "CHAR: argument must be scalar or vector" );
  sStack[j] = str;
}

 // end of file
