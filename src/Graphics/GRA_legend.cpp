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
#include "GRA_intCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_wxWidgets.h"
#include "ExGlobals.h"
#include "UsefulFunctions.h"
#include "LegendPopup.h"

void GRA_legend::Initialize()
{
  GRA_setOfCharacteristics *legendC = graphWindow_->GetGraphLegendCharacteristics();
  xlo_ = static_cast<GRA_distanceCharacteristic*>(legendC->Get(wxT("FRAMEXLO")))->GetAsWorld();
  ylo_ = static_cast<GRA_distanceCharacteristic*>(legendC->Get(wxT("FRAMEYLO")))->GetAsWorld();
  xhi_ = static_cast<GRA_distanceCharacteristic*>(legendC->Get(wxT("FRAMEXHI")))->GetAsWorld();
  yhi_ = static_cast<GRA_distanceCharacteristic*>(legendC->Get(wxT("FRAMEYHI")))->GetAsWorld();
  xLineStart_ = xlo_ + (xhi_-xlo_)*0.05;
  xLineEnd_ = xlo_ + (xhi_-xlo_)*0.4;
  xLabel_ = xlo_ + (xhi_-xlo_)*0.5;
  MakeFrame();
  MakeTitle();
}

GRA_legend::~GRA_legend()
{
  DeleteStuff();
  if( popup_ )ExGlobals::DisconnectLegendPopup();
}

void GRA_legend::DeleteStuff()
{
  while ( !entries_.empty() )
  {
    delete entries_.back();
    entries_.pop_back();
  }
  delete frame_;
  delete title_;
}

void GRA_legend::CopyStuff( GRA_legend const &rhs )
{
  xlo_ = rhs.xlo_;
  ylo_ = rhs.ylo_;
  xhi_ = rhs.xhi_;
  yhi_ = rhs.yhi_;
  xLineStart_ = rhs.xLineStart_;
  xLineEnd_ = rhs.xLineEnd_;
  xLabel_ = rhs.xLabel_;
  //
  std::size_t size = rhs.entries_.size();
  for( std::size_t i=0; i<size; ++i )
    entries_[i] = new GRA_legendEntry(*rhs.entries_[i]);
  //
  frame_ = new GRA_rectangle( *rhs.frame_ );
  title_ = new GRA_drawableText( *rhs.title_ );
  //
  popup_ = rhs.popup_;
}

void GRA_legend::AddEntry( wxString const &label )
{
  GRA_setOfCharacteristics *dataC = graphWindow_->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *legendC = graphWindow_->GetGraphLegendCharacteristics();
  GRA_setOfCharacteristics *textC = graphWindow_->GetTextCharacteristics();
  //
  GRA_intCharacteristic *plotSymbol =
    static_cast<GRA_intCharacteristic*>(dataC->Get(wxT("PLOTSYMBOL")));
  GRA_sizeCharacteristic *pSize =
    static_cast<GRA_sizeCharacteristic*>(dataC->Get(wxT("PLOTSYMBOLSIZE")));
  GRA_angleCharacteristic *pAngle =
    static_cast<GRA_angleCharacteristic*>(dataC->Get(wxT("PLOTSYMBOLANGLE")));
  GRA_colorCharacteristic *pColor =
    static_cast<GRA_colorCharacteristic*>(dataC->Get(wxT("PLOTSYMBOLCOLOR")));
  GRA_intCharacteristic *pLineWidth =
    static_cast<GRA_intCharacteristic*>(dataC->Get(wxT("PLOTSYMBOLLINEWIDTH")));
  //
  int sym = plotSymbol->IsVector() ? plotSymbol->Gets().at(0) : plotSymbol->Get();
  double symSize = pSize->IsVector() ? pSize->GetAsWorlds().at(0) : pSize->GetAsWorld();
  double symAngle = pAngle->IsVector() ? pAngle->Gets().at(0) : pAngle->Get();
  GRA_color* symColor = pColor->IsVector() ? pColor->Gets().at(0) : pColor->Get();
  int symLW = pLineWidth->IsVector() ? pLineWidth->Gets().at(0) : pLineWidth->Get();
  //
  int nSymbols = static_cast<GRA_intCharacteristic*>(legendC->Get(wxT("SYMBOLS")))->Get();
  bool drawLineSegment = static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("ENTRYLINEON")))->Get();
  int lineType = static_cast<GRA_intCharacteristic*>(dataC->Get(wxT("CURVELINETYPE")))->Get();
  int lineWidth = static_cast<GRA_intCharacteristic*>(dataC->Get(wxT("CURVELINEWIDTH")))->Get();
  GRA_color *lineColor = static_cast<GRA_colorCharacteristic*>(dataC->Get(wxT("CURVECOLOR")))->Get();
  //
  double labelHeight = static_cast<GRA_sizeCharacteristic*>(textC->Get(wxT("HEIGHT")))->GetAsWorld();
  GRA_font *labelFont = static_cast<GRA_fontCharacteristic*>(textC->Get(wxT("FONT")))->Get();
  GRA_color *labelColor = static_cast<GRA_colorCharacteristic*>(textC->Get(wxT("COLOR")))->Get();
  //
  // now create the legend entry: line segment with plotsymbols and text label
  //
  GRA_legendEntry *entry = 0;
  try
  {
    entry = new GRA_legendEntry( this, label, labelHeight, labelFont, labelColor,
                                 nSymbols, sym, symSize, symAngle, symColor, symLW,
                                 drawLineSegment, lineType, lineWidth, lineColor );
  }
  catch (EGraphicsError const &e)
  {
    delete entry;
    throw;
  }
  GRA_wxWidgets *graphicsOutput = ExGlobals::GetGraphicsOutput();
  wxClientDC dc( ExGlobals::GetwxWindow() ); 
  if( entries_.empty() )
  {
    if( static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("FRAMEON")))->Get() )
      frame_->Draw( graphicsOutput, dc );
    if( static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("TITLEON")))->Get() )
      title_->Draw( graphicsOutput, dc );
  }
  entry->Draw( graphicsOutput, dc );
  entries_.push_back( entry );
}

void GRA_legend::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  GRA_setOfCharacteristics *legendC = graphWindow_->GetGraphLegendCharacteristics();
  if( static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("ON")))->Get() )
  {
    if( static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("FRAMEON")))->Get() )
      frame_->Draw( graphicsOutput, dc );
    if( static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("TITLEON")))->Get() )
      title_->Draw( graphicsOutput, dc );
    std::vector<GRA_legendEntry*>::const_iterator end = entries_.end();
    for( std::vector<GRA_legendEntry*>::const_iterator i=entries_.begin(); i!=end; ++i )
      (*i)->Draw( graphicsOutput, dc );
  }
}

void GRA_legend::MakeFrame()
{
  if( frame_ )
  {
    delete frame_;
    frame_ = 0;
  }
  GRA_setOfCharacteristics *legendC = graphWindow_->GetGraphLegendCharacteristics();
  GRA_color *frameColor = static_cast<GRA_colorCharacteristic*>(legendC->Get(wxT("FRAMECOLOR")))->Get();
  frame_ = new GRA_rectangle( xlo_, ylo_, xhi_, yhi_, 0.0, false, frameColor, 0, 1 );
}

void GRA_legend::MakeTitle()
{
  if( title_ )
  {
    delete title_;
    title_ = 0;
  }
  GRA_setOfCharacteristics *legendC = graphWindow_->GetGraphLegendCharacteristics();
  wxString titleString( static_cast<GRA_stringCharacteristic*>(legendC->Get(wxT("TITLE")))->Get() );
  if( titleString.empty() )titleString = wxT(" ");
  double titleHeight = static_cast<GRA_sizeCharacteristic*>(legendC->Get(wxT("TITLEHEIGHT")))->GetAsWorld();
  GRA_font *titleFont = static_cast<GRA_fontCharacteristic*>(legendC->Get(wxT("TITLEFONT")))->Get();
  GRA_color *titleColor = static_cast<GRA_colorCharacteristic*>(legendC->Get(wxT("TITLECOLOR")))->Get();
  title_ = new GRA_drawableText( titleString, titleHeight, 0.0,
                                 0.5*(xhi_+xlo_), yhi_+0.1*titleHeight, 2,
                                 titleFont, titleColor );
  try
  {
    title_->Parse();
  }
  catch (EGraphicsError const &e)
  {
    delete title_;
    title_ = 0;
    throw;
  }
}

void GRA_legend::SetFrame( GRA_rectangle *frame )
{
  delete frame_;
  frame_ = new GRA_rectangle(*frame);
}

void GRA_legend::SetTitle( GRA_drawableText *title )
{ 
  delete title_;
  title_ = new GRA_drawableText(*title);
  GRA_setOfCharacteristics *legendC = graphWindow_->GetGraphLegendCharacteristics();
  static_cast<GRA_stringCharacteristic*>(legendC->Get(wxT("TITLE")))->Set( title_->GetString() );
}

bool GRA_legend::Inside( double x, double y )
{
  std::vector<double> xp(4,0.0), yp(4,0.0);
  double const eps = 0.05;
  xp[0] = xlo_ - eps;
  yp[0] = ylo_ - eps;
  xp[1] = xhi_ + eps;
  yp[1] = ylo_ - eps;
  xp[2] = xhi_ + eps;
  yp[2] = yhi_ + eps;
  xp[3] = xlo_ - eps;
  yp[3] = yhi_ + eps;
  return UsefulFunctions::InsidePolygon(x,y,xp,yp);
}

std::ostream &operator<<( std::ostream &out, GRA_legend const &legend )
{
  GRA_setOfCharacteristics *legendC = legend.graphWindow_->GetGraphLegendCharacteristics();
  std::size_t size = legend.entries_.size();
  out << "<graphlegend size=\"" << size
      << "\" frameon=\""
      << (static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("FRAMEON")))->Get()?1L:0L)
      << "\" titleon=\""
      << (static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("TITLEON")))->Get()?1L:0L)
      << "\" xlo=\"" << legend.xlo_ << "\" ylo=\"" << legend.ylo_
      << "\" xhi=\"" << legend.xhi_ << "\" yhi=\"" << legend.yhi_
      << "\">\n";
  for( std::size_t i=0; i<size; ++i )out << *legend.entries_[i];
  out << *legend.frame_ << *legend.title_;
  return out << "</graphlegend>\n";
}

//---------------- GRA_legendEntry ---------------------------------------------

GRA_legendEntry::GRA_legendEntry( GRA_legend *legend, wxString const &label,
                                  double labelHeight, GRA_font *labelFont, GRA_color *labelColor,
                                  int nSymbols, int sym, double symSize, double symAngle,
                                  GRA_color *symColor, int symLW, bool drawLineSegment,
                                  int lineType, int lineWidth, GRA_color *lineColor )
  : legend_(legend), nSymbols_(nSymbols), drawLineSegment_(drawLineSegment), lineType_(lineType),
    lineWidth_(lineWidth), lineColor_(lineColor), labelHeight_(labelHeight),
    labelFont_(labelFont), labelColor_(labelColor)
{
  entryNumber_ = legend_->GetNumberOfEntries();
  double y0 = legend_->GetYHi() - 1.5*labelHeight_*(entryNumber_+1);
  double xLabel = legend->GetXLabel();
  symbol_ = new GRA_plotSymbol(sym,symSize,symAngle,symColor,symLW);
  label_ = new GRA_drawableText(label,labelHeight_,0.0,xLabel,y0,1,labelFont_,labelColor_);
  try
  {
    label_->Parse();
  }
  catch (EGraphicsError const &e)
  {
    throw;
  }
}

void GRA_legendEntry::DeleteStuff()
{
  delete symbol_;
  delete label_;
}

void GRA_legendEntry::CopyStuff( GRA_legendEntry const &rhs )
{
  legend_ = rhs.legend_;
  label_ = new GRA_drawableText(*rhs.label_);
  symbol_ = new GRA_plotSymbol(*rhs.symbol_);
  entryNumber_ = rhs.entryNumber_;
  nSymbols_ = rhs.nSymbols_;
  drawLineSegment_ = rhs.drawLineSegment_;
  labelHeight_ = rhs.labelHeight_;
  lineType_ = rhs.lineType_;
  lineWidth_ = rhs.lineWidth_;
  lineColor_ = rhs.lineColor_;
  labelColor_ = rhs.labelColor_;
  labelFont_ = rhs.labelFont_;
}

void GRA_legendEntry::Draw( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  double y0 = legend_->GetYHi() - 1.5*labelHeight_*(entryNumber_+1);
  // first draw the line segment
  //
  double y = y0 + labelHeight_*0.5;
  double xStart = legend_->GetLineStart();
  double xEnd = legend_->GetLineEnd();
  double xLabel = legend_->GetXLabel();
  if( drawLineSegment_ )
  {
    int lineTypeSave = graphicsOutput->GetLineType();
    graphicsOutput->SetLineType( lineType_ );
    wxPen wxpen( dc.GetPen() );
    wxpen.SetWidth( lineWidth_ );
    wxpen.SetColour( ExGlobals::GetwxColor(lineColor_) );
    dc.SetPen( wxpen );
    graphicsOutput->StartLine( xStart, y );
    graphicsOutput->ContinueLine( xEnd, y, dc );
    graphicsOutput->SetLineType( lineTypeSave );
  }
  // draw the symbols on the line segment
  //
  if( nSymbols_ > 0 )
  {
    if( nSymbols_ > 1 )
    {
      double xinc = (xEnd-xStart)/(nSymbols_-1.);
      for( int i=0; i<nSymbols_-1; ++i )
      {
        symbol_->SetLocation( xStart+i*xinc, y );
        symbol_->Draw( graphicsOutput, dc );
      }
      symbol_->SetLocation( xEnd, y );
      symbol_->Draw( graphicsOutput, dc );
    }
    else
    {
      symbol_->SetLocation( (xStart+xEnd)/2.0, y );
      symbol_->Draw( graphicsOutput, dc );
    }
  }
  // draw the label at the end of the line segment
  //
  label_->SetX( xLabel );
  label_->SetY( y0-0.25*labelHeight_ );
  label_->Draw( graphicsOutput, dc );
}

std::ostream &operator<<( std::ostream &out, GRA_legendEntry const &entry )
{
  return out << "<legendentry nsymbols=\"" << entry.nSymbols_
             << "\" line=\"" << (entry.drawLineSegment_?1:0)
             << "\" linetype=\"" << entry.lineType_
             << "\" linewidth=\"" << entry.lineWidth_
             << "\" linecolor=\"" << GRA_colorControl::GetColorCode(entry.lineColor_)
             << "\">\n"
             << *entry.label_
             << *entry.symbol_
             << "</legendentry>\n";
}

// end of file
