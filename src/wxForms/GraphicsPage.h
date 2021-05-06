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
#ifndef EXTREMA_GRAPHICSPAGE
#define EXTREMA_GRAPHICSPAGE

#include <vector>

#include "wx/bitmap.h"
#include "wx/dcmemory.h"
#include "wx/notebook.h"

class GRA_window;
class GRA_drawableText;
class GRA_outputType;
class GRA_polygon;
class GRA_arrow1;
class GRA_arrow2;
class GRA_arrow3;
class GRA_ellipse;
class GRA_wxWidgets;
class GRA_color;

class GraphicsPage : public wxNotebookPage
{
public:
  GraphicsPage( wxNotebook * );
  ~GraphicsPage();

  void OnPaint( wxPaintEvent & );

  // This is an expensive operation as it redraws all the drawable objects in
  // all graphics windows, but currently this has to be done whenever anything
  // in any of them changes.
  //
  // TODO: Provide a more targeted way of refreshing, notably do not redraw
  //       everything when just adding a new drawable object, as this should be
  //       unnecessary.
  void RefreshGraphics();

  //void Reset();
  void InheritFrom( GraphicsPage const * );

  void AddGraphWindow( GRA_window * );
  void SetGraphWindow( GRA_window * );

  GRA_window *GetGraphWindow();
  GRA_window *GetGraphWindow( int );
  GRA_window *GetGraphWindow( double, double );

  void SetWindowNumber( int );
  int GetWindowNumber();
  int GetNumberOfWindows();
  std::vector<GRA_window*> &GetGraphWindows();

  void DrawGraphWindows( GRA_wxWidgets *, wxDC & );

  void ResetWindows();
  void EraseWindows();
  void ClearWindows();
  void SetWindowsDefaults();
  void DisplayBackgrounds( GRA_wxWidgets *, wxDC & );

  void ReplotAllWindows();
  void ReplotCurrentWindow( bool );

  void SetInteractiveWindowMode();
  void SetInteractiveLegendMode();
  void SetArrowPlacementMode();
  void SetPolygonPlacementMode();
  void SetEllipsePlacementMode();
  void SetTextPlacementMode( GRA_drawableText * );
  void SetAllModesFalse();
  void SetArrowType( int );
  void SetHeadsBothEnds( bool );
  void SetPolygonType( int );
  void SetDrawCircles( bool );
  void SetPolygonAngle( int );
  void SetPolygonVertices( int );
  void SetFigureLineThickness( int );
  void SetFigureLineColor( GRA_color * );
  void SetFigureFillColor( GRA_color * );

  void SavePS( wxString const & );
  void SaveBitmap( wxString const &, wxBitmapType );

  void OnMouseMove( wxMouseEvent & );
  void OnMouseLeftDown( wxMouseEvent & );
  void OnMouseRightDown( wxMouseEvent & );

  friend std::ostream &operator<<( std::ostream &, GraphicsPage const * );

  // This shouldn't be called directly, use GraphicsDC to draw on this window
  // instead.
  //
  // The returned bitmap is always valid, i.e. the page contents will be
  // re-rendered into it if necessary, making this a potentially expensive
  // operation.
  wxBitmap& GetBackingStore();

private:
  void OnSize( wxSizeEvent & );


  int currentWindowNumber_;
  std::vector<GRA_window*> graphWindows_;

  bool interactiveWindowMode_, interactiveLegendMode_;
  bool arrowPlacementMode_, polygonPlacementMode_, ellipsePlacementMode_;
  bool firstPoint_, headsBothEnds_, drawCircles_;
  int arrowType_, polygonType_, polygonAngle_, polygonVertices_, figureLineThickness_;
  GRA_color *figureLineColor_, *figureFillColor_;
  double xw1_, yw1_;
  GRA_drawableText *textToPlace_;
  GRA_arrow1 *currentArrow1_;
  GRA_arrow2 *currentArrow2_;
  GRA_arrow3 *currentArrow3_;
  GRA_polygon *currentRectangle_, *currentRegularPolygon_, *current5PtStar_;
  GRA_ellipse *currentEllipse_;

  void DeleteGraphWindows();
  void SetUpDefaultWindows();

  // Bitmap containing this window contents.
  wxBitmap backingStore_;

  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

// This class replaces the previous uses of wxClientDC in the code: instead of
// drawing directly on the window (which doesn't work with the modern GUI
// toolkits), it draws on the off-screen bitmap and, importantly, refreshes the
// window in its dtor, so that it gets repainted later using the updated bitmap
// contents.
class GraphicsDC : public wxMemoryDC
{
public:
  explicit GraphicsDC(GraphicsPage* page)
    : wxMemoryDC(page->GetBackingStore()),
      page_{page}
  {
  }

  ~GraphicsDC()
  {
    // TODO: We could try optimizing repainting by refreshing only the
    //       rectangle corresponding to the updated part of the bitmap, but
    //       it's not totally clear if we can trust GetBoundingBox() for this,
    //       so for now be conservative and refresh everything.
    page_->Refresh();
  }

private:
  GraphicsPage* const page_;

  wxDECLARE_NO_COPY_CLASS(GraphicsDC);
};

#endif
