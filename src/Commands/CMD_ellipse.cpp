/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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
#include <stdexcept>

#include "CMD_ellipse.h"
#include "ExGlobals.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "ECommandError.h"
#include "EVariableError.h"
#include "UsefulFunctions.h"
#include "NumericVariable.h"
#include "GRA_cartesianCurve.h"
#include "GRA_window.h"

CMD_ellipse *CMD_ellipse::cmd_ellipse_ = 0;

CMD_ellipse::CMD_ellipse() : Command( wxT("ELLIPSE") )
{
  AddQualifier( wxT("POPULATE"), true );
  AddQualifier( wxT("FIT"), false );
  AddQualifier( wxT("NPTS"), false );
  AddQualifier( wxT("FRACTION"), false );
  AddQualifier( wxT("XYOUT"), false );
  AddQualifier( wxT("PARAMETERS"), false );
  AddQualifier( wxT("MESSAGES"), true );
}

void CMD_ellipse::Execute( ParseLine const *p )
{
  //
  QualifierMap qualifiers;
  SetUp( p, qualifiers );
  wxString command( Name()+wxT(": ") );
  if( qualifiers[wxT("FIT")] )qualifiers[wxT("POPULATE")] = false;
  if( qualifiers[wxT("POPULATE")] )
  {
    if( qualifiers[wxT("FRACTION")] )
      throw ECommandError( command+wxT("\\FRACTION qualifier not compatible with \\POPULATE qualifier") );
    if( qualifiers[wxT("PARAMETERS")] )
      throw ECommandError( command+wxT("\\PARAMATERS qualifier not compatible with \\POPULATE qualifier") );
  }
  int counter = 1;
  if( qualifiers[wxT("FIT")] )
  {
    // ELLIPSE\FIT XIN YIN
    // ELLIPSE\FIT\NPTS XIN YIN N
    // ELLIPSE\FIT\FRACTION XIN YIN FRAC
    // ELLIPSE\FIT\FRACTION\NPTS XIN YIN FRAC N
    // ELLIPSE\FIT\XYOUT XIN YIN XO YO
    // ELLIPSE\FIT\XYOUT\NPTS XIN YIN N XO YO
    // ELLIPSE\FIT\XYOUT\FRACTION XIN YIN FRAC XO YO 
    // ELLIPSE\FIT\XYOUT\FRACTION\NPTS XIN YIN FRAC N XO YO
    // ELLIPSE\FIT\PARAMETERS XIN YIN A B XC YC ANGLE 
    // ELLIPSE\FIT\PARAMETERS\NPTS XIN YIN A B XC YC ANGLE N
    // ELLIPSE\FIT\PARAMETERS\FRAC XIN YIN FRAC A B XC YC ANGLE
    // ELLIPSE\FIT\PARAMETERS\FRAC\NPTS XIN YIN FRAC A B XC YC ANGLE N
    // ELLIPSE\FIT\PARAMETERS\XYOUT XIN YIN A B XC YC ANGLE XO YO
    // ELLIPSE\FIT\PARAMETERS\XYOUT\NPTS XIN YIN A B XC YC ANGLE N XO YO
    // ELLIPSE\FIT\PARAMETERS\XYOUT\FRAC XIN YIN FRAC A B XC YC ANGLE XO YO
    // ELLIPSE\FIT\PARAMETERS\XYOUT\FRAC\NPTS XIN YIN FRAC A B XC YC ANGLE N XO YO
    //
    // Default value of fraction = 0.9
    //
    std::vector<double> x, y;
    if( p->GetNumberOfTokens() < counter+1 )
      throw ECommandError( command+wxT("expecting independent input vector") );
    try
    {
      NumericVariable::GetVector( p->GetString(counter), wxT("independent input vector"), x );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(counter++) );
    //
    if( p->GetNumberOfTokens() < counter+1 )
      throw ECommandError( command+wxT("expecting dependent input vector") );
    try
    {
      NumericVariable::GetVector( p->GetString(counter), wxT("dependent input vector"), y );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(counter++) );

    if( x.size() != y.size() )
      throw ECommandError( command+wxT("data vector lengths are not equal") );
    int num = static_cast<int>(x.size());
    if( num <= 1 )throw ECommandError( command+wxT("data vector lengths are <= 1") );

    double fraction = 0.9;
    if( qualifiers[wxT("FRACTION")] )
    {
      if( p->GetNumberOfTokens()<counter+1 || !(p->IsString(counter)||p->IsNumeric(counter)) )
        throw ECommandError( command+wxT("expecting fraction of points inside ellipse") );
      try
      {
        NumericVariable::GetScalar( p->GetString(counter), wxT("fraction of points inside ellipse"), fraction );
      }
      catch( EVariableError const &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( fraction < 0.0 )
        throw ECommandError( command+wxT("fraction of points inside ellipse < 0") );
      if( fraction == 0.0 )
        throw ECommandError( command+wxT("fraction of points inside ellipse = 0") );
      if( fraction > 1.0 )
        throw ECommandError( command+wxT("fraction of points inside ellipse > 1") );
      AddToStackLine( p->GetString(counter++) );
    }
    if( fraction*num < 0.5 )
      throw ECommandError( command+wxT("fraction of points * data vector length < 0.5") );
    
    // fit ellipse to vectors
    //
    double xc, yc, a, b, angle;
    try
    {
      UsefulFunctions::EllipseFit( x, y, fraction, xc, yc, a, b, angle );
    }
    catch (std::runtime_error const &e)
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( qualifiers[wxT("MESSAGES")] )
    {
      ExGlobals::WriteOutput( wxString()<<wxT("Major-axis = ")<<a );
      ExGlobals::WriteOutput( wxString()<<wxT("Minor-axis = ")<<b );
      ExGlobals::WriteOutput( wxString()<<wxT("Centre = (")<<xc<<wxT(",")<<yc<<wxT(")") );
      ExGlobals::WriteOutput( wxString()<<wxT("Angle = ")<<angle );
    }
    if( qualifiers[wxT("PARAMETERS")] )
    {
      if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
        throw ECommandError( command+wxT("expecting major axis output scalar name") );
      wxString majorAxisName( p->GetString(counter++) );
      AddToStackLine( majorAxisName );
      //
      if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
        throw ECommandError( command+wxT("expecting minor axis output scalar name") );
      wxString minorAxisName( p->GetString(counter++) );
      AddToStackLine( minorAxisName );
      //
      if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
        throw ECommandError( command+wxT("expecting centre x-coordinate output scalar name") );
      wxString xcName( p->GetString(counter++) );
      AddToStackLine( xcName );
      //
      if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
        throw ECommandError( command+wxT("expecting centre y-coordinate output scalar name") );
      wxString ycName( p->GetString(counter++) );
      AddToStackLine( ycName );
      //
      if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
        throw ECommandError( command+wxT("expecting angle output scalar name") );
      wxString angleName( p->GetString(counter++) );
      AddToStackLine( angleName );
      //
      try
      {
        NumericVariable::PutVariable( majorAxisName, a, p->GetInputLine() );
        NumericVariable::PutVariable( minorAxisName, b, p->GetInputLine() );
        NumericVariable::PutVariable( xcName, xc, p->GetInputLine() );
        NumericVariable::PutVariable( ycName, yc, p->GetInputLine() );
        NumericVariable::PutVariable( angleName, angle, p->GetInputLine() );
      }
      catch( EVariableError const &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
    }
    PopulateEllipse( qualifiers[wxT("NPTS")], qualifiers[wxT("XYOUT")], p, counter,
                     a, b, xc, yc, angle );
  }
  else  // not a fit, must be populate
  {
    // ELLIPSE\POPULATE A B XC YC ANGLE
    // ELLIPSE\POPULATE\NPTS A B XC YC ANGLE N
    // ELLIPSE\POPULATE\XYOUT A B XC YC ANGLE XO YO
    // ELLIPSE\POPULATE\XYOUT\NPTS A B XC YC ANGLE N XO YO
    //
    double a, b, xc, yc, angle;
    if( p->GetNumberOfTokens()<counter+1 || !(p->IsString(counter)||p->IsNumeric(counter)) )
      throw ECommandError( command+wxT("expecting major axis length") );
    try
    {
      NumericVariable::GetScalar( p->GetString(counter), wxT("major axis length"), a );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(counter++) );
    if( a < 0.0 )
      throw ECommandError( command+wxT("major axis length < 0") );
    if( a == 0.0 )
      throw ECommandError( command+wxT("major axis length = 0") );
    //
    if( p->GetNumberOfTokens()<counter+1 || !(p->IsString(counter)||p->IsNumeric(counter)) )
      throw ECommandError( command+wxT("expecting minor axis length") );
    try
    {
      NumericVariable::GetScalar( p->GetString(counter), wxT("minor axis length"), b );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(counter++) );
    if( b < 0.0 )
      throw ECommandError( command+wxT("minor axis length < 0") );
    if( b == 0.0 )
      throw ECommandError( command+wxT("minor axis length = 0") );
    //
    if( p->GetNumberOfTokens()<counter+1 || !(p->IsString(counter)||p->IsNumeric(counter)) )
      throw ECommandError( command+wxT("expecting x-coordinate of ellipse centre") );
    try
    {
      NumericVariable::GetScalar( p->GetString(counter), wxT("x-coordinate of ellipse centre"), xc );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(counter++) );
    //
    if( p->GetNumberOfTokens()<counter+1 || !(p->IsString(counter)||p->IsNumeric(counter)) )
      throw ECommandError( command+wxT("expecting y-coordinate of ellipse centre") );
    try
    {
      NumericVariable::GetScalar( p->GetString(counter), wxT("y-coordinate of ellipse centre"), yc );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(counter++) );
    //
    if( p->GetNumberOfTokens()<counter+1 || !(p->IsString(counter)||p->IsNumeric(counter)) )
      throw ECommandError( command+wxT("expecting ellipse angle (degrees)") );
    try
    {
      NumericVariable::GetScalar( p->GetString(counter), wxT("ellipse angle (degrees)"), angle );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(counter++) );
    //
    PopulateEllipse( qualifiers[wxT("NPTS")], qualifiers[wxT("XYOUT")], p, counter,
                     a, b, xc, yc, angle );
  }
}

void CMD_ellipse::PopulateEllipse( bool npts, bool xyout, ParseLine const *p,
                                   int counter, double a, double b, double xc, double yc, double angle )
{
  wxString command( wxT("ELLIPSE: ") );
  int nOut = 260;
  if( npts )
  {
    if( p->GetNumberOfTokens()<counter+1 || !(p->IsString(counter)||p->IsNumeric(counter)) )
      throw ECommandError( command+wxT("expecting number of output points") );
    double tmp;
    try
    {
      NumericVariable::GetScalar( p->GetString(counter), wxT("number of output points"), tmp );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(counter++) );
    nOut = static_cast<int>(tmp+0.5);
    if( nOut < 0 )
      throw ECommandError( command+wxT("number of output points < 0") );
    if( nOut == 0 )
      throw ECommandError( command+wxT("number of output points = 0") );
  }
  int nd4 = nOut/4;
  std::vector<double> xout(nOut+1), yout(nOut+1);
  double const pi = acos(-1.0);
  //
  // populate 1/4 of the ellipse with nOut/4 points in quadrant 1 of
  // the standard coordinate system for the ellipse
  //
  for( int i=0; i<nd4+1; ++i )
  {
    xout[i] = a*cos(i*pi/2/nd4);
    yout[i] = b*sin(i*pi/2/nd4);
  }
  //
  // populate 1/4 of the ellipse with nOut/4 points in quadrant 2 of 
  // the standard coordinate system for the ellipse
  //
  for( int i=1; i<=nd4; ++i )
  {
    xout[i+nd4] = -xout[nd4-i];
    yout[i+nd4] = yout[nd4-i];
  }
  //
  // populate the bottom 1/2 of the ellipse with nOut/4*2 points
  //
  for( int i=1; i<=2*nd4-1; ++i )
  {
    xout[i+2*nd4] = -xout[i];
    yout[i+2*nd4] = -yout[i];
  }
  //
  // rotate the ellipse so that it has an orientation angle of angle
  // degrees w.r.t the x-axis of the new frame
  // also shift the ellipse center to (xc,yc)
  //
  double cangr = cos(angle*pi/180.);
  double sangr = sin(angle*pi/180.);
  for( int i=0; i<nOut; ++i )
  {
    double tmp = cangr*xout[i] - sangr*yout[i] + xc;
    yout[i] = sangr*xout[i] + cangr*yout[i] + yc;
    xout[i] = tmp;
  }
  xout[nOut] = xout[0];
  yout[nOut] = yout[0];
  if( xyout )
  {
    if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
      throw ECommandError( command+wxT("expecting output independent vector name") );
    wxString xoutName( p->GetString(counter++) );
    AddToStackLine( xoutName );
    //
    if( p->GetNumberOfTokens()<counter+1 || !p->IsString(counter) )
      throw ECommandError( command+wxT("expecting output dependent vector name") );
    wxString youtName( p->GetString(counter++) );
    AddToStackLine( youtName );
    //
    try
    {
      NumericVariable::PutVariable( xoutName, xout, 0, p->GetInputLine() );
      NumericVariable::PutVariable( youtName, yout, 0, p->GetInputLine() );
    }
    catch( EVariableError const &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
  }
  else
  {
    wxClientDC dc( ExGlobals::GetwxWindow() );
    std::vector<double> xe1, ye1, xe2, ye2;
    GRA_cartesianCurve *cartesianCurve = new GRA_cartesianCurve(xout,yout,xe1,ye1,xe2,ye2);
    cartesianCurve->Make();
    cartesianCurve->Draw( ExGlobals::GetGraphicsOutput(), dc );
    ExGlobals::GetGraphWindow()->AddDrawableObject( cartesianCurve );
  }
}

// end of file
