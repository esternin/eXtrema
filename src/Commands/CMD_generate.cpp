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

#include "CMD_generate.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "ExGlobals.h"
#include "UsefulFunctions.h"

CMD_generate *CMD_generate::cmd_generate_ = 0;

CMD_generate::CMD_generate() : Command( wxT("GENERATE") )
{
  AddQualifier( wxT("RANDOM"), false );
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
  SetUp( p, qualifiers );
  wxString command( wxT("GENERATE: ") );
  //
  if( p->GetNumberOfTokens()<2 || !p->IsString(1) )
    throw ECommandError( command+wxT("missing output vector name") );
  wxString name( p->GetString(1) );
  AddToStackLine( name );
  //
  double vMin, vMax, vInc;
  int vNum;
  std::vector<double> x;
  if( qualifiers[wxT("RANDOM")] )
  {
    if( p->GetNumberOfTokens() < 5 )
      throw ECommandError( command+wxT("expecting minimum, maximum and number of values") );
    double d;
    try
    {
      NumericVariable::GetScalar( p->GetString(2), wxT("minimum value"), vMin );
      AddToStackLine( p->GetString(2) );
      //
      NumericVariable::GetScalar( p->GetString(3), wxT("maximum value"), vMax );
      AddToStackLine( p->GetString(3) );
      //
      NumericVariable::GetScalar( p->GetString(4), wxT("number of values"), d );
      AddToStackLine( p->GetString(4) );
      vNum = static_cast<int>(d);
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    if( vNum <= 0 )throw ECommandError( command+wxT("number of values must be positive") );
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
      throw ECommandError( command+wxT("expecting min,inc,,npts or min,,max,npts or min,inc,max") );
    try
    {
      NumericVariable::GetScalar( p->GetString(2), wxT("minimum value"), vMin );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    AddToStackLine( p->GetString(2) );
    //
    bool incrementGiven = false;
    bool maximumGiven = false;
    bool nptsGiven = false;
    //
    if( p->IsNull(3) )
    {
      AddToStackLine();
    }
    else
    {
      incrementGiven = true;
      try
      {
        NumericVariable::GetScalar( p->GetString(3), wxT("increment"), vInc );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      if( vInc == 0.0 )throw ECommandError( command+wxT("increment = 0") );
      AddToStackLine( p->GetString(3) );
    }
    if( p->IsNull(4) )
    {
      AddToStackLine();
    }
    else
    {
      maximumGiven = true;
      try
      {
        NumericVariable::GetScalar( p->GetString(4), wxT("maximum value"), vMax );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      AddToStackLine( p->GetString(4) );
    }
    if( p->GetNumberOfTokens() > 5 )
    {
      nptsGiven = true;
      double d;
      try
      {
        NumericVariable::GetScalar( p->GetString(5), wxT("number of values"), d );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      vNum = static_cast<int>(d);
      if( vNum == 0 )throw ECommandError( command+wxT("number of values = 0") );
      if( vNum < 0 )throw ECommandError( command+wxT("number of values < 0") );
      AddToStackLine( p->GetString(5) );
    }
    if( incrementGiven )
    {
      if( maximumGiven )
      {
        vNum = static_cast<int>((vMax-vMin)/vInc) + 1;
        if( vNum <= 0 )throw ECommandError( command+wxT("number of values must be positive") );
        x.reserve( vNum );
        for( int i=0; i<vNum; ++i )x.push_back( vMin+i*vInc );
        if( fabs(x[vNum-1]-vMax) > fabs(vMax*0.000001) )
        {
          wxString s( wxT("GENERATE: calculated maximum = ") );
          s << x[vNum-1] << wxT(", given maximum = ") << vMax;
          ExGlobals::WarningMessage( s );
        }
        if( nptsGiven )ExGlobals::WarningMessage(wxT("GENERATE: number of points will be ignored"));
      }
      else if( nptsGiven )
      {
        x.reserve( vNum );
        for( int i=0; i<vNum; ++i )x.push_back( vMin+i*vInc );
      }
      else
        throw ECommandError( command+wxT("expecting either the number of points or the maximum value") );
    }
    else  // increment not given
    {
      if( !(maximumGiven || nptsGiven) )
        throw ECommandError( command+wxT("expecting the number of points and the maximum value") );
      vNum > 1 ? vInc = (vMax-vMin)/(vNum-1) : vInc = 0.0;
      x.reserve( vNum );
      for( int i=0; i<vNum-1; ++i )x.push_back( vMin+i*vInc );
      x.push_back( vMax );
    }
  }
  int order = 0;
  if( !qualifiers[wxT("RANDOM")] )order = vInc>0.0 ? 1 : 2;
  try
  {
    NumericVariable::PutVariable( name, x, order, p->GetInputLine() );
  }
  catch ( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
}

// end of file
