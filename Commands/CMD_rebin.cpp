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

#include "CMD_rebin.h"

CMD_rebin CMD_rebin::cmd_rebin_;

void CMD_rebin::Execute( ParseLine const *p )
{
  // REBIN vIn vOut n
  // REBIN mIn mOut nr nc
  //
  SetUp( p );
  std::vector<double> x;
  if( p->GetNumberOfTokens() < 2 )
    throw ECommandError( "REBIN", "expecting data vector or matrix" );
  double v1;
  int dimSizes[3];
  ExString xName( p->GetString(1) );
  int xDim, xLen, xRows, xCols;
  try
  {
    NumericVariable::GetVariable( xName, xDim, v1, x, dimSizes );
  }
  catch (EVariableError &e)
  {
    throw ECommandError( "REBIN", e.what() );
  }
  if( xDim == 0 )
  {
    ExString tmp( xName+" is a scalar" );
    throw ECommandError( "REBIN", tmp );
  }
  else if( xDim == 3 )
  {
    ExString tmp( xName+" is a tensor" );
    throw ECommandError( "REBIN", tmp );
  }
  else if( xDim == 1 )
  {
    xLen = x.size();
  }
  else
  {
    xRows = dimSizes[0];
    xCols = dimSizes[1];
  }
  AddToStackLine( p->GetString(1) );
  //
  if( p->GetNumberOfTokens()<3 || !p->IsString(2) )
    throw ECommandError( "REBIN", "expecting output counts variable name" );
  ExString countsName( p->GetString(2) );
  AddToStackLine( countsName );
  if( xDim == 1 )  // rebin a vector
  {
    if( p->GetNumberOfTokens() < 4 )throw ECommandError( "REBIN", "expecting data compression factor" );
    int compressionFactor;
    try
    {
      double d;
      NumericVariable::GetScalar( p->GetString(3), "compression factor", d );
      AddToStackLine( p->GetString(3) );
      compressionFactor = static_cast<int>(d+0.5);
    }
    catch (EVariableError &e)
    {
      throw ECommandError( "REBIN", e.what() );
    }
    if( compressionFactor < 0 )throw ECommandError( "REBIN", "compression factor < 0" );
    if( compressionFactor == 0 )throw ECommandError( "REBIN", "compression factor = 0" );
    if( xLen%compressionFactor != 0 )ExGlobals::WarningMessage("REBIN: the last bin will not be complete");
    int rNum = xLen/compressionFactor;
    std::vector<double> y( rNum, 0.0 );
    for( int i=0; i<compressionFactor; ++i )
    {
      for( int j=0; j<rNum; ++j )y[j] += x[i+j*compressionFactor];
    }
    NumericVariable::PutVariable( countsName, y, 0, p->GetInputLine() );
  }
  else                // rebin a matrix
  {
    if( p->GetNumberOfTokens() < 4 )throw ECommandError( "REBIN", "expecting row compression factor" );
    int rowCompressionFactor;
    try
    {
      double d;
      NumericVariable::GetScalar( p->GetString(3), "row compression factor", d );
      AddToStackLine( p->GetString(3) );
      rowCompressionFactor = static_cast<int>(d+0.5);
    }
    catch (EVariableError &e)
    {
      throw ECommandError( "REBIN", e.what() );
    }
    if( rowCompressionFactor < 0 )throw ECommandError( "REBIN", "row compression factor < 0" );
    if( rowCompressionFactor == 0 )throw ECommandError( "REBIN", "row compression factor = 0" );
    if( xRows%rowCompressionFactor != 0 )
      ExGlobals::WarningMessage("REBIN: the last bin row will not be complete");
    if( p->GetNumberOfTokens() < 5 )throw ECommandError( "REBIN", "expecting column compression factor" );
    int columnCompressionFactor;
    try
    {
      double d;
      NumericVariable::GetScalar( p->GetString(4), "column compression factor", d );
      AddToStackLine( p->GetString(4) );
      columnCompressionFactor = static_cast<int>(d+0.5);
    }
    catch (EVariableError &e)
    {
      throw ECommandError( "REBIN", e.what() );
    }
    if( columnCompressionFactor < 0 )throw ECommandError( "REBIN", "column compression factor < 0" );
    if( columnCompressionFactor == 0 )throw ECommandError( "REBIN", "column compression factor = 0" );
    if( xCols%columnCompressionFactor != 0 )
      ExGlobals::WarningMessage( "REBIN: the last bin column will not be complete" );
    int rNum = xRows/rowCompressionFactor;
    int cNum = xCols/columnCompressionFactor;
    //
    std::vector<double> y1( rNum*xCols, 0.0 );
    std::vector<double> y2( rNum*cNum, 0.0 );
    for( int k=0; k<xCols; ++k )
    {
      for( int i=0; i<rowCompressionFactor; ++i )
      {
        for( int j=0; j<rNum; ++j )y1[j+k*rNum] += x[i+j*rowCompressionFactor+k*xRows];
      }
    }
    for( int j=0; j<rNum; ++j )
    {
      for( int i=0; i<columnCompressionFactor; ++i )
      {
        for( int k=0; k<cNum; ++k )y2[j+k*rNum] += y1[j+(i+k*columnCompressionFactor)*rNum];
      }
    }
    NumericVariable::PutVariable( countsName, y2, rNum, cNum, p->GetInputLine() );
  }
}
 
// end of file
