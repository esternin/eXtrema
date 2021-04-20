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
#ifndef EX_EXPRCODES
#define EX_EXPRCODES

#include "ExString.h"

class Workspace;
class ListVector;
class RangeVector;
class FunctionDefinition;
class OperatorDefinition;
class NumericVariable;
class Workspace;
class TextVariable;

class ExprCodes
{
private:
  enum TokenType { UNKNOWN, FUNCTION, OPERATOR, NVARIABLE, TVARIABLE, CONSTANT, NOOP,
                   WORKSPACE, IMMEDIATEVALUE, RANGEVECTOR, LISTVECTOR,
                   TSTRING, OPENINGBRACKET, CLOSINGBRACKET, PAIRBRACKET,
                   COMMA, COLON, SEMICOLON, HASH, ASTERISK, MLOOP };

  enum OperatorType { UNDETERMINED, BINARY, UNARY };

public:
  ExprCodes()
      : nVariablePtr_(0),
        tVariablePtr_(0), functionPtr_(0), operatorPtr_(0),
        textArrayIndexPtr_(0), textCharacterIndexPtr_(0),
        workspacePtr_(0), listVectorPtr_(0), rangeVectorPtr_(0),
        operatorType_(UNDETERMINED), objectCntr_(0), argumentCntr_(0),
        indexCntr_(0), nestingLevel_(0), immediateValue_(0),
        insertedTextVar_(false), valid_(true),
        isAnIndex_(false), constantValue_(0.0), followingTerm_(7),
        tokenType_(UNKNOWN), referenceType_(UNKNOWN), functionIsDone_(false)
  {
    lindex_[0] = 0;
    lindex_[1] = 0;
    lindex_[2] = 0;
  }
  
  ~ExprCodes();

  ExprCodes( ExprCodes const &rhs )
  { CopyStuff( rhs ); }
  
  ExprCodes &operator=( ExprCodes const &rhs )
  {
    if( &rhs != this )CopyStuff( rhs );
    return *this;
  }

  bool operator==( ExprCodes const &rhs ) const
  {
    if( tokenType_ != rhs.tokenType_ )return false;
    return true;
  }
  
  bool operator!=( ExprCodes const &rhs ) const
  {
    if( tokenType_ == rhs.tokenType_ )return false;
    return true;
  }
  
  void SetIndx( int i, int j )
  { lindex_[i] = j; }
  
  int GetIndx( int i ) const
  { return lindex_[i]; }
  
  void SetNoop()
  { tokenType_ = NOOP; }
  
  void SetMloop()
  { tokenType_ = MLOOP; }
  
  void SetRangeVector()
  { tokenType_ = RANGEVECTOR; }
  
  void SetListVector()
  { tokenType_ = LISTVECTOR; }
  
  void SetOpeningBracket()
  {
    tokenType_ = OPENINGBRACKET;
    followingTerm_ = 5;
  }
  
  void SetClosingBracket()
  {
    tokenType_ = CLOSINGBRACKET;
    followingTerm_ = 4;
  }
  
  void SetPairBracket()
  {
    tokenType_ = PAIRBRACKET;
    followingTerm_ = 14;
  }
  
  void SetComma()
  {
    tokenType_ = COMMA;
    followingTerm_ = 6;
  }
  
  void SetColon()
  {
    tokenType_ = COLON;
    followingTerm_ = 9;
  }
  
  void SetSemicolon()
  {
    tokenType_ = SEMICOLON;
    followingTerm_ = 12;
  }
  
  void SetHash()
  {
    tokenType_ = HASH;
    followingTerm_ = 10;
  }
  
  void SetAsterisk()
  {
    tokenType_ = ASTERISK;
    followingTerm_ = 11;
  }
  
  bool IsaFunction() const
  { return (tokenType_ == FUNCTION); }
  
  bool IsaOperator() const
  { return (tokenType_ == OPERATOR); }
  
  bool IsaNVariable() const
  { return (tokenType_ == NVARIABLE); }
  
  bool IsaTVariable() const
  { return (tokenType_ == TVARIABLE); }
  
  bool IsaConstant() const
  { return (tokenType_ == CONSTANT); }
  
  bool IsaNoop() const
  { return (tokenType_ == NOOP); }
  
  bool IsaMloop() const
  { return (tokenType_ == MLOOP); }
  
  bool IsaWorkspace() const
  { return (tokenType_ == WORKSPACE); }
  
  bool IsaImmediateValue() const
  { return (tokenType_ == IMMEDIATEVALUE); }
  
  bool IsaRangeVector() const
  { return (tokenType_ == RANGEVECTOR); }
  
  bool IsaListVector() const
  { return (tokenType_ == LISTVECTOR); }
  
  bool IsaTString() const
  { return (tokenType_ == TSTRING); }
  
  bool IsaOpeningBracket() const
  { return (tokenType_ == OPENINGBRACKET); }
  
  bool IsaClosingBracket() const
  { return (tokenType_ == CLOSINGBRACKET); }
  
  bool IsaPairBracket() const
  { return (tokenType_ == PAIRBRACKET); }
  
  bool IsaComma() const
  { return (tokenType_ == COMMA); }
  
  bool IsaColon() const
  { return (tokenType_ == COLON); }
  
  bool IsaSemicolon() const
  { return (tokenType_ == SEMICOLON); }
  
  bool IsaHash() const
  { return (tokenType_ == HASH); }
  
  bool IsaAsterisk() const
  { return (tokenType_ == ASTERISK); }
  
  void SetReferenceType( TokenType t )
  { referenceType_ = t; }
  
  void SetRefFunction()
  { referenceType_ = FUNCTION; }
  
  void SetRefOperator()
  { referenceType_ = OPERATOR; }
  
  void SetRefNVariable()
  { referenceType_ = NVARIABLE; }
  
  void SetRefRangeVector()
  { referenceType_ = RANGEVECTOR; }
  
  void SetRefListVector()
  { referenceType_ = LISTVECTOR; }
  
  TokenType GetReferenceType() const
  { return referenceType_; }
  
  bool IsFlaggedByFunction() const
  { return (referenceType_ == FUNCTION); }
  
  bool IsFlaggedByOperator() const
  { return (referenceType_ == OPERATOR); }
  
  bool IsFlaggedByNVariable() const
  { return (referenceType_ == NVARIABLE); }
  
  bool IsFlaggedByRangeVector() const
  { return (referenceType_ == RANGEVECTOR); }
  
  bool IsFlaggedByListVector() const
  { return (referenceType_ == LISTVECTOR); }
  
  NumericVariable *GetNVarPtr() const
  { return nVariablePtr_; }
  
  void SetNVarPtr( NumericVariable *p )
  {
    nVariablePtr_ = p;
    tokenType_ = NVARIABLE;
    followingTerm_ = 2;
  }
  
  ListVector *GetListVectorPtr() const
  { return listVectorPtr_; }
  
  void SetListVectorPtr( ListVector *p )
  { listVectorPtr_ = p; }
  
  RangeVector *GetRangeVectorPtr() const
  { return rangeVectorPtr_; }
  
  void SetRangeVectorPtr( RangeVector *p )
  { rangeVectorPtr_ = p; }
  
  Workspace *GetWorkspacePtr() const
  { return workspacePtr_; }
  
  void SetWorkspacePtr( Workspace *p )
  {
    tokenType_ = WORKSPACE;
    workspacePtr_ = p;
  }
  
  TextVariable *GetTVarPtr() const
  { return tVariablePtr_; }
  
  void SetTVarPtr( TextVariable *p )
  {
    tokenType_ = TVARIABLE;
    tVariablePtr_ = p;
    followingTerm_ = 2;
  }
  
  FunctionDefinition *GetFcnPtr() const
  { return functionPtr_; }
  
  void SetFcnPtr( FunctionDefinition *p )
  {
    tokenType_ = FUNCTION;
    functionPtr_ = p;
    followingTerm_ = 1;
  }
  
  OperatorDefinition *GetOpPtr()
  { return operatorPtr_; }
  
  void SetOpPtr( OperatorDefinition *p )
  {
    tokenType_ = OPERATOR;
    operatorPtr_ = p;
    followingTerm_ = 8;
  }
  
  ExString GetConstantString() const
  { return constantString_; }
  
  double GetConstantValue() const
  { return constantValue_; }
  
  void SetConstantValue( double v, ExString s )
  {
    tokenType_ = CONSTANT;
    constantValue_ = v;
    constantString_ = ExString(s.c_str());
    followingTerm_ = 3;
  }
  
  ExString GetTString() const
  { return ExString(tString_); }
  
  void SetTString( ExString s )
  {
    tokenType_ = TSTRING;
    tString_ = s;
    followingTerm_ = 13;
  }
  
  bool GetInsertedTVar() const
  { return insertedTextVar_; }
  
  void SetInsertedTVar( bool f )
  { insertedTextVar_ = f; }
  
  Workspace *GetTAIndexPtr() const
  { return textArrayIndexPtr_; }
  
  void SetTAIndexPtr( Workspace *p )
  { textArrayIndexPtr_ = p; }
  
  Workspace *GetTCIndexPtr() const
  { return textCharacterIndexPtr_; }
  
  void SetTCIndexPtr( Workspace *p )
  { textCharacterIndexPtr_ = p; }
  
  int GetImmediateValue() const
  { return immediateValue_; }
  
  void SetImmediateValue( int v )
  {
    tokenType_ = IMMEDIATEVALUE;
    immediateValue_ = v;
  }
  
  bool IsBinary() const
  { return (operatorType_ == BINARY); }
  
  bool IsUnary() const
  { return (operatorType_ == UNARY); }
  
  void SetBinary()
  { operatorType_ = BINARY; }
  
  void SetUnary()
  { operatorType_ = UNARY; }
  
  int GetCntr() const
  { return objectCntr_; }
  
  void SetCntr( int rc )
  { objectCntr_ = rc; }
  
  int GetArgCntr() const
  { return argumentCntr_; }
  
  void SetArgCntr( int i )
  { argumentCntr_ = i; }
  
  unsigned int GetIndexCntr() const
  { return indexCntr_; }
  
  void SetIndexCntr( unsigned int i )
  { indexCntr_ = i; }
  
  int GetNestingLevel() const
  { return nestingLevel_; }
  
  void SetNestingLevel( int i )
  { nestingLevel_ = i; }
  
  bool GetValid() const
  { return valid_; }
  
  void SetValid( bool value )
  { valid_ = value; }
  
  bool IsaIndex() const
  { return isAnIndex_; }
  
  void SetIsAnIndex( bool value )
  { isAnIndex_ = value; }
  
  int GetFollowingTerm()
  { return followingTerm_; }
  
  bool GetFunctionIsDone() const
  { return functionIsDone_; }
  
  void SetFunctionIsDone()
  { functionIsDone_ = true; }

private:
  ExString tString_;
  NumericVariable *nVariablePtr_;
  //
  // say a text variable should also use the text array index pointer and
  // the text character index pointer
  //
  TextVariable *tVariablePtr_;
  FunctionDefinition *functionPtr_;
  OperatorDefinition *operatorPtr_;
  Workspace *workspacePtr_;
  ListVector *listVectorPtr_;
  RangeVector *rangeVectorPtr_;
  int immediateValue_;
  double constantValue_;
  ExString constantString_;
  TokenType tokenType_;
  TokenType referenceType_;
  OperatorType operatorType_;
  Workspace *textArrayIndexPtr_;
  Workspace *textCharacterIndexPtr_;
  int objectCntr_;
  int argumentCntr_;
  unsigned int indexCntr_;
  int nestingLevel_;
  bool insertedTextVar_;
  bool valid_;
  bool isAnIndex_;
  int lindex_[3];
  int followingTerm_;
  bool functionIsDone_;

  void CopyStuff( ExprCodes const & );
};

#endif
