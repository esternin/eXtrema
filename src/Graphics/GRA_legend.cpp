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
#include "GRA_legend.h"
#include "EGraphicsError.h"
#include "GRA_window.h"
#include "GRA_color.h"
#include "GRA_colorControl.h"
#include "GRA_font.h"
#include "GRA_drawableText.h"
#include "GRA_plotSymbol.h"
#include "GRA_rectangle.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_wxWidgets.h"
#include "ExGlobals.h"

void GRA_legend::Initialize()
{
  GRA_setOfCharacteristics *legendC = ExGlobals::GetGraphWindow()->GetGraphLegendCharacteristics();
  xlo_ = static_cast<GRA_distanceCharacteristic*>(legendC->Get(wxT("FRAMEXLO")))->GetAsWorld();
  ylo_ = static_cast<GRA_distanceCharacteristic*>(legendC->Get(wxT("FRAMEYLO")))->GetAsWorld();
  xhi_ = static_cast<GRA_distanceCharacteristic*>(legendC->Get(wxT("FRAMEXHI")))->GetAsWorld();
  yhi_ = static_cast<GRA_distanceCharacteristic*>(legendC->Get(wxT("FRAMEYHI")))->GetAsWorld();
  xLineStart_ = xlo_ + (xhi_-xlo_)*0.05;
  xLineEnd_ = xlo_ + (xhi_-xlo_)*0.4;
  xText_ = xlo_ + (xhi_-xlo_)*0.5;
}

void GRA_legend::DeleteStuff()
{
  while ( !symbols_.empty() )
  {
    while ( !symbols_.back().empty() )
    {
      delete symbols_.back().back();
      symbols_.back().pop_back();
    }
    symbols_.pop_back();
  }
  while ( !textVec_.empty() )
  {
    delete textVec_.back();
    textVec_.pop_back();
  }
  delete frame_;
  frame_ = 0;
  delete title_;
  title_ = 0;
}

void GRA_legend::CopyStuff( GRA_legend const &rhs )
{
  firstTime_ = rhs.firstTime_;
  //
  xlo_ = rhs.xlo_;
  ylo_ = rhs.ylo_;
  xhi_ = rhs.xhi_;
  yhi_ = rhs.yhi_;
  xLineStart_ = rhs.xLineStart_;
  xLineEnd_ = rhs.xLineEnd_;
  xText_ = rhs.xText_;
  //
  colors_.assign( rhs.colors_.begin(), rhs.colors_.end() );
  lineTypes_.assign( rhs.lineTypes_.begin(), rhs.lineTypes_.end() );
  lineWidths_.assign( rhs.lineWidths_.begin(), rhs.lineWidths_.end() );
  entryLines_.assign( rhs.entryLines_.begin(), rhs.entryLines_.end() );
  std::size_t size = rhs.symbols_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    std::size_t size2 = rhs.symbols_[i].size();
    for( std::size_t j=0; j<size2; ++j )symbols_[i][j] = new GRA_plotSymbol(*rhs.symbols_[i][j]);
  }
  size = rhs.textVec_.size();
  for( std::size_t i=0; i<size; ++i )
    textVec_.push_back( new GRA_drawableText(*rhs.textVec_[i]) );
  frame_ = new GRA_rectangle( *rhs.frame_ );
  title_ = new GRA_drawableText( *rhs.title_ );
}

void GRA_legend::AddEntry( wxString &label, bool entryLineOn,
                           GRA_color *color, int lineType, int lineWidth,
                           std::vector<GRA_plotSymbol*> symbols,
                           GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  labels_.push_back( label );
  entryLines_.push_back( entryLineOn );
  colors_.push_back( color );
  lineTypes_.push_back( lineType );
  lineWidths_.push_back( lineWidth );
  symbols_.push_back( symbols );
  //
  if( firstTime_ )
  {
    DrawFrameAndTitle( graphicsOutput, dc );
    firstTime_ = false;
  }
  GRA_setOfCharacteristics *textC = ExGlobals::GetGraphWindow()->GetTextCharacteristics();
  double textHeight = static_cast<GRA_sizeCharacteristic*>(textC->Get(wxT("HEIGHT")))->GetAsWorld();
  GRA_font *textFont = static_cast<GRA_fontCharacteristic*>(textC->Get(wxT("FONT")))->Get();
  GRA_color *textColor = static_cast<GRA_colorCharacteristic*>(textC->Get(wxT("COLOR")))->Get();
  //
  // plot the text string part of the legend
  //
  double y = yhi_ - 2.0*textHeight*(textVec_.size()+1);
  GRA_drawableText *dt = new GRA_drawableText(label,textHeight,0.0,xText_,y,1,textFont,textColor);
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    delete dt;
    throw;
  }
  dt->Draw( graphicsOutput, dc );
  textVec_.push_back( dt );
  //
  // now plot the line segment and the legend symbols
  //
  double yc = y + 1.5*textHeight;
  //
  if( entryLineOn ) // plot the line segment part of the legend
  {
    int lineTypeSave = graphicsOutput->GetLineType();
    graphicsOutput->SetLineType( lineType );
    wxPen wxpen( dc.GetPen() );
    wxpen.SetWidth( lineWidth );
    wxpen.SetColour( ExGlobals::GetwxColor(color) );
    dc.SetPen( wxpen );
    graphicsOutput->StartLine( xLineStart_, yc );
    graphicsOutput->ContinueLine( xLineEnd_, yc, dc );
    graphicsOutput->SetLineType( lineTypeSave );
  }
  //
  // plot the symbols on the line segment
  //
  yc -= 0.5*textHeight;
  std::size_t numsym = symbols.size();
  if( numsym > 0 )
  {
    if( numsym > 1 )
    {
      double xinc = (xLineEnd_-xLineStart_)/(numsym-1.);
      for( std::size_t i=0; i<numsym-1; ++i )
      {
        symbols[i]->SetLocation( xLineStart_+i*xinc, yc );
        symbols[i]->Draw( graphicsOutput, dc );
      }
      symbols.back()->SetLocation( xLineEnd_, yc );
      symbols.back()->Draw( graphicsOutput, dc );
    }
    else
    {
      symbols.front()->SetLocation( (xLineStart_+xLineEnd_)/2.0, yc );
      symbols.front()->Draw( graphicsOutput, dc );
    }
  }
}

void GRA_legend::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  int lineTypeSave = graphicsOutput->GetLineType();
  //
  if( frame_ )frame_->Draw( graphicsOutput, dc );
  if( title_ )title_->Draw( graphicsOutput, dc );
  //
  std::size_t size = textVec_.size();
  for( std::size_t i=0; i<size; ++i )
  {
    textVec_[i]->Draw( graphicsOutput, dc ); // plot the text portion of the legend entry
    if( entryLines_[i] )                     // plot the line segment
    {
      double yc = textVec_[i]->GetY() + textVec_[i]->GetHeight();
      graphicsOutput->SetLineType( lineTypes_[i] );
      wxPen wxpen( dc.GetPen() );
      wxpen.SetWidth( lineWidths_[i] );
      wxpen.SetColour( ExGlobals::GetwxColor(colors_[i]) );
      dc.SetPen( wxpen );
      graphicsOutput->StartLine( xLineStart_, yc );
      graphicsOutput->ContinueLine( xLineEnd_, yc, dc );
    }
    std::vector<GRA_plotSymbol*> psv( symbols_[i] );
    std::vector<GRA_plotSymbol*>::const_iterator end = psv.end();
    for( std::vector<GRA_plotSymbol*>::const_iterator j=psv.begin(); j!=end; ++j )
      (*j)->Draw( graphicsOutput, dc );
  }
  graphicsOutput->SetLineType( lineTypeSave );
}

void GRA_legend::DrawFrameAndTitle( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  GRA_setOfCharacteristics *legendC =
      ExGlobals::GetGraphWindow()->GetGraphLegendCharacteristics();
  bool frameOn = static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("FRAMEON")))->Get();
  GRA_color *frameColor =
      static_cast<GRA_colorCharacteristic*>(legendC->Get(wxT("FRAMECOLOR")))->Get();
  if( frameOn ) // plot the frame
  {
    frame_ = new GRA_rectangle(xlo_,ylo_,xhi_,yhi_,0.0,false,frameColor,0,1);
    frame_->Draw( graphicsOutput, dc );
  }
  wxString titleString(
    static_cast<GRA_stringCharacteristic*>(legendC->Get(wxT("TITLE")))->Get() );
  double titleHeight =
      static_cast<GRA_sizeCharacteristic*>(legendC->Get(wxT("TITLEHEIGHT")))->GetAsWorld();
  GRA_font *titleFont =
      static_cast<GRA_fontCharacteristic*>(legendC->Get(wxT("TITLEFONT")))->Get();
  GRA_color *titleColor =
      static_cast<GRA_colorCharacteristic*>(legendC->Get(wxT("TITLECOLOR")))->Get();
  bool titleOn = static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("TITLEON")))->Get();
  if( titleOn && !titleString.empty() && titleHeight>0.0 ) // plot the title
  {
    double x = 0.5*(xhi_+xlo_);
    double y = yhi_ + 0.5*titleHeight;
    double angle = 0.0;
    title_ = new GRA_drawableText(titleString,titleHeight,angle,x,y,2,titleFont,titleColor);
    try
    {
      title_->Parse();
    }
    catch ( EGraphicsError &e )
    {
      delete title_;
      throw;
    }
    title_->Draw( graphicsOutput, dc );
  }
}

std::ostream &operator<<( std::ostream &out, GRA_legend const &legend )
{
  std::size_t size = legend.textVec_.size();
  out << "<graphlegend size=\"" << size << "\">\n";
  for( std::size_t i=0; i<size; ++i )
  {
    std::size_t size2 = legend.symbols_[i].size();
    out << "<entry line=\"" << legend.entryLines_[i] << "\" linetype=\""
        << legend.lineTypes_[i] << "\" linewidth=\"" << legend.lineWidths_[i]
        << "\" linecolor=\"" << GRA_colorControl::GetColorCode(legend.colors_[i])
        << "\" size=\"" << size2 << "\">\n"
        << "<string><![CDATA[" << legend.labels_[i].mb_str(wxConvUTF8) << "]]></string>\n";
    for( std::size_t j=0; j<size2; ++j )out << *legend.symbols_[i][j];
    out << "</entry>\n";
  }
  return out << "</graphlegend>\n";
}
// end of file
