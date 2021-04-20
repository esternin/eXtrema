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

#include "EVariableError.h"
#include "NumericData.h"

void NumericData::CopyStuff( NumericData const &rhs )
{
  data_.assign( rhs.data_.begin(), rhs.data_.end() );
  scalarType_ = rhs.scalarType_;
  for( std::size_t i=0; i<3; ++i )dimMag_[i] = rhs.dimMag_[i];
  numberOfDimensions_ = rhs.numberOfDimensions_;
  orderType_ = rhs.orderType_;
  lowLimit_ = rhs.lowLimit_;
  highLimit_ = rhs.highLimit_;
  lowLimitSet_ = rhs.lowLimitSet_;
  highLimitSet_ = rhs.highLimitSet_;

}

void NumericData::SetData( std::size_t i, double d )
{
  if( i < data_.size() )
  {
    data_[i] = d;
  }
  else
  {
    for( std::size_t j=data_.size(); j<i; ++j )data_.push_back( 0.0 );
    data_.push_back( d );
  }
}

void NumericData::GetMinMax( double &min, double &max ) const
{
  min = data_[0];
  max = min;
  std::size_t end = data_.size();
  for( std::size_t i=1; i<end; ++i )
  {
    if( min > data_[i] )min = data_[i];
    if( max < data_[i] )max = data_[i];
  }
}

std::ostream &operator<<( std::ostream &out, NumericData const &nd )
{
  if( nd.GetNumberOfDimensions() == 0 )
  {
    out << "<data size=\"0\">" << nd.GetScalarValue() << "</data>\n";
  }
  else
  {
    std::vector<double> data( const_cast<NumericData&>(nd).GetData() );
    std::size_t size = data.size();
    out << "<data size=\"" << size << "\">";
    for( std::size_t i=0; i<size-1; ++i )out << data[i] << " ";
    out << data[size-1] << "</data>\n";
  }
  return out;
}

// end of file
