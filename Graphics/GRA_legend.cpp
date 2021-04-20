/*
Copyright (C) 2005,...,2010 Joseph L. Chuma

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

#include "GRA_legend.h"
#include "ExGlobals.h"
#include "GRA_window.h"
#include "GRA_color.h"
#include "GRA_font.h"
#include "GRA_drawableText.h"
#include "GRA_plotSymbol.h"
#include "GRA_rectangle.h"
#include "EGraphicsError.h"
#include "GRA_setOfCharacteristics.h"
#include "UsefulFunctions.h"
#include "LegendPopup.h"

void GRA_legend::Initialize()
{
  GRA_setOfCharacteristics *legendC = graphWindow_->GetGraphLegendCharacteristics();
  xlo_ = static_cast<GRA_distanceCharacteristic*>(legendC->Get("FRAMEXLO"))->GetAsWorld();
  ylo_ = static_cast<GRA_distanceCharacteristic*>(legendC->Get("FRAMEYLO"))->GetAsWorld();
  xhi_ = static_cast<GRA_distanceCharacteristic*>(legendC->Get("FRAMEXHI"))->GetAsWorld();
  yhi_ = static_cast<GRA_distanceCharacteristic*>(legendC->Get("FRAMEYHI"))->GetAsWorld();;
  xLineStart_ = xlo_ + (xhi_-xlo_)*0.05;
  xLineEnd_ = xlo_ + (xhi_-xlo_)*0.4;
  xLabel_ = xlo_ + (xhi_-xlo_)*0.5;
  //
  MakeFrame();
  MakeTitle();
}

GRA_legend::~GRA_legend()
{
  DeleteStuff();
  if( popup_ )LegendPopupForm->Disconnect();
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

void GRA_legend::AddEntry( ExString const &label )
{
  GRA_setOfCharacteristics *dataC = graphWindow_->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *legendC = graphWindow_->GetGraphLegendCharacteristics();
  GRA_setOfCharacteristics *textC = graphWindow_->GetTextCharacteristics();
  //
  GRA_intCharacteristic *plotSymbol =
    static_cast<GRA_intCharacteristic*>(dataC->Get("PLOTSYMBOL"));
  GRA_sizeCharacteristic *pSize =
    static_cast<GRA_sizeCharacteristic*>(dataC->Get("PLOTSYMBOLSIZE"));
  GRA_angleCharacteristic *pAngle =
    static_cast<GRA_angleCharacteristic*>(dataC->Get("PLOTSYMBOLANGLE"));
  GRA_colorCharacteristic *pColor =
    static_cast<GRA_colorCharacteristic*>(dataC->Get("PLOTSYMBOLCOLOR"));
  GRA_intCharacteristic *pLineWidth =
    static_cast<GRA_intCharacteristic*>(dataC->Get("PLOTSYMBOLLINEWIDTH"));
  //
  int sym = plotSymbol->IsVector() ? plotSymbol->Gets().at(0) : plotSymbol->Get();
  double symSize = pSize->IsVector() ? pSize->GetAsWorlds().at(0) : pSize->GetAsWorld();
  double symAngle = pAngle->IsVector() ? pAngle->Gets().at(0) : pAngle->Get();
  GRA_color* symColor = pColor->IsVector() ? pColor->Gets().at(0) : pColor->Get();
  int symLW = pLineWidth->IsVector() ? pLineWidth->Gets().at(0) : pLineWidth->Get();
  //
  int nSymbols = static_cast<GRA_intCharacteristic*>(legendC->Get("SYMBOLS"))->Get();
  bool drawLineSegment = static_cast<GRA_boolCharacteristic*>(legendC->Get("ENTRYLINEON"))->Get();
  int lineType = static_cast<GRA_intCharacteristic*>(dataC->Get("CURVELINETYPE"))->Get();
  int lineWidth = static_cast<GRA_intCharacteristic*>(dataC->Get("CURVELINEWIDTH"))->Get();
  GRA_color *lineColor = static_cast<GRA_colorCharacteristic*>(dataC->Get("CURVECOLOR"))->Get();
  //
  double labelHeight = static_cast<GRA_sizeCharacteristic*>(textC->Get("HEIGHT"))->GetAsWorld();
  GRA_font *labelFont = static_cast<GRA_fontCharacteristic*>(textC->Get("FONT"))->Get();
  GRA_color *labelColor = static_cast<GRA_colorCharacteristic*>(textC->Get("COLOR"))->Get();
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
  //
  GRA_outputType *graphicsOutput = ExGlobals::GetScreenOutput();
  if( entries_.empty() )
  {
    if( static_cast<GRA_boolCharacteristic*>(legendC->Get("FRAMEON"))->Get() )
      graphicsOutput->Draw( frame_ );
    if( static_cast<GRA_boolCharacteristic*>(legendC->Get("TITLEON"))->Get() )
      graphicsOutput->Draw( title_ );
  }
  entry->Draw( graphicsOutput );
  entries_.push_back( entry );
}

void GRA_legend::Draw( GRA_outputType *graphicsOutput )
{
  GRA_setOfCharacteristics *legendC = graphWindow_->GetGraphLegendCharacteristics();
//  if( static_cast<GRA_boolCharacteristic*>(legendC->Get("ON"))->Get() )
//  {
  if( static_cast<GRA_boolCharacteristic*>(legendC->Get("FRAMEON"))->Get() )graphicsOutput->Draw( frame_ );
  if( static_cast<GRA_boolCharacteristic*>(legendC->Get("TITLEON"))->Get() )graphicsOutput->Draw( title_ );
  std::vector<GRA_legendEntry*>::const_iterator end = entries_.end();
  for( std::vector<GRA_legendEntry*>::const_iterator i=entries_.begin(); i!=end; ++i )
    (*i)->Draw( graphicsOutput );
//  }
}

void GRA_legend::MakeFrame()
{
  if( frame_ )
  {
    delete frame_;
    frame_ = 0;
  }
  GRA_setOfCharacteristics *legendC = graphWindow_->GetGraphLegendCharacteristics();
  GRA_color *frameColor = static_cast<GRA_colorCharacteristic*>(legendC->Get("FRAMECOLOR"))->Get();
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
  ExString titleString( static_cast<GRA_stringCharacteristic*>(legendC->Get("TITLE"))->Get() );
  if( titleString.empty() )titleString = " ";
  double titleHeight = static_cast<GRA_sizeCharacteristic*>(legendC->Get("TITLEHEIGHT"))->GetAsWorld();
  GRA_font *titleFont = static_cast<GRA_fontCharacteristic*>(legendC->Get("TITLEFONT"))->Get();
  GRA_color *titleColor = static_cast<GRA_colorCharacteristic*>(legendC->Get("TITLECOLOR"))->Get();
  title_ = new GRA_drawableText( titleString, titleHeight, 0.0,
                                 0.5*(xhi_+xlo_), yhi_+0.1*titleHeight, 2,
                                 titleFont, titleColor );
  try
  {
    title_->Parse();
  }
  catch ( EGraphicsError &e )
  {
    delete title_;
    title_ = 0;
    throw;
  }
}

void GRA_legend::SetFrame( GRA_rectangle *frame )
{ *frame_ = *frame; }

void GRA_legend::SetTitle( GRA_drawableText *title )
{
  delete title_;
  title_ = new GRA_drawableText(*title);
  GRA_setOfCharacteristics *legendC = graphWindow_->GetGraphLegendCharacteristics();
  static_cast<GRA_stringCharacteristic*>(legendC->Get("TITLE"))->Set( title_->GetString() );
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
      << "\" frameon=\"" << static_cast<GRA_boolCharacteristic*>(legendC->Get("FRAMEON"))->Get()
      << "\" titleon=\"" << static_cast<GRA_boolCharacteristic*>(legendC->Get("TITLEON"))->Get()
      << "\" xlo=\"" << legend.xlo_ << "\" ylo=\"" << legend.ylo_
      << "\" xhi=\"" << legend.xhi_ << "\" yhi=\"" << legend.yhi_
      << "\">\n";
  for( std::size_t i=0; i<size; ++i )out << *legend.entries_[i];
  return out << *legend.frame_ << *legend.title_ << "</graphlegend>\n";
}

//---------------- GRA_legendEntry ---------------------------------------------

GRA_legendEntry::GRA_legendEntry( GRA_legend *legend, ExString const &label,
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
    delete label_;
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

void GRA_legendEntry::Draw( GRA_outputType *graphicsOutput )
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
    //int lineTypeSave = graphicsOutput->GetLineType();
    //int lineWidthSave = graphicsOutput->GetLineWidth();
    GRA_color *colorSave = graphicsOutput->GetColor();
    graphicsOutput->SetLineType( lineType_ );
    graphicsOutput->SetLineWidth( lineWidth_ );
    graphicsOutput->SetColor( lineColor_ );
    graphicsOutput->StartLine( xStart, y );
    graphicsOutput->ContinueLine( xEnd, y );
    //graphicsOutput->SetLineType( lineTypeSave );
    //graphicsOutput->SetLineWidth( lineWidthSave );
    graphicsOutput->SetColor( colorSave );
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
        graphicsOutput->Draw( symbol_ );
      }
      symbol_->SetLocation( xEnd, y );
      graphicsOutput->Draw( symbol_ );
    }
    else
    {
      symbol_->SetLocation( (xStart+xEnd)/2.0, y );
      graphicsOutput->Draw( symbol_ );
    }
  }
  // draw the label at the end of the line segment
  //
  label_->SetX( xLabel );
  label_->SetY( y0 );
  graphicsOutput->Draw( label_ );
}

std::ostream &operator<<( std::ostream &out, GRA_legendEntry const &entry )
{
  return out << "<legendentry nsymbols=\"" << entry.nSymbols_
      << "\" line=\"" << entry.drawLineSegment_
      << "\" linetype=\"" << entry.lineType_
      << "\" linewidth=\"" << entry.lineWidth_
      << "\" linecolor=\"" << GRA_colorControl::GetColorCode(entry.lineColor_)
      << "\">\n"
      << *entry.label_
      << *entry.symbol_
      << "</legendentry>\n";
}

// end of file
