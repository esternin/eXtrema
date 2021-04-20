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
#ifndef FCN_FUNCTIONDEFINITION
#define FCN_FUNCTIONDEFINITION

#define FUNCTIONS_LINKAGE_
/*
#if defined BUILDING_FUNCTIONS_DLL_
#define FUNCTIONS_LINKAGE_ __declspec(dllexport)
#else
#define FUNCTIONS_LINKAGE_ __declspec(dllimport)
#endif
*/

#include <vector>

#include "ExString.h"

class Workspace;

class FUNCTIONS_LINKAGE_ FunctionDefinition
{
protected:
  enum Type { NULLF, NUM2NUMSCALAR, NUM2NUMVECTOR, MIXED2NUM, MIXED2CHAR, CHAR2CHAR };
  enum ArgType { NUMERIC, CHARACTER };
  
  FunctionDefinition( char const *, std::size_t =1, std::size_t =1, Type =NUM2NUMSCALAR,
                      ArgType =NUMERIC, ArgType =NUMERIC, ArgType =NUMERIC,
                      ArgType =NUMERIC, ArgType =NUMERIC, ArgType =NUMERIC,
                      ArgType =NUMERIC, ArgType =NUMERIC, ArgType =NUMERIC,
                      ArgType =NUMERIC, ArgType =NUMERIC, ArgType =NUMERIC,
                      ArgType =NUMERIC, ArgType =NUMERIC, ArgType =NUMERIC,
                      ArgType =NUMERIC, ArgType =NUMERIC, ArgType =NUMERIC,
                      ArgType =NUMERIC, ArgType =NUMERIC );

public:
  virtual ~FunctionDefinition()
  {}

  bool operator==( FunctionDefinition const &rhs ) const
  { return (name_ == rhs.name_); }

  bool operator!=( FunctionDefinition const &rhs ) const
  { return (name_ != rhs.name_); }

  ExString Name() const
  { return name_; }

  void SetArgumentCounter( int i )
  { argumentCounter_ = i; }

  int GetArgumentCounter() const
  { return argumentCounter_; }

  std::size_t GetMinNumberOfArguments() const
  { return minimumNumberOfArguments_; }

  std::size_t GetMaxNumberOfArguments() const
  { return maximumNumberOfArguments_; }

  bool IsNull() const
  { return (type_==NULLF); }

  bool IsNum2NumScalar() const
  { return (type_==NUM2NUMSCALAR); }

  bool IsNum2NumVector() const
  { return (type_==NUM2NUMVECTOR); }

  bool IsMixed2Num() const
  { return (type_==MIXED2NUM); }

  bool IsMixed2Char() const
  { return (type_==MIXED2CHAR); }

  bool IsChar2Char() const
  { return (type_==CHAR2CHAR); }

  bool ArgTypeIsNumeric( int i ) const
  { return (argumentType_[i]==NUMERIC); }

  bool ArgTypeIsCharacter( int i ) const
  { return (argumentType_[i]==CHARACTER); }

  void Dump() const;
  virtual void ScalarEval( int, std::vector<double> & ) const {}
  virtual void TextScalarEval( int, std::vector<ExString> & ) const {}
  virtual void TextArrayEval( int, std::vector<Workspace*> &, std::vector<ExString> & ) const {}
  virtual void ProcessArgument( bool, int, int & ) {}
  virtual void CalcArgument( bool, int, int *, Workspace * ) {}
  virtual void ArrayEval( Workspace * ) {}

private:
  //
  // cannot call default constructor, the copy constructor, or operator=
  //
  FunctionDefinition()
  {}

  FunctionDefinition( FunctionDefinition const & )
  {}
  
  FunctionDefinition const &operator=( FunctionDefinition const & )
  { return *this; }
  
  ExString name_;
  int argumentCounter_;
  std::size_t minimumNumberOfArguments_;
  std::size_t maximumNumberOfArguments_;
  Type type_;
  ArgType argumentType_[20];
};

#endif

