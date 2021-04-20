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

#include "OPR_innerProduct.h"

OPR_innerProduct OPR_innerProduct::opr_innerProduct_;

void OPR_innerProduct::ProcessOperand( bool firstOperand, int ndmEff,
                                       ExprCodes *code,
                                       std::deque<ExprCodes*> &oCode,
                                       int &nDim, int &nLoop )
{
  if( firstOperand )
  {
    if( ndmEff == 0 )
      throw EExpressionError( "inner product first operand must be an array" );
    firstOperandDimension_ = ndmEff;
    for( unsigned short int i=0; i<firstOperandDimension_; ++i )
      code->SetIndx( i, i );
  }
  else // second operand
  {
    if( ndmEff == 0 )
      throw EExpressionError( "inner product second operand must be an array" );
    nDim = ndmEff+firstOperandDimension_-2;
    nLoop = nDim+1;
    if( nDim > 3 )
      throw EExpressionError( "inner product result dimension > 3" );
    for( int i = 0; i < ndmEff; ++i )
      code->SetIndx( i, firstOperandDimension_+i );
    //
    // overwrite RPN codes to correct the array operand loop index
    //
    if( code->IsaNVariable() && code->GetIndexCntr() > 0 ) // indexed variable
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

void OPR_innerProduct::CalcOperand( bool firstOperand, int ndmEff,
                                    int *nptEff, Workspace *ws )
{
  if( firstOperand )
  {
    firstOperandDimension_ = ndmEff;
    for( unsigned short int j=0; j<firstOperandDimension_; ++j )
    {
      ws->SetDimMag( j, nptEff[j] );
    }
  }
  else
  {
    if( int(ws->GetDimMag( firstOperandDimension_-1 )) != nptEff[0] )
    {
      throw EExpressionError( "inner product dimension mismatch" );
    }
    std::deque<ExprCodes*> &cd( ws->GetCodes() );
    ExprCodes *c = cd[cd.size()-2];
    std::size_t ndim = 0;
    if( c->IsaNVariable() )
      ndim = c->GetNVarPtr()->GetData().GetNumberOfDimensions();
    else if( c->IsaWorkspace() )
      ndim = c->GetWorkspacePtr()->GetNumberOfDimensions();
    else
      throw EExpressionError( "inner product second operand problem" );
    for( std::size_t i=0; i<ndim; ++i )ws->SetDimMag( c->GetIndx(i), nptEff[i] );
  }
}

void OPR_innerProduct::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *op1 = codes[0];
  ExprCodes *op2 = codes[1];
  std::vector<double> d1, d2;
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
      throw EExpressionError( "inner product first operand workspace is empty" );
    d1.reserve( w1->GetData().size() );
    d1.insert( d1.begin(), w1->GetData().begin(), w1->GetData().end() );
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
      throw EExpressionError( "inner product second operand workspace is empty" );
    d2.assign( w2->GetData().begin(), w2->GetData().end() );
    ndm2 = w2->GetNumberOfDimensions();
    for( std::size_t i=0; i<ndm2; ++i )size2[i] = w2->GetDimMag(i);
  }
  if( ws->GetNumberOfLoops() == 1 )           // vector <> vector = scalar
  {
    ws->SetNumberOfDimensions(0);
    ws->SetDimMag( 0, 0 );
    double value = 0.0;                       // c = a[i]*b[i]
    for( std::size_t i=0; i<size1[0]; ++i )value += d1[i] * d2[i];
    ws->SetValue( value );
  }
  else if( ws->GetNumberOfLoops() == 2 )      // vector result
  {
    ws->SetNumberOfDimensions(1);
    if( ndm1 == 2 )                           // matrix <> vector
    {
      ws->SetDimMag( 0, size1[0] );
      for( std::size_t i=0; i<size1[0]; ++i )
      {
        double value = 0.0;                   // c[i] = a[i,j]*b[j]
        for( std::size_t j=0; j<size1[1]; ++j )value += d1[i+j*size1[0]]*d2[j];
        ws->SetData( i, value );
      }
    }
    else                                      // vector <> matrix
    {
      ws->SetDimMag( 0, size2[1] );
      for( std::size_t i=0; i<size2[1]; ++i )
      {
        double value = 0.0;                   // c[i] = a[j]*b[j,i]
        for( std::size_t j=0; j<size2[0]; ++j )value += d1[j]*d2[j+i*size2[0]];
        ws->SetData( i, value );
      }
    }
  }
  else if( ws->GetNumberOfLoops() == 3 )      // matrix result
  {
    ws->SetNumberOfDimensions(2);
    if( ndm1 == 1 )                           // vector <> tensor
    {
      for( std::size_t j = 0; j < ws->GetDimMag(2); ++j )
      {
        for( std::size_t i = 0; i < ws->GetDimMag(1); ++i )
        {
          double value = 0.0;                 // c[i,j] = a[k]*b[k,i,j]
          for( std::size_t k = 0; k < ws->GetDimMag(0); ++k )
            value += d1[j] * d2[k+i*size2[0]+j*size2[0]*size2[1]];
          ws->SetData( i+j*ws->GetDimMag(0), value );
        }
      }
    }
    else if( ndm1 == 2 )                      // matrix <> matrix
    {
      ws->SetDimMag( 0, size1[0] );
      ws->SetDimMag( 1, size2[1] );
      for( std::size_t j=0; j<size2[1]; ++j )
      {
        for( std::size_t i=0; i<size1[0]; ++i )
        {
          double value = 0.0;                 // c[i,j] = a[i,k]*b[k,j]
          for( std::size_t k=0; k<size2[0]; ++k )
            value += d1[i+k*size1[0]] * d2[k+j*size2[0]];
          ws->SetData( i+j*size1[0], value );
        }
      }
    }
    else                                      // tensor <> vector
    {
      for( std::size_t j = 0; j < ws->GetDimMag(1); ++j )
      {
        for( std::size_t i = 0; i < ws->GetDimMag(0); ++i )
        {
          double value = 0.0;                 // c[i,j] = a[i,j,k]*b[k]
          for( std::size_t k = 0; k < ws->GetDimMag(2); ++k )
            value += d1[i+j*size1[0]+k*size1[0]*size1[1]] * d2[k];
          ws->SetData( i+j*ws->GetDimMag(0), value );
        }
      }
    }
  }
  else if( ws->GetNumberOfLoops() == 4 )         // tensor result
  {
    ws->SetNumberOfDimensions(3);
    if( ndm1 == 2 )                           // matrix <> tensor
    {
      for( std::size_t k = 0; k < ws->GetDimMag(3); ++k )
      {
        for( std::size_t j = 0; j < ws->GetDimMag(2); ++j )
        {
          for( std::size_t i = 0; i < ws->GetDimMag(0); ++i )
          {
            double value = 0.0;               // c[i,j,k] = a[i,l]*b[l,j,k]
            for( std::size_t l = 0; l < ws->GetDimMag(1); ++l )
              value += d1[i+l*size1[0]] * d2[l+j*size2[0]+k*size2[0]*size2[1]];
            ws->SetData( i+j*ws->GetDimMag(0)+k*ws->GetDimMag(0)*ws->GetDimMag(1), value );
          }
        }
      }
    }
    else if( ndm1 == 3 )                      // tensor <> matrix
    {
      for( std::size_t k = 0; k < ws->GetDimMag(3); ++k )
      {
        for( std::size_t j = 0; j < ws->GetDimMag(1); ++j )
        {
          for( std::size_t i = 0; i < ws->GetDimMag(0); ++i )
          {
            double value = 0.0;               // c[i,j,k] = a[i,j,l]*b[l,k]
            for( std::size_t l = 0; l < ws->GetDimMag(2); ++l )
              value += d1[i+j*size1[0]+l*size1[0]*size1[1]] * d2[l+k*size2[0]];
            ws->SetData( i+j*ws->GetDimMag(0)+k*ws->GetDimMag(0)*ws->GetDimMag(1), value );
          }
        }
      }
    }
  }
}

// end of file
