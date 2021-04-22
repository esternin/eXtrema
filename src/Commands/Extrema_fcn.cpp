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

#include "Extrema_fcn.h"

#ifdef HAVE_MINUIT

#include "EExpressionError.h"
#include "NumericData.h"
#include "Expression.h"
  
double Extrema_fcn::up() const
{ return errorDef_; }

void Extrema_fcn::SetErrorDef( double def )
{ errorDef_ = def; }

void Extrema_fcn::SetChisqDirect( bool b )
{ chisqDirect_ = b; }

void Extrema_fcn::SetExpression( Expression *expr )
{ expression_ = expr; }

double Extrema_fcn::operator()( std::vector<double> const &p ) const
{
  // Update parameter values to current values
  //
  for( std::size_t i=0; i<p.size(); ++i )expression_->SetFitParameterValue( i, p[i] );
  expression_->FinalPass();
  NumericData result( expression_->GetFinalAnswer() );
  if( chisqDirect_ )
  {
    if( result.GetNumberOfDimensions() != 0 )
      throw EExpressionError( wxT("chi-square expression must result in a scalar value") );
    return result.GetScalarValue();
  }
  else
  {
    std::vector<double> temp( result.GetData() );
    std::size_t N = data_.size();
    double chi2 = 0.0;
    if( temp.size() == 1 )
    {
      for( std::size_t i=0; i<N; ++i )
        chi2 += weights_[i]*(data_[i]-temp[0])*(data_[i]-temp[0]);
    }
    else
    {
      for( std::size_t i=0; i<N; ++i )
        chi2 += weights_[i]*(data_[i]-temp[i])*(data_[i]-temp[i]);
    }
    return chi2;
  }
}
#endif

// end of file
