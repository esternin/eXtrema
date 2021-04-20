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

#include "EExpressionError.h"
#include "ExprCodes.h"
#include "Workspace.h"
#include "NumericVariable.h"

#include "OPR_concatenation.h"

OPR_concatenation OPR_concatenation::opr_concatenation_;

void OPR_concatenation::ProcessOperand( bool firstOperand, int ndmEff,
                                        ExprCodes *code,
                                        std::deque<ExprCodes*> &oCode,
                                        int &nDim, int &nLoop )
{
  if( firstOperand )
  {
    if( ndmEff == 1 )
    {
      firstOperandDimension_ = 1;
      code->SetIndx( 0, 1 );
    }
    else if( ndmEff == 0 )
    {
      firstOperandDimension_ = 0;
      code->SetIndx( 0, 1 );
    }
    else
    {
      throw EExpressionError(
        "concatenation first operand must be a vector or scalar" );
    }
  }
  else
  {
    if( ndmEff!=0 && ndmEff!=1 )
    {
      throw EExpressionError(
        "concatenation second operand must be a vector or scalar" );
    }
    nDim = 1;
    nLoop = 2;
    code->SetIndx( 0, 1 );
    //
    // overwrite RPN codes to correct the array operand loop index
    //
    if( code->IsaNVariable() && code->GetIndexCntr()>0 && ndmEff>0 )
      oCode[oCode.size()-2]->SetImmediateValue(2);
  }
}

void OPR_concatenation::CalcOperand( bool firstOperand, int ndmEff,
                                     int *nptEff, Workspace *ws )
{
  if( firstOperand )
  {
    firstOperandDimension_ = ndmEff;
    ws->SetDimMag( 0, nptEff[0] );
  }
  else
  {
    std::deque<ExprCodes*> &cd( ws->GetCodes() );
    ExprCodes *c = cd[cd.size()-2];
    size_t ndim = 0;
    if( c->IsaNVariable() )
    {
      ndim = c->GetNVarPtr()->GetData().GetNumberOfDimensions();
    }
    else if( c->IsaWorkspace() )
    {
      ndim = c->GetWorkspacePtr()->GetNumberOfDimensions();
    }
    else if( c->IsaConstant() )
    {
      ndim = 0;
    }
    else
    {
      throw EExpressionError( "concatenation second operand problem" );
    }
    size_t size = 0;
    for( size_t i=0; i<ndim; ++i )size += nptEff[0];
    ws->SetDimMag( c->GetIndx(0)-1, size );
  }
}

void OPR_concatenation::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *op1 = codes[0];
  ExprCodes *op2 = codes[1];
  vector<double> d1, d2;
  size_t size1 = 0;
  size_t size2 = 0;
  if( op1->IsaNVariable() )
  {
    NumericData nd( op1->GetNVarPtr()->GetData() );
    if( nd.GetNumberOfDimensions() == 1 )
    {
      d1.assign( nd.GetData().begin(), nd.GetData().end() );
      size1 = nd.GetDimMag(0);
    }
    else
    {
      d1.push_back( nd.GetScalarValue() );
      size1 = 1;
    }
  }
  else if( op1->IsaWorkspace() )
  {
    Workspace *w1 = op1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError(
        "concatenation: first operand workspace is empty" );
    if( w1->GetNumberOfDimensions() == 1 )
    {
      d1.assign( w1->GetData().begin(), w1->GetData().end() );
      size1 = w1->GetDimMag(0);
    }
    else
    {
      d1.push_back( w1->GetValue() );
      size1 = 1;
    }
  }
  else if( op1->IsaConstant() )
  {
    d1.push_back( op1->GetConstantValue() );
    size1 = 1;
  }
  if( op2->IsaNVariable() )
  {
    NumericData nd( op2->GetNVarPtr()->GetData() );
    if( nd.GetNumberOfDimensions() == 1 )
    {
      d2.assign( nd.GetData().begin(), nd.GetData().end() );
      size2 = nd.GetDimMag(0);
    }
    else
    {
      d2.push_back( nd.GetScalarValue() );
      size2 = 1;
    }
  }
  else if( op2->IsaWorkspace() )
  {
    Workspace *w2 = op2->GetWorkspacePtr();
    if( w2->IsEmpty() )
      throw EExpressionError( "concatenation: second operand workspace is empty" );
    if( w2->GetNumberOfDimensions() == 1 )
    {
      d2.reserve(w2->GetData().size());
      d2.insert( d2.begin(), w2->GetData().begin(), w2->GetData().end() );
      size2 = w2->GetDimMag(0);
    }
    else
    {
      d2.push_back( w2->GetValue() );
      size2 = 1;
    }
  }
  else if( op2->IsaConstant() )
  {
    d2.push_back( op2->GetConstantValue() );
    size2 = 1;
  }
  ws->SetNumberOfDimensions(1);
  size_t cntr = 0;
  for( size_t i=0; i<size1; ++i )ws->SetData( cntr++, d1[i] );
  for( size_t i=0; i<size2; ++i )ws->SetData( cntr++, d2[i] );
  ws->SetDimMag(0,size1+size2);
}

// end of file
