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

#include <vector>

#include "GRA_window.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "EExpressionError.h"
#include "EGraphicsError.h"
#include "UsefulFunctions.h"
#include "NumericVariable.h"
#include "GRA_thiessenTriangulation.h"
#include "GRA_boxPlot.h"
#include "GRA_diffusionPlot.h"
#include "GRA_gradientPlot.h"
#include "GRA_ditheringPlot.h"
#include "GRA_cartesianAxes.h"

#include "CMD_density.h"

CMD_density CMD_density::cmd_density_;

CMD_density::CMD_density() : Command( "DENSITY" )
{
  // types of density plot
  //
  AddQualifier( "SOLIDFILL", true );
  AddQualifier( "GRADIENTS", true );
  AddQualifier( "BOXES", false );
  AddQualifier( "DIFFUSION", false );
  AddQualifier( "DITHERING", false );
  //
  AddQualifier( "AXES", true );
  AddQualifier( "LEGEND", false );
  AddQualifier( "XPROFILE", false );
  AddQualifier( "YPROFILE", false );
  AddQualifier( "BORDER", true );
  AddQualifier( "RESET", false );
  //
  AddQualifier( "LINEAR", true );         // only applies to SolidFill and RandomPoints
  AddQualifier( "HISTOGRAM", false );     // opposite of linear
  AddQualifier( "ZOOM", false );
  AddQualifier( "CONTOURS", false );
  AddQualifier( "EQUALLYSPACED", false );
  AddQualifier( "LINES", true );
  AddQualifier( "AREAS", false );
  AddQualifier( "VOLUMES", false );
  AddQualifier( "DITHERVECTOR", false );
  AddQualifier( "CONTOURLEVELS", false );
  AddQualifier( "COLOR", false );
}

void CMD_density::Execute( ParseLine const *p )
{
  //
  // DENSITY\BOXES x y v { p1 p2 { bscale }}
  //
  // DENSITY\DITHERING x y v
  // DENSITY\DITHERING\DITHERVECTOR d x y v
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
  if( !qualifiers["DITHERING"] )
  {
    if( qualifiers["CONTOURLEVELS"] )
      throw ECommandError("DENSITY","\CONTOURLEVELS only applies to dithering type density plots");
    if( qualifiers["DITHERVECTOR"] )
      throw ECommandError("DENSITY","\DITHERVECTOR only applies to dithering type density plots");
    qualifiers["LINEAR"] = !qualifiers["HISTOGRAM"];
  }
  int ifld = 1;
  std::size_t nLevels = 10;
  std::vector<int> dither;
  std::vector<double> contourLevels;
  if( qualifiers["DITHERING"] )
  {
    if( qualifiers["DITHERVECTOR"] )
    {
      std::vector<double> tmp;
      if( p->GetNumberOfTokens()<=1 || !p->IsString(1) )
        throw ECommandError( "DENSITY\\DITHERVECTOR", "expecting a dithering vector" );
      try
      {
        NumericVariable::GetVector( p->GetString(1), "dithering vector", tmp );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "DENSITY", e.what() );
      }
      AddToStackLine( p->GetString(1) );
      ++ifld;
      nLevels = tmp.size()/2;
      for( std::size_t i=0; i<2*nLevels; ++i )
      {
        dither.push_back( static_cast<int>(tmp[i]) );
        if( dither.back() == 0 )dither.back() = 10000;
      }
    }
    else
    {
      dither.push_back( 1 );
      dither.push_back( 1 );
      dither.push_back( 2 );
      dither.push_back( 1 );
      dither.push_back( 2 );
      dither.push_back( 2 );
      dither.push_back( 3 );
      dither.push_back( 2 );
      dither.push_back( 3 );
      dither.push_back( 3 );
      dither.push_back( 4 );
      dither.push_back( 3 );
      dither.push_back( 4 );
      dither.push_back( 4 );
      dither.push_back( 5 );
      dither.push_back( 5 );
      dither.push_back( 6 );
      dither.push_back( 6 );
      dither.push_back( 10000 );
      dither.push_back( 10000 );
    }
    if( qualifiers["CONTOURLEVELS"] )
    {
      if( p->GetNumberOfTokens()<=ifld || !p->IsString(ifld) )
        throw ECommandError( "DENSITY\\DITHERING", "expecting a contour levels vector" );
      try
      {
        NumericVariable::GetVector( p->GetString(ifld), "contour levels vector", contourLevels );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "DENSITY", e.what() );
      }
      AddToStackLine( p->GetString(ifld) );
      for( std::size_t i=0; i<contourLevels.size()-1; ++i )
      {
        if( contourLevels[i] >= contourLevels[i+1] )throw ECommandError(
         "DENSITY\\DITHERING", "contour level vector must be strictly monotonically increasing" );
      }
      ++ifld;
      if( qualifiers["DITHERVECTOR"] )
      {
        if( contourLevels.size() != nLevels )throw ECommandError(
         "DENSITY\\DITHERING", "number of contour levels does not match the dithering vector" );
      }
      else
      {
        if( contourLevels.size() > 9 )
        {
          for( std::size_t i=19; i<=2*contourLevels.size()-1; i+=2 )
          {
            dither.push_back( 7+(i-19)/2 );
            dither.push_back( 7+(i-19)/2 );
          }
        }
        nLevels = contourLevels.size()+1;
        dither.push_back( 10000 );
        dither.push_back( 10000 );
      }
    }
  }
  ExString namex, namey, namez;
  std::vector<double> x, y, z;
  int nrow=0, ncol=0;
  //
  if( p->GetNumberOfTokens()<=ifld || !p->IsString(ifld) )
    throw ECommandError( "DENSITY", "expecting matrix or independent vector" );
  namez = p->GetString( ifld );
  int ndim;
  double v1;
  int dimSizes[3];
  try
  {
    NumericVariable::GetVariable( namez, ndim, v1, z, dimSizes );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( "DENSITY", e.what() );
  }
  if( ndim == 0 )throw ECommandError( "DENSITY", namez+" is a scalar" );
  if( ndim == 3 )throw ECommandError( "DENSITY", namez+" is a tensor" );
  ++ifld;
  if( ndim == 2 ) // DENSITY matrix ...
  {
    nrow = dimSizes[0];
    for( int i=1; i<=nrow; ++i )y.push_back( static_cast<double>(i) );
    ncol = dimSizes[1];
    for( int i=1; i<=ncol; ++i )x.push_back( static_cast<double>(i) );
  }
  else if( ndim == 1 ) // DENSITY x ...
  {
    x.assign( z.begin(), z.end() );
    std::vector<double>().swap( z );  // empty out z
    namex = namez;
    namez.clear();
    if( p->GetNumberOfTokens()<=ifld || !p->IsString(ifld) )
      throw ECommandError( "DENSITY", "expecting dependant variable vector" );
    namey = p->GetString(ifld);
    try
    {
      NumericVariable::GetVector( namey, "dependant variable vector", y );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "DENSITY", e.what() );
    }
    AddToStackLine( namey );
    ++ifld;
    if( p->GetNumberOfTokens()<=ifld || !p->IsString(ifld) )
      throw ECommandError( "DENSITY", "expecting matrix or z-variable vector" );
    namez = p->GetString(ifld);
    try
    {
      NumericVariable::GetVariable( namez, ndim, v1, z, dimSizes );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( "DENSITY", e.what() );
    }
    if( ndim == 0 )throw ECommandError( "DENSITY", namez+" is a scalar" );
    if( ndim == 3 )throw ECommandError( "DENSITY", namez+" is a tensor" );
    ++ifld;
    if( ndim == 1 ) // density x y z ...
    {
      if( qualifiers["BOXES"] && (qualifiers["XPROFILE"] || qualifiers["YPROFILE"]) )
        throw ECommandError( "DENSITY", "profiles are not drawn with scatter plots" );
      if( x.size() != y.size() )
        throw ECommandError( "DENSITY", namex+" has different length than "+namey );
      if( x.size() != z.size() )
        throw ECommandError( "DENSITY", namex+" has different length than "+namez );
      if( !qualifiers["BOXES"] && x.size()<4 )
        throw ECommandError( "DENSITY", "grid interpolation requires at least 4 points" );
    }
    else if( ndim == 2 ) // density x y matrix ...
    {
      nrow = dimSizes[0];
      if( y.size() != static_cast<std::size_t>(nrow) )
        throw ECommandError( "DENSITY", namey+" does not have the same number of elements as the number of rows in the matrix" );
      ncol = dimSizes[1];
      if( x.size() != static_cast<std::size_t>(ncol) )
        throw ECommandError( "DENSITY", namex+" does not have the same number of elements as the number of columns in the matrix" );
    }
  }
  double fmin=0.0, fmax=1.0;
  double qmin=0.0, qmax=1.0;
  double bscale = 1.0;
  if( p->GetNumberOfTokens() > ifld )
  {
    try
    {
      NumericVariable::GetScalar( p->GetString(ifld), "p1", fmin );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( "DENSITY", e.what() );
    }
    AddToStackLine( p->GetString(ifld) );
    if( p->GetNumberOfTokens() > ++ifld )
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(ifld), "p2", fmax );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( "DENSITY", e.what() );
      }
      AddToStackLine( p->GetString(ifld) );
      if( p->GetNumberOfTokens() > ++ifld )
      {
        try
        {
          NumericVariable::GetScalar( p->GetString(ifld), "q1", qmin );
        }
        catch (EVariableError &e)
        {
          throw ECommandError( "DENSITY", e.what() );
        }
        AddToStackLine( p->GetString(ifld) );
        if( p->GetNumberOfTokens() > ++ifld )
        {
          try
          {
            NumericVariable::GetScalar( p->GetString(ifld), "q2", qmax );
          }
          catch (EVariableError &e)
          {
            throw ECommandError( "DENSITY", e.what() );
          }
          AddToStackLine( p->GetString(ifld) );
          if( qualifiers["BOXES"] )
          {
            if( p->GetNumberOfTokens() > ++ifld )
            {
              try
              {
                NumericVariable::GetScalar( p->GetString(ifld), "box size scale factor", bscale );
              }
              catch (EVariableError &e)
              {
                throw ECommandError( "DENSITY", e.what() );
              }
              AddToStackLine( p->GetString(ifld) );
              ++ifld;
            }
          }
        }
      }
    }
  }
  if( !qualifiers["BOXES"] && nrow==0 )
  {
    double xmin, xmax, ymin, ymax;
    UsefulFunctions::MinMax( x, 0, x.size(), xmin, xmax );
    UsefulFunctions::MinMax( y, 0, y.size(), ymin, ymax );
    //
    ncol = static_cast<int>(sqrt(static_cast<double>(x.size()))*5.0);
    nrow = ncol;
    double xinc = (xmax-xmin)/(ncol-1);
    double yinc = (ymax-ymin)/(ncol-1);
    std::vector<double> xi, yi;
    xi.reserve(ncol);
    yi.reserve(ncol);
    for( int i=0; i<ncol-1; ++i )
    {
      xi.push_back( xmin+i*xinc );
      yi.push_back( ymin+i*yinc );
    }
    xi[ncol-1] = xmax;
    yi[ncol-1] = ymax;
    std::vector<double> array( ncol*ncol );
    try
    {
      GRA_thiessenTriangulation tt( x, y, z );
      tt.CreateMesh();
      double eps = 0.001;
      std::size_t nit = 128;
      tt.Gradients( eps, nit );
      tt.Interpolate( ncol, ncol, xi, yi, array );
    }
    catch (EExpressionError &e)
    {
      throw ECommandError( "DENSITY", e.what() );
    }
    x.assign( xi.begin(), xi.end() );
    y.assign( yi.begin(), yi.end() );
    z.assign( array.begin(), array.end() );
  }
  if( qualifiers["BOXES"] )
  {
    GRA_boxPlot *bp = new GRA_boxPlot( x, y, z, nrow, fmin, fmax, qmin, qmax,
                                       qualifiers["XPROFILE"], qualifiers["YPROFILE"],
                                       qualifiers["BORDER"], qualifiers["ZOOM"],
                                       qualifiers["AXES"], qualifiers["RESET"],
                                       bscale );
    try
    {
      bp->Make();
      bp->Draw( ExGlobals::GetScreenOutput() );
    }
    catch (EGraphicsError &e)
    {
      delete bp;
      throw ECommandError( "DENSITY", e.what() );
    }
    ExGlobals::GetGraphWindow()->AddDrawableObject( bp );
  }
  else if( qualifiers["DIFFUSION"] )
  {
    GRA_diffusionPlot *dp =
     new GRA_diffusionPlot( x, y, z, nrow, fmin, fmax, qmin, qmax,
                            qualifiers["XPROFILE"], qualifiers["YPROFILE"],
                            qualifiers["BORDER"], qualifiers["ZOOM"],
                            qualifiers["AXES"], qualifiers["RESET"] );
    try
    {
      dp->Draw( ExGlobals::GetScreenOutput() );
    }
    catch (EGraphicsError &e)
    {
      delete dp;
      throw ECommandError( "DENSITY", e.what() );
    }
    ExGlobals::GetGraphWindow()->AddDrawableObject( dp );
  }
  else if( qualifiers["DITHERING"] )
  {
    GRA_ditheringPlot *dp =
      new GRA_ditheringPlot( x, y, z, nrow,
                             fmin, fmax, qmin, qmax,
                             qualifiers["XPROFILE"], qualifiers["YPROFILE"],
                             qualifiers["BORDER"], qualifiers["ZOOM"],
                             qualifiers["AXES"], qualifiers["RESET"],
                             qualifiers["LEGEND"], qualifiers["EQUALLYSPACED"],
                             qualifiers["AREAS"], qualifiers["VOLUMES"],
                             qualifiers["LINES"], dither, contourLevels );
    try
    {
      dp->Draw( ExGlobals::GetScreenOutput() );
    }
    catch (EGraphicsError &e)
    {
      delete dp;
      throw ECommandError( "DENSITY", e.what() );
    }
    ExGlobals::GetGraphWindow()->AddDrawableObject( dp );
    try
    {
      if( qualifiers["AREAS"] )
        NumericVariable::PutVariable( "DENSITY$AREAS", dp->GetAreas(), 0, p->GetInputLine() );
      if( qualifiers["VOLUMES"] )
        NumericVariable::PutVariable( "DENSITY$VOLUMES", dp->GetVolumes(), 0, p->GetInputLine() );
      if( qualifiers["CONTOURS"] )
        NumericVariable::PutVariable( "DENSITY$CONTOURS", dp->GetContours(), 0, p->GetInputLine() );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( "DENSITY", e.what() );
    }
  }
  else if( qualifiers["SOLIDFILL"] || qualifiers["GRADIENTS"] )
  {
    GRA_gradientPlot *gp =
     new GRA_gradientPlot( x, y, z, nrow, fmin, fmax, qmin, qmax,
                           qualifiers["XPROFILE"], qualifiers["YPROFILE"],
                           qualifiers["BORDER"], qualifiers["ZOOM"],
                           qualifiers["AXES"], qualifiers["RESET"],
                           qualifiers["LEGEND"], qualifiers["LINEAR"] );
    try
    {
      gp->Draw( ExGlobals::GetScreenOutput() );
    }
    catch (EGraphicsError &e)
    {
      delete gp;
      throw ECommandError( "DENSITY", e.what() );
    }
    ExGlobals::GetGraphWindow()->AddDrawableObject( gp );
  }
}

 // end of file

