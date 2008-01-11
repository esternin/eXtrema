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
#include "wx/config.h"
#include "wx/stattext.h"

#include "FontChooser.h"
#include "ExGlobals.h"
#include "GRA_font.h"
#include "GRA_fontControl.h"
#include "GRA_colorControl.h"
#include "GRA_colorMap.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them
//
BEGIN_EVENT_TABLE( FontChooser, wxDialog )
  EVT_RADIOBUTTON( ID_white, FontChooser::OnWhite )
  EVT_RADIOBUTTON( ID_purple, FontChooser::OnPurple )
  EVT_RADIOBUTTON( ID_salmon, FontChooser::OnSalmon )
  EVT_RADIOBUTTON( ID_maroon, FontChooser::OnMaroon )
  EVT_RADIOBUTTON( ID_black, FontChooser::OnBlack )
  EVT_RADIOBUTTON( ID_yellow, FontChooser::OnYellow )
  EVT_RADIOBUTTON( ID_sienna, FontChooser::OnSienna )
  EVT_RADIOBUTTON( ID_navy, FontChooser::OnNavy )
  EVT_RADIOBUTTON( ID_red, FontChooser::OnRed )
  EVT_RADIOBUTTON( ID_cyan, FontChooser::OnCyan )
  EVT_RADIOBUTTON( ID_tan, FontChooser::OnTan )
  EVT_RADIOBUTTON( ID_olive, FontChooser::OnOlive )
  EVT_RADIOBUTTON( ID_green, FontChooser::OnGreen )
  EVT_RADIOBUTTON( ID_brown, FontChooser::OnBrown )
  EVT_RADIOBUTTON( ID_fuchsia, FontChooser::OnFuchsia )
  EVT_RADIOBUTTON( ID_silver, FontChooser::OnSilver )
  EVT_RADIOBUTTON( ID_blue, FontChooser::OnBlue )
  EVT_RADIOBUTTON( ID_coral, FontChooser::OnCoral )
  EVT_RADIOBUTTON( ID_lime, FontChooser::OnLime )
  EVT_RADIOBUTTON( ID_teal, FontChooser::OnTeal )
  EVT_GRID_SELECT_CELL( FontChooser::OnMap )
  EVT_COMBOBOX( ID_font, FontChooser::OnFont )
  EVT_BUTTON( wxID_OK, FontChooser::OnOK )
  EVT_BUTTON( wxID_CANCEL, FontChooser::OnCancel )
  EVT_CLOSE( FontChooser::CloseEventHandler )
END_EVENT_TABLE()

FontChooser::FontChooser( wxWindow *parent )
    : wxDialog(parent,wxID_ANY,wxT("Font chooser"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP),
      parent_(parent)
{
  CreateForm();

  wxConfigBase *config = wxConfigBase::Get();
  int ulx = config->Read( wxT("/FontChooser/UPPERLEFTX"), 0l );
  int uly = config->Read( wxT("/FontChooser/UPPERLEFTY"), 640l );
  int width = config->Read( wxT("/FontChooser/WIDTH"), 480l );
  int height = config->Read( wxT("/FontChooser/HEIGHT"), 410l );
  SetSize( ulx, uly, width, height );

  Show( true );

  long lTmp;
  config->Read( wxT("/FontChooser/FONTCODE"), &lTmp, 12l );
  fontCode_ = static_cast<int>(lTmp);
  fontCB_->SetSelection( fontCode_ );
  wxString sTmp;
  config->Read( wxT("/FontChooser/TEXTHEIGHT"), &sTmp, wxT("") );
  heightTC_->SetValue( sTmp );
  sTmp.clear();
  config->Read( wxT("/FontChooser/ANGLE"), &sTmp, wxT("") );
  angleTC_->SetValue( sTmp );
  sTmp.clear();
  config->Read( wxT("/FontChooser/COLORCODE"), &lTmp, -1l );
  colorCode_ = static_cast<int>(lTmp);

  Layout();

  SetFontSample();
}

void FontChooser::CreateForm()
{
  wxBoxSizer *mainSizer = new wxBoxSizer( wxVERTICAL );

  wxPanel *topPanel = new wxPanel( this, wxID_ANY );
  wxBoxSizer *topSizer = new wxBoxSizer( wxHORIZONTAL );

  topSizer->AddSpacer( 20 );
  topSizer->Add( new wxStaticText(topPanel,wxID_ANY,wxT("Font ")), wxSizerFlags(0).Right().Border(wxTOP,5) );
  fontCB_ = new wxComboBox( topPanel, ID_font, wxT(""), wxDefaultPosition, wxSize(200,25), 0, 0, wxCB_READONLY );
  fontCB_->Append( wxT("DECORATIVE") );
  fontCB_->Append( wxT("DECORATIVEBOLD") );
  fontCB_->Append( wxT("DECORATIVEBOLDITALIC") );
  fontCB_->Append( wxT("DECORATIVEITALIC") );
  fontCB_->Append( wxT("ROMAN") );
  fontCB_->Append( wxT("ROMANBOLD") );
  fontCB_->Append( wxT("ROMANBOLDITALIC") );
  fontCB_->Append( wxT("ROMANITALIC") );
  fontCB_->Append( wxT("SCRIPT") );
  fontCB_->Append( wxT("SCRIPTBOLD") );
  fontCB_->Append( wxT("SCRIPTBOLDITALIC") );
  fontCB_->Append( wxT("SCRIPTITALIC") );
  fontCB_->Append( wxT("SWISS") );
  fontCB_->Append( wxT("SWISSBOLD") );
  fontCB_->Append( wxT("SWISSBOLDITALIC") );
  fontCB_->Append( wxT("SWISSITALIC") );
  fontCB_->Append( wxT("MODERN") );
  fontCB_->Append( wxT("MODERNBOLD") );
  fontCB_->Append( wxT("MODERNBOLDITALIC") );
  fontCB_->Append( wxT("MODERNITALIC") );
  fontCB_->Append( wxT("TELETYPE") );
  fontCB_->Append( wxT("TELETYPEBOLD") );
  fontCB_->Append( wxT("TELETYPEBOLDITALIC") );
  fontCB_->Append( wxT("TELETYPEITALIC") );
  fontCB_->Append( wxT("ARIAL") );
  fontCB_->Append( wxT("ARIALBLACK") );
  fontCB_->Append( wxT("ARIALBOLD") );
  fontCB_->Append( wxT("ARIALBOLDITALIC") );
  fontCB_->Append( wxT("ARIALITALIC") );
  fontCB_->Append( wxT("BLACKCHANCERY") );
  fontCB_->Append( wxT("COMICSANSMS") );
  fontCB_->Append( wxT("COMICSANSMSBOLD") );
  fontCB_->Append( wxT("COURIERNEW") );
  fontCB_->Append( wxT("COURIERNEWBOLD") );
  fontCB_->Append( wxT("COURIERNEWBOLDITALIC") );
  fontCB_->Append( wxT("COURIERNEWITALIC") );
  fontCB_->Append( wxT("GEORGIA") );
  fontCB_->Append( wxT("GEORGIABOLD") );
  fontCB_->Append( wxT("GEORGIABOLDITALIC") );
  fontCB_->Append( wxT("GEORGIAITALIC") );
  fontCB_->Append( wxT("IMPACT") );
  fontCB_->Append( wxT("MONOTYPE") );
  fontCB_->Append( wxT("SYMBOL") );
  fontCB_->Append( wxT("TIMESNEWROMAN") );
  fontCB_->Append( wxT("TIMESNEWROMANBOLD") );
  fontCB_->Append( wxT("TIMESNEWROMANBOLDITALIC") );
  fontCB_->Append( wxT("TIMESNEWROMANITALIC") );
  fontCB_->Append( wxT("TREBUCHETMS") );
  fontCB_->Append( wxT("TREBUCHETMSBOLD") );
  fontCB_->Append( wxT("TREBUCHETMSBOLDITALIC") );
  fontCB_->Append( wxT("TREBUCHETMSITALIC") );
  fontCB_->Append( wxT("VERDANA") );
  fontCB_->Append( wxT("VERDANABOLD") );
  fontCB_->Append( wxT("VERDANABOLDITALIC") );
  fontCB_->Append( wxT("VERDANAITALIC") );
  fontCB_->Append( wxT("WEBDINGS") );
  fontCB_->Append( wxT("WINGDINGS") );
  topSizer->Add( fontCB_, wxSizerFlags(1).Left().Border(wxALL,2) );
  topPanel->SetSizer( topSizer );
  mainSizer->Add( topPanel, wxSizerFlags(0).Expand().Border(wxALL,2) );

  wxPanel *midPanel = new wxPanel( this, wxID_ANY );
  wxBoxSizer *midSizer = new wxBoxSizer( wxHORIZONTAL );

  midSizer->AddSpacer( 5 );
  midSizer->Add( new wxStaticText(midPanel,wxID_ANY,wxT("Sample ")), wxSizerFlags(0).Right().Border(wxTOP,5) );
  sampleText_ = new MySampleText( midPanel );
  midSizer->Add( sampleText_, wxSizerFlags(1).Left().Border(wxALL,2) );
  midPanel->SetSizer( midSizer );
  mainSizer->Add( midPanel, wxSizerFlags(0).Expand().Border(wxALL,2) );

  colorNB_ = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP );
  namedPage_ = new wxPanel( colorNB_, wxID_ANY );
  wxGridSizer *namedSizer = new wxGridSizer( 8, 3, 1, 1 );

  mainSizer->AddSpacer( 10 );

  namedRB_[0] = new wxRadioButton( namedPage_, ID_white, wxT("White"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
  namedSizer->Add( namedRB_[0], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[5] = new wxRadioButton( namedPage_, ID_purple, wxT("Purple") );
  namedSizer->Add( namedRB_[5], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[10] = new wxRadioButton( namedPage_, ID_salmon, wxT("Salmon") );
  namedSizer->Add( namedRB_[10], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[15] = new wxRadioButton( namedPage_, ID_maroon, wxT("Maroon") );
  namedSizer->Add( namedRB_[15], wxSizerFlags(0).Left().Border(wxALL,2) );

  namedRB_[1] = new wxRadioButton( namedPage_, ID_black, wxT("Black") );
  namedSizer->Add( namedRB_[1], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[6] = new wxRadioButton( namedPage_, ID_yellow, wxT("Yellow") );
  namedSizer->Add( namedRB_[6], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[11] = new wxRadioButton( namedPage_, ID_sienna, wxT("Sienna") );
  namedSizer->Add( namedRB_[11], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[16] = new wxRadioButton( namedPage_, ID_navy, wxT("Navy") );
  namedSizer->Add( namedRB_[16], wxSizerFlags(0).Left().Border(wxALL,2) );

  namedRB_[2] = new wxRadioButton( namedPage_, ID_red, wxT("Red") );
  namedSizer->Add( namedRB_[2], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[7] = new wxRadioButton( namedPage_, ID_cyan, wxT("Cyan") );
  namedSizer->Add( namedRB_[7], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[12] = new wxRadioButton( namedPage_, ID_tan, wxT("Tan") );
  namedSizer->Add( namedRB_[12], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[17] = new wxRadioButton( namedPage_, ID_olive, wxT("Olive") );
  namedSizer->Add( namedRB_[17], wxSizerFlags(0).Left().Border(wxALL,2) );

  namedRB_[3] = new wxRadioButton( namedPage_, ID_green, wxT("Green") );
  namedSizer->Add( namedRB_[3], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[8] = new wxRadioButton( namedPage_, ID_brown, wxT("Brown") );
  namedSizer->Add( namedRB_[8], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[13] = new wxRadioButton( namedPage_, ID_fuchsia, wxT("Fuchsia") );
  namedSizer->Add( namedRB_[13], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[18] = new wxRadioButton( namedPage_, ID_silver, wxT("Silver") );
  namedSizer->Add( namedRB_[18], wxSizerFlags(0).Left().Border(wxALL,2) );

  namedRB_[4] = new wxRadioButton( namedPage_, ID_blue, wxT("Blue") );
  namedSizer->Add( namedRB_[4], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[9] = new wxRadioButton( namedPage_, ID_coral, wxT("Coral") );
  namedSizer->Add( namedRB_[9], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[14] = new wxRadioButton( namedPage_, ID_lime, wxT("Lime") );
  namedSizer->Add( namedRB_[14], wxSizerFlags(0).Left().Border(wxALL,2) );
  namedRB_[19] = new wxRadioButton( namedPage_, ID_teal, wxT("Teal") );
  namedSizer->Add( namedRB_[19], wxSizerFlags(0).Left().Border(wxALL,2) );

  namedPage_->SetSizer( namedSizer );
  colorNB_->AddPage( namedPage_, wxT("Named colours"), true );

  mapPage_ = new wxPanel( colorNB_, wxID_ANY );
  wxBoxSizer *mapSizer = new wxBoxSizer( wxHORIZONTAL );

  mapGrid_ = new wxGrid( mapPage_, ID_map, wxDefaultPosition, wxSize(400,200), wxSIMPLE_BORDER );
  int nColors = GRA_colorControl::GetColorMap()->GetSize();
  int ncols = static_cast<int>( sqrt(static_cast<double>(nColors))+0.5 );
  int nrows = nColors/ncols;
  while( ncols*nrows < nColors )++nrows;
  mapGrid_->CreateGrid( nrows, ncols );
  mapGrid_->BeginBatch();
  bool done = false;
  for( int j=0; j<ncols; ++j )
  {
    mapGrid_->SetColSize( j, 20 );
    mapGrid_->SetColLabelValue( j, wxString()<<(j+1) );
  }
  for( int i=0; i<nrows; ++i )
  {
    mapGrid_->SetRowSize( i, 20 );
    for( int j=0; j<ncols; ++j )
    {
      mapGrid_->SetReadOnly( i, j, true );
      if( (j+1)+i*ncols > nColors )
      {
        done = true;
        break;
      }
      mapGrid_->SetCellBackgroundColour( i, j, ExGlobals::GetwxColor(j+i*ncols+1) );
    }
    if( done )break;
  }
  mapGrid_->EndBatch();
  mapGrid_->ForceRefresh();
  mapSizer->Add( mapGrid_, wxSizerFlags(0).Center().Border(wxALL,2) );
  mapPage_->SetSizer( mapSizer );
  colorNB_->AddPage( mapPage_, wxT("Colour map"), false );
  mainSizer->Add( colorNB_, wxSizerFlags(0).Center().Border(wxALL,2) );

  mainSizer->AddSpacer( 10 );

  wxPanel *haPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *haSizer = new wxBoxSizer( wxHORIZONTAL );

  haSizer->Add( new wxStaticText(haPanel,wxID_ANY,wxT("%Height")), wxSizerFlags(0).Right().Border(wxTOP,5) );
  heightTC_ = new wxTextCtrl( haPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(150,25) );
  haSizer->Add( heightTC_, wxSizerFlags(1).Left().Border(wxALL,2) );
  haSizer->AddSpacer( 10 );
  haSizer->Add( new wxStaticText(haPanel,wxID_ANY,wxT("Angle")), wxSizerFlags(0).Right().Border(wxTOP,5) );
  angleTC_ = new wxTextCtrl( haPanel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(150,25) );
  haSizer->Add( angleTC_, wxSizerFlags(1).Left().Border(wxALL,2) );
  
  haPanel->SetSizer( haSizer );
  mainSizer->Add( haPanel, wxSizerFlags(0).Center().Border(wxALL,2) );
  
  wxPanel *bottomPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
  wxBoxSizer *bottomSizer = new wxBoxSizer( wxHORIZONTAL );
  
  wxButton *okButton = new wxButton( bottomPanel, wxID_OK, wxT("OK") );
  okButton->SetToolTip( wxT("accept the font as chosen") );
  bottomSizer->Add( okButton, wxSizerFlags(0).Border(wxALL,10) );
  
  wxButton *cancelButton = new wxButton( bottomPanel, wxID_CANCEL, wxT("Cancel") );
  cancelButton->SetToolTip( wxT("close this form making no changes to the current font") );
  bottomSizer->Add( cancelButton, wxSizerFlags(0).Border(wxALL,10) );
  
  bottomPanel->SetSizer( bottomSizer );
  mainSizer->Add( bottomPanel, wxSizerFlags(0).Centre().Border(wxALL,1) );
  
  SetSizer( mainSizer );
}

void FontChooser::CloseEventHandler( wxCloseEvent &WXUNUSED(event) )
{
  wxConfigBase *config = wxConfigBase::Get();
  if( config )
  {
    int ulx, uly;
    GetPosition( &ulx, &uly );
    config->Write( wxT("/FontChooser/UPPERLEFTX"), static_cast<long>(ulx) );
    config->Write( wxT("/FontChooser/UPPERLEFTY"), static_cast<long>(uly) );
    int width, height;
    GetSize( &width, &height );
    config->Write( wxT("/FontChooser/WIDTH"), static_cast<long>(width) );
    config->Write( wxT("/FontChooser/HEIGHT"), static_cast<long>(height) );
    //
    config->Write( wxT("/FontChooser/FONTCODE"), static_cast<long>(fontCode_) );
    config->Write( wxT("/FontChooser/TEXTHEIGHT"), heightTC_->GetValue() );
    config->Write( wxT("/FontChooser/ANGLE"), angleTC_->GetValue() );
    config->Write( wxT("/FontChooser/COLORCODE"), static_cast<long>(colorCode_) );
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
  Destroy();
}

void FontChooser::SetFontSample()
{
  sampleText_->SetText( GRA_fontControl::GetFont(fontCode_)->GetwxFont(), ExGlobals::GetwxColor(colorCode_) );
}

void FontChooser::SetFont( int fontCode, int colorCode, double height, double angle )
{
  fontCode_ = fontCode;
  fontCB_->SetSelection( fontCode_ );
  colorCode_ = colorCode;
  if( colorCode_ > 0 )colorNB_->SetSelection( 1 );
  else
  {
    colorNB_->SetSelection( 0 );
    namedRB_[-1*colorCode]->SetValue( true );
  }
  heightTC_->Clear();
  *heightTC_ << height;
  angleTC_->Clear();
  *angleTC_ << angle;
  SetFontSample();
}

void FontChooser::OnFont( wxCommandEvent &WXUNUSED(event) )
{
  fontCode_ = GRA_fontControl::GetFontCode( fontCB_->GetStringSelection() );
  SetFontSample();
}

void FontChooser::OnMap( wxGridEvent &event )
{
  int row = event.GetRow();
  int col = event.GetCol();
  colorCode_ = col+row*mapGrid_->GetNumberCols() + 1;
  SetFontSample();
}

void FontChooser::OnWhite( wxCommandEvent &event )
{
  colorCode_ = 0;
  SetFontSample();
}

void FontChooser::OnBlack( wxCommandEvent &event )
{
  colorCode_ = -1;
  SetFontSample();
}

void FontChooser::OnRed( wxCommandEvent &event )
{
  colorCode_ = -2;
  SetFontSample();
}

void FontChooser::OnGreen( wxCommandEvent &event )
{
  colorCode_ = -3;
  SetFontSample();
}

void FontChooser::OnBlue( wxCommandEvent &event )
{
  colorCode_ = -4;
  SetFontSample();
}

void FontChooser::OnPurple( wxCommandEvent &event )
{
  colorCode_ = -5;
  SetFontSample();
}

void FontChooser::OnYellow( wxCommandEvent &event )
{
  colorCode_ = -6;
  SetFontSample();
}

void FontChooser::OnCyan( wxCommandEvent &event )
{
  colorCode_ = -7;
  SetFontSample();
}

void FontChooser::OnBrown( wxCommandEvent &event )
{
  colorCode_ = -8;
  SetFontSample();
}

void FontChooser::OnCoral( wxCommandEvent &event )
{
  colorCode_ = -9;
  SetFontSample();
}

void FontChooser::OnSalmon( wxCommandEvent &event )
{
  colorCode_ = -10;
  SetFontSample();
}

void FontChooser::OnSienna( wxCommandEvent &event )
{
  colorCode_ = -11;
  SetFontSample();
}

void FontChooser::OnTan( wxCommandEvent &event )
{
  colorCode_ = -12;
  SetFontSample();
}

void FontChooser::OnFuchsia( wxCommandEvent &event )
{
  colorCode_ = -13;
  SetFontSample();
}

void FontChooser::OnLime( wxCommandEvent &event )
{
  colorCode_ = -14;
  SetFontSample();
}

void FontChooser::OnMaroon( wxCommandEvent &event )
{
  colorCode_ = -15;
  SetFontSample();
}

void FontChooser::OnNavy( wxCommandEvent &event )
{
  colorCode_ = -16;
  SetFontSample();
}

void FontChooser::OnOlive( wxCommandEvent &event )
{
  colorCode_ = -17;
  SetFontSample();
}

void FontChooser::OnSilver( wxCommandEvent &event )
{
  colorCode_ = -18;
  SetFontSample();
}

void FontChooser::OnTeal( wxCommandEvent &event )
{
  colorCode_ = -19;
  SetFontSample();
}

void FontChooser::OnOK( wxCommandEvent &WXUNUSED(event) )
{
  double height;
  if( !heightTC_->GetValue().ToDouble(&height) )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxT("invalid value for text height"), wxT("Fatal error"), wxOK|wxICON_ERROR );
    md->ShowModal();
    return;
  }
  double angle;
  if( !angleTC_->GetValue().ToDouble(&angle) )
  {
    wxMessageDialog *md = new wxMessageDialog( this, wxT("invalid value for text angle"), wxT("Fatal error"), wxOK|wxICON_ERROR );
    md->ShowModal();
    return;
  }
  ExGlobals::SetWorkingFont( fontCode_, colorCode_, height, angle );
  Close();
}

void FontChooser::OnCancel( wxCommandEvent &WXUNUSED(event) )
{ Close(); }

BEGIN_EVENT_TABLE( MySampleText, wxWindow )
  EVT_PAINT( MySampleText::OnPaint )
END_EVENT_TABLE()

MySampleText::MySampleText( wxWindow *parent )
    : wxWindow( parent, wxID_ANY, wxDefaultPosition, wxSize(300,25), wxRAISED_BORDER ),
      font_(wxNullFont), colour_(wxNullColour)
{}

void MySampleText::OnPaint( wxPaintEvent &event )
{
  if( font_.Ok() && colour_.Ok() )
  {
    font_.SetPointSize( 10 );
    wxPaintDC dc( this );
    PrepareDC( dc );
    dc.SetBackground( wxBrush(wxT("WHITE"),wxSOLID) );
    dc.Clear();
    dc.SetFont( font_ );
    dc.SetTextForeground( colour_ );
    dc.DrawText( wxT("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"), 1, 5 );
  }
}

void MySampleText::SetText( wxFont &font, wxColour colour )
{
  font_ = font;
  colour_ = colour;
  //
  font.SetPointSize( 10 );
  wxClientDC dc( this );
  PrepareDC( dc );
  dc.SetBackground( wxBrush(wxT("WHITE"),wxSOLID) );
  dc.Clear();
  dc.SetFont( font );
  dc.SetTextForeground( colour );
  dc.DrawText( wxT("AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"), 1, 5 );
}

// end of file
