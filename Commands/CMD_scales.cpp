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

#include "CMD_scales.h"
#include "ExGlobals.h"
#include "ExString.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_window.h"
#include "NumericVariable.h"
#include "UsefulFunctions.h"

CMD_scales CMD_scales::cmd_scales_;

CMD_scales::CMD_scales() : Command( "SCALES" )
{
  AddQualifier( "COMMENSURATE", false );
  AddQualifier( "MESSAGES", true );
}

void CMD_scales::Execute( ParseLine const *p )
{
  // SCALES xmin xmax nlxinc ymin ymax nlyinc
  // SCALES xmin xmax ymin ymax
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
  bool output = qualifiers["MESSAGES"] || ExGlobals::NotInaScript() || ExGlobals::GetEcho();
  if( !qualifiers["MESSAGES"] )output = false;
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxis = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxis = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
  //
  GRA_doubleCharacteristic *xAxisMin =
    static_cast<GRA_doubleCharacteristic*>(xAxis->Get("MIN"));
  GRA_doubleCharacteristic *xAxisMax =
    static_cast<GRA_doubleCharacteristic*>(xAxis->Get("MAX"));
  GRA_doubleCharacteristic *xAxisVMin =
    static_cast<GRA_doubleCharacteristic*>(xAxis->Get("VIRTUALMIN"));
  GRA_doubleCharacteristic *xAxisVMax =
    static_cast<GRA_doubleCharacteristic*>(xAxis->Get("VIRTUALMAX"));
  GRA_doubleCharacteristic *yAxisMin =
    static_cast<GRA_doubleCharacteristic*>(yAxis->Get("MIN"));
  GRA_doubleCharacteristic *yAxisMax =
    static_cast<GRA_doubleCharacteristic*>(yAxis->Get("MAX"));
  GRA_doubleCharacteristic *yAxisVMin =
    static_cast<GRA_doubleCharacteristic*>(yAxis->Get("VIRTUALMIN"));
  GRA_doubleCharacteristic *yAxisVMax =
    static_cast<GRA_doubleCharacteristic*>(yAxis->Get("VIRTUALMAX"));
  GRA_intCharacteristic *xAxisNlincs =
    static_cast<GRA_intCharacteristic*>(xAxis->Get("NLINCS"));
  GRA_intCharacteristic *yAxisNlincs =
    static_cast<GRA_intCharacteristic*>(yAxis->Get("NLINCS"));
  GRA_stringCharacteristic *autoscale =
    static_cast<GRA_stringCharacteristic*>(general->Get("AUTOSCALE"));
  //
  double xmin = xAxisMin->Get();
  double xmax = xAxisMax->Get();
  int xnlinc = xAxisNlincs->Get();
  double xvmin = xAxisVMin->Get();
  double xvmax = xAxisVMax->Get();
  double ymin = yAxisMin->Get();
  double ymax = yAxisMax->Get();
  int ynlinc = yAxisNlincs->Get();
  double yvmin = yAxisVMin->Get();
  double yvmax = yAxisVMax->Get();
  //
  if( p->GetNumberOfTokens() == 1 )
  {
    if( autoscale->Get()!="OFF" && output )ExGlobals::WriteOutput("graph autoscaling is off");
    autoscale->Set("OFF");
  }
  else
  {
    // autoscale =  0  means OFF
    //           = -1  means COMMENSURATE
    //           =  1  means ON,  = 4  means ON /VIRTUAL
    //           =  2  means X,   = 5  means X /VIRTUAL
    //           =  3  means Y,   = 6  means Y /VIRTUAL
    //
    if( p->GetNumberOfTokens() == 7 )
    {
      int tcnt = 1;
      double d;
      NumericVariable::GetScalar( p->GetString(tcnt), "x-axis minimum", d );
      xmin = d;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), "x-axis maximum", d );
      xmax = d;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), "number of x-axis major tic marks", d );
      xnlinc = static_cast<int>(d);
      xvmin = xmin;
      xvmax = xmax;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), "y-axis minimum", d );
      ymin = d;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), "y-axis maximum", d );
      ymax = d;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), "number of y-axis major tic marks", d );
      ynlinc = static_cast<int>(d);
      yvmin = ymin;
      yvmax = ymax;
      AddToStackLine( p->GetString(tcnt) );
    }
    else if( p->GetNumberOfTokens() == 5 )
    {
      int tcnt = 1;
      double d;
      NumericVariable::GetScalar( p->GetString(tcnt), "x-axis minimum", d );
      xmin = d;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), "x-axis maximum", d );
      xmax = d;
      AddToStackLine( p->GetString(tcnt) );
      xnlinc = 5;
      double inc;
      try
      {
        UsefulFunctions::Scale1( xvmin, xvmax, inc, xnlinc, xmin, xmax );
      }
      catch ( runtime_error const &e )
      {
        throw ECommandError( "SCALES", e.what() );
      }
      xnlinc = static_cast<int>( (xvmax-xvmin)/inc + 0.5 );
      NumericVariable::GetScalar( p->GetString(++tcnt), "y-axis minimum", d );
      ymin = d;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), "y-axis maximum", d );
      ymax = d;
      AddToStackLine( p->GetString(tcnt) );
      ynlinc = 5;
      try
      {
        UsefulFunctions::Scale1( yvmin, yvmax, inc, ynlinc, ymin, ymax );
      }
      catch ( runtime_error const &e )
      {
        throw ECommandError( "SCALES", e.what() );
      }
      ynlinc = static_cast<int>( (yvmax-yvmin)/inc + 0.5 );
    }
    else
    {
      throw ECommandError( "SCALES",
       "expecting: xmin xmax nlxinc ymin ymax nlyinc  or  xmin xmax ymin ymax");
    }
    autoscale->Set("OFF");
    xAxisMin->Set( xmin );
    xAxisMax->Set( xmax );
    xAxisVMin->Set( xvmin );
    xAxisVMax->Set( xvmax );
    xAxisNlincs->Set( xnlinc );
    yAxisMin->Set( ymin );
    yAxisMax->Set( ymax );
    yAxisVMin->Set( yvmin );
    yAxisVMax->Set( yvmax );
    yAxisNlincs->Set( ynlinc );
  }
  if( output )
  {
    ExString c("xmin = ");
    c += ExString(xmin) + ExString(", xmax = ") + ExString(xmax) +
        ExString(", number of large x incs = ") + ExString(xnlinc);
    ExGlobals::WriteOutput( c );
    c = ExString("ymin = ") + ExString(ymin) + ExString(", ymax = ") + ExString(ymax) +
        ExString(", number of large y incs = ") + ExString(ynlinc);
    ExGlobals::WriteOutput( c );
  }
  if( xmin==0.0 && xmax==0.0 && xnlinc==0.0 )
  {
    autoscale->Set("X");
    xAxisMax->Set( 1.0 );
    qualifiers["COMMENSURATE"] = false;
    if( output )ExGlobals::WriteOutput( "x-axis autoscaling is on" );
  }
  if( ymin==0.0 && ymax==0.0 && ynlinc==0.0 )
  {
    autoscale->Set("Y");
    yAxisMax->Set( 1.0 );
    qualifiers["COMMENSURATE"] = false;
    if( output )ExGlobals::WriteOutput( "y-axis autoscaling is on" );
  }
  if( qualifiers["COMMENSURATE"] )
  {
    double xlwind = static_cast<GRA_distanceCharacteristic*>(general->Get("XLOWERWINDOW"))->GetAsWorld();
    double xuwind = static_cast<GRA_distanceCharacteristic*>(general->Get("XUPPERWINDOW"))->GetAsWorld();
    double ylwind = static_cast<GRA_distanceCharacteristic*>(general->Get("YLOWERWINDOW"))->GetAsWorld();
    double yuwind = static_cast<GRA_distanceCharacteristic*>(general->Get("YUPPERWINDOW"))->GetAsWorld();
    double delxwind = fabs(xuwind-xlwind);
    double delywind = fabs(yuwind-ylwind);
    //
    double xlaxis = 0.15*delxwind + xlwind;
    double xuaxis = 0.90*delxwind + xlwind;
    double ylaxis = 0.15*delywind + ylwind;
    double yuaxis = 0.90*delywind + ylwind;
    double xmid = 0.50*(xlwind+xuwind);
    double ymid = 0.50*(ylwind+yuwind);
    //
    double xd = xmax-xmin;
    double yd = ymax-ymin;
    //
    double delx, dely;
    if( xd >= yd )
    {
      delx = xuaxis-xlaxis;
      dely = delx * yd / xd;
      ylaxis = ymid - 0.5*dely;
      yuaxis = ymid + 0.5*dely;
    }
    else
    {
      dely = yuaxis-ylaxis;
      delx = dely * xd / yd;
      xlaxis = xmid - 0.5*delx;
      xuaxis = xmid + 0.5*delx;
    }
    if( (ylaxis-ylwind)/delywind < 0.15 )
    {
      dely = 0.15*delywind + ylwind - ylaxis;
      ylaxis = 0.15*delywind + ylwind;
      yuaxis = yuaxis - dely;
      xlaxis = xlaxis + dely;
      xuaxis = xuaxis - dely;
    }
    else if( (xlaxis-xlwind)/delxwind < 0.15 )
    {
      delx = 0.15*delxwind + xlwind - xlaxis;
      xlaxis = 0.15*delxwind + xlwind;
      xuaxis = xuaxis - delx;
      ylaxis = ylaxis + delx;
      yuaxis = yuaxis - delx;
    }
    static_cast<GRA_distanceCharacteristic*>(yAxis->Get("LOWERAXIS"))->SetAsPercent(
     (ylaxis-ylwind)/delywind*100.0 );
    static_cast<GRA_distanceCharacteristic*>(yAxis->Get("UPPERAXIS"))->SetAsPercent(
     (yuaxis-ylwind)/delywind*100.0 );
    static_cast<GRA_distanceCharacteristic*>(xAxis->Get("LOWERAXIS"))->SetAsPercent(
     (xlaxis-xlwind)/delxwind*100.0 );
    static_cast<GRA_distanceCharacteristic*>(xAxis->Get("UPPERAXIS"))->SetAsPercent(
     (xuaxis-xlwind)/delxwind*100.0 );
  }
  static_cast<GRA_intCharacteristic*>(xAxis->Get("NUMBEROFDECIMALS"))->Set( -1 );
  static_cast<GRA_intCharacteristic*>(yAxis->Get("NUMBEROFDECIMALS"))->Set( -1 );
  static_cast<GRA_intCharacteristic*>(xAxis->Get("NUMBEROFDIGITS"))->Set( 10 );
  static_cast<GRA_intCharacteristic*>(yAxis->Get("NUMBEROFDIGITS"))->Set( 10 );
}

// end of file
