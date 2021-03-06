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
#ifndef EX_WORKSPACE
#define EX_WORKSPACE

#include <vector>
#include <deque>

#include "wx/wx.h"

#include "NumericData.h"

class Expression;
class ExprCodes;

class Workspace
{
private:
  enum Type { UNKNOWN, AMBIGUOUS, NUMERIC, CHARACTER };
  enum EvaluationType { EVALUNKNOWN, SIMPLE, ARRAYFCN, ARRAYOP };
  
public:
  Workspace();
  Workspace( int );
  Workspace( Expression * );
  Workspace( wxString & );

  ~Workspace();

  Workspace( Workspace const & );
  Workspace &operator=( Workspace const & );
  bool operator==( Workspace const & ) const;
  //
  // data functions
  //
  NumericData &GetFinalData();
  Workspace *GetFinalWorkspace();
  
  bool IsEmpty();
  bool IsUnordered() const;
  bool IsAscending() const;
  bool IsDescending() const;
  void SetUnordered();
  void SetAscending();
  void SetDescending();
  std::size_t GetNumberOfDimensions() const;
  void SetNumberOfDimensions( std::size_t );
  unsigned int *GetDimMag();
  std::size_t GetDimMag( unsigned int ) const;
  void SetDimMag( unsigned int, unsigned int );
  void SetValue( double );
  double GetValue() const;
  void SetData( std::vector<double> & );
  void SetData( int, double );
  void SetData( unsigned int, unsigned int, double );
  void SetData( unsigned int, unsigned int, unsigned int, double );
  std::vector<double> &GetData();
  double GetData( unsigned int ) const;
  double GetData( unsigned int, unsigned int ) const;
  double GetData( unsigned int, unsigned int, unsigned int ) const;
  void GetMinMax( double &, double & ) const;

  Expression *GetExpression() const
  { return expression_; }

  void SetExpression( Expression *e )
  { expression_ = e; }

  std::deque<ExprCodes*> &GetCodes()
  { return codes_; }

  int GetLevel() const
  { return level_; }

  void SetLevel( int i )
  { level_ = i; }

  bool IsSimpleEvalType() const
  { return (evalType_ == SIMPLE); }

  bool IsArrayFcnEvalType() const
  { return (evalType_ == ARRAYFCN); }
  
  bool IsArrayOpEvalType() const
  { return (evalType_ == ARRAYOP); }
  
  void SetSimple()
  { evalType_ = SIMPLE; }
  
  void SetArrayFcn()
  { evalType_ = ARRAYFCN; }
  
  void SetArrayOp()
  { evalType_ = ARRAYOP; }
  
  bool GetNcFlag() const
  { return ncFlag_; }
  
  void SetNcFlag( bool f )
  { ncFlag_ = f; }
  
  std::size_t GetNumberOfLoops() const
  { return numberOfLoops_; }
  
  void SetNumberOfLoops( std::size_t i )
  { numberOfLoops_ = i; }
  
  Workspace *GetParent() const
  { return parent_; }
  
  void SetParent( Workspace *p )
  { parent_ = p; }
  
  void SetEvaluateNow( bool flag )
  { evaluateNow_ = flag; }
  
  bool EvaluateNow() const
  { return evaluateNow_; }
  
  void SetFinished()
  { processingFinished_ = true; }
  
  bool IsFinished() const
  { return processingFinished_; }
  
  void SetRPNdone()
  { rpnDone_ = true; }
  
  bool IsRPNdone() const
  { return rpnDone_; }
  
  bool CodesAreParsed() const
  { return codesAreParsed_; }
  
  void SetCodesAreParsed()
  { codesAreParsed_ = true; }
  
  void SetType( Type t )
  { type_ = t; }
  
  bool IsUnknown() const
  { return (type_ == UNKNOWN); }
  
  bool IsAmbiguous() const
  { return (type_ == AMBIGUOUS); }
  
  bool IsNumeric() const
  { return (type_ == NUMERIC); }
  
  bool IsCharacter() const
  { return (type_ == CHARACTER); }
  
  void SetNumeric()
  { type_ = NUMERIC; }
  
  void SetCharacter()
  { type_ = CHARACTER; }
  
  wxString &GetFinalString();
  void SetFinalString( wxString & );
  //
  // end of data functions
  //
  void SetType();
  void SetUp();
  void Calculate();
  void SimpleEval();
  void DetermineType();
  void ParseAndCheck( bool );
  void ParseCodes( bool );
  void SequencingCheck();
  void BracketCheck();
  void FinalCheck();
  bool HandleFunctionsAndText();
  void RPNconvert();
  void SetHierarchy();
  void SixthPass();
  void RPNTconvert();
  void MakeNewWorkspace( int, int, Type );
  void GetMinMax( double &, double & );

private:
  void CopyStuff( Workspace const & );
  void Initialize();
  //
  Expression *expression_;
  wxString string_;
  Type type_;
  int level_, highestNestingLevel_, numberOfLoops_;
  EvaluationType evalType_;
  std::deque<ExprCodes*> codes_;
  Workspace *parent_;
  bool ncFlag_, evalFlag_, evaluateNow_, processingFinished_, rpnDone_, codesAreParsed_;
  std::size_t operatorCntr_, functionWithArgumentsCntr_, indexedVariableCntr_;
  std::size_t rangeVectorCntr_, listVectorCntr_;
  std::vector<std::size_t> fcnPos_;   // used in RPNconvert and SetHierarchy
  std::vector<std::size_t> fcnArg_;   //  "
  std::vector<std::size_t> fcnIdx_;   //  "
  std::vector<std::size_t> oprPos_;   //  "
  std::vector<int> oprAnd_;           //  "
  std::vector<std::size_t> varPos_;   //  "
  std::vector<std::size_t> varIdx_;   //  "
  std::vector<std::size_t> rngPos_;
  std::vector<std::size_t> rngIdx_;
  std::vector<std::size_t> lstPos_;
  std::vector<std::size_t> lstIdx_;
  std::vector<int> hierarchy_;
  //
  NumericData finalData_;
  wxString finalString_;
};

#endif

