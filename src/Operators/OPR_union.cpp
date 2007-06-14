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

#include "OPR_union.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "ExprCodes.h"
#include "Workspace.h"

OPR_union *OPR_union::opr_union_ = 0;

void OPR_union::ProcessOperand( bool firstOperand, int ndmEff,
                                ExprCodes *code,
                                std::deque<ExprCodes*> &oCode,
                                int &nDim, int &nLoop )
{
  if( firstOperand )
  {
    if( ndmEff != 1 )
      throw EExpressionError( wxT("union first operand must be a vector") );
    code->SetIndx(0,0);
  }
  else
  {
    if( ndmEff != 1 )
      throw EExpressionError( wxT("union second operand must be a vector") );
    nDim = 1;
    nLoop = 1;
    code->SetIndx(0,1);
    //
    // overwrite RPN codes to correct the array operand loop index
    //
    if( code->IsaNVariable() && code->GetIndexCntr()>0 )
    {
      ExprCodes *ec = oCode[oCode.size()-1];
      ec->SetImmediateValue( oCode.back()->GetIndx(0) );
    }
  }
}

void OPR_union::CalcOperand( bool firstOperand, int ndmEff,
                             int *nptEff, Workspace *ws )
{
  if( firstOperand )
    ws->SetDimMag( 0, nptEff[0] );
  else
  {
    std::deque<ExprCodes*> &cd( ws->GetCodes() );
    ExprCodes *c = cd[cd.size()-2];
    std::size_t ndim;
    if( c->IsaNVariable() )
      ndim = c->GetNVarPtr()->GetData().GetNumberOfDimensions();
    else if( c->IsaWorkspace() )
      ndim = c->GetWorkspacePtr()->GetNumberOfDimensions();
    else
      throw EExpressionError( wxT("union second operand problem") );
    if( ndim != 1 )
      throw EExpressionError( wxT("union second operand must be a vector: 2") );
    ws->SetDimMag( c->GetIndx(0)-1, nptEff[0] );
  }
}

void OPR_union::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes( ws->GetCodes() );
  ExprCodes *op1 = codes[0];
  ExprCodes *op2 = codes[1];
  std::vector<double> d1, d2;
  std::size_t size1 = 0;
  std::size_t size2 = 0;
  unsigned short int o1 = 0;   // default is unordered
  unsigned short int o2 = 0;   //   "     "     "
  if( op1->IsaNVariable() )
  {
    NumericData nd( op1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
    size1 = nd.GetDimMag(0);
    if( nd.IsAscending() )      o1 = 2;
    else if( nd.IsDescending() )o1 = 1;
  }
  else
  {
    Workspace *w1 = op1->GetWorkspacePtr();
    if( w1->IsEmpty() )
      throw EExpressionError( wxT("union operator: first operand workspace is empty") );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
    size1 = w1->GetDimMag(0);
    if( w1->IsAscending() )      o1 = 2;
    else if( w1->IsDescending() )o1 = 1;
  }
  if( op2->IsaNVariable() )
  {
    NumericData nd( op2->GetNVarPtr()->GetData() );
    d2.assign( nd.GetData().begin(), nd.GetData().end() );
    size2 = nd.GetDimMag(0);
    if( nd.IsAscending() )      o2 = 2;
    else if( nd.IsDescending() )o2 = 1;
  }
  else
  {
    Workspace *w2 = op2->GetWorkspacePtr();
    if( w2->IsEmpty() )
      throw EExpressionError( wxT("union operator: second operand workspace is empty") );
    d2.assign( w2->GetData().begin(), w2->GetData().end() );
    size2 = w2->GetDimMag(0);
    if( w2->IsAscending() )      o2 = 2;
    else if( w2->IsDescending() )o2 = 1;
  }
  ws->SetNumberOfDimensions(1);
  std::size_t cntr = 0;
  if( o1 == 0 )         // first not ordered
  {
    if( o2 == 2 )      // second ascending
    {
      ws->SetData( d2 );
      cntr = size2;
      for( std::size_t i=0; i<size1; ++i )
      {
        std::size_t j;
        bool flag = true;
        for( j=0; j<size2; ++j )
        {
          if( d2[j] == d1[i] )
          {
            flag = false;
            break;
          }
          if( d2[j] > d1[i] )break;
        }
        if( flag )ws->SetData( cntr++, d1[i] );
      }
      ws->SetUnordered();
    }
    else if( o2 == 1 )  // second descending
    {
      ws->SetData( d2 );
      cntr = size2;
      for( std::size_t i=0; i<size1; ++i )
      {
        std::size_t j;
        bool flag = true;
        for( j=0; j<size2; ++j )
        {
          if( d2[j]==d1[i] )
          {
            flag = false;
            break;
          }
          if( d2[j] < d1[i] )break;
        }
        if( flag )ws->SetData( cntr++, d1[i] );
      }
      ws->SetUnordered();
    }
    else                        // second not ordered
    {
      ws->SetData( d1 );
      cntr = size1;
      for( std::size_t j=0; j<size2; ++j )
      {
        std::size_t i;
        bool flag = true;
        for( i=0; i<size1; ++i )
        {
          if( d1[i] == d2[j] )
          {
            flag = false;
            break;
          }
        }
        if( flag )ws->SetData( cntr++, d2[j] );
      }
      ws->SetUnordered();
    }
  }
  else if( o1 == 2 )   // first ascending order
  {
    if( o2 == 2 )      // second ascending
    {
      std::size_t j, i=0;
      for( j=0; j<size2; ++j )
      {
        if( i++ == size1 )break;
        for( ; i-1<size1; ++i )
        {
          if( d1[i-1] < d2[j] )
          {
            ws->SetData( cntr++, d1[i-1] );
          }
          else if( d1[i-1] == d2[j] )
          {
            ws->SetData( cntr++, d2[j] );
            break;
          }
          else
          {
            ws->SetData( cntr++, d2[j] );
            --i;
            break;
          }
        }
      }
      if( i < size1-1 )
      {
        for( std::size_t k=i; k<size1; ++k )ws->SetData( cntr++, d1[k] );
      }
      if( j < size2-1 )
      {
        for( std::size_t k=j; k<size2; ++k )ws->SetData( cntr++, d2[k] );
      }
      ws->SetAscending();
    }
    else if( o2 == 1 ) // descending order
    {
      std::size_t j, i=0;
      for( j=0; j<size2; ++j )
      {
        if( i++ == size1 )break;
        for( ; i-1<size1; ++j )
        {
          if( d1[i-1] < d2[size2-j-1] )
          {
            ws->SetData( cntr++, d1[i-1] );
          }
          else if( d1[i-1] == d2[size2-j-1] )
          {
            ws->SetData( cntr++, d2[size2-j-1] );
            break;
          }
          else
          {
            ws->SetData( cntr++, d2[size2-j-1] );
            --i;
            break;
          }
        }
      }
      if( i < size1 )
      {
        for( std::size_t k=i; k<size1; ++k )ws->SetData( cntr++, d1[k] );
      }
      if( j < size2 )
      {
        for( std::size_t k=j; k<size2; ++k )ws->SetData( cntr++, d2[size2-k-1] );
      }
      ws->SetAscending();
    }
    else                        // second not ordered
    {
      ws->SetData( d1 );
      cntr = size1;
      for( std::size_t j=0; j<size2; ++j )
      {
        std::size_t i;
        bool flag = true;
        for( i=0; i<size1; ++i )
        {
          if( d1[i] == d2[j] )
          {
            flag = false;
            break;
          }
          if( d1[i] > d2[j] )break;
        }
        if( flag )ws->SetData( cntr++, d2[j] );
      }
      ws->SetUnordered();
    }
  }
  else if( o1 == 1 )   // first descending order
  {
    if( o2 == 2 )      // second ascending
    {
      std::size_t j, i=0;
      for( j=0; j<size2; ++j )
      {
        if( i++ == size1 )break;
        for( ; i-1<size1; ++i )
        {
          if( d1[size1-i] < d2[j] )
          {
            ws->SetData( cntr++, d1[size1-i] );
          }
          else if( d1[size1-i] == d2[j] )
          {
            ws->SetData( cntr++, d2[j] );
            break;
          }
          else
          {
            ws->SetData( cntr++, d2[j] );
            --i;
            break;
          }
        }
      }
      if( i < size1 )
      {
        for( std::size_t k=i; k<size1; ++k )ws->SetData( cntr++, d1[size1-k-1] );
      }
      if( j < size2 )
      {
        for( std::size_t k=j; k<size2; ++k )ws->SetData( cntr++, d2[k] );
      }
      ws->SetAscending();
    }
    else if( o2 == 1 ) // second descending
    {
      std::size_t j, i=0;
      for( j=0; j<size2; ++j )
      {
        if( i++ == size1 )break;
        for( ; i-1<size1; ++i )
        {
          if( d1[i-1] > d2[j] )
          {
            ws->SetData( cntr++, d1[i-1] );
          }
          else if( d1[i-1] == d2[j] )
          {
            ws->SetData( cntr++, d2[j] );
            break;
          }
          else
          {
            ws->SetData( cntr++, d2[j] );
            --i;
            break;
          }
        }
      }
      if( i < size1 )
      {
        for( std::size_t k=i; k<size1; ++k )ws->SetData( cntr++, d1[k] );
      }
      if( j < size2 )
      {
        for( std::size_t k=j; k<size2; ++k )ws->SetData( cntr++, d2[k] );
      }
      ws->SetDescending();
    }
    else                        // second not ordered
    {
      for( std::size_t i=0; i<size1; ++i )ws->SetData( cntr++, d1[size1-i-1] );
      for( std::size_t j=0; j<size2; ++j )
      {
        std::size_t i;
        bool flag = true;
        for( i=0; i<size1; ++i )
        {
          if( d1[size1-i-1] == d2[j] )
          {
            flag = false;
            break;
          }
          if( d1[size1-i-1] > d2[j] )break;
        }
        if( flag )ws->SetData( cntr++, d2[j] );
      }
    }
  }
  ws->SetDimMag( 0, cntr );
}

// end of file
