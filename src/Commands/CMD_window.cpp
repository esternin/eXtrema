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

#include "CMD_window.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ExGlobals.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "GRA_window.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_setOfCharacteristics.h"

CMD_window *CMD_window::cmd_window_ = 0;

CMD_window::CMD_window() : Command( wxT("WINDOW") )
{
  AddQualifier( wxT("TILE"), false );
  AddQualifier( wxT("MESSAGES"), true );
  AddQualifier( wxT("INHERIT"), false );
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
  SetUp( p, qualifiers );
  wxString command( Name()+wxT(": ") );
  if( qualifiers[wxT("TILE")] )
  {
    // WINDOW\TILE nx ny { start }
    //
    if( p->GetNumberOfTokens() < 3 )throw ECommandError( command+wxT("expecting 2 parameters:  nx ny") );
    double d[3];
    try
    {
      NumericVariable::GetScalar( p->GetString(1), wxT("number of windows horizontally"), d[0] );
      NumericVariable::GetScalar( p->GetString(2), wxT("number of windows vertically"), d[1] );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    int nx = static_cast<int>( d[0] );
    int ny = static_cast<int>( d[1] );
    //
    if( nx <= 0 )throw ECommandError( command+wxT("number of windows horizontally <= 0") );
    if( ny <= 0 )throw ECommandError( command+wxT("number of windows vertically <= 0") );
    //
    AddToStackLine( p->GetString(1) );
    AddToStackLine( p->GetString(2) );
    //
    int nstart = 1;
    if( p->GetNumberOfTokens() == 4 )
    {
      try
      {
        NumericVariable::GetScalar( p->GetString(3), wxT("starting window number"), d[0] );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      nstart = static_cast<int>( d[0] );
      if( nstart <= 0 )throw ECommandError( command+wxT("starting window number <= 0") );
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
    if( qualifiers[wxT("MESSAGES")] )
    {
      wxString s( wxT("window #   xlo     ylo     xup     yup") );
      ExGlobals::WriteOutput( s );
      for( int i=nstart; i<nx*ny+nstart; ++i )
      {
        GRA_window *gw = ExGlobals::GetGraphWindow( i );
        GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
        GRA_distanceCharacteristic *xlowerwindow =
          static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("XLOWERWINDOW")));
        GRA_distanceCharacteristic *ylowerwindow =
          static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("YLOWERWINDOW")));
        GRA_distanceCharacteristic *xupperwindow =
          static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("XUPPERWINDOW")));
        GRA_distanceCharacteristic *yupperwindow =
          static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("YUPPERWINDOW")));
        double xlp = xlowerwindow->Get(true);
        double ylp = ylowerwindow->Get(true);
        double xup = xupperwindow->Get(true);
        double yup = yupperwindow->Get(true);
        wxChar c[60];
        ::wxSnprintf( c, 60, wxT("  %3d   %6.2f  %6.2f  %6.2f  %6.2f percentages"), i, xlp, ylp, xup, yup );
        ExGlobals::WriteOutput( c );
        double xlw = xlowerwindow->Get(false);
        double ylw = ylowerwindow->Get(false);
        double xuw = xupperwindow->Get(false);
        double yuw = yupperwindow->Get(false);
        ::wxSnprintf( c, 60, wxT("        %6.2f  %6.2f  %6.2f  %6.2f world units"), xlw, ylw, xuw, yuw );
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
    wxString s( wxT("window #   xlo     ylo     xup     yup") );
    ExGlobals::WriteOutput( s );
    std::size_t size = ExGlobals::GetNumberOfWindows();
    for( std::size_t i=0; i<size; ++i )
    {
      GRA_window *gw = ExGlobals::GetGraphWindow( i );
      GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
      GRA_distanceCharacteristic *xlowerwindow =
        static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("XLOWERWINDOW")));
      GRA_distanceCharacteristic *ylowerwindow =
        static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("YLOWERWINDOW")));
      GRA_distanceCharacteristic *xupperwindow =
        static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("XUPPERWINDOW")));
      GRA_distanceCharacteristic *yupperwindow =
        static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("YUPPERWINDOW")));
      double xlp = xlowerwindow->Get(true);
      double ylp = ylowerwindow->Get(true);
      double xup = xupperwindow->Get(true);
      double yup = yupperwindow->Get(true);
      wxChar c[60];
      ::wxSnprintf( c, 60, wxT("  %3d   %6.2f  %6.2f  %6.2f  %6.2f percentages"), i, xlp, ylp, xup, yup );
      ExGlobals::WriteOutput( wxString(c) );
      double xlw = xlowerwindow->Get(false);
      double ylw = ylowerwindow->Get(false);
      double xuw = xupperwindow->Get(false);
      double yuw = yupperwindow->Get(false);
      ::wxSnprintf( c, 60, wxT("        %6.2f  %6.2f  %6.2f  %6.2f world units"), xlw, ylw, xuw, yuw );
      ExGlobals::WriteOutput( wxString(c) );
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
    NumericVariable::GetScalar( p->GetString(1), wxT("window number"), d );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  int n = static_cast<int>( d );
  if( n < 0 )throw ECommandError( command+wxT("window number < 0") );
  AddToStackLine( p->GetString(1) );
  //
  int nToken = 2;
  int maxWindow = ExGlobals::GetNumberOfWindows();
  int inherit = 0;
  if( qualifiers[wxT("INHERIT")] )
  {
    if( p->GetNumberOfTokens() < 3 )
      throw ECommandError( command+wxT("expecting target window number and window number to inherit from") );
    try
    {
      NumericVariable::GetScalar( p->GetString(2), wxT("window number to inherit from"), d );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    inherit = static_cast<int>( d );
    if( inherit < 0 )throw ECommandError( command+wxT("window number to inherit from < 0") );
    if( inherit >= maxWindow )
     throw ECommandError( command+wxT("window number to inherit from > largest defined window number") );
    //
    AddToStackLine( p->GetString(2) );
    nToken = 3;
  }
  if( p->GetNumberOfTokens() == nToken )
  {
    if( n >= maxWindow )
    {
      wxString s( wxT("window number ") );
      throw ECommandError( command+(s << n << wxT(" is not defined")) );
    }
  }
  else
  {
    if( p->GetNumberOfTokens() < nToken+4 )
     throw ECommandError( command+wxT("expecting xlo, ylo, xhi, yhi") );
    double d[4];
    try
    {
      NumericVariable::GetScalar( p->GetString(nToken), wxT("lower left x coordinate"), d[0] );
      NumericVariable::GetScalar( p->GetString(nToken+1), wxT("lower left y coordinate"), d[1] );
      NumericVariable::GetScalar( p->GetString(nToken+2), wxT("upper right x coordinate"), d[2] );
      NumericVariable::GetScalar( p->GetString(nToken+3), wxT("upper right y coordinate"), d[3] );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
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
  if( qualifiers[wxT("INHERIT")] )
    ExGlobals::GetGraphWindow(n)->InheritFrom( ExGlobals::GetGraphWindow(inherit) );
  ExGlobals::SetWindowNumber( n );
}

// end of file
