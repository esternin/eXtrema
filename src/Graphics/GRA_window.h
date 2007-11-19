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
#ifndef GRA_WINDOW
#define GRA_WINDOW

#include <vector>
#include <ostream>

#include "wx/wx.h"

class GRA_setOfCharacteristics;
class GRA_drawableObject;
class GRA_wxWidgets;
class GRA_legend;

class GRA_window
{
private:
  typedef std::vector<GRA_drawableObject*> drawableVec;
  typedef drawableVec::const_iterator drawableVecIter;
  typedef drawableVec::const_reverse_iterator drawableVecRIter;

public:
  GRA_window( int number ) : number_(number), xLoP_(0.0), yLoP_(0.0), xHiP_(100.0), yHiP_(100.0)
  { SetUp(); }

  GRA_window( int number, double xl, double yl, double xu, double yu )
    : number_(number), xLoP_(xl), yLoP_(yl), xHiP_(xu), yHiP_(yu)
  { SetUp(); }

  ~GRA_window()
  {
    Clear();
    DeleteCharacteristics();
  }

  GRA_window( GRA_window const &rhs )
  { CopyStuff( rhs ); }

  GRA_window &operator=( GRA_window const &rhs )
  {
    if( this != &rhs )CopyStuff( rhs );
    return *this;
  }

  void SetNumber( int n )
  { number_ = n; }

  int GetNumber() const
  { return number_; }

  void GetDimensions( double &xlo, double &ylo, double &xhi, double &yhi )
  {
    xlo = xLoP_;
    ylo = yLoP_;
    xhi = xHiP_;
    yhi = yHiP_;
  }

  void Reset();
  void Clear();
  void Erase();

  GRA_setOfCharacteristics *GetXAxisCharacteristics();
  GRA_setOfCharacteristics *GetYAxisCharacteristics();
  GRA_setOfCharacteristics *GetGeneralCharacteristics();
  GRA_setOfCharacteristics *GetTextCharacteristics();
  GRA_setOfCharacteristics *GetGraphLegendCharacteristics();
  GRA_setOfCharacteristics *GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *GetPolarCharacteristics();

  void SetXAxisCharacteristics( GRA_setOfCharacteristics const & );
  void SetYAxisCharacteristics( GRA_setOfCharacteristics const & );

  void SetDefaults()
  {
    SetGeneralDefaults();
    SetXAxisDefaults();
    SetYAxisDefaults();
    SetTextDefaults();
    SetGraphLegendDefaults();
    SetDataCurveDefaults();
    SetPolarDefaults();
  }

  void SetGeneralDefaults();
  void SetXAxisDefaults();
  void SetYAxisDefaults();
  void SetTextDefaults();
  void SetGraphLegendDefaults();
  void SetDataCurveDefaults();
  void SetPolarDefaults();

  void InheritFrom( GRA_window const * );
  
  void GraphToWorld( double, double, double &, double &, bool =false ) const;
  void GraphToWorld( std::vector<double> const &, std::vector<double> const &,
                     std::vector<double> &, std::vector<double> & ) const;
  void PolarToWorld( double, double, double &, double &, bool =false ) const;
  void PolarToWorld( std::vector<double> const &, std::vector<double> const &,
                     std::vector<double> &, std::vector<double> & ) const;

  void WorldToGraph( double, double, double &, double &, bool =false ) const;
  void WorldToPercent( double, double, double &, double & ) const;
  void PercentToWorld( double, double, double &, double & ) const;
  //
  void StartLine( double, double, double &, double & );
  void ContinueLine( double, double, double *, double *, int & );
  //
  void DrawZerolines( bool, bool );
  
  void AddDrawableObject( GRA_drawableObject *object )
  { drawableObjects_.push_back(object); }

  void RemoveDrawableObject( GRA_drawableObject * );
  
  GRA_drawableObject *GetDrawableObject( int i ) const
  {
    if( i>=0 && i<static_cast<int>(drawableObjects_.size()) )return drawableObjects_[i];
    else return 0;
  }
  
  std::vector<GRA_drawableObject*> &GetDrawableObjects()
  { return drawableObjects_; }
  
  void Draw( GRA_wxWidgets *, wxDC & );
  void RemoveLastTextString();

  bool InsideWindow( double, double ) const;

  GRA_legend *GetGraphLegend();

  void Replot();

  friend std::ostream &operator<<( std::ostream &, GRA_window const & );

private:
  void CopyStuff( GRA_window const & );
  void SetUp();
  void DeleteCharacteristics();
  //
  void CreateGeneralCharacteristics( double, double, double, double );
  void CreateXAxisCharacteristics( double, double, double, double );
  void CreateYAxisCharacteristics( double, double, double, double );
  void CreateGraphLegendCharacteristics( double, double, double, double );
  void CreateDataCurveCharacteristics( double, double );
  void CreateTextCharacteristics( double, double, double, double );
  void CreatePolarCharacteristics( double, double, double, double );
  //
  void GetValues( double &, double &, double &, double &, double &, double &,
                  double &, double &, double &, double &, double &, double &,
                  double &, double & ) const;
  //
  GRA_setOfCharacteristics *xAxisCharacteristics_;
  GRA_setOfCharacteristics *yAxisCharacteristics_;
  GRA_setOfCharacteristics *generalCharacteristics_;
  GRA_setOfCharacteristics *textCharacteristics_;
  GRA_setOfCharacteristics *graphLegendCharacteristics_;
  GRA_setOfCharacteristics *dataCurveCharacteristics_;
  GRA_setOfCharacteristics *polarCharacteristics_;
  //
  double xLoP_, yLoP_, xHiP_, yHiP_;
  double xPrevious_, yPrevious_;
  int number_;
  //
  drawableVec drawableObjects_;
};

#endif
