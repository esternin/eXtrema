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
#include "ParseToken.h"
#include "ParseLine.h"
#include "NumericVariable.h"
#include "ECommandError.h"
#include "EExpressionError.h"
#include "EVariableError.h"
#include "UsefulFunctions.h"

#include "CMD_generate.h"

CMD_generate CMD_generate::cmd_generate_;

CMD_generate::CMD_generate() : Command( "GENERATE" )
{
  AddQualifier( "RANDOM", false );
}

void CMD_generate::Execute( ParseLine const *p )
{
  // Generate a new vector, given the name, the minimum, either the increment or
  // the maximum, and the number of elements.
  //   Or
  // Generate a set of random numbers given the name, the minimum, the maximum,
  // and the number of values to generate.
  //
  // GENERATE X MIN INC,,NPTS
  // GENERATE X MIN ,,MAX NPTS
  // GENERATE X MIN INC MAX
  // GENERATE\RANDOM X MIN MAX NPTS
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
  ExString command( "GENERATE" );
  //
  if( p->GetNumberOfTokens()<2 || !p->IsString(1) )
    throw ECommandError( command, "missing output vector name" );
  ExString name( p->GetString(1) );
  AddToStackLine( name );
  //
  double vMin, vMax, vInc;
  int vNum;
  std::vector<double> x;
  if( qualifiers["RANDOM"] )
  {
    command += ExString("\\RANDOM");
    if( p->GetNumberOfTokens() < 5 )
      throw ECommandError( command, "expecting minimum, maximum and number of values" );
    double d;
    try
    {
      NumericVariable::GetScalar( p->GetString(2), "minimum value", vMin );
      AddToStackLine( p->GetString(2) );
      //
      NumericVariable::GetScalar( p->GetString(3), "maximum value", vMax );
      AddToStackLine( p->GetString(3) );
      //
      NumericVariable::GetScalar( p->GetString(4), "number of values", d );
      AddToStackLine( p->GetString(4) );
      vNum = static_cast<int>(d);
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "GENERATE", e.what() );
    }
    if( vNum <= 0 )throw ECommandError( command, "number of values must be positive" );
    double delv = vMax - vMin;
    x.reserve( vNum );
    for( int i=0; i<vNum; ++i )
    {
      double rdum = UsefulFunctions::GetSimpleRandomNumber();
      x.push_back( rdum*delv+vMin );
    }
  }
  else  // not random numbers
  {
    if( p->GetNumberOfTokens() < 5 )
      throw ECommandError( command, "expecting min,inc,,npts or min,,max,npts or min,inc,max" );
    try
    {
      NumericVariable::GetScalar( p->GetString(2), "minimum value", vMin );
      AddToStackLine( p->GetString(2) );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( "GENERATE", e.what() );
    }
    //
    bool incrementGiven = false;
    bool maximumGiven = false;
    bool nptsGiven = false;
    //
    if( p->IsNull(3) )
    {
      AddToStackLine( ",," );
    }
    else
    {
      incrementGiven = true;
      try
      {
        NumericVariable::GetScalar( p->GetString(3), "increment", vInc );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "GENERATE", e.what() );
      }
      if( vInc == 0.0 )throw ECommandError( command, "increment = 0" );
      AddToStackLine( p->GetString(3) );
    }
    if( p->IsNull(4) )
    {
      AddToStackLine( ",," );
    }
    else
    {
      maximumGiven = true;
      try
      {
        NumericVariable::GetScalar( p->GetString(4), "maximum value", vMax );
        AddToStackLine( p->GetString(4) );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "GENERATE", e.what() );
      }
    }
    if( p->GetNumberOfTokens() > 5 )
    {
      nptsGiven = true;
      double d;
      try
      {
        NumericVariable::GetScalar( p->GetString(5), "number of values", d );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( "GENERATE", e.what() );
      }
      vNum = static_cast<int>(d);
      if( vNum == 0 )throw ECommandError( command, "number of values = 0" );
      if( vNum < 0 )throw ECommandError( command, "number of values < 0" );
      AddToStackLine( p->GetString(5) );
    }
    if( incrementGiven )
    {
      if( maximumGiven )
      {
        vNum = static_cast<int>((vMax-vMin)/vInc) + 1;
        if( vNum <= 0 )throw ECommandError( command, "number of values must be positive" );
        x.reserve( vNum );
        for( int i=0; i<vNum; ++i )x.push_back( vMin+i*vInc );
        if( fabs(x[vNum-1]-vMax) > fabs(vMax*0.000001) )
        {
          ExString s( "GENERATE: calculated maximum = " );
          s += ExString(x[vNum-1]) + ExString(", given maximum = ") + ExString(vMax);
          ExGlobals::WarningMessage(s);
        }
        if( nptsGiven )ExGlobals::WarningMessage("GENERATE: number of points will be ignored");
      }
      else if( nptsGiven )
      {
        x.reserve( vNum );
        for( int i=0; i<vNum; ++i )x.push_back( vMin+i*vInc );
      }
      else
        throw ECommandError( command, "expecting either the number of points or the maximum value" );
    }
    else  // increment not given
    {
      if( !(maximumGiven || nptsGiven) )
        throw ECommandError( command, "expecting the number of points and the maximum value" );
      vNum > 1 ? vInc = (vMax-vMin)/(vNum-1) : vInc = 0.0;
      x.reserve( vNum );
      for( int i=0; i<vNum-1; ++i )x.push_back( vMin+i*vInc );
      x.push_back( vMax );
    }
  }
  int order = 0;
  if( !qualifiers["RANDOM"] )
  {
    vInc>0.0 ? order=1 : order=2;
  }
  try
  {
    NumericVariable::PutVariable( name, x, order, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command, e.what() );
  }
}

// end of file
