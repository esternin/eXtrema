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
#include "wx/datetime.h"

#include <wx/persist/toplevel.h>

#include "FitForm.h"
#include "AnalysisWindow.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "NVariableTable.h"
#include "EVariableError.h"
#include "ExGlobals.h"
#include "UsefulFunctions.h"
#include "GRA_window.h"
#include "GRA_intCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_stringCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_colorControl.h"
#include "GRA_fontControl.h"
#include "GRA_setOfCharacteristics.h"
#include "EGraphicsError.h"
#include "GRA_cartesianAxes.h"
#include "GRA_cartesianCurve.h"
#include "GRA_drawableText.h"
#include "Expression.h"
#include "EExpressionError.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( FitForm, wxFrame )
  EVT_COMBOBOX( ID_dvec, FitForm::OnDataVectorChange )
  EVT_COMBOBOX( ID_ivec, FitForm::OnIndepVectorChange )
  EVT_COMBOBOX( ID_evec, FitForm::OnErrorVectorChange )
  EVT_COMBOBOX( ID_type, FitForm::OnFitTypeChange )
  EVT_COMBOBOX( ID_degree, FitForm::OnDegreeChange )
  EVT_BUTTON( ID_test, FitForm::OnTest )
  EVT_BUTTON( ID_clear, FitForm::OnClear )
  EVT_BUTTON( wxID_APPLY, FitForm::OnApply )
  EVT_BUTTON( wxID_CLOSE, FitForm::OnClose )
  EVT_CLOSE( FitForm::CloseEventHandler )
END_EVENT_TABLE()

FitForm::FitForm( AnalysisWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("Fit"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      analysisWindow_(parent)
{
  CreateForm();
  
  wxConfigBase *config = wxConfigBase::Get();
  wxPersistentRegisterAndRestore(this, "FitForm");

  Show( true );

  FillOutForm();

  wxString sTmp;
  config->Read( wxT("/FitForm/DATA_VECTOR"), &sTmp, wxT("") );
  if( dataCB_->FindString(sTmp) > 0 )
  {
    dVecName_ = sTmp;
    dataCB_->SetStringSelection( sTmp );
    exprST_->SetLabel( wxString(wxT("   Fit expression: "))+dVecName_+wxT(" = ") );
    indepCB_->Enable( true );
    indepST_->Enable( true );
    sTmp.clear();
    config->Read( wxT("/FitForm/INDEPENDENT_VECTOR"), &sTmp, wxT("") );
    if( indepCB_->FindString(sTmp) > 0 )
    {
      iVecName_ = sTmp;
      indepCB_->SetStringSelection( sTmp );
      minST_->Enable( true );
      minTC_->Enable( true );
      maxST_->Enable( true );
      maxTC_->Enable( true );
      double imin, imax;
      iVec_ = NVariableTable::GetTable()->GetVariable( iVecName_ );
      iVec_->GetData().GetMinMax( imin, imax );
      sTmp.clear();
      minTC_->SetValue( sTmp<<imin );
      sTmp.clear();
      maxTC_->SetValue( sTmp<<imax );
      errorCB_->Enable( true );
      errorST_->Enable( true );
      sTmp.clear();
      config->Read( wxT("/FitForm/ERROR_VECTOR"), &sTmp, wxT("") );
      if( errorCB_->FindString(sTmp) > 0 )
      {
        eVecName_ = sTmp;
        errorCB_->SetStringSelection( sTmp );
      }
      typeCB_->Enable( true );
      typeST_->Enable( true );
      sTmp.clear();
      config->Read( wxT("/FitForm/FIT_TYPE"), &sTmp, wxT("") );
      if( typeCB_->FindString(sTmp) >= 0 )
      {
        typeCB_->SetStringSelection( sTmp );
        FitTypeChange();
        wxString type( typeCB_->GetStringSelection() );
        int nParams = -1;
        if( type==wxT("Legendre polynomials") || type==wxT("Polynomial") || type==wxT("Custom") )
        {
          sTmp.clear();
          config->Read( wxT("/FitForm/DEGREE"), &sTmp, wxT("") );
          long n;
          if( sTmp.ToLong(&n) )
          {
            nParams = type==wxT("Custom") ? n : n+1;
            degreeCB_->SetStringSelection( sTmp );
          }
        }
        sTmp.clear();
        config->Read( wxT("/FitForm/EXPRESSION"), &sTmp, wxT("") );
        exprTC_->SetValue( sTmp );
        for( int i=0; i<nParams; ++i )
        {
          sTmp.clear();
          config->Read( wxString(wxT("/FitForm/STARTVALUE_"))<<i, &sTmp, wxT("") );
          bool bTmp = false;
          config->Read( wxString(wxT("/FitForm/FIXED_"))<<i, &bTmp, false );
          SetupParameterFields( i, true, bTmp, sTmp );
        }
        UpdateExpression();
      }
    }
  }
  sTmp.clear();
  config->Read( wxT("/FitForm/XLOC"), &sTmp, wxT("53.0") );
  legxTC_->SetValue( sTmp );
  sTmp.clear();
  config->Read( wxT("/FitForm/YLOC"), &sTmp, wxT("89.0") );
  legyTC_->SetValue( sTmp );

  Layout();
}

void FitForm::CreateForm()
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  mainSizer->AddSpacer( 10 );
  
  wxPanel *topPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL );

  wxPanel *topRightPanel = new wxPanel( topPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxGridSizer *topRightSizer = new wxGridSizer( 5, 2, 5, 5 );

  dataST_ = new wxStaticText( topRightPanel, wxID_ANY, wxT("Data vector") );
  topRightSizer->Add( dataST_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  dataCB_ = new wxComboBox( topRightPanel, ID_dvec, wxT(""), wxDefaultPosition, wxSize(170,25), 0, 0, wxCB_READONLY );
  topRightSizer->Add( dataCB_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  indepST_ = new wxStaticText( topRightPanel, wxID_ANY, wxT("Independent vector") );
  topRightSizer->Add( indepST_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  indepCB_ = new wxComboBox( topRightPanel, ID_ivec, wxT(""), wxDefaultPosition, wxSize(170,25), 0, 0, wxCB_READONLY );
  topRightSizer->Add( indepCB_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  errorST_ = new wxStaticText( topRightPanel, wxID_ANY, wxT("Error vector") );
  topRightSizer->Add( errorST_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  errorCB_ = new wxComboBox( topRightPanel, ID_evec, wxT(""), wxDefaultPosition, wxSize(170,25), 0, 0, wxCB_READONLY );
  topRightSizer->Add( errorCB_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  typeST_ = new wxStaticText( topRightPanel, wxID_ANY, wxT("Type of fit") );
  topRightSizer->Add( typeST_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  typeCB_ = new wxComboBox( topRightPanel, ID_type, wxT(""), wxDefaultPosition, wxSize(170,25), 0, 0, wxCB_READONLY );
  topRightSizer->Add( typeCB_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  degreeST_ = new wxStaticText( topRightPanel, wxID_ANY, wxT("Degree of polynomial") );
  topRightSizer->Add( degreeST_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  degreeCB_ = new wxComboBox( topRightPanel, ID_degree, wxT(""), wxDefaultPosition, wxSize(70,25), 0, 0, wxCB_READONLY );
  topRightSizer->Add( degreeCB_, wxSizerFlags(0).Left().Border(wxALL,2) );
  topRightPanel->SetSizer( topRightSizer );
  topSizer->Add( topRightPanel, wxSizerFlags(0).Left().Border(wxALL,1) );

  wxPanel *topMidPanel = new wxPanel( topPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *topMidSizer = new wxBoxSizer( wxVERTICAL );

  topMidSizer->AddSpacer( 14 );

  wxPanel *rangePanel = new wxPanel( topMidPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
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
  topMidSizer->Add( rangePanel, wxSizerFlags(0).Left().Border(wxALL,1) );
  topMidPanel->SetSizer( topMidSizer );
  topSizer->Add( topMidPanel, wxSizerFlags(0).Left().Border(wxALL,1) );

  topSizer->AddSpacer( 25 );

  wxPanel *resultsPanel = new wxPanel( topPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER );
  wxGridSizer *resultsSizer = new wxGridSizer( 3, 2, 1, 1 );

  resultsSizer->Add( new wxStaticText(resultsPanel,wxID_ANY,wxT("")), wxSizerFlags(0).Left().Border(wxTOP,10) );
  resultsSizer->Add( new wxStaticText(resultsPanel,wxID_ANY,wxT(" Fit Results")), wxSizerFlags(0).Left().Border(wxTOP,10) );

  resultsSizer->Add( new wxStaticText(resultsPanel,wxID_ANY,wxT("Total chi square")), wxSizerFlags(0).Right().Border(wxTOP,10) );
  chiTC_ = new wxTextCtrl( resultsPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(120,25), wxTE_READONLY );
  resultsSizer->Add( chiTC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  resultsSizer->Add( new wxStaticText(resultsPanel,wxID_ANY,wxT("Confidence level")), wxSizerFlags(0).Right().Border(wxTOP,10) );
  confTC_ = new wxTextCtrl( resultsPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(120,25), wxTE_READONLY );
  resultsSizer->Add( confTC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  
  resultsSizer->Add( new wxStaticText(resultsPanel,wxID_ANY,wxT("  Degrees of freedom")), wxSizerFlags(0).Right().Border(wxTOP,10) );
  degFreeTC_ = new wxTextCtrl( resultsPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(120,25), wxTE_READONLY );
  resultsSizer->Add( degFreeTC_, wxSizerFlags(0).Left().Border(wxALL,10) );

  resultsPanel->SetSizer( resultsSizer );

  topSizer->Add( resultsPanel, wxSizerFlags(0).Left().Border(wxALL,1) );
  topPanel->SetSizer( topSizer );
  mainSizer->Add( topPanel, wxSizerFlags(0).Center().Border(wxALL,1) );

  wxPanel *exprPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *exprSizer = new wxBoxSizer( wxHORIZONTAL );

  exprST_ = new wxStaticText( exprPanel, wxID_ANY, wxT("   Fit expression: Data vector = ") );
  exprSizer->Add( exprST_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  exprTC_ = new wxTextCtrl( exprPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(520,25), wxTE_DONTWRAP );
  exprSizer->Add( exprTC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  exprPanel->SetSizer( exprSizer );
  mainSizer->Add( exprPanel, wxSizerFlags(0).Center().Border(wxALL,5) );

  wxPanel *parmPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxFlexGridSizer *parmSizer = new wxFlexGridSizer( 9, 4, 5, 5 );

  parmSizer->Add( new wxStaticText(parmPanel,wxID_ANY,wxT("Fix     ")), wxSizerFlags(0).Right().Border(wxTOP,10) );
  parmSizer->Add( new wxStaticText(parmPanel,wxID_ANY,wxT("Parameters")), wxSizerFlags(0).Center().Border(wxTOP,10) );
  parmSizer->Add( new wxStaticText(parmPanel,wxID_ANY,wxT("Start values")), wxSizerFlags(0).Center().Border(wxTOP,10) );
  parmSizer->Add( new wxStaticText(parmPanel,wxID_ANY,wxT("Fit results")), wxSizerFlags(0).Center().Border(wxTOP,10) );

  for( std::size_t i=0; i<8; ++i )
  {
    pCkB_[i] = new wxCheckBox( parmPanel, wxID_ANY, wxT("") );
    parmSizer->Add( pCkB_[i], wxSizerFlags(0).Left().Border(wxTOP,4) );
    pnameTC_[i] = new wxTextCtrl( parmPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(200,25), wxTE_READONLY );
    parmSizer->Add( pnameTC_[i], wxSizerFlags(0) );
    pstartTC_[i] = new wxTextCtrl( parmPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(200,25), wxTE_DONTWRAP );
    parmSizer->Add( pstartTC_[i], wxSizerFlags(0) );
    presultTC_[i] = new wxTextCtrl( parmPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(200,25), wxTE_READONLY );
    parmSizer->Add( presultTC_[i], wxSizerFlags(0) );
  }

  parmPanel->SetSizer( parmSizer );
  mainSizer->Add( parmPanel, wxSizerFlags(0).Center().Border(wxALL,5) );

  wxPanel *legendPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *legendSizer = new wxBoxSizer( wxHORIZONTAL );

  legendSizer->Add( new wxStaticText(legendPanel,wxID_ANY,wxT("Legend location:  %x ")), wxSizerFlags(0).Left().Border(wxTOP,5) );
  legxTC_ = new wxTextCtrl( legendPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(160,25), wxTE_DONTWRAP );
  legendSizer->Add( legxTC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  legendSizer->Add( new wxStaticText(legendPanel,wxID_ANY,wxT("   %y")), wxSizerFlags(0).Left().Border(wxTOP,5) );
  legyTC_ = new wxTextCtrl( legendPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(160,25), wxTE_DONTWRAP );
  legendSizer->Add( legyTC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  legendPanel->SetSizer( legendSizer );
  mainSizer->Add( legendPanel, wxSizerFlags(0).Center().Border(wxALL,10) );

  wxPanel *bottomPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );

  testButton_ = new wxButton( bottomPanel, ID_test, wxT("Test the fit") );
  testButton_->SetToolTip( wxT("click to plot data and the fit expression using the current parameter values") );
  bottomSizer->Add( testButton_, wxSizerFlags(0).Border(wxALL,10) );

  wxButton *clearButton = new wxButton( bottomPanel, ID_clear, wxT("Clear graphics") );
  clearButton->SetToolTip( wxT("click to clear the visualization window") );
  bottomSizer->Add( clearButton, wxSizerFlags(0).Border(wxALL,10) );
  
  applyButton_ = new wxButton( bottomPanel, wxID_APPLY, wxT("Do the fit") );
  applyButton_->SetToolTip( wxT("calculate the fit parameters and plot the result") );
  bottomSizer->Add( applyButton_, wxSizerFlags(0).Border(wxALL,10) );
  
  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close") );
  closeButton->SetToolTip( wxT("close this form") );
  bottomSizer->Add( closeButton, wxSizerFlags(0).Border(wxALL,10) );
  
  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(0).Centre().Border(wxALL,1) );
  
  SetSizer( mainSizer );
}

void FitForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    //
    config->Write( wxT("/FitForm/DATA_VECTOR"), dataCB_->GetStringSelection() );
    config->Write( wxT("/FitForm/INDEPENDENT_VECTOR"), indepCB_->GetStringSelection() );
    config->Write( wxT("/FitForm/ERROR_VECTOR"), errorCB_->GetStringSelection() );
    wxString type( typeCB_->GetStringSelection() );
    config->Write( wxT("/FitForm/FIT_TYPE"), type );
    int nParams = 0;
    if( type==wxT("Legendre polynomials") || type==wxT("Polynomial") || type==wxT("Custom") )
    {
      wxString s( degreeCB_->GetStringSelection() );
      long nl;
      s.ToLong( &nl );
      nParams = static_cast<int>(nl);
      if( type==wxT("Legendre polynomials") || type==wxT("Polynomial") )++nParams;
      config->Write( wxT("/FitForm/DEGREE"), s );
    }
    else if( type == wxT("Gaussian") )nParams = 3;
    else if( type == wxT("Least squares line") )nParams = 2;
    else if( type == wxT("Constant") )nParams = 1;
    config->Write( wxT("/FitForm/EXPRESSION"), exprTC_->GetValue() );
    for( int i=0; i<nParams; ++i )
    {
      config->Write( wxString(wxT("/FitForm/STARTVALUE_"))<<i, pstartTC_[i]->GetValue() );
      config->Write( wxString(wxT("/FitForm/FIXED_"))<<i, pCkB_[i]->IsChecked() );
    }
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
  analysisWindow_->ZeroFit();

  Destroy();
}

void FitForm::FillOutForm()
{
  UpdateVectors( dataCB_ );
  UpdateVectors( indepCB_ );
  UpdateVectors( errorCB_ );
  //
  typeCB_->Append( wxT("Polynomial") );
  typeCB_->Append( wxT("Legendre polynomials") );
  typeCB_->Append( wxT("Gaussian") );
  typeCB_->Append( wxT("Least squares line") );
  typeCB_->Append( wxT("Constant") );
  typeCB_->Append( wxT("Custom") );
  //
  degreeCB_->Append( wxT("1") );
  degreeCB_->Append( wxT("2") );
  degreeCB_->Append( wxT("3") );
  degreeCB_->Append( wxT("4") );
  degreeCB_->Append( wxT("5") );
  degreeCB_->Append( wxT("6") );
  degreeCB_->Append( wxT("7") );
  degreeCB_->Append( wxT("8") );
  //
  for( std::size_t i=0; i<8; ++i )
  {
    pCkB_[i]->SetValue( false );
    pCkB_[i]->Enable( false );
    pnameTC_[i]->Clear();
    pnameTC_[i]->Enable( false );
    pstartTC_[i]->Clear();
    pstartTC_[i]->Enable( false );
    presultTC_[i]->Clear();
    presultTC_[i]->Enable( false );
  }
  //
  dataCB_->Enable( true );
  dataST_->Enable( true );
  indepCB_->Enable( false );
  indepST_->Enable( false );
  minST_->Enable( false );
  minTC_->Enable( false );
  maxST_->Enable( false );
  maxTC_->Enable( false );
  errorCB_->Enable( false );
  errorST_->Enable( false );
  typeCB_->Enable( false );
  typeST_->Enable( false );
  degreeCB_->Enable( false );
  degreeST_->Enable( false );
  exprTC_->Enable( true );
  exprST_->Enable( true );
  //
  eVecName_.clear();
  pStartValues_.clear();
}

void FitForm::UpdateVectors( wxComboBox *cb )
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

void FitForm::ClearResults()
{
  chiTC_->Clear();
  confTC_->Clear();
  degFreeTC_->Clear();
  for( std::size_t i=0; i<8; ++i )presultTC_[i]->Clear();
}

void FitForm::SetupParameterFields( int i, bool enabled, bool checked, wxString const &value )
{
  pCkB_[i]->SetValue( checked );
  pCkB_[i]->Enable( enabled );
  pnameTC_[i]->Clear();
  pnameTC_[i]->Enable( enabled );
  pstartTC_[i]->Clear();
  pstartTC_[i]->Enable( enabled );
  if( enabled )
  {
    pnameTC_[i]->SetValue( wxString(wxT("a"))<<i );
    pstartTC_[i]->SetValue( value );
  }
  presultTC_[i]->Enable( enabled );
}

void FitForm::UpdateExpression()
{
  dVecName_ = dataCB_->GetStringSelection();
  iVecName_ = indepCB_->GetStringSelection();
  int n = -1;
  fitExpression_.clear();
  fitTitle_.clear();
  wxString selection( typeCB_->GetStringSelection() );
  if( selection == wxT("Legendre polynomials") )
  {
    long nl;
    degreeCB_->GetStringSelection().ToLong( &nl );
    n = static_cast<int>(nl);
    for( int i=0; i<n; ++i )
    {
      fitExpression_ += pnameTC_[i]->GetValue() + wxT("*LEGENDRE(");
      fitExpression_ << i;
      fitExpression_ += wxT(",") + iVecName_ + wxT(")+");
      fitTitle_ += pnameTC_[i]->GetValue() + wxT("*P<_>");
      fitTitle_ << i;
      fitTitle_ += wxT("<^>(") + iVecName_ + wxT(")+");
    }
    fitExpression_ += pnameTC_[n]->GetValue() + wxT("*LEGENDRE(");
    fitExpression_ << n;
    fitExpression_ += wxT(",") + iVecName_ + wxT(")");
    fitTitle_ += pnameTC_[n]->GetValue() + wxT("*P<_>");
    fitTitle_ << n;
    fitTitle_ += wxT("<^>(") + iVecName_ + wxT(")");
    exprTC_->SetValue( fitExpression_ );
  }
  else if( selection == wxT("Polynomial") )
  {
    long nl;
    degreeCB_->GetStringSelection().ToLong( &nl );
    n = static_cast<int>(nl);
    fitExpression_ = pnameTC_[0]->GetValue() + wxT("+") + pnameTC_[1]->GetValue() + wxT("*") + iVecName_;
    fitTitle_ = pnameTC_[0]->GetValue() + wxT("+") + pnameTC_[1]->GetValue() + wxT("*") + iVecName_;
    for( int i=2; i<=n; ++i )
    {
      fitExpression_ += wxT("+") + pnameTC_[i]->GetValue() + wxT("*") + iVecName_ + wxT("^");
      fitExpression_ << i;
      fitTitle_ += wxT("+") + pnameTC_[i]->GetValue() + wxT("*") + iVecName_ + wxT("<^>");
      fitTitle_ << i;
      fitTitle_ += wxT("<_>");
    }
    exprTC_->SetValue( fitExpression_ );
  }
  else if( selection == wxT("Gaussian") )
  {
    fitExpression_ = pnameTC_[0]->GetValue() + wxT("*exp(-(") + iVecName_ + wxT("-") +
        pnameTC_[1]->GetValue() + wxT(")^2/") + pnameTC_[2]->GetValue() + wxT("^2)");
    fitTitle_ = pnameTC_[0]->GetValue() + wxT("*exp(") + iVecName_ + wxT("-") +
        pnameTC_[1]->GetValue() + wxT(")<^>2<_>/") + pnameTC_[2]->GetValue() + wxT("<^>2<_>)");
    exprTC_->SetValue( fitExpression_ );
  }
  else if( selection == wxT("Least squares line") )
  {
    fitExpression_ = pnameTC_[0]->GetValue() + wxT("+") + pnameTC_[1]->GetValue() + wxT("*") + iVecName_;
    fitTitle_ = pnameTC_[0]->GetValue() + wxT("+") + pnameTC_[1]->GetValue() + wxT("*") + iVecName_;
    exprTC_->SetValue( fitExpression_ );
  }
  else if( selection == wxT("Constant") )
  {
    fitExpression_ = pnameTC_[0]->GetValue();
    fitTitle_ = pnameTC_[0]->GetValue();
    exprTC_->SetValue( fitExpression_ );
  }
  else // Custom
  {
    fitExpression_ = exprTC_->GetValue();
    fitTitle_ = fitExpression_;
  }
}

void FitForm::OnDataVectorChange( wxCommandEvent &WXUNUSED(event) )
{
  dVecName_ = dataCB_->GetStringSelection();
  indepCB_->Enable( true );
  indepST_->Enable( true );
  pStartValues_.clear();
  exprST_->SetLabel( wxString(wxT("   Fit expression: "))+dVecName_+wxT(" = ") );
  ClearResults();
  Layout();
}

void FitForm::OnIndepVectorChange( wxCommandEvent &WXUNUSED(event) )
{
  iVecName_ = indepCB_->GetStringSelection();
  minTC_->Enable( true );
  minST_->Enable( true );
  maxTC_->Enable( true );
  maxST_->Enable( true );
  double imin, imax;
  iVec_ = NVariableTable::GetTable()->GetVariable( iVecName_ );
  iVec_->GetData().GetMinMax( imin, imax );
  wxString s;
  minTC_->SetValue( s<<imin );
  s.clear();
  maxTC_->SetValue( s<<imax );
  //
  //long nl;
  //typeCB_->GetStringSelection().ToLong( &nl );
  //int type = static_cast<int>(nl);
  //degreeCB_->GetStringSelection().ToLong( &nl );
  //if( type==2 || type==3 || type==4 || nl>=0L )UpdateExpression();
  errorST_->Enable( true );
  errorCB_->Enable( true );
  typeCB_->Enable( true );
  typeST_->Enable( true );
  FitTypeChange();
  UpdateExpression();
  pStartValues_.clear();
  ClearResults();
}

void FitForm::OnErrorVectorChange( wxCommandEvent &WXUNUSED(event) )
{
  eVecName_.clear();
  wxString s( errorCB_->GetStringSelection() );
  if( s != wxT("<none>") )eVecName_ = s;
}

void FitForm::OnFitTypeChange( wxCommandEvent &WXUNUSED(event) )
{
  FitTypeChange();
  UpdateExpression();
  ClearResults();
}

void FitForm::FitTypeChange()
{
  wxString selection( typeCB_->GetStringSelection() );
  if( selection == wxT("Legendre polynomials") )
  {
    degreeST_->SetLabel( wxT("Degree of polynomial") );
    degreeST_->Enable( true );
    degreeCB_->Enable( true );
    degreeCB_->SetSelection( 0 );
    SetupParameterFields( 0, true, false, wxT("1.0") );
    SetupParameterFields( 1, true, false, wxT("1.0") );
    for( std::size_t i=2; i<8; ++i )SetupParameterFields( i, false, false );
    if( degreeCB_->GetCount() == 8 )degreeCB_->Delete(7);
    exprTC_->Clear();
    exprTC_->SetEditable( false );
  }
  else if( selection == wxT("Polynomial") )
  {
    degreeST_->SetLabel( wxT("Degree of Polynomial") );
    degreeST_->Enable( true );
    degreeCB_->Enable( true );
    degreeCB_->SetSelection( 0 );
    SetupParameterFields( 0, true, false, wxT("1.0") );
    SetupParameterFields( 1, true, false, wxT("1.0") );
    for( std::size_t i=2; i<8; ++i )SetupParameterFields( i, false, false );
    if( degreeCB_->GetCount() == 8 )degreeCB_->Delete(7);
    exprTC_->Clear();
    exprTC_->SetEditable( false );
  }
  else if( selection == wxT("Gaussian") )
  {
    degreeST_->Enable( false );
    degreeCB_->Enable( false );
    //
    SetupParameterFields( 0, true, false, wxT("1.0") );
    SetupParameterFields( 1, true, false, wxT("1.0") );
    SetupParameterFields( 2, true, false, wxT("1.0") );
    for( std::size_t i=3; i<8; ++i )SetupParameterFields( i, false, false );
    exprTC_->Clear();
    exprTC_->SetEditable( false );
  }
  else if( selection == wxT("Least squares line") )
  {
    degreeST_->Enable( false );
    degreeCB_->Enable( false );
    //
    SetupParameterFields( 0, true, false, wxT("1.0") );
    SetupParameterFields( 1, true, false, wxT("1.0") );
    for( std::size_t i=2; i<8; ++i )SetupParameterFields( i, false, false );
    exprTC_->Clear();
    exprTC_->SetEditable( false );
  }
  else if( selection == wxT("Constant") )
  {
    degreeST_->Enable( false );
    degreeCB_->Enable( false );
    //
    SetupParameterFields( 0, true, false, wxT("1.0") );
    for( std::size_t i=1; i<8; ++i )SetupParameterFields( i, false, false );
    exprTC_->SetEditable( false );
  }
  else // Custom
  {
    degreeST_->SetLabel( wxT("Number of parameters") );
    degreeST_->Enable( true );
    degreeCB_->Enable( true );
    degreeCB_->SetSelection( 0 );
    SetupParameterFields( 0, true, false, wxT("1.0") );
    for( std::size_t i=1; i<8; ++i )SetupParameterFields( i, false, false );
    if( degreeCB_->GetCount() == 7 )degreeCB_->Append( wxT("8") );
    exprTC_->Clear();
    exprTC_->SetEditable( true );
  }
  pStartValues_.clear();
}

void FitForm::OnDegreeChange( wxCommandEvent &WXUNUSED(event) )
{
  DegreeChange();
}

void FitForm::DegreeChange()
{
  long nl;
  degreeCB_->GetStringSelection().ToLong( &nl );
  int degree = static_cast<int>(nl);
  //
  // if custom fit, degree is number of parameters, otherwise value is degree of polynomial,
  // and there is one more parameter than the degree of the polynomial
  //
  wxString type( typeCB_->GetStringSelection() );
  int nparams = type==wxT("Custom") ? degree : degree+1;
  for( int i=0; i<nparams; ++i )SetupParameterFields( i, true, false, wxT("1.0") );
  for( int i=nparams; i<8; ++i )SetupParameterFields( i, false, false );
  UpdateExpression();
  pStartValues_.clear();
  ClearResults();
}

void FitForm::CreateParameters()
{
  bool fixed[8];
  for( std::size_t i=0; i<8; ++i )fixed[i] = pCkB_[i]->IsChecked();
  std::size_t i = 0;
  while ( i<8 && pnameTC_[i]->IsEnabled() )
  {
    wxString name( pnameTC_[i]->GetValue() );
    double startValue;
    if( !pstartTC_[i]->GetValue().ToDouble(&startValue) )
    {
      wxMessageBox( wxString(wxT("invalid start value entered for parameter "))<<i,
                    wxT("Fatal error"), wxOK|wxICON_ERROR, this );
      return;
    }
    NVariableTable::GetTable()->RemoveEntry( name );
    NumericVariable *nv = NumericVariable::PutVariable( name, startValue, pvStr_ );
    if( !pCkB_[i]->IsChecked() )nv->GetData().SetFit();
    else                        presultTC_[i]->SetValue( pstartTC_[i]->GetValue() );
    ++i;
  }
}

void FitForm::OnTest( wxCommandEvent &WXUNUSED(event) )
{
  TestTheFit();
}

void FitForm::TestTheFit()
{
  UpdateExpression();
  CreateParameters();
  //
  eVecName_.clear();
  if( errorCB_->GetSelection() > 0 )eVecName_ = errorCB_->GetStringSelection();
  //
  int nParams = 0;
  while ( pnameTC_[nParams]->IsEnabled() )++nParams;
  pStartValues_.push_back( std::vector<double>(nParams) );
  for( int i=0; i<nParams; ++i )
  {
    if( !pstartTC_[i]->GetValue().ToDouble(&pStartValues_.back().at(i)) )
    {
      wxMessageBox( wxString(wxT("invalid start value entered for parameter "))<<i,
                    wxT("Fatal error"), wxOK|wxICON_ERROR, this );
      return;
    }
  }
  GRA_window *currentWindow = ExGlobals::GetGraphWindow();
  //
  GRA_setOfCharacteristics *generalC = currentWindow->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *dataCurveC = currentWindow->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *xAxisC = currentWindow->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = currentWindow->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *textC = currentWindow->GetTextCharacteristics();
  //
  GRA_setOfCharacteristics generalCsave( *generalC );
  GRA_setOfCharacteristics dataCurveCsave( *dataCurveC );
  GRA_setOfCharacteristics xAxisCsave( *xAxisC );
  GRA_setOfCharacteristics yAxisCsave( *yAxisC );
  GRA_setOfCharacteristics textCsave( *textC );
  //
  currentWindow->Clear();
  currentWindow->Erase();
  std::vector<double> y, x, xe1, ye1, xe2, ye2;
  try
  {
    NumericVariable::GetVector( dVecName_, wxT("data variable"), y );
    NumericVariable::GetVector( iVecName_, wxT("independent variable"), x );
    if( !eVecName_.empty() )NumericVariable::GetVector( eVecName_, wxT("error variable"), ye1 );
  }
  catch( EVariableError &e )
  {
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  std::size_t num = std::min( x.size(), y.size() );
  if( num < x.size() )x.resize( num );
  if( num < y.size() )y.resize( num );
  if( !eVecName_.empty() )
  {
    num = std::min( num, ye1.size() );
    if( num < x.size() )x.resize( num );
    if( num < y.size() )y.resize( num );
    if( num < ye1.size() )ye1.resize( num );
  }
  //
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOL")))->Set( -8 );
  if( num >= 200 )
    static_cast<GRA_sizeCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOLSIZE")))->SetAsPercent( 0.5 );
  static_cast<GRA_colorCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOLCOLOR")))->Set(
    GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_colorCharacteristic*>(dataCurveC->Get(wxT("CURVECOLOR")))->Set(
    GRA_colorControl::GetColor(wxT("BLACK")) );
  //
  static_cast<GRA_stringCharacteristic*>(xAxisC->Get(wxT("LABEL")))->Set( iVecName_ );
  static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("LABELON")))->Set( true );
  //
  static_cast<GRA_stringCharacteristic*>(yAxisC->Get(wxT("LABEL")))->Set( dVecName_ );
  static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("LABELON")))->Set( true );
  //
  static_cast<GRA_boolCharacteristic*>(generalC->Get(wxT("GRAPHBOX")))->Set( true );
  //
  std::vector<double> x2(2), y2(2);
  UsefulFunctions::MinMax( x, 0, num, x2[0], x2[1] );
  if( !ye1.empty() )
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( y2[1] < y[i]+ye1[i] )y2[1] = y[i]+ye1[i];
      if( y2[0] > y[i]-ye1[i] )y2[0] = y[i]-ye1[i];
    }
  }
  else
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( y2[1] < y[i] )y2[1] = y[i];
      if( y2[0] > y[i] )y2[0] = y[i];
    }
  }
  GRA_cartesianAxes *cartesianAxes = 0;
  GRA_cartesianCurve *cartesianCurve = 0;
  try
  {
    cartesianAxes = new GRA_cartesianAxes(x2,y2,false,false);
    cartesianCurve = new GRA_cartesianCurve(x,y,xe1,ye1,xe2,ye2);
    cartesianAxes->Make();
    cartesianCurve->Make();
  }
  catch (EGraphicsError &e)
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete cartesianAxes;
    delete cartesianCurve;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  wxClientDC dc( ExGlobals::GetwxWindow() );
  cartesianAxes->Draw( ExGlobals::GetGraphicsOutput(), dc );
  cartesianCurve->Draw( ExGlobals::GetGraphicsOutput(), dc );
  currentWindow->AddDrawableObject( cartesianAxes );
  currentWindow->AddDrawableObject( cartesianCurve );
  ye1.erase( ye1.begin(), ye1.end() );
  //
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 1 );
  static_cast<GRA_angleCharacteristic*>(textC->Get(wxT("ANGLE")))->Set( 0.0 );
  static_cast<GRA_colorCharacteristic*>(textC->Get(wxT("COLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_fontCharacteristic*>(textC->Get(wxT("FONT")))->Set( GRA_fontControl::GetFont(wxT("SANS")) );
  static_cast<GRA_sizeCharacteristic*>(textC->Get(wxT("HEIGHT")))->SetAsPercent( 3.0 );
  static_cast<GRA_boolCharacteristic*>(textC->Get(wxT("INTERACTIVE")))->Set( false );
  double yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->GetAsPercent();
  double xlaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("LOWERAXIS")))->GetAsPercent();
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsPercent( xlaxis );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( yuaxis+1 );
  //
  wxString title( wxT("FIT: ") );
  title += dVecName_ + wxT(" = ") + fitTitle_;
  GRA_drawableText *dt = new GRA_drawableText( title );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
  currentWindow->AddDrawableObject( dt );
  //
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 3 ); // lower right
  static_cast<GRA_sizeCharacteristic*>(textC->Get(wxT("HEIGHT")))->SetAsPercent( 2.0 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsPercent( 98.0 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( 97.5 );
  wxDateTime now = wxDateTime::Now();
  wxString time = now.FormatTime();
  wxString date = now.FormatDate();
  dt = new GRA_drawableText( time+wxT(" ")+date );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
  currentWindow->AddDrawableObject( dt );
  //
  NumericVariable *nv = NVariableTable::GetTable()->GetVariable( dVecName_ );
  if( !nv->GetOrigin().empty() )
  {
    static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( 94.5 );
    dt = new GRA_drawableText( nv->GetOrigin() );
    try
    {
      dt->Parse();
    }
    catch ( EGraphicsError &e )
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      delete dt;
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
      return;
    }
    dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
    currentWindow->AddDrawableObject( dt );
  }
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 1 ); // lower left
  static_cast<GRA_sizeCharacteristic*>(textC->Get(wxT("HEIGHT")))->SetAsPercent( 3.0 );
  //
  // re-evaluate the last expression
  //
  nv = NVariableTable::GetTable()->GetVariable( iVecName_ );
  std::vector<double> iVec( nv->GetData().GetData() );
  double imin, imax;
  nv->GetData().GetMinMax( imin, imax );
  int newNum = 200;
  double inc = (imax-imin)/(newNum-1);
  std::vector<double> newX( newNum );
  for( int i=0; i<newNum-1; ++i )newX[i] = imin+i*inc;
  newX[newNum-1] = imax;
  nv->GetData().SetData( newX );
  nv->GetData().SetDimMag( 0, newX.size() );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsPercent( xlaxis+3 );
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOL")))->Set( 0 );
  static_cast<GRA_sizeCharacteristic*>(textC->Get(wxT("HEIGHT")))->SetAsPercent( 2.0 );
  double yloc = yuaxis;
  int nTest = pStartValues_.size();
  int icolor = -1;
  for( int i=0; i<nTest; ++i )
  {
    for( int j=0; j<nParams; ++j )
      NVariableTable::GetTable()->GetVariable(pnameTC_[j]->GetValue())->GetData().SetScalarValue( pStartValues_[i][j] );
    Expression expr( fitExpression_ );
    try
    {
      expr.Evaluate();
    }
    catch ( EExpressionError &e )
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      nv->GetData().SetData( iVec );
      nv->GetData().SetDimMag( 0, iVec.size() );
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
      return;
    }
    std::vector<double> newY( expr.GetFinalAnswer().GetData() );
    if( newY.size() == 1 )newY.insert( newY.begin()+1, newX.size()-1, newY[0] );
    //
    --icolor;
    if( icolor == -6 )--icolor; // to not use yellow (which is hard to see)
    GRA_color *color = GRA_colorControl::GetColor( icolor );
    static_cast<GRA_colorCharacteristic*>(dataCurveC->Get(wxT("CURVECOLOR")))->Set( color );
    static_cast<GRA_intCharacteristic*>(dataCurveC->Get(wxT("CURVELINETYPE")))->Set( i+2 );
    cartesianCurve = new GRA_cartesianCurve( newX, newY, xe1, ye1, xe2, ye2 );
    try
    {
      cartesianCurve->Make();
    }
    catch (EGraphicsError &e)
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      nv->GetData().SetData( iVec );
      nv->GetData().SetDimMag( 0, iVec.size() );
      delete cartesianCurve;
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
      return;
    }
    cartesianCurve->Draw( ExGlobals::GetGraphicsOutput(), dc );
    currentWindow->AddDrawableObject( cartesianCurve );
    //
    static_cast<GRA_colorCharacteristic*>(textC->Get(wxT("COLOR")))->Set( color );
    yloc -= 3.0;
    static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( yloc );
    wxString nameList;
    for( int j=0; j<nParams-1; ++j )
      nameList += pnameTC_[j]->GetValue() + wxT(" = ") + pstartTC_[j]->GetValue() + wxT(", ");
    nameList += pnameTC_[nParams-1]->GetValue() + wxT(" = ") + pstartTC_[nParams-1]->GetValue();
    GRA_drawableText *dt = new GRA_drawableText( nameList );
    try
    {
      dt->Parse();
    }
    catch ( EGraphicsError &e )
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      nv->GetData().SetData( iVec );
      nv->GetData().SetDimMag( 0, iVec.size() );
      delete dt;
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
      return;
    }
    dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
    currentWindow->AddDrawableObject( dt );
  }
  nv->GetData().SetData( iVec );
  nv->GetData().SetDimMag( 0, iVec.size() );
  //
  ExGlobals::ReplotCurrentWindow();
  //
  *generalC = generalCsave;
  *dataCurveC = dataCurveCsave;
  *xAxisC = xAxisCsave;
  *yAxisC = yAxisCsave;
  *textC = textCsave;
  //
  if( ExGlobals::StackIsOn() )
  {
    wxString line( wxT("FORM FIT,") );
    wxString degreeS( degreeCB_->GetStringSelection() );
    wxString type( typeCB_->GetStringSelection() );
    long nl;
    degreeS.ToLong( &nl );
    int degree = static_cast<int>( nl );
    int nparams = type==wxT("Custom") ? degree : degree+1;
    wxString fixed( wxT("[") );
    wxString start( wxT("[") );
    for( int i=0; i<nparams-1; ++i )
    {
      fixed += pCkB_[i]->IsChecked() ? wxT("1;") : wxT("0;");
      start += pstartTC_[i]->GetValue() + wxT(";");
    }
    fixed += pCkB_[nparams-1]->IsChecked() ? wxT("1") : wxT("0");
    start += pstartTC_[nparams-1]->GetValue();
    fixed += wxT("]");
    start += wxT("]");
    line += dVecName_ + wxT(",") + iVecName_ + wxT(",") + minTC_->GetValue() +
        wxT(",") + maxTC_->GetValue() + wxT(",") + eVecName_ + wxT(",'") + type +
        wxT("',") + degreeS + wxT(",'") + exprTC_->GetValue() + wxT("',") + fixed +
        wxT(",") + start + wxT(",") + legxTC_->GetValue() + wxT(",") +
        legyTC_->GetValue() + wxT(",'TEST THE FIT'");
    ExGlobals::WriteStack( line );
  }
}

void FitForm::OnClear( wxCommandEvent &WXUNUSED(event) )
{
  ClearGraphics();
}

void FitForm::ClearGraphics()
{
  ExGlobals::ClearGraphicsMonitor();
  ExGlobals::ClearWindows();
  pStartValues_.clear();
}
  
void FitForm::OnApply( wxCommandEvent &WXUNUSED(event) )
{
  DoTheFit();
}

void FitForm::DoTheFit()
{
  UpdateExpression();
  CreateParameters();
  pStartValues_.clear();
  //
  wxString s( dataCB_->GetStringSelection() );
  if( s == wxT("<none>") )
  {
    wxMessageBox( wxT("a data vector must be chosen"), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  dVecName_ = s;
  //
  s = indepCB_->GetStringSelection();
  if( s == wxT("<none>") )
  {
    wxMessageBox( wxT("an independent vector must be chosen"), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  iVecName_ = s;
  //
  eVecName_.clear();
  s = errorCB_->GetStringSelection();
  if( s != wxT("<none>") )eVecName_ = s;
  //
  wxString command( wxT("FIT") );
  int ntmax = 51;
  double tolerance = 0.00001;
  //
  std::vector<double> xVecSave, xVec, yVec, errorVec;
  try
  {
    NumericVariable::GetVector( dVecName_, wxT("data variable"), yVec );
    NumericVariable::GetVector( iVecName_, wxT("independent variable"), xVecSave );
    if( !eVecName_.empty() )
      NumericVariable::GetVector( eVecName_, wxT("data error variable"), errorVec );
  }
  catch( EVariableError &e )
  {
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  xVec.assign( xVecSave.begin(), xVecSave.end() );
  //
  std::size_t num = std::min( xVec.size(), yVec.size() );
  if( num < xVec.size() )xVec.resize( num );
  if( num < yVec.size() )yVec.resize( num );
  if( eVecName_.empty() )errorVec.assign( num, 1.0 );
  else
  {
    num = std::min( num, errorVec.size() );
    if( num < xVec.size() )xVec.resize( num );
    if( num < yVec.size() )yVec.resize( num );
    if( num < errorVec.size() )errorVec.resize( num );
  }
  if( !minTC_->GetValue().ToDouble(&minRange_) )
  {
    wxMessageBox( wxT("invalid value entered for min"), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  if( !maxTC_->GetValue().ToDouble(&maxRange_) )
  {
    wxMessageBox( wxT("invalid value entered for max"), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  std::vector<double> newX, newY, newE;
  for( std::size_t i=0; i<num; ++i )
  {
    if( xVec[i]>=minRange_ && xVec[i]<=maxRange_ )
    {
      newX.push_back( xVec[i] );
      newY.push_back( yVec[i] );
      newE.push_back( errorVec[i] );
    }
  }
  if( newX.empty() )
  {
    wxMessageBox( wxT("there are no data values within the independent vector range"),
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  num = newX.size();
  NumericVariable *nvX = NVariableTable::GetTable()->GetVariable( iVecName_ );
  nvX->GetData().SetData( newX );
  nvX->GetData().SetDimMag( 0, newX.size() );
  //
  // decode the expression
  //
  Expression expr( fitExpression_ );
  expr.SetIsaFit();
  try
  {
    expr.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    nvX->GetData().SetData( xVecSave );
    nvX->GetData().SetDimMag( 0, xVecSave.size() );
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  std::size_t const nparam = expr.GetNumberOfFitParameters();
  if( nparam == 0 )
  {
    nvX->GetData().SetData( xVecSave );
    nvX->GetData().SetDimMag( 0, xVecSave.size() );
    wxMessageBox( wxT("there are no fitting parameters in the expression"),
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  if( num <= nparam )
  {
    nvX->GetData().SetData( xVecSave );
    nvX->GetData().SetDimMag( 0, xVecSave.size() );
    wxMessageBox( wxT("number of data points <= number of parameters"),
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  // do the fit
  //
  std::vector<double> pp( nparam, 0.0 );
  //
  // initial parameter values
  //
  for( std::size_t i=0; i<nparam; ++i )pp[i] = expr.GetFitParameterValue( i );
  //
  std::vector<double> p1( pp );    // save original parameter values
  std::vector<double> pSave( pp );
  std::vector<double> e1( nparam ), e2( nparam );
  std::vector< std::vector<double> > errmat( nparam );
  for( std::size_t i=0; i<nparam; ++i )errmat[i].resize( nparam );
  nFree_ = 0;
  bool zeros = false;
  bool output = true;
  bool poisson = false;
  for( std::size_t i=0; i<num; ++i )newE[i] = newE[i]==0.0 ? 1.e6 : 1.0/(newE[i]*newE[i]);
  bool marquardt = false;
  try
  {
    UsefulFunctions::LeastSquaresFit( &expr, dVecName_, newY, newE, e1, e2, pp, pSave,
                                      ntmax, tolerance, chisq_, confidenceLevel_, errmat,
                                      output, zeros, poisson, marquardt, nFree_ );
  }
  catch ( EExpressionError &e )
  {
    nvX->GetData().SetData( xVecSave );
    nvX->GetData().SetDimMag( 0, xVecSave.size() );
    for( std::size_t i=0; i<nparam; ++i )expr.SetFitParameterValue( i, p1[i] );
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  catch (...)
  {
    nvX->GetData().SetData( xVecSave );
    nvX->GetData().SetDimMag( 0, xVecSave.size() );
    for( std::size_t i=0; i<nparam; ++i )expr.SetFitParameterValue( i, p1[i] );
    wxMessageBox( wxT("unknown error"), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  nvX->GetData().SetData( xVecSave );
  nvX->GetData().SetDimMag( 0, xVecSave.size() );
  //
  // the fit was successful
  // re-evaluate the last expression and make new variable using the
  // calculated parameter values
  //
  Expression expr2( fitExpression_ );
  try
  {
    expr2.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  pvStr_ = wxT("FIT: ") + dVecName_ + wxT(" = ") + fitExpression_;
  //
  // the fit is re-evaluated at every data point,
  // not just the points within the independent vector range
  //
  NumericData nd( expr2.GetFinalAnswer() );
  wxString yfit( dVecName_+wxT("$UPDATE") );
  try
  {
    switch ( nd.GetNumberOfDimensions() )
    {
      case 0:
        NumericVariable::PutVariable( yfit, nd.GetScalarValue(), pvStr_ );
        break;
      case 1:
        NumericVariable::PutVariable( yfit, nd.GetData(), 0, pvStr_ );
        break;
      case 2:
        NumericVariable::PutVariable( yfit, nd.GetData(), nd.GetDimMag(0), nd.GetDimMag(1), pvStr_ );
    }
  }
  catch ( EVariableError &e )
  {
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  //
  // make residual vector with xNew.size() values
  //
  wxString name;
  std::vector<double> residual;
  std::size_t xvSize = xVec.size();
  for( std::size_t i=0; i<xvSize; ++i )
  {
    if( xVec[i]>=minRange_ && xVec[i]<=maxRange_ )residual.push_back( nd.GetData()[i]-yVec[i] );
  }
  try
  {
    name = wxT("FIT$RESIDUAL");
    NumericVariable::PutVariable( name, residual, 0, pvStr_ );
    name = wxT("FIT$XMIN");
    NumericVariable::PutVariable( name, minRange_, pvStr_ );
    name = wxT("FIT$XMAX");
    NumericVariable::PutVariable( name, maxRange_, pvStr_ );
    name = wxT("FIT$XRANGE");
    NumericVariable::PutVariable( name, newX, 0, pvStr_ );
  }
  catch ( EVariableError &e )
  {
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  //
  // update the parameters and definition table after a successful fit
  //
  pStrings_.assign( expr.GetFitParameterNames().begin(), expr.GetFitParameterNames().end() );
  name = wxT("FIT$VAR");
  TextVariable::PutVariable( name, pStrings_, pvStr_ );
  for( std::size_t i=0; i<nparam; ++i )
  {
    expr.SetFitParameterValue( i, pp[i], pvStr_ );
    for( std::size_t j=0; j<8; ++j )
    {
      if( pStrings_[i] == pnameTC_[j]->GetValue().Upper() )
      {
        presultTC_[j]->Clear();
        *presultTC_[j] << pp[i];
        break;
      }
    }
    pStrings_[i] += wxT(" = ") + ExGlobals::GetNiceNumber(pp[i]) +
        wxT("  <pm> ") + ExGlobals::GetNiceNumber(e1[i]) + wxT("  ( ") +
        ExGlobals::GetNiceNumber(fabs(e1[i]/pp[i]*100)) + wxT("% )");
  }
  std::vector<double> corr( nparam*nparam, 0.0 );
  for( std::size_t i=0; i<nparam; ++i )
  {
    for( std::size_t j=0; j<=i; ++j )
    {
      corr[j+i*nparam] = 1.0;
      if( errmat[j][i] == 0.0 )corr[j+i*nparam] = 0.0;
      if( errmat[j][j]*errmat[i][i] > 0.0 )
      {
        corr[j+i*nparam] = errmat[j][i]/sqrt(errmat[j][j]*errmat[i][i]);
        corr[i+j*nparam] = corr[j+i*nparam];
      }
    }
    try
    {
      name = wxT("FIT$CORR");
      NumericVariable::PutVariable( name, corr, nparam, nparam, pvStr_ );
    }
    catch ( EVariableError &e )
    {
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
      return;
    }
  }
  std::vector<double> cov( nparam*nparam, 0.0 );
  for( std::size_t i=0; i<nparam; ++i )
  {
    for( std::size_t j=0; j<nparam; ++j )cov[j+i*nparam] = errmat[j][i];
  }
  chiTC_->SetValue( ExGlobals::GetNiceNumber(chisq_) );
  confTC_->SetValue( ExGlobals::GetNiceNumber(confidenceLevel_) + wxT('\%') );
  degFreeTC_->SetValue( wxString()<<nFree_ );
  try
  {
    name = wxT("FIT$COVM");
    NumericVariable::PutVariable( name, cov, nparam, nparam, pvStr_ );
    name = wxT("FIT$E1");
    NumericVariable::PutVariable( name, e1, 0, pvStr_ );
    name = wxT("FIT$E2");
    NumericVariable::PutVariable( name, e2, 0, pvStr_ );
    name = wxT("FIT$CHISQ");
    NumericVariable::PutVariable( name, chisq_, pvStr_ );
    name = wxT("FIT$CL");
    NumericVariable::PutVariable( name, confidenceLevel_, pvStr_ );
    name = wxT("FIT$FREE");
    NumericVariable::PutVariable( name, static_cast<double>(nFree_), pvStr_ );
  }
  catch ( EVariableError &e )
  {
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  PlotFit();
  ExGlobals::ReplotCurrentWindow();
}

void FitForm::PlotFit()
{
  GRA_window *currentWindow = ExGlobals::GetGraphWindow();
  //
  GRA_setOfCharacteristics *generalC = currentWindow->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *dataCurveC = currentWindow->GetDataCurveCharacteristics();
  GRA_setOfCharacteristics *xAxisC = currentWindow->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = currentWindow->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *textC = currentWindow->GetTextCharacteristics();
  //
  GRA_setOfCharacteristics generalCsave( *generalC );
  GRA_setOfCharacteristics dataCurveCsave( *dataCurveC );
  GRA_setOfCharacteristics xAxisCsave( *xAxisC );
  GRA_setOfCharacteristics yAxisCsave( *yAxisC );
  GRA_setOfCharacteristics textCsave( *textC );
  //
  currentWindow->Clear();
  currentWindow->Erase();
  std::vector<double> y, x, xe1, ye1, xe2, ye2;
  try
  {
    NumericVariable::GetVector( dVecName_, wxT("data variable"), y );
    NumericVariable::GetVector( iVecName_, wxT("independent variable"), x );
    if( !eVecName_.empty() )NumericVariable::GetVector( eVecName_, wxT("error variable"), ye1 );
  }
  catch( EVariableError &e )
  {
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  std::size_t num = std::min( x.size(), y.size() );
  if( num < x.size() )x.resize( num );
  if( num < y.size() )y.resize( num );
  if( !eVecName_.empty() )
  {
    num = std::min( num, ye1.size() );
    if( num < x.size() )x.resize( num );
    if( num < y.size() )y.resize( num );
    if( num < ye1.size() )ye1.resize( num );
  }
  double imin, imax;
  UsefulFunctions::MinMax( x, 0, num, imin, imax );
  //
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOL")))->Set( -8 );
  if( num >= 200 )
    static_cast<GRA_sizeCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOLSIZE")))->SetAsPercent( 0.5 );
  static_cast<GRA_colorCharacteristic*>(dataCurveC->Get(wxT("CURVECOLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_stringCharacteristic*>(xAxisC->Get(wxT("LABEL")))->Set( iVecName_ );
  static_cast<GRA_boolCharacteristic*>(xAxisC->Get(wxT("LABELON")))->Set( true );
  static_cast<GRA_stringCharacteristic*>(yAxisC->Get(wxT("LABEL")))->Set( dVecName_ );
  static_cast<GRA_boolCharacteristic*>(yAxisC->Get(wxT("LABELON")))->Set( true );
  //
  static_cast<GRA_boolCharacteristic*>(generalC->Get(wxT("GRAPHBOX")))->Set( true );
  //
  if( imin<minRange_ || imax>maxRange_ )
  {
    std::vector<GRA_color*> psym;
    for( std::size_t i=0; i<num; ++i )
      psym.push_back( ((minRange_<=x[i])&&(x[i]<=maxRange_)) ?
        GRA_colorControl::GetColor(wxT("PURPLE")) : GRA_colorControl::GetColor(wxT("BLACK")) );
    static_cast<GRA_colorCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOLCOLOR")))->Set( psym );
  }
  else
    static_cast<GRA_colorCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOLCOLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  //
  std::vector<double> x2(2), y2(2);
  x2[0] = imin;
  x2[1] = imax;
  if( !ye1.empty() )
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( y2[1] < y[i]+ye1[i] )y2[1] = y[i]+ye1[i];
      if( y2[0] > y[i]-ye1[i] )y2[0] = y[i]-ye1[i];
    }
  }
  else
  {
    for( std::size_t i=0; i<num; ++i )
    {
      if( y2[1] < y[i] )y2[1] = y[i];
      if( y2[0] > y[i] )y2[0] = y[i];
    }
  }
  GRA_cartesianAxes *cartesianAxes = 0;
  GRA_cartesianCurve *cartesianCurve = 0;
  try
  {
    cartesianAxes = new GRA_cartesianAxes(x2,y2,false,false);
    cartesianCurve = new GRA_cartesianCurve(x,y,xe1,ye1,xe2,ye2);
    cartesianAxes->Make();
    cartesianCurve->Make();
  }
  catch (EGraphicsError &e)
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete cartesianAxes;
    delete cartesianCurve;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  wxClientDC dc( ExGlobals::GetwxWindow() );
  cartesianAxes->Draw( ExGlobals::GetGraphicsOutput(), dc );
  cartesianCurve->Draw( ExGlobals::GetGraphicsOutput(), dc );
  currentWindow->AddDrawableObject( cartesianAxes );
  currentWindow->AddDrawableObject( cartesianCurve );
  //
  if( imin<minRange_ || imax>maxRange_ )
  {
    std::vector<double> ytmp, xtmp, xe1t, ye1t, xe2t, ye2t;
    double ymin = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("MIN")))->Get();
    double ymax = static_cast<GRA_doubleCharacteristic*>(yAxisC->Get(wxT("MAX")))->Get();
    xtmp.push_back( minRange_ );
    xtmp.push_back( minRange_ );
    ytmp.push_back( ymin-(ymax-ymin) );
    ytmp.push_back( ymax+(ymax-ymin) );
    GRA_color *color = static_cast<GRA_colorCharacteristic*>(dataCurveC->Get(wxT("CURVECOLOR")))->Get();
    int linetype = static_cast<GRA_intCharacteristic*>(dataCurveC->Get(wxT("CURVELINETYPE")))->Get();
    static_cast<GRA_colorCharacteristic*>(dataCurveC->Get(wxT("CURVECOLOR")))->Set(
      GRA_colorControl::GetColor(wxT("PURPLE")) );
    static_cast<GRA_intCharacteristic*>(dataCurveC->Get(wxT("CURVELINETYPE")))->Set( 5 );
    static_cast<GRA_intCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOL")))->Set( 0 );
    cartesianCurve = new GRA_cartesianCurve(xtmp,ytmp,xe1t,ye1t,xe2t,ye2t);
    try
    {
      cartesianCurve->Make();
    }
    catch (EGraphicsError &e)
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      delete cartesianCurve;
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
      return;
    }
    cartesianCurve->Draw( ExGlobals::GetGraphicsOutput(), dc );
    currentWindow->AddDrawableObject( cartesianCurve );
    xtmp[0] = maxRange_;
    xtmp[1] = maxRange_;
    cartesianCurve = new GRA_cartesianCurve(xtmp,ytmp,xe1t,ye1t,xe2t,ye2t);
    try
    {
      cartesianCurve->Make();
    }
    catch (EGraphicsError &e)
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      delete cartesianCurve;
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
      return;
    }
    cartesianCurve->Draw( ExGlobals::GetGraphicsOutput(), dc );
    currentWindow->AddDrawableObject( cartesianCurve );
    static_cast<GRA_colorCharacteristic*>(dataCurveC->Get(wxT("CURVECOLOR")))->Set( color );
    static_cast<GRA_intCharacteristic*>(dataCurveC->Get(wxT("CURVELINETYPE")))->Set( linetype );
  }
  ye1.erase( ye1.begin(), ye1.end() );
  //
  // re-evaluate the last expression
  //
  wxString fitDataVec( dVecName_+wxT("$FIT") );
  wxString fitIndepVec( iVecName_+wxT("$FIT") );
  NumericVariable *nv = NVariableTable::GetTable()->GetVariable( iVecName_ );
  std::vector<double> iVec( nv->GetData().GetData() );
  nv->GetData().GetMinMax( imin, imax );
  int newNum = 500;
  double inc = (imax-imin)/(newNum-1);
  std::vector<double> newX( newNum );
  for( int i=0; i<newNum-1; ++i )newX[i] = imin+i*inc;
  newX[newNum-1] = imax;
  nv->GetData().SetData( newX );
  nv->GetData().SetDimMag( 0, newX.size() );
  Expression expr( fitExpression_ );
  try
  {
    expr.Evaluate();
  }
  catch ( EExpressionError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    nv->GetData().SetData( iVec );
    nv->GetData().SetDimMag( 0, iVec.size() );
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  std::vector<double> newY( expr.GetFinalAnswer().GetData() );
  if( newY.size() == 1 )newY.resize( newX.size(), newY[0] );
  nv->GetData().SetData( iVec );
  nv->GetData().SetDimMag( 0, iVec.size() );
  try
  {
    NumericVariable::PutVariable( fitIndepVec, newX, 0, pvStr_ );
    NumericVariable::PutVariable( fitDataVec, newY, 0, pvStr_ );
  }
  catch ( EVariableError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  static_cast<GRA_intCharacteristic*>(dataCurveC->Get(wxT("PLOTSYMBOL")))->Set( 0 );
  static_cast<GRA_colorCharacteristic*>(dataCurveC->Get(wxT("CURVECOLOR")))->Set( GRA_colorControl::GetColor(-2) );
  cartesianCurve = new GRA_cartesianCurve(newX,newY,xe1,ye1,xe2,ye2);
  try
  {
    cartesianCurve->Make();
  }
  catch (EGraphicsError &e)
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete cartesianCurve;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  cartesianCurve->Draw( ExGlobals::GetGraphicsOutput(), dc );
  currentWindow->AddDrawableObject( cartesianCurve );
  //
  double const ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("LOWERAXIS")))->GetAsPercent();
  double const xlaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("LOWERAXIS")))->GetAsPercent();
  double const yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->GetAsPercent();
  double const xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->GetAsPercent();
  //
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 1 );
  static_cast<GRA_angleCharacteristic*>(textC->Get(wxT("ANGLE")))->Set( 0.0 );
  static_cast<GRA_colorCharacteristic*>(textC->Get(wxT("COLOR")))->Set( GRA_colorControl::GetColor(wxT("BLACK")) );
  static_cast<GRA_fontCharacteristic*>(textC->Get(wxT("FONT")))->Set( GRA_fontControl::GetFont(wxT("SANS")) );
  static_cast<GRA_sizeCharacteristic*>(textC->Get(wxT("HEIGHT")))->SetAsPercent( 3.0 );
  static_cast<GRA_boolCharacteristic*>(textC->Get(wxT("INTERACTIVE")))->Set( false );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsPercent( xlaxis );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( yuaxis+1 );
  //
  wxString title( wxT("FIT: ") );
  title += dVecName_ + wxT(" = ") + fitTitle_;
  GRA_drawableText *dt = new GRA_drawableText( title );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
  currentWindow->AddDrawableObject( dt );
  //
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 3 ); // lower right
  static_cast<GRA_sizeCharacteristic*>(textC->Get(wxT("HEIGHT")))->SetAsPercent( 1.5 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsPercent( 98.0 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( 97.5 );
  wxDateTime now = wxDateTime::Now();
  wxString time = now.FormatTime();
  wxString date = now.FormatDate();
  dt = new GRA_drawableText( time+wxT(" ")+date );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
  currentWindow->AddDrawableObject( dt );
  //
  nv = NVariableTable::GetTable()->GetVariable( dVecName_ );
  if( !nv->GetOrigin().empty() )
  {
    static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( 94.5 );
    dt = new GRA_drawableText( nv->GetOrigin() );
    try
    {
      dt->Parse();
    }
    catch ( EGraphicsError &e )
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      delete dt;
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
      return;
    }
    dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
    currentWindow->AddDrawableObject( dt );
  }
  double xloc, yloc;
  if( !legxTC_->GetValue().ToDouble(&xloc) )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    wxMessageBox( wxT("invalid value entered for legend x location"), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  if( !legyTC_->GetValue().ToDouble(&yloc) )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    wxMessageBox( wxT("invalid value entered for legend y location"), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  std::size_t nparams = pStrings_.size();
  static_cast<GRA_intCharacteristic*>(textC->Get(wxT("ALIGNMENT")))->Set( 1 ); // lower left
  static_cast<GRA_sizeCharacteristic*>(textC->Get(wxT("HEIGHT")))->SetAsPercent( 2.0 );
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("XLOCATION")))->SetAsPercent( xloc );
  for( std::size_t i=0; i<nparams; ++i )
  {
    yloc -= 4.0;
    static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( yloc );
    dt = new GRA_drawableText( pStrings_[i] );
    try
    {
      dt->Parse();
    }
    catch ( EGraphicsError &e )
    {
      *generalC = generalCsave;
      *dataCurveC = dataCurveCsave;
      *xAxisC = xAxisCsave;
      *yAxisC = yAxisCsave;
      *textC = textCsave;
      delete dt;
      wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
      return;
    }
    dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
    currentWindow->AddDrawableObject( dt );
  }
  yloc -= 6.0;
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( yloc );
  wxString str( wxT("Number of data points = ") );
  dt = new GRA_drawableText( str<<num );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
  currentWindow->AddDrawableObject( dt );
  //
  yloc -= 4.0;
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( yloc );
  wxString astr = wxT("Fit range: ") + ExGlobals::GetNiceNumber(minRange_) + wxT(" to ") +
                  ExGlobals::GetNiceNumber(maxRange_);
  dt = new GRA_drawableText( astr );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
  currentWindow->AddDrawableObject( dt );
  //
  yloc -= 4.0;
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( yloc );
  str = wxT("Degrees of freedom = ");
  dt = new GRA_drawableText( str<<nFree_ );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
  currentWindow->AddDrawableObject( dt );
  //
  yloc -= 4.0;
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( yloc );
  str = wxT("Total <chi><^>2<_> = ") + ExGlobals::GetNiceNumber(chisq_) + wxT(",   <chi><^>2<_>/df = ") +
        ExGlobals::GetNiceNumber(chisq_/nFree_);
  dt = new GRA_drawableText( str );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
  currentWindow->AddDrawableObject( dt );
  //
  yloc -= 4.0;
  static_cast<GRA_distanceCharacteristic*>(textC->Get(wxT("YLOCATION")))->SetAsPercent( yloc );
  str = wxT("Confidence level = ") + ExGlobals::GetNiceNumber(confidenceLevel_) + wxT("%");
  dt = new GRA_drawableText( str );
  try
  {
    dt->Parse();
  }
  catch ( EGraphicsError &e )
  {
    *generalC = generalCsave;
    *dataCurveC = dataCurveCsave;
    *xAxisC = xAxisCsave;
    *yAxisC = yAxisCsave;
    *textC = textCsave;
    delete dt;
    wxMessageBox( wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  dt->Draw( ExGlobals::GetGraphicsOutput(), dc );
  currentWindow->AddDrawableObject( dt );
  //
  *generalC = generalCsave;
  *dataCurveC = dataCurveCsave;
  *xAxisC = xAxisCsave;
  *yAxisC = yAxisCsave;
  *textC = textCsave;
  //
  if( ExGlobals::StackIsOn() )
  {
    wxString line( wxT("FORM FIT,") );
    wxString degreeS( degreeCB_->GetStringSelection() );
    wxString type( typeCB_->GetStringSelection() );
    long nl;
    degreeS.ToLong( &nl );
    int degree = static_cast<int>( nl );
    int nparams = type==wxT("Custom") ? degree : degree+1;
    wxString fixed( wxT("[") );
    wxString start( wxT("[") );
    for( int i=0; i<nparams-1; ++i )
    {
      fixed += pCkB_[i]->IsChecked() ? wxT("1;") : wxT("0;");
      start += pstartTC_[i]->GetValue() + wxT(";");
    }
    fixed += pCkB_[nparams-1]->IsChecked() ? wxT("1") : wxT("0");
    start += pstartTC_[nparams-1]->GetValue();
    fixed += wxT("]");
    start += wxT("]");
    line += dVecName_ + wxT(",") + iVecName_ + wxT(",") + minTC_->GetValue() +
        wxT(",") + maxTC_->GetValue() + wxT(",") + eVecName_ + wxT(",'") + type +
        wxT("',") + degreeS + wxT(",'") + exprTC_->GetValue() + wxT("',") +
        fixed + wxT(",") + start + wxT(",") + legxTC_->GetValue() + wxT(",") +
        legyTC_->GetValue() + wxT(",'DO THE FIT'");
    ExGlobals::WriteStack( line );
  }
}

void FitForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

void FitForm::Set( wxString const &dataName, wxString const &indepName,
                   double idmin, double idmax, wxString const &errorName,
                   wxString const &typeName, int nParams,
                   wxString const &expression,
                   std::vector<double> &fixed, std::vector<double> &start,
                   double xloc, double yloc, wxString const &action )
{
  dVecName_ = dataName.Upper();
  dataCB_->SetStringSelection( dVecName_ );
  exprST_->SetLabel( wxString(wxT("Fit expression:  "))+
                     dVecName_.Mid(std::min(dVecName_.Length(),size_t(10)))+wxT(" = ") );
  //
  iVecName_ = indepName.Upper();
  indepCB_->Enable( true );
  indepST_->Enable( true );
  indepCB_->SetStringSelection( iVecName_ );
  //
  minST_->Enable( true );
  maxST_->Enable( true );
  minTC_->Enable( true );
  maxTC_->Enable( true );
  minTC_->SetValue( wxString() << idmin );
  maxTC_->SetValue( wxString() << idmax );
  //
  eVecName_ = errorName.Upper();
  errorST_->Enable( true );
  errorCB_->Enable( true );
  errorCB_->SetStringSelection( eVecName_ );
  //
  typeCB_->Enable( true );
  typeST_->Enable( true );
  typeCB_->SetStringSelection( typeName.Upper() );
  FitTypeChange();
  if( typeCB_->GetSelection()==0 || typeCB_->GetSelection()==1 )
  {
    degreeCB_->SetSelection( nParams-2 );
    DegreeChange();
  }
  else if( typeCB_->GetSelection() == 5 )
  {
    degreeCB_->SetSelection( nParams-1 );
    DegreeChange();
  }
  exprTC_->SetValue( expression );
  UpdateExpression();
  for( int i=0; i<nParams; ++i )
    SetupParameterFields( i, true, (fixed[i]!=0.0), wxString()<<start[i] );
  legxTC_->SetValue( wxString() << xloc );
  legyTC_->SetValue( wxString() << yloc );
  if( action == wxT("CLEAR GRAPHICS") )
  {
    ClearGraphics();
  }
  else if( action == wxT("TEST THE FIT") )
  {
    TestTheFit();
  }
  else if( action == wxT("DO THE FIT") )
  {
    DoTheFit();
  }
  else if( action == wxT("CLOSE") )
  {
    Close();
  }
}

// end of file
