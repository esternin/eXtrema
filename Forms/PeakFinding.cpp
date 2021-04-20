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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "ExGlobals.h"
#include "ExString.h"
#include "GRA_window.h"
#include "GRA_cartesianCurve.h"
#include "GRA_arrow3.h"
#include "UsefulFunctions.h"
#include "EVariableError.h"
#include "NumericVariable.h"
#include "GRA_setOfCharacteristics.h"

#include "MainGraphicsWindow.h"

#include "PeakFinding.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TPeakFindingForm *PeakFindingForm;

__fastcall TPeakFindingForm::TPeakFindingForm( TComponent* Owner )
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "PeakFindingForm", "Top", 220 );
  this->Left = ini->ReadInteger( "PeakFindingForm", "Left", 850 );
  delete ini;
  Initialize();
}

void TPeakFindingForm::Initialize()
{
  first_ = true;
  currentArrow_ = 0;
  GRA_window *gw = ExGlobals::GetGraphWindow();
  std::vector<GRA_drawableObject*> objs( gw->GetDrawableObjects() );
  curve_ = 0;
  int size = objs.size();
  for( int i=size-1; i>=0; --i )
  {
    if( objs[i]->IsaCartesianCurve() )
    {
      curve_ = static_cast<GRA_cartesianCurve*>(objs[i]);
      break;
    }
  }
  if( curve_ )
  {
    xData_.assign( curve_->GetXData().begin(), curve_->GetXData().end() );
    yData_.assign( curve_->GetYData().begin(), curve_->GetYData().end() );
    nPts_ = static_cast<int>(xData_.size());
    //
    GRA_setOfCharacteristics *xaxis = gw->GetXAxisCharacteristics();
    xLogBase_ =
      static_cast<GRA_doubleCharacteristic*>(xaxis->Get("LOGBASE"))->Get();
    xMin_ =
      static_cast<GRA_doubleCharacteristic*>(xaxis->Get("MIN"))->Get();
    xMax_ =
      static_cast<GRA_doubleCharacteristic*>(xaxis->Get("MAX"))->Get();
    GRA_setOfCharacteristics *yaxis = gw->GetYAxisCharacteristics();
    double ylaxis =
      static_cast<GRA_distanceCharacteristic*>(yaxis->Get("LOWERAXIS"))->GetAsWorld();
    double yuaxis =
      static_cast<GRA_distanceCharacteristic*>(yaxis->Get("UPPERAXIS"))->GetAsWorld();
    yLogBase_ =
      static_cast<GRA_doubleCharacteristic*>(yaxis->Get("LOGBASE"))->Get();
    yMin_ =
      static_cast<GRA_doubleCharacteristic*>(yaxis->Get("MIN"))->Get();
    yMax_ =
      static_cast<GRA_doubleCharacteristic*>(yaxis->Get("MAX"))->Get();
    arrowLength_ = 0.05*(yuaxis-ylaxis);
    yRange_ = fabs( yMax_-yMin_ );
  }
}

__fastcall TPeakFindingForm::~TPeakFindingForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "PeakFindingForm", "Top", this->Top );
  ini->WriteInteger( "PeakFindingForm", "Left", this->Left );
  delete ini;
  EraseLastArrow();
  MainGraphicsForm->GetCurrentImage()->Canvas->Pen->Mode = pmCopy;
}

void TPeakFindingForm::EraseLastArrow()
{
  if( currentArrow_ ) // erase the last arrow
  {
    currentArrow_->Draw( ExGlobals::GetScreenOutput() );
    delete currentArrow_;
    currentArrow_ = 0;
  }
}

void __fastcall TPeakFindingForm::RecordCoordinates( TObject *Sender )
{
  xVec_.push_back( xLogBase_>1.0 ? pow(xLogBase_,xCurrent_) : xCurrent_ );
  yVec_.push_back( yLogBase_>1.0 ? pow(yLogBase_,yCurrent_) : yCurrent_ );
}

void __fastcall TPeakFindingForm::CloseClick( TObject *Sender )
{ CloseForm(); }

void TPeakFindingForm::CloseForm()
{
  if( !xVec_.empty() )
  {
    try
    {
      NumericVariable::PutVariable( "PEAK$X", xVec_, 0, "from PEAK FIND gui" );
      NumericVariable::PutVariable( "PEAK$Y", yVec_, 0, "from PEAK FIND gui" );
    }
    catch( EVariableError &e )
    {
      Application->MessageBox( e.what(), "Fatal Error", MB_OK );
      return;
    }
  }
  Close();
}

void __fastcall TPeakFindingForm::FormClose( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TPeakFindingForm::FormClose2( TObject *Sender, bool &CanClose )
{ PeakFindingForm = 0; }

void __fastcall TPeakFindingForm::ScanLeftClick( TObject *Sender )
{
  if( !curve_ )
  {
    Application->MessageBox( "There is no curve drawn in the current window", "Error", MB_OK );
    CloseForm();
    return;
  }
  EraseLastArrow();
  if( first_ )
  {
    xCurrent_ = xMin_;
    yCurrent_ = yMin_;
    first_ = false;
    MainGraphicsForm->GetCurrentImage()->Canvas->Pen->Mode = pmNotXor;
  }
  double xLo = xCurrent_ - 0.002*(xMax_-xMin_);
  double xHi = xCurrent_ + 0.002*(xMax_-xMin_);
  if( xLogBase_ > 1. )
  {
    xHi = pow(xLogBase_,xHi);
    xLo = pow(xLogBase_,xLo);
  }
  int searchType = MaxMinRadioGroup->ItemIndex==0 ? 1 : -1;
  //
  // Find the next peak (or minimum) to the left.  Assume that
  // the plotted x-coordinates are monotonically increasing.
  //
  // The following section codes the "tolerance" peak-finding
  // algorithm. Basically, this finds the maximum (or minimum)
  // value of the curve between consecutive crossings of the
  // line Y=YTOL, which is set to a y-value that is sufficiently
  // large (or small) to avoid finding extraneous peaks.
  //
  int nj = 0;
  int s = 0;
  int icr = 0;
  int dcr = 0;
  bool peakFound = false;
  for( int j=nPts_-1; j>=0; --j )
  {
    if( xData_[j] <= xHi )
    {
      if( ++nj > 1 )
      {
        s = UsefulFunctions::Sign(1.0001,(yData_[j]-yData_[j+1]));
        s > 0 ? icr=j+1 : dcr=j+1; // s>0 -> increasing y, s<0 -> decreasing values
      }
    }
    //
    // Check whether an extremum has been located. This is done by
    // finding an increasing crossing of the yTolerance value
    // eventually followed by a decreasing crossing for a maximum,
    // or vice-versa for a minumum. The desired extremum lies
    // between these two intervals, and is found by selecting the
    // extreme value, maximum or minimum, over this region.
    // icr and dcr refer to the j-indices of increasing and
    // decreasing crossing respectively, and are initialized to zero
    //
    if( dcr*icr == 0 )continue;
    //
    // Two y=yTolerance crossings bounding the extremum have been found.
    // Check whether these are consistent with the type of extremum
    // (maximum or minimum) desired
    //
    if( UsefulFunctions::Sign(1,icr-dcr) != searchType )continue;
    //
    // Loop over the points between these crossings and locate the
    // the extreme value
    //
    if( searchType == 1 ) // maximum sought
    {
      double ycmax = yData_[icr-1];
      jPeak_ = icr;
      for( int k=icr; k>=dcr+1; --k )
      {
        if( yData_[k-1] > ycmax )
        {
          ycmax = yData_[k-1];
          jPeak_ = k;
        }
      }
      peakFound = true;
      break;
    }
    else // minimum sought
    {
      double ycmin = yData_[dcr-1];
      jPeak_ = dcr;
      for( int k=dcr; k>=icr+1; --k )
      {
        if( yData_[k-1] < ycmin )
        {
          ycmin = yData_[k-1];
          jPeak_ = k;
        }
      }
      peakFound = true;
      break;
    }
  }
  if( !peakFound )
  {
    Application->MessageBox( "No peak found", "Info", MB_OK );
    return;
  }
  //
  // Found a peak at index jPeak_-1
  //
  xCurrent_ = xData_[jPeak_-1];
  yCurrent_ = yData_[jPeak_-1];
  //
  ShowValues();
}

void __fastcall TPeakFindingForm::ScanRightClick(TObject *Sender)
{
  if( !curve_ )
  {
    Application->MessageBox( "There is no curve drawn in the current window", "Error", MB_OK );
    CloseForm();
    return;
  }
  EraseLastArrow();
  if( first_ )
  {
    xCurrent_ = xMin_;
    yCurrent_ = yMin_;
    first_ = false;
    MainGraphicsForm->GetCanvas()->Pen->Mode = pmNotXor;
  }
  double xLo = xCurrent_ - 0.002*fabs(xMax_-xMin_);
  double xHi = xCurrent_ + 0.002*fabs(xMax_-xMin_);
  if( xLogBase_ > 1. )
  {
    xHi = pow(xLogBase_,xHi);
    xLo = pow(xLogBase_,xLo);
  }
  //
  int searchType = MaxMinRadioGroup->ItemIndex==0 ? 1 : -1;
  //
  // Find the next peak (or minimum) to the left.  Assume that
  // the plotted x-coordinates are monotonically increasing.
  //
  // The following section codes the "tolerance" peak-finding
  // algorithm. Basically, this finds the maximum (or minimum)
  // value of the curve between consecutive crossings of the
  // line Y=YTOL, which is set to a y-value that is sufficiently
  // large (or small) to avoid finding extraneous peaks.
  //
  int nj = 0;
  int s = 0;
  int icr = 0;
  int dcr = 0;
  bool peakFound = false;
  for( int j=0; j<nPts_; ++j )
  {
    if( xData_[j] >= xLo )
    {
      if( ++nj > 1 )
      {
        s = UsefulFunctions::Sign(1.0001,(yData_[j]-yData_[j-1]));
        s > 0 ? icr=j+1 : dcr=j+1; // s>0 -> increasing values, s<0 -> decreasing values
      }
    }
    //
    // Check whether an extremum has been located. This is done by
    // finding an increasing crossing of the yTolerance value
    // eventually followed by a decreasing crossing for a maximum,
    // or vice-versa for a minumum. The desired extremum lies
    // between these two intervals, and is found by selecting the
    // extreme value, maximum or minimum, over this region.
    // icr and dcr refer to the j-indices of increasing and
    // decreasing crossing respectively, and are initialized to zero
    //
    if( dcr*icr == 0 )continue;
    //
    // Two y=yTolerance crossings bounding the extremum have been found.
    // Check whether these are consistent with the type of extremum
    // (maximum or minimum) desired
    //
    if( UsefulFunctions::Sign(1,dcr-icr) != searchType )continue;
    //
    // Loop over the points between these crossings and locate the
    // the extreme value
    //
    if( searchType == 1 ) // maximum sought
    {
      double ycmax = yData_[icr-1];
      jPeak_ = icr;
      for( int k=icr; k<=dcr-1; ++k )
      {
        if( yData_[k-1] > ycmax )
        {
          ycmax = yData_[k-1];
          jPeak_ = k;
        }
      }
      peakFound = true;
      break;
    }
    else // minimum sought
    {
      double ycmin = yData_[dcr-1];
      jPeak_ = dcr;
      for( int k=dcr; k<=icr-1; ++k )
      {
        if( yData_[k-1] < ycmin )
        {
          ycmin = yData_[k-1];
          jPeak_ = k;
        }
      }
      peakFound = true;
      break;
    }
  }
  if( !peakFound )
  {
    Application->MessageBox( "No peak found", "Info", MB_OK );
    return;
  }
  //
  // Found a peak at index jPeak_-1
  //
  xCurrent_ = xData_[jPeak_-1];
  yCurrent_ = yData_[jPeak_-1];
  //
  ShowValues();
}

void TPeakFindingForm::ShowValues()
{
  AnsiString xs(xCurrent_);
  AnsiString ys(yCurrent_);
  XPeakValEdit->Text = xs;
  YPeakValEdit->Text = ys;
  //
  double xw, yw;
  ExGlobals::GetGraphWindow()->GraphToWorld( xCurrent_, yCurrent_, xw, yw, true );
  if( xLogBase_ > 1. )xCurrent_ = log(xCurrent_)/log(xLogBase_);
  if( yLogBase_ > 1. )yCurrent_ = log(yCurrent_)/log(yLogBase_);
  if( xCurrent_>=xMin_ && xCurrent_<=xMax_ )
  {
    EraseLastArrow();
    if( MaxMinRadioGroup->ItemIndex == 0 ) // maxima
    {
      currentArrow_ = new GRA_arrow3(xw,yw,xw,yw+arrowLength_,false);
      currentArrow_->Draw( ExGlobals::GetScreenOutput() );
    }
    else                                   // minima
    {
      currentArrow_ = new GRA_arrow3(xw,yw,xw,yw-arrowLength_,false);
      currentArrow_->Draw( ExGlobals::GetScreenOutput() );
    }
  }
  else
  {
    Application->MessageBox( "Extremum found outside plot range", "Info", MB_OK );
    return;
  }
}

void __fastcall TPeakFindingForm::NewCurve( TObject *Sender )
{
  EraseLastArrow();
  xData_.clear();
  yData_.clear();
  Initialize();
}

/*
void __fastcall TPeakFindingForm::RefineButtonClick(TObject *Sender)
{
  int np;
  try
  {
    np = RefineEdit->Text.ToInt();
  }
  catch (EConvertError &e)
  {
    Application->MessageBox( "Invalid integer value for number of points", "Info", MB_OK );
    return;
  }
  //
  // Do a least squares fit of the 2*n+1 points in the neighbourhood of
  // the peak to obtain a better estimate of the extremum. A least squares
  // parabola is used for the fit. The data is assumed to be monotonically
  // increasing in the independent variable. First find the data point
  // nearest the cursor position, and then use the n points on each side of
  // this point to obtain the 2*n+1 points used for the fit.
  //
  // Find the lower and upper fit range. These would normally be
  // be centered around the point jPeak_ unless these ranges bump
  // up against the limits of the data.
  //
  int jfl = jPeak_ - np;
  int jfu = jPeak_ + np;
  if( jfl < 1 )
  {
    jfl = 1;
    jfu = min(nPts_,jfl+2*np);
  }
  if( jfu > nPts_ )
  {
    jfu = nPts_;
    jfl = max(1,jfu-2*np);
  }
  int nfit = jfu-jfl+1;
  if( nfit < 2*np+1 )
    Application->MessageBox( "fewer than 2*n+1 data points available", "Info", MB_OK );
  //
  // Accumulate the necessary sums to calculate the least-squares fit
  //
  double sumx = 0.0;
  double sumx2 = 0.0;
  double sumx3 = 0.0;
  double sumx4 = 0.0;
  double sumy = 0.0;
  double sumxy = 0.0;
  double sumx2y = 0.0;
  for( int j=jfl; j<=jfu; ++j )
  {
    double xtemp = xData_[j-1];
    double ytemp = yData_[j-1];
    double xt2 = xtemp*xtemp;
    double xt3 = xt2*xtemp;
    double xt4 = xt3*xtemp;
    sumx += xtemp;
    sumx2 += xt2;
    sumx3 += xt3;
    sumx4 += xt4;
    sumy += ytemp;
    sumxy += xtemp*ytemp;
    sumx2y += xt2*ytemp;
  }
  //
  // Form the coefficients of the least-squares parabola
  //
  double c11 = sumx*sumx2 - nfit*sumx3;
  double c12 = sumx*sumx - nfit*sumx2;
  double c21 = sumx2*sumx3 - sumx*sumx4;
  double c22 = sumx2*sumx2 - sumx*sumx3;
  double det = c11*c22 - c12*c21;
  if( fabs(det) < numeric_limits<double>::min() )
  {
    Application->MessageBox( "Zero determinant found during least squares fit", "Error", MB_OK );
    return;
  }
  double z1 = sumx*sumy - nfit*sumxy;
  double z2 = sumx2*sumxy - sumx*sumx2y;
  double afit = (z1*c22 - z2*c12)/det;
  if( afit == 0.0 )
  {
    Application->MessageBox( "Quadratic term zero, no peak fit found", "Error", MB_OK );
    return;
  }
  double bfit = (z2*c11 - z1*c21)/det;
  double cfit = (sumy - afit*sumx2 - bfit*sumx)/nfit;
  xCurrent_ = -bfit/afit/2.0;
  yCurrent_ = (afit*xCurrent_+bfit)*xCurrent_+cfit;
  //
  ShowValues();
}
*/

// end of file

