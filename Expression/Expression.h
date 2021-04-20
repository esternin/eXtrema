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
#ifndef EX_EXPRESSION
#define EX_EXPRESSION

// expression evaluator:  it converts the arithmetic expression
// into an integerized code which is in Reverse Polish Notation (RPN)
// The expression consists of functions, variables, strings, and
// (binary and unary) operators. The expression also consists of numbers,
// parentheses, or commas. Numbers may have exponents denoted by E or D.
// ([{ are treated as equivalent and )]} are treated as equivalent.

#include <map>
#include <vector>
#include <string>

#include "ExString.h"
#include "NVariableTable.h"
#include "NumericData.h"
#include "NumericVariable.h"

class Workspace;
class ExprCodes;
class ListVector;
class RangeVector;

class Expression
{
private:
  typedef std::multimap< int const, Workspace* > WSLevel;
  typedef WSLevel::value_type WSEntryType;
  typedef std::pair< WSLevel::const_iterator, WSLevel::const_iterator > WSLevelPair;
  typedef std::pair< int const, Workspace* > WSLevelValuePair;
  
  enum Type { NUMERIC, CHARACTER };
  
public:
  Expression( ExString const & );
  ~Expression();
  
  Expression( Expression const &rhs )
  { CopyStuff( rhs ); }
  
  Expression & operator=( Expression const &rhs )
  {
    if( &rhs != this )CopyStuff( rhs );
    return *this;
  }

  ExString GetString() const
  { return string_; }
  
  bool IsNumeric() const
  { return (type_ == NUMERIC); }
  
  bool IsCharacter() const
  { return (type_ == CHARACTER); }
  
  bool IsaFit() const
  { return isaFit_; }
  
  void SetIsaFit()
  { isaFit_ = true; }
  
  std::size_t GetNumberOfFitParameters() const
  { return fitParameters_.size(); }

  std::vector<ExString> &GetFitParameterNames()
  {
    return fitParameters_;
  }
  
  ExString GetFitParameterName( std::size_t i ) const
  { return fitParameters_[i]; }
  
  NumericData &GetFinalAnswer()
  { return finalData_; }
  
  ExString &GetFinalString()
  { return finalString_; }
  
  void SetHighestLevel( int i )
  { highestLevel_ = i; }
  
  int GetHighestLevel() const
  { return highestLevel_; }

  void Evaluate();
  
  void TestFitParameterName( ExString & );
  
  double GetFitParameterValue( std::size_t i ) const
  { return NVariableTable::GetTable()->
        GetVariable(fitParameters_[i])->GetData().GetScalarValue(); }
  
  void SetFitParameterValue( std::size_t i, double v )
  { NVariableTable::GetTable()->
        GetVariable(fitParameters_[i])->GetData().SetScalarValue( v ); }
  
  void SetFitParameterValue( std::size_t i, double v, ExString const &s )
  {
    NumericVariable *nv =
        NVariableTable::GetTable()->GetVariable( fitParameters_[i] );
    nv->GetData().SetScalarValue( v );
    nv->AddToHistory( s );
  }
  
  void GetWorkspaces( int, std::vector<Workspace*> & );
  
  void AddWorkspace( Workspace * );
  void RemoveWorkspace( Workspace * );

  Workspace *GetFinalWorkspace()
  { return (*levels_.find(0)).second; }
  
  void SeventhPass( int );
  void FinalPass();
  void CalcText( Workspace * );
  
private:
  ExString string_;
  Type type_;

  WSLevel levels_;
  int highestLevel_;  // number of levels = highestLevel+1

  NumericData finalData_;
  ExString finalString_;

  bool isaFit_;
  std::vector<ExString> fitParameters_;

  void CopyStuff( Expression const & );

};

#endif

