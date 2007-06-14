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
#ifndef EX_NUMERICDATA
#define EX_NUMERICDATA

#include <vector>
#include <ostream>

class NumericData
{
private:
  enum ScalarType { NOTSCALAR, FIXED, FIT };

public:
  NumericData()
      : orderType_(0), numberOfDimensions_(0), lowLimitSet_(false), highLimitSet_(false)
  {
    dimMag_[0] = 0;
    dimMag_[1] = 0;
    dimMag_[2] = 0;
  }

  NumericData( unsigned int i )
      : orderType_(0), numberOfDimensions_(i), lowLimitSet_(false), highLimitSet_(false)
  {
    dimMag_[0] = 0;
    dimMag_[1] = 0;
    dimMag_[2] = 0;
  }

  virtual ~NumericData()
  {}

  NumericData( NumericData const &rhs )
  { CopyStuff( rhs ); }

  NumericData & operator=( NumericData const &rhs )
  {
    if( this != &rhs )CopyStuff( rhs );
    return *this;
  }

  bool IsEmpty() const
  { return data_.empty(); }

  void SetNumberOfDimensions( unsigned int i )
  { numberOfDimensions_ = i; }

  unsigned int GetNumberOfDimensions() const
  { return numberOfDimensions_; }

  bool IsFixed() const
  { return (scalarType_==FIXED); }

  bool IsFit() const
  { return (scalarType_==FIT); }

  void SetFixed()
  { scalarType_ = FIXED; }

  void SetFit()
  { scalarType_ = FIT; }
  
  void SetLowLimit( double x )
  {
    lowLimit_ = x;
    lowLimitSet_ = true;
  }
  
  void ReleaseLowLimit()
  { lowLimitSet_ = false; }

  bool LowLimitSet() const
  { return lowLimitSet_; }
  
  double GetLowLimit() const
  { return lowLimit_; }
  
  void SetHighLimit( double x )
  {
    highLimit_ = x;
    highLimitSet_ = true;
  }
  
  void ReleaseHighLimit()
  { highLimitSet_ = false; }

  bool HighLimitSet() const
  { return highLimitSet_; }
  
  double GetHighLimit() const
  { return highLimit_; }
  
  void SetOrderType( int i )
  { orderType_ = i%3; }

  bool IsUnordered() const
  { return (orderType_ == 0); }

  bool IsAscending() const
  { return (orderType_ == 1); }

  bool IsDescending() const
  { return (orderType_ == 2); }

  void SetUnordered()
  { orderType_ = 0; }

  void SetAscending()
  { orderType_ = 1; }

  void SetDescending()
  { orderType_ = 2; }

  double GetScalarValue() const
  { return data_[0]; }

  void SetScalarValue( double v )
  {
    std::vector<double>().swap( data_ );
    data_.push_back( v );
  }

  unsigned int *GetDimMag()
  { return dimMag_; }

  unsigned int GetDimMag( unsigned int i ) const
  { return dimMag_[i]; }

  void SetDimMag( unsigned int i, unsigned int v )
  { dimMag_[i] = v; }

  std::vector<double> &GetData()
  { return data_; }

  void SetData( std::vector<double> const &d )
  { data_.assign( d.begin(), d.end() ); }

  void SetData( std::size_t, double );
  void SetData( std::size_t, std::size_t, double );
  void SetData( std::size_t, std::size_t, std::size_t, double );

  double GetData( std::size_t i ) const
  { return data_[i]; }

  double GetData( std::size_t i, std::size_t j ) const
  { return data_[i+j*dimMag_[0]]; }

  double GetData( std::size_t i, std::size_t j, std::size_t k ) const
  { return data_[i+j*dimMag_[0]+k*dimMag_[0]*dimMag_[1]]; }

  void GetMinMax( double &, double & ) const;

  friend std::ostream &operator<<( std::ostream &, NumericData const & );

private:
  std::vector<double> data_;
  ScalarType scalarType_;
  double lowLimit_, highLimit_;
  bool lowLimitSet_, highLimitSet_;
  unsigned int dimMag_[3];
  unsigned int numberOfDimensions_;
  unsigned int orderType_;

  void CopyStuff( NumericData const & );
};

#endif
