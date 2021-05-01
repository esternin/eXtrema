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
#include <cmath>

#include "wx/wx.h"

#include "CMD_contour.h"
#include "GRA_contourLine.h"
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
#include "GRA_drawableText.h"
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
  AddQualifier( wxT("PARTIAL"),    false );
  AddQualifier( wxT("POLAR"),      false );
  AddQualifier( wxT("SPECIFIC"),   false );
  AddQualifier( wxT("LEGEND"),     false );
  AddQualifier( wxT("RESET"),      true  );
  AddQualifier( wxT("AXES"),       true  );
  AddQualifier( wxT("BORDER"),     true  );
  AddQualifier( wxT("COLORS"),     false );
  AddQualifier( wxT("COLOURS"),    false );
  AddQualifier( wxT("LINETYPES"),  false );
  AddQualifier( wxT("LINEWIDTHS"), false );
  AddQualifier( wxT("CONTINUE"),   false );
}

void CMD_contour::Execute( ParseLine const *p )
{
  // CONTOUR           { x y } v  numcntrs { minlevel { incr }}
  // CONTOUR\COLOR  colr  x y z  numcntrs { minlevel { incr }}
  // CONTOUR\SPECIFIC             { x y } v  cntrs
  // CONTOUR\SPECIFIC\COLOR  colr  x y z  cntrs
  // CONTOUR\CONTINUE { x y } v numcntrs { minlevel { incr }}
  // CONTOUR\CONTINUE\COLOR colr x y z numcntrs { minlevel { incr }}
  // CONTOUR\CONTINUE\SPECIFIC { x y } v cntrs
  // CONTOUR\CONTINUE\SPECIFIC\COLOR  colr x y z cntrs
  //
  QualifierMap qualifiers;
  SetUp( p, qualifiers );
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
  if( qualifiers[wxT("CONTINUE")] )
  {
    qualifiers[wxT("AXES")] = false;
    qualifiers[wxT("BORDER")] = false;
  }
  int ifld = 1;
  std::vector<int> colors, linetypes, linewidths;
  std::vector<double> xVector, yVector, zVector;
  if( qualifiers[wxT("COLORS")]||qualifiers[wxT("COLOURS")] )
    GetIntParameter( colors, p, ifld, wxT("expecting color (scalar or vector)") );
  if( qualifiers[wxT("LINETYPES")] )
    GetIntParameter( linetypes, p, ifld, wxT("expecting linetype (scalar or vector)") );
  if( qualifiers[wxT("LINEWIDTHS")] )
    GetIntParameter( linewidths, p, ifld, wxT("expecting linewidth (scalar or vector)") );
  GetDataParameters( xVector, yVector, zVector, p, ifld );
  double xmin, xmax, ymin, ymax, zmin, zmax;
  if( !qualifiers[wxT("CONTINUE")] )
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
  double xuaxisp=0.0, yuaxisp=0.0, xuaxis=0.0, yuaxis=0.0;
  double xll, yll, xll0;
  GRA_setOfCharacteristics *xAxis = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxis = gw->GetYAxisCharacteristics();
  wxClientDC dc( ExGlobals::GetwxWindow() );
  if( !qualifiers[wxT("CONTINUE")] )
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
      catch (EGraphicsError const &e)
      {
        delete cartesianAxes;
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      //cartesianAxes->Draw( ExGlobals::GetGraphicsOutput(), dc );
      gw->AddDrawableObject( cartesianAxes );
      if( qualifiers[wxT("BORDER")] )
      {
        double xlo, ylo, xhi, yhi;
        gw->GraphToWorld( xmin, ymin, xlo, ylo, true );
        gw->GraphToWorld( xmax, ymax, xhi, yhi );
        GRA_rectangle *border = new GRA_rectangle(xlo,ylo,xhi,yhi,0.0,false,
                                                  GRA_colorControl::GetColor(wxT("BLACK")),0,1);

        //border->Draw( ExGlobals::GetGraphicsOutput(), dc );
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
          catch ( EGraphicsError const &e )
          {
            delete tb;
            throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
          }
          //tb->Draw( ExGlobals::GetGraphicsOutput(), dc );
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
        catch ( EGraphicsError const &e )
        {
          delete tb;
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
        //tb->Draw( ExGlobals::GetGraphicsOutput(), dc );
        gw->AddDrawableObject( tb );
        tb = new GRA_drawableText(wxT("value"),ht,angle,xll2,yll,
                                  1,font,GRA_colorControl::GetColor(wxT("BLACK")));
        try
        {
          tb->Parse();
        }
        catch ( EGraphicsError const &e )
        {
          delete tb;
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
        //tb->Draw( ExGlobals::GetGraphicsOutput(), dc );
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
  if( qualifiers[wxT("SPECIFIC")] ) // a scalar or a vector of contour levels is expected
  {
    if( p->GetNumberOfTokens()-1 < ifld )
    {
      throw ECommandError( command+wxT("expecting contour levels (scalar or vector)") );
    }
    if( p->IsNumeric(ifld) )
    {
      contourVector.push_back( p->GetNumeric(ifld) );
      AddToStackLine( wxString() << p->GetNumeric(ifld++) );
    }
    else if( p->IsString(ifld) )
    {
      int ndm=0, dimSizes[]={0,0,0};
      double d=0.0;
      try
      {
        NumericVariable::GetVariable( p->GetString(ifld), ndm, d, contourVector, dimSizes );
      }
      catch( EVariableError const &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( ndm == 0 )contourVector.push_back( d );
      else if( ndm != 1 )
        throw ECommandError( command+wxT("expecting contour levels (scalar or vector)") );
      AddToStackLine( p->GetString(ifld++) );
    }
    else
      throw ECommandError( command+wxT("expecting contour level(s)") );
    numberOfContours = contourVector.size();
  }
  else // a number of contours is entered instead of a list of contour levels
  {
    if( p->GetNumberOfTokens() <= ifld )
      throw ECommandError( command+wxT("expecting the number of contours") );
    double dNumC;
    try
    {
      NumericVariable::GetScalar( p->GetString(ifld), wxT("number of contours"), dNumC );
    }
    catch (EVariableError const &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    numberOfContours = abs( static_cast<int>(dNumC) );
    if( numberOfContours < 2 )throw ECommandError( command+wxT("number of contours requested < 2") );
    AddToStackLine( p->GetString(ifld++) );
    //
    bool zminsEntered = false;
    if( p->GetNumberOfTokens() > ifld )
    {
      double d;
      NumericVariable::GetScalar( p->GetString(ifld), wxT("minimum contour level"), d );
      zmins = d;
      AddToStackLine( p->GetString(ifld++) );
      zminsEntered = true;
      if( p->GetNumberOfTokens() > ifld )
      {
        NumericVariable::GetScalar( p->GetString(ifld), wxT("contour level increment"), d );
        zincs = d;
        if( zincs == 0.0 )
          throw ECommandError( command+wxT("contour increment = 0") );
        AddToStackLine( p->GetString(ifld++) );
      }
    }
    if( zincs == 0.0 )
    {
      zmax = zVector[j1+i1*yVector.size()];
      for( int ii=i1; ii<=i2; ++ii )
      {
        for( int j=j1; j<=j2; ++j )
        {
          if( zmax < zVector[j+ii*yVector.size()] )zmax = zVector[j+ii*yVector.size()];
        }
      }
      if( zminsEntered )zmin = zmins;
      else
      {
        zmin = zVector[j1+i1*yVector.size()];
        for( int ii=i1; ii<=i2; ++ii )
        {
          for( int j=j1; j<=j2; ++j )
          {
            if( zmin > zVector[j+ii*yVector.size()] )zmin = zVector[j+ii*yVector.size()];
          }
        }
      }
      //
      // find some "nice" numbers for min, max and increment
      //
      double zmaxs;
      try
      {
        UsefulFunctions::Scale1( zmins, zmaxs, zincs, numberOfContours, zmin, zmax );
      }
      catch ( std::runtime_error const &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      numberOfContours = static_cast<int>( (zmaxs-zmins)/zincs ) + 1;
      while( zmins < zmin )
      {
        zmins += zincs;
        --numberOfContours;
      }
      while( zmaxs > zmax )
      {
        zmaxs -= zincs;
        --numberOfContours;
      }
    }
  }
  static int labelSave = 1;
  double label, level;
  for( int i=0; i<numberOfContours; ++i )
  {
    // level is the contour level
    // label is the contour label
    //
    level = qualifiers[wxT("SPECIFIC")] ? contourVector[i] : zmins+i*zincs;
    label = level;
    if( qualifiers[wxT("LEGEND")] )
    {
      GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
      label = static_cast<double>(i+labelSave);
      int ii = i + labelSave;
      double legendSize =
        static_cast<GRA_sizeCharacteristic*>(general->Get(wxT("LEGENDSIZE")))->GetAsWorld();
      yll -= 1.4*legendSize;
      wxChar c[200];
      double contourLabelHeight =
        static_cast<GRA_sizeCharacteristic*>(general->Get(wxT("CONTOURLABELHEIGHT")))->GetAsWorld();
      if( contourLabelHeight > 0.0 )
        ::wxSnprintf( c, 200, wxT("%2i = %9.2g"), ii, level );
      else if( contourLabelHeight == 0.0 )
        ::wxSnprintf( c, 200, wxT("%9.2g"), level );
      double angle = 0.0;
      if( yll > 0.0 )
      {
        GRA_font *font = static_cast<GRA_fontCharacteristic*>(general->Get(wxT("FONT")))->Get();
        GRA_color *color =
         colors.empty() ? GRA_colorControl::GetColor(wxT("BLACK")) :
                          GRA_colorControl::GetColor(colors[i%colors.size()]);
        GRA_drawableText *dt = new GRA_drawableText(c,legendSize,angle,xll0,yll,1,font,color);
        try
        {
          dt->Parse();
        }
        catch ( EGraphicsError const &e )
        {
          throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
        }
        //dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
        gw->AddDrawableObject( dt );
      }
    }
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
        colors.empty() ? GRA_colorControl::GetColor(wxT("BLACK")) :
        GRA_colorControl::GetColor(colors[i%colors.size()]);
    int lineType =
      linetypes.empty() ? 1 : linetypes[i%linetypes.size()];
    int lineWidth =
      linewidths.empty() ? 1 : linewidths[i%linewidths.size()];
    //
    std::vector<double> x, y;
    UsefulFunctions::ContourCalculation( level, xVector, yVector, zMatrix, x, y );
    GRA_contourLine *contour = new GRA_contourLine( label, x, y, color, lineType, lineWidth );
    x.clear();
    y.clear();
    contour->Make();
    //contour->Draw( ExGlobals::GetGraphicsOutput(), dc );
    gw->AddDrawableObject( contour );
  }
  // save the last legend value for the next plot
  //
  if( qualifiers[wxT("CONTINUE")] )labelSave = static_cast<int>(label);
  if( qualifiers[wxT("LEGEND")] )
  {
    if( qualifiers[wxT("RESET")] )
    {
      static_cast<GRA_distanceCharacteristic*>(xAxis->Get(wxT("UPPERAXIS")))->SetAsPercent( xuaxisp );
      static_cast<GRA_distanceCharacteristic*>(yAxis->Get(wxT("UPPERAXIS")))->SetAsPercent( yuaxisp );
    }
  }
  ExGlobals::RefreshGraphics();
}

  void CMD_contour::GetIntParameter( std::vector<int> &values, ParseLine const *p,
                                     int &ifld,wxString const &errorMessage )
  {
    if( p->GetNumberOfTokens()-1 < ifld )
    throw ECommandError( wxString(wxT("CONTOUR: "))+errorMessage );
  if( p->IsNumeric(ifld) )
  {
    values.push_back( static_cast<int>(p->GetNumeric(ifld)) );
    AddToStackLine( wxString() << p->GetNumeric(ifld++) );
  }
  else if( p->IsString(ifld) )
  {
    int ndm=0, dimSizes[]={0,0,0};
    double d=0.0;
    std::vector<double> doubles;
    try
    {
      NumericVariable::GetVariable( p->GetString(ifld), ndm, d, doubles, dimSizes );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( wxString(wxT("CONTOUR: "))+wxString(e.what(),wxConvUTF8) );
    }
    if( ndm == 0 )
    {
      values.push_back( static_cast<int>(d) );
    }
    else if( ndm == 1 )
    {
      std::size_t size = doubles.size();
      for( std::size_t i=0; i<size; ++i )
        values.push_back( static_cast<int>(doubles[i]) );
    }
    else
    {
      throw ECommandError( wxString(wxT("CONTOUR: "))+errorMessage );
    }
    AddToStackLine( p->GetString(ifld++) );
  }
  else
  {
    throw ECommandError( wxString(wxT("CONTOUR: "))+errorMessage );
  }
}

void CMD_contour::GetDataParameters( std::vector<double> &x, std::vector<double> &y,
                                     std::vector<double> &z, ParseLine const *p,
                                     int &ifld )
{
  wxString command( wxT("CONTOUR: ") );
  if( p->GetNumberOfTokens()-1 < ifld || !p->IsString(ifld) )
    throw ECommandError( command+wxT("expecting matrix or independent variable") );
  int ndm=0, dimSizes[]={0,0,0};
  double d=0.0;
  std::vector<double> doubles;
  try
  {
    NumericVariable::GetVariable( p->GetString(ifld), ndm, d, doubles, dimSizes );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  if( ndm == 0 )
    throw ECommandError( command+p->GetString(ifld)+wxT(" is a scalar") );
  if( ndm == 2 )  // contour m ...
  {
    z.assign( doubles.begin(), doubles.end() );
    AddToStackLine( p->GetString(ifld++) );
    for( int i=0; i<dimSizes[1]; ++i )x.push_back( static_cast<double>(i+1) );
    for( int i=0; i<dimSizes[0]; ++i )y.push_back( static_cast<double>(i+1) );
  }
  else if( ndm == 1 )  // contour x y ...
  {
    AddToStackLine( p->GetString(ifld++) );
    if( p->GetNumberOfTokens()-1 < ifld || !p->IsString(ifld) )
      throw ECommandError( command+wxT("expecting dependent variable") );
    x.assign( doubles.begin(), doubles.end() );
    try
    {
      NumericVariable::GetVector( p->GetString(ifld), wxT("dependent variable"), y );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(ifld++) );
    if( p->GetNumberOfTokens()-1 < ifld || !p->IsString(ifld) )
      throw ECommandError( command+wxT("expecting matrix or z-vector") );
    try
    {
      NumericVariable::GetVariable( p->GetString(ifld), ndm, d, z, dimSizes );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( ndm == 0 )
      throw ECommandError( command+p->GetString(ifld)+wxT(" is a scalar") );
    else if( ndm == 1 )  // contour x y z ...
    {
      throw ECommandError( command+wxT("contouring of scattered data is not available") );
    }
    else if( ndm == 2 )  // contour x y matrix ...
    {
      if( static_cast<int>(x.size()) < dimSizes[1] )
        throw ECommandError( command+wxT("independent vector length < number of columns of matrix") );
      if( static_cast<int>(x.size()) > dimSizes[1] )
        throw ECommandError( command+wxT("independent vector length > number of columns of matrix") );
      if( static_cast<int>(y.size()) < dimSizes[0] )
        throw ECommandError( command+wxT("dependent vector length < number of rows of matrix") );
      if( static_cast<int>(y.size()) > dimSizes[0] )
        throw ECommandError( command+wxT("dependent vector length > number of rows of matrix") );
      AddToStackLine( p->GetString(ifld++) );
    }
  }
}

// end of file
