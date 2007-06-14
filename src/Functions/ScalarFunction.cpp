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
#include "wx/wx.h"

#include "ScalarFunction.h"
#include "Workspace.h"
#include "EExpressionError.h"

ScalarFunction::ScalarFunction( wxString const &name,
                                std::size_t minArg, std::size_t maxArg,
                                Type fcnType, ArgType argType1, ArgType argType2 )
    : FunctionDefinition( name, minArg, maxArg, fcnType, argType1, argType2 )
{}

void ScalarFunction::CalcArgument( bool firstArgument, int ndmEff,
                                   int *nptEff, Workspace *ws )
{
  if( firstArgument )
  {
    for( std::size_t j=0; j<ws->GetNumberOfLoops(); ++j )
      ws->SetDimMag( static_cast<unsigned short int>(j), nptEff[j] );
  }
  else
  {
    for( std::size_t j=0; j<ws->GetNumberOfLoops(); ++j )
    {
      if( static_cast<int>(ws->GetDimMag(static_cast<unsigned short int>(j))) != nptEff[j] )
      {
        wxString buff( wxT("Error in CalcArgument: size mismatch for dimension ") );
        throw EExpressionError( buff << j );
      }
    }
  }
}

// end-of-file
