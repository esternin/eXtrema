/*
Copyright (C) 2005,...,2007 Joseph L. Chuma, TRIUMF

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
#include <cmath>
#include <vector>

#include "wx/wx.h"

#include "CMD_density.h"
#include "ExGlobals.h"
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

CMD_density *CMD_density::cmd_density_ = 0;

CMD_density::CMD_density() : Command( wxT("DENSITY") )
{
  // types of density plot
  //
  AddQualifier( wxT("SOLIDFILL"), true );
  AddQualifier( wxT("GRADIENTS"), true );
  AddQualifier( wxT("BOXES"), false );
  AddQualifier( wxT("DIFFUSION"), false );
  AddQualifier( wxT("DITHERING"), false );
  //
  AddQualifier( wxT("AXES"), true );
  AddQualifier( wxT("LEGEND"), false );
  AddQualifier( wxT("XPROFILE"), false );
  AddQualifier( wxT("YPROFILE"), false );
  AddQualifier( wxT("BORDER"), true );
  AddQualifier( wxT("RESET"), false );
  //
  AddQualifier( wxT("LINEAR"), true );         // only applies to SolidFill and RandomPoints
  AddQualifier( wxT("HISTOGRAM"), false );     // opposite of linear
  AddQualifier( wxT("ZOOM"), false );
  AddQualifier( wxT("CONTOURS"), false );
  AddQualifier( wxT("EQUALLYSPACED"), false );
  AddQualifier( wxT("LINES"), true );
  AddQualifier( wxT("AREAS"), false );
  AddQualifier( wxT("VOLUMES"), false );
  AddQualifier( wxT("DITHERVECTOR"), false );
  AddQualifier( wxT("CONTOURLEVELS"), false );
  AddQualifier( wxT("COLOR"), false );
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
  catch (ECommandError const &e)
  {
    throw;
  }
  wxString command( Name()+wxT(": ") );
  if( !qualifiers[wxT("DITHERING")] )
  {
    if( qualifiers[wxT("CONTOURLEVELS")] )
      throw ECommandError( command+wxT("\\CONTOURLEVELS only applies to dithering type density plots") );
    if( qualifiers[wxT("DITHERVECTOR")] )
      throw ECommandError( command+wxT("\\DITHERVECTOR only applies to dithering type density plots") );
    qualifiers[wxT("LINEAR")] = !qualifiers[wxT("HISTOGRAM")];
  }
  int ifld = 1;
  std::size_t nLevels = 10;
  std::vector<int> dither;
  std::vector<double> contourLevels;
  if( qualifiers[wxT("DITHERING")] )
  {
    if( qualifiers[wxT("DITHERVECTOR")] )
    {
      std::vector<double> tmp;
      if( p->GetNumberOfTokens()<=1 || !p->IsString(1) )
        throw ECommandError( command+wxT("expecting a dithering vector") );
      try
      {
        NumericVariable::GetVector( p->GetString(1), wxT("dithering vector"), tmp );
      }
      catch( EVariableError const &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
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
    if( qualifiers[wxT("CONTOURLEVELS")] )
    {
      if( p->GetNumberOfTokens()<=ifld || !p->IsString(ifld) )
        throw ECommandError( command+wxT("expecting a contour levels vector") );
      try
      {
        NumericVariable::GetVector( p->GetString(ifld), wxT("contour levels vector"), contourLevels );
      }
      catch( EVariableError const &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      AddToStackLine( p->GetString(ifld) );
      for( std::size_t i=0; i<contourLevels.size()-1; ++i )
      {
        if( contourLevels[i] >= contourLevels[i+1] )
          throw ECommandError(
            command+wxT("contour level vector must be strictly monotonically increasing") );
      }
      ++ifld;
      if( qualifiers[wxT("DITHERVECTOR")] )
      {
        if( contourLevels.size() != nLevels )
          throw ECommandError(
            command+wxT("number of contour levels does not match the dithering vector") );
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
  wxString namex, namey, namez;
  std::vector<double> x, y, z;
  int nrow=0, ncol=0;
  //
  if( p->GetNumberOfTokens()<=ifld || !p->IsString(ifld) )
    throw ECommandError( command+wxT("expecting matrix or independent vector") );
  namez = p->GetString( ifld );
  int ndim;
  double v1;
  int dimSizes[3];
  try
  {
    NumericVariable::GetVariable( namez, ndim, v1, z, dimSizes );
  }
  catch (EVariableError const &e)
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  if( ndim == 0 )
  {
    throw ECommandError( command+namez+wxT(" is a scalar") );
  }
  if( ndim == 3 )
  {
    throw ECommandError( command+namez+wxT(" is a tensor") );
  }
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
      throw ECommandError( command+wxT("expecting dependant variable vector") );
    namey = p->GetString(ifld);
    try
    {
      NumericVariable::GetVector( namey, wxT("dependant variable vector"), y );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( namey );
    ++ifld;
    if( p->GetNumberOfTokens()<=ifld || !p->IsString(ifld) )
      throw ECommandError( command+wxT("expecting matrix or z-variable vector") );
    namez = p->GetString(ifld);
    try
    {
      NumericVariable::GetVariable( namez, ndim, v1, z, dimSizes );
    }
    catch (EVariableError const &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( ndim == 0 )
    {
      throw ECommandError( command+namez+wxT(" is a scalar") );
    }
    if( ndim == 3 )
    {
      throw ECommandError( command+namez+wxT(" is a tensor") );
    }
    ++ifld;
    if( ndim == 1 ) // density x y z ...
    {
      if( qualifiers[wxT("BOXES")] && (qualifiers[wxT("XPROFILE")] || qualifiers[wxT("YPROFILE")]) )
        throw ECommandError( command+wxT("profiles are not drawn with scatter plots") );
      if( x.size() != y.size() )
        throw ECommandError( command+namex+wxT(" has different length than ")+namey );
      if( x.size() != z.size() )
        throw ECommandError( command+namex+wxT(" has different length than ")+namez );
      if( !qualifiers[wxT("BOXES")] && x.size()<4 )
        throw ECommandError( command+wxT("grid interpolation requires at least 4 points") );
    }
    else if( ndim == 2 ) // density x y matrix ...
    {
      nrow = dimSizes[0];
      if( y.size() != static_cast<std::size_t>(nrow) )
        throw ECommandError( command+namey+
          wxT(" does not have the same number of elements as the number of rows in the matrix") );
      ncol = dimSizes[1];
      if( x.size() != static_cast<std::size_t>(ncol) )
        throw ECommandError( command+namex+
         wxT(" does not have the same number of elements as the number of columns in the matrix") );
    }
  }
  double fmin=0.0, fmax=1.0;
  double qmin=0.0, qmax=1.0;
  double bscale = 1.0;
  if( p->GetNumberOfTokens() > ifld )
  {
    try
    {
      NumericVariable::GetScalar( p->GetString(ifld), wxT("p1"), fmin );
    }
    catch (EVariableError const &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(ifld) );
    if( p->GetNumberOfTokens() > ++ifld )
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(ifld), wxT("p2"), fmax );
      }
      catch (EVariableError const &e)
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      AddToStackLine( p->GetString(ifld) );
      if( p->GetNumberOfTokens() > ++ifld )
      {
        try
        {
          NumericVariable::GetScalar( p->GetString(ifld), wxT("q1"), qmin );
        }
        catch (EVariableError const &e)
        {
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
        AddToStackLine( p->GetString(ifld) );
        if( p->GetNumberOfTokens() > ++ifld )
        {
          try
          {
            NumericVariable::GetScalar( p->GetString(ifld), wxT("q2"), qmax );
          }
          catch (EVariableError const &e)
          {
            throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
          }
          AddToStackLine( p->GetString(ifld) );
          if( qualifiers[wxT("BOXES")] )
          {
            if( p->GetNumberOfTokens() > ++ifld )
            {
              try
              {
                NumericVariable::GetScalar( p->GetString(ifld), wxT("box size scale factor"), bscale );
              }
              catch (EVariableError const &e)
              {
                throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
              }
              AddToStackLine( p->GetString(ifld) );
              ++ifld;
            }
          }
        }
      }
    }
  }
  if( !qualifiers[wxT("BOXES")] && nrow==0 )
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
    catch (EExpressionError const &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    x.assign( xi.begin(), xi.end() );
    y.assign( yi.begin(), yi.end() );
    z.assign( array.begin(), array.end() );
  }
  wxClientDC dc( ExGlobals::GetwxWindow() );
  if( qualifiers[wxT("BOXES")] )
  {
    GRA_boxPlot *bp = new GRA_boxPlot( x, y, z, nrow, fmin, fmax, qmin, qmax,
                                       qualifiers[wxT("XPROFILE")], qualifiers[wxT("YPROFILE")],
                                       qualifiers[wxT("BORDER")], qualifiers[wxT("ZOOM")],
                                       qualifiers[wxT("AXES")], qualifiers[wxT("RESET")],
                                       bscale );
    try
    {
      bp->Make();
      bp->Draw( ExGlobals::GetGraphicsOutput(), dc );
    }
    catch (EGraphicsError const &e)
    {
      delete bp;
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    ExGlobals::GetGraphWindow()->AddDrawableObject( bp );
  }
  else if( qualifiers[wxT("DIFFUSION")] )
  {
    GRA_diffusionPlot *dp =
     new GRA_diffusionPlot( x, y, z, nrow, fmin, fmax, qmin, qmax,
                            qualifiers[wxT("XPROFILE")], qualifiers[wxT("YPROFILE")],
                            qualifiers[wxT("BORDER")], qualifiers[wxT("ZOOM")],
                            qualifiers[wxT("AXES")], qualifiers[wxT("RESET")] );
    try
    {
      dp->Draw( ExGlobals::GetGraphicsOutput(), dc );
    }
    catch (EGraphicsError const &e)
    {
      delete dp;
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    ExGlobals::GetGraphWindow()->AddDrawableObject( dp );
  }
  else if( qualifiers[wxT("DITHERING")] )
  {
    GRA_ditheringPlot *dp =
      new GRA_ditheringPlot( x, y, z, nrow,
                             fmin, fmax, qmin, qmax,
                             qualifiers[wxT("XPROFILE")], qualifiers[wxT("YPROFILE")],
                             qualifiers[wxT("BORDER")], qualifiers[wxT("ZOOM")],
                             qualifiers[wxT("AXES")], qualifiers[wxT("RESET")],
                             qualifiers[wxT("LEGEND")], qualifiers[wxT("EQUALLYSPACED")],
                             qualifiers[wxT("AREAS")], qualifiers[wxT("VOLUMES")],
                             qualifiers[wxT("LINES")], dither, contourLevels );
    try
    {
      dp->Draw( ExGlobals::GetGraphicsOutput(), dc );
    }
    catch (EGraphicsError const &e)
    {
      delete dp;
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    ExGlobals::GetGraphWindow()->AddDrawableObject( dp );
    try
    {
      wxString name;
      if( qualifiers[wxT("AREAS")] )
      {
        name = wxT("DENSITY$AREAS");
        NumericVariable::PutVariable( name, dp->GetAreaVec(), 0, p->GetInputLine() );
      }
      if( qualifiers[wxT("VOLUMES")] )
      {
        name = wxT("DENSITY$VOLUMES");
        NumericVariable::PutVariable( name, dp->GetVolumeVec(), 0, p->GetInputLine() );
      }
      if( qualifiers[wxT("CONTOURS")] )
      {
        name = wxT("DENSITY$CONTOURS");
        NumericVariable::PutVariable( name, dp->GetContourVec(), 0, p->GetInputLine() );
      }
    }
    catch ( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
  else if( qualifiers[wxT("SOLIDFILL")] || qualifiers[wxT("GRADIENTS")] )
  {
    GRA_gradientPlot *gp =
     new GRA_gradientPlot( x, y, z, nrow, fmin, fmax, qmin, qmax,
                           qualifiers[wxT("XPROFILE")], qualifiers[wxT("YPROFILE")],
                           qualifiers[wxT("BORDER")], qualifiers[wxT("ZOOM")],
                           qualifiers[wxT("AXES")], qualifiers[wxT("RESET")],
                           qualifiers[wxT("LEGEND")], qualifiers[wxT("LINEAR")] );
    try
    {
      gp->Draw( ExGlobals::GetGraphicsOutput(), dc );
    }
    catch (EGraphicsError const &e)
    {
      delete gp;
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    ExGlobals::GetGraphWindow()->AddDrawableObject( gp );
  }
}

// end of file
