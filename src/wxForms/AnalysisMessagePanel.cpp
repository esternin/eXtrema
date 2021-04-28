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
#include <fstream>

#include "wx/fontutil.h"

#include "AnalysisMessagePanel.h"
#include "AnalysisWindow.h"
#include "ExGlobals.h"

// the event tables connect the wxWidgets events with the
// event handler functions which process them

BEGIN_EVENT_TABLE( AnalysisMessagePanel, wxPanel )
END_EVENT_TABLE()

AnalysisMessagePanel::AnalysisMessagePanel( AnalysisWindow *parent )
    : wxPanel(parent,-1,wxDefaultPosition,wxDefaultSize)
{
  // this panel will expand both horizontally and vertically
  //
  wxBoxSizer *sizer = new wxBoxSizer( wxVERTICAL );

  sizer->Add( new wxStaticText(this,-1,wxT("Messages (output field):")),
              wxSizerFlags(0).Border(wxTOP,10) );

  cmndHistoryWindow_= new CommandHistoryWindow( this );
  sizer->Add( cmndHistoryWindow_, wxSizerFlags(1).Align(0).Expand().Border(wxALL,1) );
  
  SetSizer( sizer );
}

void AnalysisMessagePanel::WriteOutput( wxString const &s )
{ cmndHistoryWindow_->WriteOutput(s); }

void AnalysisMessagePanel::ClearOutput()
{ cmndHistoryWindow_->Clear(); }

wxString AnalysisMessagePanel::GetValue()
{ return cmndHistoryWindow_->GetValue(); }

//----------------------------------------------------------------------------------------

BEGIN_EVENT_TABLE( CommandHistoryWindow, wxTextCtrl )
  EVT_RIGHT_DOWN( CommandHistoryWindow::OnMouseRightDown )
END_EVENT_TABLE()

CommandHistoryWindow::CommandHistoryWindow( AnalysisMessagePanel *amp )
    : wxTextCtrl( amp, ID_commandHistory, wxT(""), wxDefaultPosition, wxDefaultSize,
                  wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL )
{
  SetToolTip( wxT("command history and message window\nright click to save to a file\nCTRL^C to copy selected to clipboard\nCTRL^A to select all") );
  wxTextAttr ta( GetDefaultStyle() );

  wxFont f( 10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );

  ta.SetFont( f );
  SetDefaultStyle( ta );
}

void CommandHistoryWindow::WriteOutput( wxString const &s )
{ (*this) << s << wxT('\n'); }

void CommandHistoryWindow::OnMouseRightDown( wxMouseEvent &event )
{
  event.Skip();
  std::ios_base::openmode mode;
  wxFileDialog fd( this, wxT("Save command history"), wxT(""), wxT(""),
                         wxT("Text files (*.txt)|*.txt|Any file (*.*)|*.*"),
                         wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR );
  mode = std::ios_base::out;
  wxString filename;
  if( fd.ShowModal() == wxID_OK )filename = fd.GetPath();
  if( filename.empty() )return;
  std::ofstream f( filename.mb_str(wxConvUTF8), mode );
  if( !f.is_open() )
  {
    wxMessageBox( wxString()<<wxT("could not open ")<<filename,
                  wxT("Fatal error"), wxOK|wxICON_ERROR, this );
    return;
  }
  f << GetValue().mb_str(wxConvUTF8);
  f.close();
}
  
// end of file
