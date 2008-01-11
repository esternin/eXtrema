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

#include "GenerateVectorForm.h"
#include "AnalysisWindow.h"
#include "NumericVariable.h"
#include "EVariableError.h"
#include "ExGlobals.h"
#include "UsefulFunctions.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( GenerateVectorForm, wxFrame )
  EVT_RADIOBOX( ID_choose, GenerateVectorForm::OnChoose )
  EVT_BUTTON( ID_defaults, GenerateVectorForm::OnDefaults )
  EVT_BUTTON( wxID_APPLY, GenerateVectorForm::OnApply )
  EVT_BUTTON( wxID_CLOSE, GenerateVectorForm::OnClose )
  EVT_CLOSE( GenerateVectorForm::CloseEventHandler )
END_EVENT_TABLE()

GenerateVectorForm::GenerateVectorForm( AnalysisWindow *parent )
    : wxFrame(parent,wxID_ANY,wxT("Generate a vector"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE),
      analysisWindow_(parent)
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );
  mainSizer->InsertSpacer( 0, 10 );

  wxPanel *topPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL );
  topSizer->Add(
    new wxStaticText(topPanel,wxID_ANY,wxT("Generated vector name"),wxDefaultPosition,wxDefaultSize,wxALIGN_RIGHT ),
    wxSizerFlags(0).Right().Border(wxTOP,10) );
  nameTC_ = new wxTextCtrl( topPanel, wxID_ANY );
  nameTC_->SetValue( wxT("") );
  topSizer->Add( nameTC_, wxSizerFlags(0).Left().Border(wxALL,5) );
  topPanel->SetSizer( topSizer );
  mainSizer->Add( topPanel, wxSizerFlags(0).Center().Border(wxALL,1) );

  wxPanel *midPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *midSizer = new wxBoxSizer( wxHORIZONTAL );

  wxPanel *leftPanel = new wxPanel( midPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER );
  wxBoxSizer *leftSizer = new wxBoxSizer( wxVERTICAL );

  wxString choicesArray[] = 
    { wxT("random values"), wxT("minimum increment maximum"), wxT("minimum increment number"), wxT("minimum maximum number") };
  wxArrayString choices( 4, choicesArray );
  choicesRB_ = new wxRadioBox( leftPanel, ID_choose, wxT("Choices"), wxDefaultPosition, wxDefaultSize,
                                     choices, 4, wxRA_SPECIFY_ROWS );
  choicesRB_->SetSelection( 1 );
  leftSizer->Add( choicesRB_, wxSizerFlags(0).Border(wxALL,2) );
  leftPanel->SetSizer( leftSizer );
  midSizer->Add( leftPanel, wxSizerFlags(0).Border(wxALL,10) );

  wxPanel *rightPanel = new wxPanel( midPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER );
  wxGridSizer *rightSizer = new wxGridSizer( 4, 2, 5, 5 );

  minLabel_ = new wxStaticText(rightPanel,wxID_ANY,wxT("Minimum value"),wxDefaultPosition,wxDefaultSize,wxALIGN_RIGHT );
  rightSizer->Add( minLabel_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  minValueTC_ = new wxTextCtrl( rightPanel, wxID_ANY );
  minValueTC_->SetValue( wxT("") );
  rightSizer->Add( minValueTC_, wxSizerFlags(0).Left().Border(wxALL,2) );

  incLabel_ = new wxStaticText(rightPanel,wxID_ANY,wxT("Increment"),wxDefaultPosition,wxDefaultSize,wxALIGN_RIGHT );
  rightSizer->Add( incLabel_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  incValueTC_ = new wxTextCtrl( rightPanel, wxID_ANY );
  incValueTC_->SetValue( wxT("") );
  rightSizer->Add( incValueTC_, wxSizerFlags(0).Left().Border(wxALL,2) );

  maxLabel_ = new wxStaticText(rightPanel,wxID_ANY,wxT("Maximum value"),wxDefaultPosition,wxDefaultSize,wxALIGN_RIGHT );
  rightSizer->Add( maxLabel_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  maxValueTC_ = new wxTextCtrl( rightPanel, wxID_ANY );
  maxValueTC_->SetValue( wxT("") );
  rightSizer->Add( maxValueTC_, wxSizerFlags(0).Left().Border(wxALL,2) );
  
  numLabel_ =  new wxStaticText(rightPanel,wxID_ANY,wxT("  Number of values"),wxDefaultPosition,wxDefaultSize,wxALIGN_RIGHT );
  rightSizer->Add( numLabel_, wxSizerFlags(0).Right().Border(wxTOP,5) );
  numValueTC_ = new wxTextCtrl( rightPanel, wxID_ANY );
  numValueTC_->SetValue( wxT("") );
  rightSizer->Add( numValueTC_, wxSizerFlags(0).Left().Border(wxALL,2) );

  rightPanel->SetSizer( rightSizer );
  midSizer->Add( rightPanel, wxSizerFlags(0).Border(wxALL,10) );
  midPanel->SetSizer( midSizer );

  mainSizer->Add( midPanel, wxSizerFlags(0).Center().Border(wxALL,1) );
  
  wxPanel *bottomPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );

  wxButton *defaultsButton = new wxButton( bottomPanel, ID_defaults, wxT("Reset") );
  defaultsButton->SetToolTip( wxT("clear all fields") );
  bottomSizer->Add( defaultsButton, wxSizerFlags(0).Border(wxALL,10) );

  wxButton *applyButton = new wxButton( bottomPanel, wxID_APPLY, wxT("Apply") );
  applyButton->SetToolTip( wxT("create the vector") );
  bottomSizer->Add( applyButton, wxSizerFlags(0).Border(wxALL,10) );

  wxButton *closeButton = new wxButton( bottomPanel, wxID_CLOSE, wxT("Close") );
  closeButton->SetToolTip( wxT("close this form") );
  bottomSizer->Add( closeButton, wxSizerFlags(0).Border(wxALL,10) );

  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(0).Centre().Border(wxALL,1) );

  SetSizer( mainSizer );
  
  wxConfigBase *config = wxConfigBase::Get();
  int ulx = config->Read( wxT("/GenerateVectorForm/UPPERLEFTX"), 0l );
  int uly = config->Read( wxT("/GenerateVectorForm/UPPERLEFTY"), 640l );
  int width = config->Read( wxT("/GenerateVectorForm/WIDTH"), 535l );
  int height = config->Read( wxT("/GenerateVectorForm/HEIGHT"), 255l );
  SetSize( ulx, uly, width, height );
  wxString s;
  config->Read( wxT("/GenerateVectorForm/NAME"), &s, wxT("") );
  nameTC_->SetValue( s );
  s.clear();
  config->Read( wxT("/GenerateVectorForm/MIN"), &s, wxT("") );
  minValueTC_->SetValue( s );
  s.clear();
  config->Read( wxT("/GenerateVectorForm/INC"), &s, wxT("") );
  incValueTC_->SetValue( s );
  s.clear();
  config->Read( wxT("/GenerateVectorForm/MAX"), &s, wxT("") );
  maxValueTC_->SetValue( s );
  s.clear();
  config->Read( wxT("/GenerateVectorForm/NUM"), &s ,wxT("") );
  numValueTC_->SetValue( s );
  choicesRB_->SetSelection( config->Read(wxT("/GenerateVectorForm/CHOICE"),1l) );

  SetUp();
  Show( true );
}

void GenerateVectorForm::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/GenerateVectorForm/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/GenerateVectorForm/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/GenerateVectorForm/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/GenerateVectorForm/HEIGHT"), static_cast<long>(height) );
    //
    config->Write( wxT("/GenerateVectorForm/NAME"), nameTC_->GetValue() );
    config->Write( wxT("/GenerateVectorForm/MIN"), minValueTC_->GetValue() );
    config->Write( wxT("/GenerateVectorForm/INC"), incValueTC_->GetValue() );
    config->Write( wxT("/GenerateVectorForm/MAX"), maxValueTC_->GetValue() );
    config->Write( wxT("/GenerateVectorForm/NUM"), numValueTC_->GetValue() );
    config->Write( wxT("/GenerateVectorForm/CHOICE"), static_cast<long>(choicesRB_->GetSelection()) );
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
  analysisWindow_->ZeroGenerateVector();

  Destroy();
}

void GenerateVectorForm::OnDefaults( wxCommandEvent &WXUNUSED(event) )
{ Defaults(); }

void GenerateVectorForm::Defaults()
{
  nameTC_->Clear();
  minValueTC_->Clear();
  incValueTC_->Clear();
  maxValueTC_->Clear();
  numValueTC_->Clear();
  choicesRB_->SetSelection( 1 );
  SetUp();
  //
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxT("FORM GENERATE,DEFAULTS") );
}

void GenerateVectorForm::OnChoose( wxCommandEvent &WXUNUSED(event) )
{ SetUp(); }

void GenerateVectorForm::SetUp()
{
  minValueTC_->Enable( true );
  minLabel_->Enable( true );
  incValueTC_->Enable( true );
  incLabel_->Enable( true );
  maxValueTC_->Enable( true );
  maxLabel_->Enable( true );
  numValueTC_->Enable( true );
  numLabel_->Enable( true );
  switch ( choicesRB_->GetSelection() )
  {
    case 0:
    case 3:
      incValueTC_->Enable( false );
      incLabel_->Enable( false );
      break;
    case 1:
      numValueTC_->Enable( false );
      numLabel_->Enable( false );
      break;
    case 2:
      maxValueTC_->Enable( false );
      maxLabel_->Enable( false );
      break;
  }
}
  
void GenerateVectorForm::OnApply( wxCommandEvent &WXUNUSED(event) )
{ Apply(); }

void GenerateVectorForm::Apply()
{
  wxString name( nameTC_->GetValue() );
  if( name.empty() )
  {
    wxMessageDialog *md =
        new wxMessageDialog( this, wxT("no output vector has been specified"), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
    return;
  }
  double vMin, vMax, vInc;
  int vNum;
  std::vector<double> x;
  wxString minString( minValueTC_->GetValue() );
  wxString incString( incValueTC_->GetValue() );
  wxString maxString( maxValueTC_->GetValue() );
  wxString numString( numValueTC_->GetValue() );
  int choice = choicesRB_->GetSelection();
  int order = 0;
  if( choice == 0 )
  {
    if( minString.empty() || maxString.empty() || numString.empty() )
    {
      wxMessageDialog *md =
          new wxMessageDialog( this, wxT("min, max and number of values must be provided"), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
      md->ShowModal();
      return;
    }
    try
    {
      NumericVariable::GetScalar( minString, wxT("minimum value"), vMin );
      NumericVariable::GetScalar( maxString, wxT("maximum value"), vMax );
      double d;
      NumericVariable::GetScalar( numString, wxT("number of values"), d );
      vNum = static_cast<int>(d);
    }
    catch ( EVariableError const &e )
    {
      wxMessageDialog *md =
          new wxMessageDialog( this, wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
      md->ShowModal();
      return;
    }
    if( vNum <= 0 )
    {
      wxMessageDialog *md =
          new wxMessageDialog( this, wxT("number of values must be > 0"), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
      md->ShowModal();
      return;
    }
    double delv = vMax - vMin;
    x.reserve( vNum );
    for( int i=0; i<vNum; ++i )
    {
      double rdum = UsefulFunctions::GetSimpleRandomNumber();
      x.push_back( rdum*delv+vMin );
    }
  }
  else
  {
    switch ( choice )
    {
      case 1:
        if( minString.empty() || incString.empty() || maxString.empty() )
        {
          wxMessageDialog *md =
              new wxMessageDialog( this, wxT("min, increment and max must be provided"), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
          md->ShowModal();
          return;
        }
        break;
      case 2:
        if( minString.empty() || incString.empty() || numString.empty() )
        {
          wxMessageDialog *md =
              new wxMessageDialog( this, wxT("min, increment and number of elements must be provided"), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
          md->ShowModal();
          return;
        }
        break;
      case 3:
        if( minString.empty() || maxString.empty() || numString.empty() )
        {
          wxMessageDialog *md =
              new wxMessageDialog( this, wxT("min, max and number of elements must be provided"), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
          md->ShowModal();
          return;
        }
        break;
    }
    try
    {
      NumericVariable::GetScalar( minString, wxT("minimum value"), vMin );
    }
    catch ( EVariableError const &e )
    {
      wxMessageDialog *md =
          new wxMessageDialog( this, wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
      md->ShowModal();
      return;
    }
    if( choice==1 || choice==2 )
    {
      try
      {
        NumericVariable::GetScalar( incString, wxT("increment"), vInc );
      }
      catch ( EVariableError const &e )
      {
        wxMessageDialog *md =
            new wxMessageDialog( this, wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
        md->ShowModal();
        return;
      }
      if( vInc == 0.0 )
      {
        wxMessageDialog *md =
            new wxMessageDialog( this, wxT("increment must not be 0"), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
        md->ShowModal();
        return;
      }
    }
    if( choice==1 || choice==3 )
    {
      try
      {
        NumericVariable::GetScalar( maxString, wxT("max value"), vMax );
      }
      catch ( EVariableError const &e )
      {
        wxMessageDialog *md =
            new wxMessageDialog( this, wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
        md->ShowModal();
        return;
      }
    }
    if( choice==2 || choice==3 )
    {
      try
      {
        double d;
        NumericVariable::GetScalar( numString, wxT("number of values"), d );
        vNum = static_cast<int>(d);
      }
      catch ( EVariableError const &e )
      {
        wxMessageDialog *md =
            new wxMessageDialog( this, wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
        md->ShowModal();
        return;
      }
      if( vNum <= 0 )
      {
        wxMessageDialog *md =
            new wxMessageDialog( this, wxT("number of values must be > 0"), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
        md->ShowModal();
        return;
      }
    }
    if( choice == 1 )
    {
      vNum = static_cast<int>( (vMax-vMin)/vInc ) + 1;
      if( vNum <= 0 )
      {
        wxMessageDialog *md =
            new wxMessageDialog( this, wxT("number of values must be > 0"), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
        md->ShowModal();
        return;
      }
      x.reserve( vNum );
      for( int i=0; i<vNum; ++i )x.push_back( vMin+i*vInc );
      if( fabs(x.back()-vMax) > fabs(vMax*0.000001) )
      {
        wxString s( wxT("GENERATE: calculated maximum = ") );
        s << x.back() << wxT(", given maximum = ") << vMax;
        ExGlobals::WarningMessage( s );
      }
    }
    else if( choice == 2 )
    {
      x.reserve( vNum );
      for( int i=0; i<vNum; ++i )x.push_back( vMin+i*vInc );
    }
    else if( choice == 3 )
    {
      vInc = vNum>1 ? (vMax-vMin)/(vNum-1) : 0.0;
      x.reserve( vNum );
      for( int i=0; i<vNum-1; ++i )x.push_back( vMin+i*vInc );
      x.push_back( vMax );
    }
    order = vInc>0.0 ? 1 : 2;
  }
  try
  {
    NumericVariable::PutVariable( name, x, order, wxT("from GENERATE VECTOR gui") );
  }
  catch ( EVariableError const &e )
  {
    wxMessageDialog *md =
        new wxMessageDialog( this, wxString(e.what(),wxConvUTF8), wxT("Fatal error"), wxOK|wxICON_INFORMATION );
    md->ShowModal();
    return;
  }
  //
  if( ExGlobals::StackIsOn() )
  {
    wxString line( wxT("FORM GENERATE,") );
    line += nameTC_->GetValue() + wxT(",") + (wxString()<<choicesRB_->GetSelection()) +
        wxT(",") + minValueTC_->GetValue() + wxT(",") + incValueTC_->GetValue() +
        wxT(",") + maxValueTC_->GetValue() + wxT(",") + numValueTC_->GetValue() +
        wxT(",APPLY");
    ExGlobals::WriteStack( line );
  }
}

void GenerateVectorForm::OnClose( wxCommandEvent &WXUNUSED(event) )
{ 
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxT("FORM GENERATE,CLOSE") );
  Close();
}

void GenerateVectorForm::Set( wxString const &name, int choice, double min, double inc,
                              double max, int number, wxString const &action )
{
  nameTC_->SetValue( name );
  choicesRB_->SetSelection( choice );
  minValueTC_->SetValue( wxString()<<min );
  incValueTC_->SetValue( wxString()<<inc );
  maxValueTC_->SetValue( wxString()<<max );
  numValueTC_->SetValue( wxString()<<number );
  SetUp();
  if( action == wxT("CLOSE") )
  {
    if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( wxT("FORM GENERATE,CLOSE") );
    Close();
  }
  else if( action == wxT("DEFAULTS") )
  {
    Defaults();
  }
  else if( action == wxT("APPLY" ) )
  {
    Apply();
  }
}

// end of file
