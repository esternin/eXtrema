/*
Copyright (C) 2010 Joseph L. Chuma

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
#ifndef EXTREMA_VISUALIZATION_SPEEDBUTTON_PANEL
#define EXTREMA_VISUALIZATION_SPEEDBUTTON_PANEL

#include "wx/wx.h"
#include "wx/print.h"

// The VisualizationWindow is the main extrema graphics window. It contains the
// graphics canvas as well as various menus and speed buttons.
class VisualizationWindow;

class VisualizationSpeedButtonPanel : public wxPanel
{
public:
  VisualizationSpeedButtonPanel( VisualizationWindow * );
  
  void OnClearGraphicsPage( wxCommandEvent & );
  void OnDrawGraph( wxCommandEvent & );
  void On3DPlot( wxCommandEvent & );
  void OnDrawFigure( wxCommandEvent & );
  void OnDrawText( wxCommandEvent & );
  void OnPeakFind( wxCommandEvent & );
  void OnSaveDrawing( wxCommandEvent & );
  void OnPrintDrawing( wxCommandEvent & );
  void OnNewPage( wxCommandEvent & );
  void OnRemovePage( wxCommandEvent & );
  void OnSetAspectRatio( wxCommandEvent & );
  void OnImportDrawing( wxCommandEvent & );

  void OnDrawFigures( wxCommandEvent &WXUNUSED(event) )
  {
    wxMessageDialog *md =
      new wxMessageDialog( (wxWindow*)this,
                           wxT("Click here to draw figures (coming soon)"),
                           wxT("draw figures"),
                           wxOK|wxICON_INFORMATION );
    md->ShowModal();
  }


private:
  VisualizationWindow *visualizationWindow_;

  enum {
      ID_clearGraphicsPage,
      ID_drawGraph,
      ID_drawText,
      ID_peakFind,
      ID_drawFigures,
      ID_saveDrawing,
      ID_importDrawing,
      ID_printDrawing,
      ID_newPage,
      ID_removePage,
      ID_aspectRatio,
      ID_3dplot,
      ID_drawFigure
  };
  
  DECLARE_EVENT_TABLE()
};

class MyPrintout : public wxPrintout
{
public:
  MyPrintout( wxString const &title ) : wxPrintout(title) {}
  bool OnPrintPage( int );
};

#endif
