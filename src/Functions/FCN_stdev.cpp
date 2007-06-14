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
#include "FCN_stdev.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"

FCN_stdev *FCN_stdev::fcn_stdev_ = 0;

void FCN_stdev::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
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
 
void FCN_stdev::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1=codes[0], *arg2=0;
  std::vector<double> d1, d2;
  int ndm1, ndm2, size1, size2;
  //
  // if there are 2 arguements, the first is the weights vector and
  // the second is the data vector
  // if there is 1 argument, the weights are all 1.0 and the 
  // argument is the data vector
  //
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    ndm1 = nd.GetNumberOfDimensions();
    if( ndm1 != 1 )
    {
      if( numberOfArguments_ == 1 )
        throw EExpressionError( wxT("STDEV: argument must be a vector") );
      else
        throw EExpressionError( wxT("STDEV: argument 1 must be a vector") );
    }
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
    size1 = d1.size();
  }
  else if( arg1->IsaWorkspace() )
  {
    Workspace *w = arg1->GetWorkspacePtr();
    if( w->IsEmpty() )
    {
      if( numberOfArguments_ == 1 )
        throw EExpressionError( wxT("STDEV: argument workspace is empty") );
      else
        throw EExpressionError( wxT("STDEV: argument 1 workspace is empty") );
    }
    ndm1 = w->GetNumberOfDimensions();
    if( ndm1 != 1 )
    {
      if( numberOfArguments_ == 1 )
        throw EExpressionError( wxT("STDEV: argument must be a vector") );
      else
        throw EExpressionError( wxT("STDEV: argument 1 must be a vector") );
    }
    d1.assign( w->GetData().begin(), w->GetData().end() );
    size1 = d1.size();
  }
  else           // first argument must be a constant
  {
    if( numberOfArguments_ == 1 )
      throw EExpressionError( wxT("STDEV: argument must be a vector") );
    else
      throw EExpressionError( wxT("STDEV: argument 1 must be a vector") );
  }
  //
  if( numberOfArguments_ == 2 )
  {
    arg2 = codes[1];
    if( arg2->IsaNVariable() )
    {
      NumericData nd( arg2->GetNVarPtr()->GetData() );
      ndm2 = nd.GetNumberOfDimensions();
      if( ndm2 != 1 )
        throw EExpressionError( wxT("STDEV: argument 2 must be a vector") );
      d2.assign( nd.GetData().begin(), nd.GetData().end() );
      size2 = d2.size();
    }
    else if( arg2->IsaWorkspace() )
    {
      Workspace *w = arg2->GetWorkspacePtr();
      if( w->IsEmpty() )
        throw EExpressionError( wxT("STDEV: argument 2 workspace is empty") );
      ndm2 = w->GetNumberOfDimensions();
      if( ndm2 != 1 )
        throw EExpressionError( wxT("STDEV: argument 2 must be a vector") );
      d2.assign( w->GetData().begin(), w->GetData().end() );
      size2 = d1.size();
    }
    else           // argument must be a constant
      throw EExpressionError( wxT("STDEV: argument 2 must be a vector") );
  }
  double wsum=0.0, xsum=0.0, var=0.0, stdev=0.0;
  if( numberOfArguments_ == 2 )
  {
    // the first argument is the weights vector
    // the second argument is the data vector
    //
    if( size1 != size2 )
      throw EExpressionError( wxT("STDEV: arguments must have the same length") );
    for( int i=0; i<size1; ++i )
    {
      wsum += d1[i];
      xsum += d1[i]*d2[i];
    }
    if( wsum == 0.0 )
      throw EExpressionError( wxT("STDEV: sum of the weights = 0") );
    double xmean = xsum/wsum;
    for( int i=0; i<size1; ++i )var += d1[i]*(d2[i]-xmean)*(d2[i]-xmean);
  }
  else // the argument is the data vector
  {
    wsum = static_cast<double>(size1);
    for( int i=0; i<size1; ++i )xsum += d1[i];
    double xmean = xsum/wsum;
    for( int i=0; i<size1; ++i )var += (d1[i]-xmean)*(d1[i]-xmean);
  }
  var *= size1/(size1-1.0)/wsum;
  if( var >= 0.0 )stdev = sqrt(var);
  ws->SetValue( stdev );
  ws->SetNumberOfDimensions(0);
  ws->SetDimMag( 0, 0 );
}

// end of file
