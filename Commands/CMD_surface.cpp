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

#include "ExGlobals.h"
#include "ExString.h"
#include "GRA_window.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "EGraphicsError.h"
#include "GRA_threeDFigure.h"
#include "NumericVariable.h"

#include "CMD_surface.h"

CMD_surface CMD_surface::cmd_surface_;

CMD_surface::CMD_surface() : Command( "SURFACE" )
{
  AddQualifier( "COLOR", false );
  AddQualifier( "COLOUR", false );
  AddQualifier( "HISTOGRAM", false );
  //
  color_ = false;
  histogram_ = false;
  theta_ = 40.0;
  phi_ = 40.0;
  vsf_ = 50.0;
  pdsf_ = 0.0;
}

void CMD_surface::Execute( ParseLine const *p )
{
  // SURFACE M XROT ZROT VSCALE
  //
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  ExString command( "SURFACE" );
  //
  ParseToken t( *p->GetToken(0) );
  //
  color_ = qualifiers["COLOR"] || qualifiers["COLOUR"];
  histogram_ = qualifiers["HISTOGRAM"];
  //
  std::vector<double> matrix;
  std::size_t nRow, nCol;
  try
  {
    NumericVariable::GetMatrix( p->GetString(1), "matrix", matrix, nRow, nCol  );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "SURFACE", e.what() );
  }
  AddToStackLine( p->GetString(1) );
  if( p->GetNumberOfTokens() > 2 )
  {
    try
    {
      NumericVariable::GetScalar( p->GetString(2), "rotation angle about x-axis", theta_  );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "SURFACE", e.what() );
    }
    AddToStackLine( p->GetString(2) );
    if( p->GetNumberOfTokens() > 3 )
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(3), "rotation angle about z-axis", phi_  );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "SURFACE", e.what() );
      }
      AddToStackLine( p->GetString(3) );
      if( p->GetNumberOfTokens() > 4 )
      {
        try
        {
          NumericVariable::GetScalar( p->GetString(4), "vertical scale factor", vsf_  );
        }
        catch( EVariableError &e )
        {
          throw ECommandError( "SURFACE", e.what() );
        }
        AddToStackLine( p->GetString(4) );
      }
    }
  }
  try
  {
    std::auto_ptr<GRA_threeDFigure> tdf( new GRA_threeDFigure( matrix,nRow,nCol,theta_,phi_,vsf_,pdsf_,color_,histogram_ ) );
    tdf->Draw();
  }
  catch( EGraphicsError &e )
  {
    throw ECommandError( "SURFACE", e.what() );
  }
}
 
// end of file
