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
#include <cmath>

#include "wx/wx.h"

#include "CMD_contour.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "UsefulFunctions.h"
#include "EGraphicsError.h"
#include "GRA_wxWidgets.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_window.h"
#include "GRA_contour.h"
#include "GRA_drawableText.h"
#include "GRA_axis.h"
#include "GRA_cartesianAxes.h"
#include "GRA_rectangle.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "ExGlobals.h"

CMD_contour *CMD_contour::cmd_contour_ = 0;

CMD_contour::CMD_contour() : Command( wxT("CONTOUR") )
{
  AddQualifier( wxT("PARTIAL"),  false );
  AddQualifier( wxT("POLAR"),    false );
  AddQualifier( wxT("SPECIFIC"), false );
  AddQualifier( wxT("LEGEND"),   false );
  AddQualifier( wxT("RESET"),    true  );
  AddQualifier( wxT("AXES"),     true  );
  AddQualifier( wxT("BORDER"),   true  );
  AddQualifier( wxT("COLORS"),   false );
  AddQualifier( wxT("VOLUMES"),  false );
  AddQualifier( wxT("AREAS"),    false );
  AddQualifier( wxT("CONTINUE"), false );
}

void CMD_contour::Execute( ParseLine const *p )
{
  // CONTOUR           { x y } v  numcntrs { minlevel { incr }}
  // CONTOUR\COLOR  colr  x y z  numcntrs { minlevel { incr }}
  // CONTOUR\INTERP  ntrp  x y z  numcntrs { minlevel { incr }}
  // CONTOUR\INTERP\COLOR ntrp colr x y z numcntrs { minlvl { incr }}
  // CONTOUR\SPECIFIC             { x y } v  cntrs
  // CONTOUR\SPECIFIC\COLOR  colr  x y z  cntrs
  // CONTOUR\SPECIFIC\INTERP  ntrp  x y z  cntrs
  // CONTOUR\SPECIFIC\INTERP\COLOR ntrp colr x y z cntrs
  // CONTOUR\CONTINUE { x y } v numcntrs { minlevel { incr }}
  // CONTOUR\CONTINUE\COLOR colr x y z numcntrs { minlevel { incr }}
  // CONTOUR\CONTINUE\SPECIFIC { x y } v cntrs
  // CONTOUR\CONTINUE\SPECIFIC\COLOR  colr x y z cntrs
  //
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch( ECommandError &e )
  {
    throw;
  }
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  wxString command( wxT("CONTOUR") );
  if( qualifiers[wxT("LEGEND")] )
  {
    if( static_cast<GRA_sizeCharacteristic*>(gw->GetGeneralCharacteristics()->Get(wxT("LEGENDSIZE")))->
        GetAsWorld() <= 0.0 )
      throw ECommandError( command+wxT("legend is on, but legend size <= 0"));
    if( !qualifiers[wxT("AXES")] )
      throw ECommandError( command+wxT("legend is on, but axes are off") );
  }
  static int cmode = 0; // used in tricp as mode
  if( qualifiers[wxT("CONTINUE")] && (cmode == 3) )
  {
    qualifiers[wxT("AXES")] = false;
    qualifiers[wxT("BORDER")] = false;
  }
  int ifld = 1, intrpx = 0, intrpy = 0, ndm = 0;
  int dimSizes[] = { 0, 0, 0 };
  double d = 0.0;
  std::vector<double> doubles;
  /*
  if( interpsize )  // interpolation size for area and volume vectors
  {
    try
    {
      NumericVariable::GetScalar( p->GetString(1), wxT("interpolation size"), d );
      intrpx = int( d );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( intrpx == 0 )
    {
      cmode = 0;
      throw ECommandError( command+wxT("interpolation size = 0"));
    }
    else if( intrpx < 0 )
    {
      cmode = 0;
      throw ECommandError( command+wxT("interpolation size < 0"));
    }
    intrpy = intrpx;
    stackLine += std::string(wxT(" ")) + p->GetString(1);
    ifld = 2;
  }
  */
  std::vector<int> colorVector;
  if( qualifiers[wxT("COLORS")] )  // color vector
  {
    if( p->GetNumberOfTokens()-1 < ifld )
      throw ECommandError( command+wxT("expecting color(s) scalar or vector") );
    if( p->IsNumeric(ifld) )
    {
      colorVector.push_back( static_cast<int>(p->GetNumeric(ifld)) );
      wxString s;
      AddToStackLine( s << p->GetNumeric(ifld++) );
    }
    else if( p->IsString(ifld) )
    {
      try
      {
        NumericVariable::GetVariable( p->GetString(ifld), ndm, d, doubles, dimSizes );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( ndm == 0 )
      {
        colorVector.push_back( static_cast<int>(d) );
      }
      else if( ndm == 1 )
      {
        std::size_t size = doubles.size();
        for( std::size_t i=0; i<size; ++i )colorVector.push_back( static_cast<int>(doubles[i]) );
      }
      else
      {
        throw ECommandError( command+wxT("color(s) must be a scalar or a vector") );
      }
      AddToStackLine( p->GetString(ifld++) );
    }
    else
    {
      throw ECommandError( command+wxT("expecting color(s) scalar or vector") );
    }
  }
  bool scatteredData = false;
  std::vector<double> xVector, yVector, zVector;
  if( p->GetNumberOfTokens()-1 < ifld || !p->IsString(ifld) )
    throw ECommandError( command+wxT("expecting matrix or independent variable") );
  try
  {
    NumericVariable::GetVariable( p->GetString(ifld), ndm, d, doubles, dimSizes );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  if( ndm == 0 )
    throw ECommandError( command+p->GetString(ifld)+wxT(" is a scalar") );
  if( ndm == 3 )
    throw ECommandError( command+p->GetString(ifld)+wxT(" is a tensor") );
  if( ndm == 2 )  // contour m ...
  {
    zVector.assign( doubles.begin(), doubles.end() );
    AddToStackLine( p->GetString(ifld++) );
    for( int i=0; i<dimSizes[1]; ++i )xVector.push_back( static_cast<double>(i+1) );
    for( int i=0; i<dimSizes[0]; ++i )yVector.push_back( static_cast<double>(i+1) );
  }
  else if( ndm == 1 )  // contour x y ...
  {
    AddToStackLine( p->GetString(ifld++) );
    if( p->GetNumberOfTokens()-1 < ifld || !p->IsString(ifld) )
      throw ECommandError( command+wxT("expecting dependent variable") );
    xVector.assign( doubles.begin(), doubles.end() );
    try
    {
      NumericVariable::GetVector( p->GetString(ifld), wxT("dependent variable"), yVector );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(ifld++) );
    if( p->GetNumberOfTokens()-1 < ifld || !p->IsString(ifld) )
      throw ECommandError( command+wxT("expecting matrix or z-vector") );
    try
    {
      NumericVariable::GetVariable( p->GetString(ifld), ndm, d, zVector, dimSizes );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( ndm == 0 )
      throw ECommandError( command+p->GetString(ifld)+wxT(" is a scalar") );
    else if( ndm == 3 )
      throw ECommandError( command+p->GetString(ifld)+wxT(" is a tensor") );
    else if( ndm == 1 )  // contour x y z ...
    {
      throw ECommandError( command+wxT("scattered data contouring not available yet") );
      //scatteredData = true;
      //stackLine += std::string(wxT(" ")) + p->GetString(ifld++);
    }
    else if( ndm == 2 )  // contour x y matrix ...
    {
      if( static_cast<int>(xVector.size()) < dimSizes[1] )
        throw ECommandError( command+wxT("independent vector length < number of columns of matrix") );
      if( static_cast<int>(xVector.size()) > dimSizes[1] )
        throw ECommandError( command+wxT("independent vector length > number of columns of matrix") );
      if( static_cast<int>(yVector.size()) < dimSizes[0] )
        throw ECommandError( command+wxT("dependent vector length < number of rows of matrix") );
      if( static_cast<int>(yVector.size()) > dimSizes[0] )
        throw ECommandError( command+wxT("dependent vector length > number of rows of matrix") );
      AddToStackLine( p->GetString(ifld++) );
    }
  }
  double xmin, xmax, ymin, ymax, zmin, zmax;
  /*
  if( scatteredData )
  {
    if( qualifiers[wxT("AREAS")] )
    {
      WarningMessage( wxT("CONTOUR"), wxT("area calculations not done for scattered data") );
      qualifiers[wxT("AREAS")] = false;
    }
    if( qualifiers[wxT("VOLUMES")] )
    {
      WarningMessage( wxT("CONTOUR"), wxT("volume calculations not done for scattered data") );
      qualifiers[wxT("VOLUMES")] = false;
    }
    if( interpsize )
      WarningMessage( wxT("CONTOUR"), wxT("mesh interpolation size will be ignored") );
    if( xVector.size() != yVector.size() ||
        xVector.size() != zVector.size() ||
        yVector.size() != zVector.size() )
    {
      throw ECommandError( command+wxT("input vectors have different lengths") );
    }
    if( qualifiers[wxT("CONTINUE")] && (cmode == 3) )goto L8;  // do not recalculate in tricp
    double zmin = zVector[0];
    double zmax = zmin;
    if( qualifiers[wxT("POLAR")] )
    {
      std::vector<double> xp, yp;
      double const degToRad = M_PI/180.;
      xmin = xVector[0]*cos(yVector[0]*degToRad);
      xmax = xmin;
      ymin = xVector[0]*sin(yVector[0]*degToRad);
      ymax = ymin;
      for( unsigned int j = 0; j < xVector.size(); ++j )
      {
        xp.push_back( xVector[j]*cos(yVector[j]*degToRad) );
        yp.push_back( xVector[j]*sin(yVector[j]*degToRad) );
        if( xmin > xp[j] )xmin = xp[j];
        if( xmax < xp[j] )xmax = xp[j];
        if( ymin > yp[j] )ymin = yp[j];
        if( ymax < yp[j] )ymax = yp[j];
        if( zmin > zVector[j] )zmin = zVector[j];
        if( zmax < zVector[j] )zmax = zVector[j];
      }
      xVector = xp;
      yVector = yp;
    }
    else  // not polar
    {
      xmin = xVector[0];
      xmax = xmin;
      ymin = xVector[0];
      ymax = ymin;
      for( unsigned int j = 0; j < xVector.size(); ++j )
      {
        if( xmin > xVector[j] )xmin = xVector[j];
        if( xmax < xVector[j] )xmax = xVector[j];
        if( ymin > yVector[j] )ymin = yVector[j];
        if( ymax < yVector[j] )ymax = yVector[j];
        if( zmin > zVector[j] )zmin = zVector[j];
        if( zmax < zVector[j] )zmax = zVector[j];
      }
    }
  }
  else // not scatteredData
  */
  {
    if( !qualifiers[wxT("CONTINUE")] || (cmode!=3) )
    {
      if( qualifiers[wxT("POLAR")] )
      {
        double const degToRad = M_PI/180.;
        xmin = xVector[0]*cos(yVector[0]*degToRad);
        xmax = xmin;
        ymin = xVector[0]*sin(yVector[0]*degToRad);
        ymax = ymin;
        std::size_t xsize( xVector.size() );
        for( std::size_t i=0; i<xsize; ++i )
        {
          std::size_t ysize( yVector.size() );
          for( std::size_t j=0; j<ysize; ++j )
          {
            double xp = xVector[j]*cos(yVector[j]*degToRad);
            double yp = xVector[j]*sin(yVector[j]*degToRad);
            if( xmin > xp )xmin = xp;
            if( xmax < xp )xmax = xp;
            if( ymin > yp )ymin = yp;
            if( ymax < yp )ymax = yp;
          }
        }
      }
      else // not polar
      {
        xmin = xVector[0];
        xmax = xmin;
        ymin = xVector[0];
        ymax = ymin;
        std::size_t xsize( xVector.size() );
        for( std::size_t j=0; j<xsize; ++j )
        {
          if( xmin > xVector[j] )xmin = xVector[j];
          if( xmax < xVector[j] )xmax = xVector[j];
        }
        std::size_t ysize( yVector.size() );
        for( std::size_t j=0; j<ysize; ++j )
        {
          if( ymin > yVector[j] )ymin = yVector[j];
          if( ymax < yVector[j] )ymax = yVector[j];
        }
      }
      if( xmax == xmin )
        throw ECommandError( command+wxT("minimum = maximum for independent variable") );
      if( ymax == ymin )
        throw ECommandError( command+wxT("minimum = maximum for dependent variable") );
    }
  }
  double xuaxisp=0.0, yuaxisp=0.0, xuaxis=0.0, yuaxis=0.0;
  double xll, yll, xll0;
  GRA_setOfCharacteristics *xAxis = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxis = gw->GetYAxisCharacteristics();
  wxClientDC dc( ExGlobals::GetwxWindow() );
  if( !qualifiers[wxT("CONTINUE")] || (cmode != 3) )
  {
    if( qualifiers[wxT("AXES")] )
    {
      if( qualifiers[wxT("LEGEND")] )
      {
        GRA_distanceCharacteristic *xupperaxis =
          static_cast<GRA_distanceCharacteristic*>(xAxis->Get(wxT("UPPERAXIS")));
        GRA_distanceCharacteristic *yupperaxis =
          static_cast<GRA_distanceCharacteristic*>(yAxis->Get(wxT("UPPERAXIS")));
        xuaxisp = xupperaxis->GetAsPercent();
        xupperaxis->SetAsPercent(75.0);
        yuaxisp = yupperaxis->GetAsPercent();
        yupperaxis->SetAsPercent(90.0);
        xuaxis = xupperaxis->GetAsWorld();
        yuaxis = yupperaxis->GetAsWorld();
      }
      std::vector<double> xdum, ydum, xe1, ye1, xe2, ye2;
      xdum.push_back( xmin );
      xdum.push_back( xmax );
      xdum.push_back( xmax );
      xdum.push_back( xmin );
      xdum.push_back( xmin );
      ydum.push_back( ymax );
      ydum.push_back( ymax );
      ydum.push_back( ymin );
      ydum.push_back( ymin );
      ydum.push_back( ymax );
      GRA_cartesianAxes *cartesianAxes = new GRA_cartesianAxes( xdum, ydum, false, false );
      try
      {
        cartesianAxes->Make();
      }
      catch (EGraphicsError &e)
      {
        delete cartesianAxes;
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      cartesianAxes->Draw( ExGlobals::GetGraphicsOutput(), dc );
      gw->AddDrawableObject( cartesianAxes );
      if( qualifiers[wxT("BORDER")] )
      {
        double xlo, ylo, xhi, yhi;
        gw->GraphToWorld( xmin, ymin, xlo, ylo, true );
        gw->GraphToWorld( xmax, ymax, xhi, yhi );
        GRA_rectangle *border = new GRA_rectangle(xlo,ylo,xhi,yhi,0.0,false,
                                                  GRA_colorControl::GetColor(wxT("BLACK")),0,1);

        border->Draw( ExGlobals::GetGraphicsOutput(), dc );
        gw->AddDrawableObject( border );
      }
      if( qualifiers[wxT("LEGEND")] )
      {
        GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
        GRA_fontCharacteristic *generalFont =
          static_cast<GRA_fontCharacteristic*>(general->Get(wxT("FONT")));
        GRA_sizeCharacteristic *xLargeTicLength =
          static_cast<GRA_sizeCharacteristic*>(xAxis->Get(wxT("LARGETICLENGTH")));
        GRA_sizeCharacteristic *legendSize =
          static_cast<GRA_sizeCharacteristic*>(general->Get(wxT("LEGENDSIZE")));
        GRA_sizeCharacteristic *contourLabelHeight =
          static_cast<GRA_sizeCharacteristic*>(general->Get(wxT("CONTOURLABELHEIGHT")));
        GRA_font *font = generalFont->Get();
        xll = xuaxis + 1.1 * xLargeTicLength->GetAsWorld();
        yll = yuaxis;
        double sizleg = legendSize->GetAsWorld();
        double ht = sizleg;
        double angle = 50.0;
        double xll1, xll2;
        if( contourLabelHeight->GetAsWorld() > 0.0 )
        {
          xll0 = xll + sizleg;
          xll1 = xll0 + 3*sizleg;
          xll2 = xll1 + 1.5*sizleg;
          GRA_drawableText *tb = new GRA_drawableText(wxT("label"),ht,angle,xll0,yll,
                                                      1,font,GRA_colorControl::GetColor(wxT("BLACK")));
          try
          {
            tb->Parse();
          }
          catch ( EGraphicsError &e )
          {
            throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
          }
          tb->Draw( ExGlobals::GetGraphicsOutput(), dc );
          gw->AddDrawableObject( tb );
        }
        else
        {
          sizleg = 1.2 * sizleg;
          xll0 = xll + sizleg;
          xll1 = xll0 + 3 * sizleg;
          xll2 = xll1 + 2 * sizleg;
        }
        GRA_drawableText *tb = new GRA_drawableText(wxT("contour"),ht,angle,xll1,yll,
                                                    1,font,GRA_colorControl::GetColor(wxT("BLACK")));
        try
        {
          tb->Parse();
        }
        catch ( EGraphicsError &e )
        {
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
        tb->Draw( ExGlobals::GetGraphicsOutput(), dc );
        gw->AddDrawableObject( tb );
        tb = new GRA_drawableText(wxT("value"),ht,angle,xll2,yll,
                                  1,font,GRA_colorControl::GetColor(wxT("BLACK")));
        try
        {
          tb->Parse();
        }
        catch ( EGraphicsError &e )
        {
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
        tb->Draw( ExGlobals::GetGraphicsOutput(), dc );
        gw->AddDrawableObject( tb );
      }
    }
  }
  int i1 = 0;
  int i2 = xVector.size()-1;
  int j1 = 0;
  int j2 = yVector.size()-1;
  if( qualifiers[wxT("PARTIAL")] )
  {
    GRA_doubleCharacteristic *xminC =
      static_cast<GRA_doubleCharacteristic*>(xAxis->Get(wxT("MIN")));
    GRA_doubleCharacteristic *yminC =
      static_cast<GRA_doubleCharacteristic*>(yAxis->Get(wxT("MIN")));
    GRA_doubleCharacteristic *xmaxC =
      static_cast<GRA_doubleCharacteristic*>(xAxis->Get(wxT("MAX")));
    GRA_doubleCharacteristic *ymaxC =
      static_cast<GRA_doubleCharacteristic*>(yAxis->Get(wxT("MAX")));
    double xmn = xminC->Get();
    double xmx = xmaxC->Get();
    double ymn = yminC->Get();
    double ymx = ymaxC->Get();
    i1 = std::max( 0, static_cast<int>((xmn-xmin)/(xmax-xmin)*(xVector.size()-1)) );
    i2 = std::min( static_cast<int>(xVector.size())-1,
                   static_cast<int>((xmx-xmin)/(xmax-xmin)*(xVector.size()-1)) );
    j1 = std::max( 0, static_cast<int>((ymn-ymin)/(ymax-ymin)*(yVector.size()-1)) );
    j2 = std::min( static_cast<int>(yVector.size())-1,
                   static_cast<int>((ymx-ymin)/(ymax-ymin)*(yVector.size()-1)) );
  }
  std::vector<double> contourVector;
  double zincs = 0.0;
  double zmins = 0.0;
  int numberOfContours = 0;
  if( qualifiers[wxT("SPECIFIC")] )
  {
    //
    // a vector of contour levels is expected
    // a vector of colors is optional
    //
    // contour\specific             { x y } v  cntrs
    // contour\specific\color  colr  x y z  cntrs
    // contour\specific\interp  ntrp  x y z  cntrs
    // contour\specific\interp\color ntrp colr x y z cntrs
    //
    if( p->GetNumberOfTokens()-1 < ifld )
    {
      throw ECommandError( command+wxT("expecting contour levels (scalar or vector)") );
    }
    if( p->IsNumeric(ifld) )
    {
      contourVector.push_back( p->GetNumeric(ifld) );
      wxString s;
      AddToStackLine( s << p->GetNumeric(ifld++) );
    }
    else if( p->IsString(ifld) )
    {
      try
      {
        NumericVariable::GetVariable( p->GetString(ifld), ndm, d, contourVector, dimSizes );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( ndm == 0 )contourVector.push_back( d );
      else if( ndm != 1 )throw ECommandError( command+wxT("expecting contour levels (scalar or vector)") );
      AddToStackLine( p->GetString(ifld++) );
    }
    else
      throw ECommandError( command+wxT("expecting contour level(s)") );
    numberOfContours = contourVector.size();
  }
  else // a number of contours is entered instead of a list of contour levels
  {
    // contour           { x y } v  numcntrs { minlevel { incr }}
    // contour\color  colr  x y z  numcntrs { minlevel { incr }}
    // contour\interp  ntrp  x y z  numcntrs { minlevel { incr }}
    // contour\interp\color ntrp colr x y z numcntrs { minlvl { incr }}
    //
    if( p->GetNumberOfTokens() <= ifld )
      throw ECommandError( command+wxT("expecting the number of contours") );
    double dNumC;
    try
    {
      NumericVariable::GetScalar( p->GetString(ifld), wxT("number of contours"), dNumC );
    }
    catch (EVariableError &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    numberOfContours = abs( static_cast<int>(dNumC) );
    //
    // dNumC < 0 means use real matrix values
    // dNumC > 0 means use nice numbers
    //
    if( numberOfContours == 0 )throw ECommandError( command+wxT("number of contours requested = 0"));
    else if( numberOfContours < 2 )throw ECommandError( command+wxT("number of contours requested < 2") );
    AddToStackLine( p->GetString(ifld++) );
    //
    bool zminsEntered = false;
    if( p->GetNumberOfTokens() > ifld )
    {
      NumericVariable::GetScalar( p->GetString(ifld), wxT("minimum contour level"), d );
      zmins = d;
      AddToStackLine( p->GetString(ifld++) );
      zminsEntered = true;
      if( p->GetNumberOfTokens() > ifld )
      {
        NumericVariable::GetScalar( p->GetString(ifld), wxT("contour level increment"), d );
        zincs = d;
        if( zincs == 0.0 )
        {
          if( qualifiers[wxT("LEGEND")] )
          {
            if( qualifiers[wxT("RESET")] )
            {
              static_cast<GRA_distanceCharacteristic*>(xAxis->Get(wxT("UPPERAXIS")))->SetAsPercent(xuaxisp);
              static_cast<GRA_distanceCharacteristic*>(yAxis->Get(wxT("UPPERAXIS")))->SetAsPercent(yuaxisp);
            }
          }
          cmode = 0;
          throw ECommandError( command+wxT("contour increment = 0") );
        }
        AddToStackLine( p->GetString(ifld++) );
      }
    }
    double zmin1, zmax1;
    if( zincs != 0.0 )
    {
      zmin1 = zmins;
      zmax1 = zmins + (numberOfContours-1)*zincs;
    }
    else
    {
      /*
      if( scatteredData )
      {
        if( qualifiers[wxT("PARTIAL")] )
        {
          zmin = std::numeric_limits<double>::max();
          zmax = -zmin;
          for( int ii = 0; ii < xVector.size(); ++ii )
          {
            if( xmn <= xVector[ii] && xmx >= xVector[ii] )
            {
              if( ymn <= yVector[ii] && ymx >= yVector[ii] )
              {
                if( zmin > zVector[ii] )zmin = zVector[ii];
                if( zmax < zVector[ii] )zmax = zVector[ii];
              }
            }
          }
        }
      }
      else // not scatteredData
      */
      {
        zmin = zVector[j1+i1*yVector.size()];
        zmax = zmin;
        for( int ii=i1; ii<=i2; ++ii )
        {
          for( int j=j1; j<=j2; ++j )
          {
            double xtmp = zVector[j+ii*yVector.size()];
            if( zmax < xtmp )zmax = xtmp;
            if( zmin > xtmp )zmin = xtmp;
          }
        }
      }
      //
      // find some "nice" numbers for min, max and increment
      //
      double zinc = (zmax-zmin) / (numberOfContours-1);
      if( dNumC < 0.0 )  // use real matrix values
      {
        if( zminsEntered )
        {
          zincs = (zmax-zmins) / (numberOfContours+1);
        }
        else
        {
          zincs = (zmax-zmin) / (numberOfContours+1);
          zmins = zmin+zinc;
        }
      }
      else               // use nice numbers
      {
        if( zminsEntered )zmin = zmins;
        double zmaxs;
        try
        {
          UsefulFunctions::Scale1( zmins, zmaxs, zincs, numberOfContours, zmin, zmax );
        }
        catch ( std::runtime_error &e )
        {
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
        numberOfContours = static_cast<int>( (zmaxs-zmins)/zincs ) + 1;
        zmin1 = zmins;
        zmax1 = zmaxs;
        while( zmin1 <= zmin )
        {
          zmin1 += zincs;
          zmins = zmin1;
          --numberOfContours;
        }
        while( zmax1 >= zmax )
        {
          zmax1 -= zincs;
          zmaxs = zmax1;
          --numberOfContours;
        }
      }
    }
  }
  int nx10, ny10;
  std::vector<double> iv1( numberOfContours, 0.0 );
  std::vector<double> iv2( numberOfContours, 0.0 );
  std::vector<double> iv3( numberOfContours, 0.0 );
  std::vector<double> iv4( numberOfContours, 0.0 );
  std::vector<double> iva( numberOfContours, 0.0 );
  std::vector<double> bins1( numberOfContours, 0.0 );
  std::vector<double> ivv( numberOfContours, 0.0 );
  std::vector<double> bins2( numberOfContours, 0.0 );
  double totalArea = 0.0, totalVolume = 0.0;
  if( !scatteredData ) // get space: area, volume * contour vectors
  {
    //if( !interpsize )
    {
      if( xVector.size() < 20 )intrpx = 10;
      if( xVector.size() >= 20 && xVector.size() < 50 )intrpx = 5;
      if( xVector.size() >= 50 && xVector.size() < 100 )intrpx = 3;
      if( xVector.size() >= 100 )intrpx = 2;
      //
      if( yVector.size() < 20 )intrpy = 10;
      if( yVector.size() >= 20 && yVector.size() < 50 )intrpy = 5;
      if( yVector.size() >= 50 && yVector.size() < 100 )intrpy = 5;
      if( yVector.size() >= 100 )intrpy = 2;
    }
    nx10 = (i2-i1)*intrpx + 1;
    ny10 = (j2-j1)*intrpy + 1;
  }
  if( qualifiers[wxT("AREAS")] || qualifiers[wxT("VOLUMES")] )
  {
    if( qualifiers[wxT("AREAS")] )totalArea = nx10*ny10;
    for( int ii=0; ii<nx10; ++ii )
    {
      int ixl = ii/intrpx + i1;
      int ixu = ixl + 1;
      if( ixu > i2 )ixu = i2;
      double xfrac = static_cast<double>(ii)/intrpx - ixl + i1;
      for( int j = 0; j < ny10; ++j )
      {
        int jyl = j/intrpy + j1;
        int jyu = jyl + 1;
        if( jyu > j2 )jyu = j2;
        double yfrac = static_cast<double>(j)/intrpy - jyl + j1;
        double z = (1-yfrac)*xfrac*zVector[jyu+ixl*yVector.size()] +
            (1-yfrac)*(1-xfrac)*zVector[jyl+ixl*yVector.size()] +
            xfrac*yfrac*zVector[jyu+ixu*yVector.size()] +
            yfrac*(1-xfrac)*zVector[jyl+ixu*yVector.size()];
        if( qualifiers[wxT("VOLUMES")] )totalVolume += fabs( z );
        for( int k=0; k<numberOfContours; ++k )
        {
          double hk = zmins + k*zincs; // hk is the contour level
          if( qualifiers[wxT("SPECIFIC")] )hk = contourVector[k];
          if( hk >= 0.0 )
          {
            if( z > hk )
            {
              if(   qualifiers[wxT("AREAS")] )bins1[k] += 1.0;
              if( qualifiers[wxT("VOLUMES")] )bins2[k] += fabs(z);
            }
          }
          else
          {
            if( z < hk )
            {
              if(   qualifiers[wxT("AREAS")] )bins1[k] += 1.0;
              if( qualifiers[wxT("VOLUMES")] )bins2[k] += fabs(z);
            }
          }
        }
      }
    }
  }
  static int cvalSave;
  if( cmode == 0 )cvalSave = 1;
  double cval, tour, areafrac, volmfrac;
  for( int i=0; i<numberOfContours; ++i )
  {
    // tour is the contour level
    qualifiers[wxT("SPECIFIC")] ? tour=contourVector[i] : tour=zmins+i*zincs;
    if( !scatteredData )
    {
      if( qualifiers[wxT("AREAS")] )
      {
        areafrac = 100.0*bins1[i]/totalArea;
        iva[i] = areafrac;
      }
      if( qualifiers[wxT("VOLUMES")] )
      {
        volmfrac = 100*bins2[i]/totalVolume;
        ivv[i] = volmfrac;
      }
    }
    cval = tour;
    if( qualifiers[wxT("LEGEND")] )
    {
      GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
      cval = static_cast<double>(i+cvalSave);
      int ii = i + cvalSave;
      double legendSize =
        static_cast<GRA_sizeCharacteristic*>(general->Get(wxT("LEGENDSIZE")))->GetAsWorld();
      yll -= 1.4*legendSize;
      wxChar c[200];
      double contourLabelHeight =
        static_cast<GRA_sizeCharacteristic*>(general->Get(wxT("CONTOURLABELHEIGHT")))->GetAsWorld();
      if( contourLabelHeight > 0.0 )
      {
        /*
        if( qualifiers[wxT("AREAS")] )
        {
          if( qualifiers[wxT("VOLUMES")] )
          {
            sprintf( c, wxT("%2d=%9.2g %5.1g %5.1g"),ii,tour,areafrac,volmfrac);
          }
          else
          {
            sprintf( c, wxT("%2d=%9.2g %5.2g"),ii,tour,areafrac);
          }
        }
        else
        {
          if( qualifiers[wxT("VOLUMES")] )
          {
            sprintf( c, wxT("%2d=%9.2g %5.2g"),ii,tour,volmfrac);
          }
          else
          {
            //sprintf( c, wxT("%2i =")+lgfrmt(1:nlgfrmt),ii,tour);
            sprintf( c, wxT("%2i = %9.2g"), ii, tour );
          }
        }
        */
        ::wxSnprintf( c, 200, wxT("%2i = %9.2g"), ii, tour );
      }
      else if( contourLabelHeight == 0.0 )
      {
        /*
        if( qualifiers[wxT("AREAS")] )
        {
          if( qualifiers[wxT("VOLUMES")] )
          {
            sprintf( c, wxT("%9.2g %5.1g %5.1g"),tour,areafrac,volmfrac);
          }
          else
          {
            sprintf( c, wxT("%9.2g %5.1g %5.1g"),tour,areafrac,volmfrac);
          }
        }
        else
        {
          if( qualifiers[wxT("VOLUMES")] )
          {
            sprintf( c, wxT("%9.2g %5.1g"),tour,volmfrac );
          }
          else
          {
            //sprintf( c, lgfrmt,tour);
            sprintf( c, wxT("%9.2g"), tour );
          }
        }
        */
        ::wxSnprintf( c, 200, wxT("%9.2g"), tour );
      }
      double angle = 0.0;
      if( yll > 0.0 )
      {
        GRA_font *font = static_cast<GRA_fontCharacteristic*>(general->Get(wxT("FONT")))->Get();
        GRA_color *color =
         colorVector.empty() ? GRA_colorControl::GetColor(wxT("BLACK")) :
                               GRA_colorControl::GetColor(colorVector[i%colorVector.size()]);
        GRA_drawableText *dt = new GRA_drawableText(c,legendSize,angle,xll0,yll,1,font,color);
        try
        {
          dt->Parse();
        }
        catch ( EGraphicsError &e )
        {
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
        dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
        gw->AddDrawableObject( dt );
      }
    }
    //npltd = 0;
    /*
    if( scatteredData )
    {
      call tricp(r8d(initx+1),r8d(inity+1),r8d(initz+1),num
                 ,tour,cval,r8d(initwk+1),i4d(initiwk+1),cmode,*92);
    }
    else
    */
    {
      //tb1_xmin =  std::numeric_limits<double>::max();
      //tb1_xmax = -tb1_xmin;
      //tb1_ymin =  tb1_xmin;
      //tb1_ymax = -tb1_ymin;
      std::size_t xsize = xVector.size();
      std::size_t ysize = yVector.size();
      //
      std::vector< std::vector<double> > zMatrix;
      zMatrix.reserve( ysize );
      zMatrix.resize( ysize );
      std::vector<double> temp;
      temp.resize( xsize );
      for( std::size_t j=0; j<ysize; ++j )
      {
        for( std::size_t k=0; k<xsize; ++k )temp[k] = zVector[j+k*ysize];
        zMatrix[j].resize( xsize );
        zMatrix[j] = temp;
      }
      GRA_color *color =
       colorVector.empty() ? GRA_colorControl::GetColor(wxT("BLACK")) :
                             GRA_colorControl::GetColor(colorVector[i%colorVector.size()]);
      GRA_contour *contour =
        new GRA_contour( tour, cval, color, xVector, yVector, zMatrix, qualifiers[wxT("POLAR")] );
      try
      {
        contour->Make();
      }
      catch (EGraphicsError &e)
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      contour->Draw( ExGlobals::GetGraphicsOutput(), dc );
      gw->AddDrawableObject( contour );
      //iv1[i] = tb1_xmin;
      //iv2[i] = tb1_xmax;
      //iv3[i] = tb1_ymin;
      //iv4[i] = tb1_ymax;
    }
    cmode = 3;
    /*
    if( coords )
    {
      mx[1+(i-1)*nr] = npltd;
      my[1+(i-1)*nr] = npltd;
      for( int irw = 1; irw <= npltd; ++irw )
      {
        mx[irw+1+(i-1)*nr] = vx[irw];
        my[irw+1+(i-1)*nr] = vy[irw];
      }
    }
    */
  }
  // save the last legend value for the next plot
  //
  if( qualifiers[wxT("CONTINUE")] )cvalSave = static_cast<int>(cval);
  /*
  if( coords )
  {
  //
    std::string const nameArea(wxT("CAREA"));
    std::string const nameVolm(wxT("CVOLM"));
    std::string const nameCont(wxT("CCONT"));
    std::string const nameXmin(wxT("CXMIN"));
    std::string const nameXmax(wxT("CXMAX"));
    std::string const nameYmin(wxT("CYMIN"));
    std::string const nameYmax(wxT("CYMAX"));
    std::string const nameXcnt(wxT("XCNT"));
    std::string const nameYcnt(wxT("YCNT"));
    call put_variable( namexcnt, 0,idx1,idx2,idx3
                       ,2,0.0d0,imx,nr,numberOfContours,0,strings(1)(1:lenst(1)),'contour',*92);
    call put_variable( nameycnt, 0,idx1,idx2,idx3
                       ,2,0.0d0,imy,nr,numberOfContours,0,strings(1)(1:lenst(1)),'contour',*92);
  }
  if( !qualifiers[wxT("SPECIFIC")] )
    call put_variable( namecont, 0,idx1,idx2,idx3
                       ,1,0.0d0,ivc,numberOfContours,0,0,strings(1)(1:lenst(1)),'contour',*92);
  if( !scatteredData )   // area, volumn, etc. vectors
  {
    if( qualifiers[wxT("AREAS")] )call put_variable( namearea, 0,idx1,idx2,idx3
                             ,1,0.0d0,iva,numberOfContours,0,0,strings(1)(1:lenst(1)),'contour',*92);
    if( qualifiers[wxT("VOLUMES")] )call put_variable( namevolm, 0,idx1,idx2,idx3
                               ,1,0.0d0,ivv,numberOfContours,0,0,strings(1)(1:lenst(1)),'contour',*92);
    call put_variable( namexmin, 0,idx1,idx2,idx3
                       ,1,0.0d0,iv1,numberOfContours,0,0,strings(1)(1:lenst(1)),'contour',*92);
    call put_variable( namexmax, 0,idx1,idx2,idx3
                       ,1,0.0d0,iv2,numberOfContours,0,0,strings(1)(1:lenst(1)),'contour',*92);
    call put_variable( nameymin, 0,idx1,idx2,idx3
                       ,1,0.0d0,iv3,numberOfContours,0,0,strings(1)(1:lenst(1)),'contour',*92);
    call put_variable( nameymax, 0,idx1,idx2,idx3
                       ,1,0.0d0,iv4,numberOfContours,0,0,strings(1)(1:lenst(1)),'contour',*92);
  }
  */
  if( qualifiers[wxT("LEGEND")] )
  {
    if( qualifiers[wxT("RESET")] )
    {
      static_cast<GRA_distanceCharacteristic*>(xAxis->Get(wxT("UPPERAXIS")))->SetAsPercent( xuaxisp );
      static_cast<GRA_distanceCharacteristic*>(yAxis->Get(wxT("UPPERAXIS")))->SetAsPercent( yuaxisp );
    }
  }
  if( !qualifiers[wxT("CONTINUE")] )cmode = 0;
  ExGlobals::RefreshGraphics();
}

// end of file
