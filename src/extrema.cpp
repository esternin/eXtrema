/*
Copyright (C) 2005,...,2008 Joseph L. Chuma, TRIUMF

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
#include <stdexcept>

#include "wx/image.h"
#include "wx/fileconf.h"

#include "extrema.h"
#include "ExGlobals.h"
#include "AnalysisWindow.h"

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution and also implements the
// accessor function wxGetApp() which will return the reference of the right type
// (i.e. extrema and not wxApp)

IMPLEMENT_APP(extrema)

// 'main program' equivalent, program execution starts here
bool extrema::OnInit()
{
  ::wxInitAllImageHandlers();
  //
  wxFileConfig *config = new wxFileConfig( wxT("extrema") );
  if( !config )return false;
  wxConfigBase::Set( config );
  wxConfigBase::DontCreateOnDemand();
  //
  // Initialize creates the GRA_wxWidgets instance
  ExGlobals::Initialize();
  //
  // Create the analysis window which is the main application window.  The analysis
  // window creates the VisualizationWindow, which creates the graphics window inside
  // it and informs ExGlobals of the pointer to the graphics window.
  //
  analysisWindow_ = new AnalysisWindow( this );
  if( !analysisWindow_ )
  {
    delete config;
    std::cerr << "*** ERROR: could not open analysis window ***\n";
    return false; // application exits immediately
  }
  SetTopWindow( analysisWindow_ );
  //
  try
  {
    ExGlobals::ExecuteInit();
  }
  catch( std::runtime_error const &e )
  {
    delete config;
    std::cerr << "*** ERROR: failure trying to run ExecuteInit() ***\n";
    std::cerr << e.what() << std::endl;
    return false;
  }
  //
  // success: wxApp::OnRun() will be called which will enter the main message
  // loop and the application will run
  //
  return true;
}

void extrema::QuitApp()
{
  wxMessageDialog md( (wxWindow*)analysisWindow_,
                           wxT("Do you really want to quit?"),
                           wxT("Confirm quit"),
                           wxYES_NO|wxICON_QUESTION );
  if( md.ShowModal() == wxID_YES )
  {
    analysisWindow_->Close(true);
    ExitMainLoop();
  }
}

void extrema::WriteOutput( wxString const &s )
{
  analysisWindow_->WriteOutput( s );
}

// end of file
