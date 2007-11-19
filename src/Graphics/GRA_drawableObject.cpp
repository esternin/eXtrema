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
#include "wx/wx.h"

#include "GRA_drawableObject.h"

GRA_drawableObject::GRA_drawableObject( wxChar const *type )
{
  type_ = UNKNOWN;
  wxString stype( type );
  if( stype == wxT("POINT") )               type_ = POINT;
  else if( stype == wxT("POLYLINE") )       type_ = POLYLINE;
  else if( stype == wxT("ELLIPSE") )        type_ = ELLIPSE;
  else if( stype == wxT("POLYGON") )        type_ = POLYGON;
  else if( stype == wxT("MULTILINEFIGURE") )type_ = MULTILINEFIGURE;
  else if( stype == wxT("PLOTSYMBOL") )     type_ = PLOTSYMBOL;
  else if( stype == wxT("DRAWABLETEXT") )   type_ = DRAWABLETEXT;
  else if( stype == wxT("AXIS") )           type_ = AXIS;
  else if( stype == wxT("CARTESIANCURVE") ) type_ = CARTESIANCURVE;
  else if( stype == wxT("CARTESIANAXES") )  type_ = CARTESIANAXES;
  else if( stype == wxT("GRAPHLEGEND") )    type_ = GRAPHLEGEND;
  else if( stype == wxT("CONTOUR") )        type_ = CONTOUR;
  else if( stype == wxT("BOXPLOT") )        type_ = BOXPLOT;
  else if( stype == wxT("DIFFUSIONPLOT") )  type_ = DIFFUSIONPLOT;
  else if( stype == wxT("DITHERINGPLOT") )  type_ = DITHERINGPLOT;
  else if( stype == wxT("GRADIENTPLOT") )   type_ = GRADIENTPLOT;
  else if( stype == wxT("THREEDFIGURE") )   type_ = THREEDFIGURE;
  else if( stype == wxT("POLARAXES") )      type_ = POLARAXES;
  else if( stype == wxT("POLARCURVE") )     type_ = POLARCURVE;
}

// end of file
