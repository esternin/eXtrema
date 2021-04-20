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

#include "CMD_contour.h"
#include "GRA_contourLine.h"
#include "GRA_outputType.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "GRA_window.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "EGraphicsError.h"
#include "UsefulFunctions.h"
#include "GRA_drawableText.h"
#include "NumericVariable.h"
#include "GRA_cartesianAxes.h"
#include "GRA_rectangle.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_setOfCharacteristics.h"

CMD_contour CMD_contour::cmd_contour_;

CMD_contour::CMD_contour() : Command( "CONTOUR" )
{
  AddQualifier( "PARTIAL",    false );
  AddQualifier( "POLAR",      false );
  AddQualifier( "SPECIFIC",   false );
  AddQualifier( "LEGEND",     false );
  AddQualifier( "RESET",      true  );
  AddQualifier( "AXES",       true  );
  AddQualifier( "BORDER",     true  );
  AddQualifier( "COLORS",     false );
  AddQualifier( "COLOURS",    false );
  AddQualifier( "LINETYPES",  false );
  AddQualifier( "LINEWIDTHS", false );
  AddQualifier( "VOLUMES",    false );
  AddQualifier( "AREAS",      false );
  AddQualifier( "CONTINUE",   false );
}

void CMD_contour::Execute( ParseLine const *p )
{
  // CONTOUR { x y } v  numcntrs { minlevel { incr }}
  // CONTOUR\COLORS colr x y z  numcntrs { minlevel { incr }}
  // CONTOUR\LINETYPES ltype x y z  numcntrs { minlevel { incr }}
  // CONTOUR\COLORS\LINETYPES colr ltype x y z  numcntrs { minlevel { incr }}
  // CONTOUR\LINEWIDTHS lwidth x y z  numcntrs { minlevel { incr }}
  // CONTOUR\COLORS\LINEWIDTHS colr lwidth x y z  numcntrs { minlevel { incr }}
  // CONTOUR\LINETYPES\LINEWIDTHS ltype lwidth x y z  numcntrs { minlevel { incr }}
  // CONTOUR\COLORS\LINETYPES\LINEWIDTHS colr ltype lwidth x y z  numcntrs { minlevel { incr }}
  // CONTOUR\SPECIFIC { x y } v  cntrs
  // CONTOUR\SPECIFIC\COLORS colr x y z  cntrs
  // CONTOUR\SPECIFIC\LINETYPES ltype x y z  cntrs
  // CONTOUR\SPECIFIC\LINEWIDTHS lwidth x y z  cntrs
  // CONTOUR\CONTINUE { x y } v numcntrs { minlevel { incr }}
  // CONTOUR\CONTINUE\COLORS colr x y z numcntrs { minlevel { incr }}
  // CONTOUR\CONTINUE\SPECIFIC { x y } v cntrs
  // CONTOUR\CONTINUE\SPECIFIC\COLORS  colr x y z cntrs
  //
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch( ECommandError const &e )
  {
    throw;
  }
  GRA_window *gw = ExGlobals::GetGraphWindow();
  //
  if( qualifiers["LEGEND"] )
  {
    if( static_cast<GRA_sizeCharacteristic*>(gw->GetGeneralCharacteristics()->Get("LEGENDSIZE"))->
        GetAsWorld() <= 0.0 )
      throw ECommandError( "CONTOUR", "legend is on, but legend size <= 0");
    if( !qualifiers["AXES"] )
      throw ECommandError( "CONTOUR", "legend is on, but axes are off" );
  }
  if( qualifiers["CONTINUE"] )
  {
    qualifiers["AXES"] = false;
    qualifiers["BORDER"] = false;
  }
  int ifld = 1, intrpx = 0, intrpy = 0, ndm = 0;
  int dimSizes[] = { 0, 0, 0 };
  double d = 0.0;
  std::vector<double> doubles;
  std::vector<int> colorVector, linetypeVector, linewidthVector;
  if( qualifiers["COLORS"] || qualifiers["COLOURS"] )  // color vector
  {
    if( p->GetNumberOfTokens()-1 < ifld )
      throw ECommandError( "CONTOUR", "expecting color (scalar or vector)" );
    if( p->IsNumeric(ifld) )
    {
      colorVector.push_back( static_cast<int>(p->GetNumeric(ifld)) );
      AddToStackLine( ExString(p->GetNumeric(ifld++)) );
    }
    else if( p->IsString(ifld) )
    {
      try
      {
        NumericVariable::GetVariable( p->GetString(ifld), ndm, d, doubles, dimSizes );
      }
      catch( EVariableError const &e )
      {
        throw ECommandError( "CONTOUR", e.what() );
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
        throw ECommandError( "CONTOUR", "color must be a scalar or a vector" );
      }
      AddToStackLine( p->GetString(ifld++) );
    }
    else
    {
      throw ECommandError( "CONTOUR", "expecting color (scalar or vector)" );
    }
  }
  if( qualifiers["LINETYPES"] )  // linetype vector
  {
    if( p->GetNumberOfTokens()-1 < ifld )
      throw ECommandError( "CONTOUR", "expecting linetype (scalar or vector)" );
    if( p->IsNumeric(ifld) )
    {
      linetypeVector.push_back( static_cast<int>(p->GetNumeric(ifld)) );
      AddToStackLine( ExString(p->GetNumeric(ifld++)) );
    }
    else if( p->IsString(ifld) )
    {
      try
      {
        NumericVariable::GetVariable( p->GetString(ifld), ndm, d, doubles, dimSizes );
      }
      catch( EVariableError const &e )
      {
        throw ECommandError( "CONTOUR", e.what() );
      }
      if( ndm == 0 )
      {
        linetypeVector.push_back( static_cast<int>(d) );
      }
      else if( ndm == 1 )
      {
        std::size_t size = doubles.size();
        for( std::size_t i=0; i<size; ++i )linetypeVector.push_back( static_cast<int>(doubles[i]) );
      }
      else
      {
        throw ECommandError( "CONTOUR", "linetype must be a scalar or a vector" );
      }
      AddToStackLine( p->GetString(ifld++) );
    }
    else
    {
      throw ECommandError( "CONTOUR", "expecting linetype (scalar or vector)" );
    }
  }
  if( qualifiers["LINEWIDTHS"] )  // linewidth vector
  {
    if( p->GetNumberOfTokens()-1 < ifld )
      throw ECommandError( "CONTOUR", "expecting linewidth (scalar or vector)" );
    if( p->IsNumeric(ifld) )
    {
      linewidthVector.push_back( static_cast<int>(p->GetNumeric(ifld)) );
      AddToStackLine( ExString(p->GetNumeric(ifld++)) );
    }
    else if( p->IsString(ifld) )
    {
      try
      {
        NumericVariable::GetVariable( p->GetString(ifld), ndm, d, doubles, dimSizes );
      }
      catch( EVariableError const &e )
      {
        throw ECommandError( "CONTOUR", e.what() );
      }
      if( ndm == 0 )
      {
        linewidthVector.push_back( static_cast<int>(d) );
      }
      else if( ndm == 1 )
      {
        std::size_t size = doubles.size();
        for( std::size_t i=0; i<size; ++i )linewidthVector.push_back( static_cast<int>(doubles[i]) );
      }
      else
      {
        throw ECommandError( "CONTOUR", "linewidth must be a scalar or a vector" );
      }
      AddToStackLine( p->GetString(ifld++) );
    }
    else
    {
      throw ECommandError( "CONTOUR", "expecting linewidth (scalar or vector)" );
    }
  }
  std::vector<double> xVector, yVector, zVector;
  if( p->GetNumberOfTokens()-1 < ifld || !p->IsString(ifld) )
    throw ECommandError( "CONTOUR", "expecting matrix or independent variable" );
  try
  {
    NumericVariable::GetVariable( p->GetString(ifld), ndm, d, doubles, dimSizes );
  }
  catch( EVariableError const &e )
  {
    throw ECommandError( "CONTOUR", e.what() );
  }
  if( ndm == 0 )
    throw ECommandError( "CONTOUR", p->GetString(ifld)+" is a scalar" );
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
      throw ECommandError( "CONTOUR", "expecting dependent variable" );
    xVector.assign( doubles.begin(), doubles.end() );
    try
    {
      NumericVariable::GetVector( p->GetString(ifld), "dependent variable", yVector );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( "CONTOUR", e.what() );
    }
    AddToStackLine( p->GetString(ifld++) );
    if( p->GetNumberOfTokens()-1 < ifld || !p->IsString(ifld) )
      throw ECommandError( "CONTOUR", "expecting matrix or z-vector" );
    try
    {
      NumericVariable::GetVariable( p->GetString(ifld), ndm, d, zVector, dimSizes );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( "CONTOUR", e.what() );
    }
    if( ndm == 0 )
      throw ECommandError( "CONTOUR",  p->GetString(ifld)+" is a scalar, expecting matrix" );
    else if( ndm == 1 )  // contour x y z ...
      throw ECommandError( "CONTOUR",  p->GetString(ifld)+" is a vector, expecting matrix" );
    else // contour x y matrix ...
    {
      if( static_cast<int>(xVector.size()) < dimSizes[1] )
        throw ECommandError( "CONTOUR", "independent vector length < number of columns of matrix" );
      if( static_cast<int>(xVector.size()) > dimSizes[1] )
        throw ECommandError( "CONTOUR", "independent vector length > number of columns of matrix" );
      if( static_cast<int>(yVector.size()) < dimSizes[0] )
        throw ECommandError( "CONTOUR", "dependent vector length < number of rows of matrix" );
      if( static_cast<int>(yVector.size()) > dimSizes[0] )
        throw ECommandError( "CONTOUR", "dependent vector length > number of rows of matrix" );
      AddToStackLine( p->GetString(ifld++) );
    }
  }
  double xmin, xmax, ymin, ymax, zmin, zmax;
  if( !qualifiers["CONTINUE"] )
  {
    if( qualifiers["POLAR"] )
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
      throw ECommandError( "CONTOUR", "minimum = maximum for independent variable" );
    if( ymax == ymin )
      throw ECommandError( "CONTOUR", "minimum = maximum for dependent variable" );
  }
  double xuaxisp=0.0, yuaxisp=0.0, xuaxis=0.0, yuaxis=0.0;
  double xll, yll, xll0;
  GRA_setOfCharacteristics *xAxis = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxis = gw->GetYAxisCharacteristics();
  if( !qualifiers["CONTINUE"] )
  {
    if( qualifiers["AXES"] )
    {
      if( qualifiers["LEGEND"] )
      {
        GRA_distanceCharacteristic *xupperaxis =
          static_cast<GRA_distanceCharacteristic*>(xAxis->Get("UPPERAXIS"));
        GRA_distanceCharacteristic *yupperaxis =
          static_cast<GRA_distanceCharacteristic*>(yAxis->Get("UPPERAXIS"));
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
        throw ECommandError( "CONTOUR", e.what() );
      }
      cartesianAxes->Draw( ExGlobals::GetScreenOutput() );
      gw->AddDrawableObject( cartesianAxes );
      if( qualifiers["BORDER"] )
      {
        double xlo, ylo, xhi, yhi;
        gw->GraphToWorld( xmin, ymin, xlo, ylo, true );
        gw->GraphToWorld( xmax, ymax, xhi, yhi );
        GRA_rectangle *border = new GRA_rectangle(xlo,ylo,xhi,yhi,0.0,false,
                                                  GRA_colorControl::GetColor("BLACK"),0,1);

        ExGlobals::GetScreenOutput()->Draw( border );
        gw->AddDrawableObject( border );
      }
      if( qualifiers["LEGEND"] )
      {
        GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
        GRA_fontCharacteristic *generalFont =
          static_cast<GRA_fontCharacteristic*>(general->Get("FONT"));
        GRA_sizeCharacteristic *xLargeTicLength =
          static_cast<GRA_sizeCharacteristic*>(xAxis->Get("LARGETICLENGTH"));
        GRA_sizeCharacteristic *legendSize =
          static_cast<GRA_sizeCharacteristic*>(general->Get("LEGENDSIZE"));
        GRA_sizeCharacteristic *contourLabelHeight =
          static_cast<GRA_sizeCharacteristic*>(general->Get("CONTOURLABELHEIGHT"));
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
          GRA_drawableText *tb = new GRA_drawableText("label",ht,angle,xll0,yll,
                                                      1,font,GRA_colorControl::GetColor("BLACK"));
          try
          {
            tb->Parse();
          }
          catch ( EGraphicsError const &e )
          {
            delete tb;
            throw ECommandError( "CONTOUR", e.what() );
          }
          tb->Draw( ExGlobals::GetScreenOutput() );
          gw->AddDrawableObject( tb );
        }
        else
        {
          sizleg = 1.2 * sizleg;
          xll0 = xll + sizleg;
          xll1 = xll0 + 3 * sizleg;
          xll2 = xll1 + 2 * sizleg;
        }
        GRA_drawableText *tb = new GRA_drawableText("contour",ht,angle,xll1,yll,
                                                    1,font,GRA_colorControl::GetColor("BLACK"));
        try
        {
          tb->Parse();
        }
        catch ( EGraphicsError const &e )
        {
          delete tb;
          throw ECommandError( "CONTOUR", e.what() );
        }
        tb->Draw( ExGlobals::GetScreenOutput() );
        gw->AddDrawableObject( tb );
        tb = new GRA_drawableText("value",ht,angle,xll2,yll,
                                  1,font,GRA_colorControl::GetColor("BLACK"));
        try
        {
          tb->Parse();
        }
        catch ( EGraphicsError const &e )
        {
          delete tb;
          throw ECommandError( "CONTOUR", e.what() );
        }
        tb->Draw( ExGlobals::GetScreenOutput() );
        gw->AddDrawableObject( tb );
      }
    }
  }
  int i1 = 0;
  int i2 = xVector.size()-1;
  int j1 = 0;
  int j2 = yVector.size()-1;
  if( qualifiers["PARTIAL"] )
  {
    GRA_doubleCharacteristic *xminC =
      static_cast<GRA_doubleCharacteristic*>(xAxis->Get("MIN"));
    GRA_doubleCharacteristic *yminC =
      static_cast<GRA_doubleCharacteristic*>(yAxis->Get("MIN"));
    GRA_doubleCharacteristic *xmaxC =
      static_cast<GRA_doubleCharacteristic*>(xAxis->Get("MAX"));
    GRA_doubleCharacteristic *ymaxC =
      static_cast<GRA_doubleCharacteristic*>(yAxis->Get("MAX"));
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
  if( qualifiers["SPECIFIC"] ) // a scalar or vector of contour levels is expected
  {
    if( p->GetNumberOfTokens()-1 < ifld )
    {
      throw ECommandError( "CONTOUR", "expecting contour levels (scalar or vector)" );
    }
    if( p->IsNumeric(ifld) )
    {
      contourVector.push_back( p->GetNumeric(ifld) );
      AddToStackLine( ExString(p->GetNumeric(ifld++)) );
    }
    else if( p->IsString(ifld) )
    {
      try
      {
        NumericVariable::GetVariable( p->GetString(ifld), ndm, d, contourVector, dimSizes );
      }
      catch( EVariableError const &e )
      {
        throw ECommandError( "CONTOUR", e.what() );
      }
      if( ndm == 0 )contourVector.push_back( d );
      else if( ndm != 1 )
        throw ECommandError( "CONTOUR", "expecting contour levels (scalar or vector)" );
      AddToStackLine( p->GetString(ifld++) );
    }
    else
      throw ECommandError( "CONTOUR", "expecting contour level(s)" );
    numberOfContours = contourVector.size();
  }
  else // a number of contours is entered instead of a list of contour levels
  {
    // contour { x y } z numcntrs { minlevel { incr }}
    //
    if( p->GetNumberOfTokens() <= ifld )
      throw ECommandError( "CONTOUR", "expecting the number of contours" );
    double dNumC;
    try
    {
      NumericVariable::GetScalar( p->GetString(ifld), "number of contours", dNumC );
    }
    catch (EVariableError const &e)
    {
      throw ECommandError( "CONTOUR", e.what() );
    }
    numberOfContours = abs( static_cast<int>(dNumC) );
    if( numberOfContours < 2 )
      throw ECommandError( "CONTOUR", "number of contours requested < 2" );
    AddToStackLine( p->GetString(ifld++) );
    //
    bool zminsEntered = false;
    if( p->GetNumberOfTokens() > ifld )
    {
      NumericVariable::GetScalar( p->GetString(ifld), "minimum contour level", d );
      zmins = d;
      AddToStackLine( p->GetString(ifld++) );
      zminsEntered = true;
      if( p->GetNumberOfTokens() > ifld )
      {
        NumericVariable::GetScalar( p->GetString(ifld), "contour level increment", d );
        zincs = d;
        if( zincs == 0.0 )throw ECommandError( "CONTOUR", "contour level increment = 0" );
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
      catch ( runtime_error const &e )
      {
        throw ECommandError( "CONTOUR", e.what() );
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
  // get space: area, volume * contour vectors
  if( xVector.size() < 20 )intrpx = 10;
  if( xVector.size() >= 20 && xVector.size() < 50 )intrpx = 5;
  if( xVector.size() >= 50 && xVector.size() < 100 )intrpx = 3;
  if( xVector.size() >= 100 )intrpx = 2;
  //
  if( yVector.size() < 20 )intrpy = 10;
  if( yVector.size() >= 20 && yVector.size() < 50 )intrpy = 5;
  if( yVector.size() >= 50 && yVector.size() < 100 )intrpy = 5;
  if( yVector.size() >= 100 )intrpy = 2;
  nx10 = (i2-i1)*intrpx + 1;
  ny10 = (j2-j1)*intrpy + 1;
  if( qualifiers["AREAS"] || qualifiers["VOLUMES"] )
  {
    if( qualifiers["AREAS"] )totalArea = nx10*ny10;
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
        if( qualifiers["VOLUMES"] )totalVolume += fabs( z );
        for( int k=0; k<numberOfContours; ++k )
        {
          double hk =
            qualifiers["SPECIFIC"] ? contourVector[k] : zmins + k*zincs; // hk is the contour level
          if( hk >= 0.0 )
          {
            if( z > hk )
            {
              if(   qualifiers["AREAS"] )bins1[k] += 1.0;
              if( qualifiers["VOLUMES"] )bins2[k] += fabs(z);
            }
          }
          else
          {
            if( z < hk )
            {
              if(   qualifiers["AREAS"] )bins1[k] += 1.0;
              if( qualifiers["VOLUMES"] )bins2[k] += fabs(z);
            }
          }
        }
      }
    }
  }
  static int labelSave = 1;
  double label, level, areafrac, volmfrac;
  //
  // level is the contour level
  // label is the contour label
  //
  for( int i=0; i<numberOfContours; ++i )
  {
    level = qualifiers["SPECIFIC"] ? contourVector[i] : zmins+i*zincs;
    if( qualifiers["AREAS"] )
    {
      areafrac = 100.0*bins1[i]/totalArea;
      iva[i] = areafrac;
    }
    if( qualifiers["VOLUMES"] )
    {
      volmfrac = 100*bins2[i]/totalVolume;
      ivv[i] = volmfrac;
    }
    label = level;
    if( qualifiers["LEGEND"] )
    {
      GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
      label = static_cast<double>(i+labelSave);
      int ii = i + labelSave;
      double legendSize =
        static_cast<GRA_sizeCharacteristic*>(general->Get("LEGENDSIZE"))->GetAsWorld();
      yll -= 1.4*legendSize;
      char c[200];
      double contourLabelHeight =
        static_cast<GRA_sizeCharacteristic*>(general->Get("CONTOURLABELHEIGHT"))->GetAsWorld();
      if( contourLabelHeight > 0.0 )sprintf( c, "%2i = % 9.2e", ii, level );
      else if( contourLabelHeight == 0.0 )sprintf( c, "% 9.2e", level );
      double angle = 0.0;
      if( yll > 0.0 )
      {
        GRA_font *font = static_cast<GRA_fontCharacteristic*>(general->Get("FONT"))->Get();
        GRA_color *color =
         colorVector.empty() ? GRA_colorControl::GetColor("BLACK") :
                               GRA_colorControl::GetColor(colorVector[i%colorVector.size()]);
        GRA_drawableText *dt = new GRA_drawableText(c,legendSize,angle,xll0,yll,1,font,color);
        try
        {
          dt->Parse();
        }
        catch ( EGraphicsError const &e )
        {
          delete dt;
          throw ECommandError( "CONTOUR", e.what() );
        }
        dt->Draw( ExGlobals::GetScreenOutput() );
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
        colorVector.empty() ? GRA_colorControl::GetColor("BLACK") :
        GRA_colorControl::GetColor(colorVector[i%colorVector.size()]);
    int lineType =
      linetypeVector.empty() ? 1 : linetypeVector[i%linetypeVector.size()];
    int lineWidth =
      linewidthVector.empty() ? 1 : linewidthVector[i%linewidthVector.size()];
    //
    std::vector<double> x, y;
    UsefulFunctions::ContourCalculation( level, xVector, yVector, zMatrix, x, y );
    GRA_contourLine *contour = new GRA_contourLine( label, x, y, color, lineType, lineWidth );
    x.clear();
    y.clear();
    contour->Make();
    contour->Draw( ExGlobals::GetScreenOutput() );
    gw->AddDrawableObject( contour );
  }
  if( qualifiers["CONTINUE"] )labelSave = label;  // save the last legend value for the next plot
  if( qualifiers["LEGEND"] )
  {
    if( qualifiers["RESET"] )
    {
      static_cast<GRA_distanceCharacteristic*>(xAxis->Get("UPPERAXIS"))->SetAsPercent( xuaxisp );
      static_cast<GRA_distanceCharacteristic*>(yAxis->Get("UPPERAXIS"))->SetAsPercent( yuaxisp );
    }
  }
}
// end of file
