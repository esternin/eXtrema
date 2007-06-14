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
#include "FCN_join.h"
#include "Workspace.h"
#include "ExprCodes.h"
#include "NumericVariable.h"
#include "EExpressionError.h"

FCN_join *FCN_join::fcn_join_ = 0;

void FCN_join::ProcessArgument( bool firstArgument, int ndmEff, int &nLoop )
{
  if( firstArgument )
  {
    if( ndmEff != 1 )
      throw EExpressionError( wxT("JOIN: first argument must be a vector") );
    numberOfArguments_ = 1;
    nLoop = 1;
  }
  else
  {
    nLoop = ndmEff;
    ++numberOfArguments_;
    if( numberOfArguments_==2 && ndmEff!=1 )
      throw EExpressionError( wxT("JOIN: second argument must be a vector") );
  }
}

void FCN_join::ArrayEval( Workspace *ws )
{
  std::deque<ExprCodes*> &codes = ws->GetCodes();
  ExprCodes *arg1 = codes[0];
  ExprCodes *arg2 = codes[1];
  std::vector<double> d1, d2;
  int order1=0, order2=0;
  if( arg1->IsaNVariable() )
  {
    NumericData nd( arg1->GetNVarPtr()->GetData() );
    d1.assign( nd.GetData().begin(), nd.GetData().end() );
    if( nd.IsAscending() )order1 = -1;
    else if( nd.IsDescending() )order1 = 1;
  }
  else
  {
    Workspace *w1 = arg1->GetWorkspacePtr();
    if( w1->IsEmpty() )throw EExpressionError( wxT("JOIN: first argument workspace is empty") );
    d1.assign( w1->GetData().begin(), w1->GetData().end() );
    if( w1->IsAscending() )order1 = -1;
    else if( w1->IsDescending() )order1 = 1;
  }
  std::size_t size1 = d1.size();
  if( arg2->IsaNVariable() ) // arg2 is y
  {
    NumericData nd( arg2->GetNVarPtr()->GetData() );
    d2.assign( nd.GetData().begin(), nd.GetData().end() );
    if( nd.IsAscending() )order2 = -1;
    else if( nd.IsDescending() )order2 = 1;
  }
  else
  {
    Workspace *w2 = arg2->GetWorkspacePtr();
    if( w2->IsEmpty() )
      throw EExpressionError( wxT("JOIN: second argument workspace is empty") );
    d2.assign( w2->GetData().begin(), w2->GetData().end() );
    if( w2->IsAscending() )order2 = -1;
    else if( w2->IsDescending() )order2 = 1;
  }
  std::size_t size2 = d2.size();
  //
  std::size_t npt = std::max( size1, size2 );
  std::vector<std::size_t> work1(npt,0), work2(npt,0);
  //
  std::size_t ioff = 0;
  if( order1 == 0 )  // first not ordered
  {                  // if one is not ordered, cannot see how to allow duplicate entries
    if( order2 == -1 ) // second ascending
    {
      for( std::size_t k1=1; k1<=size1; ++k1 )
      {
        double xk1 = d1[k1-1];
        for( std::size_t k2=1; k2<=size2; ++k2 )
        {
          double xk2 = d2[k2-1];
          if( xk1 == xk2 )
          {
            ws->SetData( ioff++, xk1 );
            work1[ioff-1] = k1;
            work2[ioff-1] = k2;
            break;
          }
          else if( xk1 < xk2 )
          {
            break;
          }
        }
      }
    }
    else if( order2 == 1 ) // second descending
    {
      for( std::size_t k1=1; k1<=size1; ++k1 )
      {
        double xk1 = d1[k1-1];
        for( std::size_t k2=1; k2<=size2; ++k2 )
        {
          double xk2 = d2[k2-1];
          if( xk1 == xk2 )
          {
            ws->SetData( ioff++, xk1 );
            work1[ioff-1] = k1;
            work2[ioff-1] = k2;
            break;
          }
          else if( xk1 > xk2 )
          {
            break;
          }
        }
      }
    }
    else                   // second not ordered
    {
      for( std::size_t k1=1; k1<=size1; ++k1 )
      {
        for( std::size_t k2=1; k2<=size2; ++k2 )
        {
          if( d1[k1-1] == d2[k2-1] )
          {
            ws->SetData( ioff++, d1[k1-1] );
            work1[ioff-1] = k1;
            work2[ioff-1] = k2;
            break;
          }
        }
      }
    }
  }
  else if( order1 == -1 )  // first ascending order
  {
    if( order2 == -1 )     // second ascending
    {
      std::size_t k2 = 0;
      std::size_t k1 = 0;
      double xk1, xk2;
      L10:
      if( ++k2 > size2 )goto L12;
      xk2 = d2[k2-1];
      L11:
      if( ++k1 > size1 )goto L12;
      xk1 = d1[k1-1];
      if( xk1 < xk2 )
      {
        goto L11;
      }
      else if( xk1 == xk2 )
      {
        ws->SetData( ioff++, xk1 );
        work1[ioff-1] = k1;
        work2[ioff-1] = k2;
        if( k1 == size1 )
        {
          --k1;
        }
        else
        {
          if( xk1 != d1[k1] )--k1;
        }
        goto L10;
      }
      else
      {
        --k1;
        goto L10;
      }
 L12:                        // result ascending
      ;
    }
    else if( order2 == 1 )   // second descending
    {
      std::size_t k2 = size2+1;
      std::size_t k1 = 0;
      double xk1, xk2;
      L13:
      if( --k2 < 1 )goto L15;
      xk2 = d2[k2-1];
      L14:
      if( ++k1 > size1 )goto L15;
      xk1 = d1[k1-1];
      if( xk1 < xk2 )
      {
        goto L14;
      }
      else if( xk1 == xk2 )
      {
        ws->SetData( ioff++, xk1 );
        work1[ioff-1] = k1;
        work2[ioff-1] = k2;
        if( k1 == size1 )
        {
          --k1;
        }
        else
        {
          if( xk1 != d1[k1] )--k1;
        }
        goto L13;
      }
      else
      {
        --k1;
        goto L13;
      }
 L15:                 // result ascending
      ;
    }
    else              // second not ordered
    {
      for( std::size_t k2=1; k2<=size2; ++k2 )
      {
        double xk2 = d2[k2-1];
        for( std::size_t k1=1; k1<=size1; ++k1 )
        {
          double xk1 = d1[k1-1];
          if( xk1 == xk2 )
          {
            ws->SetData( ioff++, xk1 );
            work1[ioff-1] = k1;
            work2[ioff-1] = k2;
            break;
          }
          else if( xk1 > xk2 )
          {
            break;
          }
        }
      }
    }
  }
  else if( order1 == 1 )    // first descending
  {
    if( order2 == -1 )      // second ascending
    {
      std::size_t k2 = 0;
      std::size_t k1 = size1 + 1;
      double xk1, xk2;
      L18:
      if( ++k2 > size2 )goto L20;
      xk2 = d2[k2-1];
      L19:
      if( --k1 < 1 )goto L20;
      xk1 = d1[k1-1];
      if( xk1 < xk2 )
      {
        goto L19;
      }
      else if( xk1 == xk2 )
      {
        ws->SetData( ioff++, xk1 );
        work1[ioff-1] = k1;
        work2[ioff-1] = k2;
        if( k1 == 1 )
        {
          ++k1;
        }
        else
        {
          if( xk1 != d1[k1-2] )++k1;
        }
        goto L18;
      }
      else
      {
        ++k1;
        goto L18;
      }
 L20:
      ;
    }
    else if( order2 == 1 )  // second descending
    {
      std::size_t k2 = 0;
      std::size_t k1 = 0;
      double xk1, xk2;
      L21:
      if( ++k2 > size2 )goto L23;
      xk2 = d2[k2-1];
      L22:
      if( ++k1 > size1 )goto L23;
      xk1 = d1[k1-1];
      if( xk1 > xk2 )
      {
        goto L22;
      }
      else if( xk1 == xk2 )
      {
        ws->SetData( ioff++, xk1 );
        work1[ioff-1] = k1;
        work2[ioff-1] = k2;
        if( k1 == size1 )
        {
          --k1;
        }
        else
        {
          if( xk1 != d1[k1] )--k1;
        }
        goto L21;
      }
      else
      {
        --k1;
        goto L21;
      }
 L23:
      ;
    }
    else         // second not ordered
    {
      for( std::size_t k2=1; k2<=size2; ++k2 )
      {
        double xk2 = d2[k2-1];
        for( std::size_t k1=1; k1<=size1; ++k1 )
        {
          double xk1 = d1[k1-1];
          if( xk1 == xk2 )
          {
            ws->SetData( ioff++, xk1 );
            work1[ioff-1] = k1;
            work2[ioff-1] = k2;
            break;
          }
          else if( xk1 < xk2 )
          {
            break;
          }
        }
      }
    }
  }
  for( std::size_t k1=1; k1<=ioff; ++k1 )
  {
    ws->SetData( ioff+k1-1, static_cast<double>(work1[k1-1]) );
    ws->SetData( 2*ioff+k1-1, static_cast<double>(work2[k1-1]) );
  }
  ws->SetDimMag( 0, ioff );
  ws->SetDimMag( 1, 3 );
  ws->SetNumberOfDimensions(2);
}

// end of file
