#include <math.h>
#include <iostream>
#include <ostream>
#include <iomanip>
#include <locale>
#include <codecvt>

#include "EGraphicsError.h"
#include "EVariableError.h"
#include "MyColor.h"
#include "LowLevelGraphics.h"
#include "Utilities.h"
#include "GraphWindow.h"
#include "Extrema.h"
#include "ExFont.h"

#include "TextBuffer.h"

TextBuffer::TextBuffer()
{ font_ = NULL; }

TextBuffer::TextBuffer( std::string text, double height, double angle,
                        double x, double y, int textAlign, ExFont *font )
   : text_(text), height_(height), angle_(angle),
     x_(x), y_(y), textAlign_(textAlign)
{
  LowLevelGraphics *llg = LowLevelGraphics::GetPtr();
  xMin_ = llg->GetXminW();
  xMax_ = llg->GetXmaxW();
  yMin_ = llg->GetYminW();
  yMax_ = llg->GetYmaxW();
  heightP_ = 0.0;
  font_ = new ExFont(*font);
  addedViaTextCommand_ = false;
  graphUnits_ = false;
  cos_ = cos( angle_*M_PI/180.0 );
  sin_ = sin( angle_*M_PI/180.0 );
  if( fabs(sin_) < 0.000001 )sin_ = 0.0;
  if( fabs(cos_) < 0.000001 )cos_ = 0.0;
  subSupFactor_ = 0.667;
  erase_ = false;
#ifdef DEBUG
fprintf(stderr, "TextBuffer::TextBuffer x=%f, y=%f\n",x,y);
#endif
}

TextBuffer::~TextBuffer()
{
  if( font_ != NULL )delete font_;
}

void TextBuffer::CopyStuff( TextBuffer const &rhs )
{
  xMin_ = rhs.xMin_;
  xMax_ = rhs.xMax_;
  yMin_ = rhs.yMin_;
  yMax_ = rhs.yMax_;
  text_ = rhs.text_;
  textAlign_ = rhs.textAlign_;
  height_ = rhs.height_;
  heightP_ = rhs.heightP_;
  angle_ = rhs.angle_;
  cos_ = rhs.cos_;
  sin_ = rhs.sin_;
  x_ = rhs.x_;
  y_ = rhs.y_;
  xP_ = rhs.xP_;
  yP_ = rhs.yP_;
  xG_ = rhs.xG_;
  yG_ = rhs.yG_;

#ifdef DEBUG
fprintf(stderr, "TextBuffer CopyStuff x_ = %f and y_ = %f\n", x_, y_);
#endif

  if( font_ != NULL )delete font_;
  font_ = new ExFont(*rhs.font_);

  addedViaTextCommand_ = rhs.addedViaTextCommand_;
  graphUnits_ = rhs.graphUnits_;
  subSupFactor_ = rhs.subSupFactor_;
  erase_ = rhs.erase_;
}

TextBuffer::TextBuffer( TextBuffer const &rhs )
{
  CopyStuff( rhs );
}

TextBuffer &TextBuffer::operator=( TextBuffer const &rhs )
{
  if( this != &rhs )CopyStuff( rhs );
  return *this;
}

bool TextBuffer::operator==( TextBuffer const &rhs ) const
{
  if( text_ != rhs.text_ )return false;
  if( xMin_ != rhs.xMin_ )return false;
  if( xMax_ != rhs.xMax_ )return false;
  if( yMin_ != rhs.yMin_ )return false;
  if( yMax_ != rhs.yMax_ )return false;
  if( x_ != rhs.x_ )return false;
  if( y_ != rhs.y_ )return false;
  if( height_ != rhs.height_ )return false;
  if( angle_ != rhs.angle_ )return false;
  if( textAlign_ != rhs.textAlign_ )return false;
  if( font_ != rhs.font_ )return false;
  if( addedViaTextCommand_ != rhs.addedViaTextCommand_ )return false;
  if( graphUnits_ != rhs.graphUnits_ )return false;
  return true;
}

void TextBuffer::SetText( std::string &s )
{ text_ = s; }

std::string TextBuffer::GetText() const
{ return text_; }

void TextBuffer::SetHeight( double h )
{ height_ = h; }

double TextBuffer::GetHeight() const
{ return height_; }

void TextBuffer::SetHeightP( double h )
{ heightP_ = h; }

double TextBuffer::GetHeightP() const
{ return heightP_; }

void TextBuffer::SetAngle( double a )
{
  angle_ = a;
  cos_ = cos( angle_*M_PI/180.0 );
  sin_ = sin( angle_*M_PI/180.0 );
  if( fabs(sin_) < 0.000001 )sin_ = 0.0;
  if( fabs(cos_) < 0.000001 )cos_ = 0.0;
}

double TextBuffer::GetAngle() const
{ return angle_; }

void TextBuffer::SetX( double h )
{ x_ = h; 
#ifdef DEBUG
fprintf(stderr, "TextBuffer SetX x= %f\n", x_);
#endif
}
double TextBuffer::GetX() const
{ return x_; 
#ifdef DEBUG
fprintf(stderr, "TextBuffer GetX x=%f\n", x_);
#endif
}

void TextBuffer::SetXP( double h )
{ xP_ = h; }

double TextBuffer::GetXP() const
{ return xP_; }

void TextBuffer::SetXG( double h )
{ xG_ = h; }

double TextBuffer::GetXG() const
{ return xG_; }

void TextBuffer::SetY( double h )
{ y_ = h;
#ifdef DEBUG
fprintf(stderr, "TextBuffer SetY y=%f\n", y_);
#endif 
}

double TextBuffer::GetY() const
{ return y_;
#ifdef DEBUG
fprintf(stderr, "TextBuffer GetY y=%f\n", y_);
#endif
 }

void TextBuffer::SetYP( double h )
{ yP_ = h; }

double TextBuffer::GetYP() const
{ return yP_; }

void TextBuffer::SetYG( double h )
{ yG_ = h; }

double TextBuffer::GetYG() const
{ return yG_; }

int TextBuffer::GetTextAlign() const
{ return textAlign_; }

void TextBuffer::SetTextAlign( int j )
{ textAlign_ = j; }

ExFont *TextBuffer::GetFont() const
{ return font_; }

std::string TextBuffer::GetFontName() const
{ return font_->GetName(); }

void TextBuffer::SetFontName( std::string &s )
{ font_->SetName( s ); }

bool TextBuffer::AddedViaTextCommand() const
{ return addedViaTextCommand_; }

void TextBuffer::SetViaTextCommand()
{ addedViaTextCommand_ = true; }

bool TextBuffer::GetGraphUnits() const
{ return graphUnits_; }

void TextBuffer::GetRGB( int &r, int &g, int &b ) const
{
  MyColor c( font_->GetColor() );
  c.GetRGB(r,g,b);
}

void TextBuffer::SetRGB( int r, int g, int b )
{
  MyColor c(r,g,b);
  font_->SetColor( c.GetCode() );
}

void TextBuffer::SetColor( MyColor *mc )
{
  font_->SetColor( mc->GetCode() );
}

void TextBuffer::SetGraphUnits( bool b )
{
  graphUnits_ = b;
  if( graphUnits_ )Utilities::GetPtr()->WorldToGraph( x_, y_, xG_, yG_ );
}

void TextBuffer::DoCommand( std::string &command, ExFont *font,
                            double &x, double &y, double &height, double const angle,
                            bool &italics, bool &bold,
                            double const cosx, double const sinx,
                            int &upCounter, int &downCounter )
{
  Utilities *util = Utilities::GetPtr();
  GraphWindow *gw = GetGraphWindow();
  double const xFactor = (gw->GetXUpper()-gw->GetXLower())/100.;
  double const yFactor = (gw->GetYUpper()-gw->GetYLower())/100.;
  //
  size_t len = command.size();
  //util->uprCase( command );
  //
  if( std::toupper(command[0]) == 'F' ) // set the font
  {
    font->SetName( command.substr(1,len-1) );
  }
  else if( std::toupper(command[0]) == 'H' ) // set the height
  {
    double tmp;
    if( command[len-1] == '%' )
    {
      util->char2double( command.substr(1,len-2), tmp );
      height = tmp*yFactor;
    }
    else
    {
      util->char2double( command.substr(1,len-1), tmp );
      height = tmp;
    }
  }
  else if( std::toupper(command[0]) == 'Z' ) // include a horizontal space
  {
    double horizSpace;
    if( command[len-1] == '%' )
    {
      util->char2double( command.substr(1,len-2), horizSpace );
      horizSpace *= xFactor;
    }
    else
    {
      util->char2double( command.substr(1,len-1), horizSpace );
    }
    x += cosx*horizSpace;
    y += sinx*horizSpace;
  }
  else if( std::toupper(command[0]) == 'V' ) // include a vertical space
  {
    double vertSpace;
    if( command[len-1] == '%' )
    {
      util->char2double( command.substr(1,len-2), vertSpace );
      vertSpace *= yFactor;
    }
    else
    {
      util->char2double( command.substr(1,len-1), vertSpace );
    }
    x -= sinx*vertSpace;
    y += cosx*vertSpace;
  }
  else if( command[0]=='^' ) // super-script flag
  {
    if( downCounter > 0 )
    {
      --downCounter;
      x -= sinx*height;
      y += cosx*height;
      height /= subSupFactor_;
    }
    else
    {
      ++upCounter;
      height *= subSupFactor_;
      x -= sinx*height;
      y += cosx*height;
    }
  }
  else if( command[0]=='_' ) // sub-script flag
  {
    if( upCounter > 0 )
    {
      --upCounter;
      x += sinx*height;
      y -= cosx*height;
      height /= subSupFactor_;
    }
    else
    {
      ++downCounter;
      height *= subSupFactor_;
      x += sinx*height;
      y -= cosx*height;
    }
  }
  else if( std::toupper(command[0]) == 'C' ) // colour
  {
    if( !erase_ )
    {
      std::string colorName( command.substr(1,len-1) );
      MyColor c( colorName );
      font->SetColor( c.GetCode() );
    }
  }
}

bool TextBuffer::Special( std::string &command,ExFont *font )
{
  std::string s( command );
  Utilities::GetPtr()->uprCase( s );
  bool upperCase = (command[0]==s[0]);
  //
  char const *names[] = {
   "ALPHA","BETA","GAMMA","DELTA","EPSILON","ZETA","ETA","THETA","IOTA","KAPPA",
   "LAMBDA","MU","NU","XI","OMICRON","PI","RHO","SIGMA","TAU","UPSILON",
   "PHI","CHI","PSI","OMEGA","VARTHETA","VARPHI","VAREPSILON","ALEPH","NABLA","PARTIAL",
   "LEFTARROW","UPARROW","DOWNARROW","RIGHTARROW","PERP","MID","BULLET","SUM","PROD","INT",
   "SURD","PLUS","MINUS","PM","TIMES","DIV","OPLUS","OTIMES","CAP","SUBSET",
   "CUP","SUPSET","DEGREE","LANGLE","RANGLE","NEG","THEREFORE","ANGLE","VEE","WEDGE",
   "CDOT","INFTY","IN","NI","PROPTO","EXISTS","FORALL","NEQ","EQUIV","APPROX",
   "SIM","LT","GT","LEQ","GEQ","VARPI","CLUBSUIT","DIAMONDSUIT","HEARTSUIT","SPADESUIT",
   "SUBSETEQ","SUPSETEQ","LDOTS","LEFTRIGHTARROW","EMPTYSET","WP","RE","IM","HOOKLEFTARROW","COPYRIGHT",
   "REGISTERED","TM","DIAMOND" };
//  char const uName[] = {
//    65,  66,  71,  68,  69,  90,  72,  81,  73,  75,
//    76,  77,  78,  88,  79,  80,  82,  83,  84, 161,
//    70,  67,  89,  87,  74, 106, 101, 192, 209, 182,
//   220, 221, 223, 222,  94, 189, 183, 229, 213, 242,
//   214,  43,  45, 177, 180, 184, 197, 196, 199, 204,
//   200, 201, 176, 225, 241, 216,  92, 208, 218, 217,
//   215, 165, 206, 207, 181,  36,  34, 185, 186, 187,
//   126,  60,  62, 163, 179, 118, 167, 168, 169, 170,
//   205, 202, 188, 219, 198, 195, 194, 193, 191, 227,
/    226, 228, 224 };
//  char const lName[] = {
//    97,  98, 103, 100, 206, 122, 104, 113, 105, 107,
//   108, 109, 110, 120, 111, 112, 114, 115, 116, 117,
//   102,  99, 121, 119,  74, 106, 101, 192, 209, 182,
//   172, 173, 175, 174,  94, 189, 183, 229, 213, 242,
//   214,  43,  45, 177, 180, 184, 197, 196, 199, 204,
//   200, 201, 176, 225, 241, 216,  92, 208, 218, 217,
//   215, 165, 206, 207, 181,  36,  34, 185, 186, 187,
//   126,  60,  62, 163, 179, 118, 167, 168, 169, 170,
//   205, 202, 188, 171, 198, 195, 194, 193, 191, 227,
//   226, 228, 224 };
  const wchar_t* uName[] = {
   L"\u0391",L"\u0392",L"\u0393",L"\u0394",L"\u0395",L"\u0396",L"\u0397",L"\u0398",L"\u0399",L"\u039A",
   L"\u039B",L"\u039C",L"\u039D",L"\u039E",L"\u039F",L"\u03A0",L"\u03A1",L"\u03A3",L"\u03A4",L"\u03A5",
   L"\u03A6",L"\u03A7",L"\u03A8",L"\u03A9",L"\u03D1",L"\u03D5",L"\u03F5",L"\u2135",L"\u2207",L"\u2202",
   L"\u2190",L"\u2191",L"\u2193",L"\u2192",L"\u27C2",L"\u2223",L"\u2022",L"\u2211",L"\u220F",L"\u222B",
   L"\u221A",L"\u002B",L"\u2212",L"\u00B1",L"\u00D7",L"\u00F7",L"\u2295",L"\u2297",L"\u2229",L"\u2282",
   L"\u222A",L"\u2283",L"\u02DA",L"\u27E8",L"\u27E9",L"\u002D",L"\u2234",L"\u2220",L"\u2228",L"\u2227",
   L"\u02D9",L"\u221E",L"\u2208",L"\u220B",L"\u221D",L"\u2203",L"\u2200",L"\u2260",L"\u003D",L"\u2248",
   L"\u007E",L"\u003C",L"\u003E",L"\u2264",L"\u2265",L"\u03D6",L"\u2663",L"\u2666",L"\u2665",L"\u2660",
   L"\u2286",L"\u2287",L"\u2026",L"\u21D4",L"\u2205",L"\u2118",L"\u211C",L"\u2111",L"\u21B2",L"\u00A9",
   L"\u00AE",L"\u2122",L"\u25CA" };
  const wchar_t* lName[] = {
   L"\u03B1",L"\u03B2",L"\u03B3",L"\u03B4",L"\u03B5",L"\u03B6",L"\u03B7",L"\u03B8",L"\u03B9",L"\u03BA",
   L"\u03BB",L"\u03BC",L"\u03BD",L"\u03BE",L"\u03BF",L"\u03C0",L"\u03C1",L"\u03C3",L"\u03C4",L"\u03C5",
   L"\u03C6",L"\u03C7",L"\u03C8",L"\u03C9",L"\u03D1",L"\u03D5",L"\u03F5",L"\u2135",L"\u2207",L"\u2202",
   L"\u2190",L"\u2191",L"\u2193",L"\u2192",L"\u27C2",L"\u2223",L"\u2022",L"\u2211",L"\u220F",L"\u222B",
   L"\u221A",L"\u002B",L"\u2212",L"\u00B1",L"\u00D7",L"\u00F7",L"\u2295",L"\u2297",L"\u2229",L"\u2282",
   L"\u222A",L"\u2283",L"\u02DA",L"\u27E8",L"\u27E9",L"\u002D",L"\u2234",L"\u2220",L"\u2228",L"\u2227",
   L"\u02D9",L"\u221E",L"\u2208",L"\u220B",L"\u221D",L"\u2203",L"\u2200",L"\u2260",L"\u003D",L"\u2248",
   L"\u007E",L"\u003C",L"\u003E",L"\u2264",L"\u2265",L"\u03D6",L"\u2663",L"\u2666",L"\u2665",L"\u2660",
   L"\u2286",L"\u2287",L"\u2026",L"\u2194",L"\u2205",L"\u2118",L"\u211C",L"\u2111",L"\u21B2",L"\u00A9",
   L"\u00AE",L"\u2122",L"\u25CA" };
//  const char *fName[] = {
//   "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol",
//   "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol",
//   "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol",
//   "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol",
//   "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol",
//   "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol",
//   "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol",
//   "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "arial",  "symbol",
//   "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol",
//   "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol", "symbol",
//   "symbol", "symbol", "symbol"};
  size_t const nNames = sizeof(uName)/sizeof(uName[0]);
  //
  for( size_t i=0; i<nNames; ++i )
  {
    if( s == std::string(names[i]) )
    {
      // no need to set font to "symbol", in UTF-8 all fonts have special symbols
      //font->SetName( fName[i] );
      //upperCase ? command=std::string(1,uName[i]) : command=std::string(1,lName[i]);
      // convert our wchar_t (all ours are 16-bit, but may be implemented as e.g. 32-bit wchar) to bytes
      std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
      std::string u8str;
      upperCase ? u8str = convert.to_bytes(uName[i]) : u8str = convert.to_bytes(lName[i]);
      command=std::string(u8str);
#ifdef DEBUG
fprintf(stderr," command=\"%s\"\n",command.c_str());
#endif
      return true;
    }
  }
  return false;
}

void TextBuffer::DoCommand2( std::string &command, ExFont *font, double &height,
                             bool &italics, bool &bold, double &plen,
                             int &upCounter, int &downCounter )
{
  Utilities *util = Utilities::GetPtr();
  GraphWindow *gw = GetGraphWindow();
  //
  size_t len = command.size();
  //util->uprCase( command );
  //
  if( std::toupper(command[0]) == 'F' ) // set the font
  {
    font->SetName( command.substr(1,len-1) );
  }
  else if( std::toupper(command[0]) == 'H' ) // set the height
  {
    double tmp;
    if( command[len-1] == '%' )
    {
      util->char2double( command.substr(1,len-2), tmp );
      height = tmp*(gw->GetYUpper()-gw->GetYLower())/100.;
    }
    else
    {
      util->char2double( command.substr(1,len-1), tmp );
      height = tmp;
    }
  }
  else if( std::toupper(command[0]) == 'Z' ) // include a horizontal space
  {
    double horizSpace;
    if( command[len-1] == '%' )
    {
      util->char2double( command.substr(1,len-2), horizSpace );
      horizSpace *= (gw->GetXUpper()-gw->GetXLower())/100.;
    }
    else
    {
      util->char2double( command.substr(1,len-1), horizSpace );
    }
    plen += horizSpace;
  }
  else if( std::toupper(command[0]) == 'V' ) // include a vertical space
  {
    // do nothing
  }
  else if( command[0]=='^' ) // super-script flag
  {
    if( downCounter > 0 )
    {
      --downCounter;
      height_ /= subSupFactor_;
    }
    else
    {
      ++upCounter;
      height *= subSupFactor_;
    }
  }
  else if( command[0]=='_' ) // sub-script flag
  {
    if( upCounter > 0 )
    {
      --upCounter;
      height /= subSupFactor_;
    }
    else
    {
      ++downCounter;
      height *= subSupFactor_;
    }
  }
  else if( std::toupper(command[0]) == 'C' ) // colour
  {
    // do nothing
  }
  else
  {
    std::string s( "incorrect text-format command:  <" );
    s += command + std::string(">");
    throw EGraphicsError( s.c_str() );
  }
}

double TextBuffer::GetTextWidth( ExFont *font )
{
  if( text_.empty() )return 0.0;
  LowLevelGraphics *llg = LowLevelGraphics::GetPtr();
  //
  int const classes[128] = {
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
  int const stateTable[4][3] = {
   //  <   >  othr
     { 1,  0,  0 },
     {-1, -2,  2 },
     {-1,  3,  2 },
     { 1, -2,  0 } };
  double height = height_;
  std::string text, command;
  bool italics = false;
  bool bold = false;
  double plen = 0.0;
  int upCounter=0, downCounter=0;
  int currentState, newState=0;
  size_t length = text_.size();
  for( size_t i=0; i<length; ++i )
  {
    currentState = newState;
    int iascii = toascii( text_[i] );
    newState = stateTable[currentState][classes[iascii]];
    if( newState == 0 ) // just add to text
    {
      text.append( 1, text_[i] );
    }
    else if( newState == 1 ) // found <, start of command
    {
      if( !text.empty() )
      {
        plen += llg->GetTextWidth( text, height, font );
        text.erase();
      }
      command.erase();
    }
    else if( newState == 2 ) // add to command
    {
      command.append( 1, text_[i] );
    }
    else if( newState == 3 ) // found >, end of command
    {
      ExFont *f = new ExFont(*font);
      if( Special(command,font) )
      {
        plen += llg->GetTextWidth( command, height, font );
        *font = *f;
      }
      else
      {
        DoCommand2( command, font, height, italics, bold, plen, upCounter, downCounter );
      }
      delete f;
    }
    else if( newState == -1 )
    {
      char c[200];
      sprintf( c, "invalid < found at location %d in %s", i+1, text_.c_str() );
      throw EGraphicsError( c );
    }
    else if( newState == -2 )
    {
      char c[200];
      sprintf( c, "invalid > found at location %d in %s", i+1, text_.c_str() );
      throw EGraphicsError( c );
    }
  }
  if( !text.empty() )plen += llg->GetTextWidth( text, height, font );
  return plen;
}

void TextBuffer::SetUp( double &x, double &y, double &angle, double &height,
                        double &cosx, double &sinx, ExFont *font )
{
  if( graphUnits_ )Utilities::GetPtr()->GraphToWorld( xG_, yG_, x_, y_ );
  LowLevelGraphics *llg = LowLevelGraphics::GetPtr();
  x = (x_-xMin_)/(xMax_-xMin_)*(llg->GetXmaxW()-llg->GetXminW())+llg->GetXminW();
  y = (y_-yMin_)/(yMax_-yMin_)*(llg->GetYmaxW()-llg->GetYminW())+llg->GetYminW();
  //
  angle = angle_ - static_cast<int>(angle_/360.0)*360.0;
  if( angle < 0.0 )angle += 360.0;
  height = height_;
  //
  double plen;
  try
  {
    plen = GetTextWidth( font );
  }
  catch (EGraphicsError &e)
  {
    throw;
  }
  //
  cosx = cos_;
  sinx = sin_;
  //
  switch (textAlign_)
  {
   case 1:               // lower left
     break;
   case 2:               // lower center
     x -= 0.5*plen*cosx;
     y -= 0.5*plen*sinx;
     break;
   case 3:               // lower right
     x -= plen*cosx;
     y -= plen*sinx;
     break;
   case 4:               // center left
     y -= 0.5*height;
     break;
   case 5:               // center center
     x -= 0.5*plen*cosx;
     y -= 0.5*(plen*sinx + height);
     break;
   case 6:               // center right
     x -= plen*cosx;
     y -= plen*sinx + 0.5*height;
     break;
   case 7:               // upper left
     y -= height;
     break;
   case 8:               // upper center
     x -= 0.5*plen*cosx;
     y -= 0.5*plen*sinx + height;
     break;
   case 9:               // upper right
     x -= plen*cosx;
     y -= plen*sinx + height;
     break;
   case 10:               // lower left at 90deg
     x -= height;
     y -= height;
     angle = 90.0;
     cosx = 0.0;
     sinx = 1.0;
     break;
   case 11:              // lower left at 270deg
     x += height;
     y -= height;
     angle = 270.0;
     cosx = 0.0;
     sinx = -1.0;
     break;
   case 12:              // lower center at 90deg
     x -= height;
     y -= 0.5*plen;
     angle = 90.0;
     cosx = 0.0;
     sinx = 1.0;
     break;
  }
}

void TextBuffer::Draw( std::ofstream &outFile )
{
  if( text_.empty() )return;
  LowLevelGraphics *llg = LowLevelGraphics::GetPtr();
  //
  double x, y, angle, height, cosx, sinx;
  ExFont *font= new ExFont( *font_ );
  try
  {
    SetUp( x, y, angle, height, cosx, sinx, font );
  }
  catch (EGraphicsError &e)
  {
    delete font;
    throw;
  }
  int const classes[128] = {
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
  int const stateTable[4][3] = {
   //  <   >  othr
     { 1,  0,  0 },
     {-1, -2,  2 },
     {-1,  3,  2 },
     { 1, -2,  0 } };
  bool italics = false;
  bool bold = false;
  std::string text, command;
  int upCounter=0, downCounter=0;
  int currentState, newState=0;
  size_t length = text_.size();
  for( size_t i=0; i<length; ++i )
  {
    currentState = newState;
    int iascii = toascii( text_[i] );
    newState = stateTable[currentState][classes[iascii]];
    if( newState == 0 ) // just add to text
    {
      text.append( 1, text_[i] );
    }
    else if( newState == 1 ) // found <, start of command
    {
      if( !text.empty() )
      {
        Draw2( text, height, angle, x, y, font, outFile );
        double tmp = llg->GetTextWidth( text, height, font );
        x += tmp*cosx;
        y += tmp*sinx;
        text.erase();
      }
      command.erase();
    }
    else if( newState == 2 ) // add to command
    {
      command.append( 1, text_[i] );
    }
    else if( newState == 3 ) // found >, end of command
    {
      ExFont *f = new ExFont(*font);
      if( Special(command,font) )
      {
        Draw2( command, height, angle, x, y, font, outFile );
        double tmp = llg->GetTextWidth( command, height, font );
        x += tmp*cosx;
        y += tmp*sinx;
        *font = *f;
      }
      else
      {
        DoCommand( command, font, x, y, height, angle, italics, bold,
                   cosx, sinx, upCounter, downCounter );
      }
      delete f;
    }
    else if( newState == -1 )
    {
      delete font;
      char c[200];
      sprintf( c, "invalid < found at location %d in %s", i+1, text_.c_str() );
      throw EGraphicsError( c );
    }
    else if( newState == -2 )
    {
      delete font;
      char c[200];
      sprintf( c, "invalid > found at location %d in %s", i+1, text_.c_str() );
      throw EGraphicsError( c );
    }
  }
  if( !text.empty() )Draw2( text, height, angle, x, y, font, outFile );
  delete font;
}

void TextBuffer::Draw2( std::string &text, double height, double angle,
                       double x, double y, ExFont *font, std::ofstream &outFile )
{
  LowLevelGraphics *llg = LowLevelGraphics::GetPtr();
  bool const landscape = (llg->GetOrientation()==std::string("LANDSCAPE"));
  int r, g, b;
  MyColor c( font->GetColor() );
  c.GetRGB(r,g,b);
  double dr = static_cast<double>(r)/255.0;
  double dg = static_cast<double>(g)/255.0;
  double db = static_cast<double>(b)/255.0;
  outFile << "stroke" << std::endl;
  outFile << dr << " " << dg << " " << db << " setrgbcolor" << std::endl;
  //
  int h = static_cast<int>(72*height+0.5);
  std::string fontName( font->GetName() );
  outFile << "/" << fontName.c_str() << " findfont " << h
          << " scalefont setfont" << std::endl;
  double xp, yp;
  llg->WorldToPrinter( x, y, xp, yp );
  int ix = static_cast<int>(72*xp+0.5);
  int iy = static_cast<int>(72*yp+0.5);
  outFile << ix << " " << iy << " moveto" << std::endl;
  if( landscape )
  {
    outFile << (90+angle) << " rotate" << std::endl;
  }
  else
  {
    if( angle != 0.0 )outFile << angle << " rotate" << std::endl;
  }
  size_t tEnd = text.size();
  outFile << "(";
  for( size_t i=0; i<tEnd; ++i )
  {
    int j = static_cast<int>(text[i]);
    if( j < 0 )j += 256;
    outFile << "\\" << std::setfill('0') << std::setw(3) << std::oct << j;
  }
  outFile << ") show" << std::endl;
  if( landscape )
  {
    outFile << (-(90+angle)) << " rotate" << std::endl;
  }
  else
  {
    if( angle != 0.0 )outFile << (-angle) << " rotate" << std::endl;
  }
}

void TextBuffer::Draw( gdImagePtr gd )
{
  if( text_.empty() )return;
  LowLevelGraphics *llg = LowLevelGraphics::GetPtr();
  //
  double x, y, angle, height, cosx, sinx;
  ExFont *font= new ExFont(*font_);
#ifdef DEBUG
fprintf(stderr, "TextBuffer Draw x = %f, y= %f\n", x, y);
#endif
  try
  {
    SetUp( x, y, angle, height, cosx, sinx, font );
  }
  catch (EGraphicsError &e)
  {
    delete font;
    throw;
  }
  int const classes[128] = {
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 2, 1, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 };
  int const stateTable[4][3] = {
   //  <   >  othr
     { 1,  0,  0 },
     {-1, -2,  2 },
     {-1,  3,  2 },
     { 1, -2,  0 } };
  bool italics = false;
  bool bold = false;
  std::string text, command;
  int upCounter=0, downCounter=0;
  int currentState, newState=0;
  size_t length = text_.size();
  for( size_t i=0; i<length; ++i )
  {
    currentState = newState;
    int iascii = toascii( text_[i] );
    newState = stateTable[currentState][classes[iascii]];
    if( newState == 0 ) // just add to text
    {
      text.append( 1, text_[i] );
    }
    else if( newState == 1 ) // found <, start of command
    {
      if( !text.empty() )
      {
        Draw2( text, height, angle, x, y, font, gd );
        double tmp = llg->GetTextWidth( text, height, font );
        x += tmp*cosx;
        y += tmp*sinx;
        text.erase();
      }
      command.erase();
    }
    else if( newState == 2 ) // add to command
    {
      command.append( 1, text_[i] );
    }
    else if( newState == 3 ) // found >, end of command
    {
      ExFont *f = new ExFont(*font);
      if( Special(command,font) )
      {
        Draw2( command, height, angle, x, y, font, gd );
        double tmp = llg->GetTextWidth( command, height, font );
        x += tmp*cosx;
        y += tmp*sinx;
        *font = *f;
      }
      else
      {
        DoCommand( command, font, x, y, height, angle, italics, bold,
                   cosx, sinx, upCounter, downCounter );
      }
      delete f;
    }
    else if( newState == -1 )
    {
      delete font;
      char c[200];
      sprintf( c, "invalid < found at location %d in %s", i+1, text_.c_str() );
      throw EGraphicsError( c );
    }
    else if( newState == -2 )
    {
      delete font;
      char c[200];
      sprintf( c, "invalid > found at location %d in %s", i+1, text_.c_str() );
      throw EGraphicsError( c );
    }
  }
  if( !text.empty() )Draw2( text, height, angle, x, y, font, gd );
  delete font;
}

void TextBuffer::Draw2( std::string &text, double height, double angle,
                       double x, double y, ExFont *font, gdImagePtr gd )
{
  LowLevelGraphics *llg = LowLevelGraphics::GetPtr();
  //
  int r, g, b;
  MyColor c( font->GetColor() );
  c.GetRGB(r,g,b);
  int clr = gdImageColorExact( gd, r, g, b );
  if( clr == -1 )clr = gdImageColorAllocate( gd, r, g, b );
  //
  height *= 72.; // height in pts
  //
  int ix, iy;
  llg->WorldToMonitor( x, y, ix, iy );
  //
  angle *= M_PI/180.0;
  //
  int brect[8];
  char *err = gdImageStringFT( gd, &brect[0], clr,
                               const_cast<char*>(font->GetName().c_str()),
                               height, angle, ix, iy, const_cast<char*>(text.c_str()) );
  if( err )throw EGraphicsError( err );
}

// end of file

