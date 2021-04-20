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

#include "ExGlobals.h"
#include "ExString.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "GRA_window.h"
#include "NumericVariable.h"
#include "GRA_setOfCharacteristics.h"

#include "CMD_window.h"

CMD_window CMD_window::cmd_window_;

CMD_window::CMD_window() : Command( "WINDOW" )
{
  AddQualifier( "TILE", false );
  AddQualifier( "MESSAGES", true );
  AddQualifier( "INHERIT", false );
}

void CMD_window::Execute( ParseLine const *p )
{
  // Make or select a graph window
  //
  // WINDOW { nwindow { xl xu yl yu }}
  // WINDOW\TILE nx ny { nstart }
  // WINDOW\INHERIT target inherit
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
  if( qualifiers["TILE"] )
  {
    // WINDOW\TILE nx ny { start }
    //
    if( p->GetNumberOfTokens() < 3 )throw ECommandError( "WINDOW\\TILE", "expecting 2 parameters:  nx ny" );
    double d[3];
    try
    {
      NumericVariable::GetScalar( p->GetString(1), "number of windows horizontally", d[0] );
      NumericVariable::GetScalar( p->GetString(2), "number of windows vertically", d[1] );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "WINDOW\\TILE", e.what() );
    }
    int nx = static_cast<int>( d[0] );
    int ny = static_cast<int>( d[1] );
    //
    if( nx <= 0 )throw ECommandError( "WINDOW\\TILE", "number of windows horizontally <= 0" );
    if( ny <= 0 )throw ECommandError( "WINDOW\\TILE", "number of windows vertically <= 0" );
    //
    AddToStackLine( p->GetString(1) );
    AddToStackLine( p->GetString(2) );
    //
    int nstart = 1;
    if( p->GetNumberOfTokens() == 4 )
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(3), "starting window number", d[0] );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "WINDOW\\TILE", e.what() );
      }
      nstart = static_cast<int>( d[0] );
      if( nstart <= 0 )throw ECommandError( "WINDOW\\TILE", "starting window number <= 0" );
      AddToStackLine( p->GetString(3) );
    }
    double xinc = 100.0/nx;
    double yinc = 100.0/ny;
    for( int j=0; j<ny; ++j )
    {
      for( int i=0; i<nx; ++i )
      {
        int n = nstart+i+j*nx;
        double xl = i*xinc;
        double xu = (i+1)*xinc;
        double yl = 100.0-(j+1)*yinc;
        double yu = 100.0-j*yinc;
        ExGlobals::AddGraphWindow( new GRA_window(n,xl,yl,xu,yu) );
      }
    }
    ExGlobals::SetWindowNumber( nstart );
    if( qualifiers["MESSAGES"] )
    {
      ExString s( "window #   xlo     ylo     xup     yup" );
      ExGlobals::WriteOutput( s );
      for( int i=nstart; i<nx*ny+nstart; ++i )
      {
        GRA_window *gw = ExGlobals::GetGraphWindow( i );
        GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
        GRA_distanceCharacteristic *xlowerwindow =
          static_cast<GRA_distanceCharacteristic*>(general->Get("XLOWERWINDOW"));
        GRA_distanceCharacteristic *ylowerwindow =
          static_cast<GRA_distanceCharacteristic*>(general->Get("YLOWERWINDOW"));
        GRA_distanceCharacteristic *xupperwindow =
          static_cast<GRA_distanceCharacteristic*>(general->Get("XUPPERWINDOW"));
        GRA_distanceCharacteristic *yupperwindow =
          static_cast<GRA_distanceCharacteristic*>(general->Get("YUPPERWINDOW"));
        double xlp = xlowerwindow->Get(true);
        double ylp = ylowerwindow->Get(true);
        double xup = xupperwindow->Get(true);
        double yup = yupperwindow->Get(true);
        char c[60];
        sprintf( c, "  %3d   %6.2f  %6.2f  %6.2f  %6.2f percentages", i, xlp, ylp, xup, yup );
        ExGlobals::WriteOutput( c );
        double xlw = xlowerwindow->Get(false);
        double ylw = ylowerwindow->Get(false);
        double xuw = xupperwindow->Get(false);
        double yuw = yupperwindow->Get(false);
        sprintf( c, "        %6.2f  %6.2f  %6.2f  %6.2f world units", xlw, ylw, xuw, yuw );
        ExGlobals::WriteOutput( c );
      }
    }
    return;
  }
  //
  // WINDOW
  //
  if( p->GetNumberOfTokens() == 1 )
  {
    ExString s( "window #   xlo     ylo     xup     yup" );
    ExGlobals::WriteOutput( s );
    std::size_t size = ExGlobals::GetNumberOfWindows();
    for( std::size_t i=0; i<size; ++i )
    {
      GRA_window *gw = ExGlobals::GetGraphWindow( i );
      GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
      GRA_distanceCharacteristic *xlowerwindow =
        static_cast<GRA_distanceCharacteristic*>(general->Get("XLOWERWINDOW"));
      GRA_distanceCharacteristic *ylowerwindow =
        static_cast<GRA_distanceCharacteristic*>(general->Get("YLOWERWINDOW"));
      GRA_distanceCharacteristic *xupperwindow =
        static_cast<GRA_distanceCharacteristic*>(general->Get("XUPPERWINDOW"));
      GRA_distanceCharacteristic *yupperwindow =
        static_cast<GRA_distanceCharacteristic*>(general->Get("YUPPERWINDOW"));
      double xlp = xlowerwindow->Get(true);
      double ylp = ylowerwindow->Get(true);
      double xup = xupperwindow->Get(true);
      double yup = yupperwindow->Get(true);
      char c[60];
      sprintf( c, "  %3d   %6.2f  %6.2f  %6.2f  %6.2f percentages", i, xlp, ylp, xup, yup );
      ExGlobals::WriteOutput( ExString(c) );
      double xlw = xlowerwindow->Get(false);
      double ylw = ylowerwindow->Get(false);
      double xuw = xupperwindow->Get(false);
      double yuw = yupperwindow->Get(false);
      sprintf( c, "        %6.2f  %6.2f  %6.2f  %6.2f world units", xlw, ylw, xuw, yuw );
      ExGlobals::WriteOutput( ExString(c) );
    }
    return;
  }
  //
  // WINDOW n { xl yl xu yu }
  // WINDOW\INHERIT n inherit { xl yl xu yu }
  //
  double d;
  try
  {
    NumericVariable::GetScalar( p->GetString(1), "window number", d );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "WINDOW", e.what() );
  }
  int n = static_cast<int>( d );
  if( n < 0 )throw ECommandError( "WINDOW", "window number < 0" );
  AddToStackLine( p->GetString(1) );
  //
  int nToken = 2;
  int maxWindow = ExGlobals::GetNumberOfWindows();
  int inherit = 0;
  if( qualifiers["INHERIT"] )
  {
    if( p->GetNumberOfTokens() < 3 )
      throw ECommandError( "WINDOW", "expecting target window number and window number to inherit from" );
    try
    {
      NumericVariable::GetScalar( p->GetString(2), "window number to inherit from", d );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "WINDOW\\INHERIT", e.what() );
    }
    inherit = static_cast<int>( d );
    if( inherit < 0 )throw ECommandError( "WINDOW", "window number to inherit from < 0" );
    if( inherit > maxWindow )
     throw ECommandError( "WINDOW", "window number to inherit from > largest defined window number" );
    //
    AddToStackLine( p->GetString(2) );
    nToken = 3;
  }
  if( p->GetNumberOfTokens() == nToken )
  {
    if( n >= maxWindow )
     throw ECommandError( "WINDOW", ExString("window number ")+ExString(n)+" is not defined" );
  }
  else
  {
    if( p->GetNumberOfTokens() < nToken+4 )
     throw ECommandError( "WINDOW", "expecting xlo, ylo, xhi, yhi" );
    double d[4];
    try
    {
      NumericVariable::GetScalar( p->GetString(nToken), "lower left x coordinate", d[0] );
      NumericVariable::GetScalar( p->GetString(nToken+1), "lower left y coordinate", d[1] );
      NumericVariable::GetScalar( p->GetString(nToken+2), "upper right x coordinate", d[2] );
      NumericVariable::GetScalar( p->GetString(nToken+3), "upper right y coordinate", d[3] );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "WINDOW", e.what() );
    }
    double xlp = d[0];
    AddToStackLine( p->GetString(2) );
    double ylp = d[1];
    AddToStackLine( p->GetString(3) );
    double xup = d[2];
    AddToStackLine( p->GetString(4) );
    double yup = d[3];
    AddToStackLine( p->GetString(5) );
    ExGlobals::AddGraphWindow( new GRA_window(n,xlp,ylp,xup,yup) );
  }
  if( qualifiers["INHERIT"] )
    ExGlobals::GetGraphWindow(n)->InheritFrom( ExGlobals::GetGraphWindow(inherit) );
  ExGlobals::SetWindowNumber( n );
}

// end of file
