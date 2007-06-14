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
#include "ListVector.h"
#include "Workspace.h"
#include "NumericVariable.h"

void ListVector::CopyStuff( ListVector const &rhs )
{
  dimension_ = rhs.dimension_;
  numberOfDimensions_ = rhs.numberOfDimensions_;
  numberOfItems_ = rhs.numberOfItems_;
  useType_ = rhs.useType_;
  itemTypes_.assign( rhs.itemTypes_.begin(), rhs.itemTypes_.end() );
  nvItems_.assign( rhs.nvItems_.begin(), rhs.nvItems_.end() );
  wsItems_.assign( rhs.wsItems_.begin(), rhs.wsItems_.end() );
  constantItems_.assign( rhs.constantItems_.begin(), rhs.constantItems_.end() );
}

std::size_t ListVector::GetNpt() const
{
  std::size_t npt = 0;
  switch (numberOfDimensions_)
  {
    case 1:
      npt = numberOfItems_;
      break;
    case 2:
      itemTypes_[0]==NUMERICVARIABLE ? npt=const_cast<NumericVariable*>(nvItems_[0])->GetData().GetDimMag(0) :
                                       npt=const_cast<Workspace*>(wsItems_[0])->GetDimMag(0);
      break;
    case 3:
      itemTypes_[0]==NUMERICVARIABLE ?
        npt = const_cast<NumericVariable*>(nvItems_[0])->GetData().GetDimMag(0)*
              const_cast<NumericVariable*>(nvItems_[0])->GetData().GetDimMag(1) :
        npt = const_cast<Workspace*>(wsItems_[0])->GetDimMag(0)*
              const_cast<Workspace*>(wsItems_[0])->GetDimMag(1);
  }
  return npt;
}

double ListVector::GetItem( std::size_t const i ) const
{
  double result;
  switch (itemTypes_[i] )
  {
    case NUMERICVARIABLE:
      result = const_cast<NumericVariable*>(nvItems_[i])->GetData().GetScalarValue();
      break;
    case WORKSPACE:
      result = wsItems_[i]->GetValue();
      break;
    default:
      result = constantItems_[i];
  }
  if( useType_ == INT )result = static_cast<double>(static_cast<int>(result));
  return result;
}

// end of file
