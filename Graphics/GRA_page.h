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
#ifndef GRA_PAGE
#define GRA_PAGE

#include <vcl/ComCtrls.hpp>

#include <vector>

class GRA_window;
class GRA_drawableText;
class GRA_outputType;
class GRA_polygon;
class GRA_arrow1;
class GRA_arrow2;
class GRA_arrow3;
class GRA_ellipse;

class GRA_page : public TTabSheet
{
public:
  GRA_page( TPageControl * );
  __fastcall ~GRA_page();

  void InheritFrom( GRA_page const * );

  void Reset();
  TImage *GetImage() const;

  void AddGraphWindow( GRA_window * );
  void SetGraphWindow( GRA_window * );

  GRA_window *GetGraphWindow();
  GRA_window *GetGraphWindow( int );
  GRA_window *GetGraphWindow( double, double );

  void SetWindowNumber( int );
  int GetWindowNumber();
  int GetNumberOfWindows();
  std::vector<GRA_window*> &GetGraphWindows();

  void DrawGraphWindows( GRA_outputType * );

  void EraseWindows();
  void ResetWindows();
  void ClearWindows();
  void SetWindowsDefaults();
  void DisplayBackgrounds( GRA_outputType * );

  void ReplotAllWindows();
  void ReplotCurrentWindow();

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

  DYNAMIC void __fastcall ImageMouseMove( TObject *, TShiftState, int, int );
  DYNAMIC void __fastcall ImageMouseDown(TObject *, TMouseButton, TShiftState, int, int );
  DYNAMIC void __fastcall ImageMouseUp(TObject *, TMouseButton, TShiftState, int, int );

  friend std::ostream &operator<<( std::ostream &, GRA_page const & );

private:
  int currentWindowNumber_;
  std::vector<GRA_window*> graphWindows_;

  TImage *image_;

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
};

#endif
