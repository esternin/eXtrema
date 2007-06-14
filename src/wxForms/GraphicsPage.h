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
#ifndef EXTREMA_GRAPHICSPAGE
#define EXTREMA_GRAPHICSPAGE

#include <vector>

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

class GraphicsPage : public wxNotebookPage
{
public:
  GraphicsPage( wxNotebook * );
  ~GraphicsPage();

  void Paint();
  void OnPaint( wxPaintEvent & );
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
  void ClearWindows();
  void SetWindowsDefaults();

  void ReplotAllWindows();
  void ReplotCurrentWindow( bool );

  void SetInteractiveWindowMode( bool );
  void SetInteractiveLegendMode( bool );
  void SetArrowPlacementMode( bool );
  void SetPolygonPlacementMode( bool );
  void SetEllipsePlacementMode( bool );
  void SetTextPlacementMode( GRA_drawableText * );
  bool GetTextPlacementMode() const;
  void SetArrowType( int );
  void SetHeadsBothEnds( bool );
  void SetPolygonType( int );
  void SetDrawCircles( bool );
  void SetPolygonAngle( double );
  
  void SavePS( wxString const & );
  void SaveBitmap( wxString const &, int );

  void OnMouseMove( wxMouseEvent & );
  void OnMouseLeftDown( wxMouseEvent & );

  friend std::ostream &operator<<( std::ostream &, GraphicsPage const * );

private:
  int currentWindowNumber_;
  std::vector<GRA_window*> graphWindows_;

  bool mouseDown_, firstPoint_;
  bool interactiveWindowMode_, interactiveLegendMode_;
  bool textPlacementMode_, arrowPlacementMode_;
  bool polygonPlacementMode_, ellipsePlacementMode_;
  bool headsBothEnds_, drawCircles_;
  int xWStart_, yWStart_, xLast_, yLast_;
  int arrowType_, polygonType_;
  double polygonAngle_;
  double xw1_, yw1_;
  double xLo_, xUp_, yLo_, yUp_;
  GRA_drawableText *textToPlace_;
  GRA_polygon *currentPolygon_;
  GRA_arrow1 *currentArrow1_;
  GRA_arrow2 *currentArrow2_;
  GRA_arrow3 *currentArrow3_;
  GRA_ellipse *currentEllipse_;

  void DeleteGraphWindows();
  void SetUpDefaultWindows();

  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

#endif
