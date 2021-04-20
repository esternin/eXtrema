/*
Copyright (C) 2005,...,2010 Joseph L. Chuma

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
#pragma hdrstop

#include "GRA_drawableObject.h"

GRA_drawableObject::GRA_drawableObject( char const *type )
{
  type_ = UNKNOWN;
  ExString stype( type );
  if( stype == "POINT" )               type_ = POINT;
  else if( stype == "POLYLINE" )       type_ = POLYLINE;
  else if( stype == "ELLIPSE" )        type_ = ELLIPSE;
  else if( stype == "POLYGON" )        type_ = POLYGON;
  else if( stype == "MULTILINEFIGURE" )type_ = MULTILINEFIGURE;
  else if( stype == "PLOTSYMBOL" )     type_ = PLOTSYMBOL;
  else if( stype == "DRAWABLETEXT" )   type_ = DRAWABLETEXT;
  else if( stype == "AXIS" )           type_ = AXIS;
  else if( stype == "CARTESIANCURVE" ) type_ = CARTESIANCURVE;
  else if( stype == "CARTESIANAXES" )  type_ = CARTESIANAXES;
  else if( stype == "GRAPHLEGEND" )    type_ = GRAPHLEGEND;
  else if( stype == "CONTOUR" )        type_ = CONTOUR;
  else if( stype == "BOXPLOT" )        type_ = BOXPLOT;
  else if( stype == "DIFFUSIONPLOT" )  type_ = DIFFUSIONPLOT;
  else if( stype == "DITHERINGPLOT" )  type_ = DITHERINGPLOT;
  else if( stype == "GRADIENTPLOT" )   type_ = GRADIENTPLOT;
  else if( stype == "THREEDFIGURE" )   type_ = THREEDFIGURE;
  else if( stype == "POLARAXES" )      type_ = POLARAXES;
  else if( stype == "POLARCURVE" )     type_ = POLARCURVE;
  else if( stype == "THREEDPLOT" )     type_ = THREEDPLOT;
}

// end of file
