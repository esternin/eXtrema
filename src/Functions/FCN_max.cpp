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
#include "FCN_max.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"

FCN_max *FCN_max::fcn_max_ = 0;

void FCN_max::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    numberOfArguments_ = 1;
    nLoop = 0;
  }
  else
  {
    ++numberOfArguments_;
    nLoop = ndmEff;
  }
}
 
void FCN_max::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  std::vector<double> d1;
  int ndm1, size1[3];
  double val1;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    ndm1 = nd.GetNumberOfDimensions();
    if( ndm1 == 0 )
      val1 = nd.GetScalarValue();
    else
      d1.assign( nd.GetData().begin(), nd.GetData().end() );
    for( std::size_t i=0; i<3; ++i )size1[i] = nd.GetDimMag(i);
  }
  else if( arg1->IsaWorkspace() )
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( wxT("MAX: argument 1 workspace is empty") );
    ndm1 = w1->GetNumberOfDimensions();
    if( ndm1 == 0 )
      val1 = w1->GetValue();
    else
      d1.assign( w1->GetData().begin(), w1->GetData().end() );
    for( std::size_t i=0; i<3; ++i )size1[i] = w1->GetDimMag(i);
  }
  else           // first argument must be a constant
  {
    ndm1 = 0;
    val1 = arg1->GetConstantValue();
  }
  int num1 = 0;
  if( ndm1 == 1 )
    num1 = size1[0];
  else if( ndm1==2 )
    num1 = size1[0]*size1[1];
  if( numberOfArguments_ == 1 )
  {
    if( num1 == 0 )
    {
      ws->SetValue( val1 );
    }
    else
    {
      ws->SetValue( d1[0] );
      for( int i=0; i<num1; ++i )
      {
        if( ws->GetValue() < d1[i] )ws->SetValue( d1[i] );
      }
    }
    ws->SetNumberOfDimensions(0);
    ws->SetDimMag( 0, 0 );
  }
  else
  {
    ExprCodes *arg2 = codes[1];
    std::vector<double> d2;
    int ndm2, size2[3];
    double val2;
    if( arg2->IsaNVariable() )
    {
      NumericData nd( arg2->GetNVarPtr()->GetData() );
      ndm2 = nd.GetNumberOfDimensions();
      if( ndm2 == 0 )
        val2 = nd.GetScalarValue();
      else
        d2.assign( nd.GetData().begin(), nd.GetData().end() );
      for( std::size_t i=0; i<3; ++i )size2[i] = nd.GetDimMag(i);
    }
    else if( arg2->IsaWorkspace() )
    {
      Workspace *w2 = arg2->GetWorkspacePtr();
      if( w2->IsEmpty() )
        throw EExpressionError( wxT("MAX: argument 2 workspace is empty") );
      ndm2 = w2->GetNumberOfDimensions();
      if( ndm2 == 0 )
        val2 = w2->GetValue();
      else
        d2.assign( w2->GetData().begin(), w2->GetData().end() );
      for( std::size_t i=0; i<3; ++i )size2[i] = w2->GetDimMag(i);
    }
    else           // second argument must be a constant
    {
      ndm2 = 0;
      val2 = arg2->GetConstantValue();
    }
    int num2 = 0;
    if( ndm2 == 0 )
    {
      if( ndm1 != 0 )
        throw EExpressionError( wxT("MAX: all function arguments must be the same type") );
    }
    else if( ndm2 == 1 )
    {
      if( ndm1 != 1 )
        throw EExpressionError( wxT("MAX: all function arguments must be the same type") );
      num2 = size2[0];
    }
    else if( ndm2 == 2 )
    {
      if( ndm1 != 2 )
        throw EExpressionError( wxT("MAX: all function arguments must be the same type") );
      num2 = size2[0]*size2[1];
    }
    if( num1 != num2 )
      throw EExpressionError( wxT("MAX: all function arguments must be the same size") );
    if( num1 == 0 )
    {
      ws->SetValue( std::max( val1, val2 ) );
      ws->SetNumberOfDimensions(0);
      ws->SetDimMag( 0, 0 );
    }
    else
    {
      for( int i=0; i<num1; ++i )ws->SetData( i, std::max(d1[i],d2[i]) );
      ws->SetNumberOfDimensions(1);
      ws->SetDimMag( 0, num1 );
    }
    int narg = 2;
    while( narg < numberOfArguments_ )
    {
      arg2 = codes[narg];
      if( arg2->IsaNVariable() )
      {
        NumericData nd( arg2->GetNVarPtr()->GetData() );
        ndm2 = nd.GetNumberOfDimensions();
        if( ndm2 == 0 )
          val2 = nd.GetScalarValue();
        else
          d2.assign( nd.GetData().begin(), nd.GetData().end() );
        for( std::size_t i=0; i<3; ++i )size2[i] = nd.GetDimMag(i);
      }
      else if( arg2->IsaWorkspace() )
      {
        Workspace *w2 = arg2->GetWorkspacePtr();
        if( w2->IsEmpty() )
          throw EExpressionError( wxT("MAX: argument workspace is empty") );
        ndm2 = w2->GetNumberOfDimensions();
        if( ndm2 == 0 )
          val2 = w2->GetValue();
        else
          d2.assign( w2->GetData().begin(), w2->GetData().end() );
        for( std::size_t i=0; i<3; ++i )size2[i] = w2->GetDimMag(i);
      }
      else           // first argument must be a constant
      {
        ndm2 = 0;
        val2 = arg2->GetConstantValue();
      }
      if( ndm2==0 )
      {
        if( ndm1 != 0 )
          throw EExpressionError( wxT("MAX: all function arguments must be the same type") );
        num2 = 0;
      }
      else if( ndm2 == 1 )
      {
        if( ndm1 != 1 )
          throw EExpressionError( wxT("MAX: all function arguments must be the same type") );
        num2 = size2[0];
      }
      else if( ndm2 == 2 )
      {
        if( ndm1 != 2 )
          throw EExpressionError( wxT("MAX: all function arguments must be the same type") );
        num2 = size2[0]*size2[1];
      }
      if( num1 != num2 )
        throw EExpressionError( wxT("MAX: all function arguments must be the same size") );
      if( num1 == 0 )
      {
        ws->SetValue( std::max(ws->GetValue(),val2) );
      }
      else
      {
        for( int i=0; i<num1; ++i )ws->SetData( i, std::max(ws->GetData(i),d2[i]) );
      }
      ++narg;
    }
  }
}

// end of file
