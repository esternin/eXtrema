/*
Copyright (C) 2005,...,2007 Joseph L. Chuma, TRIUMF

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
#include <stdexcept>

#include "wx/wx.h"

#include "GRA_drawableText.h"
#include "EGraphicsError.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_window.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_simpleText.h"
#include "GRA_font.h"
#include "GRA_fontControl.h"
#include "GRA_wxWidgets.h"
#include "ExGlobals.h"
#include "EVariableError.h"
#include "ToAscii.h"
#include "UsefulFunctions.h"
#include "TextPopup.h"
#include "GRA_specialCharacters.h"

std::vector <GRA_specialCharacter> SpecialCharacters {
      // for Greek letters make an approximation, helps debugging PS fonts
      {"ALPHA","Alpha",0x0391,'A'},
      {"alpha","alpha",0x03B1,'a'},
      {"BETA","Beta",0x0392,'B'},
      {"beta","beta",0x03B2,'b'},
      {"GAMMA","Gamma",0x0393,'G'},
      {"gamma","gamma",0x03B3,'g'},
      {"DELTA","Delta",0x0394,'D'},
      {"delta","delta",0x03B4,'d'},
      {"EPSILON","Epsilon",0x0395,'E'},
      {"epsilon","epsilon",0x03B5,'e'},
      {"ZETA","Zeta",0x0396,'Z'},
      {"zeta","zeta",0x03B6,'z'},
      {"ETA","Eta",0x0397,'H'},	
      {"eta","eta",0x03B7,'h'},
      {"THETA","Theta",0x0398,'F'},
      {"theta","theta",0x03B8,'f'},
      {"IOTA","Iota",0x0399,'I'},
      {"iota","iota",0x03B9,'i'},
      {"KAPPA","Kappa",0x039A,'K'},
      {"kappa","kappa",0x03BA,'k'},
      {"LAMBDA","Lambda",0x039B,'L'},
      {"lambda","lambda",0x03BB,'l'},
      {"MU","Mu",0x039C,'M'},
      {"mu","mu",0x03BC,'m'},
      {"NU","Nu",0x039D,'N'},
      {"nu","nu",0x03BD,'n'},
      {"XI","Xi",0x039E,'Q'},
      {"xi","xi",0x03BE,'q'},
      {"OMICRON","Omicron",0x039F,'O'},
      {"omicron","omicron",0x03BF,'o'},
      {"PI","Pi",0x03A0,'P'},
      {"pi","pi",0x03C0,'p'},
      {"RHO","Rho",0x03A1,'R'},
      {"rho","rho",0x03C1,'r'},
      {"SIGMA","Sigma",0x03A3,'S'},
      {"sigma","sigma",0x03C3,'s'},
      {"TAU","Tau",0x03A4,'T'},
      {"tau","tau",0x03C4,'t'},
      {"UPSILON","Upsilon",0x03A5,'U'},
      {"upsilon","upsilon",0x03C5,'u'},
      {"PHI","Phi",0x03A6,'V'},
      {"phi","phi",0x03C6,'v'},
      {"CHI","Chi",0x03A7,'X'},
      {"chi","chi",0x03C7,'x'},
      {"PSI","Psi",0x03A8,'Y'},
      {"psi","psi",0x03C8,'y'},	
      {"OMEGA","Omega",0x03A9,'W'},
      {"omega","omega",0x03C9,'w'},
      // for the rest, fill in order, unless there is an obvious candidate in ASCII
      {"VARTHETA","theta1",0x03D1,0x01},
      {"VARPHI","phi1",0x03D5,0x02},
      {"VAREPSILON","uni03F5",0x03F5,0x03},
      {"ALEPH","aleph",0x2135,0x04},
      {"NABLA","nabla",0x2207,0x05},
      {"PARTIAL","partialdiff",0x2202,0x06},
      {"LEFTARROW","arrowleft",0x2190,0x5F},	// _
      {"UPARROW","arrowup",0x2191,0x5E},	// ^
      {"DOWNARROW","arrowdown",0x2193,0x5B},	// [
      {"RIGHTARROW","arrowright",0x2192,0x5D},// ]
      {"PERP","perpendicular",0x22A5,0x07},
      {"MID","divides",0x2223,0x7E},
      {"BULLET","bullet",0x2022,0x08},
      {"SUM","summation",0x2211,0x09},
      {"PROD","product",0x220F,0x0A},
      {"INT","integral",0x222B,0x0B},
      {"SURD","radical",0x221A,0x0C},
      {"PLUS","plus",0x002B,0x002B},
      {"MINUS","minus",0x2212,0x2D},		// -
      {"PM","plusminus",0x00B1,0x3B},		// ;
      {"TIMES","multiply",0x00D7,0x2A},	// *
      {"DIV","divide",0x00F7,0x3A},		// :
      {"OPLUS","pluscircle",0x2295,0x0D},
      {"OTIMES","timescircle",0x2297,0x0E},
      {"CAP","intersection",0x2229,0x0F},
      {"SUBSET","propersubset",0x2282,0x10},
      {"CUP","union",0x222A,0x11},
      {"SUPSET","propersuperset",0x2283,0x12},
      {"DEGREE","degree",0x02DA,0x13},	//
      {"LANGLE","angleleft",0x2329,0x14},	//
      {"RANGLE","angleright",0x232A,0x15},	//
      {"NEG","logicalnot",0x00AC,0x7C},
      {"THEREFORE","therefore",0x2234,0x16},
      {"ANGLE","angle",0x2220,0x17},
      {"VEE","logicalor",0x2228,0x18},
      {"WEDGE","logicaland",0x2227,0x19},
      {"CDOT","dotmath",0x22C5,0x2E},		// .
      {"INFTY","infinity",0x221E,0x1A},
      {"IN","element",0x2208,0x1B},
      {"NI","suchthat",0x220B,0x1C},
      {"PROPTO","proportional",0x221D,0x1D},
      {"EXISTS","thereexists",0x2203,0x1E},
      {"FORALL","universal",0x2200,0x1F},
      {"NEQ","notequal",0x2260,0x21},		// !
      {"EQUIV","equivalence",0x2261,0x3D},	// =
      {"APPROX","approxequal",0x2248,0x22},	// "
      {"SIM","similar",0x007E,0x5C},		// ~
      {"LT","less",0x003C,0x3C},		// <
      {"GT","greater",0x003E,0x3E},		// >
      {"LEQ","lessequal",0x2264,0x23},		// #
      {"GEQ","greaterequal",0x2265,0x24},		// $
      {"VARPI","omega1",0x03D6,0x25},		// %
      {"CLUBSUIT","club",0x2663,0x26},		// &
      {"DIAMONDSUIT","diamond",0x2666,0x27},		// '
      {"HEARTSUIT","heart",0x2665,0x28},		// (
      {"SPADESUIT","spade",0x2660,0x29},		// )
      {"SUBSETEQ","subsetorequal",0x2286,0x2F},	// /
      {"SUPSETEQ","supersetorequal",0x2287,0x30},	// 0
      {"LDOTS","ellipsis",0x2026,0x31},		// 1
      {"LEFTRIGHTARROW","arrowdblboth",0x21D4,0x32},	// 2
      {"EMPTYSET","emptyset",0x2205,0x33},		// 3
      {"WP","weierstrass",0x2118,0x34},		// 4
      {"RE","Rfraktur",0x211C,0x35},			// 5
      {"IM","Ifraktur",0x2111,0x36},			// 6
      {"HOOKLEFTARROW","carriagereturn",0x21B5,0x37}, 	// 7 0x21B5 ?
      {"COPYRIGHT","copyright",0x00A9,0x38},		// 8
      {"REGISTERED","registered",0x00AE,0x39},	// 9
      {"TM","trademark",0x2122,0x4A},			// J
      {"DIAMOND","lozenge",0x25CA,0x6A} 		// j
  };

namespace
{

// Return font needed for the simple simple text display.
wxFont MakeFont( GRA_wxWidgets* graphicsOutput, GRA_simpleText* text )
{
  double height = text->GetHeight();
  wxFont font( text->GetFont()->GetwxFont() );
  int xo, yo1, yo2;
  graphicsOutput->WorldToOutputType( 0.0, 0.0, xo, yo1 );
  graphicsOutput->WorldToOutputType( 0.0, height, xo, yo2 );
  int h = yo1 - yo2;
  font.SetPointSize( h );
  font.SetWeight( text->GetWeight() );
  font.SetStyle( text->GetStyle() );
  wxLogTrace("font", "GRA_drawableText::MakeFont: world height=%g, font height=%d, style=%d(%d), weight=%d(%d)",
          height, h, text->GetStyle(), wxFONTSTYLE_NORMAL, text->GetWeight(), wxFONTWEIGHT_NORMAL);
  return font;
}

} // anonymous namespace

GRA_drawableText::GRA_drawableText( wxString const &inputString, double height,
                                    double angle, double x, double y, int alignment,
                                    GRA_font *font, GRA_color *color )
    : GRA_drawableObject(wxT("DRAWABLETEXT")), inputString_(inputString), height_(height),
      angle_(angle), x_(x), y_(y), alignment_(alignment),
      color_(color), font_(font), graphUnits_(false),
      subsupFactor_(0.75), popup_(false)
{}

GRA_drawableText::GRA_drawableText( wxString const &inputString, bool graphUnits )
    : GRA_drawableObject(wxT("DRAWABLETEXT")), inputString_(inputString), graphUnits_(graphUnits),
      subsupFactor_(0.75), popup_(false)
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *textCharacteristics = gw->GetTextCharacteristics();
  x_ = static_cast<GRA_distanceCharacteristic*>(textCharacteristics->Get(wxT("XLOCATION")))->GetAsWorld();
  y_ = static_cast<GRA_distanceCharacteristic*>(textCharacteristics->Get(wxT("YLOCATION")))->GetAsWorld();
  if( graphUnits_ )
  {
    double xg=x_, yg=y_;
    ExGlobals::GetGraphWindow()->WorldToGraph( xg, yg, x_, y_ );
  }
  angle_ = static_cast<GRA_angleCharacteristic*>(textCharacteristics->Get(wxT("ANGLE")))->Get();
  alignment_ = static_cast<GRA_intCharacteristic*>(textCharacteristics->Get(wxT("ALIGNMENT")))->Get();
  font_ = static_cast<GRA_fontCharacteristic*>(textCharacteristics->Get(wxT("FONT")))->Get();
  color_ = static_cast<GRA_colorCharacteristic*>(textCharacteristics->Get(wxT("COLOR")))->Get();
  height_ = static_cast<GRA_sizeCharacteristic*>(textCharacteristics->Get(wxT("HEIGHT")))->GetAsWorld();
}

GRA_drawableText::~GRA_drawableText()
{
  DeleteStuff();
  if( popup_ )ExGlobals::DisconnectTextPopup();
}

void GRA_drawableText::DeleteStuff()
{
  while ( !strings_.empty() )
  {
    delete strings_.back();
    strings_.pop_back();
  }
}

void GRA_drawableText::CopyStuff( GRA_drawableText const &rhs )
{
  x_ = rhs.x_;
  y_ = rhs.y_;
  graphUnits_ = rhs.graphUnits_;
  angle_ = rhs.angle_;
  alignment_ = rhs.alignment_;
  height_ = rhs.height_;
  font_ = rhs.font_;
  color_ = rhs.color_;
  inputString_ = rhs.inputString_;
  textVec().swap( strings_ );
  textVecIter end = rhs.strings_.end();
  for( textVecIter i=rhs.strings_.begin(); i!=end; ++i )
   strings_.push_back( new GRA_simpleText(**i) );
  popup_ = rhs.popup_;
}

bool GRA_drawableText::operator==( GRA_drawableText const &rhs ) const
{
  if( inputString_ != rhs.inputString_ || x_ != rhs.x_ || y_ != rhs.y_ ||
      graphUnits_ != rhs.graphUnits_ || angle_ != rhs.angle_ || height_ != rhs.height_ ||
      alignment_ != rhs.alignment_ || font_ != rhs.font_ || color_ != rhs.color_ )return false;
  return true;
}

std::ostream &operator<<( std::ostream &out, GRA_drawableText const &dt )
{
  return out << "<drawabletext x=\"" << dt.x_ << "\" y=\"" << dt.y_ << "\" height=\""
             << dt.height_ << "\" angle=\"" << dt.angle_ << "\" alignment=\""
             << dt.alignment_ << "\" color=\"" << GRA_colorControl::GetColorCode(dt.color_)
             << "\" font=\"" << GRA_fontControl::GetFontCode(dt.font_)
             << "\" graphunits=\"" << dt.graphUnits_ << "\">\n"
             << "<string><![CDATA[" << dt.inputString_.mb_str(wxConvUTF8)
             << "]]></string>\n</drawabletext>\n";
}

void GRA_drawableText::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  int xminM, yminM, xmaxM, ymaxM;
  graphicsOutput->GetLimits( xminM, yminM, xmaxM, ymaxM );
  double xminW, yminW, xmaxW, ymaxW;
  ExGlobals::GetWorldLimits( xminW, yminW, xmaxW, ymaxW );
  double yConvert = (ymaxW-yminW)/(ymaxM-yminM);
  double xConvert = (xmaxW-xminW)/(xmaxM-xminM);
  //
  std::size_t size;
  double angle, cosx, sinx, xLoc, yLoc;
  std::vector<int> wv;
  GetStringStuff( size, angle, cosx, sinx, xLoc, yLoc, wv, xConvert, yConvert, graphicsOutput, dc );
  double width = 0.0;
  for( std::size_t i=0; i<size; ++i )
  {
    xLoc += width*cosx - strings_[i]->GetYShift()*sinx + strings_[i]->GetXShift()*cosx;
    yLoc += width*sinx + strings_[i]->GetYShift()*cosx + strings_[i]->GetXShift()*sinx;
    width = wv[i]*xConvert;

    int x, y;
    graphicsOutput->WorldToOutputType( xLoc, yLoc, x, y );
    dc.SetFont( MakeFont( graphicsOutput, strings_[i] ) );
    dc.SetTextForeground( ExGlobals::GetwxColor(strings_[i]->GetColor()) );
    dc.DrawRotatedText( strings_[i]->GetString(), x, y, angle );
    //
    int w, h, descent, externalLeading;
    dc.GetTextExtent( strings_[i]->GetString(), &w, &h, &descent, &externalLeading );
    strings_[i]->SetBoundary( x, y, w, h );
  }
}

void GRA_drawableText::Erase( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  int xminM, yminM, xmaxM, ymaxM;
  graphicsOutput->GetLimits( xminM, yminM, xmaxM, ymaxM );
  double xminW, yminW, xmaxW, ymaxW;
  ExGlobals::GetWorldLimits( xminW, yminW, xmaxW, ymaxW );
  double yConvert = (ymaxW-yminW)/(ymaxM-yminM);
  double xConvert = (xmaxW-xminW)/(xmaxM-xminM);
  //
  std::size_t size;
  double angle, cosx, sinx, xLoc, yLoc;
  std::vector<int> wv;
  GetStringStuff( size, angle, cosx, sinx, xLoc, yLoc, wv, xConvert, yConvert, graphicsOutput, dc );
  double width = 0.0;
  for( std::size_t i=0; i<size; ++i )
  {
    xLoc += width*cosx - strings_[i]->GetYShift()*sinx + strings_[i]->GetXShift()*cosx;
    yLoc += width*sinx + strings_[i]->GetYShift()*cosx + strings_[i]->GetXShift()*sinx;
    width = wv[i]*xConvert;

    int x, y;
    graphicsOutput->WorldToOutputType( xLoc, yLoc, x, y );
    dc.SetFont( MakeFont( graphicsOutput, strings_[i] ) );
    dc.SetTextForeground( dc.GetBackground().GetColour() );
    dc.DrawRotatedText( strings_[i]->GetString(), x, y, angle );
  }
}

void GRA_drawableText::GetStringStuff( std::size_t &size,
                                       double &angle, double &cosx, double &sinx,
                                       double &xLoc, double &yLoc,
                                       std::vector<int> &wv,
                                       double xConvert, double yConvert,
                                       GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  int length = 0;
  int maxHeight = 0;
  size = strings_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    dc.SetFont( MakeFont( graphicsOutput, strings_[i] ) );

    int w, h, descent, extLead;
    dc.GetTextExtent( strings_[i]->GetString(), &w, &h, &descent, &extLead );
    wv.push_back( w );
    maxHeight = std::max( maxHeight, h );
    length += w;
  }
  double width = length*xConvert;
  double height = maxHeight*yConvert;
  AlignXY( width, height, angle, xLoc, yLoc, sinx, cosx );
}

void GRA_drawableText::AlignXY( double width, double height,
                                double &angle, double &xLoc, double &yLoc,
                                double &sinx, double &cosx )
{
  angle = angle_;
  xLoc = x_;
  yLoc = y_;
  if( graphUnits_ )
  {
    double xg=xLoc, yg=yLoc;
    ExGlobals::GetGraphWindow()->GraphToWorld( xg, yg, xLoc, yLoc );
  }
  cosx = cos( angle*M_PI/180.0 );
  sinx = sin( angle*M_PI/180.0 );
  double const eps = 0.0001;
  if( fabs(cosx) <= eps )cosx = 0.0;
  if( fabs(sinx) <= eps )sinx = 0.0;
  if( fabs(cosx-1.0) <= eps )cosx = 1.0;
  if( fabs(sinx-1.0) <= eps )sinx = 1.0;
  //
  switch (alignment_)
  {
    case 1:               // lower left
      xLoc -= height*sinx;
      yLoc += height*cosx;
      break;
    case 2:               // lower center
      xLoc -= 0.5*width*cosx + height*sinx;
      yLoc -= 0.5*width*sinx - height*cosx;
      break;
    case 3:               // lower right
      xLoc -= width*cosx + height*sinx;
      yLoc -= width*sinx - height*cosx;
      break;
    case 4:               // center left
      xLoc -= 0.5*height*sinx;
      yLoc += 0.5*height*cosx;
      break;
    case 5:               // center center
      xLoc -= 0.5*width*cosx + 0.5*height*sinx;
      yLoc -= 0.5*width*sinx - 0.5*height*cosx;
      break;
    case 6:               // center right
      xLoc -= width*cosx + 0.5*height*sinx;
      yLoc -= width*sinx - 0.5*height*cosx;
      break;
    case 7:               // upper left
      break;
    case 8:               // upper center
      xLoc -= 0.5*width*cosx;
      yLoc -= 0.5*width*sinx;
      break;
    case 9:               // upper right
      xLoc -= width*cosx;
      yLoc -= width*sinx;
      break;
    case 10:               // lower left at 90deg
      angle = 90.0;
      cosx = 0.0;
      sinx = 1.0;
      xLoc -= height;
      break;
    case 11:              // lower left at 270deg
      angle = 270.0;
      cosx = 0.0;
      sinx = -1.0;
      xLoc += height;
      break;
    case 12:              // lower center at 90deg
      angle = 90.0;
      cosx = 0.0;
      sinx = 1.0;
      xLoc -= height;
      yLoc -= 0.5*width;
      break;
  }
}

void GRA_drawableText::Parse()
{
  textVec().swap( strings_ );
  //
  // multiple adjacent commands can entered as <comand><command>  or  <command,command>
  //
  double xShift=0.0, yShift=0.0;
  wxFontStyle style = wxFONTSTYLE_NORMAL;
  wxFontWeight weight = wxFONTWEIGHT_NORMAL;
  int upCounter = 0;
  int downCounter = 0;
  double height = height_;
  //
  int const classes[128] = {
   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 3,
   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 1, 3,
   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
   3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 };
  //
  //                               <   >   ,  othr
  int const stateTable[5][4] = { { 1,  0,  0,  0 },
                                 {-1, -2, -3,  2 },
                                 {-1,  3,  4,  2 },
                                 { 1, -2,  0,  0 },
                                 {-1, -2, -3,  2 } };
  wxString text, command;
  int currentState, newState=0;
  std::size_t length = inputString_.size();
  for( std::size_t i=0; i<length; ++i )
  {
    currentState = newState;
    newState = stateTable[currentState][classes[TryConvertToAscii(inputString_[i])]];
    switch (newState)
    {
      case 0: // just add to text
      {
        text.append( inputString_[i] );
        break;
      }
      case 1: // found <, start of command
      {
        if( !text.empty() )
        {
          GRA_simpleText *tmp = new GRA_simpleText(text,height,color_,font_,style,weight,xShift,yShift);
          strings_.push_back( tmp );
          text.erase();
          xShift = 0.0;
          yShift = 0.0;
        }
        break;
      }
      case 2: // add to command
      {
        command.append( inputString_[i] );
        break;
      }
      case 3: // found > (end of command)
      case 4: // found , (end of command)
      {
        try
        {
          DetermineCommand( command, height, xShift, yShift, style, weight, upCounter, downCounter );
        }
        catch ( std::runtime_error &e )
        {
          throw EGraphicsError( wxString(e.what(),wxConvUTF8) );
        }
        command.erase();
        break;
      }
      case -1:
      {
        wxString s(wxT("invalid < found at location "));
        s << i+1 << wxT(" in ") << inputString_;
        throw EGraphicsError( s );
      }
      case -2:
      {
        wxString s(wxT("invalid > found at location "));
        s << i+1 << wxT(" in ") << inputString_;
        throw EGraphicsError( s );
      }
      case -3:
      {
        wxString s(wxT("invalid comma found at location "));
        s << i+1 << wxT(" in ") << inputString_;
        throw EGraphicsError( s );
      }
    }
  }
  if( !text.empty() )
  {
    GRA_simpleText *tmp = new GRA_simpleText(text,height,color_,font_,style,weight,xShift,yShift);
    strings_.push_back( tmp );
  }
}

void GRA_drawableText::DetermineCommand( wxString &command,
                                         double &height, double &xShift, double &yShift,
                                         wxFontStyle &style, wxFontWeight &weight,
                                         int &upCounter, int &downCounter )

{
  wxString fontName;
  wxChar c = Special( command, fontName );
  if( c )
  {
    GRA_simpleText *tmp = new GRA_simpleText(c,height,color_,fontName.c_str(),style,weight,xShift,yShift);
    strings_.push_back( tmp );
    xShift = 0.0;
    yShift = 0.0;
    return;
  }
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
  double const xlowerw = static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("XLOWERWINDOW")))->GetAsWorld();
  double const xupperw = static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("XUPPERWINDOW")))->GetAsWorld();
  double const ylowerw = static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("YLOWERWINDOW")))->GetAsWorld();
  double const yupperw = static_cast<GRA_distanceCharacteristic*>(general->Get(wxT("YUPPERWINDOW")))->GetAsWorld();
  double const dx = xupperw-xlowerw;
  double const dy = yupperw-ylowerw;
  //
  std::size_t len = command.size();
  command.UpperCase();  // convert to upper case
  //
  if( command[0] == wxT('B') ) // boldface on/off
  {
    if(command.substr(1,len-1)==wxT('1'))
      weight = wxFONTWEIGHT_BOLD ;
    else
      weight = wxFONTWEIGHT_NORMAL ;
  }
  else if( command[0] == wxT('F') ) // set the font
  {
    font_ = GRA_fontControl::GetFont( command.substr(1,len-1) );
  }
  else if( command[0] == wxT('H') ) // set the height
  {
    bool percent = (command[len-1]==wxT('%'));
    std::size_t end = percent ? len-2 : len-1;
    wxString s( command.substr(1,end) );
    if( !s.ToDouble(&height) )throw std::runtime_error( "invalid value for height" );
    if( percent )height = std::min( dy, std::max(0.0,height*dy/100.) );
  }
  else if( command[0] == wxT('I') ) // italics on/off
  {
    if(command.substr(1,len-1)==wxT('1'))
      style = wxFONTSTYLE_ITALIC;
    else
      style = wxFONTSTYLE_NORMAL;
  }
  else if( command[0] == wxT('Z') ) // include a horizontal space
  {
    bool percent = (command[len-1]==wxT('%'));
    std::size_t end = percent ? len-2 : len-1;
    wxString s( command.substr(1,end) );
    if( !s.ToDouble(&xShift) )throw std::runtime_error( "invalid value for horizontal space" );
    if( percent )xShift *= dx/100.;
  }
  else if( command[0] == wxT('V') ) // include a vertical space
  {
    bool percent = (command[len-1]==wxT('%'));
    std::size_t end = percent ? len-2 : len-1;
    wxString s( command.substr(1,end) );
    if( !s.ToDouble(&yShift) )throw std::runtime_error( "invalid value for vertical space" );
    if( percent )yShift *= dy/100.;
  }
  else if( command[0] == wxT('^') ) // super-script flag
  {
    if( downCounter > 0 )
    {
      --downCounter;
      height /= subsupFactor_;
      yShift += 0.6*height;
    }
    else
    {
      ++upCounter;
      yShift += 0.6*height;
      height *= subsupFactor_;
    }
  }
  else if( command[0] == wxT('_') ) // sub-script flag
  {
    if( upCounter > 0 )
    {
      --upCounter;
      height /= subsupFactor_;
      yShift -= 0.6*height;
    }
    else
    {
      ++downCounter;
      yShift -= 0.6*height;
      height *= subsupFactor_;
    }
  }
  else if( command[0] == wxT('C') ) // colour
  {
    long int tmp;
    wxString s( command.substr(1,len-1) );
    s.ToLong( &tmp );
    GRA_color *c = GRA_colorControl::GetColor( static_cast<int>(tmp) );
    if( !c )throw std::runtime_error( "invalid color" );
    color_ = c;
  }
  else
  {
    wxString s( wxT("invalid text formatting command: \"") );
    s += command;
    s += wxT("\"");
    throw std::runtime_error( std::string(s.mb_str(wxConvUTF8)) );
  }
}

wxChar GRA_drawableText::Special( wxString const &command, wxString &fontName )
{
  wxString s( command.Upper() );
  bool upperCase = (const_cast<wxString&>(command)[0]==s[0]);
  wxChar uc;

  for( auto const& sc: SpecialCharacters )
  {
    if( s == sc.ename )
    {
      uc = sc.ucode;
      if( !upperCase && uc >= L'Α' && uc <= L'Ω' )  // Greek letters differ in upper/lowercase
	      uc += 0x0020 ;
      return uc;		// everything else is the same in upper/lowercase
    }
  }
  return 0;
}

bool GRA_drawableText::Inside( double x, double y )
{
  std::vector<double> xp(4,0.0), yp(4,0.0);
  std::vector<GRA_simpleText*>::const_iterator end = strings_.end();
  for( std::vector<GRA_simpleText*>::const_iterator i=strings_.begin(); i!=end; ++i )
  {
    int xlo, ylo, xhi, yhi;
    (*i)->GetBoundary( xlo, ylo, xhi, yhi );
    ExGlobals::GetGraphicsOutput()->OutputTypeToWorld( xlo, ylo, xp[0], yp[0] );
    ExGlobals::GetGraphicsOutput()->OutputTypeToWorld( xhi, yhi, xp[2], yp[2] );
    xp[1] = xp[2];
    yp[1] = yp[0];
    xp[3] = xp[0];
    yp[3] = yp[2];
    if( UsefulFunctions::InsidePolygon(x,y,xp,yp) )return true;
  }
  return false;
}

double GRA_drawableText::GetWidth() const
{
  std::vector<GRA_simpleText*>::const_iterator end = strings_.end();
  double width = 0.0;
  for( std::vector<GRA_simpleText*>::const_iterator i=strings_.begin(); i!=end; ++i )
  {
    int xlo, ylo, xhi, yhi;
    (*i)->GetBoundary( xlo, ylo, xhi, yhi );
    double xlow, ylow, xhiw, yhiw;
    ExGlobals::GetGraphicsOutput()->OutputTypeToWorld( xlo, ylo, xlow, ylow );
    ExGlobals::GetGraphicsOutput()->OutputTypeToWorld( xhi, yhi, xhiw, yhiw );
    width += xhiw-xlow;
  }
  return width;
}

// end of file
