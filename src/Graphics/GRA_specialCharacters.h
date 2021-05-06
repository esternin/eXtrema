/*
Copyright (C) 2021 Edward Sternin

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
#ifndef GRA_SPECIALCHARACTERS
#define GRA_SPECIALCHARACTERS

struct GRA_specialCharacter
  {
    wxChar const *ename;   // special character eXtrema name
    wxChar const *pname;   // special character PostScript name
    wxChar const ucode;    // Unicode character code
    wxChar const cid;      // remapped to this ascii
  };

extern GRA_specialCharacter SpecialCharacters[];
/*
GRA_specialCharacter SpecialCharacters[] =
  {
      // for Greek letters make an approximation
      {wxT("ALPHA"),wxT("Alpha"),0x0391,0x41},	// A
      {wxT("alpha"),wxT("alpha"),0x03B1,0x61},	// a
      {wxT("BETA"),wxT("Beta"),0x0392,0x42},	// B
      {wxT("beta"),wxT("beta"),0x03B2,0x62},	// b
      {wxT("GAMMA"),wxT("Gamma"),0x0393,0x47},	// G
      {wxT("gamma"),wxT("gamma"),0x03B3,0x67},	// g
      {wxT("DELTA"),wxT("Delta"),0x0394,0x44},	// D
      {wxT("delta"),wxT("delta"),0x03B4,0x64},	// d
      {wxT("EPSILON"),wxT("Epsilon"),0x0395,0x45},	// E
      {wxT("epsilon"),wxT("epsilon"),0x03B5,0x65},	// e
      {wxT("ZETA"),wxT("Zeta"),0x0396,0x5A},	// Z
      {wxT("zeta"),wxT("zeta"),0x03B6,0x7A},	// z
      {wxT("ETA"),wxT("Eta"),0x0397,0x48},		// H
      {wxT("eta"),wxT("eta"),0x03B7,0x68},		// h
      {wxT("THETA"),wxT("Theta"),0x0398,0x46},	// F
      {wxT("theta"),wxT("theta"),0x03B8,0x66},	// f
      {wxT("IOTA"),wxT("Iota"),0x0399,0x49},	// I
      {wxT("iota"),wxT("iota"),0x03B9,0x69},	// i
      {wxT("KAPPA"),wxT("Kappa"),0x039A,0x4B},	// K
      {wxT("kappa"),wxT("kappa"),0x03BA,0x6B},	// k
      {wxT("LAMBDA"),wxT("Lambda"),0x039B,0x4C},	// L
      {wxT("lambda"),wxT("lambda"),0x03BB,0x6C},	// l
      {wxT("MU"),wxT("Mu"),0x039C,0x4D},		// M
      {wxT("mu"),wxT("mu"),0x03BC,0x6D},		// m
      {wxT("NU"),wxT("Nu"),0x039D,0x4E},		// N
      {wxT("nu"),wxT("nu"),0x03BD,0x6E},		// n
      {wxT("XI"),wxT("Xi"),0x039E,0x51},		// Q
      {wxT("xi"),wxT("xi"),0x03BE,0x71},		// q
      {wxT("OMICRON"),wxT("Omicron"),0x039F,0x4F},	// O
      {wxT("omicron"),wxT("omicron"),0x03BF,0x6F},	// o
      {wxT("PI"),wxT("Pi"),0x03A0,0x50},		// P
      {wxT("pi"),wxT("pi"),0x03C0,0x70},		// P
      {wxT("RHO"),wxT("Rho"),0x03A1,0x52},		// R
      {wxT("rho"),wxT("rho"),0x03C1,0x72},		// r
      {wxT("SIGMA"),wxT("Sigma"),0x03A3,0x53},	// S
      {wxT("sigma"),wxT("sigma"),0x03C3,0x73},	// s
      {wxT("TAU"),wxT("Tau"),0x03A4,0x54},		// T
      {wxT("tau"),wxT("tau"),0x03C4,0x74},		// t
      {wxT("UPSILON"),wxT("Upsilon"),0x03A5,0x55},	// U
      {wxT("upsilon"),wxT("upsilon"),0x03C5,0x75},	// u
      {wxT("PHI"),wxT("Phi"),0x03A6,0x56},		// V
      {wxT("phi"),wxT("phi"),0x03C6,0x76},		// v
      {wxT("CHI"),wxT("Chi"),0x03A7,0x58},		// X
      {wxT("chi"),wxT("chi"),0x03C7,0x78},		// x
      {wxT("PSI"),wxT("Psi"),0x03A8,0x59},		// Y
      {wxT("psi"),wxT("psi"),0x03C8,0x79},		// y
      {wxT("OMEGA"),wxT("Omega"),0x03A9,0x57},	// W
      {wxT("omega"),wxT("omega"),0x03C9,0x77},	// w
      // for the rest, fill in order, unless there is an obvious candidate in ASCII
      {wxT("VARTHETA"),wxT("theta1"),0x03D1,0x01},
      {wxT("VARPHI"),wxT("phi1"),0x03D5,0x02},
      {wxT("VAREPSILON"),wxT("uni03F5"),0x03F5,0x03},
      {wxT("ALEPH"),wxT("aleph"),0x2135,0x04},
      {wxT("NABLA"),wxT("nabla"),0x2207,0x05},
      {wxT("PARTIAL"),wxT("partialdiff"),0x2202,0x06},
      {wxT("LEFTARROW"),wxT("arrowleft"),0x2190,0x5F},	// _
      {wxT("UPARROW"),wxT("arrowup"),0x2191,0x5E},	// ^
      {wxT("DOWNARROW"),wxT("arrowdown"),0x2193,0x5B},	// [
      {wxT("RIGHTARROW"),wxT("arrowright"),0x2192,0x5D},// ]
      {wxT("PERP"),wxT("perpendicular"),0x27C2,0x07},
      {wxT("MID"),wxT("divides"),0x2223,0x7E},
      {wxT("BULLET"),wxT("bullet"),0x2022,0x08},
      {wxT("SUM"),wxT("summation"),0x2211,0x09},
      {wxT("PROD"),wxT("product"),0x220F,0x0A},
      {wxT("INT"),wxT("integral"),0x222B,0x0B},
      {wxT("SURD"),wxT("radical"),0x221A,0x0C},
      {wxT("PLUS"),wxT("plus"),0x002B,0x002B},
      {wxT("MINUS"),wxT("minus"),0x2212,0x2D},		// -
      {wxT("PM"),wxT("plusminus"),0x00B1,0x3B},		// ;
      {wxT("TIMES"),wxT("multiply"),0x00D7,0x2A},	// *
      {wxT("DIV"),wxT("divide"),0x00F7,0x3A},		// :
      {wxT("OPLUS"),wxT("pluscircle"),0x2295,0x0D},
      {wxT("OTIMES"),wxT("timescircle"),0x2297,0x0E},
      {wxT("CAP"),wxT("intersection"),0x2229,0x0F},
      {wxT("SUBSET"),wxT("propersubset"),0x2282,0x10},
      {wxT("CUP"),wxT("union"),0x222A,0x11},
      {wxT("SUPSET"),wxT("propersuperset"),0x2283,0x12},
      {wxT("DEGREE"),wxT("degree"),0x02DA,0x13},	//
      {wxT("LANGLE"),wxT("angleleft"),0x27E8,0x14},	//
      {wxT("RANGLE"),wxT("angleright"),0x27E9,0x15},	//
      {wxT("NEG"),wxT("logicalnot"),0x00AC,0x7C},
      {wxT("THEREFORE"),wxT("therefore"),0x2234,0x16},
      {wxT("ANGLE"),wxT("angle"),0x2220,0x17},
      {wxT("VEE"),wxT("logicalor"),0x2228,0x18},
      {wxT("WEDGE"),wxT("logicaland"),0x2227,0x19},
      {wxT("CDOT"),wxT("uni22C5"),0x22C5,0x2E},		// .
      {wxT("INFTY"),wxT("infinity"),0x221E,0x1A},
      {wxT("IN"),wxT("element"),0x2208,0x1B},
      {wxT("NI"),wxT("suchthat"),0x220B,0x1C},
      {wxT("PROPTO"),wxT("proportional"),0x221D,0x1D},
      {wxT("EXISTS"),wxT("thereexists"),0x2203,0x1E},
      {wxT("FORALL"),wxT("universal"),0x2200,0x1F},
      {wxT("NEQ"),wxT("notequal"),0x2260,0x21},		// !
      {wxT("EQUIV"),wxT("equivalence"),0x2261,0x3D},	// =
      {wxT("APPROX"),wxT("approxequal"),0x2248,0x22},	// "
      {wxT("SIM"),wxT("similar"),0x007E,0x5C},		// ~
      {wxT("LT"),wxT("less"),0x003C,0x3C},		// <
      {wxT("GT"),wxT("greater"),0x003E,0x3E},		// >
      {wxT("LEQ"),wxT("lessequal"),0x2264,0x23},		// #
      {wxT("GEQ"),wxT("greaterequal"),0x2265,0x24},		// $
      {wxT("VAROMEGA"),wxT("omega1"),0x03D6,0x25},		// %
      {wxT("CLUBSUIT"),wxT("club"),0x2663,0x26},		// &
      {wxT("DIAMONDSUIT"),wxT("diamond"),0x2666,0x27},		// '
      {wxT("HEARTSUIT"),wxT("heart"),0x2665,0x28},		// (
      {wxT("SPADESUIT"),wxT("spade"),0x2660,0x29},		// )
      {wxT("SUBSETEQ"),wxT("subsetorequal"),0x2286,0x2F},	// /
      {wxT("SUPSETEQ"),wxT("supersetorequal"),0x2287,0x30},	// 0
      {wxT("LDOTS"),wxT("ellipsis"),0x2026,0x31},		// 1
      {wxT("LEFTRIGHTARROW"),wxT("arrowdblboth"),0x21D4,0x32},	// 2
      {wxT("EMPTYSET"),wxT("emptyset"),0x2205,0x33},		// 3
      {wxT("WP"),wxT("weierstrass"),0x2118,0x34},		// 4
      {wxT("RE"),wxT("Rfraktur"),0x211C,0x35},			// 5
      {wxT("IM"),wxT("Ifraktur"),0x2111,0x36},			// 6
      {wxT("HOOKLEFTARROW"),wxT("carriagereturn"),0x21B5,0x37}, 	// 7 0x21B5 ?
      {wxT("COPYRIGHT"),wxT("copyright"),0x00A9,0x38},		// 8
      {wxT("REGISTERED"),wxT("registered"),0x00AE,0x39},	// 9
      {wxT("TM"),wxT("trademark"),0x2122,0x4A},			// J
      {wxT("DIAMOND"),wxT("lozenge"),0x25CA,0x6A} 		// j
  };
*/
#endif
