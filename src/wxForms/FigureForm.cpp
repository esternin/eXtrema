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

#include "wx/string.h"
#include "wx/stattext.h"
#include "wx/gdicmn.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/settings.h"
#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/icon.h"
#include "wx/statbmp.h"
#include "wx/sizer.h"
#include "wx/panel.h"
#include "wx/checkbox.h"
#include "wx/button.h"
#include "wx/textctrl.h"
#include "wx/notebook.h"
#include "wx/bmpbuttn.h"
#include "wx/frame.h"

#include "FigureForm.h"
#include "ExGlobals.h"
#include "VisualizationWindow.h"
#include "GraphicsPage.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( FigureForm, wxFrame )
  EVT_BUTTON( ID_arrow1, FigureForm::OnArrow1 )
  EVT_BUTTON( ID_arrow2, FigureForm::OnArrow2 )
  EVT_BUTTON( ID_arrow3, FigureForm::OnArrow3 )
  EVT_CHECKBOX( ID_twoHeads, FigureForm::OnTwoHeads )
  EVT_CHECKBOX( ID_circle, FigureForm::OnCircle )
  EVT_TEXT( ID_angle, FigureForm::OnPolygonAngleChange )
  EVT_BUTTON( ID_drawArrow, FigureForm::OnDrawArrow )
  EVT_BUTTON( ID_rectangle, FigureForm::OnRectangle )
  EVT_BUTTON( ID_square, FigureForm::OnSquare )
  EVT_BUTTON( ID_star, FigureForm::OnStar )
  EVT_BUTTON( ID_drawPolygon, FigureForm::OnDrawPolygon )
  EVT_BUTTON( ID_drawEllipse, FigureForm::OnDrawEllipse )
  EVT_BUTTON( ID_linethickness, FigureForm::OnLineThickness )
  EVT_BUTTON( ID_linecolor, FigureForm::OnLineColor )
  EVT_BUTTON( ID_fillcolor, FigureForm::OnFillColor )
  EVT_BUTTON( wxID_CLOSE, FigureForm::OnClose )
  EVT_CLOSE( FigureForm::CloseEventHandler )
END_EVENT_TABLE()

FigureForm::FigureForm( VisualizationWindow *parent )
  : wxFrame(parent,wxID_ANY,wxT("Draw Figures"),wxDefaultPosition,wxDefaultSize,
            wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL),
    visualizationWindow_(parent)
{
  CreateForm();

  wxConfigBase *config = wxConfigBase::Get();
  int ulx = config->Read( wxT("/FigureForm/UPPERLEFTX"), 100l );
  int uly = config->Read( wxT("/FigureForm/UPPERLEFTY"), 640l );
  int width = config->Read( wxT("/FigureForm/WIDTH"), 200l );
  int height = config->Read( wxT("/FigureForm/HEIGHT"), 200l );

  SetSize( ulx, uly, width, height );
  Show( true );
  
  arrowType_ = config->Read( wxT("/FigureForm/ARROWTYPE"), 1l );
  polygonType_ = config->Read( wxT("/FigureForm/POLYGONTYPE"), 1l );

  double polygonAngle = 0.0;
  config->Read( wxT("/FigureForm/POLYGONANGLE"), &polygonAngle );

  bool twoHeads = false;
  config->Read( wxT("/FigureDrawForm/HEADSBOTHENDS"), &twoHeads );

  bool drawCircles = false;
  config->Read( wxT("/FigureDrawForm/DRAWCIRCLES"), &drawCircles );

  FillOutForm( polygonAngle, twoHeads, drawCircles );
}

void FigureForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/FigureForm/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/FigureForm/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/FigureForm/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/FigureForm/HEIGHT"), static_cast<long>(height) );
    //
    config->Write( wxT("/FigureForm/ARROWTYPE"), static_cast<long>(arrowType_) );
    config->Write( wxT("/FigureForm/POLYGONTYPE"), static_cast<long>(polygonType_) );
    config->Write( wxT("/FigureForm/POLYGONANGLE"), polygonAngleTextCtrl_->GetValue() );
    config->Write( wxT("/FigureForm/HEADSBOTHENDS"), twoHeadsCheckBox_->GetValue() );
    config->Write( wxT("/FigureForm/DRAWCIRCLES"), makeCircleCheckBox_->GetValue() );
  }
  visualizationWindow_->ZeroFigureForm();
  Destroy();
}

void FigureForm::CreateForm()
{
  SetSizeHints( wxDefaultSize, wxDefaultSize );
  
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
	
  wxPanel *topPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                   wxTAB_TRAVERSAL );
  wxBoxSizer *topPanelSizer = new wxBoxSizer( wxHORIZONTAL );
  
  wxNotebook *figureNotebook = new wxNotebook( topPanel, wxID_ANY, wxDefaultPosition,
                                               wxDefaultSize, 0 );
  wxPanel *arrowPanel = new wxPanel( figureNotebook, wxID_ANY, wxDefaultPosition,
                                     wxDefaultSize, wxTAB_TRAVERSAL );
  wxBoxSizer *apSizer = new wxBoxSizer( wxVERTICAL );
  
  wxStaticText *chooseArrowStaticText = new wxStaticText( arrowPanel, wxID_ANY,
                                                          wxT("Choose arrow type"),
                                                          wxDefaultPosition, wxDefaultSize, 0 );
  chooseArrowStaticText->Wrap( -1 );
  apSizer->Add( chooseArrowStaticText, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 15 );
  
  wxPanel *arrowImagesPanel = new wxPanel( arrowPanel, wxID_ANY, wxDefaultPosition,
                                           wxDefaultSize, wxTAB_TRAVERSAL );
  wxBoxSizer* arrowImagesSizer = new wxBoxSizer( wxHORIZONTAL );
  
  wxString imageDir = ExGlobals::GetImagePath();
  arrow1_ = new wxBitmapButton( arrowImagesPanel, ID_arrow1,
                                wxBitmap(imageDir+wxT("/arrow1.bmp"),wxBITMAP_TYPE_BMP) );
  arrowImagesSizer->Add( arrow1_, 0, wxALL, 1 );
  
  arrow2_ = new wxBitmapButton( arrowImagesPanel, ID_arrow2,
                                wxBitmap(imageDir+wxT("/arrow2.bmp"),wxBITMAP_TYPE_BMP) );
  arrowImagesSizer->Add( arrow2_, 0, wxALL, 1 );
  
  arrow3_ = new wxBitmapButton( arrowImagesPanel, ID_arrow3,
                                wxBitmap(imageDir+wxT("/arrow3.bmp"),wxBITMAP_TYPE_BMP) );
  arrowImagesSizer->Add( arrow3_, 0, wxALL, 1 );
  
  arrowImagesPanel->SetSizer( arrowImagesSizer );
  arrowImagesPanel->Layout();
  arrowImagesSizer->Fit( arrowImagesPanel );
  apSizer->Add( arrowImagesPanel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
  
  twoHeadsCheckBox_ = new wxCheckBox( arrowPanel, ID_twoHeads,
                                      wxT("Heads on both ends"),
                                      wxDefaultPosition, wxDefaultSize, 0 );
  
  apSizer->Add( twoHeadsCheckBox_, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxTOP, 15 );
  
  wxButton *DrawArrowButton = new wxButton( arrowPanel, ID_drawArrow, wxT("Draw"),
                                            wxDefaultPosition, wxDefaultSize, 0 );
  apSizer->Add( DrawArrowButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15 );
  
  arrowPanel->SetSizer( apSizer );
  arrowPanel->Layout();
  apSizer->Fit( arrowPanel );
  
  figureNotebook->AddPage( arrowPanel, wxT("Arrows"), false );
  
  wxPanel *polygonPanel = new wxPanel( figureNotebook, wxID_ANY, wxDefaultPosition,
                                       wxDefaultSize, wxTAB_TRAVERSAL );
  wxBoxSizer* ppSizer = new wxBoxSizer( wxVERTICAL );
  
  wxStaticText *choosePolyStaticText = new wxStaticText( polygonPanel, wxID_ANY,
                                                         wxT("Choose polygon type"),
                                                         wxDefaultPosition, wxDefaultSize, 0 );
  choosePolyStaticText->Wrap( -1 );
  ppSizer->Add( choosePolyStaticText, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 15 );
  
  wxPanel *polygonImagesPanel = new wxPanel( polygonPanel, wxID_ANY, wxDefaultPosition,
                                             wxDefaultSize, wxTAB_TRAVERSAL );
  wxBoxSizer* polygonImagesSizer = new wxBoxSizer( wxHORIZONTAL );
  
  rectangle_ = new wxBitmapButton( polygonImagesPanel, ID_rectangle,
                                   wxBitmap(imageDir+wxT("/rectangle.bmp"),wxBITMAP_TYPE_BMP) );
  polygonImagesSizer->Add( rectangle_, 0, wxALL, 1 );
  
  square_ = new wxBitmapButton( polygonImagesPanel, ID_square,
                                wxBitmap(imageDir+wxT("/square.bmp"),wxBITMAP_TYPE_BMP) );
  polygonImagesSizer->Add( square_, 0, wxALL, 1 );
  
  star_ = new wxBitmapButton( polygonImagesPanel, ID_star,
                              wxBitmap(imageDir+wxT("/star.bmp"),wxBITMAP_TYPE_BMP) );
  polygonImagesSizer->Add( star_, 0, wxALL, 1 );
  
  polygonImagesPanel->SetSizer( polygonImagesSizer );
  polygonImagesPanel->Layout();
  polygonImagesSizer->Fit( polygonImagesPanel );
  ppSizer->Add( polygonImagesPanel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
  
  wxPanel *polyAnglePanel = new wxPanel( polygonPanel, wxID_ANY, wxDefaultPosition,
                                         wxDefaultSize, wxTAB_TRAVERSAL );
  wxBoxSizer *polyAngleSizer = new wxBoxSizer( wxHORIZONTAL );
  
  polygonAngleSC_ =
    new ExSpinCtrlI<FigureForm>( polyAnglePanel, wxT("Angle (degrees)"), 0, 360, this,
                                 &FigureForm::OnPolygonAngle );
  polygonAngleSC_->SetToolTip( wxT("set the angle for the polygon figure (0 to 360 degrees)") );
  polyAngleSizer->Add( polygonAngleSC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  polyAnglePanel->SetSizer( polyAngleSizer );
  polyAnglePanel->Layout();
  polyAngleSizer->Fit( polyAnglePanel );
  ppSizer->Add( polyAnglePanel, 0, wxEXPAND|wxTOP, 50 );
  
  wxButton *DrawPolygonButton = new wxButton( polygonPanel, ID_drawPolygon, wxT("Draw"),
                                              wxDefaultPosition, wxDefaultSize, 0 );
  ppSizer->Add( DrawPolygonButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 15 );
  
  polygonPanel->SetSizer( ppSizer );
  polygonPanel->Layout();
  ppSizer->Fit( polygonPanel );
  figureNotebook->AddPage( polygonPanel, wxT("Polygons"), false );
  
  wxPanel *ellipsePanel = new wxPanel( figureNotebook, wxID_ANY, wxDefaultPosition,
                                       wxDefaultSize, wxTAB_TRAVERSAL );
  wxBoxSizer *epSizer = new wxBoxSizer( wxVERTICAL );
  
  makeCircleCheckBox_ = new wxCheckBox( ellipsePanel, ID_circle,
                                        wxT("Make it a circle"),
                                        wxDefaultPosition, wxDefaultSize, 0 );
  
  epSizer->Add( makeCircleCheckBox_, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 70 );
  
  wxButton *DrawEllipseButton = new wxButton( ellipsePanel, ID_drawEllipse, wxT("Draw"),
                                              wxDefaultPosition, wxDefaultSize, 0 );
  epSizer->Add( DrawEllipseButton, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 80 );
  
  ellipsePanel->SetSizer( epSizer );
  ellipsePanel->Layout();
  epSizer->Fit( ellipsePanel );
  figureNotebook->AddPage( ellipsePanel, wxT("Ellipses"), false );
  
  topPanelSizer->Add( figureNotebook, 0, wxALL, 1 );
  
  wxPanel *propertiesPanel = new wxPanel( topPanel, wxID_ANY, wxDefaultPosition,
                                          wxDefaultSize, wxTAB_TRAVERSAL );
  wxBoxSizer *propertiesPanelSizer = new wxBoxSizer( wxVERTICAL );

  
  lineThicknessSC_ =
    new ExSpinCtrlI<FigureForm>( propertiesPanel, wxT("Line width"), 1, 10, this,
                                 &FigureForm::OnLineThickness );
  lineThicknessSC_->SetToolTip( wxT("set the line thickness for the figure") );
  propertiesPanelSizer->Add( lineThicknessSC_, wxSizerFlags(0).Center().Border(wxALL,5) );
  
  lineColorCC_ =
      new ExColorCtrl<FigureForm>( propertiesPanel, wxT("Line color"), this, &FigureForm::OnLineColor );
  lineColorCC_->SetToolTip( wxT("change the line color") );
  propertiesPanelSizer->Add( lineColorCC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  fillColorCC_ =
      new ExColorCtrl<FigureForm>( propertiesPanel, wxT("Fill color"), this, &FigureForm::OnFillColor );
  propertiesPanelSizer->Add( fillColorCC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  propertiesPanel->SetSizer( propertiesPanelSizer );
  propertiesPanel->Layout();
  propertiesPanelSizer->Fit( propertiesPanel );
  topPanelSizer->Add( propertiesPanel, 0, wxEXPAND | wxALL, 1 );
  
  topPanel->SetSizer( topPanelSizer );
  topPanel->Layout();
  topPanelSizer->Fit( topPanel );
  mainSizer->Add( topPanel, 0, wxEXPAND | wxALL, 1 );
  
  wxPanel *bottomPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition,
                                      wxDefaultSize, wxTAB_TRAVERSAL );
  wxBoxSizer* bpSizer = new wxBoxSizer( wxVERTICAL );
  
  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close"),
                                        wxDefaultPosition, wxDefaultSize, 0 );
  bpSizer->Add( closeButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
  
  bottomPanel->SetSizer( bpSizer );
  bottomPanel->Layout();
  bpSizer->Fit( bottomPanel );
  mainSizer->Add( bottomPanel, 0, wxEXPAND | wxALL, 1 );
  
  SetSizer( mainSizer );
  Layout();
  mainSizer->Fit( this );
}

void FigureForm::FillOutForm( double polygonAngle, bool twoHeads, bool drawCircles )
{
  lineThicknessSC_->SetValue( polygonAngle );
  twoHeadsCheckBox_->SetValue( twoHeads );
  makeCircleCheckBox_->SetValue( drawCircles );

  wxString imageDir = ExGlobals::GetImagePath();
  switch( arrowType_ )
  {
    case 1:
      if( twoHeadsCheckBox_->IsChecked() )
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow12.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow23.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow33.bmp"),wxBITMAP_TYPE_BMP));
      }
      else
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow1.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow21.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow31.bmp"),wxBITMAP_TYPE_BMP));
      }
      break;
    case 2:
      if( twoHeadsCheckBox_->IsChecked() )
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow13.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow22.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow33.bmp"),wxBITMAP_TYPE_BMP));
      }
      else
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow11.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow2.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow31.bmp"),wxBITMAP_TYPE_BMP));
      }
      break;
    case 3:
      if( twoHeadsCheckBox_->IsChecked() )
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow13.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow23.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow32.bmp"),wxBITMAP_TYPE_BMP));
      }
      else
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow11.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow21.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow3.bmp"),wxBITMAP_TYPE_BMP));
      }
      break;
  }
  GraphicsPage *page = dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage());
  page->SetArrowType( arrowType_ );
  page->SetHeadsBothEnds( twoHeadsCheckBox_->IsChecked() );
  page->SetDrawCircles( makeCircleCheckBox_->IsChecked() );
  switch( polygonType_ )
  {
    case 1:  // rectangle
      rectangle_->SetBitmapLabel(wxBitmap(imageDir+wxT("/rectangle.bmp"),wxBITMAP_TYPE_BMP));
      square_->SetBitmapLabel(wxBitmap(imageDir+wxT("/square1.bmp"),wxBITMAP_TYPE_BMP));
      star_->SetBitmapLabel(wxBitmap(imageDir+wxT("/star1.bmp"),wxBITMAP_TYPE_BMP));
      break;
    case 2:  // square
      rectangle_->SetBitmapLabel(wxBitmap(imageDir+wxT("/rectangle1.bmp"),wxBITMAP_TYPE_BMP));
      square_->SetBitmapLabel(wxBitmap(imageDir+wxT("/square.bmp"),wxBITMAP_TYPE_BMP));
      star_->SetBitmapLabel(wxBitmap(imageDir+wxT("/star1.bmp"),wxBITMAP_TYPE_BMP));
      break;
    case 3:  // 5 pt star
      rectangle_->SetBitmapLabel(wxBitmap(imageDir+wxT("/rectangle1.bmp"),wxBITMAP_TYPE_BMP));
      square_->SetBitmapLabel(wxBitmap(imageDir+wxT("/square1.bmp"),wxBITMAP_TYPE_BMP));
      star_->SetBitmapLabel(wxBitmap(imageDir+wxT("/star.bmp"),wxBITMAP_TYPE_BMP));
      break;
  }
  page->SetPolygonType( polygonType_ );
  page->SetPolygonAngle( polygonAngle_ );
}

void FigureForm::OnArrow1( wxCommandEvent &WXUNUSED(event) )
{
  switch( arrowType_ )
  {
    case 1:
      break;
    case 2:
      if( twoHeadsCheckBox_->IsChecked() )
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow12.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow23.bmp"),wxBITMAP_TYPE_BMP));
      }
      else
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow1.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow21.bmp"),wxBITMAP_TYPE_BMP));
      }
      break;
    case 3:
      if( twoHeadsCheckBox_->IsChecked() )
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow12.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow33.bmp"),wxBITMAP_TYPE_BMP));
      }
      else
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow1.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow31.bmp"),wxBITMAP_TYPE_BMP));
      }
      break;
  }
  arrowType_ = 1;
  dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage())->SetArrowType( arrowType_ );
}

void FigureForm::OnArrow2( wxCommandEvent &WXUNUSED(event) )
{
  switch( arrowType_ )
  {
    case 1:
      if( twoHeadsCheckBox_->IsChecked() )
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow13.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow22.bmp"),wxBITMAP_TYPE_BMP));
      }
      else
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow11.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow2.bmp"),wxBITMAP_TYPE_BMP));
      }
      break;
    case 2:
      break;
    case 3:
      if( twoHeadsCheckBox_->IsChecked() )
      {
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow22.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow33.bmp"),wxBITMAP_TYPE_BMP));
      }
      else
      {
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow2.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow31.bmp"),wxBITMAP_TYPE_BMP));
      }
      break;
  }
  arrowType_ = 2;
  dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage())->SetArrowType( arrowType_ );
}

void FigureForm::OnArrow3( wxCommandEvent &WXUNUSED(event) )
{
  switch( arrowType_ )
  {
    case 1:
      if( twoHeadsCheckBox_->IsChecked() )
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow13.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow32.bmp"),wxBITMAP_TYPE_BMP));
      }
      else
      {
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow11.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow3.bmp"),wxBITMAP_TYPE_BMP));
      }
      break;
    case 2:
      if( twoHeadsCheckBox_->IsChecked() )
      {
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow23.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow32.bmp"),wxBITMAP_TYPE_BMP));
      }
      else
      {
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow21.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow3.bmp"),wxBITMAP_TYPE_BMP));
      }
      break;
    case 3:
      break;
  }
  arrowType_ = 3;
  dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage())->SetArrowType( arrowType_ );
}

void FigureForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

void FigureForm::OnTwoHeads( wxCommandEvent &WXUNUSED(event) )
{
  switch( arrowType_ )
  {
    case 1:
      if( twoHeadsCheckBox_->IsChecked() )
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow12.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow23.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow33.bmp"),wxBITMAP_TYPE_BMP));
      else
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow1.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow21.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow31.bmp"),wxBITMAP_TYPE_BMP));
      break;
    case 2:
      if( twoHeadsCheckBox_->IsChecked() )
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow13.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow22.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow33.bmp"),wxBITMAP_TYPE_BMP));
      else
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow11.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow2.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow31.bmp"),wxBITMAP_TYPE_BMP));
      break;
    case 3:
      if( twoHeadsCheckBox_->IsChecked() )
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow13.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow23.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow32.bmp"),wxBITMAP_TYPE_BMP));
      else
        arrow1_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow11.bmp"),wxBITMAP_TYPE_BMP));
        arrow2_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow21.bmp"),wxBITMAP_TYPE_BMP));
        arrow3_->SetBitmapLabel(wxBitmap(imageDir+wxT("/arrow3.bmp"),wxBITMAP_TYPE_BMP));
      break;
  }
  dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage())->
      SetHeadsBothEnds( twoHeadsCheckBox_->IsChecked() );
}

void FigureForm::OnCircle( wxCommandEvent &WXUNUSED(event) )
{
  dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage())->
      SetDrawCircles( makeCircleCheckBox_->IsChecked() );
}

void FigureForm::OnPolygonAngle( int angle )
{
  dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage())->
      SetPolygonAngle( angle );
}

void FigureForm::OnDrawArrow( wxCommandEvent &WXUNUSED(event) )
{
  dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage())->SetArrowPlacementMode();
  std::vector<wxString> lines;
  lines.push_back( wxString(wxT("Left click in the visualization window to choose the arrow base,")) );
  lines.push_back( wxString(wxT("then left click again to choose the arrow end point")) );
  ExGlobals::ShowHint( lines );
}

void FigureForm::OnDrawEllipse( wxCommandEvent &WXUNUSED(event) )
{
  dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage())->SetEllipsePlacementMode();
}

void FigureForm::OnDrawPolygon( wxCommandEvent &WXUNUSED(event) )
{
  GraphicsPage *page = dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage());
  page->SetPolygonPlacementMode();
  double angle;
  polygonAngleTextCtrl_->GetValue().ToDouble( &angle );
  page->SetPolygonAngle( angle );
}

void FigureForm::OnLineThickness( int val )
{
  dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage())->SetFigureLineThickness( val );
}

void FigureForm::OnLineColor( GRA_color *color )
{
  dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage())->SetFigureLineColor( color );
}

void FigureForm::OnFillColor( GRA_color *color )
{
  if( GRA_colorControl::GetColorCode(color) == 0 )color = 0;
  dynamic_cast<GraphicsPage*>(ExGlobals::GetVisualizationWindow()->GetPage())->SetFigureFillColor( color );
}
