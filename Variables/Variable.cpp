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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "Variable.h"
#include "EVariableError.h"
#include "ExGlobals.h"

void Variable::AddToHistory( ExString s )
{
  history_.push_front(s);
  if( static_cast<int>(history_.size()) >= ExGlobals::GetMaxHistory() )history_.pop_back();
}

ExString Variable::SimpleName( ExString const &name )
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
  ExString uName( name.UpperCase() );
  //
  int start = -1;
  int newState = 1;
  std::size_t i = 0;
  while ( i < uName.length() )
  {
    int currentState = newState;
    std::size_t iascii = toascii( uName[i] );
    newState = stateTable[currentState-1][classes[iascii]-1];
    if( newState == 2 )         // start variable name
      start = i;
    else if( newState == 100 )  // variable with no indices
      break;
    else if( newState == -10 )
      throw EVariableError( ExString("invalid first character in name: ")+name );
    else if( newState == -20 )
      throw EVariableError( "no variable name was given" );
    else if( newState == -30 )
      throw EVariableError( uName[i]+" is an invalid character in "+name );
    ++i;
  }
  if( start == -1 )throw EVariableError("no variable name was given");
  return uName.substr(start,i-start);
}

void Variable::CheckReservedName( ExString const &name )
{
  std::size_t const nNames = 200;
  ExString const reservedName[nNames] =
  {
         "LOG",          "LN",     "DIGAMMA",         "PSI",       "LOG10",        "AIRY",
         "EXP",       "LOGAM",        "SQRT",       "BESI0",         "SIN",        "SINE",
       "BESI1",       "BESK0",         "COS",      "COSINE",       "BESK1",       "BESJ0",
         "TAN",     "TANGENT",       "BESJ1",       "BESY0",        "ASIN",       "ARSIN",
       "BESY1",      "COSINT",      "DAWSON",      "ELLICK",        "ACOS",       "ARCOS",
      "ELLICE",         "ERF",      "EXPINT",        "FREQ",        "ATAN",       "ARTAN",
      "ARCTAN",     "GAUSSIN",      "SININT",       "GAMMA",       "ATAN2",      "ARTAN2",
     "ARCTAN2",       "DILOG",     "STRUVE0",     "STRUVE1",        "SINH",        "COSH",
        "TANH",        "CSCH",        "SECH",        "COTH",         "ABS",       "ASINH",
      "ARSINH",      "ARCOSH",         "MAX",       "ACOSH",       "ACOTH",      "ARCOTH",
         "MIN",       "ASECH",      "ARSECH",       "ACSCH",      "ARCSCH",         "COT",
         "SEC",         "CSC",         "INT",        "IFIX",         "MOD",       "ARCOT",
        "ACOT",       "ARSEC",        "SIGN",        "ASEC",         "DIM",       "ARCSC",
         "RAN",       "VOIGT",        "ACSC",       "FREC1",       "FREC2",       "FRES1",
       "FRES2",         "PLM",        "PLMN",        "PLMU",    "FERDIRAC",        "ERFC",
        "BIRY",          "EI",   "FACTORIAL",         "BER",         "BEI",        "AERF",
        "EXPN",      "RADMAC",       "WALSH",        "BETA",       "BINOM",       "CHEBY",
       "CHISQ",    "CHISQINV",    "FINELLIC",    "EINELLIC",     "GAMMAIN",    "GAMMACIN",
    "GAMMATIN",     "HERMITE",    "LAGUERRE",    "LEGENDRE",     "STUDENT",    "STUDENTI",
      "BETAIN",    "BIVARNOR",      "CHLOGU",      "FISHER",       "POICA",      "JACOBI",
      "HYPGEO",         "KER",         "KEI",        "SIND",        "COSD",        "TAND",
       "ASIND",       "ACOSD",       "ATAND",      "ATAN2D",        "NINT",       "AERFC",
       "GAUSS",      "AGAUSS",        "PROB",       "ATANH",      "ARTANH",      "GAMMLN",
       "GAMMQ",      "CLEBSG",      "WIGN3J",      "WIGN6J",      "WIGN9J",      "RACAHC",
      "JAHNUF",      "NORMAL",        "TINA",      "ELTIME",       "RCHAR",        "CHAR",
       "ICHAR",    "EVALUATE",      "EXPAND",        "DATE",        "TIME",       "UCASE",
       "LCASE",       "TCASE",       "INDEX",        "CLEN",        "VLEN",         "LEN",
       "WHERE",         "EQS",         "NES",         "SUB",         "SUP",        "ROLL",
        "STEP",        "WRAP",        "NULL",        "FOLD",      "UNFOLD",   "CONVOLUTE",
    "INTEGRAL",  "DERIVATIVE", "INTERPOLATE",      "SMOOTH",         "FFT",      "SAVGOL",
   "SPLINTERP",   "SPLSMOOTH",       "FIRST",        "LAST",        "IFFT",       "EXIST",
     "VARNAME",      "ICLOSE",      "GAUSSJ",        "AREA",   "TRANSLATE",    "PFACTORS",
     "INVERSE", "DETERMINANT",    "IDENTITY",       "EIGEN",      "IEQUAL",      "STRING",
        "JOIN",     "VARTYPE"
  };
  ExString nameUp( name.UpperCase() );
  for( std::size_t i=0; i<nNames; ++i )
  {
    if( nameUp == reservedName[i] )
      throw EVariableError( ExString("attempt to use reserved name: ")+nameUp );
  }
}

// end of file
