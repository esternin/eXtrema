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
#include <cmath>

#include "wx/wx.h"

#include "CMD_filter.h"
#include "ECommandError.h"
#include "ParseToken.h"
#include "ParseLine.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "ExGlobals.h"

CMD_filter *CMD_filter::cmd_filter_ = 0;

CMD_filter::CMD_filter() : Command( wxT("FILTER") )
{
  AddQualifier( wxT("MEDIAN"), false );
  AddQualifier( wxT("MEAN"), false );
  AddQualifier( wxT("RECURSIVE"), false );
}

void CMD_filter::Execute( ParseLine const *p )
{
  // FILTER\MEAN  yin yout npt
  //
  // FILTER\MEDIAN  yin,yout  npt
  //      This command filters the data array YIN through a window
  //      "npt" points in width. The default filter is a running median
  //      filter; others may be specified using the appropriate modifier
  //      (e.g. \MEAN ).
  //
  // FILTER\-RECURSIVE  yin yout  cv
  //      This syntax allows for the specification of a general
  //      -RECURSIVE filter of arbitrary length by means of the list of
  //      filter coefficients cv={c1,c2,...cm}. The i'th output point of
  //      yout is the sum of the ck*yin(i-m/2+k-1) where k=1 to m. Thus,
  //      for odd m we apply the c's symmetrically about yin(i)
  //
  // FILTER\RECURSIVE  yin yout  cv dv
  //      This syntax allows for the specification of a general RECURSIVE
  //      filter of arbitrary length by means of the list of filter
  //      coefficients cv=[c1;c2;...;cm] and a list of secondary filter
  //      coefficients dv=[d1;d2;...;dn] which operate on the previously
  //      made output yout(i-k) where k=1 to n. Thus, given just d1 and
  //      d2, we apply d1 to yout(i-1) and d2 to yout(i-2) in generating
  //      yout(i).
  //
  QualifierMap qualifiers;
  try
  {
    SetUp( p, qualifiers );
  }
  catch (ECommandError &e)
  {
    throw;
  }
  wxString command( Name()+wxT(": ") );
  if( p->GetNumberOfTokens()<2 || !p->IsString(1) )
    throw ECommandError( command+wxT("expecting data vector") );
  std::vector<double> x;
  try
  {
    NumericVariable::GetVector( p->GetString(1), wxT("data vector"), x );
  }
  catch( EVariableError &e )
  {
    throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
  }
  AddToStackLine( p->GetString(1) );
  std::size_t numx = x.size();
  //
  if( p->GetNumberOfTokens()<3 || !p->IsString(2) )
    throw ECommandError( command+wxT("expecting output results vector") );
  wxString resultsName( p->GetString(2) );
  AddToStackLine( resultsName );
  //
  std::vector<double> yout( numx, 0.0 );
  if( qualifiers[wxT("MEAN")] || qualifiers[wxT("MEDIAN")] )
  {
    // perform a running filter operation of width nfiltc points
    //
    if( p->GetNumberOfTokens() < 4 )
      throw ECommandError( command+wxT("expecting filter width") );
    double width;
    try
    {
      NumericVariable::GetScalar( p->GetString(3), wxT("filter width"), width );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    std::size_t nfiltc = static_cast<std::size_t>(fabs(width)+0.5);
    if( nfiltc < 2 )
      throw ECommandError( command+wxT("filter cannot be applied to less than 2 points") );
    else if( nfiltc > numx )
      throw ECommandError( command+wxT("filter length > input data vector length") );
    AddToStackLine( p->GetString(3) );
    //
    // check the modifier setting for special filters, and either fill the
    // coefficient array cofc if this is a linear filter, or perform the
    // necessary sorting operations if this a median filter
    //
    if( qualifiers[wxT("MEAN")] )
    {
      if( width > 0.0 )
      {
        for( std::size_t j=1; j<=numx; ++j )
        {
          int jlo = j-nfiltc/2;
          int jup = j+(nfiltc-1)/2;
          if( jlo < 1 )
          {
            jlo = 1;
            jup = jlo + nfiltc - 1;
          }
          else if( jup > static_cast<int>(numx) )
          {
            jup = numx;
            jlo = jup - nfiltc + 1;
          }
          double yfilt = 0.0;
          for( int jj=jlo; jj<=jup; ++jj )yfilt += x[jj-1]/nfiltc;
          yout[j-1] = yfilt;
        }
      }
      else             // perform 'fast' running mean
      {
        double sum = 0.0;
        //
        // initialization summing
        //
        for( std::size_t i=1; i<=nfiltc; ++i )
        {
          std::size_t ii = i-nfiltc/2;
          ii = std::max(ii,static_cast<std::size_t>(1)); // set data below 1st point to it
          ii = std::min(ii,numx);  // set data above last point to it
          sum += x[ii-1];
        }
        yout[0] = sum/nfiltc;
        //
        // continue summing, adding/dropping a single point
        //
        for( std::size_t j=2; j<=numx; ++j )
        {
          std::size_t ii = j+(nfiltc-1)/2;
          ii = std::min(ii,numx);
          sum += x[ii-1];
          ii = j-nfiltc/2-1;
          ii = std::max(static_cast<std::size_t>(1),ii);
          //
          // subtract oldest point from sum
          //
          sum -= x[ii-1];
          yout[j-1] = sum/nfiltc;
        }
      }
    }
    else       // median filter
    {
      std::vector<double> temp( nfiltc, 0.0 );
      if( nfiltc/2*2 != nfiltc )  // odd number of points
      {
        std::size_t const k = nfiltc/2 + 1;
        for( std::size_t j=1; j<=numx; ++j )
        {
          int jlo = j-nfiltc/2;
          int jup = j+(nfiltc-1)/2;
          if( jlo < 1 )
          {
            jlo = 1;
            jup = jlo + nfiltc - 1;
          }
          else if( jup > static_cast<int>(numx) )
          {
            jup = numx;
            jlo = jup - nfiltc + 1;
          }
          for( std::size_t i=1; i<=nfiltc; ++i )temp[i-1] = x[jlo+i-2];
          yout[j-1] = Median( temp, k );
        }
      }
      else  // even number of points
      {
        // take the average of filter applied twice, once when data point is below
        // 'centre' and once when data point is above 'centre'
        //
        std::size_t k1 = nfiltc/2 + 1;
        std::size_t k2 = nfiltc/2;
        for( std::size_t j=1; j<=numx; ++j )
        {
          int jlo = j-nfiltc/2;
          int jup = j+(nfiltc-1)/2;
          if( jlo < 1 )
          {
            jlo = 1;
            jup = jlo + nfiltc - 1;
          }
          else if( jup > static_cast<int>(numx) )
          {
            jup = numx;
            jlo = jup - nfiltc + 1;
          }
          //
          // e.g. 4 point filter  -->v v v v
          //              . . .  x x x x x x x x x x x x x . . .
          //     point being acted upon  ^
          //     k1=3
          for( std::size_t i=1; i<=nfiltc; ++i )temp[i-1] = x[jlo+i-2];
          double answer1 = Median( temp, k1 );
          jlo = j-(nfiltc-1)/2;
          jup = j+nfiltc/2;
          if( jlo < 1 )
          {
            jlo = 1;
            jup = jlo + nfiltc - 1;
          }
          else if( jup > static_cast<int>(numx) )
          {
            jup = numx;
            jlo = jup - nfiltc + 1;
          }
          //
          // eg. 4 point filter  -->  v v v v
          //             . . .  x x x x x x x x x x x x x . . .
          //    point being acted upon  ^
          //    k2=2
          for( std::size_t i=1; i<=nfiltc; ++i )temp[i-1] = x[jlo+i-2];
          double answer2 = Median( temp, k2 );
          yout[j-1] = (answer1+answer2)/2.0;
        }
      }
    }
  }
  else  // not mean or median
  {
    if( p->GetNumberOfTokens()<4 || !p->IsString(3) )
      throw ECommandError( command+wxT("expecting data coefficients vector") );
    std::vector<double> cv;
    try
    {
      NumericVariable::GetVector( p->GetString(3), wxT("data coefficients vector"), cv );
    }
    catch( EVariableError &e )
    {
      throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
    }
    std::size_t nfiltc = cv.size();
    if( nfiltc > numx )
      throw ECommandError( command+wxT("coefficients vector length > input data vector length") );
    AddToStackLine( p->GetString(3) );
    //
    std::size_t nfiltd;
    std::vector<double> cv2;
    if( qualifiers[wxT("RECURSIVE")] )
    {
      if( p->GetNumberOfTokens()<5 || !p->IsString(4) )
        throw ECommandError( command+wxT("expecting output coefficients vector") );
      try
      {
        NumericVariable::GetVector( p->GetString(4), wxT("output coefficients vector"), cv2 );
      }
      catch( EVariableError &e )
      {
        throw ECommandError( command+wxString(e.what(),wxConvUTF8) );
      }
      nfiltd = cv2.size();
      AddToStackLine( p->GetString(3) );
    }
    //
    // this section performs the necessary linear filtering
    // operations. this includes most of the standard filters,
    // such as the mean, for which the output is a linear
    // combination of the input values. it does not, however,
    // include the median filter
    //
    if( !qualifiers[wxT("RECURSIVE")] )
    {
      for( std::size_t j=1; j<=numx; ++j )
      {
        int jlo = j - nfiltc/2;
        int jup = j + (nfiltc-1)/2;
        if( jlo < 1 )
        {
          jlo = 1;
          jup = jlo + nfiltc - 1;
        }
        else if( jup > static_cast<int>(numx) )
        {
          jup = numx;
          jlo = jup - nfiltc + 1;
        }
        double yfilt = 0.0;
        std::size_t ifilt = 1;
        for( int jj=jlo; jj<=jup; ++jj )
        {
          yfilt += cv[ifilt-1]*x[jj-1];
          ++ifilt;
        }
        yout[j-1] = yfilt;
      }
    }
    else   // secondary set of coefficients was entered
    {
      for( std::size_t j=1; j<=numx; ++j )
      {
        yout[j-1] = 0.0;
        int jlo = j-nfiltd;
        int jup = j-1;
        double yfiltd = 0.0;
        int ifilt = 1;
        for( int jj=jup; jj>=jlo; --jj ) // we assume yout[i-1]=0 for i<1
        {
          if( jj > 0 )yfiltd += cv2[ifilt-1]*yout[jj-1];
          ++ifilt;
        }
        jlo = j-nfiltc/2;
        jup = j+(nfiltc-1)/2;
        if( jlo < 1 )
        {
          jlo = 1;
          jup = jlo + nfiltc - 1;
        }
        else if( jup > static_cast<int>(numx) )
        {
          jup = numx;
          jlo = jup - nfiltc + 1;
        }
        double yfiltc = 0.0;
        ifilt = 1;
        for( int jj=jlo; jj<=jup; ++jj )
        {
          yfiltc += cv[ifilt-1]*x[jj-1];
          ++ifilt;
        }
        yout[j-1] = yfiltc + yfiltd;
      }
    }
  }
  NumericVariable::PutVariable( resultsName, yout, 0, p->GetInputLine() );
}

double  CMD_filter::Median( std::vector<double> &v, std::size_t k )
{
  // This routine finds the Kth ordered element of ARRAY size N.
  // Great for finding the median or any percentile value.
  //
  // The median of a vector is defined as the value which has the same
  // number of elements of lesser and greater value, or if the vector
  // has an even number of elements, then the average of the two elements
  // adjacent to the midpoint.
  //
  // To find the median of a vector of 100 elements you would
  // have to call this routine twice with values for K of 50 and 51
  // and then average the two numbers.
  //
  // The method was taken from Niklaus Wirth's 
  //	"ALGORITHMS + DATA STRUCTURES = PROGRAMS"
  //
  // The method is quite efficient. It works by partitioning the
  // array at the kth element and SWAPPING all elements
  // of smaller index and greater value with all elements of higher index
  // of lesser value.
  //
  // WARNING - The ARRAY passed is PARTIALLY SORTED by this routine.
  //
  std::size_t n = v.size();
  if( k > n )k = n;
  if( k < 1 )k = 1;
  std::size_t left = 1;
  std::size_t right = n;
  while( left < right )
  {
    double sentinal = v[k-1];
    std::size_t i = left;
    std::size_t j = right;
L150:
    while( v[i-1] < sentinal )++i;
    while( sentinal < v[j-1] )--j;
    if( i <= j )
    {
      double temp = v[i-1];
      v[i-1] = v[j-1];
      v[j-1] = temp;
      ++i; --j;
    }
    if( i <= j )goto L150;
    if( j < k )left = i;
    if( k < i )right = j;
  }
  return v[k-1];
}

 // end of file

