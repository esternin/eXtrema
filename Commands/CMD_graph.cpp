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
#include "ProjectHeader.h"
#pragma hdrstop

#include "CMD_graph.h"
#include "ExGlobals.h"
#include "ExString.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "GRA_window.h"
#include "GRA_legend.h"
#include "GRA_plotSymbol.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "EGraphicsError.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_characteristic.h"
#include "GRA_cartesianAxes.h"
#include "GRA_cartesianCurve.h"
#include "UsefulFunctions.h"
#include "GRA_polarAxes.h"
#include "GRA_polarCurve.h"

CMD_graph CMD_graph::cmd_graph_;

CMD_graph::CMD_graph() : Command( "GRAPH" )
{
  AddQualifier( "POLAR",     false );
  AddQualifier( "RADAR",     false );
  AddQualifier( "HISTOGRAM", false );
  AddQualifier( "REPLOT",    true );
  AddQualifier( "AXESONLY",  false );
  AddQualifier( "OVERLAY",   false );
  AddQualifier( "YONRIGHT",  false );
  AddQualifier( "XONTOP",    false );
  AddQualifier( "SMOOTH",    false );
}

void CMD_graph::Execute( ParseLine const *p )
{
  // GRAPH X Y                           Draw axes & curve autoscaling to X Y
  // GRAPH\HISTOGRAM X Y                 Draw axes & histgram of X Y
  // GRAPH X MATRIX                      Draw axes &   "     " to X matrix
  // GRAPH X Y YE1 XE1 YE2 XE2             "    "  &   "     " to X Y
  //                                                  and error bars
  // GRAPH\AXESONLY                      Draw axes as previously set up
  // GRAPH\AXESONLY X Y                    "    "  autoscaling to X and Y
  // GRAPH\OVERLAY X Y                   Draw curve of X Y
  // GRAPH\OVERLAY X Y YE1 XE1 YE2 XE2     "    "   "  " " and error bars
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
  ExString command( "GRAPH" );
  //
  ParseToken t( *p->GetToken(0) );
  //
  double const degToRad = M_PI/180.;
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *dataC = gw->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *legendC = gw->GetGraphLegendCharacteristics();
  GRA_intCharacteristic *histType =
    static_cast<GRA_intCharacteristic*>(dataC->Get("HISTOGRAMTYPE"));
  int hstyp = histType->Get();
  if( qualifiers["HISTOGRAM"] )histType->Set(2);
  //
  std::vector<double> xe1, ye1, xe2, ye2;
  std::vector<double> x, y;
  bool xE1=false, yE1=false, xE2=false, yE2=false;
  std::size_t num = 0;
  //
  int ifld = 0;
  bool legendIsOn = static_cast<GRA_boolCharacteristic*>(legendC->Get("ON"))->Get();
  if( qualifiers["POLAR"] || qualifiers["RADAR"] )legendIsOn = false;
  if( legendIsOn && !qualifiers["AXESONLY"] && (p->GetNumberOfTokens()<2 || !p->IsString(1)) )
    throw ECommandError( "GRAPH", "expecting legend label" );
  //
  ExString legendLabel;
  if( p->GetNumberOfTokens()>1 && p->IsString(1) )
  {
    ++ifld; // there must be a legend entry, unless \AXESONLY
    try
    {
      TextVariable::GetVariable( p->GetString(1), false, legendLabel );
      AddToStackLine( p->GetString(1) );
    }
    catch( EVariableError const &e )
    {
      if( qualifiers["AXESONLY"] || !legendIsOn )--ifld;
      else throw ECommandError( "GRAPH", "expecting legend label" );
    }
  }
  if( p->GetNumberOfTokens() == ++ifld )
  {
    num = 1;
    if( qualifiers["AXESONLY"] )
    {
      x.push_back( 0.0 );
      y.push_back( 0.0 );
      if( qualifiers["POLAR"] || qualifiers["RADAR"] )
      {
        num = 2;
        GRA_setOfCharacteristics *polarC = gw->GetPolarCharacteristics();
        x.push_back( static_cast<GRA_doubleCharacteristic*>(polarC->Get("MAX"))->Get() );
        y.push_back( 0.0 );
      }
    }
    else throw ECommandError( "GRAPH", "expecting something to plot" );
  }
  else
  {
    ExString xName, yName;
    if( p->GetNumberOfTokens() == ifld+1 )  // just a y vector entered
    {
      if( p->IsNull(ifld) )throw ECommandError( "GRAPH", "dependent variable not entered" );
      if( p->IsEqualSign(ifld) )
        throw ECommandError( "GRAPH", "equal sign encountered; expecting dependent variable" );
      if( p->IsNumeric(ifld) )
      {
        y.push_back( p->GetNumeric(ifld) );
        x.push_back( 1.0 );
        yName = ExString(p->GetNumeric(ifld));
        AddToStackLine( yName );
      }
      else
      {
        try
        {
          NumericVariable::GetVector( p->GetString(ifld), "dependent variable", y );
        }
        catch( EVariableError &e )
        {
          throw ECommandError( "GRAPH", e.what() );
        }
        std::size_t size = y.size();
        for( std::size_t i=0; i<size; ++i )x.push_back( static_cast<double>(i+1) );
        yName = p->GetString(ifld);
        AddToStackLine( yName );
      }
    }
    else // at least x and y were entered
    {
      if( p->IsNull(ifld) )throw ECommandError( "GRAPH", "independent variable not entered" );
      if( p->IsEqualSign(ifld) )
        throw ECommandError( "GRAPH", "equal sign encountered; expecting independent variable" );
      if( p->IsNumeric(ifld) )
      {
        x.push_back( p->GetNumeric(ifld) );
        xName = ExString(p->GetNumeric(ifld));
      }
      else
      {
        try
        {
          NumericVariable::GetVector( p->GetString(ifld), "independent variable", x );
        }
        catch( EVariableError &e )
        {
          throw ECommandError( "GRAPH", e.what() );
        }
        xName = p->GetString(ifld);
      }
      AddToStackLine( xName );
      ++ifld;
      if( p->IsNull(ifld) )throw ECommandError( "GRAPH", "dependent variable not entered" );
      if( p->IsEqualSign(ifld) )
        throw ECommandError( "GRAPH", "equal sign encountered; expecting dependent variable" );
      if( p->IsNumeric(ifld) )
      {
        y.push_back( p->GetNumeric(ifld) );
        yName = ExString(p->GetNumeric(ifld));
      }
      else
      {
        try
        {
          NumericVariable::GetVector( p->GetString(ifld), "dependent variable", y );
        }
        catch( EVariableError &e )
        {
          throw ECommandError( "GRAPH", e.what() );
        }
        yName = p->GetString(ifld);
      }
      AddToStackLine( yName );
      if( x.size() != y.size() )
        ExGlobals::WarningMessage( "GRAPH: input vectors have different lengths, using minimum" );
    }
    num = std::min(x.size(),y.size());
    ExString ye1Name, ye2Name, xe1Name, xe2Name;
    ++ifld;
    if( p->GetNumberOfTokens() >= ifld+1 ) // there is a y error vector
    {
      if( p->IsEqualSign(ifld) )
        throw ECommandError( "GRAPH", "equal sign encountered; expecting a y error vector" );
      if( p->IsNull(ifld) )
      {
        ye1Name.assign( ",," );
      }
      else if( p->IsNumeric(ifld) )
      {
        yE1 = true;
        ye1.push_back( p->GetNumeric(ifld) );
        ye1Name = ExString(p->GetNumeric(ifld));
      }
      else if( p->IsString(ifld) )
      {
        yE1 = true;
        try
        {
          NumericVariable::GetVector( p->GetString(ifld), "y error vector", ye1 );
        }
        catch( EVariableError &e )
        {
          throw ECommandError( "GRAPH", e.what() );
        }
        ye1Name = p->GetString(ifld);
      }
    }
    if( !ye1Name.empty() )AddToStackLine( ye1Name );
    ++ifld;
    if( p->GetNumberOfTokens() >= ifld+1 ) // there is an x error vector
    {
      if( p->IsEqualSign(ifld) )
        throw ECommandError( "GRAPH", "equal sign encountered; expecting an x error vector" );
      if( p->IsNull(ifld) )
      {
        xe1Name.assign( ",," );
      }
      else if( p->IsNumeric(ifld) )
      {
        xE1 = true;
        xe1.push_back( p->GetNumeric(ifld) );
        xe1Name = ExString(p->GetNumeric(ifld));
      }
      else if( p->IsString(ifld) )
      {
        xE1 = true;
        try
        {
          NumericVariable::GetVector( p->GetString(ifld), "x error vector", xe1 );
        }
        catch( EVariableError &e )
        {
          throw ECommandError( "GRAPH", e.what() );
        }
        xe1Name = p->GetString(ifld);
      }
    }
    if( !xe1Name.empty() )AddToStackLine( xe1Name );
    ++ifld;
    if( p->GetNumberOfTokens() >= ifld+1 ) // there is a second y error vector
    {
      if( p->IsEqualSign(ifld) )
        throw ECommandError( "GRAPH", "equal sign encountered; expecting a second y error vector" );
      if( p->IsNull(ifld) )
      {
        ye2Name.assign( ",," );
      }
      else if( p->IsNumeric(ifld) )
      {
        yE2 = true;
        ye2.push_back( p->GetNumeric(ifld) );
        ye2Name = ExString(p->GetNumeric(ifld));
      }
      else if( p->IsString(ifld) )
      {
        yE2 = true;
        try
        {
          NumericVariable::GetVector( p->GetString(ifld), "second y error vector", ye2 );
        }
        catch( EVariableError &e )
        {
          throw ECommandError( "GRAPH", e.what() );
        }
        ye2Name = p->GetString(ifld);
      }
    }
    if( !ye2Name.empty() )AddToStackLine( ye2Name );
    ++ifld;
    if( p->GetNumberOfTokens() >= ifld+1 ) // there is a second x error vector
    {
      if( p->IsEqualSign(ifld) )
        throw ECommandError( "GRAPH", "equal sign encountered; expecting second x error vector" );
      if( p->IsNumeric(ifld) )
      {
        xE2 = true;
        xe2.push_back( p->GetNumeric(ifld) );
        xe2Name = ExString(p->GetNumeric(ifld));
      }
      else if( p->IsString(ifld) )
      {
        xE2 = true;
        try
        {
          NumericVariable::GetVector( p->GetString(ifld), "second x error vector", xe2 );
        }
        catch( EVariableError &e )
        {
          throw ECommandError( "GRAPH", e.what() );
        }
        xe2Name = p->GetString(ifld);
      }
    }
    if( !xe2Name.empty() )AddToStackLine( xe2Name );
  }
  if( yE1 )num = std::min(num,ye1.size());
  if( xE1 )num = std::min(num,xe1.size());
  if( yE2 )num = std::min(num,ye2.size());
  if( xE2 )num = std::min(num,xe2.size());
  if( num < x.size() )x.resize( num );
  if( num < y.size() )y.resize( num );
  if( xE1 && num < xe1.size() )xe1.resize( num );
  if( yE1 && num < ye1.size() )ye1.resize( num );
  if( xE2 && num < xe2.size() )xe2.resize( num );
  if( yE2 && num < ye2.size() )ye2.resize( num );

  std::vector<double> x2(x), y2(y);
  if( yE1 )
  {
    if( yE2 )
    {
      for( std::size_t i=0; i<num; ++i )
      {
        x2.push_back(x[i]);
        y2.push_back(y[i]+ye2[i]);
        x2.push_back(x[i]);
        y2.push_back(y[i]-ye1[i]);
      }
    }
    else
    {
      for( std::size_t i=0; i<num; ++i )
      {
        x2.push_back(x[i]);
        y2.push_back(y[i]+ye1[i]);
        x2.push_back(x[i]);
        y2.push_back(y[i]-ye1[i]);
      }
    }
  }
  if( xE1 )
  {
    if( xE2 )
    {
      for( std::size_t i=0; i<num; ++i )
      {
        x2.push_back(x[i]+xe2[i]);
        y2.push_back(y[i]);
        x2.push_back(x[i]-xe1[i]);
        y2.push_back(y[i]);
      }
    }
    else
    {
      for( std::size_t i=0; i<num; ++i )
      {
        x2.push_back(x[i]+xe1[i]);
        y2.push_back(y[i]);
        x2.push_back(x[i]-xe1[i]);
        y2.push_back(y[i]);
      }
    }
  }
  if( qualifiers["POLAR"] || qualifiers["RADAR"] )
  {
    GRA_polarAxes *polarAxes = 0;
    GRA_polarCurve *polarCurve = 0;
    try
    {
      if( !qualifiers["OVERLAY"] )
      {
        polarAxes = new GRA_polarAxes(x2,y2,qualifiers["RADAR"]);
        polarAxes->Make();
        polarAxes->Draw( ExGlobals::GetScreenOutput() );
        gw->AddDrawableObject( polarAxes );
      }
      if( !qualifiers["AXESONLY"] )
      {
        polarCurve = new GRA_polarCurve(x,y);
        polarCurve->Make();
        polarCurve->Draw( ExGlobals::GetScreenOutput() );
        gw->AddDrawableObject( polarCurve );
      }
    }
    catch ( EGraphicsError const &e )
    {
      delete polarAxes;
      delete polarCurve;
      throw ECommandError( "GRAPH", e.what() );
    }
  }
  else
  {
    GRA_cartesianAxes *cartesianAxes = 0;
    GRA_cartesianCurve *cartesianCurve = 0;
    try
    {
      if( !qualifiers["OVERLAY"] )
      {
        cartesianAxes = new GRA_cartesianAxes(x,y,qualifiers["XONTOP"],qualifiers["YONRIGHT"]);
        cartesianAxes->Make();
        cartesianAxes->Draw( ExGlobals::GetScreenOutput() );
        gw->AddDrawableObject( cartesianAxes );
      }
      if( !qualifiers["AXESONLY"] )
      {
        cartesianCurve = new GRA_cartesianCurve(x,y,xe1,ye1,xe2,ye2,qualifiers["SMOOTH"]);
        cartesianCurve->Make();
        cartesianCurve->Draw( ExGlobals::GetScreenOutput() );
        gw->AddDrawableObject( cartesianCurve );
      }
    }
    catch ( EGraphicsError const &e )
    {
      delete cartesianAxes;
      delete cartesianCurve;
      throw ECommandError( "GRAPH", e.what() );
    }
  }
  if( legendIsOn && !qualifiers["AXESONLY"] )
  {
    try
    {
      gw->GetGraphLegend()->AddEntry( legendLabel );
    }
    catch (EGraphicsError const &e)
    {
      throw ECommandError( "GRAPH", e.what() );
    }
  }
  histType->Set( hstyp );
}

// end of file
