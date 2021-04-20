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

#include "GRA_drawableText.h"
#include "EGraphicsError.h"
#include "EVariableError.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_window.h"
#include "GRA_colorControl.h"
#include "GRA_simpleText.h"
#include "GRA_borlandMonitor.h"
#include "UsefulFunctions.h"
#include "TextPopup.h"

GRA_drawableText::GRA_drawableText( ExString const &inputString, bool graphUnits )
    : GRA_drawableObject("DRAWABLETEXT"), inputString_(inputString), graphUnits_(graphUnits),
      subsupFactor_(0.7), popup_(false)
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *textCharacteristics = gw->GetTextCharacteristics();
  x_ = static_cast<GRA_distanceCharacteristic*>(textCharacteristics->Get("XLOCATION"))->GetAsWorld();
  y_ = static_cast<GRA_distanceCharacteristic*>(textCharacteristics->Get("YLOCATION"))->GetAsWorld();
  if( graphUnits_ )
  {
    double xg=x_, yg=y_;
    ExGlobals::GetGraphWindow()->WorldToGraph( xg, yg, x_, y_ );
  }
  angle_ = static_cast<GRA_angleCharacteristic*>(textCharacteristics->Get("ANGLE"))->Get();
  alignment_ = static_cast<GRA_intCharacteristic*>(textCharacteristics->Get("ALIGNMENT"))->Get();
  font_ = static_cast<GRA_fontCharacteristic*>(textCharacteristics->Get("FONT"))->Get();
  color_ = static_cast<GRA_colorCharacteristic*>(textCharacteristics->Get("COLOR"))->Get();
  height_ = static_cast<GRA_sizeCharacteristic*>(textCharacteristics->Get("HEIGHT"))->GetAsWorld();
}

GRA_drawableText::~GRA_drawableText()
{
  DeleteStuff();
  if( popup_ )TextPopupForm->Disconnect();
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
             << "<string><![CDATA[" << dt.inputString_ << "]]></string>\n</drawabletext>\n";
}

double GRA_drawableText::GetWidth() const
{
  std::vector<GRA_simpleText*>::const_iterator end = strings_.end();
  double width = 0.0;
  for( std::vector<GRA_simpleText*>::const_iterator i=strings_.begin(); i!=end; ++i )
  {
    int xlo, ylo, xhi, yhi;
    (*i)->GetBoundary( xlo, ylo, xhi, yhi );
    GRA_borlandMonitor *bm = static_cast<GRA_borlandMonitor*>(ExGlobals::GetScreenOutput());
    double xlow, ylow, xhiw, yhiw;
    bm->OutputTypeToWorld( xlo, ylo, xlow, ylow );
    bm->OutputTypeToWorld( xhi, yhi, xhiw, yhiw );
    width += xhiw-xlow;
  }
  return width;
}

bool GRA_drawableText::Inside( double x, double y )
{
  std::vector<double> xp(4,0.0), yp(4,0.0);
  std::vector<GRA_simpleText*>::const_iterator end = strings_.end();
  for( std::vector<GRA_simpleText*>::const_iterator i=strings_.begin(); i!=end; ++i )
  {
    GRA_borlandMonitor *bm = static_cast<GRA_borlandMonitor*>(ExGlobals::GetScreenOutput());
    int xlo, ylo, xhi, yhi;
    (*i)->GetBoundary( xlo, ylo, xhi, yhi );
    bm->OutputTypeToWorld( xlo, ylo, xp[0], yp[0] );
    bm->OutputTypeToWorld( xhi, yhi, xp[2], yp[2] );
    xp[1] = xp[2];
    yp[1] = yp[0];
    xp[3] = xp[0];
    yp[3] = yp[2];
    if( UsefulFunctions::InsidePolygon(x,y,xp,yp) )return true;
  }
  return false;
}

void GRA_drawableText::Parse()
{
  textVec().swap( strings_ );
  //
  // multiple adjacent commands can entered as <comand><command>  or  <command,command>
  //
  double xShift=0.0, yShift=0.0;
  bool italics = false;
  bool bold = false;
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
  ExString text, command;
  int currentState, newState=0;
  std::size_t length = inputString_.size();
  for( std::size_t i=0; i<length; ++i )
  {
    currentState = newState;
    newState = stateTable[currentState][classes[toascii(inputString_[i])]];
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
          GRA_simpleText *tmp = new GRA_simpleText(text,height,color_,font_,xShift,yShift);
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
          DetermineCommand( command, height, xShift, yShift, italics,
                            bold, upCounter, downCounter );
        }
        catch ( runtime_error &e )
        {
          throw EGraphicsError( e.what() );
        }
        command.erase();
        break;
      }
      case -1:
      {
        ExString s("invalid < found at location ");
        s += ExString(i+1) + " in " + inputString_;
        throw EGraphicsError( s );
      }
      case -2:
      {
        ExString s("invalid > found at location ");
        s += ExString(i+1) + " in " + inputString_;
        throw EGraphicsError( s );
      }
      case -3:
      {
        ExString s("invalid comma found at location ");
        s += ExString(i+1) + " in " + inputString_;
        throw EGraphicsError( s );
      }
    }
  }
  if( !text.empty() )
  {
    GRA_simpleText *tmp = new GRA_simpleText(text,height,color_,font_,xShift,yShift);
    strings_.push_back( tmp );
  }
}

void GRA_drawableText::DetermineCommand( ExString &command,
                                         double &height, double &xShift, double &yShift,
                                         bool &italics, bool &bold,
                                         int &upCounter, int &downCounter )
{
  char *fontName;
  char c = Special( command, fontName );
  if( c )
  {
    GRA_simpleText *tmp = new GRA_simpleText(c,height,color_,fontName,xShift,yShift);
    strings_.push_back( tmp );
    delete [] fontName;
    xShift = 0.0;
    yShift = 0.0;
    return;
  }
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
  double const xlowerw = static_cast<GRA_distanceCharacteristic*>(general->Get("XLOWERWINDOW"))->GetAsWorld();
  double const xupperw = static_cast<GRA_distanceCharacteristic*>(general->Get("XUPPERWINDOW"))->GetAsWorld();
  double const ylowerw = static_cast<GRA_distanceCharacteristic*>(general->Get("YLOWERWINDOW"))->GetAsWorld();
  double const yupperw = static_cast<GRA_distanceCharacteristic*>(general->Get("YUPPERWINDOW"))->GetAsWorld();
  double const dx = xupperw-xlowerw;
  double const dy = yupperw-ylowerw;
  //
  std::size_t len = command.size();
  command.ToUpper();  // convert to upper case
  //
  if( command[0] == 'F' ) // set the font
  {
    font_->SetName( command.substr(1,len-1) );
  }
  else if( command[0] == 'H' ) // set the height
  {
    bool percent = (command[len-1]=='%');
    std::size_t end;
    percent ? end=len-2 : end=len-1;
    try
    {
      height = command.substr(1,end).ToDouble();
    }
    catch ( runtime_error &e )
    {
      throw;
    }
    if( percent )height = std::min( dy, std::max(0.0,height*dy/100.) );
  }
  else if( command[0] == 'Z' ) // include a horizontal space
  {
    bool percent = (command[len-1]=='%');
    std::size_t end;
    percent ? end=len-2 : end=len-1;
    try
    {
      xShift = command.substr(1,end).ToDouble();
    }
    catch ( runtime_error &e )
    {
      throw;
    }
    if( percent )xShift = xShift*dx/100.;
  }
  else if( command[0] == 'V' ) // include a vertical space
  {
    bool percent = (command[len-1]=='%');
    std::size_t end;
    percent ? end=len-2 : end=len-1;
    try
    {
      yShift = command.substr(1,end).ToDouble();
    }
    catch ( runtime_error &e )
    {
      throw;
    }
    if( percent )yShift = yShift*dy/100.;
  }
  else if( command[0]=='^' ) // super-script flag
  {
    if( downCounter > 0 )
    {
      --downCounter;
      //yShift += height;
      //height /= subsupFactor_;
      height /= subsupFactor_;
      yShift += 0.35*height;
    }
    else
    {
      ++upCounter;
      //height *= subsupFactor_;
      //yShift += height;
      yShift += 0.5*height;
      height *= subsupFactor_;
    }
  }
  else if( command[0]=='_' ) // sub-script flag
  {
    if( upCounter > 0 )
    {
      --upCounter;
      //yShift -= height;
      //height /= subsupFactor_;
      height /= subsupFactor_;
      yShift -= 0.5*height;
    }
    else
    {
      ++downCounter;
      //height *= subsupFactor_;
      //yShift -= height;
      yShift -= 0.35*height;
      height *= subsupFactor_;
    }
  }
  else if( command[0] == 'C' ) // colour
  {
    GRA_color *c = GRA_colorControl::GetColor( command.substr(1,len-1).ToInt() );
    if( !c )throw runtime_error("invalid color");
    color_ = c;
  }
  else
  {
    ExString s("invalid text formatting command: ");
    s += command;
    throw runtime_error( s.c_str() );
  }
}

char GRA_drawableText::Special( ExString const &command, char *&fontName )
{
  ExString s( command.UpperCase() );
  //
  bool upperCase = (const_cast<ExString&>(command)[0]==s[0]);
  //
  struct specialCharacter
  {
    char *name;  // special character name
    char uName;  // upper case character
    char lName;  // lower case character
    char *fName; // font name
  };
  std::size_t const nNames = 93;
  specialCharacter spc[] =
  {
      {"ALPHA",65,97,"Symbol"},        {"BETA",66,98,"Symbol"},            {"GAMMA",71,103,"Symbol"},
      {"DELTA",68,100,"Symbol"},       {"EPSILON",69,206,"Symbol"},        {"ZETA",90,122,"Symbol"},
      {"ETA",72,104,"Symbol"},         {"THETA",81,113,"Symbol"},          {"IOTA",73,105,"Symbol"},
      {"KAPPA",75,107,"Symbol"},       {"LAMBDA",76,108,"Symbol"},         {"MU",77,109,"Symbol"},
      {"NU",78,110,"Symbol"},          {"XI",88,120,"Symbol"},             {"OMICRON",79,111,"Symbol"},
      {"PI",80,112,"Symbol"},          {"RHO",82,114,"Symbol"},            {"SIGMA",83,115,"Symbol"},
      {"TAU",84,116,"Symbol"},         {"UPSILON",161,117,"Symbol"},       {"PHI",70,102,"Symbol"},
      {"CHI",67,99,"Symbol"},          {"PSI",89,121,"Symbol"},            {"OMEGA",87,119,"Symbol"},
      {"VARTHETA",74,74,"Symbol"},     {"VARPHI",106,106,"Symbol"},        {"VAREPSILON",101,101,"Symbol"},
      {"ALEPH",192,192,"Symbol"},      {"NABLA",209,209,"Symbol"},         {"PARTIAL",182,182,"Symbol"},
      {"LEFTARROW",220,172,"Symbol"},  {"UPARROW",221,173,"Symbol"},       {"DOWNARROW",223,175,"Symbol"},
      {"RIGHTARROW",222,174,"Symbol"}, {"PERP",94,94,"Symbol"},            {"MID",189,189,"Symbol"},
      {"BULLET",183,183,"Symbol"},     {"SUM",229,229,"Symbol"},           {"PROD",213,213,"Symbol"},
      {"INT",242,242,"Symbol"},        {"SURD",214,214,"Symbol"},          {"PLUS",43,43,"Symbol"},
      {"MINUS",45,45,"Symbol"},        {"PM",177,177,"Symbol"},            {"TIMES",180,180,"Symbol"},
      {"DIV",184,184,"Symbol"},        {"OPLUS",197,197,"Symbol"},         {"OTIMES",196,196,"Symbol"},
      {"CAP",199,199,"Symbol"},        {"SUBSET",204,204,"Symbol"},        {"CUP",200,200,"Symbol"},
      {"SUPSET",201,201,"Symbol"},     {"DEGREE",176,176,"Symbol"},        {"LANGLE",225,225,"Symbol"},
      {"RANGLE",241,241,"Symbol"},     {"NEG",216,216,"Symbol"},           {"THEREFORE",92,92,"Symbol"},
      {"ANGLE",208,208,"Symbol"},      {"VEE",218,218,"Symbol"},           {"WEDGE",217,217,"Symbol"},
      {"CDOT",215,215,"Symbol"},       {"INFTY",165,165,"Symbol"},         {"IN",206,206,"Symbol"},
      {"NI",207,207,"Symbol"},         {"PROPTO",181,181,"Symbol"},        {"EXISTS",36,36,"Symbol"},
      {"FORALL",34,34,"Symbol"},       {"NEQ",185,185,"Symbol"},           {"EQUIV",186,186,"Symbol"},
      {"APPROX",187,187,"Symbol"},     {"SIM",126,126,"Arial"},            {"LT",60,60,"Symbol"},
      {"GT",62,62,"Symbol"},           {"LEQ",163,163,"Symbol"},           {"GEQ",179,179,"Symbol"},
      {"VARPI",118,118,"Symbol"},      {"CLUBSUIT",167,167,"Symbol"},      {"DIAMONDSUIT",168,168,"Symbol"},
      {"HEARTSUIT",169,169,"Symbol"},  {"SPADESUIT",170,170,"Symbol"},     {"SUBSETEQ",205,205,"Symbol"},
      {"SUPSETEQ",202,202,"Symbol"},   {"LDOTS",188,188,"Symbol"},         {"LEFTRIGHTARROW",219,171,"Symbol"},
      {"EMPTYSET",198,198,"Symbol"},   {"WP",195,195,"Symbol"},            {"RE",194,194,"Symbol"},
      {"IM",193,193,"Symbol"},         {"HOOKLEFTARROW",191,191,"Symbol"}, {"COPYRIGHT",227,227,"Symbol"},
      {"REGISTERED",226,226,"Symbol"}, {"TM",228,228,"Symbol"},            {"DIAMOND",224,224,"Symbol"} 
  };
  for( std::size_t i=0; i<nNames; ++i )
  {
    if( s == spc[i].name )
    {
      char c;
      upperCase ? c=spc[i].uName : c=spc[i].lName;
      fontName = new char [ strlen(spc[i].fName)+1 ];
      strcpy(fontName,spc[i].fName);
      return c;
    }
  }
  return 0;
}

// end of file
