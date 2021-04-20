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
#ifndef EX_LISTVECTOR
#define EX_LISTVECTOR

#include <vector>

class NumericVariable;
class Workspace;

class ListVector
{
private:
  enum ItemType { NUMERICVARIABLE, WORKSPACE, CONSTANT };
  enum UseType { REAL, INT };
  
public:
  ListVector()
      : dimension_(1), numberOfDimensions_(0), numberOfItems_(0), useType_(REAL)
  {}

  ~ListVector()
  {}
  
  ListVector( ListVector const &rhs )
  { CopyStuff( rhs ); }
  
  ListVector &operator=( ListVector const &rhs )
  {
    if( &rhs != this )CopyStuff( rhs );
    return *this;
  }

  void SetItemCntr( std::size_t n )
  { numberOfItems_ = n; }
  
  std::size_t GetItemCntr() const
  { return numberOfItems_; }
  
  void SetNumberOfDimensions( std::size_t i )
  { numberOfDimensions_ = i; }
  
  std::size_t GetNumberOfDimensions() const
  { return numberOfDimensions_; }
  
  void SetDimension( std::size_t i )
  { dimension_ = i; }
  
  std::size_t GetDimension() const
  { return dimension_; }

  void SetItem( NumericVariable const *nv )
  {
    nvItems_.push_back( nv );
    wsItems_.push_back( 0 );
    constantItems_.push_back( 0.0 );
    itemTypes_.push_back( NUMERICVARIABLE );
  }
  
  void SetItem( Workspace const *ws )
  {
    nvItems_.push_back( 0 );
    wsItems_.push_back( ws );
    constantItems_.push_back( 0.0 );
    itemTypes_.push_back( WORKSPACE );
  }
  
  void SetItem( double d )
  {
    nvItems_.push_back( 0 );
    wsItems_.push_back( 0 );
    constantItems_.push_back( d );
    itemTypes_.push_back( CONSTANT );
  }
  
  NumericVariable const *GetNVItem( std::size_t i )
  { return nvItems_[i]; }

  Workspace const *GetWSItem( std::size_t i )
  { return wsItems_[i]; }
  
  double GetConstantItem( std::size_t i ) const
  { return constantItems_[i]; }
  
  bool ItemIsaNVariable( std::size_t i ) const
  { return (itemTypes_[i] == NUMERICVARIABLE); }
  
  bool ItemIsaWorkspace( std::size_t i ) const
  { return (itemTypes_[i] == WORKSPACE); }
  
  bool ItemIsaConstant( std::size_t i ) const
  { return (itemTypes_[i] == CONSTANT); }

  double GetItem( std::size_t const i ) const;

  std::size_t GetNpt() const;
  
  void SetReal()
  { useType_ = REAL; }
  
  void SetInt()
  { useType_ = INT; }
  
  bool IsReal() const
  { return (useType_==REAL); }
  
  bool IsInt() const
  { return (useType_==INT); }
  
private:
  std::size_t numberOfItems_;
  std::size_t dimension_;
  std::size_t numberOfDimensions_;
  UseType useType_;
  std::vector<ItemType> itemTypes_;
  std::vector<NumericVariable const *> nvItems_;
  std::vector<Workspace const *> wsItems_;
  std::vector<double> constantItems_;
  
  void CopyStuff( ListVector const & );
};

#endif

