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
#include "EExpressionError.h"
#include "NumericVariable.h"
#include "GRA_window.h"

#include "CMD_world.h"

CMD_world CMD_world::cmd_world_;

CMD_world::CMD_world() : Command( "WORLD" )
{
  AddQualifier( "PERCENT", false );
  AddQualifier( "MESSAGES", true );
}

void CMD_world::Execute( ParseLine const *p )
{
  // WORLD xin yin xout yout
  // WORLD\PERCENT xin yin xout yout
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
  if( p->GetNumberOfTokens() < 3 )throw ECommandError( "WORLD", "expecting x and y" );
  if( p->IsNull(1) || p->IsNull(2) )throw ECommandError( "WORLD", "expecting x and y" );
  int xDim, yDim;
  double xValue, yValue;
  std::vector<double> xData, yData;
  int dimSize[3];
  try
  {
    NumericVariable::GetVariable( p->GetString(1), xDim, xValue, xData, dimSize );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "WORLD", e.what() );
  }
  if( xDim == 2 )throw ECommandError( "WORLD", "independent input variable is a matrix" );
  if( xDim == 3 )throw ECommandError( "WORLD", "independent input variable is a tensor" );
  try
  {
    NumericVariable::GetVariable( p->GetString(2), yDim, yValue, yData, dimSize );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( "WORLD", e.what() );
  }
  if( yDim == 2 )throw ECommandError( "WORLD", "dependent input variable is a matrix" );
  if( yDim == 3 )throw ECommandError( "WORLD", "dependent input variable is a tensor" );
  if( xDim != yDim )throw ECommandError( "WORLD", "input variables must be the same type" );
  AddToStackLine( p->GetString(1) );
  AddToStackLine( p->GetString(2) );
  //
  ExString xName, yName;
  bool makeVar = false;
  if( p->IsString(3) )
  {
    if( !p->IsString(4) )
      throw ECommandError( "WORLD", "both output variables must be present if one is present" );
    makeVar = true;
    xName = p->GetString(3);
    yName = p->GetString(4);
    AddToStackLine( xName );
    AddToStackLine( yName );
  }
  if( xDim == 0 ) // scalars were input
  {
    double xout, yout;
    ExGlobals::GetGraphWindow()->GraphToWorld( xValue, yValue, xout, yout, true );
    if( qualifiers["PERCENT"] )
    {
      double xtmp, ytmp;
      ExGlobals::GetGraphWindow()->WorldToPercent( xout, yout, xtmp, ytmp );
      xout = xtmp;
      yout = ytmp;
    }
    if( makeVar )
    {
      try
      {
        NumericVariable::PutVariable( xName, xout, p->GetInputLine() );
        NumericVariable::PutVariable( yName, yout, p->GetInputLine() );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( "WORLD", e.what() );
      }
    }
    else
    {
      ExString message = qualifiers["PERCENT"] ? "percentage of the current window: xout = " :
                                                 "world units: xout = ";
      message += ExString(xout) + ExString(", yout = ") + ExString(yout);
      ExGlobals::WriteOutput( message );
    }
  }
  else // vectors were input
  {
    std::size_t vSize = xData.size();
    std::vector<double> xout(vSize), yout(vSize);
    for( std::size_t i=0; i<vSize; ++i )
    {
      double xtmp, ytmp;
      ExGlobals::GetGraphWindow()->GraphToWorld( xData[i], yData[i], xtmp, ytmp );
      xout[i] = xtmp;
      yout[i] = ytmp;
      if( qualifiers["PERCENT"] )
      {
        double xtmp, ytmp;
        ExGlobals::GetGraphWindow()->WorldToPercent( xout[i], yout[i], xtmp, ytmp );
        xout[i] = xtmp;
        yout[i] = ytmp;
      }
    }
    if( makeVar )
    {
      try
      {
        NumericVariable::PutVariable( xName, xout, 0, p->GetInputLine() );
        NumericVariable::PutVariable( yName, yout, 0, p->GetInputLine() );
      }
      catch (EVariableError &e)
      {
        throw ECommandError( "WORLD", e.what() );
      }
    }
    else
    {
      ExString message = qualifiers["PERCENT"] ? "percentage of the current window: xout = " :
                                                 "world units: xout = ";
      message += ExString(xout[0]) + ExString(", yout = ") + ExString(yout[0]);
      ExGlobals::WriteOutput( message );
      for( std::size_t i=1; i<vSize; ++i )
      {
        qualifiers["PERCENT"] ? message.assign( "                                  xout = ") :
                                message.assign( "             xout = " );
        message += ExString(xout[i]) + ExString(", yout = ") + ExString(yout[i]);
        ExGlobals::WriteOutput( message );
      }
    }
  }
}
 
// end of file
