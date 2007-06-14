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
#include "FCN_char.h"
#include "Workspace.h"
#include "EExpressionError.h"

FCN_char *FCN_char::fcn_char_ = 0;

void FCN_char::TextArrayEval( int const j, std::vector<Workspace*> &wStack,
                              std::vector<wxString> &sStack ) const
{
  Workspace *n = wStack[j];
  if( n->IsEmpty() )throw EExpressionError( wxT("CHAR: argument workspace is empty") );
  wxString str;
  if( n->GetNumberOfDimensions() == 0 )
  {
    int x = static_cast<int>( n->GetValue() );
    if( x < 0 )       throw EExpressionError( wxT("CHAR: argument < 0") );
    else if( x > 127 )throw EExpressionError( wxT("CHAR: argument > 127") );
    str = wxString( 1, static_cast<wxChar>(x) );
  }
  else if( n->GetNumberOfDimensions() == 1 )
  {
    std::size_t end = n->GetDimMag(0);
    for( std::size_t i=0; i<end; ++i )
    {
      int x = static_cast<int>( n->GetData(i) );
      if( x < 0 )       throw EExpressionError( wxT("CHAR: argument < 0") );
      else if( x > 127 )throw EExpressionError( wxT("CHAR: argument > 127") );
      str += wxString( 1, static_cast<wxChar>(x) );
    }
  }
  else throw EExpressionError( wxT("CHAR: argument must be scalar or vector") );
  sStack[j] = str;
}

 // end of file
