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
#include <ios>
#include <stdexcept>
#include <fstream>

#include "GRA_colorControl.h"
#include "GRA_color.h"
#include "GRA_colorMap.h"
#include "EGraphicsError.h"

namespace GRA_colorControl
{
GRA_colorMap *current_ =0;
GRA_colorMap *userdefined_ =0;
GRA_colorMap *thermal_ =0;
GRA_colorMap *gray256_ =0;
GRA_colorMap *gray16_ =0;
GRA_colorMap *rainbow_ =0;
GRA_colorMap *redblue16_ =0;
GRA_colorMap *classic8_ =0;
GRA_colorMap *topo256_ =0;
GRA_colorMap *default_ =0;
std::vector<GRA_color*> namedColors_;

void Initialize()
{
  CreateThermalColorMap();
  CreateGray256ColorMap();
  CreateGray16ColorMap();
  CreateRainbowColorMap();
  CreateRedBlue16ColorMap();
  CreateClassic8ColorMap();
  CreateTopo256ColorMap();
  //
  CreateDefaultColorMap();
  userdefined_ = 0;
  //
  current_ = default_; // created just above
  //
  CreateNamedColors();
}

void DeleteStuff()
{
  delete thermal_;
  delete gray256_;
  delete gray16_;
  delete rainbow_;
  delete redblue16_;
  delete classic8_;
  delete topo256_;
  delete default_;
  delete userdefined_;
  std::vector<GRA_color*>::const_iterator end = namedColors_.end();
  for( std::vector<GRA_color*>::const_iterator i=namedColors_.begin(); i!=end; ++i )
    delete *i;
}

wxString GetColorMapName()
{ return current_->GetName(); }

GRA_colorMap *GetColorMap()
{ return current_; }

void SetFile( wxString const &fileName )
{
  std::ifstream readIn;
  readIn.open( fileName.mb_str(), std::ios_base::in );
  if( !readIn )
  {
    wxString s(wxT("could not open file: "));
    s += fileName;
    throw EGraphicsError( s );
  }
  GRA_colorMap *tmp = new GRA_colorMap( wxT("USERDEFINED") );
  for( int record=0; ; ++record )
  {
    std::string sc;
    sc.clear();
    if( !std::getline(readIn,sc) )break;
    if( sc.empty() )continue; // the record is empty, nothing on the line
    std::string rs, gs, bs;
    std::string const prefix("0x");
    try
    {
      rs = prefix + sc.substr(0,2).c_str();
      gs = prefix + sc.substr(3,2).c_str();
      bs = prefix + sc.substr(6,2).c_str();
    }
    catch (std::exception const &e)
    {
      throw EGraphicsError( wxString(e.what(),wxConvUTF8) );
    }
    long int rl, gl, bl;
    wxString((wxChar*)rs.c_str()).ToLong( &rl );
    wxString((wxChar*)gs.c_str()).ToLong( &gl );
    wxString((wxChar*)bs.c_str()).ToLong( &bl );
    tmp->AddColor( new GRA_color(static_cast<int>(rl),static_cast<int>(gl),static_cast<int>(bl)) );
  }
  delete userdefined_;
  userdefined_ = tmp;
  current_ = userdefined_;
}

void CreateNamedColors()
{
  namedColors_.push_back( new GRA_color(wxT("WHITE"),255,255,255) );
  namedColors_.push_back( new GRA_color(wxT("BLACK"),0,0,0) );
  namedColors_.push_back( new GRA_color(wxT("RED"),255,0,0) );
  namedColors_.push_back( new GRA_color(wxT("GREEN"),0,255,0) );
  namedColors_.push_back( new GRA_color(wxT("BLUE"),0,0,255) );
  namedColors_.push_back( new GRA_color(wxT("PURPLE"),160,32,240) );
  namedColors_.push_back( new GRA_color(wxT("YELLOW"),255,255,0) );
  namedColors_.push_back( new GRA_color(wxT("CYAN"),0,255,255) );
  namedColors_.push_back( new GRA_color(wxT("BROWN"),165,42,42) );
  namedColors_.push_back( new GRA_color(wxT("CORAL"),255,127,80) );
  namedColors_.push_back( new GRA_color(wxT("SALMON"),250,128,114) );
  namedColors_.push_back( new GRA_color(wxT("SIENNA"),160,82,45) );
  namedColors_.push_back( new GRA_color(wxT("TAN"),210,180,140) );
  namedColors_.push_back( new GRA_color(wxT("FUCHSIA"),255,0,255) );
  namedColors_.push_back( new GRA_color(wxT("LIME"),50,205,50) );
  namedColors_.push_back( new GRA_color(wxT("MAROON"),128,0,0) );
  namedColors_.push_back( new GRA_color(wxT("NAVY"),0,0,128) );
  namedColors_.push_back( new GRA_color(wxT("OLIVE"),128,128,0) );
  namedColors_.push_back( new GRA_color(wxT("SILVER"),192,192,192) );
  namedColors_.push_back( new GRA_color(wxT("TEAL"),0,128,128) );
}

void SetColorMap( wxString const &inName )
{
  wxString name( inName.Upper() );
  if( name == wxString(wxT("THERMAL")) )current_ = thermal_;
  else if( name == wxString(wxT("GRAY256")) )current_ = gray256_;
  else if( name == wxString(wxT("GRAY16")) )current_ = gray16_;
  else if( name == wxString(wxT("RAINBOW")) )current_ = rainbow_;
  else if( name == wxString(wxT("REDBLUE16")) )current_ = redblue16_;
  else if( name == wxString(wxT("CLASSIC8")) )current_ = classic8_;
  else if( name == wxString(wxT("TOPO256")) )current_ = topo256_;
  else if( name == wxString(wxT("DEFAULT")) )current_ = default_;
  else if( name == wxString(wxT("USERDEFINED")) && userdefined_ )current_ = userdefined_;
  else throw EGraphicsError( wxT("unknown color map") );
}

GRA_colorMap *GetColorMap( wxString const &inName )
{
  wxString name( inName.Upper() );
  if( name == wxString(wxT("THERMAL")) )return thermal_;
  else if( name == wxString(wxT("GRAY256")) )return gray256_;
  else if( name == wxString(wxT("GRAY16")) )return gray16_;
  else if( name == wxString(wxT("RAINBOW")) )return rainbow_;
  else if( name == wxString(wxT("REDBLUE16")) )return redblue16_;
  else if( name == wxString(wxT("CLASSIC8")) )return classic8_;
  else if( name == wxString(wxT("TOPO256")) )return topo256_;
  else if( name == wxString(wxT("DEFAULT")) )return default_;
  else if( name == wxString(wxT("USERDEFINED")) && userdefined_ )return userdefined_;
  else throw EGraphicsError( wxT("unknown color map") );
}

void SetColorMapUser( GRA_colorMap *colorMap )
{
  delete userdefined_;
  userdefined_ = colorMap;
  current_ = userdefined_;
}

void SetColorMap( GRA_colorMap *colorMap )
{ current_ = colorMap; }

int GetNumberOfNamedColors()
{ return namedColors_.size(); }

int GetColorCode( wxString const &nameIn )
{
  // since color must be a named color, return int <= 0
  //
  wxString name( nameIn.Upper() );
  std::size_t size = namedColors_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    if( name == namedColors_[i]->GetName() )return -1*static_cast<int>(i);
  }
  wxString message(wxT("color "));
  message += name + wxString(wxT(" not found"));
  throw EGraphicsError( message );
}

int GetColorCode( int r, int g, int b )
{
  std::size_t size = namedColors_.size();
  GRA_color color(r,g,b);
  for( std::size_t i=0; i<size; ++i )
  {
    if( *namedColors_[i] == color )return -1*static_cast<int>(i);
  }
  return current_->GetCode( r, g, b ); // returns 0 if r,g,b not in color map
}

int GetColorCode( GRA_color *color )
{
  if( !color )return 0;
  //
  // if color is a named color, return int <= 0
  // if color is not named, return int > 0
  //
  std::size_t size = namedColors_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    if( namedColors_[i] == color )return -1*static_cast<int>(i);
  }
  return current_->GetCode( color );
}

GRA_color *GetColor( wxString const &nameIn )
{
  GRA_color *color = 0;
  wxString name( nameIn.Upper() );
  int size = namedColors_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    if( name == namedColors_[i]->GetName() )
    {
      color = namedColors_[i];
      break;
    }
  }
  return color;
}

GRA_color *GetColor( int i )
{
  // if i <= 0, color is a named color
  // if i > 0, color is not named
  // no need to check the size of i, since the code number will wrap
  //
  if( i > 0 )return current_->GetColor(i);
  return namedColors_[abs(i)%namedColors_.size()];
}

int GetColorMapSize()
{ return current_->GetSize(); }

void ColorCodeToRGB( int code, int &r, int &g, int &b )
{
  GRA_color *color = GetColor( code );
  color->GetRGB(r,g,b);
}

void CreateThermalColorMap()
{
  thermal_ = new GRA_colorMap( wxT("THERMAL") );
  for( int i=0; i<=252; i+=3 )thermal_->AddColor( new GRA_color(i,0,0) );
  for( int i=0; i<=252; i+=3 )thermal_->AddColor( new GRA_color(255,i,0) );
  for( int i=0; i<=252; i+=3 )thermal_->AddColor( new GRA_color(255,255,i) );
  thermal_->AddColor( new GRA_color(255,255,255) );
}

void CreateGray256ColorMap()
{
  gray256_ = new GRA_colorMap( wxT("GRAY256") );
  for( int i=0; i<256; ++i )gray256_->AddColor( new GRA_color(i,i,i) );
}

void CreateGray16ColorMap()
{
  gray16_ = new GRA_colorMap( wxT("GRAY16") );
  for( int i=0; i<=210; i+=15 )gray16_->AddColor( new GRA_color(i,i,i) );
  gray16_->AddColor( new GRA_color(255,255,255) );
}

void CreateRainbowColorMap()
{
  rainbow_ = new GRA_colorMap( wxT("RAINBOW") );
  for( int i=0; i<=248; i+=4 )rainbow_->AddColor( new GRA_color(255,i,0) );
  rainbow_->AddColor( new GRA_color(255,255,0) );
  rainbow_->AddColor( new GRA_color(253,255,0) );
  for( int i=252; i>=0; i-=4 )rainbow_->AddColor( new GRA_color(i,255,0) );
  for( int i=4; i<=248; i+=4 )rainbow_->AddColor( new GRA_color(0,255,i) );
  rainbow_->AddColor( new GRA_color(0,255,255) );
  for( int i=252; i>=0; i-=4 )rainbow_->AddColor( new GRA_color(0,i,255) );
}

void CreateRedBlue16ColorMap()
{
  redblue16_ = new GRA_colorMap( wxT("REDBLUE16") );
  redblue16_->AddColor( new GRA_color(255,0,0) );
  for( int i=16; i<=224; i+=16 )redblue16_->AddColor( new GRA_color(256-i,0,i) );
  redblue16_->AddColor( new GRA_color(0,0,255) );
}

void CreateClassic8ColorMap()
{
  classic8_ = new GRA_colorMap( wxT("CLASSIC8") );
  classic8_->AddColor( new GRA_color(0,0,0) );
  classic8_->AddColor( new GRA_color(255,0,0) );
  classic8_->AddColor( new GRA_color(0,0,255) );
  classic8_->AddColor( new GRA_color(128,0,128) );
  classic8_->AddColor( new GRA_color(0,255,0) );
  classic8_->AddColor( new GRA_color(255,255,0) );
  classic8_->AddColor( new GRA_color(0,255,255) );
  classic8_->AddColor( new GRA_color(255,255,255) );
}

void CreateTopo256ColorMap()
{
  topo256_ = new GRA_colorMap( wxT("TOPO256") );
  for( int i=0; i<=248; i+=4 )topo256_->AddColor( new GRA_color(0,0,i) );
  topo256_->AddColor( new GRA_color(0,0,255) );
  topo256_->AddColor( new GRA_color(4,4,255) );
  topo256_->AddColor( new GRA_color(7,7,255) );
  topo256_->AddColor( new GRA_color(10,10,255) );
  topo256_->AddColor( new GRA_color(13,13,255) );
  topo256_->AddColor( new GRA_color(16,16,255) );
  topo256_->AddColor( new GRA_color(19,19,255) );
  topo256_->AddColor( new GRA_color(22,22,255) );
  topo256_->AddColor( new GRA_color(26,26,255) );
  topo256_->AddColor( new GRA_color(29,29,255) );
  topo256_->AddColor( new GRA_color(32,32,255) );
  topo256_->AddColor( new GRA_color(35,35,255) );
  topo256_->AddColor( new GRA_color(38,38,255) );
  topo256_->AddColor( new GRA_color(41,41,255) );
  topo256_->AddColor( new GRA_color(44,44,255) );
  topo256_->AddColor( new GRA_color(48,48,255) );
  topo256_->AddColor( new GRA_color(51,51,255) );
  topo256_->AddColor( new GRA_color(54,54,255) );
  topo256_->AddColor( new GRA_color(57,57,255) );
  topo256_->AddColor( new GRA_color(60,60,255) );
  topo256_->AddColor( new GRA_color(63,63,255) );
  topo256_->AddColor( new GRA_color(66,66,255) );
  topo256_->AddColor( new GRA_color(70,70,255) );
  topo256_->AddColor( new GRA_color(73,73,255) );
  topo256_->AddColor( new GRA_color(76,76,255) );
  topo256_->AddColor( new GRA_color(79,79,255) );
  topo256_->AddColor( new GRA_color(82,82,255) );
  topo256_->AddColor( new GRA_color(85,85,255) );
  topo256_->AddColor( new GRA_color(88,88,255) );
  topo256_->AddColor( new GRA_color(92,92,255) );
  topo256_->AddColor( new GRA_color(95,95,255) );
  topo256_->AddColor( new GRA_color(98,98,255) );
  topo256_->AddColor( new GRA_color(101,101,255) );
  topo256_->AddColor( new GRA_color(104,104,255) );
  topo256_->AddColor( new GRA_color(107,107,255) );
  topo256_->AddColor( new GRA_color(110,110,255) );
  topo256_->AddColor( new GRA_color(113,113,255) );
  topo256_->AddColor( new GRA_color(117,117,255) );
  topo256_->AddColor( new GRA_color(120,120,255) );
  topo256_->AddColor( new GRA_color(123,123,255) );
  topo256_->AddColor( new GRA_color(126,126,255) );
  topo256_->AddColor( new GRA_color(129,129,255) );
  topo256_->AddColor( new GRA_color(132,132,255) );
  topo256_->AddColor( new GRA_color(135,135,255) );
  topo256_->AddColor( new GRA_color(139,139,255) );
  topo256_->AddColor( new GRA_color(142,142,255) );
  topo256_->AddColor( new GRA_color(145,145,255) );
  topo256_->AddColor( new GRA_color(148,148,255) );
  topo256_->AddColor( new GRA_color(151,151,255) );
  topo256_->AddColor( new GRA_color(154,154,255) );
  topo256_->AddColor( new GRA_color(157,157,255) );
  topo256_->AddColor( new GRA_color(161,161,255) );
  topo256_->AddColor( new GRA_color(164,164,255) );
  topo256_->AddColor( new GRA_color(167,167,255) );
  topo256_->AddColor( new GRA_color(170,170,255) );
  topo256_->AddColor( new GRA_color(173,173,255) );
  topo256_->AddColor( new GRA_color(176,176,255) );
  topo256_->AddColor( new GRA_color(179,179,255) );
  topo256_->AddColor( new GRA_color(183,183,255) );
  topo256_->AddColor( new GRA_color(186,186,255) );
  topo256_->AddColor( new GRA_color(189,189,255) );
  topo256_->AddColor( new GRA_color(192,192,255) );
  topo256_->AddColor( new GRA_color(195,195,255) );
  topo256_->AddColor( new GRA_color(198,198,255) );
  topo256_->AddColor( new GRA_color(204,204,255) );
  topo256_->AddColor( new GRA_color(0,153,0) );
  topo256_->AddColor( new GRA_color(0,158,0) );
  topo256_->AddColor( new GRA_color(0,162,0) );
  topo256_->AddColor( new GRA_color(0,166,0) );
  topo256_->AddColor( new GRA_color(0,170,0) );
  topo256_->AddColor( new GRA_color(0,174,0) );
  topo256_->AddColor( new GRA_color(0,178,0) );
  topo256_->AddColor( new GRA_color(0,182,0) );
  topo256_->AddColor( new GRA_color(0,186,0) );
  topo256_->AddColor( new GRA_color(0,190,0) );
  topo256_->AddColor( new GRA_color(0,194,0) );
  topo256_->AddColor( new GRA_color(0,198,0) );
  topo256_->AddColor( new GRA_color(0,202,0) );
  topo256_->AddColor( new GRA_color(0,207,0) );
  topo256_->AddColor( new GRA_color(0,211,0) );
  topo256_->AddColor( new GRA_color(0,215,0) );
  topo256_->AddColor( new GRA_color(0,219,0) );
  topo256_->AddColor( new GRA_color(0,223,0) );
  topo256_->AddColor( new GRA_color(0,227,0) );
  topo256_->AddColor( new GRA_color(0,231,0) );
  topo256_->AddColor( new GRA_color(0,235,0) );
  topo256_->AddColor( new GRA_color(0,239,0) );
  topo256_->AddColor( new GRA_color(0,243,0) );
  topo256_->AddColor( new GRA_color(0,247,0) );
  topo256_->AddColor( new GRA_color(0,255,0) );
  topo256_->AddColor( new GRA_color(10,255,0) );
  topo256_->AddColor( new GRA_color(19,255,0) );
  topo256_->AddColor( new GRA_color(29,255,0) );
  topo256_->AddColor( new GRA_color(38,255,0) );
  topo256_->AddColor( new GRA_color(48,255,0) );
  topo256_->AddColor( new GRA_color(57,255,0) );
  topo256_->AddColor( new GRA_color(67,255,0) );
  topo256_->AddColor( new GRA_color(76,255,0) );
  topo256_->AddColor( new GRA_color(85,255,0) );
  topo256_->AddColor( new GRA_color(95,255,0) );
  topo256_->AddColor( new GRA_color(104,255,0) );
  topo256_->AddColor( new GRA_color(114,255,0) );
  topo256_->AddColor( new GRA_color(123,255,0) );
  topo256_->AddColor( new GRA_color(133,255,0) );
  topo256_->AddColor( new GRA_color(142,255,0) );
  topo256_->AddColor( new GRA_color(152,255,0) );
  topo256_->AddColor( new GRA_color(161,255,0) );
  topo256_->AddColor( new GRA_color(171,255,0) );
  topo256_->AddColor( new GRA_color(180,255,0) );
  topo256_->AddColor( new GRA_color(189,255,0) );
  topo256_->AddColor( new GRA_color(199,255,0) );
  topo256_->AddColor( new GRA_color(208,255,0) );
  topo256_->AddColor( new GRA_color(218,255,0) );
  topo256_->AddColor( new GRA_color(227,255,0) );
  topo256_->AddColor( new GRA_color(237,255,0) );
  topo256_->AddColor( new GRA_color(255,255,0) );
  topo256_->AddColor( new GRA_color(255,252,0) );
  topo256_->AddColor( new GRA_color(255,248,0) );
  topo256_->AddColor( new GRA_color(255,244,0) );
  topo256_->AddColor( new GRA_color(255,240,0) );
  topo256_->AddColor( new GRA_color(255,237,0) );
  topo256_->AddColor( new GRA_color(255,233,0) );
  topo256_->AddColor( new GRA_color(255,229,0) );
  topo256_->AddColor( new GRA_color(255,225,0) );
  topo256_->AddColor( new GRA_color(255,222,0) );
  topo256_->AddColor( new GRA_color(255,218,0) );
  topo256_->AddColor( new GRA_color(255,214,0) );
  topo256_->AddColor( new GRA_color(255,210,0) );
  topo256_->AddColor( new GRA_color(255,206,0) );
  topo256_->AddColor( new GRA_color(255,203,0) );
  topo256_->AddColor( new GRA_color(255,199,0) );
  topo256_->AddColor( new GRA_color(255,195,0) );
  topo256_->AddColor( new GRA_color(255,191,0) );
  topo256_->AddColor( new GRA_color(255,188,0) );
  topo256_->AddColor( new GRA_color(255,184,0) );
  topo256_->AddColor( new GRA_color(255,180,0) );
  topo256_->AddColor( new GRA_color(255,176,0) );
  topo256_->AddColor( new GRA_color(255,172,0) );
  topo256_->AddColor( new GRA_color(255,169,0) );
  topo256_->AddColor( new GRA_color(255,165,0) );
  topo256_->AddColor( new GRA_color(255,161,0) );
  topo256_->AddColor( new GRA_color(255,153,0) );
  topo256_->AddColor( new GRA_color(255,152,0) );
  topo256_->AddColor( new GRA_color(248,150,0) );
  topo256_->AddColor( new GRA_color(244,148,0) );
  topo256_->AddColor( new GRA_color(240,146,0) );
  topo256_->AddColor( new GRA_color(237,144,0) );
  topo256_->AddColor( new GRA_color(233,142,0) );
  topo256_->AddColor( new GRA_color(229,140,0) );
  topo256_->AddColor( new GRA_color(225,138,0) );
  topo256_->AddColor( new GRA_color(222,137,0) );
  topo256_->AddColor( new GRA_color(218,135,0) );
  topo256_->AddColor( new GRA_color(214,133,0) );
  topo256_->AddColor( new GRA_color(210,131,0) );
  topo256_->AddColor( new GRA_color(206,129,0) );
  topo256_->AddColor( new GRA_color(203,127,0) );
  topo256_->AddColor( new GRA_color(199,125,0) );
  topo256_->AddColor( new GRA_color(195,123,0) );
  topo256_->AddColor( new GRA_color(191,121,0) );
  topo256_->AddColor( new GRA_color(188,120,0) );
  topo256_->AddColor( new GRA_color(184,118,0) );
  topo256_->AddColor( new GRA_color(180,116,0) );
  topo256_->AddColor( new GRA_color(176,114,0) );
  topo256_->AddColor( new GRA_color(172,112,0) );
  topo256_->AddColor( new GRA_color(169,110,0) );
  topo256_->AddColor( new GRA_color(165,108,0) );
  topo256_->AddColor( new GRA_color(161,106,0) );
  topo256_->AddColor( new GRA_color(153,102,0) );
  topo256_->AddColor( new GRA_color(157,108,10) );
  topo256_->AddColor( new GRA_color(161,114,20) );
  topo256_->AddColor( new GRA_color(165,120,30) );
  topo256_->AddColor( new GRA_color(169,126,40) );
  topo256_->AddColor( new GRA_color(173,132,50) );
  topo256_->AddColor( new GRA_color(177,138,59) );
  topo256_->AddColor( new GRA_color(181,144,69) );
  topo256_->AddColor( new GRA_color(185,150,79) );
  topo256_->AddColor( new GRA_color(189,155,89) );
  topo256_->AddColor( new GRA_color(193,161,99) );
  topo256_->AddColor( new GRA_color(197,167,108) );
  topo256_->AddColor( new GRA_color(201,173,118) );
  topo256_->AddColor( new GRA_color(205,179,128) );
  topo256_->AddColor( new GRA_color(208,185,138) );
  topo256_->AddColor( new GRA_color(212,191,148) );
  topo256_->AddColor( new GRA_color(216,197,157) );
  topo256_->AddColor( new GRA_color(220,203,167) );
  topo256_->AddColor( new GRA_color(224,208,177) );
  topo256_->AddColor( new GRA_color(228,214,187) );
  topo256_->AddColor( new GRA_color(232,220,197) );
  topo256_->AddColor( new GRA_color(236,226,206) );
  topo256_->AddColor( new GRA_color(240,232,216) );
  topo256_->AddColor( new GRA_color(244,238,226) );
  topo256_->AddColor( new GRA_color(248,244,236) );
  topo256_->AddColor( new GRA_color(255,255,255) );
}

void CreateDefaultColorMap()
{
  default_ = new GRA_colorMap( wxT("DEFAULT") );
  for( int i=0; i<6; ++i )
  {
    for( int j=0; j<6; ++j )
    {
      for( int k=0; k<6; ++k )
        default_->AddColor( new GRA_color(i*0x33,j*0x33,k*0x33) );
    }
  }
}

} // end of namespace

// end of file
