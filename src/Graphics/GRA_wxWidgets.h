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
#ifndef GRA_WXWIDGETS
#define GRA_WXWIDGETS

#include <vector>

#include "wx/wx.h"

class GRA_wxWidgets
{
  // this class has the transformations from the world coordinate system to
  // the output device coordinate system, and vise versa
  // and knows how to do simple pen up, pen down operations
  // and how to draw dashed/dotted lines
  //
public:
  GRA_wxWidgets( int, int, int, int );

  virtual ~GRA_wxWidgets()
  {}

  void OutputTypeToWorld( int, int, double &, double & ) const;
  void WorldToOutputType( double, double, int &, int & ) const;
  
  void SetLineType( int );
  int GetLineType() const;
  
  void GetLimits( int &, int &, int &, int & ) const;
  
  void Plot( double, double, int, wxDC & );
  void PlotPoint( double, double, wxDC & );
  void PenUp( double, double );
  void PenDown( double, double, wxDC & );
  void StartLine( double, double );
  void ContinueLine( double, double, wxDC & );
  
  void DrawLineSet( int, double, double, double );
  void DrawLineGet( int, double &, double &, double & );
  void SetDefaultDrawLineTable();
  void DrawLineScale( int, double );
  void DrawLineScale( double );
  
protected:
  void SetUpTransformationMatrices();
  
  int xMin_, yMin_, xMax_, yMax_;
  double w2oMatrix_[2][2], w2oShift_[2]; // world to output transformation matrix
  double o2wMatrix_[2][2], o2wShift_[2]; // output to world transformation matrix
  
  double xPrevious_, yPrevious_; // world units
  int penPrevious_;
  
  int currentLineType_;
  bool newLine_;
  double drawLineTable_[3][10];
};

#endif
