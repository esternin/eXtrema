/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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

#include "GRA_page.h"
#include "EGraphicsError.h"
#include "ExGlobals.h"
#include "GRA_window.h"
#include "GRA_drawableObject.h"
#include "GRA_cartesianAxes.h"
#include "GRA_cartesianCurve.h"
#include "GRA_axis.h"
#include "GRA_legend.h"
#include "MainGraphicsWindow.h"
#include "GRA_borlandMonitor.h"
#include "GraphicsSubwindow.h"
#include "GRA_arrow1.h"
#include "GRA_arrow2.h"
#include "GRA_arrow3.h"
#include "GRA_polygon.h"
#include "GRA_ellipse.h"
#include "GRA_rectangle.h"
#include "GRA_star5pt.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_colorCharacteristic.h"
#include "AxisPopup.h"
#include "CurvePopup.h"
#include "TextPopup.h"
#include "LegendPopup.h"

GRA_page::GRA_page( TPageControl *pc ) : TTabSheet(pc)
{
  int xmin, ymin, xmax, ymax;
  ExGlobals::GetMonitorLimits( xmin, ymin, xmax, ymax );

  PageControl = pc;
  Caption = AnsiString("Page ")+IntToStr(PageIndex+1);

  image_ = new TImage( this );

  image_->OnMouseDown = ImageMouseDown;
  image_->OnMouseMove = ImageMouseMove;
  image_->OnMouseUp = ImageMouseUp;

  image_->Cursor = crCross;

  image_->Canvas->Font->Name = AnsiString("Arial");
  image_->Canvas->Font->Color = clBlack;
  image_->Canvas->Font->Charset = DEFAULT_CHARSET;

  image_->Name = AnsiString("Page")+IntToStr(PageIndex+1);
  image_->Parent = this;
  image_->Anchors = TAnchors() << akLeft << akTop << akRight << akBottom;
  image_->Center = true;
  image_->Left = 2;
  image_->Top = 2;
  image_->Height = ymax-ymin+120;
  image_->Width = xmax-xmin+50;
  image_->Show();
  image_->Canvas->CopyMode = cmWhiteness;
  TRect aRect( xmin, ymin, xmax, ymax );
  image_->Canvas->CopyRect( aRect, image_->Canvas, aRect );
  image_->Canvas->CopyMode = cmSrcCopy;

  SetUpDefaultWindows();

  mouseDown_ = false;
  firstPoint_ = true;
  interactiveWindowMode_ = false;
  interactiveLegendMode_ = false;
  textPlacementMode_ = false;
  arrowPlacementMode_ = false;
  polygonPlacementMode_ = false;
  ellipsePlacementMode_ = false;
  arrowType_ = 1;
  polygonType_ = 1;
  polygonAngle_ = 0.0;
  headsBothEnds_ = false;
  drawCircles_ = false;
  textToPlace_ = 0;
  currentPolygon_ = 0;
  currentArrow1_ = 0;
  currentArrow2_ = 0;
  currentArrow3_ = 0;
  currentEllipse_ = 0;
}

__fastcall GRA_page::~GRA_page()
{
  DeleteGraphWindows();
  delete image_;
}

void GRA_page::DeleteGraphWindows()
{
  while( !graphWindows_.empty() )
  {
    delete graphWindows_.back();
    graphWindows_.pop_back();
  }
}

std::ostream &operator<<( std::ostream &out, GRA_page const &gp )
{
  out << "<graphicspage currentwindow=\"" << gp.currentWindowNumber_ << "\">\n";
  for( std::size_t i=0; i<gp.graphWindows_.size(); ++i )out << *gp.graphWindows_[i];
  return out << "</graphicspage>\n";
}

void GRA_page::InheritFrom( GRA_page const *mPage )
{
  DeleteGraphWindows();
  int mWindows = mPage->graphWindows_.size();
  for( int i=0; i<mWindows; ++i )
  {
    GRA_window *mWindow = const_cast<GRA_page*>(mPage)->GetGraphWindow(i);
    double xlo, ylo, xhi, yhi;
    mWindow->GetDimensions( xlo, ylo, xhi, yhi );
    graphWindows_.push_back( new GRA_window(i,xlo,ylo,xhi,yhi) );
    graphWindows_.back()->InheritFrom( mWindow );
  }
}

void GRA_page::SetUpDefaultWindows()
{
  DeleteGraphWindows();
  currentWindowNumber_ = 0;
  graphWindows_.push_back( new GRA_window(  0,  0.0,  0.0, 100.0, 100.0 ) );
  graphWindows_.push_back( new GRA_window(  1,  0.0,  0.0,  50.0, 100.0 ) );
  graphWindows_.push_back( new GRA_window(  2, 50.0,  0.0, 100.0, 100.0 ) );
  graphWindows_.push_back( new GRA_window(  3,  0.0, 50.0, 100.0, 100.0 ) );
  graphWindows_.push_back( new GRA_window(  4,  0.0,  0.0, 100.0,  50.0 ) );
  graphWindows_.push_back( new GRA_window(  5,  0.0, 50.0,  50.0, 100.0 ) );
  graphWindows_.push_back( new GRA_window(  6,  0.0,  0.0,  50.0,  50.0 ) );
  graphWindows_.push_back( new GRA_window(  7, 50.0, 50.0, 100.0, 100.0 ) );
  graphWindows_.push_back( new GRA_window(  8, 50.0,  0.0, 100.0,  50.0 ) );
  graphWindows_.push_back( new GRA_window(  9, 10.0, 10.0,  90.0,  90.0 ) );
  graphWindows_.push_back( new GRA_window( 10, 25.0, 50.0,  75.0, 100.0 ) );
  graphWindows_.push_back( new GRA_window( 11, 25.0,  0.0,  75.0,  50.0 ) );
  graphWindows_.push_back( new GRA_window( 12,  0.0, 75.0,  50.0, 100.0 ) );
  graphWindows_.push_back( new GRA_window( 13, 50.0, 75.0, 100.0, 100.0 ) );
  graphWindows_.push_back( new GRA_window( 14,  0.0, 50.0,  50.0,  75.0 ) );
  graphWindows_.push_back( new GRA_window( 15, 50.0, 50.0, 100.0,  75.0 ) );
}

void GRA_page::Reset()
{
  int xmin, ymin, xmax, ymax;
  ExGlobals::GetMonitorLimits( xmin, ymin, xmax, ymax );
  int height = ymax - ymin;
  int width = xmax - xmin;
  image_->Height = height;
  image_->Width = width;
  Graphics::TBitmap *formImage = MainGraphicsForm->GetFormImage(); // this is the whole form
  formImage->Height = height;
  formImage->Width = width;
  image_->Picture->Assign(formImage);
  delete formImage;
  static_cast<GRA_borlandMonitor*>(ExGlobals::GetScreenOutput())->SetCanvas(image_->Canvas);
}

TImage *GRA_page::GetImage() const
{ return image_; }

void GRA_page::AddGraphWindow( GRA_window *gw )
{
  int n = gw->GetNumber();
  int maxWindow = graphWindows_.size();
  if( n >= maxWindow )
  {
    int i = maxWindow-1;
    while( ++i < n )graphWindows_.push_back( new GRA_window(i) );
    graphWindows_.push_back( gw );
  }
  else
  {
    delete graphWindows_[n];
    graphWindows_[n] = gw;
  }
}

void GRA_page::SetGraphWindow( GRA_window *gw )
{
  MainGraphicsForm->SetPage( this );
  currentWindowNumber_ =
    std::distance( graphWindows_.begin(),
                   std::find(graphWindows_.begin(),graphWindows_.end(),gw) );
}

GRA_window *GRA_page::GetGraphWindow()
{ return graphWindows_[currentWindowNumber_]; }

GRA_window *GRA_page::GetGraphWindow( int n )
{
  GRA_window *gw = 0;
  if( n < static_cast<int>(graphWindows_.size()) )gw = graphWindows_[n];
  return gw;
}

GRA_window *GRA_page::GetGraphWindow( double x, double y )
{
  std::vector<GRA_window*>::const_iterator GWend = graphWindows_.end();
  for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=GWend; ++i )
    if( (*i)->InsideWindow(x,y) )return *i;
  return 0;
}

void GRA_page::SetWindowNumber( int n )
{ currentWindowNumber_ = n; }

int GRA_page::GetWindowNumber()
{ return currentWindowNumber_; }

int GRA_page::GetNumberOfWindows()
{ return static_cast<int>(graphWindows_.size()); }

std::vector<GRA_window*> &GRA_page::GetGraphWindows()
{ return graphWindows_; }

void GRA_page::DrawGraphWindows( GRA_outputType *outputType )
{
  GRA_window *gw = graphWindows_[currentWindowNumber_];
  try
  {
    std::vector<GRA_window*>::const_iterator end = graphWindows_.end();
    for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=end; ++i )
    {
      SetGraphWindow( *i );
      (*i)->Draw( outputType );
    }
  }
  catch (EGraphicsError &e)
  {
    SetGraphWindow( gw );
    throw;
  }
  SetGraphWindow( gw );
}

void GRA_page::DisplayBackgrounds( GRA_outputType *outputType )
{
  std::vector<GRA_window*>::const_iterator end = graphWindows_.end();
  for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=end; ++i )
    (*i)->DisplayBackground( outputType );
}

void GRA_page::EraseWindows()
{
  std::vector<GRA_window*>::const_iterator end = graphWindows_.end();
  for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=end; ++i )
    (*i)->Erase();
}

void GRA_page::ResetWindows()
{
  std::vector<GRA_window*>::const_iterator end = graphWindows_.end();
  for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=end; ++i )
    (*i)->Reset();
}

void GRA_page::ClearWindows()
{
  // destroy all drawableObjects in all graph windows
  //
  std::vector<GRA_window*>::const_iterator gwEnd = graphWindows_.end();
  for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=gwEnd; ++i )
    (*i)->Clear();
}

void GRA_page::SetWindowsDefaults()
{
  // reset defaults in all graph windows
  //
  std::vector<GRA_window*>::const_iterator gwEnd = graphWindows_.end();
  for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=gwEnd; ++i )
    (*i)->SetDefaults();
}

void GRA_page::ReplotAllWindows()
{
  std::size_t const nSave = currentWindowNumber_;
  std::size_t const size = graphWindows_.size();
  try
  {
    for( std::size_t i=0; i<size; ++i )
    {
      currentWindowNumber_ = i;
      ReplotCurrentWindow();
    }
  }
  catch (EGraphicsError &e)
  {
    currentWindowNumber_ = nSave;
    throw;
  }
  currentWindowNumber_ = nSave;
}

void GRA_page::ReplotCurrentWindow()
{
  if( graphWindows_[currentWindowNumber_]->GetDrawableObjects().empty() )return;
  graphWindows_[currentWindowNumber_]->Erase();
  try
  {
    graphWindows_[currentWindowNumber_]->Replot();
  }
  catch (EGraphicsError &e)
  {
    throw;
  }
}

void __fastcall GRA_page::ImageMouseMove( TObject *Sender, TShiftState Shift, int x, int y )
{
  GRA_window *gw = graphWindows_[currentWindowNumber_];
  GRA_borlandMonitor *monitor =
    static_cast<GRA_borlandMonitor*>(ExGlobals::GetScreenOutput());
  double xw, yw, xg, yg, xp, yp;
  try
  {
    // unitsType:  0 = graph, 1 = world, 2 = percentages, 3 = pixels
    AnsiString s1, s2;
    switch ( MainGraphicsForm->GetUnitsType() )
    {
      case 0:                // graph units
        monitor->OutputTypeToWorld( x, y, xw, yw );
        gw->WorldToGraph( xw, yw, xg, yg, true );
        s1 = AnsiString("x = ") + AnsiString(xg);
        s2 = AnsiString("y = ") + AnsiString(yg);
        break;
      case 1:                // world units
        monitor->OutputTypeToWorld( x, y, xw, yw );
        s1 = AnsiString("x = ") + AnsiString(xw);
        s2 = AnsiString("y = ") + AnsiString(yw);
        break;
      case 2:                // percentages
        monitor->OutputTypeToWorld( x, y, xw, yw );
        gw->WorldToPercent( xw, yw, xp, yp );
        s1 = AnsiString("%x = ") + AnsiString(xp);
        s2 = AnsiString("%y = ") + AnsiString(yp);
        break;
      default:                // pixels
        s1 = AnsiString("x = ") + AnsiString(x);
        s2 = AnsiString("y = ") + AnsiString(y);
        break;
    }
    MainGraphicsForm->SetStatusBarText( s1, s2 );
  }
  catch (EGraphicsError &e)
  {
    // space filler
  }
  if( mouseDown_ )
  {
    if( interactiveWindowMode_ || interactiveLegendMode_ )
    {
      if( firstPoint_ )
      {
        firstPoint_ = false;
        image_->Canvas->Pen->Mode = pmNotXor;
      }
      else
      {
        image_->Canvas->MoveTo( xWStart_, yWStart_ );
        image_->Canvas->LineTo( xLast_, yWStart_ );
        image_->Canvas->LineTo( xLast_, yLast_ );
        image_->Canvas->LineTo( xWStart_, yLast_ );
        image_->Canvas->LineTo( xWStart_, yWStart_ );
      }
      image_->Canvas->MoveTo( xWStart_, yWStart_ );
      image_->Canvas->LineTo( x, yWStart_ );
      image_->Canvas->LineTo( x, y );
      image_->Canvas->LineTo( xWStart_, y );
      image_->Canvas->LineTo( xWStart_, yWStart_ );
      xLast_ = x;
      yLast_ = y;
    }
    else if( arrowPlacementMode_ )
    {
      if( firstPoint_ )
      {
        firstPoint_ = false;
        image_->Canvas->Pen->Mode = pmNotXor;
      }
      else
      {
        switch( arrowType_ )
        {
          case 1:
            currentArrow1_->Draw( monitor );
            delete currentArrow1_;
            break;
          case 2:
            currentArrow2_->Draw( monitor );
            delete currentArrow2_;
            break;
          case 3:
            currentArrow3_->Draw( monitor );
            delete currentArrow3_;
            break;
        }
      }
      monitor->OutputTypeToWorld( x, y, xw, yw );
      GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
      GRA_setOfCharacteristics *dataCurve = gw->GetDataCurveCharacteristics();
      GRA_color *lc = static_cast<GRA_colorCharacteristic*>(dataCurve->Get("CURVECOLOR"))->Get();
      GRA_colorCharacteristic *fcChar = static_cast<GRA_colorCharacteristic*>(general->Get("AREAFILLCOLOR"));
      GRA_color *fc = fcChar->IsVector() ? fcChar->Gets().at(0) : fcChar->Get();
      int lw = static_cast<GRA_intCharacteristic*>(general->Get("LINEWIDTH"))->Get();
      double widthFactor = static_cast<GRA_doubleCharacteristic*>(general->Get("ARROWHEADWIDTH"))->Get();
      double lengthFactor = static_cast<GRA_doubleCharacteristic*>(general->Get("ARROWHEADLENGTH"))->Get();
      switch( arrowType_ )
      {
        case 1:
          currentArrow1_ = new GRA_arrow1(xw,yw,xw1_,yw1_,headsBothEnds_,lc,fc,lw,widthFactor,lengthFactor);
          currentArrow1_->Draw( monitor );
          break;
        case 2:
          currentArrow2_ = new GRA_arrow2(xw,yw,xw1_,yw1_,headsBothEnds_,lc,fc,lw,widthFactor,lengthFactor);
          currentArrow2_->Draw( monitor );
          break;
        case 3:
          currentArrow3_ = new GRA_arrow3(xw,yw,xw1_,yw1_,headsBothEnds_,lc,lw,widthFactor,lengthFactor);
          currentArrow3_->Draw( monitor );
          break;
      }
    }
    else if( polygonPlacementMode_ )
    {
      if( firstPoint_ )
      {
        firstPoint_ = false;
        image_->Canvas->Pen->Mode = pmNotXor;
      }
      else
      {
        currentPolygon_->Draw( monitor );
        delete currentPolygon_;
      }
      monitor->OutputTypeToWorld( x, y, xw, yw );
      GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
      GRA_setOfCharacteristics *dataCurve = gw->GetDataCurveCharacteristics();
      GRA_color *lc = static_cast<GRA_colorCharacteristic*>(dataCurve->Get("CURVECOLOR"))->Get();
      GRA_colorCharacteristic *fcChar = static_cast<GRA_colorCharacteristic*>(general->Get("AREAFILLCOLOR"));
      GRA_color *fc = fcChar->IsVector() ? fcChar->Gets().at(0) : fcChar->Get();
      int lw = static_cast<GRA_intCharacteristic*>(general->Get("LINEWIDTH"))->Get();
      switch( polygonType_ )
      {
        case 1: // rectangle
          currentPolygon_ = new GRA_rectangle(xw1_,yw1_,xw,yw,polygonAngle_,false,lc,fc,lw);
          break;
        case 2: // square
          currentPolygon_ = new GRA_rectangle(xw1_,yw1_,xw,yw,polygonAngle_,true,lc,fc,lw);
          break;
        case 3: // 5 pt star
          currentPolygon_ = new GRA_star5pt(xw1_,yw1_,xw,yw,polygonAngle_,lc,fc,lw);
          break;
      }
      currentPolygon_->Draw( monitor );
    }
    else if( ellipsePlacementMode_ )
    {
      if( firstPoint_ )
      {
        firstPoint_ = false;
        image_->Canvas->Pen->Mode = pmNotXor;
      }
      else
      {
        currentEllipse_->Draw( monitor );
        delete currentEllipse_;
      }
      monitor->OutputTypeToWorld( x, y, xw, yw );
      GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
      GRA_setOfCharacteristics *dataCurve = gw->GetDataCurveCharacteristics();
      GRA_color *lc = static_cast<GRA_colorCharacteristic*>(dataCurve->Get("CURVECOLOR"))->Get();
      GRA_colorCharacteristic *fcChar = static_cast<GRA_colorCharacteristic*>(general->Get("AREAFILLCOLOR"));
      GRA_color *fc = fcChar->IsVector() ? fcChar->Gets().at(0) : fcChar->Get();
      int lw = static_cast<GRA_intCharacteristic*>(general->Get("LINEWIDTH"))->Get();
      currentEllipse_ = new GRA_ellipse(xw1_,yw1_,xw,yw,drawCircles_,lc,fc,lw);
      currentEllipse_->Draw( monitor );
    }
  }
}

void __fastcall GRA_page::ImageMouseDown( TObject *sender, TMouseButton button,
                                          TShiftState shift, int xPixel, int yPixel )
{
  if( interactiveWindowMode_ || interactiveLegendMode_ )
  {
    xWStart_ = xPixel;
    yWStart_ = yPixel;
    mouseDown_ = true;
    return;
  }
  if( arrowPlacementMode_ || polygonPlacementMode_ || ellipsePlacementMode_ )
  {
    static_cast<GRA_borlandMonitor*>(ExGlobals::GetScreenOutput())->OutputTypeToWorld( xPixel, yPixel, xw1_, yw1_ );
    mouseDown_ = true;
    return;
  }
  if( textPlacementMode_ )return;
  if( button == mbRight )
  {
    // loop over all GRA_window's on the page
    double xW, yW;
    static_cast<GRA_borlandMonitor*>(ExGlobals::GetScreenOutput())->OutputTypeToWorld( xPixel, yPixel, xW, yW );
    std::vector<GRA_window*>::const_iterator GWend = graphWindows_.end();
    for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=GWend; ++i )
    {
      // loop over all drawable objects in each GRA_window
      std::vector<GRA_drawableObject*>::const_iterator DOend = (*i)->GetDrawableObjects().end();
      for( std::vector<GRA_drawableObject*>::const_iterator j=(*i)->GetDrawableObjects().begin(); j!=DOend; ++j )
      {
        if( (*j)->IsaCartesianAxes() )
        {
          GRA_axis *x=0, *y=0, *boxX=0, *boxY=0;
          static_cast<GRA_cartesianAxes*>(*j)->GetAxes(x,y,boxX,boxY);
          if( x && x->Inside(xW,yW) )
          {
            if( AxisPopupForm )AxisPopupForm->Close();
            AxisPopupForm = new TAxisPopupForm(this);
            AxisPopupForm->Show();
            AxisPopupForm->Setup( this, *i, 'X' );
            return;
          }
          if( y && y->Inside(xW,yW) )
          {
            if( AxisPopupForm )AxisPopupForm->Close();
            AxisPopupForm = new TAxisPopupForm(this);
            AxisPopupForm->Show();
            AxisPopupForm->Setup( this, *i, 'Y' );
            return;
          }
        }
        else if( (*j)->IsaCartesianCurve() )
        {
          GRA_cartesianCurve *cc = static_cast<GRA_cartesianCurve*>(*j);
          if( cc->Inside(xW,yW) )
          {
            if( CurvePopupForm )CurvePopupForm->Close();
            CurvePopupForm = new TCurvePopupForm(this);
            CurvePopupForm->Show();
            CurvePopupForm->Setup( this, *i, cc );
            return;
          }
        }
        else if( (*j)->IsaDrawableText() )
        {
          GRA_drawableText *dt = static_cast<GRA_drawableText*>(*j);
          if( dt->Inside(xW,yW) )
          {
            if( TextPopupForm )TextPopupForm->Close();
            TextPopupForm = new TTextPopupForm(this);
            TextPopupForm->Show();
            TextPopupForm->Setup( this, *i, dt );
            return;
          }
        }
        else if( (*j)->IsaGraphLegend() )
        {
          GRA_legend *legend = static_cast<GRA_legend*>(*j);
          if( legend->Inside(xW,yW) )
          {
            if( LegendPopupForm )LegendPopupForm->Close();
            LegendPopupForm = new TLegendPopupForm(this);
            LegendPopupForm->Show();
            LegendPopupForm->Setup( this, *i, legend );
            return;
          }
        }
      }
    }
  }
}

void __fastcall GRA_page::ImageMouseUp( TObject *sender, TMouseButton button,
                                        TShiftState shift, int x, int y )
{
  GRA_borlandMonitor *monitor = static_cast<GRA_borlandMonitor*>(ExGlobals::GetScreenOutput());
  GRA_window *gw = graphWindows_[currentWindowNumber_];
  if( textPlacementMode_ )
  {
    if( !textToPlace_ )
    {
      ExGlobals::WriteOutput("ERROR: there is no text to draw");
      return;
    }
    gw->AddDrawableObject( textToPlace_ );
    textPlacementMode_ = false;
    GRA_setOfCharacteristics *textC = gw->GetTextCharacteristics();
    double xw, yw;
    monitor->OutputTypeToWorld( x, y, xw, yw );
    static_cast<GRA_distanceCharacteristic*>(textC->Get("XLOCATION"))->SetAsWorld( xw );
    static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsWorld( yw );

    textToPlace_->SetX( xw );
    textToPlace_->SetY( yw );
    monitor->Draw( textToPlace_ );

    MainGraphicsForm->HideHintWindow();

    if( ExGlobals::StackIsOn() )
    {
      ExGlobals::WriteStack( ExString("SET TEXTALIGN ")+ExString(textToPlace_->GetAlignment()) );
      ExGlobals::WriteStack( ExString("SET XTEXTLOCATION ")+ExString(xw) );
      ExGlobals::WriteStack( ExString("SET YTEXTLOCATION ")+ExString(yw) );
      ExGlobals::WriteStack( "SET TEXTINTERACTIVE 0" );
      ExGlobals::WriteStack( ExString("TEXT '")+textToPlace_->GetString()+"'" );
      ExGlobals::WriteStack( "SET TEXTINTERACTIVE 1" );
    }
    ExGlobals::RestartScripts();
  }
  else if( interactiveWindowMode_ || interactiveLegendMode_ )
  {
    firstPoint_ = true;
    mouseDown_ = false;
    //
    // erase the previous rectangle
    //
    image_->Canvas->MoveTo( xWStart_, yWStart_ );
    image_->Canvas->LineTo( xLast_, yWStart_ );
    image_->Canvas->LineTo( xLast_, yLast_ );
    image_->Canvas->LineTo( xWStart_, yLast_ );
    image_->Canvas->LineTo( xWStart_, yWStart_ );
    //
    // draw the current rectangle
    //
    image_->Canvas->MoveTo( xWStart_, yWStart_ );
    image_->Canvas->LineTo( x, yWStart_ );
    image_->Canvas->LineTo( x, y );
    image_->Canvas->LineTo( xWStart_, y );
    image_->Canvas->LineTo( xWStart_, yWStart_ );
    int answer = Application->MessageBox("Is this OK?","Confirmation",MB_YESNO);
    image_->Canvas->MoveTo( xWStart_, yWStart_ );
    image_->Canvas->LineTo( x, yWStart_ );
    image_->Canvas->LineTo( x, y );
    image_->Canvas->LineTo( xWStart_, y );
    image_->Canvas->LineTo( xWStart_, yWStart_ );
    image_->Canvas->Pen->Mode = pmCopy;
    if( answer == IDNO )
    {
      interactiveWindowMode_ = false;
      interactiveLegendMode_ = false;
      return;
    }
    if( xWStart_ < x )
    {
      if( y < yWStart_ )
      {
        monitor->OutputTypeToWorld( xWStart_, yWStart_, xLo_, yLo_ );
        monitor->OutputTypeToWorld( x, y, xUp_, yUp_ );
      }
      else
      {
        monitor->OutputTypeToWorld( xWStart_, y, xLo_, yLo_ );
        monitor->OutputTypeToWorld( x, yWStart_, xUp_, yUp_ );
      }
    }
    else
    {
      if( y < yWStart_ )
      {
        monitor->OutputTypeToWorld( x, yWStart_, xLo_, yLo_ );
        monitor->OutputTypeToWorld( xWStart_, y, xUp_, yUp_ );
      }
      else
      {
        monitor->OutputTypeToWorld( x, y, xLo_, yLo_ );
        monitor->OutputTypeToWorld( xWStart_, yWStart_, xUp_, yUp_ );
      }
    }
    if( interactiveWindowMode_ )
    {
      interactiveWindowMode_ = false;
      double xl, yl, xu, yu;
      gw->WorldToPercent( xLo_, yLo_, xl, yl );
      gw->WorldToPercent( xUp_, yUp_, xu, yu );
      GraphicsSubwindowForm->SetUp(xl,yl,xu,yu);
    }
    if( interactiveLegendMode_ )
    {
      interactiveLegendMode_ = false;
      int units = 0;
      double xlo, ylo, xup, yup;
      if( units == 0 )       // percent
      {
        gw->WorldToPercent( xLo_, yLo_, xlo, ylo );
        gw->WorldToPercent( xUp_, yUp_, xup, yup );
      }
      else if( units == 1 ) // graph
      {
        gw->WorldToGraph( xLo_, yLo_, xlo, ylo );
        gw->WorldToGraph( xUp_, yUp_, xup, yup );
      }
      else                  // world
      {
        xlo = xLo_;
        ylo = yLo_;
        xup = xUp_;
        yup = yUp_;
      }
    }
  }
  else if( arrowPlacementMode_ )
  {
    firstPoint_ = true;
    mouseDown_ = false;
    switch( arrowType_ )
    {
      case 1:
        if( !currentArrow1_ )return;
        currentArrow1_->Draw( monitor ); // erase current arrow
        delete currentArrow1_;
        break;
      case 2:
        if( !currentArrow2_ )return;
        currentArrow2_->Draw( monitor ); // erase current arrow
        delete currentArrow2_;
        break;
      case 3:
        if( !currentArrow3_ )return;
        currentArrow3_->Draw( monitor ); // erase current arrow
        delete currentArrow3_;
        break;
    }
    double xw, yw;
    monitor->OutputTypeToWorld( x, y, xw, yw );
    GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
    GRA_setOfCharacteristics *dataCurve = gw->GetDataCurveCharacteristics();
    GRA_color *colorSave = static_cast<GRA_colorCharacteristic*>(general->Get("COLOR"))->Get();
    GRA_color *lc = static_cast<GRA_colorCharacteristic*>(dataCurve->Get("CURVECOLOR"))->Get();
    GRA_colorCharacteristic *fcChar = static_cast<GRA_colorCharacteristic*>(general->Get("AREAFILLCOLOR"));
    GRA_color *fc = fcChar->IsVector() ? fcChar->Gets().at(0) : fcChar->Get();
    int lw = static_cast<GRA_intCharacteristic*>(general->Get("LINEWIDTH"))->Get();
    double widthFactor = static_cast<GRA_doubleCharacteristic*>(general->Get("ARROWHEADWIDTH"))->Get();
    double lengthFactor = static_cast<GRA_doubleCharacteristic*>(general->Get("ARROWHEADLENGTH"))->Get();
    switch( arrowType_ )
    {
      case 1:
        currentArrow1_ = new GRA_arrow1(xw,yw,xw1_,yw1_,headsBothEnds_,lc,fc,lw,widthFactor,lengthFactor);
        currentArrow1_->Draw( monitor );
        break;
      case 2:
        currentArrow2_ = new GRA_arrow2(xw,yw,xw1_,yw1_,headsBothEnds_,lc,fc,lw,widthFactor,lengthFactor);
        currentArrow2_->Draw( monitor );
        break;
      case 3:
        currentArrow3_ = new GRA_arrow3(xw,yw,xw1_,yw1_,headsBothEnds_,lc,lw,widthFactor,lengthFactor);
        currentArrow3_->Draw( monitor );
        break;
    }
    int answer = Application->MessageBox("Is this OK?","Confirmation",MB_YESNO);
    switch( arrowType_ )
    {
      case 1:
        currentArrow1_->Draw( monitor );
        delete currentArrow1_;
        currentArrow1_ = 0;
        break;
      case 2:
        currentArrow2_->Draw( monitor );
        delete currentArrow2_;
        currentArrow2_ = 0;
        break;
      case 3:
        currentArrow3_->Draw( monitor );
        delete currentArrow3_;
        currentArrow3_ = 0;
        break;
    }
    image_->Canvas->Pen->Mode = pmCopy;
    arrowPlacementMode_ = false;
    if( answer == IDNO )return;
    switch( arrowType_ )
    {
      case 1:
        currentArrow1_ = new GRA_arrow1(xw,yw,xw1_,yw1_,headsBothEnds_,lc,fc,lw,widthFactor,lengthFactor);
        currentArrow1_->Draw( monitor );
        gw->AddDrawableObject( currentArrow1_ );
        break;
      case 2:
        currentArrow2_ = new GRA_arrow2(xw,yw,xw1_,yw1_,headsBothEnds_,lc,fc,lw,widthFactor,lengthFactor);
        currentArrow2_->Draw( monitor );
        gw->AddDrawableObject( currentArrow2_ );
        break;
      case 3:
        currentArrow3_ = new GRA_arrow3(xw,yw,xw1_,yw1_,headsBothEnds_,lc,lw,widthFactor,lengthFactor);
        currentArrow3_->Draw( monitor );
        gw->AddDrawableObject( currentArrow3_ );
        break;
    }
    static_cast<GRA_colorCharacteristic*>(general->Get("COLOR"))->Set( colorSave );
  }
  else if( polygonPlacementMode_ )
  {
    firstPoint_ = true;
    mouseDown_ = false;
    if( !currentPolygon_ )return;
    currentPolygon_->Draw( monitor ); // erase current rectangle
    delete currentPolygon_;
    double xw, yw;
    GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
    GRA_setOfCharacteristics *dataCurve = gw->GetDataCurveCharacteristics();
    GRA_color *colorSave = static_cast<GRA_colorCharacteristic*>(general->Get("COLOR"))->Get();
    GRA_color *lc = static_cast<GRA_colorCharacteristic*>(dataCurve->Get("CURVECOLOR"))->Get();
    GRA_colorCharacteristic *fcChar = static_cast<GRA_colorCharacteristic*>(general->Get("AREAFILLCOLOR"));
    GRA_color *fc = fcChar->IsVector() ? fcChar->Gets().at(0) : fcChar->Get();
    int lw = static_cast<GRA_intCharacteristic*>(general->Get("LINEWIDTH"))->Get();
    monitor->OutputTypeToWorld( x, y, xw, yw );
    switch( polygonType_ )
    {
      case 1:
        currentPolygon_ = new GRA_rectangle(xw1_,yw1_,xw,yw,polygonAngle_,false,lc,fc,lw);
        break;
      case 2:
        currentPolygon_ = new GRA_rectangle(xw1_,yw1_,xw,yw,polygonAngle_,true,lc,fc,lw);
        break;
      case 3:
        currentPolygon_ = new GRA_star5pt(xw1_,yw1_,xw,yw,polygonAngle_,lc,fc,lw);
        break;
    }
    currentPolygon_->Draw( monitor );
    int answer = Application->MessageBox("Is this OK?","Confirmation",MB_YESNO);
    currentPolygon_->Draw( monitor );
    delete currentPolygon_;
    currentPolygon_ = 0;
    image_->Canvas->Pen->Mode = pmCopy;
    polygonPlacementMode_ = false;
    if( answer == IDNO )return;
    switch( polygonType_ )
    {
      case 1:
        currentPolygon_ = new GRA_rectangle(xw1_,yw1_,xw,yw,polygonAngle_,false,lc,fc,lw);
        break;
      case 2:
        currentPolygon_ = new GRA_rectangle(xw1_,yw1_,xw,yw,polygonAngle_,true,lc,fc,lw);
        break;
      case 3:
        currentPolygon_ = new GRA_star5pt(xw1_,yw1_,xw,yw,polygonAngle_,lc,fc,lw);
        break;
    }
    currentPolygon_->Draw( monitor );
    gw->AddDrawableObject( currentPolygon_ );
    static_cast<GRA_colorCharacteristic*>(general->Get("COLOR"))->Set( colorSave );
  }
  else if( ellipsePlacementMode_ )
  {
    firstPoint_ = true;
    mouseDown_ = false;
    if( !currentEllipse_ )return;
    currentEllipse_->Draw( monitor ); // erase current rectangle
    delete currentEllipse_;
    double xw, yw;
    GRA_setOfCharacteristics *general = gw->GetGeneralCharacteristics();
    GRA_setOfCharacteristics *dataCurve = gw->GetDataCurveCharacteristics();
    GRA_color *colorSave = static_cast<GRA_colorCharacteristic*>(general->Get("COLOR"))->Get();
    GRA_color *lc = static_cast<GRA_colorCharacteristic*>(dataCurve->Get("CURVECOLOR"))->Get();
    GRA_colorCharacteristic *fcChar = static_cast<GRA_colorCharacteristic*>(general->Get("AREAFILLCOLOR"));
    GRA_color *fc = fcChar->IsVector() ? fcChar->Gets().at(0) : fcChar->Get();
    int lw = static_cast<GRA_intCharacteristic*>(general->Get("LINEWIDTH"))->Get();
    monitor->OutputTypeToWorld( x, y, xw, yw );
    currentEllipse_ = new GRA_ellipse(xw1_,yw1_,xw,yw,drawCircles_,lc,fc,lw,1);
    currentEllipse_->Draw( monitor );
    int answer = Application->MessageBox("Is this OK?","Confirmation",MB_YESNO);
    currentEllipse_->Draw( monitor );
    delete currentEllipse_;
    currentEllipse_ = 0;
    image_->Canvas->Pen->Mode = pmCopy;
    ellipsePlacementMode_ = false;
    if( answer == IDNO )return;
    currentEllipse_ = new GRA_ellipse(xw1_,yw1_,xw,yw,drawCircles_,lc,fc,lw,1);
    currentEllipse_->Draw( monitor );
    gw->AddDrawableObject( currentEllipse_ );
    static_cast<GRA_colorCharacteristic*>(general->Get("COLOR"))->Set( colorSave );
  }
}

void GRA_page::SetInteractiveWindowMode( bool b )
{ interactiveWindowMode_ = b; }

void GRA_page::SetInteractiveLegendMode( bool b )
{ interactiveLegendMode_ = b; }

void GRA_page::SetArrowPlacementMode( bool b )
{ arrowPlacementMode_ = b; }

void GRA_page::SetPolygonPlacementMode( bool b )
{ polygonPlacementMode_ = b; }

void GRA_page::SetEllipsePlacementMode( bool b )
{ ellipsePlacementMode_ = b; }

void GRA_page::SetTextPlacementMode( GRA_drawableText *dt )
{
  textPlacementMode_ = true;
  textToPlace_ = dt;
}

bool GRA_page::GetTextPlacementMode() const
{ return textPlacementMode_; }

void GRA_page::SetArrowType( int i )
{ arrowType_ = i; }

void GRA_page::SetHeadsBothEnds( bool b )
{ headsBothEnds_ = b; }

void GRA_page::SetPolygonType( int i )
{ polygonType_ = i; }

void GRA_page::SetDrawCircles( bool b )
{ drawCircles_ = b; }

void GRA_page::SetPolygonAngle( double a )
{ polygonAngle_ = a; }

