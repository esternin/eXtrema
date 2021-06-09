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
#include <fstream>

#include "VisualizationSpeedButtonPanel.h"
#include "VisualizationWindow.h"
#include "ECommandError.h"
#include "ExGlobals.h"
#include "GRA_wxWidgets.h"
#include "EGraphicsError.h"
#include "ImportWindow.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them

BEGIN_EVENT_TABLE( VisualizationSpeedButtonPanel, wxPanel )
  EVT_BUTTON( ID_clearGraphicsPage, VisualizationSpeedButtonPanel::OnClearGraphicsPage )
  EVT_BUTTON( ID_drawGraph, VisualizationSpeedButtonPanel::OnDrawGraph )
  EVT_BUTTON( ID_3dplot, VisualizationSpeedButtonPanel::On3DPlot )
  EVT_BUTTON( ID_drawFigure, VisualizationSpeedButtonPanel::OnDrawFigure )
  EVT_BUTTON( ID_drawText, VisualizationSpeedButtonPanel::OnDrawText )
  EVT_BUTTON( ID_peakFind, VisualizationSpeedButtonPanel::OnPeakFind )
  EVT_BUTTON( ID_drawFigures, VisualizationSpeedButtonPanel::OnDrawFigures )
  EVT_BUTTON( ID_saveDrawing, VisualizationSpeedButtonPanel::OnSaveDrawing )
  EVT_BUTTON( ID_importDrawing, VisualizationSpeedButtonPanel::OnImportDrawing )
  EVT_BUTTON( ID_printDrawing, VisualizationSpeedButtonPanel::OnPrintDrawing )
  EVT_BUTTON( ID_newPage, VisualizationSpeedButtonPanel::OnNewPage )
  EVT_BUTTON( ID_removePage, VisualizationSpeedButtonPanel::OnRemovePage )
  EVT_BUTTON( ID_aspectRatio, VisualizationSpeedButtonPanel::OnSetAspectRatio )
END_EVENT_TABLE()
  //EVT_BUTTON( ID_lineWidth, VisualizationSpeedButtonPanel::OnLineWidth )
  //EVT_BUTTON( ID_lineColor, VisualizationSpeedButtonPanel::OnLineColor )
  //EVT_BUTTON( ID_fillColor, VisualizationSpeedButtonPanel::OnFillColor )

VisualizationSpeedButtonPanel::VisualizationSpeedButtonPanel( VisualizationWindow *parent )
    : wxPanel( parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxRAISED_BORDER|wxTAB_TRAVERSAL ),
      visualizationWindow_(parent)
{
  wxString imageDir = ExGlobals::GetImagePath();
  wxBoxSizer *sizer = new wxBoxSizer( wxHORIZONTAL );

  wxBitmapButton *clearGraphicsPageButton =
    new wxBitmapButton( (wxWindow*)this, ID_clearGraphicsPage,
                        wxBitmap(imageDir+wxT("/clear.GIF"),wxBITMAP_TYPE_GIF) );
  clearGraphicsPageButton->SetToolTip( wxT("clear the entire graphics page") );
  sizer->Add( clearGraphicsPageButton, wxSizerFlags(0).Border(wxTOP|wxLEFT|wxRIGHT,5) );

  wxBitmapButton *drawGraphButton =
    new wxBitmapButton( (wxWindow*)this, ID_drawGraph,
                        wxBitmap(imageDir+wxT("/drawgraph.GIF"),wxBITMAP_TYPE_GIF) );
  drawGraphButton->SetToolTip( wxT("draw a graph") );
  sizer->Add( drawGraphButton, wxSizerFlags(0).Border(wxTOP|wxRIGHT,5) );

  wxBitmapButton *threeDPlotButton =
    new wxBitmapButton( (wxWindow*)this, ID_3dplot,
                        wxBitmap(imageDir+wxT("/3dplot.bmp"),wxBITMAP_TYPE_BMP) );
  threeDPlotButton->SetToolTip( wxT("draw a 3D graph") );
  sizer->Add( threeDPlotButton, wxSizerFlags(0).Border(wxTOP|wxRIGHT,5) );

  wxBitmapButton *drawTextButton =
    new wxBitmapButton( (wxWindow*)this, ID_drawText,
                        wxBitmap(imageDir+wxT("/drawtext.GIF"),wxBITMAP_TYPE_GIF) );
  drawTextButton->SetToolTip( wxT("draw a text string in the graphics window") );
  sizer->Add( drawTextButton, wxSizerFlags(0).Border(wxTOP|wxRIGHT,5) );

  sizer->InsertSpacer( 3, 10 );
  
  wxBitmapButton *peakFindButton =
    new wxBitmapButton( (wxWindow*)this, ID_peakFind,
                        wxBitmap(imageDir+wxT("/peakfind.GIF"),wxBITMAP_TYPE_GIF) );
  peakFindButton->SetToolTip( wxT("find peaks") );
  sizer->Add( peakFindButton, wxSizerFlags(0).Border(wxTOP|wxRIGHT,5) );
  
  wxBitmapButton *drawFigureButton =
    new wxBitmapButton( (wxWindow*)this, ID_drawFigure,
                        wxBitmap(imageDir+wxT("/drawfigure.GIF"),wxBITMAP_TYPE_GIF) );
  drawFigureButton->SetToolTip( wxT("draw geometric figures") );
  sizer->Add( drawFigureButton, wxSizerFlags(0).Border(wxTOP|wxRIGHT,5) );

  sizer->InsertSpacer( 6, 10 );
  
  wxBitmapButton *saveDrawingButton =
    new wxBitmapButton( (wxWindow*)this, ID_saveDrawing,
                        wxBitmap(imageDir+wxT("/save.GIF"),wxBITMAP_TYPE_GIF) );
  saveDrawingButton->SetToolTip( wxT("save the graphics page to a file") );
  sizer->Add( saveDrawingButton, wxSizerFlags(0).Border(wxTOP|wxRIGHT,5) );
  
  wxBitmapButton *importDrawingButton =
    new wxBitmapButton( (wxWindow*)this, ID_importDrawing,
                        wxBitmap(imageDir+wxT("/importdrawing.GIF"),wxBITMAP_TYPE_GIF) );
  importDrawingButton->SetToolTip( wxT("import a drawing") );
  sizer->Add( importDrawingButton, wxSizerFlags(0).Border(wxTOP|wxRIGHT,5) );
  
  wxBitmapButton *printDrawingButton =
    new wxBitmapButton( (wxWindow*)this, ID_printDrawing,
                        wxBitmap(imageDir+wxT("/print.GIF"),wxBITMAP_TYPE_GIF) );
  printDrawingButton->SetToolTip( wxT("print the graphics page") );
  sizer->Add( printDrawingButton, wxSizerFlags(0).Border(wxTOP|wxRIGHT,5) );

  sizer->InsertSpacer( 10, 10 );
  
  wxBitmapButton *newPageButton =
    new wxBitmapButton( (wxWindow*)this, ID_newPage,
                        wxBitmap(imageDir+wxT("/newPage.GIF"),wxBITMAP_TYPE_GIF) );
  newPageButton->SetToolTip( wxT("open a new graphics page") );
  sizer->Add( newPageButton, wxSizerFlags(0).Border(wxTOP|wxRIGHT,5) );
  
  wxBitmapButton *removePageButton =
    new wxBitmapButton( (wxWindow*)this, ID_removePage,
                        wxBitmap(imageDir+wxT("/removePage.GIF"),wxBITMAP_TYPE_GIF) );
  removePageButton->SetToolTip( wxT("delete the last graphics page") );
  sizer->Add( removePageButton, wxSizerFlags(0).Border(wxTOP|wxRIGHT,5) );

  sizer->InsertSpacer( 13, 10 );
  
  wxBitmapButton *aspectRatioButton =
    new wxBitmapButton( (wxWindow*)this, ID_aspectRatio,
                        wxBitmap(imageDir+wxT("/aspectRatio.GIF"),wxBITMAP_TYPE_GIF) );
  aspectRatioButton->SetToolTip( wxT("set the graphics page aspect ratio") );
  sizer->Add( aspectRatioButton, wxSizerFlags(0).Border(wxTOP|wxRIGHT,5) );
  
  SetSizer( sizer );
}

void VisualizationSpeedButtonPanel::OnImportDrawing( wxCommandEvent &WXUNUSED(event) )
{
  wxString wildcard( wxT("PNG (*.png)|*.png|JPEG (*.jpeg,*.jpg)|*.jpeg;*.jpg") );
  wxFileDialog fd( this, wxT("Choose a PNG or JPEG file"), wxT(""), wxT(""),
                   wildcard,
                   wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR|wxFD_PREVIEW );
  if( fd.ShowModal() != wxID_OK )return;
  wxString filename( fd.GetFilename() );
  if( filename.empty() )return;
  wxString extension( ExGlobals::GetFileExtension(filename).MakeUpper() );
  //
  // open the png file and open a window to display it in
  //
  wxImage image = extension==wxT(".PNG") ? wxImage(filename,wxBITMAP_TYPE_PNG) :
                                           wxImage(filename,wxBITMAP_TYPE_JPEG);
  ImportForm *importForm = new ImportForm( this, image );
}

void VisualizationSpeedButtonPanel::OnSaveDrawing( wxCommandEvent &WXUNUSED(event) )
{
#ifdef __WINDOWS__
  wxString wildcard( wxT("Encapsulated PostScript (*.eps)|*.eps|PNG file (*.png)|*.png|JPEG file (*.jpg)|*.jpg|Windows metafile (*.wmf)|*.wmf|Windows enhanced metafile (*.emf)|*.emf") );
#else
  wxString wildcard( wxT("Encapsulated PostScript (*.eps)|*.eps|PNG file (*.png)|*.png|JPEG file (*.jpg)|*.jpg") );
#endif
  wxFileDialog fd( this, wxT("Choose an output file"), wxT(""), wxT(""),
                   wildcard, wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR );
  if( fd.ShowModal() != wxID_OK )return;
  wxString filename( fd.GetFilename() );
  if( filename.empty() )return;
  wxString extension( ExGlobals::GetFileExtension(filename) );
  if( extension.empty() )
  {
    switch ( fd.GetFilterIndex() )
    {
      case 0:
        extension = wxT(".eps");
        break;
      case 1:
        extension = wxT(".png");
        break;
      case 2:
        extension = wxT(".jpg");
        break;
      case 3:
        extension = wxT(".wmf");
        break;
      case 4:
        extension = wxT(".emf");
        break;
    }
    filename += extension;
  }
  std::ofstream outFile;
  outFile.open( filename.mb_str(wxConvUTF8), std::ios_base::out );
  if( !outFile.is_open() )
  {
    wxString message( wxT("unable to open ") );
    message += filename;
    wxMessageBox( message, wxT("error"), wxOK|wxICON_ERROR );
    return;
  }
  outFile.close();
  if( extension == wxT(".eps") )
  {
    try
    {
      visualizationWindow_->SavePS( filename );
    }
    catch ( EGraphicsError const &e )
    {
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("error"), wxOK|wxICON_ERROR );
      return;
    }
    if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxString(wxT("HARDCOPY\\POSTSCRIPT "))+filename );
  }
  else if( extension == wxT(".png") )
  {
    try
    {
      int xminM, yminM, xmaxM, ymaxM;
      ExGlobals::GetMonitorLimits( xminM, yminM, xmaxM, ymaxM );
      visualizationWindow_->SaveBitmap( xminM, yminM, xmaxM, ymaxM,
                                        filename, wxBITMAP_TYPE_PNG );
    }
    catch ( EGraphicsError const &e )
    {
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("error"), wxOK|wxICON_ERROR );
    }
    if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxString(wxT("HARDCOPY\\PNG "))+filename );
  }
  else if( extension == wxT(".jpg") )
  {
    try
    {
      int xminM, yminM, xmaxM, ymaxM;
      ExGlobals::GetMonitorLimits( xminM, yminM, xmaxM, ymaxM );
      visualizationWindow_->SaveBitmap( xminM, yminM, xmaxM, ymaxM,
                                        filename, wxBITMAP_TYPE_JPEG );
    }
    catch ( EGraphicsError const &e )
    {
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("error"), wxOK|wxICON_ERROR );
    }
    if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxString(wxT("HARDCOPY\\JPEG "))+filename );
  }
  else if( extension == wxT(".wmf") )
  {
#ifdef __WINDOWS__
    if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxString(wxT("HARDCOPY\\WMF "))+filename );
#else
    wxMessageBox( wxT("windows metafiles only available on Windows"),
                  wxT("error"), wxOK|wxICON_ERROR );
#endif
  }
  else if( extension == wxT(".emf") )
  {
#ifdef __WINDOWS__
    if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxString(wxT("HARDCOPY\\EMF "))+filename );
#else
    wxMessageBox( wxT("windows enhanced metafiles only available on Windows"),
                  wxT("error"), wxOK|wxICON_ERROR );
#endif
  }
}
  
void VisualizationSpeedButtonPanel::OnClearGraphicsPage( wxCommandEvent &event )
{ visualizationWindow_->OnClearGraphicsPage( event ); }
  
void VisualizationSpeedButtonPanel::OnDrawGraph( wxCommandEvent &event )
{ visualizationWindow_->OnDrawGraph( event ); }
  
void VisualizationSpeedButtonPanel::On3DPlot( wxCommandEvent &event )
{ visualizationWindow_->On3DPlot( event ); }
  
void VisualizationSpeedButtonPanel::OnDrawFigure( wxCommandEvent &event )
{ visualizationWindow_->OnDrawFigure( event ); }
  
void VisualizationSpeedButtonPanel::OnDrawText( wxCommandEvent &event )
{ visualizationWindow_->OnDrawText( event ); }

void VisualizationSpeedButtonPanel::OnPeakFind( wxCommandEvent &event )
{ visualizationWindow_->OnPeakFind( event ); }
  
void VisualizationSpeedButtonPanel::OnPrintDrawing( wxCommandEvent &WXUNUSED(event) )
{
  wxPrintData *printData( ExGlobals::GetPrintData() );

  // Select orientation matching the current aspect ratio.
  printData->SetOrientation( ExGlobals::GetAspectRatio() >= 1 ? wxPORTRAIT : wxLANDSCAPE );

  wxPrintDialogData printDialogData( *printData );
  wxPrinter printer( &printDialogData );
  MyPrintout printout( wxT("Extrema printing") );
  if( !printer.Print(visualizationWindow_,&printout,true /*prompt*/) )
  {
    //if( wxPrinter::GetLastError() == wxPRINTER_CANCELLED )
    //  wxMessageBox( wxT("You canceled printing"), wxT("Printing"), wxOK );
    //else
    //  wxMessageBox(
    //    wxT("There was a problem printing.\nPerhaps your printer is not set correctly?"),
    //    wxT("Printing"), wxOK );
  }
  else *printData = printer.GetPrintDialogData().GetPrintData();
}

bool MyPrintout::OnPrintPage( int page )
{
  wxDC *dc = GetDC();
  if( dc )
  {
    long xo=0, yo=0;
    dc->SetDeviceOrigin( xo, yo );
    dc->SetMapMode( wxMM_TEXT );

    int dch = dc->GetPPI().GetHeight();  
    int dcw = dc->GetPPI().GetWidth();
    double xminW, yminW, xmaxW, ymaxW;
    ExGlobals::GetWorldLimits( xminW, yminW, xmaxW, ymaxW );
    int xmin = static_cast<int>(xminW*dcw+0.5);
    int ymin = static_cast<int>(yminW*dch+0.5);
    int xmax = static_cast<int>(xmaxW*dcw+0.5);
    int ymax = static_cast<int>(ymaxW*dch+0.5);

    double fontScale = ExGlobals::GetFontScale();
    int ppiScreenX = wxScreenDC().GetPPI().GetWidth();
    ExGlobals::SetFontScale( 96.0 / (double)ppiScreenX );

    //wxLogDebug("VisualizationSpeedButtonPanel::OnPrintPage: WorldLimits=%g..%g x %g..%g", xminW, xmaxW, yminW, ymaxW);
    //wxLogDebug("VisualizationSpeedButtonPanel::OnPrintPage: map to ps()=%d..%d x %d..%d", xmin, xmax, ymin, ymax);

    // If the aspect ratio of the graph window does not match the aspect ratio of the selected paper, 
    // adjust the scale of the image to fit within the limiting dimension, e.g. a landscape graph on a portraint page
    double scale;
    int pageWidth, pageHeight;
    GetPageSizePixels( &pageWidth, &pageHeight );
    if( (xmax-xmin) > (ymax-ymin) ) // landscape, x is the limiting direction
    {
       scale = ( pageWidth > pageHeight ) ? 1.0 : (double)pageWidth / (double)pageHeight;
       xmin -= 72;                  // there seems to be a built-in 1" margin, shift
       xmax -= 72;
    }
    else                            // portrait, y is the limiting direction
    {
       scale = ( pageHeight > pageWidth ) ? 1.0 : (double)pageHeight / (double)pageWidth;
    }
    wxLogDebug("VisualizationSpeedButtonPanel::OnPrintPage: scale=%g, page=%dx%d", scale, pageWidth, pageHeight);
    dc->SetUserScale( scale, scale );

    dc->StartDoc( wxT("extrema printing...") );
    GRA_wxWidgets ps( xmin, ymin, xmax, ymax );
    try
    {
      ExGlobals::DrawGraphWindows( &ps, *dc );
    }
    catch (EGraphicsError &e)
    {
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("error"), wxOK|wxICON_ERROR );
      return false;
    }
    dc->EndDoc();
    ExGlobals::SetFontScale( fontScale );
    return true;
  }
  else return false;
}

void VisualizationSpeedButtonPanel::OnNewPage( wxCommandEvent &event )
{ visualizationWindow_->OnNewPage( event ); }

void VisualizationSpeedButtonPanel::OnRemovePage( wxCommandEvent &event )
{ visualizationWindow_->OnRemovePage( event ); }

void VisualizationSpeedButtonPanel::OnSetAspectRatio( wxCommandEvent &event )
{ visualizationWindow_->OnSetAspectRatio( event ); }

// end of file
