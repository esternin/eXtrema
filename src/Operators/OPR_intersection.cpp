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

#include "OPR_intersection.h"
#include "NumericVariable.h"
#include "EExpressionError.h"
#include "ExprCodes.h"
#include "Workspace.h"

OPR_intersection *OPR_intersection::opr_intersection_ = 0;

void OPR_intersection::ProcessOperand( bool firstOperand, int ndmEff,
                                       ExprCodes *code,
                                       std::deque<ExprCodes*> &oCode,
                                       int &nDim, int &nLoop )
{
  if( firstOperand )
  {
    if( ndmEff != 1 )
      throw EExpressionError( wxT("intersection first operand must be a vector") );
    code->SetIndx(0,0);
  }
  else
  {
    if( ndmEff != 1 )
      throw EExpressionError( wxT("intersection second operand must be a vector") );
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

void OPR_intersection::CalcOperand( bool firstOperand, int ndmEff,
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
      throw EExpressionError( wxT("intersection second operand problem") );
    if( ndim != 1 )
      throw EExpressionError( wxT("intersection second operand must be a vector: 2") );
    ws->SetDimMag( c->GetIndx(0)-1, nptEff[0] );
  }
}

void OPR_intersection::ArrayEval( Workspace *ws )
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
      throw EExpressionError( wxT("intersection operator: first operand workspace is empty") );
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
      throw EExpressionError( wxT("intersection operator: second operand workspace is empty") );
    d2.assign( w2->GetData().begin(), w2->GetData().end() );
    size2 = w2->GetDimMag(0);
    if( w2->IsAscending() )      o2 = 2;
    else if( w2->IsDescending() )o2 = 1;
  }
  ws->SetNumberOfDimensions(1);
  std::size_t cntr = 0;
  if( o1 == 0 )     // first not ordered
  {
    if( o2 == 2 )  // second ascending
    {
      for( std::size_t i=0; i<size1; ++i )
      {
        for( std::size_t j=0; j<size2; ++j )
        {
          if( d1[i] == d2[j] )
          {
            ws->SetData(cntr++,d1[i]);
            break;
          }
          else if( d1[i] < d2[j] )
          {
            break;
          }
        }
      }
      ws->SetUnordered();
    }
    else if( o2 == 1 ) // second descending
    {
      for( std::size_t i=0; i<size1; ++i )
      {
        for( std::size_t j=0; j<size2; ++j )
        {
          if( d1[i] == d2[j] )
          {
            ws->SetData(cntr++,d1[i]);
            break;
          }
          else if( d1[i] > d2[j] )
          {
            break;
          }
        }
      }
      ws->SetUnordered();
    }
    else                        // second not ordered
    {
      for( std::size_t i=0; i<size1; ++i )
      {
        for( std::size_t j=0; j<size2; ++j )
        {
          if( d1[i] == d2[j] )
          {
            ws->SetData(cntr++,d1[i]);
            break;
          }
        }
      }
      ws->SetUnordered();
    }
  }
  else if( o1 == 2 )  // first ascending order
  {
    if( o2 == 2 )     // second ascending
    {
      std::size_t i = 0;
      for( std::size_t j=0; j<size2; ++j )
      {
        while( i<size1 && d1[i]<d2[j] )++i;
        if( i == size1 )break;
        if( d1[i] == d2[j] )ws->SetData(cntr++,d1[i]);
      }
      ws->SetAscending();
    }
    else if( o2 == 1 ) // second descending
    {
      std::size_t i = 0;
      for( std::size_t jj=0; jj<size2; ++jj )
      {
        std::size_t j = size2-jj-1;
        while( i<size1 && d1[i]<d2[j] )++i;
        if( i == size1 )break;
        if( d1[i] == d2[j] )ws->SetData(cntr++,d1[i]);
      }
      ws->SetAscending();
    }
    else                        // second not ordered
    {
      for( std::size_t j=0; j<size2; ++j )
      {
        for( std::size_t i=0; i<size1; ++i )
        {
          if( d1[i] == d2[j] )
          {
            ws->SetData( cntr++, d1[i] );
            break;
          }
          else if( d1[i] > d2[j] )
          {
            break;
          }
        }
      }
      ws->SetUnordered();
    }
  }
  else if( o1 == 1 )   // first descending
  {
    if( o2 == 2 )      // second ascending
    {
      std::size_t i = 0;
      for( std::size_t j=0; j<size2; ++j )
      {
        while( i<size1 && d1[size1-i-1]<d2[j] )++i;
        if( i == size1 )break;
        if( d1[size1-i-1] == d2[j] )ws->SetData(cntr++,d1[size1-i-1]);
      }
      ws->SetAscending();
    }
    else if( o2 == 1 ) // second descending
    {
      std::size_t i = 0;
      for( std::size_t j=0; j<size2; ++j )
      {
        while( i<size1 && d1[i]>d2[j] )++i;
        if( i == size1 )break;
        if( d1[i] == d2[j] )ws->SetData(cntr++,d1[i]);
      }
      ws->SetDescending();
    }
    else                        // second not ordered
    {
      for( std::size_t j=0; j<size2; ++j )
      {
        for( std::size_t i=0; i<size1; ++i )
        {
          if( d1[i] == d2[j] )
          {
            ws->SetData( cntr++, d1[i] );
            break;
          }
          else if( d1[i] < d2[j] )
          {
            break;
          }
        }
      }
      ws->SetUnordered();
    }
  }
  if( cntr == 0 )throw EExpressionError( wxT("intersection is empty") );
  ws->SetDimMag( 0, cntr );
}

// end of file
