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

#include "CMD_surface.h"

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
#include "GRA_surfacePlot.h"

CMD_surface *CMD_surface::cmd_surface_ = 0;

CMD_surface::CMD_surface() : Command( wxT("SURFACE") )
{
  AddQualifier( wxT("COLOR"), false );
  AddQualifier( wxT("COLOUR"), false );
  AddQualifier( wxT("CONTOURS"), false );
  AddQualifier( wxT("AXES"), true );
  AddQualifier( wxT("SIDES"), true );
}

void CMD_surface::Execute( ParseLine const *p )
{
  // SURFACE { X Y } Z { azimuth altitude }
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
  //
  wxString command( wxT("SURFACE: ") );
  //
  wxString namex, namey, namez;
  std::vector<double> x, y, z;
  int nRow=0, nCol=0;
  //
  int counter = 1;
  if( p->GetNumberOfTokens()<=counter || !p->IsString(counter) )
    throw ECommandError( command+wxT("expecting matrix or independent vector") );
  namez = p->GetString( counter );
  int ndim;
  double v1;
  int dimSizes[3];
  try
  {
    NumericVariable::GetVariable( namez, ndim, v1, z, dimSizes );
  }
  catch (EVariableError &e)
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
  ++counter;
  if( ndim == 2 ) // SURFACE matrix ...
  {
    AddToStackLine( namez );
    nRow = dimSizes[0];
    for( int i=1; i<=nRow; ++i )y.push_back( static_cast<double>(i) );
    nCol = dimSizes[1];
    for( int i=1; i<=nCol; ++i )x.push_back( static_cast<double>(i) );
  }
  else if( ndim == 1 ) // SURFACE x ...
  {
    x.assign( z.begin(), z.end() );
    std::vector<double>().swap( z );  // empty out z
    namex = namez;
    namez.clear();
    if( p->GetNumberOfTokens()<=counter || !p->IsString(counter) )
      throw ECommandError( command+wxT("expecting dependant variable vector") );
    namey = p->GetString(counter);
    try
    {
      NumericVariable::GetVector( namey, wxT("dependant variable vector"), y );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( namey );
    ++counter;
    if( p->GetNumberOfTokens()<=counter || !p->IsString(counter) )
      throw ECommandError( command+wxT("expecting matrix or z-variable vector") );
    namez = p->GetString(counter);
    try
    {
      NumericVariable::GetVariable( namez, ndim, v1, z, dimSizes );
    }
    catch (EVariableError &e)
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
    ++counter;
    if( ndim == 1 ) // surface x y z ...
    {
      if( x.size() != y.size() )
        throw ECommandError( command+namex+wxT(" has different length than ")+namey );
      if( x.size() != z.size() )
        throw ECommandError( command+namex+wxT(" has different length than ")+namez );
    }
    else if( ndim == 2 ) // density x y matrix ...
    {
      nRow = dimSizes[0];
      if( y.size() != static_cast<std::size_t>(nRow) )
        throw ECommandError( command+namey+
          wxT(" does not have the same number of elements as the number of rows in the matrix") );
      nCol = dimSizes[1];
      if( x.size() != static_cast<std::size_t>(nCol) )
        throw ECommandError( command+namex+
         wxT(" does not have the same number of elements as the number of columns in the matrix") );
    }
  }
  double azimuth = 24.0;
  double altitude = 33.0;
  if( p->GetNumberOfTokens() > counter )
  {
    try
    {
      NumericVariable::GetScalar( p->GetString(counter), wxT("azimuth"), azimuth );
      NumericVariable::GetScalar( p->GetString(counter+1), wxT("altitude"), altitude );
    }
    catch ( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(counter) );
    AddToStackLine( p->GetString(counter+1) );
    if( altitude > 90.0 )altitude = 90.0;
    if( altitude < 0.0 )altitude = 0.0;
  }
  if( nRow == 0 )
  {
    double xmin, xmax, ymin, ymax;
    UsefulFunctions::MinMax( x, 0, x.size(), xmin, xmax );
    UsefulFunctions::MinMax( y, 0, y.size(), ymin, ymax );
    //
    nCol = static_cast<int>(sqrt(static_cast<double>(x.size()))*5.0);
    nRow = nCol;
    double xinc = (xmax-xmin)/(nCol-1);
    double yinc = (ymax-ymin)/(nCol-1);
    std::vector<double> xi, yi;
    xi.reserve(nCol);
    yi.reserve(nCol);
    for( int i=0; i<nCol-1; ++i )
    {
      xi.push_back( xmin+i*xinc );
      yi.push_back( ymin+i*yinc );
    }
    xi[nCol-1] = xmax;
    yi[nCol-1] = ymax;
    std::vector<double> array( nCol*nCol );
    try
    {
      GRA_thiessenTriangulation tt( x, y, z );
      tt.CreateMesh();
      double eps = 0.001;
      std::size_t nit = 128;
      tt.Gradients( eps, nit );
      tt.Interpolate( nCol, nCol, xi, yi, array );
    }
    catch (EExpressionError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    x.assign( xi.begin(), xi.end() );
    y.assign( yi.begin(), yi.end() );
    z.assign( array.begin(), array.end() );
  }
  //
  // clear the current graph window
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  gw->Erase();
  gw->Clear();
  //
  bool colour = qualifiers[wxT("COLOR")] || qualifiers[wxT("COLOUR")];
  bool contours = qualifiers[wxT("CONTOURS")];
  bool axes = qualifiers[wxT("AXES")];
  bool sides = qualifiers[wxT("SIDES")];
  //
  wxClientDC dc( ExGlobals::GetwxWindow() );
  GRA_surfacePlot *sp = new GRA_surfacePlot( x, y, z, nRow, azimuth, altitude,
                                             colour, contours, axes, sides );
  try
  {
    sp->Draw( ExGlobals::GetGraphicsOutput(), dc );
  }
  catch ( EGraphicsError &e )
  {
    delete sp;
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  ExGlobals::GetGraphWindow()->AddDrawableObject( sp );
  ExGlobals::RefreshGraphics();
}

// end of file
