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

#include "ExprCodes.h"
#include "Workspace.h"

#include "OPR_outerProduct.h"

OPR_outerProduct OPR_outerProduct::opr_outerProduct_;

void OPR_outerProduct::ProcessOperand( bool firstOperand, int ndmEff,
                                       ExprCodes *code,
                                       std::deque<ExprCodes*> &oCode,
                                       int &nDim, int &nLoop )
{
  if( firstOperand )
  {
    if( ndmEff == 0 )
      throw EExpressionError( "outer product first operand must be an array" );
    firstOperandDimension_ = ndmEff;
    for( unsigned short int i=0; i<firstOperandDimension_; ++i )code->SetIndx(i,i+1);
  }
  else
  {
    if( ndmEff == 0 )
      throw EExpressionError( "outer product second operand must be an array" );
    nDim = ndmEff + firstOperandDimension_;
    nLoop = nDim;
    if( nDim > 3 )
      throw EExpressionError( "outer product result dimension > 3" );
    for( int i = 0; i < ndmEff; ++i )
      code->SetIndx( i, firstOperandDimension_+i+1 );
    //
    // overwrite RPN codes to correct the array operand loop index
    //
    if( code->IsaNVariable() && code->GetIndexCntr() > 0 )
    {
      std::size_t nBack = 0;
      for( int i=ndmEff-1; i>=0; --i )
      {
        ExprCodes *ec = oCode[oCode.size()-nBack-1];
        ec->SetImmediateValue( oCode.back()->GetIndx(i) );
        nBack += 5;
      }
    }
  }
}

void OPR_outerProduct::CalcOperand( bool firstOperand, int ndmEff,
                                    int *nptEff, Workspace *ws )
{
  if( firstOperand )
  {
    firstOperandDimension_ = ndmEff;
    for( unsigned short int j=0; j<firstOperandDimension_; ++j )ws->SetDimMag(j,nptEff[j]);
  }
  else
  {
    std::deque<ExprCodes*> &cd( ws->GetCodes() );
    ExprCodes *c = cd[cd.size()-2];
    std::size_t ndim = 0;
    if( c->IsaNVariable() )
      ndim = c->GetNVarPtr()->GetData().GetNumberOfDimensions();
    else if( c->IsaWorkspace() )
      ndim = c->GetWorkspacePtr()->GetNumberOfDimensions();
    else
      throw EExpressionError( "outer product second operand problem" );
    for( std::size_t i=0; i<ndim; ++i )ws->SetDimMag(c->GetIndx(i)-1,nptEff[i]);
  }
}

void OPR_outerProduct::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *op1 = codes[0];
  ExprCodes *op2 = codes[1];
  vector<double> d1, d2;
  std::size_t ndm1, ndm2;
  std::size_t size1[3] = {0,0,0};
  std::size_t size2[3] = {0,0,0};
  if( op1->IsaNVariable() )
  {
    NumericData nd( op1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
    ndm1 = nd.GetNumberOfDimensions();
    for( std::size_t i=0; i<ndm1; ++i )size1[i] = nd.GetDimMag(i);
  }
  else     // first operand must be a workspace
  {
    Workspace *w1 = op1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( "outer product: first operand workspace is empty" );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
    ndm1 = w1->GetNumberOfDimensions();
    for( std::size_t i=0; i<ndm1; ++i )size1[i] = w1->GetDimMag(i);
  }
  if( op2->IsaNVariable() )
  {
    NumericData nd( op2->GetNVarPtr()->GetData() );
    d2.assign( nd.GetData().begin(), nd.GetData().end() );
    ndm2 = nd.GetNumberOfDimensions();
    for( std::size_t i=0; i<ndm2; ++i )size2[i] = nd.GetDimMag(i);
  }
  else    // second operand must be a workspace
  {
    Workspace *w2 = op2->GetWorkspacePtr();
    if( w2->IsEmpty() )
      throw EExpressionError( "outer product: second operand workspace is empty" );
    d2.assign( w2->GetData().begin(), w2->GetData().end() );
    ndm2 = w2->GetNumberOfDimensions();
    for( std::size_t i=0; i<ndm2; ++i )size2[i] = w2->GetDimMag(i);
  }
  std::size_t dm0 = ws->GetDimMag(0);
  std::size_t dm1 = ws->GetDimMag(1);
  std::size_t dm2 = ws->GetDimMag(2);
  if( ws->GetNumberOfLoops() == 2 )       // matrix result
  {                                       // c[i,j] = a[i]*b[j]
    ws->SetNumberOfDimensions(2);
    for( std::size_t i=0; i<dm0; ++i )
    {
      for( std::size_t j=0; j<dm1; ++j )
        ws->SetData( i+j*dm0, d1[i]*d2[j] );
    }
  }
  else if( ws->GetNumberOfLoops() == 3 )  // tensor result
  {
    ws->SetNumberOfDimensions(3);
    if( ndm1 == 2 )                       // matrix >< vector
    {                                     // c[i,j,k] = a[i,j]*b[k]
      for( std::size_t k=0; k<dm2; ++k )
      {
        for( std::size_t j=0; j<dm1; ++j )
        {
          for( std::size_t i=0; i<dm0; ++i )
            ws->SetData( i+j*dm0+k*dm0*dm1, d1[i+j*size1[0]]*d2[k] );
        }
      }
    }
    else if( ndm1 == 1 )                  // vector >< matrix
    {                                     // c[i,j,k] = a[i]<>b[j,k]
      for( std::size_t k=0; k<dm2; ++k )
      {
        for( std::size_t j=0; j<dm1; ++j )
        {
          for( std::size_t i=0; i<dm0; ++i )
            ws->SetData( i+j*dm0+k*dm0*dm1, d1[i]*d2[j+k*size1[0]] );
        }
      }
    }
  }
}

// end of file
