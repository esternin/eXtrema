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
#include <vector>

#include "OPR_reflect.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "ExprCodes.h"
#include "Workspace.h"

OPR_reflect *OPR_reflect::opr_reflect_ = 0;

void OPR_reflect::ProcessOperand( bool firstOperand, int ndmEff,
                                  ExprCodes *code,
                                  std::deque<ExprCodes*> &oCode,
                                  int &nDim, int &nLoop )
{
  if( firstOperand )
  {
    if( ndmEff != 2 )
      throw EExpressionError( wxT("reflect operand must be a matrix") );
    code->SetIndx( 0, 0 );
    code->SetIndx( 1, 1 );
  }
}

void OPR_reflect::CalcOperand( bool firstOperand, int ndmEff,
                               int *nptEff, Workspace *ws )
{
  if( firstOperand )
  {
    ws->SetDimMag( 0, nptEff[0] );
    ws->SetDimMag( 1, nptEff[1] );
  }
}

void OPR_reflect::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *op1 = codes[0];
  std::vector<double> d1;
  std::size_t ndm1;
  std::size_t size1[3] = {0,0,0};
  if( op1->IsaNVariable() )
  {
    NumericData nd( op1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
    ndm1 = nd.GetNumberOfDimensions();
    for( std::size_t i=0; i<ndm1; ++i )size1[i] = nd.GetDimMag(i);
  }
  else
  {
    Workspace *w1 = op1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( wxT("reflect operator: operand workspace is empty") );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
    ndm1 = w1->GetNumberOfDimensions();
    for( std::size_t i=0; i<ndm1; ++i )size1[i] = w1->GetDimMag(i);
  }
  //
  // c[i,j] = a[i,#-j+1]
  //
  ws->SetNumberOfDimensions(2);
  std::size_t dm0 = ws->GetDimMag(0);
  std::size_t dm1 = ws->GetDimMag(1);
  for( std::size_t j=0; j<dm1; ++j )
  {
    for( std::size_t i=0; i<dm0; ++i )
      ws->SetData( i+(size1[1]-j-1)*dm0, d1[i+j*size1[0]] );
  }
}

// end of file
