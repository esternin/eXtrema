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

#include "CMD_scales.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "NumericVariable.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_window.h"
#include "UsefulFunctions.h"

CMD_scales *CMD_scales::cmd_scales_ = 0;

CMD_scales::CMD_scales() : Command( wxT("SCALES") )
{
  AddQualifier( wxT("COMMENSURATE"), false );
  AddQualifier( wxT("MESSAGES"), true );
}

void CMD_scales::Execute( ParseLine const *p )
{
  // SCALES xmin xmax nlxinc ymin ymax nlyinc
  // SCALES xmin xmax ymin ymax
  //
  QualifierMap qualifiers;
  SetUp( p, qualifiers );
  //
  bool output = qualifiers[wxT("MESSAGES")] || ExGlobals::NotInaScript() || ExGlobals::GetEcho();
  if( !qualifiers[wxT("MESSAGES")] )output = false;
  //
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *xAxis = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxis = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
  //
  GRA_doubleCharacteristic *xAxisMin =
    static_cast<GRA_doubleCharacteristic*>(xAxis->Get(wxT("MIN")));
  GRA_doubleCharacteristic *xAxisMax =
    static_cast<GRA_doubleCharacteristic*>(xAxis->Get(wxT("MAX")));
  GRA_doubleCharacteristic *xAxisVMin =
    static_cast<GRA_doubleCharacteristic*>(xAxis->Get(wxT("VIRTUALMIN")));
  GRA_doubleCharacteristic *xAxisVMax =
    static_cast<GRA_doubleCharacteristic*>(xAxis->Get(wxT("VIRTUALMAX")));
  GRA_doubleCharacteristic *yAxisMin =
    static_cast<GRA_doubleCharacteristic*>(yAxis->Get(wxT("MIN")));
  GRA_doubleCharacteristic *yAxisMax =
    static_cast<GRA_doubleCharacteristic*>(yAxis->Get(wxT("MAX")));
  GRA_doubleCharacteristic *yAxisVMin =
    static_cast<GRA_doubleCharacteristic*>(yAxis->Get(wxT("VIRTUALMIN")));
  GRA_doubleCharacteristic *yAxisVMax =
    static_cast<GRA_doubleCharacteristic*>(yAxis->Get(wxT("VIRTUALMAX")));
  GRA_intCharacteristic *xAxisNlincs =
    static_cast<GRA_intCharacteristic*>(xAxis->Get(wxT("NLINCS")));
  GRA_intCharacteristic *yAxisNlincs =
    static_cast<GRA_intCharacteristic*>(yAxis->Get(wxT("NLINCS")));
  GRA_stringCharacteristic *autoscale =
    static_cast<GRA_stringCharacteristic*>(general->Get(wxT("AUTOSCALE")));
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
    if( autoscale->Get()!=wxT("OFF") && output )ExGlobals::WriteOutput(wxT("graph autoscaling is off"));
    autoscale->Set(wxT("OFF"));
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
      NumericVariable::GetScalar( p->GetString(tcnt), wxT("x-axis minimum"), d );
      xmin = d;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), wxT("x-axis maximum"), d );
      xmax = d;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), wxT("number of x-axis major tic marks"), d );
      xnlinc = static_cast<int>(d);
      xvmin = xmin;
      xvmax = xmax;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), wxT("y-axis minimum"), d );
      ymin = d;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), wxT("y-axis maximum"), d );
      ymax = d;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), wxT("number of y-axis major tic marks"), d );
      ynlinc = static_cast<int>(d);
      yvmin = ymin;
      yvmax = ymax;
      AddToStackLine( p->GetString(tcnt) );
    }
    else if( p->GetNumberOfTokens() == 5 )
    {
      int tcnt = 1;
      double d;
      NumericVariable::GetScalar( p->GetString(tcnt), wxT("x-axis minimum"), d );
      xmin = d;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), wxT("x-axis maximum"), d );
      xmax = d;
      AddToStackLine( p->GetString(tcnt) );
      xnlinc = 5;
      double inc;
      try
      {
        UsefulFunctions::Scale1( xvmin, xvmax, inc, xnlinc, xmin, xmax );
      }
      catch( std::runtime_error const &e )
      {
        throw ECommandError( Name()+wxT(": ")+wxString(e.what(),wxConvUTF8) );
      }
      xnlinc = static_cast<int>( (xvmax-xvmin)/inc+0.5 );
      NumericVariable::GetScalar( p->GetString(++tcnt), wxT("y-axis minimum"), d );
      ymin = d;
      AddToStackLine( p->GetString(tcnt) );
      NumericVariable::GetScalar( p->GetString(++tcnt), wxT("y-axis maximum"), d );
      ymax = d;
      AddToStackLine( p->GetString(tcnt) );
      ynlinc = 5;
      try
      {
        UsefulFunctions::Scale1( yvmin, yvmax, inc, ynlinc, ymin, ymax );
      }
      catch( std::runtime_error const &e )
      {
        throw ECommandError( Name()+wxT(": ")+wxString(e.what(),wxConvUTF8) );
      }
      ynlinc = static_cast<int>( (yvmax-yvmin)/inc+0.5 );
    }
    else throw ECommandError( Name()+wxT(": expecting: xmin xmax nlxinc ymin ymax nlyinc  or  xmin xmax ymin ymax"));
    autoscale->Set(wxT("OFF"));
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
    wxString c(wxT("xmin = "));
    c << xmin << wxT(", xmax = ") << xmax << wxT(", number of large x incs = ") << xnlinc;
    ExGlobals::WriteOutput( c );
    c.clear();
    c << wxT("ymin = ") << ymin << wxT(", ymax = ") << ymax
      << wxT(", number of large y incs = ") << ynlinc;
    ExGlobals::WriteOutput( c );
  }
  if( xmin==0.0 && xmax==0.0 && xnlinc==0.0 )
  {
    autoscale->Set(wxT("X"));
    xAxisMax->Set( 1.0 );
    qualifiers[wxT("COMMENSURATE")] = false;
    if( output )ExGlobals::WriteOutput( wxT("x-axis autoscaling is on") );
  }
  if( ymin==0.0 && ymax==0.0 && ynlinc==0.0 )
  {
    autoscale->Set(wxT("Y"));
    yAxisMax->Set( 1.0 );
    qualifiers[wxT("COMMENSURATE")] = false;
    if( output )ExGlobals::WriteOutput( wxT("y-axis autoscaling is on") );
  }
  if( qualifiers[wxT("COMMENSURATE")] )
  {
    double xlwind = static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("XLOWERWINDOW")))->GetAsWorld();
    double xuwind = static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("XUPPERWINDOW")))->GetAsWorld();
    double ylwind = static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("YLOWERWINDOW")))->GetAsWorld();
    double yuwind = static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("YUPPERWINDOW")))->GetAsWorld();
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
    static_cast<GRA_distanceCharacteristic*>(yAxis->Get(wxT("LOWERAXIS")))->SetAsPercent(
     (ylaxis-ylwind)/delywind*100.0 );
    static_cast<GRA_distanceCharacteristic*>(yAxis->Get(wxT("UPPERAXIS")))->SetAsPercent(
     (yuaxis-ylwind)/delywind*100.0 );
    static_cast<GRA_distanceCharacteristic*>(xAxis->Get(wxT("LOWERAXIS")))->SetAsPercent(
     (xlaxis-xlwind)/delxwind*100.0 );
    static_cast<GRA_distanceCharacteristic*>(xAxis->Get(wxT("UPPERAXIS")))->SetAsPercent(
     (xuaxis-xlwind)/delxwind*100.0 );
  }
  static_cast<GRA_intCharacteristic*>(xAxis->Get(wxT("NUMBEROFDECIMALS")))->Set( -1 );
  static_cast<GRA_intCharacteristic*>(yAxis->Get(wxT("NUMBEROFDECIMALS")))->Set( -1 );
  static_cast<GRA_intCharacteristic*>(xAxis->Get(wxT("NUMBEROFDIGITS")))->Set( 10 );
  static_cast<GRA_intCharacteristic*>(yAxis->Get(wxT("NUMBEROFDIGITS")))->Set( 10 );
}
 
// end of file
