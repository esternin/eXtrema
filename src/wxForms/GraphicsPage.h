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
  void SetFigureLineThickness( int );

  void SavePS( wxString const & );
  void SaveBitmap( wxString const &, int );

  void OnMouseMove( wxMouseEvent & );
  void OnMouseLeftDown( wxMouseEvent & );
  void OnMouseLeftUp( wxMouseEvent & );
  void OnMouseRightDown( wxMouseEvent & );

  friend std::ostream &operator<<( std::ostream &, GraphicsPage const * );

private:
  int currentWindowNumber_;
  std::vector<GRA_window*> graphWindows_;

  bool interactiveWindowMode_, interactiveLegendMode_;
  bool arrowPlacementMode_, polygonPlacementMode_, ellipsePlacementMode_;
  bool firstPoint_, headsBothEnds_, drawCircles_;
  int arrowType_, polygonType_, polygonAngle_, figureLineThickness_;
  GRA_color *figureLineColor_, *figureFillColor_;
  double xw1_, yw1_;
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
