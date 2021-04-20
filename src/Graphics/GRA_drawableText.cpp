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
#include "UsefulFunctions.h"
#include "TextPopup.h"

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
  int ppi = dc.GetPPI().GetWidth();
  for( std::size_t i=0; i<size; ++i )
  {
    xLoc += width*cosx - strings_[i]->GetYShift()*sinx + strings_[i]->GetXShift()*cosx;
    yLoc += width*sinx + strings_[i]->GetYShift()*cosx + strings_[i]->GetXShift()*sinx;
    width = wv[i]*xConvert;
    double height = strings_[i]->GetHeight();

    int xo, yo1, yo2;
    graphicsOutput->WorldToOutputType( 0.0, 0.0, xo, yo1 );
    graphicsOutput->WorldToOutputType( 0.0, height, xo, yo2 );
    int h = yo1 - yo2;

    int x, y;
    graphicsOutput->WorldToOutputType( xLoc, yLoc, x, y );
    //
    wxFont font( strings_[i]->GetFont()->GetwxFont() );
    font.SetPointSize( h );
    dc.SetFont( font );
    dc.SetTextForeground( ExGlobals::GetwxColor(strings_[i]->GetColor()) );
    dc.DrawRotatedText( strings_[i]->GetString(), x, y, angle );
    //
    int w, descent, externalLeading;
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
  int ppi = dc.GetPPI().GetWidth();
  for( std::size_t i=0; i<size; ++i )
  {
    xLoc += width*cosx - strings_[i]->GetYShift()*sinx + strings_[i]->GetXShift()*cosx;
    yLoc += width*sinx + strings_[i]->GetYShift()*cosx + strings_[i]->GetXShift()*sinx;
    width = wv[i]*xConvert;
    double height = strings_[i]->GetHeight();

    int xo, yo1, yo2;
    graphicsOutput->WorldToOutputType( 0.0, 0.0, xo, yo1 );
    graphicsOutput->WorldToOutputType( 0.0, height, xo, yo2 );
    int h = yo2 - yo1;

    int x, y;
    graphicsOutput->WorldToOutputType( xLoc, yLoc, x, y );
    //
    wxFont font( strings_[i]->GetFont()->GetwxFont() );
    font.SetPointSize( h );
    dc.SetFont( font );
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
  int ppi = dc.GetPPI().GetWidth();
  for( std::size_t i=0; i<size; ++i )
  {
    double height = strings_[i]->GetHeight();

    int xo, yo1, yo2;
    graphicsOutput->WorldToOutputType( 0.0, 0.0, xo, yo1 );
    graphicsOutput->WorldToOutputType( 0.0, height, xo, yo2 );
    int h = yo1 - yo2;

    wxFont font( strings_[i]->GetFont()->GetwxFont() );
    //font.SetPointSize( static_cast<int>(height*ppi+0.5) );
    font.SetPointSize( h );
    dc.SetFont( font );
    int w, descent, extLead;
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
  wxString text, command;
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
          DetermineCommand( command, height, xShift, yShift, italics, bold, upCounter, downCounter );
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
    GRA_simpleText *tmp = new GRA_simpleText(text,height,color_,font_,xShift,yShift);
    strings_.push_back( tmp );
  }
}

void GRA_drawableText::DetermineCommand( wxString &command,
                                         double &height, double &xShift, double &yShift,
                                         bool &italics, bool &bold,
                                         int &upCounter, int &downCounter )
{
  wxString fontName;
  wxChar c = Special( command, fontName );
  if( c )
  {
    GRA_simpleText *tmp = new GRA_simpleText(c,height,color_,fontName.c_str(),xShift,yShift);
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
  if( command[0] == wxT('F') ) // set the font
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
    wxString s( wxT("invalid text formatting command: ") );
    s += command;
    throw std::runtime_error( std::string(s.mb_str(wxConvUTF8)) );
  }
}

wxChar GRA_drawableText::Special( wxString const &command, wxString &fontName )
{
  wxString s( command.Upper() );
  //
  bool upperCase = (const_cast<wxString&>(command)[0]==s[0]);
  //
/*
  struct specialCharacter
  {
    wxChar const *name;  // special character name
    wxChar const uName;  // upper case character
    wxChar const lName;  // lower case character
    wxChar const *fName; // font name
  };
  std::size_t const nNames = 93;
  specialCharacter spc[] =
  {
      {wxT("ALPHA"),0x41,0x61,wxT("Symbol")},          {wxT("BETA"),0x42,0x62,wxT("Symbol")},          {wxT("GAMMA"),0x47,0x67,wxT("Symbol")},
      {wxT("DELTA"),0x44,0x64,wxT("Symbol")},          {wxT("EPSILON"),0x45,0x65,wxT("Symbol")},       {wxT("ZETA"),0x5a,0x7a,wxT("Symbol")},
      {wxT("ETA"),0x48,0x68,wxT("Symbol")},            {wxT("THETA"),0x51,0x71,wxT("Symbol")},         {wxT("IOTA"),0x49,0x69,wxT("Symbol")},
      {wxT("KAPPA"),0x4b,0x6b,wxT("Symbol")},          {wxT("LAMBDA"),0x4c,0x6c,wxT("Symbol")},        {wxT("MU"),0x4d,0x6d,wxT("Symbol")},
      {wxT("NU"),0x4e,0x6e,wxT("Symbol")},             {wxT("XI"),0x58,0x78,wxT("Symbol")},            {wxT("OMICRON"),0x4f,0x6f,wxT("Symbol")},
      {wxT("PI"),0x50,0x70,wxT("Symbol")},             {wxT("RHO"),0x52,0x72,wxT("Symbol")},           {wxT("SIGMA"),0x53,0x73,wxT("Symbol")},
      {wxT("TAU"),0x54,0x74,wxT("Symbol")},            {wxT("UPSILON"),0x55,0x75,wxT("Symbol")},       {wxT("PHI"),0x46,0x66,wxT("Symbol")},
      {wxT("CHI"),0x43,0x63,wxT("Symbol")},            {wxT("PSI"),0x59,0x79,wxT("Symbol")},           {wxT("OMEGA"),0x57,0x77,wxT("Symbol")},
      {wxT("VARTHETA"),0x51,0x71,wxT("Symbol")},       {wxT("VARPHI"),0x6a,0x6a,wxT("Symbol")},        {wxT("VAREPSILON"),0x65,0x65,wxT("Symbol")},
      {wxT("ALEPH"),0xf0c0,0xf0c0,wxT("Symbol")},          {wxT("NABLA"),0x2014,0x2014,wxT("Symbol")},     {wxT("PARTIAL"),0x2202,0x2202,wxT("Symbol")},
      {wxT("LEFTARROW"),0x2039,0xa8,wxT("Symbol")},    {wxT("UPARROW"),0x203a,0x2260,wxT("Symbol")},   {wxT("DOWNARROW"),0xfb02,0xd8,wxT("Symbol")},
      {wxT("RIGHTARROW"),0xfb01,0xc6,wxT("Symbol")},   {wxT("PERP"),0x5e,0x5e,wxT("Symbol")},          {wxT("MID"),0x2d9,0x2d9,wxT("Symbol")},
      {wxT("BULLET"),0x2211,0x2211,wxT("Symbol")},     {wxT("SUM"),0xc2,0xc2,wxT("Symbol")},           {wxT("PROD"),0x2019,0x2019,wxT("Symbol")},
      {wxT("INT"),0xf0f2,0xf0f2,wxT("Symbol")},            {wxT("SURD"),0xf7,0xf7,wxT("Symbol")},          {wxT("PLUS"),0x2b,0x2b,wxT("Symbol")},
      {wxT("MINUS"),0x2d,0x2d,wxT("Symbol")},          {wxT("PM"),0xb1,0xb1,wxT("Symbol")},            {wxT("TIMES"),0xa5,0xa5,wxT("Symbol")},
      {wxT("DIV"),0x220f,0x220f,wxT("Symbol")},        {wxT("OPLUS"),0x2248,0x2248,wxT("Symbol")},     {wxT("OTIMES"),0x192,0x192,wxT("Symbol")},
      {wxT("CAP"),0xab,0xab,wxT("Symbol")},            {wxT("SUBSET"),0xc3,0xc3,wxT("Symbol")},        {wxT("CUP"),0xbb,0xbb,wxT("Symbol")},
      {wxT("SUPSET"),0x2026,0x2026,wxT("Symbol")},     {wxT("DEGREE"),0x221e,0x221e,wxT("Symbol")},    {wxT("LANGLE"),0xb7,0xb7,wxT("Symbol")},
      {wxT("RANGLE"),0xd2,0xd2,wxT("Symbol")},         {wxT("NEG"),0xff,0xff,wxT("Symbol")},           {wxT("THEREFORE"),0x5c,0x5c,wxT("Symbol")},
      {wxT("ANGLE"),0x2013,0x2013,wxT("Symbol")},      {wxT("VEE"),0x2044,0x2044,wxT("Symbol")},       {wxT("WEDGE"),0x2044,0x2044,wxT("Symbol")},
      {wxT("CDOT"),0x2e,0x2e,wxT("Symbol")},           {wxT("INFTY"),0x2022,0x2022,wxT("Symbol")},     {wxT("IN"),0x152,0x152,wxT("Symbol")},
      {wxT("NI"),0x153,0x153,wxT("Symbol")},           {wxT("PROPTO"),0xb5,0xb5,wxT("Symbol")},        {wxT("EXISTS"),0x24,0x24,wxT("Symbol")},
      {wxT("FORALL"),0x22,0x22,wxT("Symbol")},         {wxT("NEQ"),0x3c0,0x3c0,wxT("Symbol")},         {wxT("EQUIV"),0x222b,0x222b,wxT("Symbol")},
      {wxT("APPROX"),0xaa,0xaa,wxT("Symbol")},         {wxT("SIM"),0x7e,0x7e,wxT("Symbol")},           {wxT("LT"),0x3c,0x3c,wxT("Symbol")},
      {wxT("GT"),0x3e,0x3e,wxT("Symbol")},             {wxT("LEQ"),0xa3,0xa3,wxT("Symbol")},           {wxT("GEQ"),0x2265,0x2265,wxT("Symbol")},
      {wxT("VARPI"),0x76,0x76,wxT("Symbol")},          {wxT("CLUBSUIT"),0xdf,0xdf,wxT("Symbol")},      {wxT("DIAMONDSUIT"),0xae,0xae,wxT("Symbol")},
      {wxT("HEARTSUIT"),0xa9,0xa9,wxT("Symbol")},      {wxT("SPADESUIT"),0x2122,0x2122,wxT("Symbol")}, {wxT("SUBSETEQ"),0xd5,0xd5,wxT("Symbol")},
      {wxT("SUPSETEQ"),0xa0,0xa0,wxT("Symbol")},       {wxT("LDOTS"),0xba,0xba,wxT("Symbol")},         {wxT("LEFTRIGHTARROW"),0x20ac,0xb4,wxT("Symbol")},
      {wxT("EMPTYSET"),0x2206,0x2206,wxT("Symbol")},   {wxT("WP"),0x221a,0x221a,wxT("Symbol")},        {wxT("RE"),0xac,0xac,wxT("Symbol")},
      {wxT("IM"),0xa1,0xa1,wxT("Symbol")},             {wxT("HOOKLEFTARROW"),0xf8,0xf8,wxT("Symbol")}, {wxT("COPYRIGHT"),0x201d,0x201d,wxT("Symbol")},
      {wxT("REGISTERED"),0xf0e2,0xf0e2,wxT("Symbol")}, {wxT("TM"),0x2030,0x2030,wxT("Symbol")},        {wxT("DIAMOND"),0x2021,0x2021,wxT("Symbol")} 
  };
*/
  struct specialCharacter
  {
    wxChar const *name;  // special character name
    wxChar const uName;  // upper case character
    wxChar const lName;  // lower case character
  };
  std::size_t const nNames = 93;
  specialCharacter spc[] =
  {
      {wxT("ALPHA"),0x0391,0x03B1},      {wxT("BETA"),0x0392,0x03B2},          {wxT("GAMMA"),0x0393,0x03B3},
      {wxT("DELTA"),0x0394,0x03B4},      {wxT("EPSILON"),0x0395,0x03B5},       {wxT("ZETA"),0x0396,0x03B6},
      {wxT("ETA"),0x0397,0x03B7},        {wxT("THETA"),0x0398,0x03B8},         {wxT("IOTA"),0x0399,0x03B9},
      {wxT("KAPPA"),0x039A,0x03BA},      {wxT("LAMBDA"),0x039B,0x03BB},        {wxT("MU"),0x039C,0x03BC},
      {wxT("NU"),0x039D,0x03BD},         {wxT("XI"),0x039E,0x03BE},            {wxT("OMICRON"),0x039F,0x03BF},
      {wxT("PI"),0x03A0,0x03C0},         {wxT("RHO"),0x03A1,0x03C1},           {wxT("SIGMA"),0x03A3,0x03C3},
      {wxT("TAU"),0x03A4,0x03C4},        {wxT("UPSILON"),0x03A5,0x03C5},       {wxT("PHI"),0x03A6,0x03C6},
      {wxT("CHI"),0x03A7,0x03C7},        {wxT("PSI"),0x03A8,0x03C8},           {wxT("OMEGA"),0x03A9,0x03C9},
      {wxT("VARTHETA"),0x03D1,0x03D1},   {wxT("VARPHI"),0x03D5,0x03D5},        {wxT("VAREPSILON"),0x03F5,0x03F5},
      {wxT("ALEPH"),0x2135,0x2135},      {wxT("NABLA"),0x2207,0x2207},         {wxT("PARTIAL"),0x2202,0x2202},
      {wxT("LEFTARROW"),0x2190,0x2190},  {wxT("UPARROW"),0x2191,0x2191},       {wxT("DOWNARROW"),0x2193,0x2193},
      {wxT("RIGHTARROW"),0x2192,0x2192}, {wxT("PERP"),0x27C2,0x27C2},          {wxT("MID"),0x2223,0x2223},
      {wxT("BULLET"),0x2022,0x2022},     {wxT("SUM"),0x00E5,0x00E5},           {wxT("PROD"),0x220F,0x220F},
      {wxT("INT"),0x222B,0x222B},        {wxT("SURD"),0x221A,0x221A},          {wxT("PLUS"),0x002B,0x002B},
      {wxT("MINUS"),0x2212,0x2212},      {wxT("PM"),0x00B1,0x00B1},            {wxT("TIMES"),0x00B4,0x00B4},
      {wxT("DIV"),0x00B8,0x00B8},        {wxT("OPLUS"),0x2295,0x2295},         {wxT("OTIMES"),0x2297,0x2297},
      {wxT("CAP"),0x2229,0x2229},        {wxT("SUBSET"),0x2282,0x2282},        {wxT("CUP"),0x222A,0x222A},
      {wxT("SUPSET"),0x2283,0x2283},     {wxT("DEGREE"),0x02DA,0x02DA},        {wxT("LANGLE"),0x27E8,0x27E8},
      {wxT("RANGLE"),0x27E9,0x27E9},     {wxT("NEG"),0x00D8,0x00D8},           {wxT("THEREFORE"),0x2234,0x2234},
      {wxT("ANGLE"),0x2220,0x2220},      {wxT("VEE"),0x2228,0x2228},           {wxT("WEDGE"),0x2227,0x2227},
      {wxT("CDOT"),0x00D7,0x00D7},       {wxT("INFTY"),0x221E,0x221E},         {wxT("IN"),0x2208,0x2208},
      {wxT("NI"),0x220B,0x220B},         {wxT("PROPTO"),0x221D,0x221D},        {wxT("EXISTS"),0x2203,0x2203},
      {wxT("FORALL"),0x2200,0x2200},     {wxT("NEQ"),0x2260,0x2260},           {wxT("EQUIV"),0x00BA,0x00BA},
      {wxT("APPROX"),0x2248,0x2248},     {wxT("SIM"),0x007E,0x007E},           {wxT("LT"),0x003C,0x003C},
      {wxT("GT"),0x003E,0x003E},         {wxT("LEQ"),0x2264,0x2264},           {wxT("GEQ"),0x2265,0x2265},
      {wxT("VARPI"),0x03D6,0x03D6},      {wxT("CLUBSUIT"),0x2663,0x2663},      {wxT("DIAMONDSUIT"),0x2666,0x2666},
      {wxT("HEARTSUIT"),0x2665,0x2665},  {wxT("SPADESUIT"),0x2660,0x2660},     {wxT("SUBSETEQ"),0x2286,0x2286},
      {wxT("SUPSETEQ"),0x2287,0x2287},   {wxT("LDOTS"),0x2026,0x2026},         {wxT("LEFTRIGHTARROW"),0x21D4,0x21D4},
      {wxT("EMPTYSET"),0x2205,0x2205},   {wxT("WP"),0x2118,0x2118},            {wxT("RE"),0x211C,0x211C},
      {wxT("IM"),0x2111,0x2111},         {wxT("HOOKLEFTARROW"),0x21B2,0x21B2}, {wxT("COPYRIGHT"),0x00E3,0x00E3},
      {wxT("REGISTERED"),0x00E2,0x00E2}, {wxT("TM"),0x2122,0x2122},            {wxT("DIAMOND"),0x25CA,0x25CA} 
  };
  for( std::size_t i=0; i<nNames; ++i )
  {
    if( s == spc[i].name )
    {
      //fontName = spc[i].fName;
      fontName = wxT("symbol");
      return upperCase ? spc[i].uName : spc[i].lName;
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
