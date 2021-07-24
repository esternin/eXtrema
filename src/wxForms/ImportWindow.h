/*
Copyright (C) 2008 Joseph L. Chuma, TRIUMF

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
#ifndef EXTREMA_IMPORT_WINDOW
#define EXTREMA_IMPORT_WINDOW

#include <iostream>
#include <vector>

#include "wx/wx.h"
#include "wx/dcbuffer.h"

class DigitizeForm;
class DigitizeInfo;
class ImportWindow;
class GetCoordinates;

// The ImportForm is created when a file (png, jpeg, etc.) is imported
// (initiated from the VisualizationWindow). It contains in ImportWindow,
// the graphics canvas, as well as a button for digitizing.

class ImportForm : public wxFrame
{
public:
  ImportForm( wxWindow *, wxImage & );
  
  wxPanel* GetMainPanel() const { return mainPanel_; }

  void ZeroDigitizeForm();

  void StartDigitizing();
  void StopDigitizing();
  bool IsDigitizing() const;
  void SetDigitized( int, int );

  void StartPicking();
  void StopPicking();
  bool IsPicking() const;
  void SetPicked( int, int );

  void SetPoints( double, double );

  // event handlers
  void CloseEventHandler( wxCloseEvent & );
  void OnPaint( wxPaintEvent & );
  void OnDigitize( wxCommandEvent & );
  void OnClose( wxCommandEvent & );
  void OnMouseLeftDown( wxMouseEvent & );

private:
  wxPanel* mainPanel_;
  DigitizeForm *digitizeForm_;
  wxBitmap *bitmap_;
  ImportWindow *window_;

  bool digitizing_, picking_;

  enum { ID_digitize };
  
  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

class ImportWindow : public wxWindow
{
public:
  ImportWindow( ImportForm *, wxBitmap * );
  
  // event handlers
  void CloseEventHandler( wxCloseEvent & );
  void OnPaint( wxPaintEvent & );
  void OnMouseLeftDown( wxMouseEvent & );
  void SetPoints( double, double );

private:
  ImportForm *parent_;
  wxBitmap *bitmap_;

  DECLARE_EVENT_TABLE()
};

class DigitizeForm : public wxFrame
{
public:
  DigitizeForm( ImportForm * );
  
  void StartPicking();
  void StartDigitizing();
  void AbortDigitizing();
  void SetPicked( int, int );
  void SetDigitized( int, int );
  void SetPoints( double, double );

  void ZeroDigitizeInfo();

  void OnStartStop( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );
  void OnClose( wxCommandEvent & );

private:
  void SetupCoordinateTransform();
  
  ImportForm *parent_;
  DigitizeInfo *digitizeInfo_;

  wxButton *startStopB_;
  wxTextCtrl *xTC_, *yTC_;
  wxStaticText *fpST_, *spST_, *tpST_, *numST_, *lastST_;

  bool digitizing_;
  int nPick_, nDigitized_;
  double xUser_[3], yUser_[3];
  int xImage_[3], yImage_[3];
  double a_, b_, c_, d_, e_, f_;
  std::vector<double> xd_, yd_;

  enum { ID_start };
  
  DECLARE_EVENT_TABLE()
};

class DigitizeInfo : public wxFrame
{
public:
  DigitizeInfo( DigitizeForm * );

  void Next();
  
  void CloseEventHandler( wxCloseEvent & );
  void OnAbort( wxCommandEvent & );

private:
  DigitizeForm *parent_;
  wxStaticText *infoST_;
  wxButton *abortB_;
  int nPoints_;
  
  enum { ID_abort };

  DECLARE_EVENT_TABLE()
};

class GetCoordinates : public wxFrame
{
public:
  GetCoordinates( ImportWindow *, int, int );
  
  void OnOK( wxCommandEvent & );
  void CloseEventHandler( wxCloseEvent & );

private:
  bool GetValues( double &, double & );
  
  ImportWindow *parent_;
  wxTextCtrl *xTC_, *yTC_;

  DECLARE_EVENT_TABLE()
};

#endif
