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
#include <memory>

#include "ExprCodes.h"
#include "RangeVector.h"
#include "ListVector.h"
#include "NumericVariable.h"
#include "TextVariable.h"

ExprCodes::~ExprCodes()
{
  switch (tokenType_)
  {
    case RANGEVECTOR:
      if( rangeVectorPtr_ != 0 )delete rangeVectorPtr_;
      break;
    case LISTVECTOR:
      if( listVectorPtr_ != 0 )delete listVectorPtr_;
      break;
  }
}

void ExprCodes::CopyStuff( ExprCodes const &rhs )
{
  if( listVectorPtr_ )
  {
    delete listVectorPtr_;
    listVectorPtr_ = 0;
  }
  if( rangeVectorPtr_ )
  {
    delete rangeVectorPtr_;
    rangeVectorPtr_ = 0;
  }
  if( rhs.tokenType_ == LISTVECTOR )
  {
    std::unique_ptr<ListVector> s( new ListVector(*rhs.listVectorPtr_) );
    listVectorPtr_ = s.release();
  }
  else if( rhs.tokenType_ == RANGEVECTOR )
  {
    std::unique_ptr<RangeVector> s( new RangeVector(*rhs.rangeVectorPtr_) );
    rangeVectorPtr_ = s.release();
  }
  tokenType_ = rhs.tokenType_;
  //
  // the following pointers should not be deleted here
  //
  nVariablePtr_ = rhs.nVariablePtr_;
  tVariablePtr_ = rhs.tVariablePtr_;
  functionPtr_ = rhs.functionPtr_;
  operatorPtr_ = rhs.operatorPtr_;
  textArrayIndexPtr_ = rhs.textArrayIndexPtr_;
  textCharacterIndexPtr_ = rhs.textCharacterIndexPtr_;
  workspacePtr_ = rhs.workspacePtr_;
  //
  tString_ = rhs.tString_;
  referenceType_ = rhs.referenceType_;
  operatorType_ = rhs.operatorType_;
  objectCntr_ = rhs.objectCntr_;
  argumentCntr_ = rhs.argumentCntr_;
  indexCntr_ = rhs.indexCntr_;
  nestingLevel_ = rhs.nestingLevel_;
  immediateValue_ = rhs.immediateValue_;
  insertedTextVar_ = rhs.insertedTextVar_;
  valid_ = rhs.valid_;
  isAnIndex_ = rhs.isAnIndex_;
  constantValue_ = rhs.constantValue_;
  constantString_ = rhs.constantString_;
  followingTerm_ = rhs.followingTerm_;
  //
  functionIsDone_ = rhs.functionIsDone_;
  //
  for( std::size_t i=0; i<3; ++i )lindex_[i] = rhs.lindex_[i];
}
  
void ExprCodes::SetConstantValue( double v, wxString s )
{
  tokenType_ = CONSTANT;
  constantValue_ = v;
  constantString_ = s;
  followingTerm_ = 3;
}

wxString ExprCodes::GetTString() const
{ return tString_; }

// end of file
