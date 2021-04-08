/*
Copyright (C) 2007...2010 Joseph L. Chuma

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

#if __GNUC__ > 4 || \
  (__GNUC__ == 4 && (__GNUC_MINOR__ > 3 || \
    (__GNUC_MINOR__ == 3 && __GNUC_PATCHLEVEL__ >= 0)))
#include <hash_map>
#else
#include <ext/hash_map>
#endif
#include <memory>

#include "GraphicsPage.h"
#include "EGraphicsError.h"
#include "ExGlobals.h"
#include "GRA_color.h"
#include "GRA_window.h"
#include "GRA_drawableObject.h"
#include "GRA_drawableText.h"
#include "VisualizationWindow.h"
#include "GRA_wxWidgets.h"
#include "GRA_arrow1.h"
#include "GRA_arrow2.h"
#include "GRA_arrow3.h"
#include "GRA_polygon.h"
#include "GRA_ellipse.h"
#include "GRA_rectangle.h"
#include "GRA_star5pt.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_postscript.h"
#include "GRA_point.h"
#include "GRA_polyline.h"
#include "GRA_plotSymbol.h"
#include "GRA_cartesianAxes.h"
#include "GRA_cartesianCurve.h"
#include "GRA_legend.h"
#include "GRA_contourLine.h"
#include "GRA_boxPlot.h"
#include "GRA_diffusionPlot.h"
#include "GRA_ditheringPlot.h"
#include "GRA_gradientPlot.h"
#include "GRA_polarAxes.h"
#include "GRA_polarCurve.h"
#include "GRA_axis.h"
#include "AxisPopup.h"
#include "CurvePopup.h"
#include "TextPopup.h"
#include "LegendPopup.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_bitmap.h"
#include "GRA_colorControl.h"

BEGIN_EVENT_TABLE( GraphicsPage, wxNotebookPage )
  EVT_PAINT( GraphicsPage::OnPaint )
  EVT_LEFT_DOWN( GraphicsPage::OnMouseLeftDown )
  EVT_RIGHT_DOWN( GraphicsPage::OnMouseRightDown )
  EVT_MOTION( GraphicsPage::OnMouseMove )
END_EVENT_TABLE()

GraphicsPage::GraphicsPage( wxNotebook *nb )
    : wxNotebookPage(nb,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxRAISED_BORDER)
{
  int pageCount = nb->GetPageCount();
  nb->AddPage( this, wxString()<<wxT("Page ")<<(pageCount+1), true );

  int xmin, ymin, xmax, ymax;
  ExGlobals::GetMonitorLimits( xmin, ymin, xmax, ymax );

  SetCursor( *wxCROSS_CURSOR );
  SetClientSize( xmax-xmin, ymax-ymin );

  SetUpDefaultWindows();

  firstPoint_ = true;
  interactiveWindowMode_ = false;
  interactiveLegendMode_ = false;
  arrowPlacementMode_ = false;
  polygonPlacementMode_ = false;
  ellipsePlacementMode_ = false;
  arrowType_ = 1;
  polygonType_ = 1;
  polygonAngle_ = 0;
  polygonVertices_ = 3;
  headsBothEnds_ = false;
  drawCircles_ = false;
  textToPlace_ = 0;
  currentArrow1_ = 0;
  currentArrow2_ = 0;
  currentArrow3_ = 0;
  currentRectangle_ = 0;
  currentRegularPolygon_ = 0;
  current5PtStar_ = 0;
  currentEllipse_ = 0;
  figureLineThickness_ = 1;
  figureLineColor_ = GRA_colorControl::GetColor( wxT("BLACK") );
  figureFillColor_ = 0;
}

GraphicsPage::~GraphicsPage()
{ DeleteGraphWindows(); }

std::ostream &operator<<( std::ostream &out, GraphicsPage const *gp )
{
  out << "<graphicspage currentwindow=\"" << gp->currentWindowNumber_ << "\">\n";
  for( std::size_t i=0; i<gp->graphWindows_.size(); ++i )out << *gp->graphWindows_[i];
  return out << "</graphicspage>\n";
}

void GraphicsPage::DeleteGraphWindows()
{
  while( !graphWindows_.empty() )
  {
    delete graphWindows_.back();
    graphWindows_.pop_back();
  }
}
  
void GraphicsPage::Paint()
{
  wxPaintDC dc( this );
  dc.SetBackground( *wxWHITE_BRUSH );
  dc.Clear();
  DrawGraphWindows( ExGlobals::GetGraphicsOutput(), dc );
}

void GraphicsPage::OnPaint( wxPaintEvent &event )
{ Paint(); }

void GraphicsPage::RefreshGraphics()
{ Refresh(); }

void GraphicsPage::SetUpDefaultWindows()
{
  while( !graphWindows_.empty() )
  {
    delete graphWindows_.back();
    graphWindows_.pop_back();
  }
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

//void GraphicsPage::Reset()
//{}

void GraphicsPage::InheritFrom( GraphicsPage const *inherit )
{
  DeleteGraphWindows();
  int mWindows = inherit->graphWindows_.size();
  for( int i=0; i<mWindows; ++i )
  {
    GRA_window *window = const_cast<GraphicsPage*>(inherit)->GetGraphWindow(i);
    double xlo, ylo, xhi, yhi;
    window->GetDimensions( xlo, ylo, xhi, yhi );
    graphWindows_.push_back( new GRA_window(i,xlo,ylo,xhi,yhi) );
    graphWindows_.back()->InheritFrom( window );
  }
}

void GraphicsPage::AddGraphWindow( GRA_window *gw )
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

void GraphicsPage::SetGraphWindow( GRA_window *gw )
{
  ExGlobals::GetVisualizationWindow()->SetPage( this );
  currentWindowNumber_ =
    std::distance( graphWindows_.begin(),
                   std::find(graphWindows_.begin(),graphWindows_.end(),gw) );
}

GRA_window *GraphicsPage::GetGraphWindow()
{ return graphWindows_[currentWindowNumber_]; }

GRA_window *GraphicsPage::GetGraphWindow( int n )
{
  GRA_window *gw = 0;
  if( n < static_cast<int>(graphWindows_.size()) )gw = graphWindows_[n];
  return gw;
}

GRA_window *GraphicsPage::GetGraphWindow( double x, double y )
{
  std::vector<GRA_window*>::const_iterator GWend = graphWindows_.end();
  for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=GWend; ++i )
    if( (*i)->InsideWindow(x,y) )return *i;
  return 0;
}

void GraphicsPage::SetWindowNumber( int n )
{ currentWindowNumber_ = n; }

int GraphicsPage::GetWindowNumber()
{ return currentWindowNumber_; }

int GraphicsPage::GetNumberOfWindows()
{ return static_cast<int>(graphWindows_.size()); }

std::vector<GRA_window*> &GraphicsPage::GetGraphWindows()
{ return graphWindows_; }

void GraphicsPage::DrawGraphWindows( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  GRA_window *gw = graphWindows_[currentWindowNumber_];
  try
  {
    std::vector<GRA_window*>::const_iterator end = graphWindows_.end();
    for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=end; ++i )
    {
      SetGraphWindow( *i );
      (*i)->Draw( graphicsOutput, dc );
    }
  }
  catch ( EGraphicsError const &e )
  {
    SetGraphWindow( gw );
    throw;
  }
  SetGraphWindow( gw );
}

void GraphicsPage::ResetWindows()
{
  std::vector<GRA_window*>::const_iterator end = graphWindows_.end();
  for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=end; ++i )
    (*i)->Reset();
}

void GraphicsPage::EraseWindows()
{
  // do not destroy any drawableObjects, just erase the window
  //
  std::vector<GRA_window*>::const_iterator gwEnd = graphWindows_.end();
  for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=gwEnd; ++i )
    (*i)->Erase();
}

void GraphicsPage::ClearWindows()
{
  // destroy all drawableObjects in all graph windows on this page
  //
  std::vector<GRA_window*>::const_iterator gwEnd = graphWindows_.end();
  for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=gwEnd; ++i )
    (*i)->Clear();
}

void GraphicsPage::SetWindowsDefaults()
{
  // reset defaults in all graph windows
  //
  std::vector<GRA_window*>::const_iterator gwEnd = graphWindows_.end();
  for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=gwEnd; ++i )
    (*i)->SetDefaults();
}

void GraphicsPage::DisplayBackgrounds( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{
  std::vector<GRA_window*>::const_iterator gwEnd = graphWindows_.end();
  for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=gwEnd; ++i )
    (*i)->DisplayBackground( graphicsOutput, dc );
}

void GraphicsPage::ReplotAllWindows()
{
  std::size_t const nSave = currentWindowNumber_;
  std::size_t const size = graphWindows_.size();
  try
  {
    for( std::size_t i=0; i<size; ++i )
    {
      currentWindowNumber_ = i;
      ReplotCurrentWindow( true );
    }
  }
  catch ( EGraphicsError const &e )
  {
    currentWindowNumber_ = nSave;
    throw;
  }
  currentWindowNumber_ = nSave;
}

void GraphicsPage::ReplotCurrentWindow( bool repaint )
{
  if( graphWindows_[currentWindowNumber_]->GetDrawableObjects().empty() )return;
  graphWindows_[currentWindowNumber_]->Erase();
  try
  {
    graphWindows_[currentWindowNumber_]->Replot();
  }
  catch ( EGraphicsError const &e )
  {
    throw;
  }
  if( repaint )Paint();
}

void GraphicsPage::OnMouseLeftDown( wxMouseEvent &event )
{
  GRA_wxWidgets *graphicsOutput = ExGlobals::GetGraphicsOutput();
  if( textToPlace_ )
  {
    GRA_window *gw = graphWindows_[currentWindowNumber_];
    long xl, yl;
    event.GetPosition( &xl, &yl );
    double x, y;
    graphicsOutput->OutputTypeToWorld( static_cast<int>(xl), static_cast<int>(yl), x, y );
    //
    wxClientDC dc( this );
    //PrepareDC( dc );
    //
    textToPlace_->SetX( x );
    textToPlace_->SetY( y );
    textToPlace_->Draw( graphicsOutput, dc );
    gw->AddDrawableObject( textToPlace_ );
    ExGlobals::HideHint();
    if( ExGlobals::StackIsOn() )
    {
      ExGlobals::WriteStack(
        wxString(wxT("SET TEXTALIGN "))+(wxString()<<textToPlace_->GetAlignment()) );
      ExGlobals::WriteStack( wxString(wxT("SET XTEXTLOCATION "))+(wxString()<<x) );
      ExGlobals::WriteStack( wxString(wxT("SET YTEXTLOCATION "))+(wxString()<<y) );
      ExGlobals::WriteStack( wxT("SET TEXTINTERACTIVE 0") );
      ExGlobals::WriteStack( wxString(wxT("TEXT '"))+textToPlace_->GetString()+wxT("'") );
      ExGlobals::WriteStack( wxT("SET TEXTINTERACTIVE 1") );
    }
    textToPlace_ = 0;
    ExGlobals::RestartScripts();
  }
  else if( arrowPlacementMode_ || polygonPlacementMode_ || ellipsePlacementMode_ )
  {
    if( firstPoint_ )
    {
      firstPoint_ = false;
      long xl, yl;
      event.GetPosition( &xl, &yl );
      graphicsOutput->OutputTypeToWorld( static_cast<int>(xl), static_cast<int>(yl), xw1_, yw1_ );
    }
    else
    {
      firstPoint_ = true;
      if( arrowPlacementMode_ )
      {
        arrowPlacementMode_ = false;
        wxClientDC dc( this );
        dc.SetLogicalFunction( wxCOPY );
        switch (arrowType_)
        {
          case 1:
          {
            currentArrow1_->Draw( graphicsOutput, dc );
            GRA_arrow1 *a1 = new GRA_arrow1( *currentArrow1_ );
            graphWindows_[currentWindowNumber_]->AddDrawableObject( a1 );
            delete currentArrow1_;
            currentArrow1_ = 0;
            break;
          }
          case 2:
          {
            currentArrow2_->Draw( graphicsOutput, dc );
            GRA_arrow2 *a2 = new GRA_arrow2( *currentArrow2_ );
            graphWindows_[currentWindowNumber_]->AddDrawableObject( a2 );
            delete currentArrow2_;
            currentArrow2_ = 0;
            break;
          }
          case 3:
          {
            currentArrow3_->Draw( graphicsOutput, dc );
            GRA_arrow3 *a3 = new GRA_arrow3( *currentArrow3_ );
            graphWindows_[currentWindowNumber_]->AddDrawableObject( a3 );
            delete currentArrow3_;
            currentArrow3_ = 0;
            break;
          }
        }
      }
      else if( polygonPlacementMode_ )
      {
        polygonPlacementMode_ = false;
        wxClientDC dc( this );
        dc.SetLogicalFunction( wxCOPY );
        switch (polygonType_)
        {
          case 1:
          {
            currentRectangle_->Draw( graphicsOutput, dc );
            GRA_rectangle *a1 = new GRA_rectangle( *static_cast<GRA_rectangle*>(currentRectangle_) );
            graphWindows_[currentWindowNumber_]->AddDrawableObject( a1 );
            delete currentRectangle_;
            currentRectangle_ = 0;
            break;
          }
          case 2:
          {
            currentRegularPolygon_->Draw( graphicsOutput, dc );
            GRA_polygon *a2 = new GRA_polygon( *currentRegularPolygon_ );
            graphWindows_[currentWindowNumber_]->AddDrawableObject( a2 );
            delete currentRegularPolygon_;
            currentRegularPolygon_ = 0;
            break;
          }
          case 3:
          {
            current5PtStar_->Draw( graphicsOutput, dc );
            GRA_star5pt *a3 = new GRA_star5pt( *static_cast<GRA_star5pt*>(current5PtStar_) );
            graphWindows_[currentWindowNumber_]->AddDrawableObject( a3 );
            delete current5PtStar_;
            current5PtStar_ = 0;
            break;
          }
        }
      }
      else if( ellipsePlacementMode_ )
      {
        ellipsePlacementMode_ = false;
        wxClientDC dc( this );
        dc.SetLogicalFunction( wxCOPY );
        currentEllipse_->Draw( graphicsOutput, dc );
        GRA_ellipse *a1 = new GRA_ellipse( *currentEllipse_ );
        graphWindows_[currentWindowNumber_]->AddDrawableObject( a1 );
        delete currentEllipse_;
        currentEllipse_ = 0;
      }
      Refresh();
      Update();
    }
  }
}

void GraphicsPage::OnMouseRightDown( wxMouseEvent &event )
{
  long xl, yl;
  event.GetPosition( &xl, &yl );
  double xW, yW;
  ExGlobals::GetGraphicsOutput()->OutputTypeToWorld( static_cast<int>(xl), static_cast<int>(yl), xW, yW );
  //
  // loop over all GRA_window's on the page
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
          AxisPopup *axisPopup = ExGlobals::GetAxisPopup( this );
          axisPopup->Setup( *i, 'X' );
          return;
        }
        if( y && y->Inside(xW,yW) )
        {
          AxisPopup *axisPopup = ExGlobals::GetAxisPopup( this );
          axisPopup->Setup( *i, 'Y' );
          return;
        }
      }
      else if( (*j)->IsaCartesianCurve() )
      {
        GRA_cartesianCurve *cc = static_cast<GRA_cartesianCurve*>(*j);
        if( cc->Inside(xW,yW) )
        {
          CurvePopup *curvePopup = ExGlobals::GetCurvePopup( this );
          curvePopup->Setup( *i, cc );
          return;
        }
      }
      else if( (*j)->IsaDrawableText() )
      {
        GRA_drawableText *dt = static_cast<GRA_drawableText*>(*j);
        if( dt->Inside(xW,yW) )
        {
          TextPopup *textPopup = ExGlobals::GetTextPopup( this );
          textPopup->Setup( *i, dt );
          return;
        }
      }
      else if( (*j)->IsaGraphLegend() )
      {
        GRA_legend *legend = static_cast<GRA_legend*>(*j);
        if( legend->Inside(xW,yW) )
        {
          LegendPopup *legendPopup = ExGlobals::GetLegendPopup( this );
          legendPopup->Setup( *i, legend );
          return;
        }
      }
    }
  }
}

void GraphicsPage::OnMouseMove( wxMouseEvent &event )
{
  GRA_wxWidgets *graphicsOutput = ExGlobals::GetGraphicsOutput();
  MyStatusBar *statusBar =
    dynamic_cast<MyStatusBar*>(ExGlobals::GetVisualizationWindow()->GetStatusBar());
  switch ( statusBar->GetUnitsType() )
  {
    case 0:  // graph units
    {
      double xw, yw, xg, yg;
      graphicsOutput->OutputTypeToWorld( (int)event.GetX(), (int)event.GetY(), xw, yw );
      ExGlobals::GetGraphWindow()->WorldToGraph( xw, yw, xg, yg, true );
      statusBar->SetStatusText( wxString(wxT("x = "))<<xg, 1 );
      statusBar->SetStatusText( wxString(wxT("y = "))<<yg, 2 );
      break;
    }
    case 1:  // world units
    {
      double xw, yw;
      graphicsOutput->OutputTypeToWorld( (int)event.GetX(), (int)event.GetY(), xw, yw );
      statusBar->SetStatusText( wxString(wxT("x = "))<<xw, 1 );
      statusBar->SetStatusText( wxString(wxT("y = "))<<yw, 2 );
      break;
    }
    case 2:  // percentages
    {
      double xw, yw, xp, yp;
      graphicsOutput->OutputTypeToWorld( (int)event.GetX(), (int)event.GetY(), xw, yw );
      ExGlobals::GetGraphWindow()->WorldToPercent( xw, yw, xp, yp );
      statusBar->SetStatusText( wxString(wxT("%x = "))<<xp, 1 );
      statusBar->SetStatusText( wxString(wxT("%y = "))<<yp, 2 );
      break;
    }
    case 3:  // pixels
    {
      int w, h;
      GetSize( &w, &h );
      statusBar->SetStatusText( wxString(wxT("x = "))<<event.GetX(), 1 );
      statusBar->SetStatusText( wxString(wxT("y = "))<<h-event.GetY(), 2 );
      break;
    }
  }
  if( firstPoint_ )return;
  if( arrowPlacementMode_ )
  {
    wxClientDC dc( this );
    dc.SetLogicalFunction( wxINVERT );
    long xl, yl;
    event.GetPosition( &xl, &yl );
    double xw2, yw2;
    graphicsOutput->OutputTypeToWorld( static_cast<int>(xl), static_cast<int>(yl), xw2, yw2 );
    GRA_setOfCharacteristics *genC = ExGlobals::GetGraphWindow()->GetGeneralCharacteristics();
    double headWidth = static_cast<GRA_doubleCharacteristic*>(genC->Get(wxT("ARROWHEADWIDTH")))->Get();
    double headLength = static_cast<GRA_doubleCharacteristic*>(genC->Get(wxT("ARROWHEADLENGTH")))->Get();
    switch (arrowType_)
    {
      case 1:
      {
        if( currentArrow1_ )
        {
          ExGlobals::HideHint();
          currentArrow1_->Draw( graphicsOutput, dc );
          delete currentArrow1_;
        }
        currentArrow1_ = new GRA_arrow1( xw2, yw2, xw1_, yw1_, headsBothEnds_,
                                         figureLineColor_, figureFillColor_, figureLineThickness_,
                                         headWidth, headLength );
        currentArrow1_->Draw( graphicsOutput, dc );
        break;
      }
      case 2:
      {
        if( currentArrow2_ )
        {
          ExGlobals::HideHint();
          currentArrow2_->Draw( graphicsOutput, dc );
          delete currentArrow2_;
        }
        currentArrow2_ = new GRA_arrow2( xw2, yw2, xw1_, yw1_, headsBothEnds_,
                                         figureLineColor_, figureFillColor_, figureLineThickness_,
                                         headWidth, headLength );
        currentArrow2_->Draw( graphicsOutput, dc );
        break;
      }
      case 3:
      {
        if( currentArrow3_ )
        {
          ExGlobals::HideHint();
          currentArrow3_->Draw( graphicsOutput, dc );
          delete currentArrow3_;
        }
        currentArrow3_ = new GRA_arrow3( xw2, yw2, xw1_, yw1_, headsBothEnds_,
                                         figureLineColor_, figureLineThickness_,
                                         headWidth, headLength );
        currentArrow3_->Draw( graphicsOutput, dc );
      }
    }
  }
  else if( polygonPlacementMode_ )
  {
    wxClientDC dc( this );
    dc.SetLogicalFunction( wxINVERT );
    long xl, yl;
    event.GetPosition( &xl, &yl );
    double xw2, yw2;
    graphicsOutput->OutputTypeToWorld( static_cast<int>(xl), static_cast<int>(yl), xw2, yw2 );
    switch (polygonType_)
    {
      case 1:
      {
        if( currentRectangle_ )
        {
          ExGlobals::HideHint();
          currentRectangle_->Draw( graphicsOutput, dc );
          delete currentRectangle_;
        }
        currentRectangle_ = new GRA_rectangle( xw1_, yw1_, xw2, yw2, polygonAngle_, false, 
                                               figureLineColor_, figureFillColor_, figureLineThickness_ );
        currentRectangle_->Draw( graphicsOutput, dc );
        break;
      }
      case 2:
      {
        if( currentRegularPolygon_ )
        {
          ExGlobals::HideHint();
          currentRegularPolygon_->Draw( graphicsOutput, dc );
          delete currentRegularPolygon_;
        }
        double radius = sqrt((xw1_-xw2)*(xw1_-xw2)+(yw1_-yw2)*(yw1_-yw2));
        currentRegularPolygon_ = new GRA_polygon( xw1_, yw1_, polygonAngle_, radius, polygonVertices_,
                                                  figureLineColor_, figureFillColor_, figureLineThickness_ );
        currentRegularPolygon_->Draw( graphicsOutput, dc );
        break;
      }
      case 3:
      {
        if( current5PtStar_ )
        {
          ExGlobals::HideHint();
          current5PtStar_->Draw( graphicsOutput, dc );
          delete current5PtStar_;
        }
        double radius = sqrt((xw1_-xw2)*(xw1_-xw2)+(yw1_-yw2)*(yw1_-yw2));
        current5PtStar_ = new GRA_star5pt( xw1_, yw1_, 2*radius, polygonAngle_,
                                           figureLineColor_, figureFillColor_, figureLineThickness_ );
        current5PtStar_->Draw( graphicsOutput, dc );
      }
    }
  }
  else if( ellipsePlacementMode_ )
  {
    wxClientDC dc( this );
    dc.SetLogicalFunction( wxINVERT );
    long xl, yl;
    event.GetPosition( &xl, &yl );
    double xw2, yw2;
    graphicsOutput->OutputTypeToWorld( static_cast<int>(xl), static_cast<int>(yl), xw2, yw2 );
    if( currentEllipse_ )
    {
      ExGlobals::HideHint();
      currentEllipse_->Draw( graphicsOutput, dc );
      delete currentEllipse_;
    }
    if( drawCircles_ )
    {
      double radius = sqrt((xw1_-xw2)*(xw1_-xw2)+(yw1_-yw2)*(yw1_-yw2));
      currentEllipse_ = new GRA_ellipse( xw1_-radius, yw1_-radius, xw1_+radius, yw1_+radius, true,
                                         figureLineColor_, figureFillColor_, figureLineThickness_ );
    }
    else
    {
      currentEllipse_ = new GRA_ellipse( xw1_, yw1_, xw2, yw2, false,
                                         figureLineColor_, figureFillColor_, figureLineThickness_ );
    }
    currentEllipse_->Draw( graphicsOutput, dc );
  }
}

void GraphicsPage::SetInteractiveWindowMode()
{
  SetAllModesFalse();
  interactiveWindowMode_ = true;
}

void GraphicsPage::SetInteractiveLegendMode()
{
  SetAllModesFalse();
  interactiveLegendMode_ = true;
}

void GraphicsPage::SetArrowPlacementMode()
{
  SetAllModesFalse();
  arrowPlacementMode_ = true;
}

void GraphicsPage::SetPolygonPlacementMode()
{
  SetAllModesFalse();
  polygonPlacementMode_ = true;
}

void GraphicsPage::SetEllipsePlacementMode()
{
  SetAllModesFalse();
  ellipsePlacementMode_ = true;
}

void GraphicsPage::SetTextPlacementMode( GRA_drawableText *dt )
{
  SetAllModesFalse();
  textToPlace_ = dt;
}

void GraphicsPage::SetAllModesFalse()
{
  interactiveWindowMode_ = false;
  interactiveLegendMode_ = false;
  arrowPlacementMode_ = false;
  polygonPlacementMode_ = false;
  ellipsePlacementMode_ = false;
  if( textToPlace_ )
  {
    delete textToPlace_;
    textToPlace_ = 0;
  }
}

void GraphicsPage::SetArrowType( int i )
{ arrowType_ = i; }

void GraphicsPage::SetHeadsBothEnds( bool b )
{ headsBothEnds_ = b; }

void GraphicsPage::SetPolygonType( int i )
{ polygonType_ = i; }

void GraphicsPage::SetPolygonAngle( int angle )
{ polygonAngle_ = angle; }

void GraphicsPage::SetPolygonVertices( int vertices )
{ polygonVertices_ = vertices; }

void GraphicsPage::SetDrawCircles( bool b )
{ drawCircles_ = b; }

void GraphicsPage::SetFigureLineThickness( int i )
{ figureLineThickness_ = i; }

void GraphicsPage::SetFigureLineColor( GRA_color *c )
{ figureLineColor_ = c; }

void GraphicsPage::SetFigureFillColor( GRA_color *c )
{ figureFillColor_ = c; }

void GraphicsPage::SavePS( wxString const &filename )
{
  GRA_postscript ps;
  try
  {
    ps.Initialize( filename );
  }
  catch( EGraphicsError const &e )
  {
    throw;
  }
  int gwSave = currentWindowNumber_;
  try
  {
    std::vector<GRA_window*>::const_iterator end = graphWindows_.end();
    for( std::vector<GRA_window*>::const_iterator i=graphWindows_.begin(); i!=end; ++i )
    {
      currentWindowNumber_ =
        std::distance( graphWindows_.begin(),
                       std::find(graphWindows_.begin(),graphWindows_.end(),*i) );
      ps.DisplayBackground( *i );
      //
      std::vector<GRA_drawableObject*>::const_iterator endObj = (*i)->GetDrawableObjects().end();
      for( std::vector<GRA_drawableObject*>::const_iterator j=(*i)->GetDrawableObjects().begin();
           j!=endObj; ++j )
      {
        if( (*j)->IsaPoint() )ps.Draw( static_cast<GRA_point*>(*j) );
        else if( (*j)->IsaPolyline() )ps.Draw( static_cast<GRA_polyline*>(*j) );
        else if( (*j)->IsaEllipse() )ps.Draw( static_cast<GRA_ellipse*>(*j) );
        else if( (*j)->IsaPolygon() )ps.Draw( static_cast<GRA_polygon*>(*j) );
        else if( (*j)->IsaMultilineFigure() )ps.Draw( static_cast<GRA_multiLineFigure*>(*j) );
        else if( (*j)->IsaPlotsymbol() )ps.Draw( static_cast<GRA_plotSymbol*>(*j) );
        else if( (*j)->IsaDrawableText() )ps.Draw( static_cast<GRA_drawableText*>(*j) );
        else if( (*j)->IsaCartesianAxes() )ps.Draw( static_cast<GRA_cartesianAxes*>(*j) );
        else if( (*j)->IsaCartesianCurve() )ps.Draw( static_cast<GRA_cartesianCurve*>(*j) );
        else if( (*j)->IsaGraphLegend() )ps.Draw( static_cast<GRA_legend*>(*j) );
        else if( (*j)->IsaContour() )ps.Draw( static_cast<GRA_contourLine*>(*j) );
        else if( (*j)->IsaBoxPlot() )ps.Draw( static_cast<GRA_boxPlot*>(*j) );
        else if( (*j)->IsaDiffusionPlot() )ps.Draw( static_cast<GRA_diffusionPlot*>(*j) );
        else if( (*j)->IsaDitheringPlot() )ps.Draw( static_cast<GRA_ditheringPlot*>(*j) );
        else if( (*j)->IsaGradientPlot() )ps.Draw( static_cast<GRA_gradientPlot*>(*j) );
        else if( (*j)->IsaPolarAxes() )ps.Draw( static_cast<GRA_polarAxes*>(*j) );
        else if( (*j)->IsaPolarCurve() )ps.Draw( static_cast<GRA_polarCurve*>(*j) );
        //else if( (*j)->IsaAxis() )ps.Draw( static_cast<GRA_axis*>(*j) );
        //else if( (*j)->IsaThreeDFigure() )ps.Draw( static_cast<GRA_threeDFigure*>(*j) );
        //else if( (*j)->IsaSurfacePlot() )ps.Draw( static_cast<GRA_surfacePlot*>(*j) );
      }
    }
  }
  catch( EGraphicsError const &e )
  {
    currentWindowNumber_ = gwSave;
    throw;
  }
  ps.EndDoc();
  currentWindowNumber_ = gwSave;
}

void GraphicsPage::SaveBitmap( wxString const &filename, int type )
{
  int xmin, ymin, xmax, ymax;
  ExGlobals::GetMonitorLimits( xmin, ymin, xmax, ymax );
  GRA_wxWidgets ps( xmin, ymin, xmax, ymax );
  wxBitmap tempBM( xmax-xmin, ymax-ymin );
  wxMemoryDC dc;
  dc.SelectObject( tempBM );
  dc.SetBackground( *wxWHITE_BRUSH );
  dc.Clear();
  dc.StartDoc( wxT("produced by EXTREMA") );
  try
  {
    DrawGraphWindows( &ps, dc );
  }
  catch (EGraphicsError &e)
  {
    throw;
  }
  dc.EndDoc();
  wxImage image( tempBM.ConvertToImage() );
  image.SaveFile( filename, type );
}

