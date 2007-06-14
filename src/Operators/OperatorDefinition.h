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
#ifndef OPR_OPERATORDEFINITION
#define OPR_OPERATORDEFINITION

#include <deque>
#include <vector>

#include "wx/wx.h"

class Workspace;
class ExprCodes;

class OperatorDefinition
{
protected:
  enum Type { SCALAR, ARRAY, CHARACTER };

  OperatorDefinition( wxString const &, int, int, Type );
  
public:
  virtual ~OperatorDefinition()
  {}
  
  bool operator==( OperatorDefinition const &rhs ) const
  { return (name_ == rhs.name_); }

  bool operator!=( OperatorDefinition const &rhs ) const
  { return (name_ != rhs.name_); }

  wxString Name() const
  { return name_; }

  int UnaryPriority() const
  { return unaryPriority_; }
  
  int BinaryPriority() const
  { return binaryPriority_; }
  
  void OperandCounter( unsigned short int i )
  { operandCounter_ = i; }
  
  unsigned short int OperandCounter() const
  { return operandCounter_; }
  
  bool IsScalar() const
  { return (type_ == SCALAR); }
  
  bool IsCharacter() const
  { return (type_ == CHARACTER); }
  
  bool IsArray() const
  { return (type_ == ARRAY); }
  
  void SetArray()
  { type_ = ARRAY; }
  
  void SetCharacter()
  { type_ = CHARACTER; }

  virtual void ProcessOperand( bool, int, ExprCodes *,
                               std::deque<ExprCodes*> &,
                               int &, int & )
  {}
  
  virtual void CalcOperand( bool, int, int *, Workspace * )
  {}
  
  virtual void TextScalarEval( int, std::vector<wxString> & ) const
  {}
  
  virtual double ScalarEval( double, double ) const
  { return 0.0; }

  virtual double ScalarEval( double ) const
  { return 0.0; }
  
  virtual void ArrayEval( Workspace * )
  {}

private:
  //
  // singleton class:
  // cannot call default constructor, copy constructor, or operator=
  //
  OperatorDefinition()
  {}
  
  OperatorDefinition( OperatorDefinition const & )
  {}
  
  OperatorDefinition const &operator=( OperatorDefinition const & )
  { return *this; }

  wxString name_;
  int unaryPriority_, binaryPriority_;
  unsigned short int operandCounter_;
  Type type_;
};

#endif

