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
#include <limits>

#include "Workspace.h"
#include "EExpressionError.h"
#include "ExprCodes.h"
#include "ListVector.h"
#include "RangeVector.h"
#include "Expression.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "FunctionDefinition.h"
#include "FunctionTable.h"
#include "OperatorDefinition.h"
#include "OperatorTable.h"
#include "ToAscii.h"

Workspace::Workspace()
    : level_(0), expression_(0), string_( wxT("") )
{ Initialize(); }

Workspace::Workspace( int i )
    : level_(i), expression_(0), string_( wxT("") )
{ Initialize(); }

Workspace::Workspace( Expression *e )
    : level_(-1), expression_(e), string_(e->GetString())
{ Initialize(); }

Workspace::Workspace( wxString &s )
    : level_(-1), expression_(0), string_(s)
{ Initialize(); }

Workspace::Workspace( Workspace const &rhs )
{ CopyStuff(rhs); }

Workspace &Workspace::operator=( Workspace const &rhs )
{
  if( &rhs != this )CopyStuff(rhs);
  return *this;
}

void Workspace::Initialize()
{
  evalType_ = EVALUNKNOWN;
  ncFlag_ = true;
  parent_ = NULL;
  numberOfLoops_ = 0;
  evalFlag_ = false;
  type_ = UNKNOWN;
  evaluateNow_ = true;
  processingFinished_ = false;
  rpnDone_ = false;
  codesAreParsed_ = false;
  operatorCntr_ = 0;
  functionWithArgumentsCntr_ = 0;
  indexedVariableCntr_ = 0;
  rangeVectorCntr_ = 0;
  listVectorCntr_ = 0;
  highestNestingLevel_ = 0;
}

Workspace::~Workspace()
{
  while ( !codes_.empty() )
  {
    delete codes_.back();
    codes_.pop_back();
  }
}

void Workspace::CopyStuff( Workspace const &rhs )
{
  while ( !codes_.empty() )
  {
    delete codes_.back();
    codes_.pop_back();
  }
  std::deque<ExprCodes*>::const_iterator rhsEnd( rhs.codes_.end() );
  for( std::deque<ExprCodes*>::const_iterator i(rhs.codes_.begin()); i!=rhsEnd; ++i )
  {
    ExprCodes *e = new ExprCodes( **i );
    codes_.push_back( e );
  }
  evalType_ = rhs.evalType_;
  ncFlag_ = rhs.ncFlag_;
  parent_ = rhs.parent_;
  numberOfLoops_ = rhs.numberOfLoops_;
  evalFlag_ = rhs.evalFlag_;
  finalData_ = rhs.finalData_;
  expression_ = rhs.expression_;
  type_ = rhs.type_;
  string_ = rhs.string_;
  level_ = rhs.level_;
  evaluateNow_ = rhs.evaluateNow_;
  processingFinished_ = rhs.processingFinished_;
  rpnDone_ = rhs.rpnDone_;
  codesAreParsed_ = rhs.codesAreParsed_;
  //
  operatorCntr_ = rhs.operatorCntr_;
  functionWithArgumentsCntr_ = rhs.functionWithArgumentsCntr_;
  indexedVariableCntr_ = rhs.indexedVariableCntr_;
  rangeVectorCntr_ = rhs.rangeVectorCntr_;
  listVectorCntr_ = rhs.listVectorCntr_;
  fcnPos_ = rhs.fcnPos_;
  fcnArg_ = rhs.fcnArg_;
  fcnIdx_ = rhs.fcnIdx_;
  oprPos_ = rhs.oprPos_;
  oprAnd_ = rhs.oprAnd_;
  varPos_ = rhs.varPos_;
  varIdx_ = rhs.varIdx_;
  rngPos_ = rhs.rngPos_;
  rngIdx_ = rhs.rngIdx_;
  lstPos_ = rhs.lstPos_;
  lstIdx_ = rhs.lstIdx_;
  highestNestingLevel_ = rhs.highestNestingLevel_;
  hierarchy_ = rhs.hierarchy_;
}

bool Workspace::operator==( Workspace const &right ) const
{
  if( right.codes_.size() != codes_.size() )return false;
  std::size_t size( codes_.size() );
  for( std::size_t i(0); i < size; ++i )
  {
    if( *codes_[i] != *right.codes_[i] )return false;
  }
  return true;
}

wxString &Workspace::GetFinalString()
{ return finalString_; }

void Workspace::SetFinalString( wxString &s )
{ finalString_ = s; }

NumericData &Workspace::GetFinalData()
{ return finalData_; }

bool Workspace::IsEmpty()
{ return finalData_.IsEmpty(); }

bool Workspace::IsUnordered() const
{ return finalData_.IsUnordered(); }

bool Workspace::IsAscending() const
{ return finalData_.IsAscending(); }

bool Workspace::IsDescending() const
{ return finalData_.IsDescending(); }

void Workspace::SetUnordered()
{ finalData_.SetUnordered(); }

void Workspace::SetAscending()
{ finalData_.SetAscending(); }

void Workspace::SetDescending()
{ finalData_.SetDescending(); }

std::size_t Workspace::GetNumberOfDimensions() const
{ return finalData_.GetNumberOfDimensions(); }

void Workspace::SetNumberOfDimensions( std::size_t i )
{ finalData_.SetNumberOfDimensions( i ); }

unsigned int *Workspace::GetDimMag()
{ return finalData_.GetDimMag(); }

std::size_t Workspace::GetDimMag( unsigned int i ) const
{ return finalData_.GetDimMag( i ); }

void Workspace::SetDimMag( unsigned int i, unsigned int v )
{ finalData_.SetDimMag( i, v ); }

void Workspace::SetValue( double v )
{ finalData_.SetScalarValue( v ); }

double Workspace::GetValue() const
{ return finalData_.GetScalarValue(); }

void Workspace::SetData( std::vector<double> &d )
{ finalData_.SetData( d ); }

void Workspace::SetData( int i, double d )
{ finalData_.SetData( i, d ); }

void Workspace::SetData( unsigned int i, unsigned int j, double d )
{ finalData_.SetData( i, j, d ); }

void Workspace::SetData( unsigned int i, unsigned int j, unsigned int k, double d )
{ finalData_.SetData( i, j, k, d ); }

std::vector<double> &Workspace::GetData()
{ return finalData_.GetData(); }

double Workspace::GetData( unsigned int i ) const
{ return finalData_.GetData( i ); }

double Workspace::GetData( unsigned int i, unsigned int j ) const
{ return finalData_.GetData( i, j ); }

double Workspace::GetData( unsigned int i, unsigned int j, unsigned int k ) const
{ return finalData_.GetData( i, j, k ); }

void Workspace::GetMinMax( double &min, double &max ) const
{ finalData_.GetMinMax( min, max ); }

void Workspace::ParseAndCheck( bool expand )
{
  if( CodesAreParsed() )return;
  ParseCodes( expand );
  if( codes_.empty() )
  {
    throw EExpressionError( wxT("blank expression") );
  }
  if( expand )return;
  SequencingCheck();
  BracketCheck();
  FinalCheck();
  DetermineType();
}

void Workspace::ParseCodes( bool expand )
{
  // Define the lexical scanner used to lexical scan input expression
  // string into tokens. The output of this operation is a list of tokens,
  // or basic mathematical entities, that can then be ordered into Reverse
  // Polish Notation (RPN) and put onto a command stack for later evaluation.
  // For example, the expression  2*EXP(X)+1 would be parsed by the lexical
  // scanner to give:
  //
  //   Token      Output state     Description
  //    2          -3               Constant number
  //    *          -8               Operator
  //    EXP        -1               Variable or function name
  //    (          55               Left parenthesis
  //    X          -1               Variable or function name
  //    )          54               Right parenthesis
  //    +          -8               Operator
  //    1          -3                 //onstant number
  //    EOL        -7               End of line
  //
  //   Lexical scanner transition table: EXPARS(11,19)
  //   This table has 11 transition states, 19 character classes, and 14
  //   output states (which indicate the token type). The description of
  //   the table is as follows:
  //
  //        EXPARS    lexical  scanner  (to parse expressions)
  // TRANSITION
  //   STATE -->   1    2    3    4    5    6    7    8    9   10   11   12
  // CLASS
  //  1 EOL       -7   -1 -102   -8   -3 -103   -3 -104 -104   -3   -8 -108
  //  2 Others   101   -1    3   -8   -3 -103 -109 -104 -104   -3   -8   12
  //  3 Blank      1    2    3    4    5    6    7    8    9   10   11   12
  //  4 A-C,F-Z$_  2    2    3   -8   -3 -103 -109 -104 -104   -3   -8   12
  //  5 E,D,e,d    2    2    3   -8    8 -103    8 -104 -104   -3   -8   12
  //  6 0-9        5    2    3   -8    5    7    7   10   10   10   -8   12
  //  7 .          6   -1    3   -8    7 -103 -103 -104 -104   -3   -8   12
  //  8 )]}       54   -1    3   -8   -3 -103   -3 -104 -104   -3  -11   12
  //  9 ([{       55   -1    3   -8   -3 -103   -3 -104 -104   -3   -8   12
  // 10 ,         56   -1    3   -8   -3 -103   -3 -104 -104   -3  -11   12
  // 11 :         59   -1    3   -8   -3 -103   -3 -104 -104   -3  -11   12
  // 12 "          3   -1   58   -8   -3 -103   -3 -104 -104   -3   -8   12
  // 13 #         60   -1    3   -8   -3 -103   -3 -104 -104   -3   -8   12
  // 14 *         11   -1    3    4   -3 -103   -3 -104 -104   -3    4   12
  // 15 +-         4   -1    3    4   -3 -103   -3    9 -104   -3    4   12
  // 16 Oper chars 4   -1    3    4   -3 -103   -3 -104 -104   -3    4   12
  // 17 ;         62   -1    3   -8   -3 -103   -3 -104 -104   -3 -105   12
  // 18 `         12   -1    3   -8   -3 -103   -3 -104 -104   -3   -8 -106
  // 19 '       -107 -107 -107 -107 -107 -107 -107 -107 -107 -107 -107  -13
  //
  // Transition state key:
  //  1  Start                        7  Constant (real number)
  //  2  Variable or function name    8  Real number + exponent (D,E)
  //  3  Operator in quotes           9  Real number + exponent sign
  //  4  Operator                    10  Real number exponent
  //  5  Integer constant            11  Wildcard
  //  6  Decimal point               12  Quote string
  //
  // Output state key:
  // -1  Function or variable name   -8 (or 58)  Operator
  // -3  Constant number             -9 (or 59)  Colon :
  // -4 (or 54)  Right bracket      -10 (or 60)  Hash character #
  // -5 (or 55)  Left  bracket      -11  Wildcard character *
  // -6 (or 56)  Comma ,            -12 (or 62)  Semicolon ;
  // -7  EOL (End-Of-Line)          -13  Quote string
  //
  // Error states
  //  -101  invalid character
  //  -102  no closing quote on operator
  //  -103  invalid decimal point
  //  -104  invalid exponent
  //  -105  invalid semicolon after wildcard
  //  -106  invalid opening quote
  //  -107  invalid closing quote
  //  -108  closing quote missing
  //  -109  invalid real number
  //
  //  A negative output state indicates the last character scanned can be
  //  thrown away, and the scan pointer ILAST advanced in preparation for
  //  the next scan. An output state larger than 50 indicates the last
  //  character scanned forms an integral part of the next token, and must
  //  be kept and re-used in the next scan. The scan pointer ILAST is
  //  decremented by 1 to ensure this is done
  //
  //  exClass contains the table of class codes for each of the 128 ASCII
  //  characters, with the index of each character being its decimal ASCII
  //  code value + 1. For example, the character '2' is a numeric digit with
  //  class code 6 (see below), and has an ASCII code value of decimal 50.
  //  Therefore, exClass(50 + 1)= 6
  //
  //         Class     Characters
  //             1     end of line    (end of input line encountered)
  //             2     invalid characters
  //             3     blank or tab
  //             4     A-C F-Z a-c f-z $ _
  //             5     D E d e
  //             6     0-9
  //             7     .
  //             8     )]}
  //             9     ([{
  //            10     ,
  //            11     :
  //            12     "
  //            13     #
  //            14     *
  //            15     +-
  //            16     ! % & = ~ ^ @ | \ < > / <> ><  (operators)
  //            17     ;
  //            18     `
  //            19     '
  //
  int const exClass[128] = {
      2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  2,  2,  2,  2,  2,  2,
      2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
      3, 16, 12, 13,  4, 16, 16, 19,  9,  8, 14, 15, 10, 15,  7, 16,
      6,  6,  6,  6,  6,  6,  6,  6,  6,  6, 11, 17, 16, 16, 16, 16,
     16,  4,  4,  4,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  9, 16,  8, 16,  4,
     18,  4,  4,  4,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
      4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  9, 16,  8, 16,  2
  };
  int const expars[19][13] = {
      {  -7,  -1,-102,  -8,  -3,-103,  -3,-104,-104,  -3,  -8,-108,-108 }, // eol
      { 101,  -1,   3,  -8,  -3,-103,-109,-104,-104,  -3,  -8,  12,  13 }, // invalid char
      {   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13 }, // blank
      {   2,   2,   3,  -8,  -3,-103,-109,-104,-104,  -3,  -8,  12,  13 }, // a-z
      {   2,   2,   3,  -8,   8,-103,   8,-104,-104,  -3,  -8,  12,  13 }, // ed
      {   5,   2,   3,  -8,   5,   7,   7,  10,  10,  10,  -8,  12,  13 }, // 0-9
      {   6,  -1,   3,  -8,   7,-103,-103,-104,-104,  -3,  -8,  12,  13 }, // .
      {  54,  -1,   3,  -8,  -3,-103,  -3,-104,-104,  -3, -11,  12,  13 }, // )
      {  55,  -1,   3,  -8,  -3,-103,  -3,-104,-104,  -3,  -8,  12,  13 }, // (
      {  56,  -1,   3,  -8,  -3,-103,  -3,-104,-104,  -3, -11,  12,  13 }, // ,
      {  59,  -1,   3,  -8,  -3,-103,  -3,-104,-104,  -3, -11,  12,  13 }, // :
      {   3,  -1,  63,  -8,  -3,-103,  -3,-104,-104,  -3,  -8,  12,  13 }, // "
      {  60,  -1,   3,  -8,  -3,-103,  -3,-104,-104,  -3,  -8,  12,  13 }, // #
      {  11,  -1,   3,   4,  -3,-103,  -3,-104,-104,  -3,   4,  12,  13 }, // *
      {   4,  -1,   3,   4,  -3,-103,  -3,   9,-104,  -3,   4,  12,  13 }, // +-
      {   4,  -1,   3,   4,  -3,-103,  -3,-104,-104,  -3,   4,  12,  13 }, // operators
      {  62,  -1,   3,  -8,  -3,-103,  -3,-104,-104,  -3,-105,  12,  13 }, // ;
      {  12,  -1,   3,  -8,  -3,-103,  -3,-104,-104,  -3,  -8,  12,  13 }, // `
      {  13,  -1,   3,  -8,  -3,-103,  -3,-104,-104,  -3,  -8,  63,  63 }  // '
  };
  std::size_t last = 0;
  std::size_t first = 0;
  std::size_t ilast;
  //
  //   Main loop for scanning the expression for tokens. The result
  //   returned is a list of "tokens" types comprising the original
  //   expression, from left to right.  For example, the expression
  //
  //   BFIELD[47]*1.0015
  //
  //   would yield a token list like:
  //         Variable name      BFIELD
  //         Left parenthesis   [
  //         Real constant      47
  //         Right parenthesis  ]
  //         Operator           *
  //         Real constant      1.0015
  //
  //   Perform a lexical scan using the transition table EXPARS described above.
  //   Return a token consisting of the next operand or operator in the arithmetic
  //   expression being scanned. Process repeatedly until the end of the input
  //   buffer is reached, as flagged by the return of an EOL (End-Of-Line) token.
  //   LAST points to the last index scanned, and is useful since it will usually
  //   point to the source of error in case of a problem. The number of internal
  //   (or transition) states coded in the lexical scanner array EXPARS is 12.
  //   ISTATE indicates the type of the returned token, as previously documented.
  //
  while ( first < string_.length() )
  {
    first = last+1;
    //
    //  Perform a lexical scan of the string contained in the input buffer
    //  using the transition table array expars.
    //  Array ex_class contains the class codes for each of the ASCII characters
    //  (the index of ex_class is the decimal ASCII value). ilast will be the
    //  position of the last character scanned.  In the event of error, ilast will
    //  point to the location of the error.
    //
    //   istate: The output state returned after completion of the lexical scan,
    //           as determined by the transition table EXPARS. The convention is to
    //           denote output states by the negative integers -1, -2, -3, ... to
    //           clearly separate these states from internal (transition) states of
    //           the transition table itself.
    //
    ilast = 0;    // field pointer
    int state = 1;
    //
    //   Main scan loop: check the current state and if this is a transition state
    //   (i.e. the current field is not yet finished) then proceed with the next
    //   character; else end the field and return.
    //

    //std::cout << "string_.length= " << string_.length() << ", first= " << first << "\n";

    while ( state >= 1 && state <= 13 )
    {
      //  1 <= state <= 13 (transition state) so increment the scan pointer
      //
      //  Use the transition table to evaluate the new state
      //  given the present state and class
      //
      if( ++ilast > string_.length()-first+1 ) // > input line length
      {
        state = expars[0][state-1];

        //std::cout << "state= " << state << "\n";

      }
      else       //  deduce the class of the current character
      {
        int iascii = TryConvertToAscii( string_[first+ilast-2] );
        state = expars[exClass[iascii]-1][state-1];

        //std::cout << "iascii= " << iascii << ", state= " << state << "\n";

      }
    }
    if( state == -7 )return; // end-of-line
    if( state <= -100 )      // an error has been detected
    {
      switch ( -state-100 )
      {
        case 1:
          throw EExpressionError( wxT("invalid character") );
        case 2:
          throw EExpressionError( wxT("no closing quote on operator") );
        case 3:
          throw EExpressionError( wxT("invalid decimal point") );
        case 4:
          throw EExpressionError( wxT("invalid exponent") );
        case 5:
          throw EExpressionError( wxT("invalid semicolon after wildcard") );
        case 6:
          throw EExpressionError( wxT("invalid opening quote") );
        case 7:
          throw EExpressionError( wxT("invalid closing quote") );
        case 8:
          throw EExpressionError( wxT("closing quote missing") );
        case 9:
          throw EExpressionError( wxT("invalid real number") );
      }
    }
    TOP:
    if( state <= 0 )--ilast;
    //
    //  output states
    //        4         )]}
    //        5         ([{
    //        6          ,
    //        8          "
    //        9          :
    //       10          #
    //       12          ;
    //       13        '...'
    //
    last = first+ilast-1;    // absolute buffer position
    wxString name( string_.substr(first-1,last-first+1) );
    //
    // trim leading and trailing blanks
    //
    wxString const blank( wxT(' ') );
    int rTemp = name.find_last_not_of( blank );
    int lTemp = name.find_first_not_of( blank );
    name = name.substr( lTemp, rTemp-lTemp+1 );
    //
    ExprCodes *aCode = new ExprCodes;
    //
    state = abs(state);
    if( state >= 50 )state -= 50; // a valid output state
    //
    // special characters  ,  :   #   *   ;
    //                     6  9  10  11  12
    // are valid only inside the range of a variable or a function
    //
    if( state==1 || state==2 )    // function or variable (numeric or text)
    {
      wxString name2( name.Upper() );
      if( FunctionTable::GetTable()->Contains( name2 ) )
      {
        FunctionDefinition *fcn = FunctionTable::GetTable()->GetFunction(name2);
        if( fcn->Name() == wxT("STRING") )
        {
          ++ilast;
          if( string_[last+1]=='\'' || string_[last+1]=='"' || string_[last+1]=='`' )
          {
            for( std::size_t j=last+2; j<string_.size(); ++j )
            {
              if( string_[j] == ')' )
              {
                for( std::size_t k=j+1; k<string_.size(); ++k )
                {
                  string_[k-1] = string_[k];
                }
                string_.resize( string_.size()-1 );
                for( std::size_t k=last+1; k<string_.size(); ++k )
                {
                  string_[first+k-last-2] = string_[k];
                }
                string_.resize( string_.size()-7 );
                ilast = j-7-first+1;
                break;
              }
            }
          }
          else
          {
            string_[last] = '\'';
            for( std::size_t j=last+1; j<string_.size(); ++j )
            {
              if( string_[j] == ')' )
              {
                string_[j] = '\'';
                for( std::size_t k=last+1; k<=string_.size(); ++k )
                {
                  string_[first+k-last-2] = string_[k-1];
                }
                string_.resize( string_.size()-6 );
                ilast = j-5-first+1;
                break;
              }
            }
          }
          delete aCode;
          state = 13;
          goto TOP;
        }
        aCode->SetFcnPtr( fcn );
      }
      else if( NVariableTable::GetTable()->Contains( name2 ) )
      {
        NumericVariable *aVariable = NVariableTable::GetTable()->GetVariable( name2 );
        //
        if( expression_ && (expression_->IsaFit() &&
                            aVariable->GetData().GetNumberOfDimensions()==0 &&
                            aVariable->GetData().IsFit()) )
        {
          expression_->TestFitParameterName( name2 );
        }
        //
        aCode->SetNVarPtr( aVariable  );
        // ivused[++nvused] = aVariable;
      }
      else if( TVariableTable::GetTable()->Contains( name2 ) )
      {
        TextVariable *aVariable = TVariableTable::GetTable()->GetVariable( name2 );
        aCode->SetTVarPtr( aVariable );
      }
      else if( expand || (GetLevel()==0 && IsCharacter() && codes_.size()==1) )
      { // single character string assumed to be a quote string without quotes
        aCode->SetTString( name );
      }
      else
      {
        delete aCode;
        aCode = NULL;
        throw EExpressionError( name+wxT(" is undefined: not a function or a variable") );
      }
    }
    else if( state == 3 ) // constant number
    {
      //std::cout << "name= |" << name.mb_str(wxConvUTF8) << "|\n";

      double real8;
      if( !name.ToDouble(&real8) )
      {
        delete aCode;
        aCode = 0;
        throw EExpressionError( wxString(wxT("invalid real number: "))+name );
      }
      aCode->SetConstantValue( real8, name );
    }
    else if( state == 4 ) // closing bracket     )  ]  }
    {
      aCode->SetClosingBracket();
    }
    else if( state == 5 ) // opening bracket    (  [  {
    {
      if( codes_.size() > 1 )              // more than 1 token on input
      {
        if( codes_.back()->IsaClosingBracket() )
        {                                    // previous token is  )
          delete aCode;
          aCode = NULL;
          codes_.back()->SetPairBracket();    // )( combination bracket
          continue;
        }
        else                                 // coded as a single RPN code
        {
          aCode->SetOpeningBracket();
        }
      }
      else                                   // single ( token on input
      {
        aCode->SetOpeningBracket();
      }
    }
    else if( state == 6 ) // comma  ,
    {
      aCode->SetComma();
      aCode->SetValid( false );   // valid only in brackets
    }
    else if( state == 8 ) // operator
    {
      wxString name2( name.Upper() );
      if( OperatorTable::GetTable()->Contains( name2 ) )
      {
        OperatorDefinition *op = OperatorTable::GetTable()->GetOperator(name2);
        aCode->SetOpPtr( op );
      }
      else
      {
        delete aCode;
        aCode = 0;
        throw EExpressionError( wxString(wxT("unknown operator: "))+name2 );
      }
    }
    else if( state == 9 ) // colon   :
    {
      aCode->SetColon();
      aCode->SetValid( false );    // valid only in brackets
    }
    else if( state == 10 ) // hash character   #
    {
      aCode->SetHash();
      aCode->SetValid( false );    // valid only in brackets
    }
    else if( state == 11 ) // wildcard character   *
    {
      aCode->SetAsterisk();
      aCode->SetValid( false );    // valid only in brackets
    }
    else if( state == 12 ) // semicolon    ;
    {
      aCode->SetSemicolon();
      aCode->SetValid( false );    // valid only in brackets
    }
    else if( state == 13 ) // quote string `...'
    {
      if( name.length() == 2 )
      {
        delete aCode;
        aCode = 0;
        throw EExpressionError( wxT("null quote string") );
      }
      aCode->SetTString( name.substr(1,name.length()-2) );
    }
    codes_.push_back( aCode );
  }
  return;
}

void Workspace::DetermineType()
{
  // Need to handle special case of // operator, which can be character (default)
  // or array type.  If expression is numeric, set // to be array, but then have
  // to set it to character in the constructor for Expression class.
  //
  if( type_ != UNKNOWN )return;
  bool ambiguous = false;
  std::deque<ExprCodes*>::const_iterator end( codes_.end() );
  for( std::deque<ExprCodes*>::const_iterator j(codes_.begin()); j!=end; ++j )
  {
    ExprCodes *code = *j;
    if( code->IsaFunction() )
    {
      if( !ambiguous )
      {
        FunctionDefinition const *fp = code->GetFcnPtr();
        if( fp->IsNum2NumScalar() || fp->IsNum2NumVector() || fp->IsMixed2Num() )
        {
          type_ = NUMERIC;
          OperatorTable::GetTable()->GetOperator(wxT("//"))->SetArray();
          return;
        }
        else
        {
          type_ = CHARACTER;
          return;
        }
      }
    }
    else if( code->IsaNVariable() )
    {
      if( !ambiguous )
      {
        type_ = NUMERIC;
        OperatorTable::GetTable()->GetOperator(wxT("//"))->SetArray();
        return;
      }
    }
    else if( code->IsaWorkspace() )
    {
      if( !ambiguous )
      {
        Workspace const *ws = code->GetWorkspacePtr();
        if( ws->IsNumeric() )
        {
          type_ = NUMERIC;
          OperatorTable::GetTable()->GetOperator(wxT("//"))->SetArray();
          return;
        }
        else
        {
          type_ = CHARACTER;
          return;
        }
      }
    }
    else if( code->IsaTVariable() )
    {
      ambiguous = true;
    }
    else if( code->IsaTString() )
    {
      type_ = CHARACTER;
      return;
    }
    else if( code->IsaConstant() || code->IsaImmediateValue() )
    {
      if( !ambiguous )
      {
        type_ = NUMERIC;
        OperatorTable::GetTable()->GetOperator(wxT("//"))->SetArray();
        return;
      }
    }
    else if( code->IsaOperator() )
    {
      if( !ambiguous )
      {
        OperatorDefinition *op = code->GetOpPtr();
        if( op->IsCharacter() )
        {
          type_ = CHARACTER;
          return;
        }
        else
        {
          type_ = NUMERIC;
          OperatorTable::GetTable()->GetOperator(wxT("//"))->SetArray();
          return;
        }
      }
    }
    else if( code->IsaOpeningBracket() )
    {
    }
    else if( code->IsaClosingBracket() )
    {
    }
    else if( code->IsaPairBracket() || code->IsaComma() || code->IsaColon() ||
             code->IsaSemicolon() || code->IsaHash() || code->IsaAsterisk() )
    {
    }
    else if( code->IsaRangeVector() || code->IsaListVector() )
    {
      if( !ambiguous )
      {
        type_ = NUMERIC;
        OperatorTable::GetTable()->GetOperator(wxT("//"))->SetArray();
        return;
      }
    }
  }
  type_ = CHARACTER;
  return;
}

void Workspace::SequencingCheck()
{
  // The lexical scan of the expression is finished
  // Check for errors which involve invalid sequencing
  // of the terms in the expression
  //
  std::size_t const errorTable[14][14] =
      { { 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0 }, // 11
        { 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0 }, // 11
        { 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0 }, // 12
        { 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0 }, // 11
        { 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1 }, // 13
        { 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1 }, // 13
        { 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1 }, // 13
        { 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1 }, // 14
        { 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1 }, // 15
        { 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0 }, // 12
        { 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1 }, // 16
        { 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1 }, // 13
        { 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0 }, // 17
        { 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1 } }; // 13
  int currentTerm;
  int followingTerm = 6;
  wxString const term[14] = { wxT("Function"), wxT("Variable"), wxT("Constant"),
                              wxT(")"), wxT("("), wxT(","), wxT("Null"), wxT("Operator"),
                              wxT(":"), wxT("#"), wxT("Wildcard"), wxT(";"),
                              wxT("Quote string"), wxT(")(") };
  //
  //  both currentTerm and followingTerm start off as the number of the "null" term
  //  which refers to the beginning or end of the expression
  //
  std::deque<ExprCodes*>::const_iterator end( codes_.end() );
  for( std::deque<ExprCodes*>::const_iterator i(codes_.begin()); i!=end; ++i )
  {
    ExprCodes *code = *i;
    //
    // update the current term number and find the code for the following term
    //
    currentTerm = followingTerm;
    followingTerm = code->GetFollowingTerm()-1;
    if( code->IsaOperator() )
    {
      //
      //  Determine the operator type: oparg=1 (unary) or oparg=2 (binary)
      //
      OperatorDefinition *op = code->GetOpPtr();
      unsigned short int oparg = 0;
      if( op->UnaryPriority() == 0 )oparg = 2;
      if( op->BinaryPriority() == 0 )oparg = 1;
      if( oparg == 0 )
      {
        //
        // Here the operator is allowed to be either unary or binary.
        // The operator type of the following term is determined by the
        // current term number currentTerm. If the current term is a function,
        // variable, constant, # or ), then the operator is binary.
        //
        oparg = 1;
        if( currentTerm<4 || currentTerm==9 )oparg = 2;
      }
      if( oparg==2 && currentTerm==6 )
        throw EExpressionError( wxT("NULL followed by binary operator") );
      oparg==2 ? code->SetBinary() : code->SetUnary();
      op->OperandCounter( oparg );
    }
    //
    //Do the sequencing check of the terms
    // Term  1      Function name
    // Term  2      Variable name
    // Term  3      Constant number
    // Term  4      Right parenthesis )]}
    // Term  5      Left  parenthesis ([{
    // Term  6      Comma ,
    // Term  7      End-of-line (null)
    // Term  8      Operator
    // Term  9      Colon :
    // Term 10      Hash # (current max. defined value of subscript)
    // Term 11      Wildcard subscript range *
    // Term 12      Semicolon ;
    // Term 13      Text (quote) string `....'
    // Term 14      Close & open parenthesis pair )(
    //  Check to see if followingTerm is allowed to follow currentTerm, i.e. if the
    //  following term may come after the current term. Branch to 40 if
    //  allowed, otherwise to 30 if not
    // The following terms are grouped together and treated identically with
    // regard to sequencing errors
    //  1. Variable-like:         1,2,4
    //  2. Constant-like:         3,10
    //  3. Field separators:      5,6,7,12,14
    //  4. Operators:             8
    //  5. Colon:                 9
    //  6. Wildcard subscripts:  11
    //  7. Text (quote) string:  13
    //
    if( errorTable[currentTerm][followingTerm] )
      throw EExpressionError( term[currentTerm]+wxT(" followed by ")+term[followingTerm] );
  }
  currentTerm = followingTerm;
  followingTerm = 6;
  if( errorTable[currentTerm][followingTerm] )
    throw EExpressionError( term[currentTerm]+wxT(" followed by ")+term[followingTerm] );
  return;
}
 
void Workspace::BracketCheck()
{
  //  Check for mismatched parentheses, wrong number of variable
  //  subscripts, and accumulate the number of routine arguments
  //  level is the depth within parentheses. When searching
  //  forward level starts off as zero and is incremented by 1 for every
  //  "(" found, and is decremented by 1 for every ")" found. If level
  //  becomes negative then a ")" with no leading "(" has been found
  //
  int level = 0;
  std::deque<ExprCodes*>::const_iterator end( codes_.end() );
  for( std::deque<ExprCodes*>::const_iterator i(codes_.begin()); i!=end; ++i )
  {
    // search for closing bracket with no opening bracket
    //
    if( (*i)->IsaClosingBracket() )--level;
    if( (*i)->IsaOpeningBracket() )++level;
    if( level < 0 )
    {
      throw EExpressionError( wxT("closing bracket has no opening bracket") );
    }
  }
  if( level > 0 )throw EExpressionError( wxT("opening bracket has no closing bracket") );
  //
  // level > 0 means there are more opening brackets than closing ones
  // To find the offending bracket, scan the expression backwards for openings
  // until a mismatch is found.  level is incremented by 1 for every closing
  // found, and is decremented by 1 for every opening found. If level becomes
  // negative then a opening with no following closing has been found.
  //
  std::size_t shift = 0;
  for( std::size_t i=0; i<codes_.size(); i+=shift+1 )
  {
    shift = 0;
    if( codes_[i]->IsaOpeningBracket() )
    {
      // find out if the opening bracket begins a $null
      // The search for entries/ranges terminates at level zero. If pair bracket
      // found and level 1 opening bracket not preceeded by a function or a variable
      // then we have found a $null
      //
      int level = 1;
      bool insertNull = false;
      for( std::size_t j=i+1; j<codes_.size(); ++j )
      {
        if( codes_[j]->IsaOpeningBracket() )++level;
        if( level == 1 )
        {
          if( codes_[j]->IsaClosingBracket() )
          {
            if( insertNull ) // insert null function code for indexed expression
            {
              ExprCodes *aCode = new ExprCodes;
              codes_.push_back( aCode );
              int end = static_cast<int>(codes_.size())-2;
              for( int jj=end; jj>=static_cast<int>(i); --jj )*codes_[jj+1] = *codes_[jj];
              codes_[i]->SetFcnPtr( FunctionTable::GetTable()->GetFunction(wxT("NULLFUNC")) );
              shift = 1;
            }
            break;     // end of $null item, leave j loop
          }
          else if( codes_[j]->IsaPairBracket() )
          {
            if( insertNull )throw EExpressionError( wxT("invalid usage of )(") );
            if( i == 0 || (i>0 &&
                           !(codes_[i-1]->IsaFunction()||codes_[i-1]->IsaTVariable())) )
              insertNull = true;
            if( insertNull )codes_[j]->SetValid( true );
          }
        }
        if( codes_[j]->IsaClosingBracket() )--level;
      }   // end of j loop
    }   // end of if opening bracket
  }   // end of i loop
}

void Workspace::MakeNewWorkspace( int j, int k, Type type )
{
  int level = GetLevel() + 1;
  if( level > expression_->GetHighestLevel() )
    expression_->SetHighestLevel( level );
  Workspace *aWs = new Workspace(level);
  aWs->SetType( type );
  aWs->SetExpression( expression_ );
  ExprCodes *aCode = new ExprCodes;
  aCode->SetOpeningBracket();
  aWs->GetCodes().push_back( aCode );
  for( int i=k+1; i<=j-1; ++i )
  {
    ExprCodes *e = new ExprCodes;
    aWs->GetCodes().push_back( e );
    *e = *codes_[i];
  }
  ExprCodes *bCode = new ExprCodes;
  bCode->SetClosingBracket();
  aWs->GetCodes().push_back( bCode );
  aWs->SetCodesAreParsed();
  expression_->AddWorkspace( aWs );
  codes_[k+1]->SetWorkspacePtr( aWs );
  for( std::size_t i=j+1; i<=codes_.size(); ++i )*codes_[i-j+k+1] = *codes_[i-1];
  for( int i=0; i<j-k-2; ++i )
  {
    delete codes_.back();
    codes_.pop_back();
  }
}

void Workspace::FinalCheck()
{
  SetCodesAreParsed();
  //
  // Check for invalid commas, colons, hash symbols and wildcards
  // Also check that all of the functions have the correct number of
  // arguments numberOfArguments(1,I) <= NARG <= numberOfArguments(2,I),
  // and that all of the variables have the correct number of indices,
  // and at most three ranges
  //
  std::size_t shift = 0;
  for( std::size_t i=0; i<codes_.size(); i+=shift+1 )
  {
    shift = 0;
    ExprCodes *code = codes_[i];
    bool notValid = !code->GetValid();
    if( code->IsaClosingBracket() || code->IsaConstant() ||
        code->IsaOperator() || code->IsaImmediateValue() )continue;
    if( code->IsaColon() )
    {
      if( notValid )throw EExpressionError( wxT("invalid colon") );
    }
    else if( code->IsaHash() )
    {
      if( notValid )throw EExpressionError( wxT("invalid hash symbol") );
    }
    else if( code->IsaAsterisk() )
    {
      if( notValid )throw EExpressionError( wxT("invalid wildcard index") );
    }
    else if( code->IsaSemicolon() )
    {
      if( notValid )throw EExpressionError( wxT("invalid semicolon") );
    }
    else if( code->IsaOpeningBracket() ||
             code->IsaComma() ||
             code->IsaPairBracket() )
    {
      if( code->IsaComma() && notValid )throw EExpressionError( wxT("invalid comma") );
      //
      // find out if the opening bracket begins a range or list vector
      //  list vector:  number of entries = one plus number of semicolons at level 1
      //  range vector: number of ranges  = one plus number of colons at level 1
      // The search for entries/ranges terminates at level zero
      // If colons/semicolons (but not both) are found, then
      // we have found a range/list vector
      //
      bool gotoOuterLoop = false;
      int level = 1;
      bool isaRangeVector = false;
      bool isaListVector = false;
      std::size_t listItemCounter = 0;        // list vector item counter
      std::size_t rangeItemCounter = 0;       // range vector item counter
      for( std::size_t j=i+1; j < codes_.size(); ++j )  // i points to opening bracket
      {
        if( codes_[j]->IsaOpeningBracket() )++level;
        if( level == 1 )
        {
          if( codes_[j]->IsaClosingBracket() || codes_[j]->IsaComma() ||
              codes_[j]->IsaPairBracket() )
          {
            if( isaRangeVector || isaListVector )
            {
              codes_.push_back( new ExprCodes );
              codes_.push_back( new ExprCodes );
              //
              // shift codes down
              //
              for( int k=codes_.size()-3; k>=static_cast<int>(i+1); --k )*codes_[k+2] = *codes_[k];
              if( isaRangeVector )
              {
                ++rangeVectorCntr_;
                RangeVector *rngvec = new RangeVector;
                rngvec->SetItemCntr( rangeItemCounter );
                codes_[i+1]->SetRangeVector();
                codes_[i+1]->SetRangeVectorPtr( rngvec );
                codes_[i+1]->SetCntr( rangeVectorCntr_ );
                codes_[i+2]->SetOpeningBracket();
                codes_[i+2]->SetRefRangeVector();
                codes_[i+2]->SetCntr( rangeVectorCntr_ );
              }
              else                         // must be a list vector
              {
                ++listVectorCntr_;
                ListVector *lstvec = new ListVector;
                lstvec->SetItemCntr( listItemCounter );
                codes_[i+1]->SetListVector();
                codes_[i+1]->SetListVectorPtr( lstvec );
                codes_[i+1]->SetCntr( listVectorCntr_ );
                codes_[i+2]->SetOpeningBracket();
                codes_[i+2]->SetRefListVector();
                codes_[i+2]->SetCntr( listVectorCntr_ );
              }
              j += 2;
              codes_.push_back( new ExprCodes );
              //
              // shift codes down
              //
              for( int k=codes_.size()-2; k>=static_cast<int>(j); --k )*codes_[k+1] = *codes_[k];
              codes_[j]->SetClosingBracket();
              isaRangeVector ? codes_[j]->SetRefRangeVector() : codes_[j]->SetRefListVector();
              shift = 2;
            }
            gotoOuterLoop = true;
            break;  // end of range or list item
          }
          else if( codes_[j]->IsaColon() )   // colon
          {
            if( isaListVector )throw EExpressionError( wxT("colon not allowed in list of entries") );
            if( isaRangeVector )
            {
              if( rangeItemCounter >= 3 )throw EExpressionError( wxT("more than 3 items in a range") );
              ++rangeItemCounter;
              codes_[j]->SetValid( true );
            }
            else               // first indication of range vector
            {
              isaRangeVector = true;
              rangeItemCounter = 2;       // initialize range counter
              codes_[j]->SetValid( true );
            }
          }
          else if( codes_[j]->IsaSemicolon() )  // semicolon
          {
            if( isaRangeVector )
              throw EExpressionError(  wxT("semicolon not allowed in range specification") );
            if( isaListVector )
            {
              ++listItemCounter;
              codes_[j]->SetValid( true );
            }
            else               // first indication of list vector
            {
              isaListVector = true;
              listItemCounter = 2;        // initialize list counter
              codes_[j]->SetValid( true );
            }
          }
        }                                        // end of if level == 1
        if( codes_[j]->IsaClosingBracket() )--level;  // closing bracket
      }                                          // end of for loop over j
      if( gotoOuterLoop )continue;
    }
    else if( code->IsaFunction() )
    {
      std::size_t indexCounter = 0;
      std::size_t argCounter = 0;
      std::size_t jopen, jclose, jpair;
      FunctionDefinition *fcn = code->GetFcnPtr();
      if( i < codes_.size()-1 )
      {
        if( codes_[i+1]->IsaOpeningBracket() )
        {
          argCounter = 1;    // must be at least one argument
          std::size_t level = 0;  // the depth of brackets following the function name
          //
          // The number of arguments is the number of commas (at level 1) + 1
          //
          jopen = i+1; // points to opening bracket
          for( std::size_t j=jopen; j<codes_.size(); ++j )
          {
            if( codes_[j]->IsaClosingBracket() )--level;
            else if( codes_[j]->IsaOpeningBracket() )++level;
            if( level == 0 )
            {
              jclose = j;
              if( fcn->Name() == wxT("VARNAME") )
              {
                if( j-jopen > 2 )
                  throw EExpressionError(
                    wxT("VARNAME function expects a simple variable name"));
                wxString name;
                if( codes_[jopen+1]->IsaNVariable() )
                  name = codes_[jopen+1]->GetNVarPtr()->GetName();
                else if( codes_[jopen+1]->IsaTVariable() )
                  name = codes_[jopen+1]->GetTVarPtr()->GetName();
                else
                  throw EExpressionError(wxT("VARNAME function expects a variable name"));
                codes_[i]->SetTString( name );
                // shift up to eliminate extraneous codes
                for( std::size_t k=j+1; k<codes_.size(); ++k )*codes_[k-3] = *codes_[k];
                for( std::size_t k=0; k<3; ++k )
                {
                  delete codes_.back();
                  codes_.pop_back();
                }
              }
              else if( fcn->Name() == wxT("VARTYPE") )
              {
                ExprCodes *e = codes_[jopen+1];
                wxString type(wxT("unknown"));
                if( j-jopen == 2 )
                {
                  if( e->IsaFunction() )type = wxT("function");
                  else if( e->IsaNVariable() )
                  {
                    NumericData nd( e->GetNVarPtr()->GetData() );
                    if( nd.GetNumberOfDimensions() == 0 )type = wxT("scalar");
                    else if( nd.GetNumberOfDimensions() == 1 )type = wxT("vector");
                    else if( nd.GetNumberOfDimensions() == 2 )type = wxT("matrix");
                    else if( nd.GetNumberOfDimensions() == 3 )type = wxT("tensor");
                  }
                  else if( e->IsaTVariable() )
                  {
                    TextVariable *tv = e->GetTVarPtr();
                    if( tv->GetData().GetNumberOfDimensions() == 0 )type = wxT("string");
                    else type = wxT("string array");
                  }
                  else if( e->IsaConstant() )
                  {
                    type = wxT("number");
                  }
                  else if( e->IsaTString() )
                  {
                    type = wxT("literal string");
                  }
                }
                codes_[i]->SetTString( type );
                // shift up to eliminate extraneous codes
                for( std::size_t k=j+1; k<codes_.size(); ++k )*codes_[k-3] = *codes_[k];
                for( std::size_t k=0; k<3; ++k )
                {
                  delete codes_.back();
                  codes_.pop_back();
                }
              }
              break;   // stop searching for arguments
            }
            if( level == 1 )  // within range
            {
              if( codes_[j]->IsaComma() )
              {
                codes_[j]->SetValid( true );
                indexCounter > 0 ? ++indexCounter : ++argCounter;
              }
              else if( codes_[j]->IsaPairBracket() )
              {
                jpair = j;   // jpair points to close & open bracket pair
                codes_[j]->SetValid( true );
                if( indexCounter > 0 )  // already processed pair
                  throw EExpressionError( wxT("only one )( is allowed on a function") );
                if( fcn->Name() == wxT("VARNAME") )
                  throw EExpressionError(  wxT("indices are not allowed on the VARNAME function") );
                indexCounter = 1;  // must be at least one index
              }   // end of code type test
            }   // end level= 0,1
          }   // end jtabl= 1,5
        }   // end of if next code is an opening bracket
      }   // end of if i+1 < codes_.size()
      if( argCounter < fcn->GetMinNumberOfArguments() || argCounter > fcn->GetMaxNumberOfArguments() )
        throw EExpressionError( wxString(wxT("function "))+fcn->Name()+
                                wxT(" has the wrong number of arguments") );
      //fcn->SetArgumentCounter( argCounter );
      if( indexCounter > 0 )
      {
        // switch argument and index fields
        // jopen points to starting opening bracket
        // jpair points to pair bracket
        // jclose points to final closing bracket
        // argument goes to temporary buffer
        //
        ExprCodes **tempCodes = new ExprCodes* [jpair-jopen];
        for( std::size_t j=jopen+1; j<=jpair; ++j )tempCodes[j-jopen-1] = codes_[j];
        std::size_t ntemp = jpair-jopen-1;
        //
        // index goes to argument field
        //
        for( std::size_t j=jpair+1; j<jclose; ++j )codes_[j-jpair+jopen] = codes_[j];
        //
        // insert pair bracket character after new argument
        //
        jpair = jopen+jclose-jpair;
        codes_[jpair] = tempCodes[ntemp];
        //
        // argument goes to index field
        //
        for( std::size_t j=0; j<ntemp; ++j )codes_[jpair+j+1] = tempCodes[j];
        delete [] tempCodes;
      }
      code->SetIndexCntr( indexCounter );
      code->SetArgCntr( argCounter );
    }
    else if( code->IsaNVariable() )
    {
      NumericVariable *v = code->GetNVarPtr();
      std::size_t indexCounter = 0;
      if( i < codes_.size()-1 )  // variable may have arguments
      {
        if( codes_[i+1]->IsaOpeningBracket() )
        {
          indexCounter = 1;   // must be at least one argument
          std::size_t level = 0;
          //
          // level is the depth of brackets following the variable name
          // the number of arguments is 1+ the number of commas at level 1
          //
          for( std::size_t j=i+1; j<codes_.size(); ++j )
          {
            if( codes_[j]->IsaClosingBracket() )--level;
            if( codes_[j]->IsaOpeningBracket() )++level;
            if( !level )break;         // end search for arguments
            if( level == 1 )          // within range
            {
              if( codes_[j]->IsaComma() )
              {
                if( v->GetData().GetNumberOfDimensions() == 0 )
                  throw EExpressionError(
                    wxString(wxT("scalar variable "))+v->GetName()+
                    wxT(" cannot have indices") );
                ++indexCounter;
                if( indexCounter > v->GetData().GetNumberOfDimensions() )
                  throw EExpressionError( wxString(wxT("variable "))+v->GetName()+
                                          wxT(" has too many indices") );
                codes_[j]->SetValid( true );  // mark comma as valid
              }
              else if( codes_[j]->IsaHash() )
              {
                codes_[j]->SetImmediateValue( v->GetData().GetDimMag(0) );
              }
              else if( codes_[j]->IsaAsterisk() )
              {
                codes_.push_back( new ExprCodes );
                codes_.push_back( new ExprCodes );
                for( int jj=codes_.size()-3; jj>=static_cast<int>(j+1); --jj )*codes_[jj+2] = *codes_[jj];
                codes_[j]->SetImmediateValue( 1 );
                codes_[j]->SetValid( true );
                codes_[j+1]->SetColon();
                codes_[j+1]->SetValid( true );
                codes_[j+2]->SetImmediateValue( v->GetData().GetDimMag(indexCounter-1) );
                codes_[j+2]->SetValid( true );
              }
            }
          }   // end of j for loop
        }   // end of if opening bracket
      }   // end of if i < codes_.size()
      if( indexCounter>0 && indexCounter!=v->GetData().GetNumberOfDimensions() )
        throw EExpressionError( wxString(wxT("variable "))+v->GetName()+
                                wxT(" has wrong number of indices") );
      code->SetIndexCntr( indexCounter );
    }
    else if( code->IsaTVariable() )
    {
      TextVariable *v = code->GetTVarPtr();
      if( i == codes_.size()-1 )     // text variable is last code
      {
        if( v->GetData().GetNumberOfDimensions() == 1 )
          throw EExpressionError( wxString(wxT("array text variable "))+v->GetName()+
                                  wxT(" has no array subscript") );
        continue;
      }
      if( code->GetInsertedTVar() )       // inserted text variable due to hash
      {
        code->SetInsertedTVar( false );
        continue;
      }
      bool arg1 = false;
      bool arg2 = false;
      std::size_t jPair;
      if( codes_[i+1]->IsaOpeningBracket() )
      {
        std::size_t jopen = i+1;
        std::size_t level = 0;
        for( std::size_t j=jopen; j<codes_.size(); ++j )
        {
          if( codes_[j]->IsaClosingBracket() )--level;
          if( codes_[j]->IsaOpeningBracket() )++level;
          if( level == 0 )break;     // end search for indices
          if( level == 1 )           // within range
          {
            if( codes_[j]->IsaComma() )
              throw EExpressionError( wxString(wxT("comma not allowed in text variable "))+
                                      v->GetName()+wxT(" index") );
            else if( codes_[j]->IsaPairBracket() )
            {
              if( arg1 )
                throw EExpressionError( wxString(wxT("text variable "))+v->GetName()+
                                        wxT(" has too many indices") );
              arg1 = true;
              jPair = j;
            }
            else if( codes_[j]->IsaHash() )
            {
              if( arg1 ) // processing second index
              {
                std::size_t oSize = codes_.size();
                for( std::size_t k=0; k<3+jPair-i; ++k )codes_.push_back( new ExprCodes );
                for( std::size_t k=oSize-1; k>j; --k )*codes_[k+3+jPair-i] = *codes_[k];
                codes_[j]->SetFcnPtr(FunctionTable::GetTable()->GetFunction(wxT("CLEN")));
                codes_[j+1]->SetOpeningBracket();
                for( std::size_t k=i; k<jPair; ++k )
                {
                  *codes_[j+2+k-i] = *codes_[k];
                }
                codes_[j+2]->SetInsertedTVar( true );
                codes_[j+2+jPair-i]->SetClosingBracket();
                codes_[j+3+jPair-i]->SetClosingBracket();
                j += 3+jPair;
              }
              else              // processing first index
              {
                if( v->GetData().GetNumberOfDimensions() == 0 ) // scalar text variable
                  codes_[j]->SetImmediateValue( v->GetData().GetScalarValue().size() );
                else                                            // array text variable
                  codes_[j]->SetImmediateValue( v->GetData().GetData().size() ); // max array index
              }
            }
            else if( codes_[j]->IsaAsterisk() )
              throw EExpressionError( wxString(wxT("wild card not allowed in text variable "))+
                                      v->GetName()+wxT(" index") );
          }   // end of if level=1
        }   // end of j for loop
        if( !arg1 )arg1 = true; // must be >= 1 argument
        else       arg2 = true; // must be 2 arguments
      }
      else          // unindexed text variable
      {
        if( v->GetData().GetNumberOfDimensions() == 1 )
          throw EExpressionError(
            wxString(wxT("array text variable "))+v->GetName()+
            wxT(" must have an array index") );
      }        // opening bracket
      if( v->GetData().GetNumberOfDimensions() == 0 )
      {
        if( arg2 )throw EExpressionError( wxString(wxT("text variable "))+v->GetName()+
                                          wxT(" is not an array text variable") );
      }
      else
      {
        if( !arg1 )throw EExpressionError( wxString(wxT("array text variable "))+v->GetName()+
                                           wxT(" must have an array index") );
      }
    }   // test of code types
  }   // loop over all codes
}

bool Workspace::HandleFunctionsAndText()
{
  bool madeNewWorkspace = false;
  for( std::size_t i=0; i<codes_.size(); ++i )
  {
    ExprCodes *code = codes_[i];
    if( code->IsaFunction() )
    {
      int indexCounter = 0;
      int argCounter = 0;
      FunctionDefinition *fcn = code->GetFcnPtr();
      if( i < codes_.size()-1 && !code->GetFunctionIsDone() )
      {
        code->SetFunctionIsDone();
        if( codes_[i+1]->IsaOpeningBracket() )
        {
          argCounter = 1;    // must be at least one argument
          std::size_t level = 0;  // the depth of brackets following the function name
          //
          // The number of arguments is the number of commas (at level 1) + 1
          //
          std::size_t k = i+1;
          for( std::size_t j=i+1; j<codes_.size(); ++j )
          {
            if( codes_[j]->IsaClosingBracket() )--level;
            else if( codes_[j]->IsaOpeningBracket() )++level;
            if( level == 0 )
            {
              if( indexCounter == 0 )
              {
                if( fcn->IsMixed2Num() )
                {
                  // only extract a character argument into a Workspace
                  // k points to the opening bracket or comma,
                  // j points to the closing bracket ( in the original Workspace )
                  //
                  if( fcn->ArgTypeIsCharacter(argCounter-1) )
                  {
                    if( fcn->Name() == wxT("EVALUATE") )
                    {
                      type_ = NUMERIC;
                      // remove the EVALUATE function
                      for( std::size_t ii=i; ii<codes_.size()-1; ++ii )*codes_[ii] = *codes_[ii+1];
                      delete codes_.back();
                      codes_.erase( codes_.end()-1 );
                    }
                    else
                    {
                      MakeNewWorkspace( j, k, CHARACTER );
                    }
                    madeNewWorkspace = true;
                    j = k+2;
                  }
                }
                else if( fcn->IsMixed2Char() )
                {
                  if( fcn->ArgTypeIsNumeric( argCounter-1 ) )
                  {
                    // only extract a numeric argument into a Workspace
                    //
                    MakeNewWorkspace( j, k, NUMERIC );
                    madeNewWorkspace = true;
                    j = k+2;
                  }
                }
              }
              break;   // stop searching for arguments
            }
            if( level == 1 )  // within range
            {
              if( codes_[j]->IsaComma() )
              {
                if( indexCounter > 0 )    // processing function index
                {
                  ++indexCounter;
                  k = j;
                }
                else
                {
                  if( fcn->IsMixed2Num() )
                  {
                    if( fcn->ArgTypeIsCharacter( argCounter-1 ) )
                    {
                      // only extract a character argument into a Workspace
                      //
                      if( fcn->Name() == wxT("EVALUATE") )
                      {
                        type_ = NUMERIC;
                        // remove the EVALUATE function
                        for( std::size_t ii=i; ii<codes_.size()-1; ++ii )*codes_[ii] = *codes_[ii+1];
                        delete codes_.back();
                        codes_.erase( codes_.end()-1 );
                      }
                      else
                      {
                        MakeNewWorkspace( j, k, CHARACTER );
                      }
                      madeNewWorkspace = true;
                      j = k+2;
                    }
                  }
                  else if( fcn->IsMixed2Char() )
                  {
                    if( fcn->ArgTypeIsNumeric( argCounter-1 ) )
                    {
                      // only extract a numeric argument into a Workspace
                      //
                      MakeNewWorkspace( j, k, NUMERIC );
                      madeNewWorkspace = true;
                      j = k+2;
                    }
                  }
                  k = j;
                  ++argCounter;
                }
              }
              else if( codes_[j]->IsaPairBracket() )
              {
                indexCounter = 1;  // must be at least one index
                if( fcn->IsMixed2Num() )
                {
                  if( fcn->ArgTypeIsCharacter( argCounter-1 ) )
                  {
                    if( fcn->Name() == wxT("EVALUATE") )
                    {
                      type_ = NUMERIC;
                      // remove the EVALUATE function
                      for( std::size_t ii=i; ii<codes_.size()-1; ++ii )*codes_[ii] = *codes_[ii+1];
                      delete codes_.back();
                      codes_.erase( codes_.end()-1 );
                    }
                    else
                    {
                      MakeNewWorkspace( j, k, CHARACTER );
                    }
                    madeNewWorkspace = true;
                    j = k+2;
                  }
                }
                else if( fcn->IsMixed2Char() )
                {
                  if( fcn->ArgTypeIsNumeric( argCounter-1 ) )
                  {
                    MakeNewWorkspace( j, k, NUMERIC );
                    madeNewWorkspace = true;
                    j = k+2;
                  }
                }
                k = j;
              }   // end of code type test
            }   // end level= 0,1
          }   // end jtabl= 1,5
        }   // end of if next code is an opening bracket
      }   // end of if i+1 < codes_.size()
    }
    else if( code->IsaTVariable() )
    {
      if( i == codes_.size()-1 )continue;     // text variable is last code
      Workspace *argPtr1 = NULL;
      Workspace *argPtr2 = NULL;
      TextVariable *v = code->GetTVarPtr();
      if( codes_[i+1]->IsaOpeningBracket() )
      {
        std::size_t j;
        std::size_t jopen = i+1;
        std::size_t level = 0;
        for( j=jopen; j<codes_.size(); ++j )
        {
          if( codes_[j]->IsaClosingBracket() )--level;
          if( codes_[j]->IsaOpeningBracket() )++level;
          if( level == 0 )break;     // end search for indices
          if( level == 1 )           // within range
          {
            if( codes_[j]->IsaPairBracket() )
            {
              int expLevel = GetLevel() + 2;
              if( expLevel > expression_->GetHighestLevel() )expression_->SetHighestLevel(expLevel);
              Workspace *aWs = new Workspace(expLevel);
              aWs->SetExpression( expression_ );
              ExprCodes *aCode = new ExprCodes;
              aCode->SetOpeningBracket();
              aWs->codes_.push_back( aCode );
              for( std::size_t k=jopen+1; k<=j-1; ++k )
              {
                ExprCodes *e = new ExprCodes;
                aWs->GetCodes().push_back( e );
                *e = *codes_[k];
              }
              ExprCodes *bCode = new ExprCodes;
              bCode->SetClosingBracket();
              aWs->GetCodes().push_back( bCode );
              aWs->SetCodesAreParsed();
              aWs->SetNumeric();
              expression_->AddWorkspace( aWs );
              std::size_t codeSize = codes_.size();
              for( std::size_t k=j+1; k<codeSize; ++k )*codes_[k-j+jopen] = *codes_[k];
              for( std::size_t k=1; k<=j-jopen; ++k )
              {
                delete codes_[codeSize-k];
                codes_.pop_back();
              }
              argPtr1 = aWs;
              j = jopen;
            }
          }   // end of if level=1
        }   // end of j for loop
        int expLevel = GetLevel() + 1;
        if( expLevel > expression_->GetHighestLevel() )expression_->SetHighestLevel(expLevel);
        Workspace *aWs = new Workspace(expLevel);
        aWs->SetExpression( expression_ );
        ExprCodes *aCode = new ExprCodes;
        aCode->SetOpeningBracket();
        aWs->GetCodes().push_back( aCode );
        for( std::size_t k=jopen+1; k<=j-1; ++k )
        {
          ExprCodes *e = new ExprCodes;
          aWs->GetCodes().push_back( e );
          *e = *codes_[k];
        }
        ExprCodes *bCode = new ExprCodes;
        bCode->SetClosingBracket();
        aWs->GetCodes().push_back( bCode );
        aWs->SetNumeric();
        aWs->SetCodesAreParsed();
        expression_->AddWorkspace( aWs );
        std::size_t codeSize = codes_.size();
        for( std::size_t k=j+1; k<codeSize; ++k )*codes_[k-j+jopen-1] = *codes_[k];
        for( std::size_t k=1; k<=j-jopen+1; ++k )
        {
          delete codes_[codeSize-k];
          codes_.pop_back();
        }
        j = jopen-1;
        if( argPtr1 == NULL )argPtr1 = aWs; // must be >= 1 argument
        else                 argPtr2 = aWs; // must be 2 arguments
      }
      if( v->GetData().GetNumberOfDimensions() == 0 )
        code->SetTCIndexPtr( argPtr1 );
      else
      {
        code->SetTAIndexPtr( argPtr1 );  // array  index
        code->SetTCIndexPtr( argPtr2 );  // character index
      }
    }   // end of if test of code types
  }   // end of loop over all codes
  return madeNewWorkspace;
}

void Workspace::RPNconvert()
{
  // 4th Pass:
  // Loop over the input codes and reorder into Reverse Polish Notation (RPN).
  //   oprPos_[] = Position of binary operator token (found when taken off stack)
  //   oprAnd_[] = Start of 1st operand of binary operator
  //   fcnPos_[] = Position of function name token
  //   fcnArg_[] = Position of start of first function argument
  //   fcnIdx_[] = Position of start of first function index
  //   varPos_[] = Position of indexed variable name token
  //   varIdx_[] = Position of start of first index
  //   All of these locations refer to position in output stack
  //                                             
  std::size_t codeSize = codes_.size();
  oprPos_.resize( codeSize );
  std::vector<int>().swap( oprAnd_ );
  oprAnd_.insert( oprAnd_.begin(), codeSize, -1 );
  fcnPos_.resize( codeSize );
  fcnArg_.resize( codeSize );
  fcnIdx_.resize( codeSize );
  varPos_.resize( codeSize );
  varIdx_.resize( codeSize );
  rngPos_.resize( codeSize );
  rngIdx_.resize( codeSize );
  lstPos_.resize( codeSize );
  lstIdx_.resize( codeSize );
  std::deque<ExprCodes*> aStack;
  std::deque<ExprCodes*> outBuf;
  std::deque<ExprCodes*>::const_iterator end( codes_.end() );
  for( std::deque<ExprCodes*>::const_iterator i(codes_.begin()); i!=end; ++i )
  {
    ExprCodes *code = *i;
    if( code->IsaFunction() )
    {
      if( code->GetArgCntr() > 0 )       // function has arguments
      {
        ++functionWithArgumentsCntr_;
        if( code->GetIndexCntr() > 0 )   // indexed function
        {
          std::deque<ExprCodes*>::const_iterator jpair( end );
          int level = 0;
          for( std::deque<ExprCodes*>::const_iterator j(i+1); j!=end; ++j )
          {
            ExprCodes *jCode = *j;
            if( jCode->IsaOpeningBracket() )++level;
            if( jCode->IsaClosingBracket() )--level;
            if( jCode->IsaPairBracket() && level == 1 )
            {
              jpair = j;
              break;
            }
          }
          assert( jpair != end ); //  )( not found
          (*jpair)->SetCntr( functionWithArgumentsCntr_ );
          fcnIdx_[functionWithArgumentsCntr_-1] = outBuf.size(); // position of start of index field
        }
        else                        // next output token is first argument
        {
          fcnArg_[functionWithArgumentsCntr_-1] = outBuf.size(); // position of start of arg. field
        }
        code->SetCntr( functionWithArgumentsCntr_ );
        ExprCodes *nextCode = *(i+1);
        nextCode->SetRefFunction();            // flag following opening bracket as
        nextCode->SetCntr( functionWithArgumentsCntr_ );  // start of function arguments
        aStack.push_front( code );
      }
      else                                   // function has no arguments
      {                                      // push directly onto output buffer
        outBuf.push_back( code );
        if( oprAnd_[operatorCntr_] == -1 ||
            outBuf.back()->IsaFunction() || outBuf.back()->IsaNVariable() ||
            outBuf.back()->IsaConstant() || outBuf.back()->IsaImmediateValue() ||
            outBuf.back()->IsaWorkspace() )
          oprAnd_[operatorCntr_] = outBuf.size()-1;
      }
    }
    else if( code->IsaNVariable() )
    {
      if( code->GetIndexCntr() > 0 )         // variable has indices
      {
        code->SetCntr( ++indexedVariableCntr_ );
        ExprCodes *nextCode = *(i+1);
        nextCode->SetRefNVariable();            // flag following opening bracket as
        nextCode->SetCntr( indexedVariableCntr_ ); // start of variable arguments
        aStack.push_front( code );
        varIdx_[indexedVariableCntr_-1] = outBuf.size(); // next output token is first index
      }
      else                                   // variable has no indices
      {                                      // push directly onto output buffer
        outBuf.push_back( code );
        if( oprAnd_[operatorCntr_] == -1 ||
            outBuf.back()->IsaFunction() || outBuf.back()->IsaNVariable() ||
            outBuf.back()->IsaConstant() || outBuf.back()->IsaImmediateValue() ||
            outBuf.back()->IsaWorkspace() )
          oprAnd_[operatorCntr_] = outBuf.size()-1;
      }
    }
    else if( code->IsaConstant() )
    {                                        // push directly onto output buffer
      outBuf.push_back( code );
      if( oprAnd_[operatorCntr_] == -1 ||
          outBuf.back()->IsaFunction() || outBuf.back()->IsaNVariable() ||
          outBuf.back()->IsaConstant() || outBuf.back()->IsaImmediateValue() ||
          outBuf.back()->IsaWorkspace() )
        oprAnd_[operatorCntr_] = outBuf.size()-1;
    }
    else if( code->IsaOperator() )
    {
      code->SetCntr( ++operatorCntr_ );
      OperatorDefinition *op = code->GetOpPtr();
      //
      // |operatorPriority| is the operator priority or precedence
      // The sign gives associativity, with + for left and - for right
      //
      int operatorPriority;
      if( code->IsBinary() )
        operatorPriority = op->BinaryPriority();
      else
        operatorPriority = op->UnaryPriority();
      //
      // Pop codes from the stack onto the output buffer until an
      // operator of lower precedence than that of the current,
      // or until an opening bracket, comma or colon is encountered
      //
      std::size_t aStackSize( aStack.size() );
      for( std::size_t j = 0; j < aStackSize; ++j )
      {
        ExprCodes *stackCode = aStack.front();
        if( stackCode->IsaFunction() )
        {                              // pop function onto output buffer
          aStack.pop_front();
          outBuf.push_back( stackCode );
          if( oprAnd_[operatorCntr_-1] == -1 ) // current operator
            oprAnd_[operatorCntr_-1] = outBuf.size()-1;
          if( stackCode->GetArgCntr() != 0 )                    // function has arguments
            fcnPos_[stackCode->GetCntr()-1] = outBuf.size()-1;   // function location
        }
        else if( stackCode->IsaNVariable() )
        {                              // pop variable onto output buffer
          aStack.pop_front();
          outBuf.push_back( stackCode );
          if( oprAnd_[operatorCntr_-1] == -1 )
            oprAnd_[operatorCntr_-1] = outBuf.size()-1;
          if( stackCode->GetIndexCntr() != 0)                 // variable has indices
            varPos_[stackCode->GetCntr()-1] = outBuf.size()-1; // variable location
        }
        else if( stackCode->IsaOperator() )
        {
          OperatorDefinition *op2 = stackCode->GetOpPtr();
          int opPrio;
          if( stackCode->IsBinary() )
            opPrio = op2->BinaryPriority();
          else
            opPrio = op2->UnaryPriority();
          //
          // If the stack operator has a lower precedence than the current operator,
          // stop popping the stack and push the current operator onto the stack.
          // If not, then pop the operator off the stack onto the output buffer.
          //
          if( operatorPriority < 0 )
          {
            if( abs(opPrio) <= abs(operatorPriority) )break;
          }
          else
          {
            if( abs(opPrio) <  abs(operatorPriority) )break;
          }
          //
          // stack operator has a higher precedence than input stream operator
          //
          aStack.pop_front();
          if( op2->IsArray() )    // array operator
          {
            ExprCodes *aCode = new ExprCodes; // write no-op code to output stream
            aCode->SetNoop();
            aCode->SetRefOperator();
            aCode->SetCntr( stackCode->GetCntr() );
            outBuf.push_back( aCode );
          }
          outBuf.push_back( stackCode );
          oprPos_[stackCode->GetCntr()-1] = outBuf.size()-1;
        }
        else if( stackCode->IsaOpeningBracket() )
        {
          break;                                  // stop popping the stack
        }
        else if( stackCode->IsaRangeVector() )
        {
          if( oprAnd_[operatorCntr_-1] == -1 )oprAnd_[operatorCntr_-1] = outBuf.size()-1;
          rngPos_[stackCode->GetCntr()-1] = outBuf.size()-1;
          aStack.pop_front();
          outBuf.push_back( stackCode );
        }
        else if( stackCode->IsaListVector() )
        {
          if( oprAnd_[operatorCntr_-1] == -1 )oprAnd_[operatorCntr_-1] = outBuf.size()-1;
          lstPos_[stackCode->GetCntr()-1] = outBuf.size()-1;
          aStack.pop_front();
          outBuf.push_back( stackCode );
        }
      }   // end of j loop over codes on the stack
      if( code->IsBinary() )  // binary operator on input stream
      {
        // pop a no-op code to the output buffer
        // to mark the start of the second operand
        //
        ExprCodes *lastCode = outBuf.back();
        //
        // ensure only one no-op written to output stream
        //
        if( lastCode->IsaNoop() )
        {
          lastCode->SetCntr( operatorCntr_ );
        }
        else
        {
          ExprCodes *aCode = new ExprCodes;
          aCode->SetNoop();
          aCode->SetRefOperator();
          aCode->SetCntr( operatorCntr_ );
          outBuf.push_back( aCode );
        }
      }
      aStack.push_front( code );
      //
      // If the token pushed onto the stack is an operator, then the start
      // position oprAnd_[] of first operand is saved. If the current token
      // on the output stack is a variable or constant, then oprAnd_[] already
      // contains the correct location. If this however is an operator also,
      // then the start of the first operand will coincide with the start of
      // that operator, the index (operatorCntr_) of which should be one less
      // than that of the operator currently being pushed onto the operator stack
      //
      if( code->IsUnary() )
      {
        // define start of unary operand, next output token is operand
        oprAnd_[operatorCntr_-1] = outBuf.size();
      }
      else                         // binary operator
      {
        std::size_t nout = outBuf.size()-1;
        ExprCodes *lastCode = outBuf[nout];
        if( lastCode->IsaFunction() )
        {
          if( lastCode->GetArgCntr() )
          {
            fcnPos_[lastCode->GetCntr()-1] = nout;
            oprAnd_[operatorCntr_-1] = fcnArg_[lastCode->GetCntr()-1];
          }
        }
        else if( lastCode->IsaNVariable() )
        {
          if( lastCode->GetIndexCntr() > 0 )
          {
            if( !lastCode->GetNVarPtr()->GetData().GetNumberOfDimensions() == 0 )
            {
              varPos_[lastCode->GetCntr()-1] = nout;
              oprAnd_[operatorCntr_-1] = varIdx_[lastCode->GetCntr()-1];
            }
          }
        }
        else if( lastCode->IsaOperator() )
        {
          oprAnd_[operatorCntr_-1] = oprAnd_[lastCode->GetCntr()-1];
        }
        else if( lastCode->IsaNoop() )
        {
          ExprCodes *prevCode = outBuf[nout-1];
          if( prevCode->IsaFunction() )
          {
            if( prevCode->GetArgCntr() > 0 )
            {
              fcnPos_[prevCode->GetCntr()-1] = nout-1;
              oprAnd_[operatorCntr_-1] = fcnArg_[prevCode->GetCntr()-1];
            }
          }
          else if( prevCode->IsaNVariable() )
          {
            if( prevCode->GetIndexCntr() > 0 )
            {
              varPos_[prevCode->GetCntr()-1] = nout-1;
              oprAnd_[operatorCntr_-1] = varIdx_[prevCode->GetCntr()-1];
            }
          }
          else if( prevCode->IsaOperator() )
          {
            oprAnd_[operatorCntr_-1] = oprAnd_[prevCode->GetCntr()-1];
          }
          else if( prevCode->IsaRangeVector() )
          {
            rngPos_[prevCode->GetCntr()-1] = nout-1;
            oprAnd_[operatorCntr_-1] = rngIdx_[prevCode->GetCntr()-1];
          }
          else if( prevCode->IsaListVector() )
          {
            lstPos_[prevCode->GetCntr()-1] = nout-1;
            oprAnd_[operatorCntr_-1] = lstIdx_[prevCode->GetCntr()-1];
          }
        }
        else if( lastCode->IsaRangeVector() )
        {
          rngPos_[lastCode->GetCntr()-1] = nout-1;
          oprAnd_[operatorCntr_-1] = rngIdx_[lastCode->GetCntr()-1];
        }
        else if( lastCode->IsaListVector() )
        {
          lstPos_[lastCode->GetCntr()-1] = nout-1;
          oprAnd_[operatorCntr_-1] = lstIdx_[lastCode->GetCntr()-1];
        }
      }
    }
    else if( code->IsaOpeningBracket() )
    {
      // opening bracket: push onto stack
      // closing bracket: pop stack up to corresponding opening bracket and discard
      //    pair bracket: pop stack up to corresponding opening bracket
      //           comma: pop stack up to corresponding opening bracket
      //           colon: pop stack up to corresponding opening bracket
      //       semicolon: pop stack up to corresponding opening bracket
      //            hash: already taken care of
      //        asterisk: already taken care of
      //
      aStack.push_front( code );
    }
    else if( code->IsaClosingBracket() || code->IsaPairBracket() ||
             code->IsaComma()          || code->IsaSemicolon() ||
             code->IsaColon() )
    {
      // pop codes from the stack onto the output buffer until the
      // corresponding ( is encountered
      //
      std::size_t stackSize( aStack.size() );
      for( std::size_t j=0; j<stackSize; ++j )
      {
        ExprCodes *stackCode = aStack.front();
        if( stackCode->IsaFunction() )
        {                              // pop function onto output buffer
          if( !code->IsaClosingBracket() )
          {
            // write no-op code to output stream
            //
            ExprCodes *aCode = new ExprCodes;
            aCode->SetNoop();
            aCode->SetRefFunction();
            aCode->SetCntr( stackCode->GetCntr() );
            outBuf.push_back( aCode );
          }
          outBuf.push_back( stackCode );
          if( stackCode->GetArgCntr() != 0 )                    // function has arguments
            fcnPos_[stackCode->GetCntr()-1] = outBuf.size()-1;     // function location
        }
        else if( stackCode->IsaNVariable() )
        {                      // pop variable onto output buffer
          if( stackCode->GetIndexCntr() != 0 )
          {
            if( !code->IsaClosingBracket() )
            {
              // write no-op code to output stream
              //
              ExprCodes *aCode = new ExprCodes;
              aCode->SetNoop();
              aCode->SetRefNVariable();
              aCode->SetCntr( stackCode->GetCntr() );
              outBuf.push_back( aCode );
            }
          }
          varPos_[stackCode->GetCntr()-1] = outBuf.size();
          outBuf.push_back( stackCode );
          if( oprAnd_[operatorCntr_] == -1 )    // refers to next operator
            oprAnd_[operatorCntr_] = outBuf.size();
        }
        else if( stackCode->IsaOperator() )
        {                      // pop operator onto output buffer
          OperatorDefinition *op = stackCode->GetOpPtr();
          if( op->IsArray() )  // array operator
          {                    // write no-op code to output stream
            ExprCodes *aCode = new ExprCodes;
            aCode->SetNoop();
            aCode->SetRefOperator();
            aCode->SetCntr( stackCode->GetCntr() );
            outBuf.push_back( aCode );
          }
          outBuf.push_back( stackCode );
          oprPos_[stackCode->GetCntr()-1] = outBuf.size()-1;
        }
        else if( stackCode->IsaOpeningBracket() )
        {
          if( code->IsaClosingBracket() )
          {
            delete stackCode;
            aStack.pop_front();
          }
          else if( code->IsaColon() )// ensure only one no-op written to output stream
          {
            if( outBuf.back()->IsaNoop() )
            {
              outBuf.back()->SetReferenceType( stackCode->GetReferenceType() );
              outBuf.back()->SetCntr( stackCode->GetCntr() );
            }
            else
            {
              ExprCodes *aCode = new ExprCodes;
              aCode->SetNoop();
              aCode->SetCntr( stackCode->GetCntr() );
              aCode->SetReferenceType( stackCode->GetReferenceType() );
              outBuf.push_back( aCode );
            }
          }
          else if( code->IsaSemicolon() )// ensure only one no-op written to output stream
          {
            if( outBuf.back()->IsaNoop() )
            {
              outBuf.back()->SetReferenceType( stackCode->GetReferenceType() );
              outBuf.back()->SetCntr( stackCode->GetCntr() );
            }
            else
            {
              ExprCodes *aCode = new ExprCodes;
              aCode->SetNoop();
              aCode->SetCntr( stackCode->GetCntr() );
              aCode->SetReferenceType( stackCode->GetReferenceType() );
              outBuf.push_back( aCode );
            }
          }
          else if( code->IsaComma() )
          {
            if( outBuf.back()->IsaNoop() )
            {
              outBuf.back()->SetReferenceType( stackCode->GetReferenceType() );
              outBuf.back()->SetCntr( stackCode->GetCntr() );
            }
            else
            {
              ExprCodes *aCode = new ExprCodes;
              aCode->SetNoop();
              aCode->SetReferenceType( stackCode->GetReferenceType() );
              aCode->SetCntr( stackCode->GetCntr() );
              outBuf.push_back( aCode );
            }
          }
          break; // stop popping the stack
        }
        else if( stackCode->IsaRangeVector() )
        {                       // pop onto output buffer
          if( oprAnd_[operatorCntr_] == -1 )oprAnd_[operatorCntr_] = outBuf.size()-1;
          if( !code->IsaClosingBracket() ) // write no-op code to output stream
          {
            ExprCodes *aCode = new ExprCodes;
            aCode->SetNoop();
            if( code->IsaPairBracket() )
            {
              aCode->SetRefFunction();
              aCode->SetCntr( code->GetCntr() );
            }
            else
            {
              aCode->SetRefRangeVector();
              aCode->SetCntr( stackCode->GetCntr() );
            }
            outBuf.push_back( aCode );
          }
          rngPos_[stackCode->GetCntr()-1] = outBuf.size();
          outBuf.push_back( stackCode );
        }
        else if( stackCode->IsaListVector() )  // pop onto output buffer
        {
          if( oprAnd_[operatorCntr_] == -1 )oprAnd_[operatorCntr_] = outBuf.size()-1;
          if( !code->IsaClosingBracket() ) // write no-op code to output stream
          {
            ExprCodes *aCode = new ExprCodes;
            aCode->SetNoop();
            aCode->SetRefListVector();
            aCode->SetCntr( stackCode->GetCntr() );
            outBuf.push_back( aCode );
          }
          lstPos_[stackCode->GetCntr()-1] = outBuf.size();
          outBuf.push_back( stackCode );
        }
        aStack.pop_front();
      }   // end of j loop over stack codes
      if( code->IsaPairBracket() )
      {
        ExprCodes *aCode = new ExprCodes;
        aCode->SetNoop();
        aCode->SetReferenceType( code->GetReferenceType() );
        aCode->SetCntr( code->GetCntr() );
        outBuf.push_back( aCode );
        fcnArg_[code->GetCntr()-1] = outBuf.size();
      }
      delete code;
    }
    else if( code->IsaImmediateValue() )
    {
      outBuf.push_back( code );
      if( oprAnd_[operatorCntr_] == -1 ||
          outBuf.back()->IsaFunction() || outBuf.back()->IsaNVariable() ||
          outBuf.back()->IsaConstant() || outBuf.back()->IsaImmediateValue() ||
          outBuf.back()->IsaWorkspace() )
        oprAnd_[operatorCntr_] = outBuf.size()-1;
    }
    else if( code->IsaRangeVector() )
    {
      //
      // next output token is first index
      //
      rngIdx_[code->GetCntr()-1] = outBuf.size();
      aStack.push_front( code );
    }
    else if( code->IsaListVector() )
    {
      //
      // next output token is first item
      //
      lstIdx_[code->GetCntr()-1] = outBuf.size();
      aStack.push_front( code );
    }
    else if( code->IsaWorkspace() )
    {
      outBuf.push_back( code );
      if( oprAnd_[operatorCntr_] == -1 ||
          outBuf.back()->IsaFunction() || outBuf.back()->IsaNVariable() ||
          outBuf.back()->IsaConstant() || outBuf.back()->IsaImmediateValue() ||
          outBuf.back()->IsaWorkspace() )
        oprAnd_[operatorCntr_] = outBuf.size()-1;
    }
    else if( code->IsaTString() )
    {
      outBuf.push_back( code );
    }
  }   // end of loop over all the codes
  //
  // pop the remaining elements of the stack
  //
  std::size_t jFinal( aStack.size() );
  for( std::size_t j = 0; j < jFinal; ++j )
  {
    ExprCodes *stackCode = aStack.front();
    if( stackCode->IsaFunction() )
    {
      //
      // write no-op code to output stream
      //
      ExprCodes *aCode = new ExprCodes;
      aCode->SetNoop();
      aCode->SetRefFunction();
      aCode->SetCntr( stackCode->GetCntr() );
      outBuf.push_back( aCode );
      //
      // store function w/args location
      //
      if( stackCode->GetArgCntr() != 0 )fcnPos_[stackCode->GetCntr()-1] = outBuf.size();
    }
    else if( stackCode->IsaNVariable() )
    {
      if( oprAnd_[operatorCntr_] == -1 )oprAnd_[operatorCntr_] = outBuf.size();
      if( stackCode->GetIndexCntr() )varPos_[stackCode->GetCntr()-1] = outBuf.size();
    }
    else if( stackCode->IsaOperator() )
    {
      OperatorDefinition *op = stackCode->GetOpPtr();
      if( op->IsArray() )
      {
        // write no-op code to output stream
        //
        ExprCodes *aCode = new ExprCodes;
        aCode->SetNoop();
        aCode->SetRefOperator();
        aCode->SetCntr( stackCode->GetCntr() );
        outBuf.push_back( aCode );
      }
      oprPos_[stackCode->GetCntr()-1] = outBuf.size();
    }
    else if( stackCode->IsaRangeVector() )
    {
      if( oprAnd_[operatorCntr_] == -1 )oprAnd_[operatorCntr_] = outBuf.size();
      rngPos_[stackCode->GetCntr()-1] = outBuf.size();
    }
    else if( stackCode->IsaListVector() )
    {
      if( oprAnd_[operatorCntr_] == -1 )oprAnd_[operatorCntr_] = outBuf.size();
      lstPos_[stackCode->GetCntr()-1] = outBuf.size();
    }
    aStack.pop_front();
    outBuf.push_back( stackCode );
  }
  codes_ = outBuf;
  if( codes_.back()->IsaNoop() )
  {
    delete codes_.back();
    codes_.pop_back();
  }
}

void Workspace::SetHierarchy()
{
  // Fifth pass:
  //
  //  Loop over all operands, and if an array operation is involved,
  //  then increment and decrement the hierachy level accordingly at the
  //  start and end of the operator"s range
  //
  hierarchy_.insert( hierarchy_.begin(), codes_.size(), 0 );
  for( std::size_t i=0; i<operatorCntr_; ++i )
  {
    std::size_t opLocation = oprPos_[i];
    std::size_t operandStart = oprAnd_[i];
    OperatorDefinition *op = codes_[opLocation]->GetOpPtr();
    if( op->IsArray() )
    {
      --hierarchy_[opLocation];
      hierarchy_[operandStart] += 2;
      if( opLocation < codes_.size() - 1 )
      {
        if( codes_[opLocation+1]->IsaNoop() )
        {
          if( opLocation < codes_.size() - 2 )--hierarchy_[opLocation+2];
        }
        else
          --hierarchy_[opLocation+1];
      }
    }
  }
  //
  // loop over all functions
  // increment/decrement the hierachy levels where array functions are involved
  //
  for( std::size_t i=0; i<functionWithArgumentsCntr_; ++i )
  {
    std::size_t fcnLocation = fcnPos_[i];
    std::size_t argStart = fcnArg_[i];
    std::size_t indexStart = fcnIdx_[i];
    FunctionDefinition *f = codes_[fcnLocation]->GetFcnPtr();
    if( codes_[fcnLocation]->GetIndexCntr() != 0 )
    {
      --hierarchy_[fcnLocation];
      hierarchy_[argStart] += 2;
      ++hierarchy_[indexStart];
      if( fcnLocation < codes_.size() - 1 )
      {
        if( codes_[fcnLocation+1]->IsaNoop() )
        {
          if( fcnLocation < codes_.size() - 2 )
            hierarchy_[fcnLocation+2] -= 2;
        }
        else
          hierarchy_[fcnLocation+1] -= 2;
      }
    }
    else if( f->IsNum2NumVector() || f->IsMixed2Num() )
    {
      --hierarchy_[fcnLocation];
      hierarchy_[argStart] += 2;
      if( fcnLocation < codes_.size() - 1 )
      {
        if( codes_[fcnLocation+1]->IsaNoop() )
        {
          if( fcnLocation < codes_.size() - 2 )--hierarchy_[fcnLocation+2];
        }
        else
        {
          --hierarchy_[fcnLocation+1];
        }
      }
    }
    else
    {
      --hierarchy_[fcnLocation];
      ++hierarchy_[argStart];
    }
  }
  //
  // Loop over all indexed variables, range and list vectors,
  // increment/decrement the hierachy levels for each index evaluation
  //
  for( std::size_t i=0; i<indexedVariableCntr_; ++i )
  {
    std::size_t varLocation = varPos_[i];
    std::size_t indexStart = varIdx_[i];
    --hierarchy_[varLocation];
    ++hierarchy_[indexStart];
  }
  std::size_t rngCntr = 0;
  for( std::size_t i=0; i<codes_.size(); ++i )
  {
    if( codes_[i]->IsaRangeVector() )
    {
      std::size_t rngLocation = rngPos_[rngCntr];
      std::size_t itemStart = rngIdx_[rngCntr];
      --hierarchy_[rngLocation];
      ++hierarchy_[itemStart];
      ++rngCntr;
    }
  }
  for( std::size_t i=0; i<listVectorCntr_; ++i )
  {
    std::size_t lstLocation = lstPos_[i];
    std::size_t itemStart = lstIdx_[i];
    --hierarchy_[lstLocation];
    ++hierarchy_[itemStart];
  }
  //
  // Loop over all the code values and use the incremental hierarchy
  // changes, accumulated in hierarchy, to prepare the array of actual
  // hierarchy nesting levels in nestingLevel
  //
  codes_[0]->SetNestingLevel( hierarchy_[0] );
  std::size_t j = 1;
  std::deque<ExprCodes*>::const_iterator end( codes_.end() );
  for( std::deque<ExprCodes*>::const_iterator i(codes_.begin()+1); i!=end; ++j, ++i )
  {
    (*i)->SetNestingLevel( (*(i-1))->GetNestingLevel() + hierarchy_[j] );
  }
  int theMinimumNestingLevel = std::numeric_limits<int>::max();
  for( std::deque<ExprCodes*>::const_iterator i(codes_.begin()); i!=end; ++i )
  {
    int n = (*i)->GetNestingLevel();
    if( (*i)->IsaNoop() && n > 0 )(*i)->SetNestingLevel( --n );
    if( n > highestNestingLevel_ )highestNestingLevel_ = n; // total number of levels
    if( n < theMinimumNestingLevel )theMinimumNestingLevel = n; // lowest level used
  }
  assert( theMinimumNestingLevel >= 0 );
  if( theMinimumNestingLevel > 0 )   // shift all codes from theMinimumNestingLevel --> 0
  {
    for( std::deque<ExprCodes*>::const_iterator i(codes_.begin()); i!=end; ++i )
      (*i)->SetNestingLevel( (*i)->GetNestingLevel() - theMinimumNestingLevel );
    highestNestingLevel_ -= theMinimumNestingLevel;
  }
  for( std::deque<ExprCodes*>::const_iterator i(codes_.begin()); i!=end; ++i )
  {
    if( (*i)->IsaNoop() )
    {
      std::size_t cntr = (*i)->GetCntr();
      if( (*i)->IsFlaggedByFunction() )
      {
        if( codes_[fcnPos_[cntr-1]]->GetIndexCntr() == 0 )
          (*i)->SetNestingLevel( codes_[fcnPos_[cntr-1]]->GetNestingLevel() );
      }
      else if( (*i)->IsFlaggedByNVariable() )
        (*i)->SetNestingLevel( codes_[varPos_[cntr-1]]->GetNestingLevel() );
      else if( (*i)->IsFlaggedByOperator() )
        (*i)->SetNestingLevel( codes_[oprPos_[cntr-1]]->GetNestingLevel() );
      else if( (*i)->IsFlaggedByRangeVector() )
      {
        (*i)->SetNestingLevel( codes_[rngPos_[cntr-1]]->GetNestingLevel() );
      }
      else if( (*i)->IsFlaggedByListVector() )
        (*i)->SetNestingLevel( codes_[lstPos_[cntr-1]]->GetNestingLevel() );
    }
  }
  std::vector<std::size_t>().swap( oprPos_ );
  std::vector<int>().swap( oprAnd_ );
  std::vector<std::size_t>().swap( fcnPos_ );
  std::vector<std::size_t>().swap( fcnArg_ );
  std::vector<std::size_t>().swap( fcnIdx_ );
  std::vector<std::size_t>().swap( varPos_ );
  std::vector<std::size_t>().swap( varIdx_ );
  std::vector<std::size_t>().swap( rngPos_ );
  std::vector<std::size_t>().swap( rngIdx_ );
  std::vector<std::size_t>().swap( lstPos_ );
  std::vector<std::size_t>().swap( lstIdx_ );
}

void Workspace::SixthPass()
{
  //  The RPN code sequence obtained must now be broken into portions
  //  containing only scalar operations that can be simply looped over
  //  and evaluated on an element-by-element basis. The final result will
  //  be a tree of such sub-expressions (evaluated as workspaces)
  //
  int highestNestingLevel_2 = highestNestingLevel_;
  for( int level=highestNestingLevel_2; level>=0; --level )
  {
    Workspace *ws = NULL;
    bool addedCodeToWorkspace = false;
    bool pack = true;
    std::deque<ExprCodes*> newCodes;
    for( std::size_t i=0; i<codes_.size(); ++i )
    {
      // loop over the code array, when a new section at the appropriate
      // hierachy (nesting level) is found, define a new workspace variable to
      // replace this section, which is copied into a sub-expression. The resulting
      // array of codes (always <= original array in size) is compressed back
      // into the original codes
      //
      ExprCodes *iCode = codes_[i];
      if( iCode->GetNestingLevel() == level )
      {
        if( !addedCodeToWorkspace && !iCode->IsaNoop() )
        {
          if( i==codes_.size()-1 && codes_.size()>1 ) // >1 code && the last code
          {
            iCode->SetNestingLevel( iCode->GetNestingLevel()-1 );
            newCodes.push_back( iCode );
          }
          else                        // only 1 code || it's not the last code
          {
            ws = new Workspace(level_+1);
            ws->SetExpression( expression_ );
            ws->SetCodesAreParsed();
            ws->SetRPNdone();
            ws->GetCodes().push_back( iCode );
            expression_->AddWorkspace( ws );
            if( iCode->IsaWorkspace() )iCode->GetWorkspacePtr()->SetParent( ws );
            ExprCodes *aCode = new ExprCodes;
            aCode->SetWorkspacePtr( ws );        // replace section in workspace
            aCode->SetNestingLevel( std::max(0,level-1) );
            newCodes.push_back( aCode );
            addedCodeToWorkspace = true;
          }
        }
        else                   // addedCodeToWorkspace or current code is no-op
        {
          if( iCode->IsaNoop() )  // current code is a no-op
          {
            delete iCode;
          }
          else
          {
            ws->GetCodes().push_back( iCode );  // add current code to workspace
            if( iCode->IsaWorkspace() )iCode->GetWorkspacePtr()->SetParent( ws );
            ws->SetNcFlag( true );
            addedCodeToWorkspace = true;
          }
        }
      }
      else if( iCode->GetNestingLevel() < level )
      {
        if( addedCodeToWorkspace )
        {
          std::deque<ExprCodes*> &cd = ws->GetCodes();
          if( cd.size() == 1 )
          {
            int n = newCodes.back()->GetNestingLevel();
            cd.front()->SetNestingLevel( n );
            *newCodes.back() = *cd.front();
            expression_->RemoveWorkspace( ws );
            delete ws;
            ws = NULL;
          }
          else
          {
            ExprCodes *lastCode = cd.back();
            if( lastCode->IsaRangeVector() &&
                lastCode->GetRangeVectorPtr()->GetItemCntr() == cd.size()-1 )
            {
              ws->SetNcFlag( !ws->GetNcFlag() );
            }
            else if( lastCode->IsaListVector() &&
                     lastCode->GetListVectorPtr()->GetItemCntr() == cd.size()-1 )
            {
              //ws->SetNcFlag( !ws->GetNcFlag() );
              ws->SetNcFlag( true );
            }
            else if( lastCode->IsaFunction() )
            {
              FunctionDefinition *f = lastCode->GetFcnPtr();
              if( f->IsNum2NumVector() || f->IsMixed2Num() ||
                  lastCode->GetIndexCntr() > 0 )
              {
                pack = false;
                ws->SetNcFlag( true );
                newCodes.back()->SetIndexCntr( lastCode->GetIndexCntr() );
              }
            }
            else if( lastCode->IsaOperator() )
            {
              OperatorDefinition *op = lastCode->GetOpPtr();
              if( op->IsArray() )
              {
                pack = false;
                ws->SetNcFlag( true );
              }
            }
            if( pack )
            {
              std::size_t cdSize = cd.size();
              for( std::size_t j=0; j<cdSize; ++j )
              {
                if( cd.at(j)->IsaWorkspace() )
                {
                  Workspace *w = cd.at(j)->GetWorkspacePtr();
                  std::deque<ExprCodes*> &cdj = w->GetCodes();
                  if( !w->GetNcFlag() )
                  {
                    ExprCodes *etmp = cd.at(j);
                    cd.insert( cd.begin()+j, cdj.begin(), cdj.end() );
                    j += cdj.size();
                    cd.erase( cd.begin()+j );
                    --j;
                    cdSize = cd.size();
                    std::deque<ExprCodes*>().swap( cdj );
                    expression_->RemoveWorkspace( w );
                    delete w;
                    w = NULL;
                    delete etmp;
                  }
                }
              }
              std::deque<ExprCodes*>::const_iterator cdend( cd.end() );
              for( std::deque<ExprCodes*>::const_iterator j(cd.begin()); j!=cdend; ++j )
              {
                if( (*j)->IsaWorkspace() )(*j)->GetWorkspacePtr()->SetParent( ws );
              }
            }   // end of if pack
          }   // end of if cd.size() > 1
        }   // end of if addedCodeToWorkspace, cd goes out of scope
        newCodes.push_back( iCode );
        addedCodeToWorkspace = false;
      }
    }   // end of i loop over code array
    //if( flag && codes_.back()->GetNestingLevel() == level )
    // flag --> addedCodeToWorkspace || only 1 code || lastCode is no-op
    if( codes_.back()->GetNestingLevel() == level )
    {
      std::deque<ExprCodes*> &cd = ws->GetCodes();
      if( cd.size() == 1 )
      {
        int n = newCodes.back()->GetNestingLevel();
        cd.front()->SetNestingLevel( n );
        *newCodes.back() = *cd.front();
        expression_->RemoveWorkspace( ws );
        delete ws;
        ws = NULL;
      }
      else
      {
        ExprCodes *lastCode = cd.back();
        if( lastCode->IsaRangeVector() &&
            lastCode->GetRangeVectorPtr()->GetItemCntr() == cd.size()-1 )
        {
          ws->SetNcFlag( !ws->GetNcFlag() );
        }
        else if( lastCode->IsaListVector() &&
                 lastCode->GetListVectorPtr()->GetItemCntr() == cd.size()-1 )
        {
          ws->SetNcFlag( !ws->GetNcFlag() );
        }
        else if( lastCode->IsaFunction() )
        {
          FunctionDefinition *f = lastCode->GetFcnPtr();
          if( f->IsNum2NumVector() || f->IsMixed2Num() || lastCode->GetIndexCntr() > 0 )
          {
            pack = false;
            ws->SetNcFlag( true );
            newCodes.back()->SetIndexCntr( lastCode->GetIndexCntr() );
          }
        }
        else if( lastCode->IsaOperator() )
        {
          OperatorDefinition *op = lastCode->GetOpPtr();
          if( op->IsArray() )
          {
            pack = false;
            ws->SetNcFlag( true );
          }
        }
        if( pack )
        {
          std::size_t cdSize = cd.size();
          for( std::size_t k=0; k<cdSize; ++k )
          {
            std::deque<ExprCodes*>::iterator j = cd.begin()+k;
            ExprCodes *e = *j;
            if( e->IsaWorkspace() )
            {
              Workspace *w = e->GetWorkspacePtr();
              std::deque<ExprCodes*> &wcd = w->GetCodes();
              if( !w->GetNcFlag() )
              {
                std::deque<ExprCodes*> temp( cd.begin(), j );
                std::size_t wcdEnd = wcd.size();
                for( std::size_t i=0; i<wcdEnd; ++i )
                {
                  ExprCodes *tmp = new ExprCodes();
                  *tmp = *wcd.at(i);
                  temp.push_back( tmp );
                }
                temp.insert( temp.end(), j+1, cd.end() );
                cd.assign( temp.begin(), temp.end() );
                k += wcd.size() - 1;
                expression_->RemoveWorkspace( w );
                delete w;
                w = NULL;
                delete e;
              }
            }
          }
          std::deque<ExprCodes*>::const_iterator newend( newCodes.end() );
          for( std::deque<ExprCodes*>::const_iterator j(newCodes.begin()); j!=newend; ++j )
          {
            if( (*j)->IsaWorkspace() )(*j)->GetWorkspacePtr()->SetParent( ws );
          }
        }   // end of if pack
      }   // end of if cd->size() > 1
    }
    codes_.assign( newCodes.begin(), newCodes.end() );
    std::deque<ExprCodes*>().swap( newCodes );
  }
  assert( codes_.size() == 1 );
  rpnDone_ = true;
}

void Workspace::RPNTconvert()
{
  // 4th Pass:
  // Loop over the input codes and reorder into Reverse Polish Notation (RPN).
  //
  std::deque<ExprCodes*> aStack;
  std::deque<ExprCodes*> outBuf;
  for( std::size_t i=0; i<codes_.size(); ++i )
  {
    ExprCodes *code = codes_[i];
    if( code->IsaFunction() )
    {
      FunctionDefinition *fcn = code->GetFcnPtr();
      if( fcn->IsNum2NumScalar() || fcn->IsNum2NumVector() || fcn->IsMixed2Num() )
      {
        throw EExpressionError( wxT("numeric function in character expression") );
      }
      if( code->GetArgCntr() == 0 )       // function has no arguments
        outBuf.push_back( code );
      else
        aStack.push_front( code );
    }
    else if( code->IsaOperator() )
    {
      OperatorDefinition *opr = code->GetOpPtr();
      if( opr->IsScalar() || opr->IsArray() )
      {
        if( opr->Name() != wxT("//") )
          throw EExpressionError( wxT("numeric operator in character expression") );
      }
      //
      // |operatorPriority| is the operator priority or precedence
      // The sign gives associativity, with + for left and - for right
      //
      int operatorPriority;
      if( code->IsBinary() )
        operatorPriority = opr->BinaryPriority();
      else
        operatorPriority = opr->UnaryPriority();
      //
      // Pop codes from the stack onto the output buffer until an
      // operator of lower precedence than that of the current,
      // or until an opening bracket, comma or colon is encountered
      //
      std::size_t aStackSize( aStack.size() );
      for( std::size_t j = 0; j < aStackSize; ++j )
      {
        ExprCodes *stackCode = aStack.front();
        aStack.pop_front();
        if( stackCode->IsaFunction() )
        {                              // pop function onto output buffer
          outBuf.push_back( stackCode );
        }
        else if( stackCode->IsaOperator() )
        {
          OperatorDefinition *op2 = stackCode->GetOpPtr();
          int opPrio;
          if( stackCode->IsBinary() )
            opPrio = op2->BinaryPriority();
          else
            opPrio = op2->UnaryPriority();
          //
          // If the stack operator has a lower precedence than the current operator,
          // stop popping the stack and push the current operator onto the stack.
          // If not, then pop the operator off the stack onto the output buffer.
          //
          if( operatorPriority < 0 )
          {
            if( abs(opPrio) <= abs(operatorPriority) )break;
          }
          else
          {
            if( abs(opPrio) <  abs(operatorPriority) )break;
          }
          //
          // stack operator has a higher precedence than input stream operator
          //
          outBuf.push_back( stackCode );
        }
        else if( stackCode->IsaOpeningBracket() )
        {
          delete stackCode;
          break;             // stop popping the stack
        }
      }   // end of j loop over codes on the stack
      aStack.push_front( code );
    }
    else if( code->IsaOpeningBracket() )
    {
      // opening bracket: push onto stack
      // closing bracket: pop stack up to corresponding opening bracket and discard
      //    pair bracket: pop stack up to corresponding opening bracket
      //           comma: pop stack up to corresponding opening bracket
      //           colon: pop stack up to corresponding opening bracket
      //       semicolon: pop stack up to corresponding opening bracket
      //            hash: already taken care of
      //        asterisk: already taken care of
      //
      aStack.push_front( code );
    }
    else if( code->IsaClosingBracket() || code->IsaComma() )
    {
      // pop codes from the stack onto the output buffer until the
      // corresponding ( is encountered
      //
      std::size_t aStackSize( aStack.size() );
      for( std::size_t j = 0; j < aStackSize; ++j )
      {
        ExprCodes *stackCode = aStack.front();
        aStack.pop_front();
        if( stackCode->IsaFunction() )
        {                              // pop function onto output buffer
          outBuf.push_back( stackCode );
        }
        else if( stackCode->IsaOperator() )
        {                              // pop operator onto output buffer
          outBuf.push_back( stackCode );
        }
        else if( stackCode->IsaOpeningBracket() )
        {
          delete stackCode;
          break; // stop popping the stack
        }
      }   // end of j loop over stack codes
      delete code;
    }
    else if( code->IsaTVariable() )
    {
      outBuf.push_back( code );
    }
    else if( code->IsaWorkspace() )
    {
      outBuf.push_back( code );
    }
    else if( code->IsaTString() )
    {
      outBuf.push_back( code );
    }
    else if( code->IsaNVariable() )
    {
      throw EExpressionError( wxT("numeric variable in character expression") );
    }
    else if( code->IsaConstant() )
    {
      outBuf.push_back( code );
    }
    else if( code->IsaImmediateValue() )
    {
      throw EExpressionError( wxT("numeric constant in character expression") );
    }
    else if( code->IsaRangeVector() )
    {
      throw EExpressionError( wxT("range vector in character expression") );
    }
    else if( code->IsaListVector() )
    {
      throw EExpressionError( wxT("list vector in character expression") );
    }
  }  // end of loop over all the codes
  //
  // pop the remaining elements of the stack
  //
  std::size_t aStackSize( aStack.size() );
  for( std::size_t j=0; j<aStackSize; ++j )
  {
    ExprCodes *stackCode = aStack.front();
    aStack.pop_front();
    outBuf.push_back( stackCode );
  }
  codes_ = outBuf;
}

void Workspace::SetType()
{
  ExprCodes *lastCode = codes_.back();
  if( lastCode->IsaFunction() &&
      (lastCode->GetFcnPtr()->IsNum2NumVector() || lastCode->GetFcnPtr()->IsMixed2Num()) )
    evalType_ = ARRAYFCN;
  else if( lastCode->IsaOperator() && lastCode->GetOpPtr()->IsArray() )
    evalType_ = ARRAYOP;
  else
    evalType_ = SIMPLE;
}

void Workspace::SetUp()
{
  // Mark codes which are indices of other variables
  //
  ExprCodes *lastCode = codes_.back();
  std::size_t codeSize = codes_.size();
  for( std::size_t i=0; i<codeSize; ++i )
  {
    ExprCodes *code = codes_[codeSize-1-i];
    if( code->IsaNVariable() )
    {
      NumericVariable *v = code->GetNVarPtr();
      std::size_t ndm = v->GetData().GetNumberOfDimensions();
      std::size_t nix = code->GetIndexCntr();
      if( nix != 0 )                 // variable has indices
      {
        if( nix != ndm )throw EExpressionError( wxString(wxT("variable "))+v->GetName()+
                                                wxT(" has wrong number of indices") );
        std::size_t ii = 0;
        for( std::size_t j=0; j<ndm; ++j )
        {
          ExprCodes *aCode = codes_[codeSize-1-i-(j+1)];
          aCode->SetIsAnIndex( true );
          if( aCode->IsaNVariable() )
          {
            NumericVariable *vb = aCode->GetNVarPtr();
            if( vb->GetData().GetNumberOfDimensions() == 2 )
              throw EExpressionError( wxString(wxT("variable "))+v->GetName()+
                                      wxT(" has a matrix as an index") );
            else if( vb->GetData().GetNumberOfDimensions() == 3 )
              throw EExpressionError( wxString(wxT("variable "))+v->GetName()+
                                      wxT(" has a tensor as an index") );
            aCode->SetIndx(0,ndm-j-1);
          }
          else if( aCode->IsaWorkspace() )
          {
            Workspace const *ws = aCode->GetWorkspacePtr();
            if( ws->GetNumberOfDimensions() == 2 )
              throw EExpressionError( wxString(wxT("variable "))+v->GetName()+
                                      wxT(" has matrix as an index") );
            else if( ws->GetNumberOfDimensions() == 3 )
              throw EExpressionError( wxString(wxT("variable "))+v->GetName()+
                                      wxT(" has a tensor as an index") );
            aCode->SetIndx(0,ndm-j-1);
          }
          else if( aCode->IsaRangeVector() )
          {
            RangeVector *rng = aCode->GetRangeVectorPtr();
            rng->SetDimension( ndm-j );
            rng->SetInt();
            std::size_t n = rng->GetItemCntr();
            rng->SetItemCntr( 0 );
            for( std::size_t m=0; m<n; ++m )
            {
              ExprCodes *bCode = codes_[codeSize-1-i-(j+1+n)+m];
              if( bCode->IsaNVariable() )
              {
                if( !bCode->GetNVarPtr()->GetData().GetNumberOfDimensions() == 0 )
                  throw EExpressionError( 
                    wxString(wxT("item "))<<m+1<<wxT(" of range is invalid") );
                rng->SetValue( m, bCode->GetNVarPtr()->GetData().GetScalarValue() );
              }
              else if( bCode->IsaWorkspace() )
              {
                Workspace *ws = bCode->GetWorkspacePtr();
                if( ws->IsEmpty() )
                  throw EExpressionError(wxT("range element workspace is empty"));
                if( ws->GetNumberOfDimensions() != 0 )
                  throw EExpressionError( 
                    wxString(wxT("item "))<<m+1<<wxT(" of range is invalid") );
                rng->SetValue( m, ws->GetValue() );
              }
              else if( bCode->IsaConstant() )
              {
                rng->SetValue( m, bCode->GetConstantValue() );
              }
              else if( bCode->IsaImmediateValue() )
              {
                rng->SetValue( m, static_cast<double>(bCode->GetImmediateValue()) );
              }
              else
                throw EExpressionError( wxString(wxT("item "))<<m+1<<wxT(" of range is invalid") );
              delete bCode;
              codes_.erase( codes_.begin()+codeSize-1-i-(j+1+n)+m );
              --codeSize;
            }
            if( rng->GetInc() == 0.0 )
              throw EExpressionError( wxT("range vector has zero increment") );
            if( rng->GetNpt() < 1 )
              throw EExpressionError( wxT("range vector has no elements") );
            int istrv = static_cast<int>(rng->GetStart());
            int iendv = static_cast<int>(rng->GetEnd());
            if( istrv<1 || istrv>static_cast<int>(v->GetData().GetDimMag(ndm-j-1)) ||
                iendv<1 || iendv>static_cast<int>(v->GetData().GetDimMag(ndm-j-1)) )
              throw EExpressionError(
                wxString(wxT("subscripts out of range for index "))<<ndm-j
                         <<wxT(" for variable ")<<v->GetName() );
            ++ii;
          }
          else if( aCode->IsaListVector() )
          {
            ListVector *lst = aCode->GetListVectorPtr();
            lst->SetDimension( ndm-j );
            lst->SetNumberOfDimensions( 1 );
            lst->SetInt();
            std::size_t n = lst->GetItemCntr();
            int idx;
            for( std::size_t m=0; m<n; ++m )
            {
              ExprCodes *bCode = codes_[codeSize-1-i-(j+1+n)+m];
              if( bCode->IsaNVariable() )
              {
                if( !bCode->GetNVarPtr()->GetData().GetNumberOfDimensions() == 0 )
                  throw EExpressionError(
                    wxT("list vector items used as indices must be scalars") );
                lst->SetItem( bCode->GetNVarPtr() );
                idx = static_cast<int>(bCode->GetNVarPtr()->GetData().GetScalarValue());
              }
              else if( bCode->IsaWorkspace() )
              {
                Workspace *ws = bCode->GetWorkspacePtr();
                if( ws->IsEmpty() )
                  throw EExpressionError(wxT("list element workspace is empty"));
                if( ws->GetNumberOfDimensions() != 0 )
                  throw EExpressionError(wxT("list vector items used as indices must be scalars") );
                lst->SetItem( ws );
                idx = static_cast<int>(ws->GetValue());
              }
              else if( bCode->IsaConstant() )
              {
                lst->SetItem( bCode->GetConstantValue() );
                idx = static_cast<int>(bCode->GetConstantValue());
              }
              else
                throw EExpressionError( wxString(wxT("item "))<<m+1
                                        <<wxT(" of list vector is invalid") );
              if( idx<1 || idx>static_cast<int>(v->GetData().GetDimMag(ndm-j-1)) )
                throw EExpressionError(
                  wxString(wxT("subscripts out of range for index "))<<ndm-j
                  <<wxT(" for variable ")<<v->GetName() );
              delete bCode;
              codes_.erase( codes_.begin()+codeSize-1-i-(j+1+n)+m );
              --codeSize;
            }
            ++ii;
          }
        }
        i += ii;
      }
    }
    else if( code->IsaWorkspace() )
    {
      Workspace *ws = code->GetWorkspacePtr();
      if( ws->IsEmpty() )
        throw EExpressionError( wxT("function or expression has empty workspace") );
      std::size_t nix = code->GetIndexCntr();
      std::size_t ndm = ws->GetNumberOfDimensions();
      if( nix != 0 )
      {
        if( nix != ndm )
          throw EExpressionError( wxT("function or expression has wrong number of indices") );
        for( std::size_t j=0; j<ndm; ++j )
        {
          ExprCodes *aCode = codes_[codeSize-1-i-(j+1)];
          aCode->SetIsAnIndex( true );
          if( aCode->IsaNVariable() )
          {
            NumericVariable *vb = aCode->GetNVarPtr();
            if( vb->GetData().GetNumberOfDimensions() == 2 )
              throw EExpressionError( wxT("function or expression has a matrix as an index") );
            else if( vb->GetData().GetNumberOfDimensions() == 3 )
              throw EExpressionError( wxT("function or expression has a tensor as an index") );
            aCode->SetIndx(0,ndm-j-1);
          }
          else if( aCode->IsaWorkspace() )
          {
            Workspace *w = aCode->GetWorkspacePtr();
            if( w->IsEmpty() )
              throw EExpressionError( wxT("function or expression workspace index is empty") );
            if( w->GetNumberOfDimensions() == 2 )
              throw EExpressionError( wxT("function or expression has a matrix as an index") );
            else if( w->GetNumberOfDimensions() == 3 )
              throw EExpressionError( wxT("function or expression has a tensor as an index") );
            aCode->SetIndx(0,ndm-j-1);
          }
          else if( aCode->IsaRangeVector() )
          {
            RangeVector *rng = aCode->GetRangeVectorPtr();
            rng->SetDimension( ndm-j );
            rng->SetInt();
            std::size_t n = rng->GetItemCntr();
            rng->SetItemCntr( 0 );
            for( std::size_t m=0; m<n; ++m )
            {
              ExprCodes *bCode = codes_[codeSize-1-i-(j+1+n)+m];
              if( bCode->IsaNVariable() )
              {
                if( !bCode->GetNVarPtr()->GetData().GetNumberOfDimensions() == 0 )
                  throw EExpressionError( wxString(wxT("item "))<<m+1
                                          <<wxT(" of range is invalid") );
                rng->SetValue( m, bCode->GetNVarPtr()->GetData().GetScalarValue() );
              }
              else if( bCode->IsaWorkspace() )
              {
                Workspace *w = bCode->GetWorkspacePtr();
                if( w->IsEmpty() )
                  throw EExpressionError( wxT("range item workspace is empty") );
                if( w->GetNumberOfDimensions() != 0 )
                  throw EExpressionError( wxString(wxT("item "))<<m+1
                                          <<wxT(" of range is invalid") );
                rng->SetValue( m, w->GetValue() );
              }
              else if( bCode->IsaConstant() )
              {
                rng->SetValue( m, bCode->GetConstantValue() );
              }
              else if( bCode->IsaImmediateValue() )
              {
                rng->SetValue( m, static_cast<double>(bCode->GetImmediateValue()) );
              }
              else
                throw EExpressionError( wxString(wxT("item "))<<m+1
                                        <<wxT(" of range is invalid") );
              delete bCode;
              codes_.erase( codes_.begin()+codeSize-1-i-(j+1+n)+m );
              --codeSize;
            }
            if( rng->GetInc() == 0.0 )
              throw EExpressionError( wxT("range vector has zero increment") );
            if( rng->GetNpt() < 1 )
              throw EExpressionError( wxT("range vector has no elements") );
            int istrv = static_cast<int>(rng->GetStart());
            int iendv = static_cast<int>(rng->GetEnd());
            if( istrv<1 || istrv>static_cast<int>(ws->GetDimMag(ndm-j-1)) ||
                iendv<1 || iendv>static_cast<int>(ws->GetDimMag(ndm-j-1)) )
              throw EExpressionError(
                wxString(wxT("subscripts out of range for index "))<<ndm-j );
            ++i;
          }
          else if( aCode->IsaListVector() )
          {
            ListVector *lst = aCode->GetListVectorPtr();
            lst->SetDimension( ndm-j );
            lst->SetNumberOfDimensions( 1 );
            lst->SetInt();
            std::size_t n = lst->GetItemCntr();
            for( std::size_t m=0; m<n; ++m )
            {
              int idx;
              ExprCodes *bCode = codes_[codeSize-1-i-(j+1+n)+m];
              if( bCode->IsaNVariable() )
              {
                if( !bCode->GetNVarPtr()->GetData().GetNumberOfDimensions() == 0 )
                  throw EExpressionError(
                  wxT("list vector items used as indices must be scalars") );
                lst->SetItem( bCode->GetNVarPtr() );
                idx = static_cast<int>(bCode->GetNVarPtr()->GetData().GetScalarValue());
              }
              else if( bCode->IsaWorkspace() )
              {
                Workspace *w = bCode->GetWorkspacePtr();
                if( w->IsEmpty() )
                  throw EExpressionError( wxT("list item workspace is empty") );
                if( w->GetNumberOfDimensions() != 0 )
                  throw EExpressionError(wxT("list vector items used as indices must be scalars") );
                lst->SetItem( w );
                idx = static_cast<int>(w->GetValue());
              }
              else if( bCode->IsaConstant() )
              {
                lst->SetItem( bCode->GetConstantValue() );
                idx = static_cast<int>(bCode->GetConstantValue());
              }
              else
                throw EExpressionError( wxString(wxT("item "))<<m+1
                                        <<wxT(" of list vector is invalid") );
              if( idx<1 || idx>static_cast<int>(ws->GetDimMag(ndm-j-1)) )
                throw EExpressionError( wxString(wxT("subscripts out of range for index "))
                                        <<ndm-j);
              delete bCode;
              codes_.erase( codes_.begin()+codeSize-1-i-(j+1+n)+m );
              --codeSize;
            }
            ++i;
          }
        }
      }
    }
    else if( code->IsaRangeVector() )
    {
      RangeVector *rng = code->GetRangeVectorPtr();
      std::size_t n = rng->GetItemCntr();
      for( std::size_t m=0; m<n; ++m )
      {
        ExprCodes *aCode = codes_[codeSize-1-i-n+m];
        if( aCode->IsaNVariable() )
        {
          if( !aCode->GetNVarPtr()->GetData().GetNumberOfDimensions() == 0 )
            throw EExpressionError( wxString(wxT("item "))<<m+1<<wxT(" of range is invalid") );
          rng->SetValue( m, aCode->GetNVarPtr()->GetData().GetScalarValue() );
        }
        else if( aCode->IsaWorkspace() )
        {
          Workspace *w = aCode->GetWorkspacePtr();
          if( w->IsEmpty() )
            throw EExpressionError( wxT("range item workspace is empty") );
          if( w->GetNumberOfDimensions() != 0 )
            throw EExpressionError( wxString(wxT("item "))<<m+1<<wxT(" of range is invalid") );
          rng->SetValue( m, w->GetValue() );
        }
        else if( aCode->IsaConstant() )
        {
          rng->SetValue( m, aCode->GetConstantValue() );
        }
        else if( aCode->IsaImmediateValue() )
        {
          rng->SetValue( m, static_cast<double>(aCode->GetImmediateValue()) );
        }
        else
          throw EExpressionError( wxString(wxT("item "))<<m+1<<wxT(" of range is invalid") );
        delete aCode;
        codes_.erase( codes_.begin()+codeSize-1-i-n+m );
        --codeSize;
      }
      if( rng->GetInc() == 0.0 )
        throw EExpressionError( wxT("range vector has zero increment") );
      if( rng->GetNpt() < 1 )
        throw EExpressionError( wxT("range vector has no elements") );
    }
    else if( code->IsaListVector() )
    {
      ListVector *lst = code->GetListVectorPtr();
      std::size_t n = lst->GetItemCntr();
      for( std::size_t m=0; m<n; ++m )
      {
        ExprCodes *aCode = codes_[codeSize-1-i-n+m];
        if( aCode->IsaNVariable() )
        {
          if( m == 0 )
          {
            lst->SetNumberOfDimensions( 1+aCode->GetNVarPtr()->GetData().GetNumberOfDimensions() );
          }
          else
          {
            if( 1+aCode->GetNVarPtr()->GetData().GetNumberOfDimensions() != lst->GetNumberOfDimensions() )
              throw EExpressionError(
              wxT("all items in a list must have the the same number of dimensions") );
          }
          lst->SetItem( aCode->GetNVarPtr() );
        }
        else if( aCode->IsaWorkspace() )
        {
          Workspace *w = aCode->GetWorkspacePtr();
          if( w->IsEmpty() )
            throw EExpressionError( wxT("list item workspace is empty") );
          if( m == 0 )
          {
            lst->SetNumberOfDimensions( 1+w->GetNumberOfDimensions() );
          }
          else
          {
            if( 1+w->GetNumberOfDimensions() != lst->GetNumberOfDimensions() )
              throw EExpressionError(
                wxT("all items in a list must have the the same number of dimensions") );
          }
          lst->SetItem( aCode->GetWorkspacePtr() );
        }
        else if( aCode->IsaConstant() )
        {
          if( m == 0 )
          {
            lst->SetNumberOfDimensions( 1 );
          }
          else
          {
            if( 1 != lst->GetNumberOfDimensions() )
              throw EExpressionError(
                wxT("all items in a list must have the the same number of dimensions") );
          }
          lst->SetItem( aCode->GetConstantValue() );
        }
        else
          throw EExpressionError( wxString(wxT("item "))<<m+1<<wxT(" of list is invalid") );
        delete aCode;
        codes_.erase( codes_.begin()+codeSize-1-i-n+m );
        --codeSize;
      }
    }
  }   // end of reverse for loop over codes
  int nloop = 0;
  bool first = true;
  std::deque<ExprCodes*> oCode;
  int ndmEff = 0;
  for( std::size_t i=0; i<codes_.size(); ++i )
  {
    std::size_t ndm = 0;
    ExprCodes *code = codes_[i];
    if( code->IsaNVariable() || code->IsaConstant() ||
        code->IsaWorkspace() || code->IsaImmediateValue() ||
        code->IsaRangeVector() || code->IsaListVector() ||
        code->IsaTString() )
    {
      if( code->IsaConstant() || code->IsaImmediateValue() )
      {
        ndmEff = 0;
      }
      else if( code->IsaNVariable() )
      {
        NumericVariable *v = code->GetNVarPtr();
        ndm = v->GetData().GetNumberOfDimensions();
        ndmEff = ndm;
        if( code->GetIndexCntr() > 0 ) // there are indices
        {
          ndmEff = 0;
          std::size_t shift = 0;
          for( std::size_t j=0; j<ndm; ++j )
          {
            std::size_t k = i-ndm+j+shift;
            shift = 0;
            if( codes_[k]->IsaNVariable() &&
                codes_[k]->GetNVarPtr()->GetData().GetNumberOfDimensions()==1 )
            {
              ++ndmEff;
            }
            else if( codes_[k]->IsaWorkspace() &&
                     codes_[k]->GetWorkspacePtr()->GetNumberOfDimensions()==1  )
            {
              ++ndmEff;
            }
            else if( codes_[k]->IsaRangeVector() )
            {
              ++ndmEff;
            }
            else if( codes_[k]->IsaListVector() )
            {
              ++ndmEff;
            }
          }
        }
      }
      else if( code->IsaWorkspace() )
      {
        Workspace *ws = code->GetWorkspacePtr();
        if( ws->IsEmpty() )
          throw EExpressionError( wxT("workspace is empty") );
        ndm = ws->GetNumberOfDimensions();
        ndmEff = ndm;
        if( code->GetIndexCntr() > 0 ) // there are indices
        {
          ndmEff = 0;
          std::size_t shift = 0;
          for( std::size_t j=0; j<ndm; ++j )
          {
            std::size_t k = i-ndm+j+shift;
            shift = 0;
            if( codes_[k]->IsaNVariable() &&
                codes_[k]->GetNVarPtr()->GetData().GetNumberOfDimensions()==1 )
            {
              ++ndmEff;
            }
            else if( codes_[k]->IsaWorkspace() &&
                     codes_[k]->GetWorkspacePtr()->GetNumberOfDimensions()==1  )
            {
              ++ndmEff;
            }
            else if( codes_[k]->IsaRangeVector() )
            {
              ++ndmEff;
            }
            else if( codes_[k]->IsaListVector() )
            {
              ++ndmEff;
            }
          }
        }
      }
      else if( code->IsaRangeVector() )
      {
        ndmEff = 1;
      }
      else if( code->IsaListVector() )
      {
        ndmEff = code->GetListVectorPtr()->GetNumberOfDimensions();
      }
      //
      // Store loop information for evaluation
      //
      if( !code->IsaIndex() )       // do not process indices of variables
      {
        if( lastCode->IsaOperator() )
        {
          if( IsSimpleEvalType() && ndmEff > 0 ) // element by element array evaluation
          {
            if( first )
            {
              nloop = ndmEff;
              SetNumberOfDimensions( ndmEff );
            }
            else
            {
              if( ndmEff != nloop )
                throw EExpressionError( wxT("dimension mismatch (ndmEff != nloop)") );
            }
            for( int j=0; j<nloop; ++j )code->SetIndx( j, j );
            first = false;
          }
          else if( IsArrayOpEvalType() )
          {
            int tmp = GetNumberOfDimensions();
            lastCode->GetOpPtr()->ProcessOperand( first, ndmEff, code, oCode, tmp, nloop );
            SetNumberOfDimensions( tmp );
            first = false;
          }
        }
        else if( lastCode->IsaFunction() )
        {
          if( IsSimpleEvalType() && ndmEff>0 ) // element by element array evaluation
          {
            if( first )
            {
              nloop = ndmEff;
              SetNumberOfDimensions( ndmEff );
              first = false;
            }
            else
            {
              if( ndmEff != nloop )
                throw EExpressionError( wxT("dimension mismatch: (ndmEff != nloop)") );
            }
          }
          else if( IsArrayFcnEvalType() )
          {
            lastCode->GetFcnPtr()->ProcessArgument( first, ndmEff, nloop );
            SetNumberOfDimensions( ndmEff );
            first = false;
          }
          for( unsigned short int j=0; j<nloop; ++j )code->SetIndx( j, j );
        }
        else
        {
          if( IsSimpleEvalType() && ndmEff > 0 )
          {
            if( first )
            {
              first = false;
              nloop = ndmEff;
              SetNumberOfDimensions( ndmEff );
            }
            else
            {
              if( nloop != ndmEff )
                throw EExpressionError( wxString(wxT("dimension mismatch: "))<<nloop
                                        <<wxT(" != ")<<ndmEff );
            }
            for( unsigned short int j=0; j<nloop; ++j )code->SetIndx( j, j );
          }
        }
      }
    }    // end of if code is NumericVariable || Constant || ...
    oCode.push_back( code );
  }
  if( IsSimpleEvalType() )  // append explicit loop function
  {
    ExprCodes *c = new ExprCodes;
    c->SetMloop();
    c->SetIsAnIndex( true );
    oCode.push_back( c );
  }
  codes_ = oCode;
  SetNumberOfLoops( nloop );
  SetNumberOfDimensions( nloop );
}

void Workspace::Calculate()
{
  //std::cout << "start Calculate\n";

  ExprCodes *lastCode = codes_.back();
  evalFlag_ = true;

  std::size_t ndmEff = 0;
  bool first = true;
  int nptEff[] = {0,0,0};
  for( std::size_t i=0; i<codes_.size(); ++i )  // loop over codes in workspace
  {
    ExprCodes *code = codes_[i];
    if( code->IsaNVariable() || code->IsaConstant() ||
        code->IsaWorkspace() || code->IsaImmediateValue() ||
        code->IsaRangeVector() || code->IsaListVector() ||
        code->IsaTString() )
    {
      std::size_t ndm = 0;
      if( code->IsaNVariable() )
      {
        NumericVariable *v = code->GetNVarPtr();
        ndm = v->GetData().GetNumberOfDimensions();
        if( code->GetIndexCntr() == 0 )           // no indices
        {
          for( std::size_t j=0; j<ndm; ++j )nptEff[j] = v->GetData().GetDimMag(j);
          ndmEff = ndm;
        }
        else                                      // there are indices
        {
          std::size_t nback = 0;
          ExprCodes *temp[3];
          for( std::size_t j=0; j<ndm; ++j )  // loop over variable dimensions
          {
            ExprCodes *aCode = codes_[i-nback-1];
            temp[ndm-1-j] = aCode;
            ++nback;
          }
          ndmEff = 0;
          for( std::size_t j=0; j<ndm; ++j )
          {
            ExprCodes *aCode = temp[j];
            if( aCode->IsaNVariable() )
            {
              int dm = aCode->GetNVarPtr()->GetData().GetDimMag(0);
              if( dm > 0 )nptEff[ndmEff++] = dm;
            }
            else if( aCode->IsaWorkspace() )
            {
              int dm = aCode->GetWorkspacePtr()->GetDimMag(0);
              if( dm > 0 )nptEff[ndmEff++] = dm;
            }
            else if( aCode->IsaRangeVector() )
            {
              RangeVector *rng = aCode->GetRangeVectorPtr();
              nptEff[ndmEff++] = rng->GetNpt();
            }
            else if( aCode->IsaListVector() )
            {
              ListVector *lst = aCode->GetListVectorPtr();
              nptEff[ndmEff++] = lst->GetNpt();
            }
          }
        }
      }
      else if( code->IsaWorkspace() )
      {
        Workspace *ws = code->GetWorkspacePtr();
        if( ws->IsEmpty() )throw EExpressionError( wxT("workspace is empty") );
        ndm = ws->GetNumberOfDimensions();
        if( code->GetIndexCntr() == 0 )         // no indices
        {
          for( std::size_t j=0; j<ndm; ++j )nptEff[j] = ws->GetDimMag( j );
          ndmEff = ndm;
        }
        else                                    // there are indices
        {
          std::size_t nback = 0;
          ExprCodes *temp[3];
          for( std::size_t j=0; j<ndm; ++j )
          {
            ExprCodes *aCode = codes_[i-nback-1];
            temp[ndm-1-j] = aCode;
            ++nback;
          }
          ndmEff = 0;
          for( std::size_t j=0; j<ndm; ++j )
          {
            ExprCodes *aCode = temp[j];
            if( temp[j]->IsaNVariable() )
            {
              int dm = aCode->GetNVarPtr()->GetData().GetDimMag(0);
              if( dm > 0 )nptEff[ndmEff++] = dm;
            }
            else if( temp[j]->IsaWorkspace() )
            {
              int dm = aCode->GetWorkspacePtr()->GetDimMag(0);
              if( dm > 0 )nptEff[ndmEff++] = dm;
            }
            else if( temp[j]->IsaRangeVector() )
            {
              RangeVector *rng = aCode->GetRangeVectorPtr();
              nptEff[ndmEff++] = rng->GetNpt();
            }
            else if( temp[j]->IsaListVector() )
            {
              ListVector *lst = aCode->GetListVectorPtr();
              nptEff[ndmEff++] = lst->GetNpt();
            }
          }
        }
      }
      else if( code->IsaTString() )
      {
        nptEff[0] = code->GetTString().size();
      }
      else if( code->IsaRangeVector() )
      {
        RangeVector *rng = code->GetRangeVectorPtr();
        ndm = 1;
        ndmEff = 1;
        nptEff[0] = rng->GetNpt();
      }
      else if( code->IsaListVector() )
      {
        ListVector *lst = code->GetListVectorPtr();
        ndm = lst->GetNumberOfDimensions();
        ndmEff = ndm;
        //
        // we already know that the list is all 0, 1, or 2 dimensional entities
        //
        if( ndm == 1 )
        {
          nptEff[0] = lst->GetItemCntr();
        }
        else if( ndm == 2 )
        {
          if( lst->ItemIsaNVariable(0) )
            nptEff[0] = const_cast<NumericVariable*>(lst->GetNVItem(0))->GetData().GetDimMag(0);
          else
            nptEff[0] = const_cast<Workspace*>(lst->GetWSItem(0))->GetDimMag(0);
          nptEff[1] = lst->GetItemCntr();
        }
        else if( ndm == 3 )
        {
          if( lst->ItemIsaNVariable(0) )
          {
            NumericVariable *nv = const_cast<NumericVariable*>(lst->GetNVItem(0));
            nptEff[1] = nv->GetData().GetDimMag(0);
            nptEff[0] = nv->GetData().GetDimMag(1);
          }
          else
          {
            Workspace *ws = const_cast<Workspace*>(lst->GetWSItem(0));
            nptEff[1] = ws->GetDimMag(0);
            nptEff[0] = ws->GetDimMag(1);
          }
          nptEff[2] = lst->GetItemCntr();
        }
        for( std::size_t j=0; j<ndmEff; ++j )SetDimMag( j, nptEff[j] );
      }
      if( ndm == 0 )         // scalar
      {
        ndmEff = 0;
        nptEff[0] = 1;
      }
      //
      // Store loop information for evaluation
      //
      if( !code->IsaIndex() )       // do not process indices of variables
      {
        if( lastCode->IsaOperator() )
        {
          if( (IsSimpleEvalType() && ndmEff>0) || IsArrayOpEvalType() )
          {
            // element by element array evaluation  or  array operator
            //
            lastCode->GetOpPtr()->CalcOperand( first, ndmEff, nptEff, this );
            first = false;
          }
        }
        else if( lastCode->IsaFunction() )
        {
          if( (IsSimpleEvalType() && ndmEff>0) || IsArrayFcnEvalType() )
          {
            // element by element array evaluation  or  array function
            //
            lastCode->GetFcnPtr()->CalcArgument( first, ndmEff, nptEff, this );
            first = false;
          }
        }
        else
        {
          if( IsSimpleEvalType() && ndmEff>0 )
          {
            if( first )
            {
              for( std::size_t j = 0; j < GetNumberOfLoops(); ++j )
                SetDimMag( static_cast<unsigned short int>(j), nptEff[j] );
              first = false;
            }
            else
            {
              for( std::size_t j = 0; j < GetNumberOfLoops(); ++j )
              {
                if( static_cast<int>(GetDimMag(static_cast<unsigned short int>(j))) != nptEff[j] )
                  throw EExpressionError( wxString(wxT("size mismatch for dimension "))<<j );
              }
            }
          }
        }
      }
    }    // end of if variable, workspace, etc.
  }    // end of loop over codes in workspace
  //
  // actually do the calculations
  //
  if( IsSimpleEvalType() )
  {
    SimpleEval();
  }
  else if( IsArrayFcnEvalType() )
  {
    codes_.back()->GetFcnPtr()->ArrayEval( this );
  }
  else if( IsArrayOpEvalType() )
  {
    codes_.back()->GetOpPtr()->ArrayEval( this );
  }

  //std::cout << "end Calculate\n";

}

void Workspace::SimpleEval()
{
  // 9th Pass
  //
  //  evaluate this workspace and store the result
  //  element by element evaluation
  //
  std::vector<double> rStack;
  int loopCntr[] = {0,0,0};
  int ndm = 0;
TOP:
  int j = -1;
  std::deque<ExprCodes*>::const_iterator end( codes_.end() );
  for( std::deque<ExprCodes*>::const_iterator i(codes_.begin()); i!=end; ++i )
  {
    if( (*i)->IsaTString() )
    {
      throw EExpressionError( wxT("character string in numeric expression") );
    }
    else if( (*i)->IsaFunction() )
    {
      int narg = (*i)->GetArgCntr();
      j -= narg-1;
      (*i)->GetFcnPtr()->ScalarEval( j, rStack );
    }
    else if( (*i)->IsaOperator() )
    {
      if( (*i)->IsBinary() )
      {
        --j;
        rStack[j] = (*i)->GetOpPtr()->ScalarEval( rStack[j], rStack[j+1] );
        rStack.pop_back();
      }
      else
      {
        rStack[j] = (*i)->GetOpPtr()->ScalarEval( rStack[j] );
      }
    }
    else if( (*i)->IsaConstant() )
    {
      ++j;
      rStack.push_back( (*i)->GetConstantValue() );
    }
    else if( (*i)->IsaImmediateValue() )
    {
      ++j;
      rStack.push_back( static_cast<double>( (*i)->GetImmediateValue() ) );
    }
    else if( (*i)->IsaNVariable() )
    {
      NumericData &nd( (*i)->GetNVarPtr()->GetData() );
      if( nd.IsEmpty() )return;
      if( nd.GetNumberOfDimensions() == 0 )
      {
        ++j;
        rStack.push_back( nd.GetScalarValue() );
      }
      else                                 // vector, matrix or tensor
      {
        double value;
        int i1, i2, i3;
        if( (*i)->GetIndexCntr() == 0 )       // unindexed array variable
        {
          switch ( nd.GetNumberOfDimensions() )
          {
            case 1:
              ndm = (*i)->GetIndx(0);
              i1 = loopCntr[ndm];
              value = nd.GetData( i1 );
              break;
            case 2:
              i1 = loopCntr[ (*i)->GetIndx(0) ];
              i2 = loopCntr[ (*i)->GetIndx(1) ];
              value = nd.GetData( i1, i2 );
              break;
            case 3:
              i1 = loopCntr[ (*i)->GetIndx(0) ];
              i2 = loopCntr[ (*i)->GetIndx(1) ];
              i3 = loopCntr[ (*i)->GetIndx(2) ];
              value = nd.GetData( i1, i2, i3 );
              break;
          }
          ++j;
          rStack.push_back( value );
        }
        else                         // indexed array variable
        {
          switch( nd.GetNumberOfDimensions() )
          {
            case 1:
              i1 = static_cast<int>(rStack[j])-1;
              if( i1 < 0 )throw EExpressionError( wxT("vector subscript < minimum") );
              if( i1 >= static_cast<int>(nd.GetDimMag(0)) )
                throw EExpressionError( wxT("vector subscript > maximum") );
              value = nd.GetData( i1 );
              break;
            case 2:
              --j;
              i1 = static_cast<int>(rStack[j])-1;
              i2 = static_cast<int>(rStack[j+1])-1;
              rStack.pop_back();
              if( i1 < 0 )throw EExpressionError( wxT("matrix first subscript < minimum") );
              if( i1 >= static_cast<int>(nd.GetDimMag(0)) )
                throw EExpressionError( wxT("matrix first subscript > maximum") );
              if( i2 < 0 )throw EExpressionError( wxT("matrix second subscript < minimum") );
              if( i2 >= static_cast<int>(nd.GetDimMag(1)) )
                throw EExpressionError( wxT("matrix second subscript > maximum") );
              value = nd.GetData( i1, i2 );
              break;
            case 3:
              --(--j);
              i1 = static_cast<int>(rStack[j])-1;
              i2 = static_cast<int>(rStack[j+1])-1;
              i2 = static_cast<int>(rStack[j+2])-1;
              rStack.pop_back();
              rStack.pop_back();
              if( i1 < 0 )throw EExpressionError( wxT("tensor first subscript < minimum") );
              if( i1 >= static_cast<int>(nd.GetDimMag(0)) )
                throw EExpressionError( wxT("tensor first subscript > maximum") );
              if( i2 < 0 )throw EExpressionError( wxT("tensor second subscript < minimum") );
              if( i2 >= static_cast<int>(nd.GetDimMag(1)) )
                throw EExpressionError( wxT("tensor second subscript > maximum") );
              if( i3 < 0 )throw EExpressionError( wxT("tensor third subscript < minimum") );
              if( i3 >= static_cast<int>(nd.GetDimMag(2)) )
                throw EExpressionError( wxT("tensor third subscript > maximum") );
              value = nd.GetData( i1, i2, i3 );
              break;
          }
          rStack[j] = value;
        }
      }
    }
    else if( (*i)->IsaWorkspace() )   // workspace variables
    {
      Workspace *ws = (*i)->GetWorkspacePtr();
      if( !ws || ws->IsEmpty() )return;
      if( ws->GetNumberOfDimensions() == 0 )   // scalar variable
      {
        ++j;
        rStack.push_back( ws->GetValue() );
      }
      else                             // vector or matrix or tensor
      {
        double value;
        int i1, i2, i3;
        if( (*i)->GetIndexCntr() == 0 )   // unindexed array variable
        {
          switch ( ws->GetNumberOfDimensions() )
          {
            case 1:
              ndm = (*i)->GetIndx(0);
              i1 = loopCntr[ndm];
              value = ws->GetData( i1 );

              //std::cout << "ndm=" << ndm << ", loopCntr[ndm]=" << loopCntr[ndm]
              //          << ", value=" << value << "\n";

              break;
            case 2:
              i1 = loopCntr[ (*i)->GetIndx(0) ];
              i2 = loopCntr[ (*i)->GetIndx(1) ];
              value = ws->GetData( i1, i2 );
              break;
            case 3:
              i1 = loopCntr[ (*i)->GetIndx(0) ];
              i2 = loopCntr[ (*i)->GetIndx(1) ];
              i3 = loopCntr[ (*i)->GetIndx(2) ];
              value = ws->GetData( i1, i2, i3 );
              break;
          }
          ++j;
          rStack.push_back( value );
        }
        else                   // indexed array variable
        {
          switch( ws->GetNumberOfDimensions() )
          {
            case 1:
              i1 = static_cast<int>(rStack[j]) - 1;
              if( i1 < 0 )
                throw EExpressionError( wxT("workspace subscript < 0") );
              if( i1 >= static_cast<int>(ws->GetDimMag(0)) )
                throw EExpressionError( wxT("workspace subscript > maximum") );
              value = ws->GetData( i1 );
              break;
            case 2:
              --j;
              i1 = static_cast<int>(rStack[j]) - 1;
              i2 = static_cast<int>(rStack[j+1]) - 1;
              rStack.pop_back();
              if( i1 < 0 )
                throw EExpressionError( wxT("workspace first subscript < minimum") );
              if( i1 >= static_cast<int>(ws->GetDimMag(0)) )
                throw EExpressionError( wxT("workspace first subscript > maximum") );
              if( i2 < 0 )
                throw EExpressionError( wxT("workspace second subscript < minimum") );
              if( i2 >= static_cast<int>(ws->GetDimMag(1)) )
                throw EExpressionError( wxT("workspace second subscript > maximum") );
              value = ws->GetData( i1, i2 );
              break;
            case 3:
              --(--j);
              i1 = static_cast<int>(rStack[j]) - 1;
              i2 = static_cast<int>(rStack[j+1]) - 1;
              i2 = static_cast<int>(rStack[j+2]) - 1;
              rStack.pop_back();
              rStack.pop_back();
              if( i1 < 0 )
                throw EExpressionError( wxT("workspace first subscript < minimum") );
              if( i1 >= static_cast<int>(ws->GetDimMag(0)) )
                throw EExpressionError( wxT("workspace first subscript > maximum") );
              if( i2 < 0 )
                throw EExpressionError( wxT("workspace second subscript < minimum") );
              if( i2 >= static_cast<int>(ws->GetDimMag(1)) )
                throw EExpressionError( wxT("workspace second subscript > maximum") );
              if( i3 < 0 )
                throw EExpressionError( wxT("workspace third subscript < minimum") );
              if( i3 >= static_cast<int>(ws->GetDimMag(2)) )
                throw EExpressionError( wxT("workspace third subscript > maximum") );
              value = ws->GetData( i1, i2, i3 );
              break;
          }
        }
        rStack[j] = value;
      }
    }
    else if( (*i)->IsaRangeVector() )
    {
      ++j;
      RangeVector *rng = (*i)->GetRangeVectorPtr();
      ndm = rng->GetDimension()-1;
      rStack.push_back( rng->GetItem(loopCntr[ndm]) );
    }
    else if( (*i)->IsaListVector() )
    {
      ++j;
      ListVector *lst = (*i)->GetListVectorPtr();
      ndm = lst->GetDimension()-1;
      if( lst->GetNumberOfDimensions() == 1 )
      {
        int listItem = loopCntr[ndm];
        rStack.push_back( lst->GetItem(listItem) );
      }
      else if( lst->GetNumberOfDimensions() == 2 )
      {
        int listItem = loopCntr[(*i)->GetIndx(1)];
        int i1 = loopCntr[ (*i)->GetIndx(0) ];
        if( lst->ItemIsaNVariable(listItem) )
        {
          rStack.push_back(
            const_cast<NumericVariable*>(lst->GetNVItem(listItem))->GetData().GetData(i1));
        }
        else
        {
          rStack.push_back(lst->GetWSItem(listItem)->GetData(i1));
        }
      }
    }
    else if( (*i)->IsaMloop() )
    {
      assert( j == 0 );               // otherwise the RPN code is invalid
      switch ( GetNumberOfLoops() )
      {
        case 0:   // scalar expression
          SetValue( rStack[0] );
          break;
        case 1:   // vector expression
          SetData( loopCntr[ndm], rStack[0] );
          if( loopCntr[ndm]+1 < static_cast<int>(GetDimMag(0)) )
          {
            ++loopCntr[ndm];
            std::vector<double>().swap( rStack );
            goto TOP;
          }
          break;
        case 2:   // matrix expression
          SetData( loopCntr[0]+loopCntr[1]*GetDimMag(0), rStack[0] );
          if( loopCntr[0]+1 == static_cast<int>(GetDimMag(0)) )
          {
            if( loopCntr[1]+1 < static_cast<int>(GetDimMag(1)) )
            {
              ++loopCntr[1];
              loopCntr[0] = 0;
              std::vector<double>().swap( rStack );
              goto TOP;
            }
          }
          else
          {
            ++loopCntr[0];
            std::vector<double>().swap( rStack );
            goto TOP;
          }
          break;
        case 3:   // tensor expression
          SetData( loopCntr[0]+loopCntr[1]*GetDimMag(0)+
                   loopCntr[2]*GetDimMag(0)*GetDimMag(1), rStack[0] );
          if( loopCntr[0]+1 == static_cast<int>(GetDimMag(0)) )
          {
            if( loopCntr[1]+1 == static_cast<int>(GetDimMag(1)) )
            {
              if( loopCntr[2]+1 < static_cast<int>(GetDimMag(2)) )
              {
                ++loopCntr[2];
                loopCntr[1] = 0;
                loopCntr[0] = 0;
                std::vector<double>().swap( rStack );
                goto TOP;
              }
              else
              {
                ++loopCntr[1];
                loopCntr[0] = 0;
                std::vector<double>().swap( rStack );
                goto TOP;
              }
            }
          }
          else
          {
            ++loopCntr[0];
            std::vector<double>().swap( rStack );
            goto TOP;
          }
          break;
      }
    }
  }    // end of loop over codes in workspace
}

Workspace *Workspace::GetFinalWorkspace()
{
  return expression_->GetFinalWorkspace();
}

// end of file
