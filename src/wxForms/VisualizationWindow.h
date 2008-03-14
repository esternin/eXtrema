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
#ifndef EXTREMA_VISUALIZATION_WINDOW
#define EXTREMA_VISUALIZATION_WINDOW

#include <iostream>
#include <vector>

#include "wx/wx.h"
#include "wx/dcbuffer.h"
#include "wx/notebook.h"

class GraphForm;
class TextForm;
class PeakFindForm;
class GRA_drawableText;
class VisualizationSpeedButtonPanel;
class GRA_wxWidgets;
class MyStatusBar;
class GRA_window;
class SetAspectRatioForm;
class GraphicsPage;

// The VisualizationWindow is the main extrema graphics window. It contains the
// graphics canvas as well as various menus and speed buttons.

class VisualizationWindow : public wxFrame
{
public:
  VisualizationWindow( wxWindow * );

  void ZeroGraphForm();
  GraphForm *GetGraphForm();
  void SetGraphForm( GraphForm * );

  MyStatusBar *GetStatusBar();

  void ZeroTextForm();
  TextForm *GetTextForm();
  void SetTextForm( TextForm * );
  void SetInteractiveText( GRA_drawableText * );

  void SaveBitmap( int, int, int, int, wxString const &, int );
  void SavePS( wxString const & );

  void ClearWindows();
  void ReplotAllWindows();
  void ReplotCurrentWindow( bool );

  void RefreshGraphics();
  void ResetPages();
  void ResetWindows();
  void ClearAllPages();

  void DeleteAllPages();
  void MakeFirstPage();
  int GetNumberOfPages();
  wxWindow *GetPage();
  GraphicsPage *GetPage( int );
  void NewPage( int );
  void SetPage( int );
  void SetPage( GraphicsPage * );
  void InheritPage( int );

  void ZeroPeakFindForm();
  void ZeroSetAspectRatioForm();

  GRA_window *GetGraphWindow();
  GRA_window *GetGraphWindow( int );
  GRA_window *GetGraphWindow( double, double );
  void SetWindowNumber( int );
  int GetWindowNumber();
  int GetNumberOfWindows();
  std::vector<GRA_window*> &GetGraphWindows();
  void AddGraphWindow( GRA_window * );
  void DrawGraphWindows( GRA_wxWidgets *, wxDC & );
  
  // event handlers
  void CloseEventHandler( wxCloseEvent & );
  void OnSize( wxSizeEvent & );

  void OnImportPNG( wxCommandEvent & );
  void OnImportJPEG( wxCommandEvent & );

  void OnSavePS( wxCommandEvent & );
  void OnSavePNG( wxCommandEvent & );
  void OnSaveJPEG( wxCommandEvent & );
  void OnDrawGraph( wxCommandEvent & );
  void OnDrawText( wxCommandEvent & );

  void OnPeakFind( wxCommandEvent & );

  void OnPrint( wxCommandEvent & );

  void OnClearGraphicsPage( wxCommandEvent & );
  void OnClearWindow( wxCommandEvent & );

  void OnReplotAll( wxCommandEvent & );
  void OnReplotCurrent( wxCommandEvent & );

  void OnZerolinesH( wxCommandEvent & );
  void OnZerolinesV( wxCommandEvent & );
  void OnZerolinesB( wxCommandEvent & );

  void OnResetDefaults( wxCommandEvent & );

  void OnNewPage( wxCommandEvent & );
  void OnRemovePage( wxCommandEvent & );

  void OnConfigureWindow( wxCommandEvent & );
  void OnSetAspectRatio( wxCommandEvent & );

  friend std::ostream &operator<<( std::ostream &, VisualizationWindow const * );
  
private:
  GraphForm *graphForm_;
  TextForm *textForm_;
  SetAspectRatioForm  *setAspectRatioForm_;
  PeakFindForm *peakFindForm_;
  GRA_drawableText *textToPlace_;
  VisualizationSpeedButtonPanel *visualizationSpeedButtonPanel_;
  wxNotebook *notebook_;
  MyStatusBar *statusBar_;

  enum {
      ID_importPNG,
      ID_importJPEG,
      ID_savePS,
      ID_savePNG,
      ID_saveJPEG,
      ID_print,
      ID_clearGraphicsPage,
      ID_clearWindow,
      ID_drawGraph,
      ID_drawText,
      ID_replotAll,
      ID_replotCurrent,
      ID_zerolinesH,
      ID_zerolinesV,
      ID_zerolinesB,
      ID_resetDefaults,
      ID_configureWindow,
      ID_newPage,
      ID_removePage,
      ID_aspectRatio
  };

  // any class wishing to process wxWidgets events must use this macro
  DECLARE_EVENT_TABLE()
};

class MyStatusBar : public wxStatusBar
{
public:
  MyStatusBar( VisualizationWindow * );
  virtual ~MyStatusBar() {}

  int GetUnitsType();
  void OnChangeUnits( wxCommandEvent & );
  void OnSize( wxSizeEvent & );

private:
  void SetButtonSize();

  int unitsType_; // 0=graph, 1=world, 2=percentages, 3=pixels
  wxString units_[4];
  wxButton *unitsButton_;

  enum {
      ID_changeUnits
  };
 
  DECLARE_EVENT_TABLE()
};

#endif
