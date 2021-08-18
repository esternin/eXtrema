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
#include "Variable.h"
#include "EVariableError.h"
#include "ExGlobals.h"
#include "ToAscii.h"

Variable::Variable()
{}

Variable::Variable( wxString const &name ) : name_(name.Upper())
{}

Variable::~Variable()
{}

Variable::Variable( Variable const &rhs )
{ CopyStuff(rhs); }

Variable &Variable::operator=( Variable const &rhs )
{
  if( this != &rhs )CopyStuff(rhs);
  return *this;
}

void Variable::AddToHistory( wxString s )
{
  history_.push_front(s);
  if( static_cast<int>(history_.size()) >= ExGlobals::GetMaxHistory() )history_.pop_back();
}

std::deque<wxString> const &Variable::GetHistory()
{ return history_; }

void Variable::SetHistory( std::deque<wxString> &s )
{ history_.swap( s ); }

void Variable::SetName( wxString const &name )
{ name_ = name; }

wxString &Variable::GetName()
{ return name_; }

void Variable::SetOrigin( wxString const &origin )
{ origin_ = origin; }

wxString const &Variable::GetOrigin() const
{ return origin_; }

wxString Variable::SimpleName( wxString const &name )
{
  int const classes[128] =
  {
      6, 6, 6, 6, 6, 6, 6, 6, 6, 1, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      1, 6, 6, 6, 3, 6, 6, 6, 4, 5, 6, 6, 6, 6, 6, 6,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6,
      6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 6, 5, 6, 3,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 4, 6, 5, 6, 6
  };
  int const stateTable[3][6] =
  {
    // blnk  A-Z  $_0-9   {([   ])}   other
      {  1,   2,   -10,  -20,  -20,    2},
      {100,   3,     3,  100,  -30,    3},
      {100,   3,     3,  100,  -30,    3}
  };
  wxString uName( name.Upper() );
  //
  int start = -1;
  int newState = 1;
  std::size_t i = 0;
  while ( i < uName.length() )
  {
    int currentState = newState;
    int iascii = TryConvertToAscii( uName[i] );
    newState = stateTable[currentState-1][classes[iascii]-1];
    if( newState == 2 )         // start variable name
      start = i;
    else if( newState == 100 )  // variable with no indices
      break;
    else if( newState == -10 )
      throw EVariableError( wxString(wxT("invalid first character in name: "))+name );
    else if( newState == -20 )
      throw EVariableError( wxT("no variable name was given") );
    else if( newState == -30 )
      throw EVariableError( wxString::Format(wxT("%c is an invalid character in %s"), uName[i], name) );
    ++i;
  }
  if( start == -1 )throw EVariableError(wxT("no variable name was given"));
  return uName.substr(start,i-start);
}

void Variable::CheckReservedName( wxString const &name )
{
  std::size_t const nNames = 200;
  wxString const reservedName[nNames] =
  {
    wxT("LOG"), wxT("LN"), wxT("DIGAMMA"), wxT("PSI"), wxT("LOG10"), wxT("AIRY"),
    wxT("EXP"), wxT("LOGAM"), wxT("SQRT"), wxT("BESI0"), wxT("SIN"), wxT("SINE"),
    wxT("BESI1"), wxT("BESK0"), wxT("COS"), wxT("COSINE"), wxT("BESK1"), wxT("BESJ0"),
    wxT("TAN"), wxT("TANGENT"), wxT("BESJ1"), wxT("BESY0"), wxT("ASIN"), wxT("ARSIN"),
    wxT("BESY1"), wxT("COSINT"), wxT("DAWSON"), wxT("ELLICK"), wxT("ACOS"), wxT("ARCOS"),
    wxT("ELLICE"), wxT("ERF"), wxT("EXPINT"), wxT("FREQ"), wxT("ATAN"), wxT("ARTAN"),
    wxT("ARCTAN"), wxT("GAUSSIN"), wxT("SININT"), wxT("GAMMA"), wxT("ATAN2"), wxT("ARTAN2"),
    wxT("ARCTAN2"), wxT("DILOG"), wxT("STRUVE0"), wxT("STRUVE1"), wxT("SINH"), wxT("COSH"),
    wxT("TANH"), wxT("CSCH"), wxT("SECH"), wxT("COTH"), wxT("ABS"), wxT("ASINH"),
    wxT("ARSINH"), wxT("ARCOSH"), wxT("MAX"), wxT("ACOSH"), wxT("ACOTH"), wxT("ARCOTH"),
    wxT("MIN"), wxT("ASECH"), wxT("ARSECH"), wxT("ACSCH"), wxT("ARCSCH"), wxT("COT"),
    wxT("SEC"), wxT("CSC"), wxT("INT"), wxT("IFIX"), wxT("MOD"), wxT("ARCOT"),
    wxT("ACOT"), wxT("ARSEC"), wxT("SIGN"), wxT("ASEC"), wxT("DIM"), wxT("ARCSC"),
    wxT("RAN"), wxT("VOIGT"), wxT("ACSC"), wxT("FREC1"), wxT("FREC2"), wxT("FRES1"),
    wxT("FRES2"), wxT("PLM"), wxT("PLMN"), wxT("PLMU"), wxT("FERDIRAC"), wxT("ERFC"),
    wxT("BIRY"), wxT("EI"), wxT("FACTORIAL"), wxT("BER"), wxT("BEI"), wxT("AERF"),
    wxT("EXPN"), wxT("RADMAC"), wxT("WALSH"), wxT("BETA"), wxT("BINOM"), wxT("CHEBY"),
    wxT("CHISQ"), wxT("CHISQINV"), wxT("FINELLIC"), wxT("EINELLIC"), wxT("GAMMAIN"), wxT("GAMMACIN"),
    wxT("GAMMATIN"), wxT("HERMITE"), wxT("LAGUERRE"), wxT("LEGENDRE"), wxT("STUDENT"), wxT("STUDENTI"),
    wxT("BETAIN"), wxT("BIVARNOR"), wxT("CHLOGU"), wxT("FISHER"), wxT("POICA"), wxT("JACOBI"),
    wxT("HYPGEO"), wxT("KER"), wxT("KEI"), wxT("SIND"), wxT("COSD"), wxT("TAND"),
    wxT("ASIND"), wxT("ACOSD"), wxT("ATAND"), wxT("ATAN2D"), wxT("NINT"), wxT("AERFC"),
    wxT("GAUSS"), wxT("AGAUSS"), wxT("PROB"), wxT("ATANH"), wxT("ARTANH"), wxT("GAMMLN"),
    wxT("GAMMQ"), wxT("CLEBSG"), wxT("WIGN3J"), wxT("WIGN6J"), wxT("WIGN9J"), wxT("RACAHC"),
    wxT("JAHNUF"), wxT("NORMAL"), wxT("TINA"), wxT("ELTIME"), wxT("RCHAR"), wxT("CHAR"),
    wxT("ICHAR"), wxT("EVALUATE"), wxT("EXPAND"), wxT("DATE"), wxT("TIME"), wxT("UCASE"),
    wxT("LCASE"), wxT("TCASE"), wxT("INDEX"), wxT("CLEN"), wxT("VLEN"), wxT("LEN"),
    wxT("WHERE"), wxT("EQS"), wxT("NES"), wxT("SUB"), wxT("SUP"), wxT("ROLL"),
    wxT("STEP"), wxT("WRAP"), wxT("NULL"), wxT("FOLD"), wxT("UNFOLD"), wxT("CONVOLUTE"),
    wxT("INTEGRAL"), wxT("DERIVATIVE"), wxT("INTERPOLATE"), wxT("SMOOTH"), wxT("FFT"), wxT("SAVGOL"),
    wxT("SPLINTERP"), wxT("SPLSMOOTH"), wxT("FIRST"), wxT("LAST"), wxT("IFFT"), wxT("EXIST"),
    wxT("VARNAME"), wxT("ICLOSE"), wxT("GAUSSJ"), wxT("AREA"), wxT("TRANSLATE"), wxT("PFACTORS"),
    wxT("INVERSE"), wxT("DETERMINANT"), wxT("IDENTITY"), wxT("EIGEN"), wxT("IEQUAL"), wxT("STRING"),
    wxT("JOIN"), wxT("VARTYPE")
  };
  wxString nameUp( name.Upper() );
  for( std::size_t i=0; i<nNames; ++i )
  {
    if( nameUp == reservedName[i] )
      throw EVariableError( wxString(wxT("attempt to use reserved name: "))+nameUp );
  }
}

void Variable::CopyStuff( Variable const &rhs )
{
  name_ = rhs.name_;
  history_.assign( rhs.history_.begin(), rhs.history_.end() );
}

// end of file
