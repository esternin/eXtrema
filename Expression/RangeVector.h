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
#ifndef EX_RANGEVECTOR
#define EX_RANGEVECTOR

class RangeVector
{
private:
  enum UseType { REAL, INT };

public:
  RangeVector()
      : numberOfItems_(0), dimension_(1), useType_(REAL)
  {
    values_[0] = 0.0;
    values_[1] = 0.0;
    values_[2] = 1.0;
  }
  
  RangeVector( RangeVector const &rhs )
  { CopyStuff( rhs ); }
  
  RangeVector & operator=( RangeVector const &rhs )
  {
    if( &rhs != this )CopyStuff( rhs );
    return *this;
  }

  double GetItem( std::size_t i ) const
  {
    double result = values_[0] + i*values_[2];
    if( useType_ == INT )result = static_cast<double>(static_cast<int>(result));
    return result;
  }
  
  void SetItemCntr( std::size_t i )
  { numberOfItems_ = i; }
  
  std::size_t GetItemCntr() const
  { return numberOfItems_; }
  
  void SetValue( int i, double v )
  { values_[i] = v; }
  
  int GetNpt() const
  {
    double tmp = (values_[1]-values_[0])/values_[2];
    return static_cast<int>(tmp+0.499999)+1;
  }
  
  double GetStart() const
  { return values_[0]; }
  
  double GetEnd() const
  { return values_[1]; }
  
  double GetInc() const
  { return values_[2]; }
  
  void SetDimension( int i )
  { dimension_ = i; }

  int GetDimension() const
  { return dimension_; }
  
  bool IsReal() const
  { return useType_==REAL; }
  
  bool IsInt() const
  { return useType_==INT; }
  
  void SetReal()
  { useType_ = REAL; }

  void SetInt()
  { useType_ = INT; }

private:
  std::size_t numberOfItems_;
  double values_[3];
  int dimension_;
  UseType useType_;

  void CopyStuff( RangeVector const &rhs )
  {
    numberOfItems_ = rhs.numberOfItems_;
    values_[0] = rhs.values_[0];
    values_[1] = rhs.values_[1];
    values_[2] = rhs.values_[2];
    dimension_ = rhs.dimension_;
    useType_ = rhs.useType_;
  }
};

#endif

