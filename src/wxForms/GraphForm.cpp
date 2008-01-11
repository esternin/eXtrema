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
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

#include "wx/config.h"
#include "wx/statline.h"
#include "wx/stattext.h"

#include "GraphForm.h"
#include "VisualizationWindow.h"
#include "NumericVariable.h"
#include "EVariableError.h"
#include "NVariableTable.h"
#include "ExGlobals.h"
#include "GRA_window.h"
#include "GRA_intCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_colorControl.h"
#include "GRA_setOfCharacteristics.h"
#include "EGraphicsError.h"
#include "GRA_cartesianAxes.h"
#include "GRA_cartesianCurve.h"
#include "GRA_drawableText.h"
#include "UsefulFunctions.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( GraphForm, wxFrame )
  EVT_COMBOBOX( ID_ivec, GraphForm::OnIndepChange )
  EVT_COMBOBOX( ID_symbol, GraphForm::OnPlotsymbolChange )
  EVT_BUTTON( ID_clear, GraphForm::OnClear )
  EVT_BUTTON( wxID_APPLY, GraphForm::OnDraw )
  EVT_BUTTON( wxID_CLOSE, GraphForm::OnClose )
  EVT_CLOSE( GraphForm::CloseEventHandler )
END_EVENT_TABLE()

GraphForm::GraphForm( VisualizationWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("Graph Form"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      visualizationWindow_(parent)
{
  int const foo[] = {0,1,14,2,3,4,5,6,15,7,8,16,9,17,10,18,11,12,13};
  symArray_.assign( foo, foo+(sizeof foo/sizeof *foo) );
  //
  CreateForm();
  //
  wxConfigBase *config = wxConfigBase::Get();
  int ulx = config->Read( wxT("/GraphForm/UPPERLEFTX"), 0l );
  int uly = config->Read( wxT("/GraphForm/UPPERLEFTY"), 640l );
  int width = config->Read( wxT("/GraphForm/WIDTH"), 555l );
  int height = config->Read( wxT("/GraphForm/HEIGHT"), 200l );
  SetSize( ulx, uly, width, height );
  //
  Show( true );
  //
  FillOutForm();
  //
  color_ = 0;
  //
  wxString sTmp;
  config->Read( wxT("/GraphForm/DATA_VECTOR"), &sTmp, wxT("") );
  if( dataCB_->FindString(sTmp) > 0 )dataCB_->SetStringSelection( sTmp );
  sTmp.clear();
  config->Read( wxT("/GraphForm/INDEPENDENT_VECTOR"), &sTmp, wxT("") );
  if( indepCB_->FindString(sTmp) > 0 )
  {
    indepCB_->SetStringSelection( sTmp );
    minST_->Enable( true );
    minTC_->Enable( true );
    maxST_->Enable( true );
    maxTC_->Enable( true );
    double imin, imax;
    NumericVariable *vec = NVariableTable::GetTable()->GetVariable( sTmp );
    vec->GetData().GetMinMax( imin, imax );
    minTC_->SetValue( wxString()<<imin );
    maxTC_->SetValue( wxString()<<imax );
  }
  sTmp.clear();
  config->Read( wxT("/GraphForm/ERROR_VECTOR"), &sTmp, wxT("") );
  if( errorCB_->FindString(sTmp) > 0 )errorCB_->SetStringSelection( sTmp );
  int ps = config->Read( wxT("/GraphForm/PLOTSYMBOL"), 18l );
  symbolCB_->SetSelection( wxNOT_FOUND );
  int size = symArray_.size();
  for( int i=0; i<size; ++i )
  {
    if( abs(ps) == symArray_[i] )
    {
      symbolCB_->SetSelection( i );
      break;
    }
  }
  connectCkB_->SetValue( ps >= 0 );
  connectCkB_->Enable( symbolCB_->GetStringSelection() != wxT("<none>") );

  Layout();
}

void GraphForm::CreateForm()
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  mainSizer->AddSpacer( 10 );
  
  wxPanel *topPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL );

  wxPanel *topLeftPanel = new wxPanel( topPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxGridSizer *topLeftSizer = new wxGridSizer( 4, 2, 5, 5 );

  dataST_ = new wxStaticText( topLeftPanel, wxID_ANY, wxT("Dependent vector") );
  topLeftSizer->Add( dataST_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  dataCB_ = new wxComboBox( topLeftPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(170,25), 0, 0, wxCB_READONLY );
  topLeftSizer->Add( dataCB_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  indepST_ = new wxStaticText( topLeftPanel, wxID_ANY, wxT("Independent vector (optional)") );
  topLeftSizer->Add( indepST_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  indepCB_ = new wxComboBox( topLeftPanel, ID_ivec, wxT(""), wxDefaultPosition, wxSize(170,25), 0, 0, wxCB_READONLY );
  topLeftSizer->Add( indepCB_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  errorST_ = new wxStaticText( topLeftPanel, wxID_ANY, wxT("Error vector (optional)") );
  topLeftSizer->Add( errorST_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  errorCB_ = new wxComboBox( topLeftPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(170,25), 0, 0, wxCB_READONLY );
  topLeftSizer->Add( errorCB_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  symbolST_ = new wxStaticText( topLeftPanel, wxID_ANY, wxT("Plot symbol") );
  topLeftSizer->Add( symbolST_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  symbolCB_ = new wxComboBox( topLeftPanel, ID_symbol, wxT(""), wxDefaultPosition, wxSize(170,25), 0, 0, wxCB_READONLY );
  topLeftSizer->Add( symbolCB_, wxSizerFlags(0).Left().Border(wxALL,2) );

  topLeftPanel->SetSizer( topLeftSizer );
  topSizer->Add( topLeftPanel, wxSizerFlags(0).Left().Border(wxALL,1) );

  wxPanel *topRightPanel = new wxPanel( topPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *topRightSizer = new wxBoxSizer( wxVERTICAL );

  topRightSizer->AddSpacer( 14 );

  wxPanel *rangePanel = new wxPanel( topRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *rangeSizer = new wxBoxSizer( wxHORIZONTAL );

  wxPanel *minPanel = new wxPanel( rangePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *minSizer = new wxBoxSizer( wxVERTICAL );
  minST_ = new wxStaticText( minPanel, wxID_ANY, wxT("Min") );
  minSizer->Add( minST_, wxSizerFlags(0).Center().Border(wxALL,1) );
  minTC_ = new wxTextCtrl( minPanel, wxID_ANY );
  minSizer->Add( minTC_, wxSizerFlags(0).Center().Border(wxALL,1) );
  minPanel->SetSizer( minSizer );
  rangeSizer->Add( minPanel, wxSizerFlags(0).Left().Border(wxALL,1) );

  wxPanel *maxPanel = new wxPanel( rangePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *maxSizer = new wxBoxSizer( wxVERTICAL );
  maxST_ = new wxStaticText( maxPanel, wxID_ANY, wxT("Max") );
  maxSizer->Add( maxST_, wxSizerFlags(0).Center().Border(wxALL,1) );
  maxTC_ = new wxTextCtrl( maxPanel, wxID_ANY );
  maxSizer->Add( maxTC_, wxSizerFlags(0).Center().Border(wxALL,1) );
  maxPanel->SetSizer( maxSizer );
  rangeSizer->Add( maxPanel, wxSizerFlags(0).Left().Border(wxALL,1) );

  rangePanel->SetSizer( rangeSizer );
  topRightSizer->Add( rangePanel, wxSizerFlags(0).Left().Border(wxALL,1) );

  topRightSizer->AddSpacer( 40 );

  connectCkB_ = new wxCheckBox( topRightPanel, wxID_ANY, wxT("Connect plot symbols") );
  topRightSizer->Add( connectCkB_, wxSizerFlags(0).Center().Border(wxALL,1) );

  topRightPanel->SetSizer( topRightSizer );
  topSizer->Add( topRightPanel, wxSizerFlags(0).Left().Border(wxALL,1) );

  topPanel->SetSizer( topSizer );
  mainSizer->Add( topPanel, wxSizerFlags(0).Center().Border(wxALL,1) );

  wxPanel *bottomPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );

  wxButton *drawButton = new wxButton( bottomPanel, wxID_APPLY, wxT("Draw") );
  drawButton->SetToolTip( wxT("click to plot the chosen vectors") );
  bottomSizer->Add( drawButton, wxSizerFlags(0).Border(wxALL,10) );
  
  wxButton *clearButton = new wxButton( bottomPanel, ID_clear, wxT("Clear graphics") );
  clearButton->SetToolTip( wxT("click to clear the visualization window") );
  bottomSizer->Add( clearButton, wxSizerFlags(0).Border(wxALL,10) );
  
  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close") );
  closeButton->SetToolTip( wxT("close this form") );
  bottomSizer->Add( closeButton, wxSizerFlags(0).Border(wxALL,10) );
  
  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(0).Centre().Border(wxALL,1) );
  
  SetSizer( mainSizer );
}

void GraphForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/GraphForm/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/GraphForm/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/GraphForm/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/GraphForm/HEIGHT"), static_cast<long>(height) );
    //
    config->Write( wxT("/GraphForm/DATA_VECTOR"), dataCB_->GetStringSelection() );
    config->Write( wxT("/GraphForm/INDEPENDENT_VECTOR"), indepCB_->GetStringSelection() );
    config->Write( wxT("/GraphForm/ERROR_VECTOR"), errorCB_->GetStringSelection() );
    int ps = symArray_[symbolCB_->GetSelection()];
    if( !connectCkB_->IsChecked() )ps *= -1;
    config->Write( wxT("/GraphForm/PLOTSYMBOL"), long(ps) );
  }
  //
  // close all child windows
  /*
  wxWindowList children( GetChildren() );
  wxWindowListNode *node = children.GetFirst();
  while( node )
  {
    wxWindow *window = node->GetData();
    window->Close(true);
    node = node->GetNext();
  }
  */
  visualizationWindow_->ZeroGraphForm();

  Destroy();
}

void GraphForm::FillOutForm()
{
  UpdateVectors( dataCB_ );
  UpdateVectors( indepCB_ );
  UpdateVectors( errorCB_ );
  //
  symbolCB_->Append( wxT("<none>") );
  symbolCB_->Append( wxT("square") );
  symbolCB_->Append( wxT("square filled") );
  symbolCB_->Append( wxT("cross") );
  symbolCB_->Append( wxT("square with cross") );
  symbolCB_->Append( wxT("plus") );
  symbolCB_->Append( wxT("diamond") );
  symbolCB_->Append( wxT("diamond with plus") );
  symbolCB_->Append( wxT("diamond filled") );
  symbolCB_->Append( wxT("asterix") );
  symbolCB_->Append( wxT("triangle") );
  symbolCB_->Append( wxT("triangle filled") );
  symbolCB_->Append( wxT("circle") );
  symbolCB_->Append( wxT("circle filled") );
  symbolCB_->Append( wxT("star") );
  symbolCB_->Append( wxT("star filled") );
  symbolCB_->Append( wxT("point") );
  symbolCB_->Append( wxT("arrow starting at data") );
  symbolCB_->Append( wxT("arrow centred at data") );
  //
  symbolCB_->SetSelection( 0 );
  connectCkB_->Enable( false );
  //
  minST_->Enable( false );
  minTC_->Enable( false );
  maxST_->Enable( false );
  maxTC_->Enable( false );
}

void GraphForm::UpdateVectors( wxComboBox *cb )
{
  cb->Clear();
  cb->Append( wxT("<none>") );
  NVariableTable *nvTable = NVariableTable::GetTable();
  int entries = nvTable->Entries();
  for( int i=0; i<entries; ++i )
  {
    NumericVariable *nv = nvTable->GetEntry(i);
    if( nv->GetData().GetNumberOfDimensions() == 1 )cb->Append( nv->GetName() );
  }
}

void GraphForm::OnPlotsymbolChange( wxCommandEvent &WXUNUSED(event) )
{
  connectCkB_->Enable( symbolCB_->GetStringSelection() != wxT("<none>") );
}

void GraphForm::OnIndepChange( wxCommandEvent &WXUNUSED(event) )
{
  wxString vecName( indepCB_->GetStringSelection() );
  if( vecName == wxT("<none>") )
  {
    minTC_->Clear();
    minTC_->Enable( false );
    minST_->Enable( false );
    maxTC_->Clear();
    maxTC_->Enable( false );
    maxST_->Enable( false );
  }
  else
  {
    minTC_->Enable( true );
    minST_->Enable( true );
    maxTC_->Enable( true );
    maxST_->Enable( true );
    double imin, imax;
    NumericVariable *vec = NVariableTable::GetTable()->GetVariable( vecName );
    vec->GetData().GetMinMax( imin, imax );
    minTC_->SetValue( wxString()<<imin );
    maxTC_->SetValue( wxString()<<imax );
  }
}

void GraphForm::OnClear( wxCommandEvent &WXUNUSED(event) )
{
  ClearGraphics();
}

void GraphForm::ClearGraphics()
{
  ExGlobals::ClearGraphicsMonitor();
  ExGlobals::ClearWindows();
  color_ = 0;
}
  
void GraphForm::OnDraw( wxCommandEvent &WXUNUSED(event) )
{
  Draw();
}

void GraphForm::Draw()
{
  GRA_window *currentWindow = ExGlobals::GetGraphWindow();
  //
  GRA_setOfCharacteristics *generalC = currentWindow->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *dataCurveC = currentWindow->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *xAxisC = currentWindow->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = currentWindow->GetYAxisCharacteristics();
  //
  std::vector<double> x, y, xe1, ye1, xe2, ye2;
  //
  wxString depName( dataCB_->GetStringSelection() );
  if( depName == wxT("<none>") )
  {
    wxMessageDialog *md =
        new wxMessageDialog( this, wxT("a dependent vector must be chosen"), wxT("Fatal error"), wxOK|wxICON_ERROR );
    md->ShowModal();
    return;
  }
  try
  {
    NumericVariable::GetVector( depName, wxT("dependent variable"), y );
  }
  catch( EVariableError &e )
  {
    wxMessageDialog *md =
        new wxMessageDialog( this, wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR );
    md->ShowModal();
    return;
  }
  wxString indepName( indepCB_->GetStringSelection() );
  double minRange, maxRange;
  if( indepName == wxT("<none>") )
  {
    std::size_t size = y.size();
    for( std::size_t i=0; i<size; ++i )x.push_back( static_cast<double>(i+1) );
    minRange = 1;
    maxRange = size;
  }
  else                 // x was chosen from the list
  {
    try
    {
      NumericVariable::GetVector( indepName, wxT("independent variable"), x );
    }
    catch( EVariableError &e )
    {
      wxMessageDialog *md =
          new wxMessageDialog( this, wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR );
      md->ShowModal();
      return;
    }
    if( !minTC_->GetValue().ToDouble(&minRange) )
    {
      wxMessageDialog *md =
          new wxMessageDialog( this, wxT("invalid value entered for min"), wxT("Fatal error"), wxOK|wxICON_ERROR );
      md->ShowModal();
      return;
    }
    if( !maxTC_->GetValue().ToDouble(&maxRange) )
    {
      wxMessageDialog *md =
          new wxMessageDialog( this, wxT("invalid value entered for max"), wxT("Fatal error"), wxOK|wxICON_ERROR );
      md->ShowModal();
      return;
    }
  }
  wxString errorName( errorCB_->GetStringSelection() );
  if( errorName != wxT("<none>") )
  {
    try
    {
      NumericVariable::GetVector( errorName, wxT("error vector"), ye1 );
    }
    catch( EVariableError &e )
    {
      wxMessageDialog *md =
          new wxMessageDialog( this, wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR );
      md->ShowModal();
      return;
    }
  }
  std::size_t num = std::min( x.size(), y.size() );
  if( num < x.size() )x.resize( num );
  if( num < y.size() )y.resize( num );
  if( !ye1.empty() )
  {
    num = std::min( num, ye1.size() );
    if( num < x.size() )x.resize( num );
    if( num < y.size() )y.resize( num );
    if( num < ye1.size() )ye1.resize( num );
  }
  std::vector<double> newX, newY, newE;
  for( std::size_t i=0; i<num; ++i )
  {
    if( x[i]>=minRange && x[i]<=maxRange )
    {
      newX.push_back( x[i] );
      newY.push_back( y[i] );
      if( !ye1.empty() )newE.push_back( ye1[i] );
    }
  }
  if( newX.empty() )
  {
    wxMessageDialog *md =
        new wxMessageDialog( this, wxT("there are no data values within the specified independent vector range"),
                             wxT("Fatal error"), wxOK|wxICON_ERROR );
    md->ShowModal();
    return;
  }
  num = newX.size();
  --color_;
  if( color_ == -6 )--color_; // to not use yellow (which is hard to see)
  static_cast<GRA_boolCharacteristic*>(generalC->Get(wxT("GRAPHBOX")))->Set( true );
  //
  int ps = symArray_[symbolCB_->GetSelection()];
  if( !connectCkB_->IsChecked() )ps *= -1;
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOL")))->Set( ps );
  //
  if( ps!=0 && num>=200 )
    static_cast<GRA_sizeCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOLSIZE")))->SetAsPercent( 0.5 );
  static_cast<GRA_colorCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOLCOLOR")))->Set( GRA_colorControl::GetColor(color_) );
  static_cast<GRA_colorCharacteristic*>(dataCurveC->Get(wxT("CURVECOLOR")))->Set( GRA_colorControl::GetColor(color_) );
  //
  if( indepName != wxT("<none>") )
  {
    static_cast<GRA_stringCharacteristic*>(xAxisC->Get(wxT("LABEL")))->Set( indepName );
    static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("LABELON")))->Set( true );
  }
  else static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("LABELON")))->Set( false );
  //
  static_cast<GRA_stringCharacteristic*>(yAxisC->Get(wxT("LABEL")))->Set( depName );
  static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("LABELON")))->Set( true );
  //
  std::vector<double> x2(2), y2(2);
  UsefulFunctions::MinMax( newX, 0, num, x2[0], x2[1] );
  if( !newE.empty() )
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( y2[1] < newY[i]+newE[i] )y2[1] = newY[i]+newE[i];
      if( y2[0] > newY[i]-newE[i] )y2[0] = newY[i]-newE[i];
    }
  }
  else
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( y2[1] < newY[i] )y2[1] = newY[i];
      if( y2[0] > newY[i] )y2[0] = newY[i];
    }
  }
  GRA_cartesianAxes *cartesianAxes = 0;
  GRA_cartesianCurve *cartesianCurve = 0;
  try
  {
    cartesianAxes = new GRA_cartesianAxes(x2,y2,false,false);
    cartesianCurve = new GRA_cartesianCurve(newX,newY,xe1,newE,xe2,ye2,false);
    cartesianAxes->Make();
    cartesianCurve->Make();
  }
  catch (EGraphicsError &e)
  {
    delete cartesianAxes;
    delete cartesianCurve;
    wxMessageDialog *md =
        new wxMessageDialog( this, wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR );
    md->ShowModal();
    return;
  }
  wxClientDC dc( ExGlobals::GetwxWindow() );
  cartesianAxes->Draw( ExGlobals::GetGraphicsOutput(), dc );
  cartesianCurve->Draw( ExGlobals::GetGraphicsOutput(), dc );

  currentWindow->AddDrawableObject( cartesianAxes );
  currentWindow->AddDrawableObject( cartesianCurve );

  ExGlobals::ReplotCurrentWindow();
  ExGlobals::RefreshGraphics();
  //
  if( ExGlobals::StackIsOn() )
  {
    wxString line( wxT("FORM GRAPH,") );
    int ps = symArray_[symbolCB_->GetSelection()];
    if( !connectCkB_->IsChecked() )ps *= -1;
    if( indepName == wxT("<none>") )indepName.clear();
    if( errorName == wxT("<none>") )errorName.clear();
    line += depName + wxT(",") + indepName + wxT(",") +
        minTC_->GetValue() + wxT(",") + maxTC_->GetValue() +
        wxT(",") + errorName + wxT(",") + (wxString()<<ps) + wxT(",DRAW");
    ExGlobals::WriteStack( line );
  }
}

void GraphForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

void GraphForm::Set( wxString const &depName, wxString const &indepName,
                     double idmin, double idmax, wxString const &errorName,
                     int ps, wxString const &action )
{
  dataCB_->SetStringSelection( depName.Upper() );
  indepCB_->SetStringSelection( indepName.Upper() );
  minTC_->Enable( true );
  maxTC_->Enable( true );
  minST_->Enable( true );
  maxST_->Enable( true );
  minTC_->SetValue( wxString() << idmin );
  maxTC_->SetValue( wxString() << idmax );
  errorCB_->SetStringSelection( errorName.Upper() );
  symbolCB_->SetSelection( wxNOT_FOUND );
  int size = symArray_.size();
  for( int i=0; i<size; ++i )
  {
    if( abs(ps) == symArray_[i] )
    {
      symbolCB_->SetSelection( i );
      break;
    }
  }
  connectCkB_->SetValue( ps>=0 );
  if( action == wxT("CLEAR") )
  {
    ClearGraphics();
  }
  else if( action == wxT("DRAW") )
  {
    Draw();
  }
  else if( action == wxT("CLOSE") )
  {
    Close();
  }
}

// end of file
