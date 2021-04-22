/*
Copyright (C) 2005,...,2010 Joseph L. Chuma

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
#include <limits>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdlib.h>

#include "wx/html/helpctrl.h"
#include "wx/utils.h"
#include "wx/cmndata.h"

#include "ExGlobals.h"
#include "ExXML.h"
#include "extrema.h"
#include "EGraphicsError.h"
#include "GRA_font.h"
#include "GRA_window.h"
#include "GRA_wxWidgets.h"
#include "GRA_color.h"
#include "GRA_colorMap.h"
#include "GRA_colorControl.h"
#include "GRA_fontControl.h"
#include "GRA_sizeCharacteristic.h"
#include "GRA_distanceCharacteristic.h"
#include "GRA_intCharacteristic.h"
#include "GRA_doubleCharacteristic.h"
#include "GRA_boolCharacteristic.h"
#include "GRA_angleCharacteristic.h"
#include "GRA_fontCharacteristic.h"
#include "GRA_colorCharacteristic.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_point.h"
#include "GRA_polyline.h"
#include "GRA_polygon.h"
#include "GRA_rectangle.h"
#include "GRA_multiLineFigure.h"
#include "GRA_ellipse.h"
#include "GRA_drawableText.h"
#include "GRA_cartesianAxes.h"
#include "GRA_cartesianCurve.h"
#include "GRA_axis.h"
#include "GRA_plotSymbol.h"
#include "GRA_legend.h"
#include "GRA_errorBar.h"
#include "GRA_contourLine.h"
#include "GRA_boxPlot.h"
#include "GRA_diffusionPlot.h"
#include "GRA_ditheringPlot.h"
#include "GRA_gradientPlot.h"
#include "ECommandError.h"
#include "Command.h"
#include "CommandTable.h"
#include "FunctionTable.h"
#include "OperatorTable.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "ESyntaxError.h"
#include "Script.h"
#include "ParseLine.h"
#include "VisualizationWindow.h"
#include "AnalysisWindow.h"
#include "HintForm.h"
#include "GRA_postscript.h"
#include "GraphicsPage.h"
#include "AxisPopup.h"
#include "CurvePopup.h"
#include "TextPopup.h"
#include "LegendPopup.h"
#include "GRA_polarAxes.h"
#include "GRA_polarCurve.h"

DECLARE_APP(extrema)

namespace ExGlobals
{
double const xminWDef_ = 0.0;
double const yminWDef_ = 0.0;
double const xmaxWDef_ = 11.0;
double const ymaxWDef_ = 8.5;
int const xminMDef_ = 0;
int const yminMDef_ = 0;
int const xmaxMDef_ = 647;
int const ymaxMDef_ = 500;
double const pageBorder_ = 0.25; // edge gap of graphics page (inch)

double splineTension_;
double xminW_, yminW_, xmaxW_, ymaxW_;
double xminClip_, yminClip_, xmaxClip_, ymaxClip_;
int xminM_, yminM_, xmaxM_, ymaxM_;
//wxWindow *graphicsWindow_;
GRA_wxWidgets *graphicsOutput_;
wxString scriptExtension_, stackExtension_;
wxChar continuationCharacter_, executeCharacter_;
bool echo_, pausing_, stackIsOn_, stackSaved_, executeFlag_;
int nHistory_, maxHistory_, currentScriptNumber_, scriptNumberSave_;
std::vector<Script*> scripts_;
bool workingColorFlag_, workingFontFlag_;
double workingFontHeight_, workingFontAngle_;
int workingColorCode_, workingFontCode_, workingFontColorCode_;
wxString currentPath_, executablePath_, helpPath_, imagePath_, stackFile_;
bool noviceMode_, executeCommand_, returnCommand_, pausingScript_, restartingScript_;
wxHtmlHelpController *help_;
wxPrintData *printData_;
VisualizationWindow *visualizationWindow_;
AnalysisWindow *analysisWindow_;
std::ofstream stackStream_;
HintForm *hintForm_;
bool prepareToExecuteScript_, prepareToStopScript_, prepareToPauseScript_;
std::map<wxString,wxString> alias_;
AxisPopup *axisPopup_;
CurvePopup *curvePopup_;
TextPopup *textPopup_;
LegendPopup *legendPopup_;

void Initialize()
{
  if( !wxGetEnv(wxT("EXTREMA_DIR"),&executablePath_) )
  {
    //
    // EX_BINDIR is defined as $(prefix)/bin which is /usr/local/bin by default
    // EX_DATADIR is defined as $(prefix)/share  which is /usr/local/share by default
    //

    //std::cout << "EX_BINDIR = " << EX_BINDIR << "\n"
    //          << "EX_DATADIR = " << EX_DATADIR << "\n";        
    
    executablePath_ = wxT(EX_BINDIR);
    imagePath_ = wxString(wxT(EX_DATADIR)) + wxT("/extrema/Images");
    helpPath_ = wxString(wxT(EX_DATADIR)) + wxT("/extrema/Help");
  }
  else
  {
    // extrema is not installed, so must define environment variable EXTREMA_DIR,
    // pointing to the extrema executable, e.g.,
    //
    // $ export EXTREMA_DIR=~/extrema-x.y.z
    //
    imagePath_ = executablePath_ + wxT("/Images");
    helpPath_ = executablePath_ + wxT("/Help");
  }
  currentPath_ = ::wxGetCwd().c_str();
  //
  help_ = new wxHtmlHelpController();
  help_->SetTempDir( currentPath_ );
  if( !help_->AddBook(wxFileName(helpPath_+wxT("/extrema.hhp"))) )
    wxMessageBox( wxT("Failed to add book \"")+helpPath_+wxT("/extrema.hhp\"") );
  //
  printData_ = new wxPrintData();
  //
  splineTension_ = 0.0;
  //
  workingColorFlag_ = false;
  workingFontFlag_ = false;
  //
  //graphicsWindow_ = 0;
  visualizationWindow_ = 0;
  analysisWindow_ = 0;
  axisPopup_ = 0;
  curvePopup_ = 0;
  textPopup_ = 0;
  legendPopup_ = 0;
  //
  // default world coordinates (inches)
  //
  xminW_ = xminWDef_;
  yminW_ = yminWDef_;
  xmaxW_ = xmaxWDef_;
  ymaxW_ = ymaxWDef_;
  //
  // default clipping boundary
  //
  xminClip_ = xminW_ + pageBorder_;
  yminClip_ = yminW_ + pageBorder_;
  xmaxClip_ = xmaxW_ - pageBorder_;
  ymaxClip_ = ymaxW_ - pageBorder_;
  //
  // default monitor coordinates (pixels)   11/8.5 = 647/500
  //
  xminM_ = xminMDef_;
  yminM_ = yminMDef_;
  xmaxM_ = xmaxMDef_;
  ymaxM_ = ymaxMDef_;
  //
  GRA_colorControl::Initialize();
  GRA_fontControl::Initialize();
  //
  executeFlag_ = true;
  scriptExtension_ = wxT("pcm");
  stackExtension_ = wxT("stk");
  currentScriptNumber_ = 0;
  scriptNumberSave_ = 0;
  executeCommand_ = false;
  returnCommand_ = false;
  pausingScript_ = false;
  restartingScript_ = false;
  prepareToStopScript_ = false;
  prepareToExecuteScript_ = false;
  prepareToPauseScript_ = false;
  //
  continuationCharacter_ = wxT('-');
  executeCharacter_ = wxT('@');
  nHistory_ = 1;
  maxHistory_ = 5;
  pausing_ = false;
  echo_ = false;
  stackIsOn_ = false;
  stackSaved_ = false;
  //
  noviceMode_ = true;
  //
  hintForm_ = new HintForm();
  //
  CommandTable::GetTable()->CreateCommands();
  FunctionTable::GetTable()->CreateFunctions();
  OperatorTable::GetTable()->CreateOperators();
  NVariableTable::GetTable();
  TVariableTable::GetTable();
}

void SetAnalysisWindow( AnalysisWindow *vw )
{ analysisWindow_ = vw; }

AnalysisWindow *GetAnalysisWindow()
{ return analysisWindow_; }

void SetVisualizationWindow( VisualizationWindow *vw )
{ visualizationWindow_ = vw; }

VisualizationWindow *GetVisualizationWindow()
{ return visualizationWindow_; }

AxisPopup *GetAxisPopup( GraphicsPage *page )
{
  if( axisPopup_ )axisPopup_->Close();
  axisPopup_ = new AxisPopup( page );
  axisPopup_->Show();
  return axisPopup_;
}

void ZeroAxisPopup()
{ axisPopup_ = 0; }

CurvePopup *GetCurvePopup( GraphicsPage *page )
{
  if( curvePopup_ )curvePopup_->Close();
  curvePopup_ = new CurvePopup( page );
  curvePopup_->Show();
  return curvePopup_;
}

void ZeroCurvePopup()
{ curvePopup_ = 0; }

void DisconnectCurvePopup()
{ curvePopup_->Disconnect(); }

TextPopup *GetTextPopup( GraphicsPage *page )
{
  if( textPopup_ )textPopup_->Close();
  textPopup_ = new TextPopup( page );
  textPopup_->Show();
  return textPopup_;
}

void ZeroTextPopup()
{ textPopup_ = 0; }

void DisconnectTextPopup()
{ textPopup_->Disconnect(); }

LegendPopup *GetLegendPopup( GraphicsPage *page )
{
  if( legendPopup_ )legendPopup_->Close();
  legendPopup_ = new LegendPopup( page );
  legendPopup_->Show();
  return legendPopup_;
}

void ZeroLegendPopup()
{ legendPopup_ = 0; }

void DisconnectLegendPopup()
{ legendPopup_->Disconnect(); }

void ShowHint( std::vector<wxString> const &lines )
{ hintForm_->ShowHint(lines); }

void HideHint()
{ hintForm_->HideHint(); }

wxPrintData *GetPrintData()
{ return printData_; }
    
void StartHelp()
{ help_->DisplayContents(); }

void ExecuteInit()
{
  wxString fname( wxT("extremaInit.pcm") );
  wxString parameters;
  if( wxTheApp->argc > 1 )
  {
    wxString arg1( wxTheApp->argv[1] );
    if( arg1.size()>2 && arg1.substr(0,2)==wxT("--") )
    {
      wxString command( fname );
      if( arg1.substr(2)==wxT("script") && wxTheApp->argc>2 )command = wxTheApp->argv[2];
      std::size_t b = command.find(wxT(" "));
      fname = command.substr(0,b);
      if( b != command.npos )parameters = command.substr(b);
    }
  }
  std::ifstream in;
  in.open( fname.mb_str(wxConvUTF8) );
  if( !in.is_open() )
  {
    if( ::wxPathOnly(fname).IsEmpty() )
    {
      wxString fname2( ExGlobals::GetCurrentPath()+wxT("/")+fname );
      in.open( fname2.mb_str(wxConvUTF8) );
      if( in.is_open() )fname = fname2;
      else
      {
        fname2 = ExGlobals::GetExecutablePath()+wxT("/")+fname;
        in.open( fname2.mb_str(wxConvUTF8) );
        if( !in.is_open() )return;
        fname = fname2;
      }
    }
    else return;
  }
  in.close();
  try
  {
    ParseLine p( wxString(wxT("EXECUTE '"))+fname+wxT("'")+parameters );
    try
    {
      p.ParseIt();
    }
    catch( ESyntaxError const &e )
    {
      throw std::runtime_error( e.what() );
    }
    Command *command = CommandTable::GetTable()->Get(wxT("EXECUTE"));
    if( !command )std::runtime_error( "EXECUTE command not found in table" );
    command->Execute( &p );
  }
  catch( ECommandError const &e )
  {
    throw std::runtime_error( e.what() );
  }
  try
  {
    RunScript();
  }
  catch( std::runtime_error const &e )
  {
    ShowScriptError( e.what() );
    StopAllScripts();
  }
}

wxWindow *GetwxWindow()
{ return visualizationWindow_->GetPage(); }

GRA_wxWidgets *GetGraphicsOutput()
{ return graphicsOutput_; }

wxColour GetwxColor( GRA_color *color )
{
  int r =255, g =255, b =255;
  if( color )color->GetRGB( r, g, b );
  return wxColour( static_cast<unsigned char>(r),
                   static_cast<unsigned char>(g),
                   static_cast<unsigned char>(b) );
}

wxColour GetwxColor( int code )
{
  int r, g, b;
  GRA_colorControl::ColorCodeToRGB( code, r, g, b );
  return wxColour( static_cast<unsigned char>(r),
                   static_cast<unsigned char>(g),
                   static_cast<unsigned char>(b) );
}

void QuitApp()
{ ::wxGetApp().QuitApp(); }

void DeleteStuff()
{
  delete hintForm_;
  delete printData_;
  delete help_;
  delete graphicsOutput_;
  while( !scripts_.empty() )
  {
    delete scripts_.back();
    scripts_.pop_back();
  }
  TVariableTable::GetTable()->ClearTable();
  NVariableTable::GetTable()->ClearTable();
  GRA_fontControl::DeleteStuff();
  GRA_colorControl::DeleteStuff();
  CommandTable::GetTable()->ClearTable();
}

bool GetNoviceMode()
{ return noviceMode_; }

void SetNoviceMode( bool mode )
{ noviceMode_ = mode; }

void ToggleNoviceMode()
{ noviceMode_ = !noviceMode_; }

void SetCurrentPath( wxString const &path )
{ currentPath_ = path; }

wxString GetCurrentPath()
{ return currentPath_; }

wxString GetExecutablePath()
{ return executablePath_; }

wxString GetImagePath()
{ return imagePath_; }

wxString GetHelpPath()
{ return helpPath_; }

void SetTension( double t )
{ splineTension_ = t; }

double GetTension()
{ return splineTension_; }

wxString GetNiceNumber( double d )
{
  int width = 10;
  int precision = 3;
  std::stringstream ss;
  std::string s;
  if( fabs(d)>=1000.0 || fabs(d)<=0.01 )
  {
    ss << std::setw(width) << std::setprecision(precision) << std::scientific << d;
  }
  else
  {
    if( fabs(d) < 10.0 )
    {
      width = 6;
      precision = 3;
    }
    else if( fabs(d) < 100.0 )
    {
      width = 6;
      precision = 2;
    }
    else if( fabs(d) < 1000.0 )
    {
      width = 6;
      precision = 1;
    }
    ss << std::setw(width) << std::setprecision(precision) << std::fixed << d;
  }
  ss >> s;
  if( d >= 0.0 )s = std::string(" ")+s;
  return wxString( s.c_str(), wxConvUTF8 );
}

void SetWorkingColorFlag( bool b )
{ workingColorFlag_ = b; }

bool GetWorkingColorFlag()
{ return workingColorFlag_; }

void SetWorkingColor( int code )
{ workingColorCode_=code; workingColorFlag_=true; }

GRA_color *GetWorkingColor()
{ return GRA_colorControl::GetColor(workingColorCode_); }

void SetWorkingFontFlag( bool b )
{ workingFontFlag_ = b; }

bool GetWorkingFontFlag()
{ return workingFontFlag_; }

void SetWorkingFont( int fontCode, int colorCode, double height, double angle )
{
  workingFontFlag_ = true;
  workingFontCode_ = fontCode;
  workingFontColorCode_ = colorCode;
  workingFontHeight_ = height;
  workingFontAngle_ = angle;
}

GRA_font *GetWorkingFont()
{ return GRA_fontControl::GetFont(workingFontCode_); }

GRA_color *GetWorkingFontColor()
{ return GRA_colorControl::GetColor(workingFontColorCode_); }

double GetWorkingFontHeight()
{ return workingFontHeight_; }

double GetWorkingFontAngle()
{ return workingFontAngle_; }

void WarningMessage( wxString const &s )
{ WriteOutput( wxString(wxT("Warning: "))+s ); }

void ResetClippingBoundary()
{
  xminClip_ = xminW_ + pageBorder_;
  yminClip_ = yminW_ + pageBorder_;
  xmaxClip_ = xmaxW_ - pageBorder_;
  ymaxClip_ = ymaxW_ - pageBorder_;
}

void SetAspectRatio( double r ) // r = height/width
{
  xminW_ = xminWDef_;
  yminW_ = yminWDef_;
  if( r < 1. )
  {
    xmaxW_ = xmaxWDef_;
    ymaxW_ = xmaxWDef_*r;
  }
  else
  {
    xmaxW_ = ymaxWDef_;
    ymaxW_ = ymaxWDef_*r;
  }
  //
  xminClip_ = xminW_ + pageBorder_;
  yminClip_ = yminW_ + pageBorder_;
  xmaxClip_ = xmaxW_ - pageBorder_;
  ymaxClip_ = ymaxW_ - pageBorder_;
  //
  xminM_ = xminMDef_;
  yminM_ = yminMDef_;
  if( r < 1. )
  {
    xmaxM_ = xmaxMDef_;
    ymaxM_ = static_cast<int>(xmaxMDef_*r+0.5);
  }
  else
  {
    xmaxM_ = ymaxMDef_;
    ymaxM_ = static_cast<int>(ymaxMDef_*r+0.5);
  }
  //
  delete graphicsOutput_;
  graphicsOutput_ = new GRA_wxWidgets( xminM_, yminM_, xmaxM_, ymaxM_ );
  //
  visualizationWindow_->ClearAllPages();
  visualizationWindow_->Layout();
}

double GetAspectRatio()
{
  return (ymaxW_-yminW_)/(xmaxW_-xminW_);
}

void ResetWorldLimits()
{
  xminW_ = xminWDef_;
  yminW_ = yminWDef_;
  xmaxW_ = xmaxWDef_;
  ymaxW_ = ymaxWDef_;
  //
  xminClip_ = xminW_ + pageBorder_;
  yminClip_ = yminW_ + pageBorder_;
  xmaxClip_ = xmaxW_ - pageBorder_;
  ymaxClip_ = ymaxW_ - pageBorder_;
  //
  delete graphicsOutput_;
  graphicsOutput_ = new GRA_wxWidgets( xminM_, yminM_, xmaxM_, ymaxM_ );

  visualizationWindow_->ClearAllPages();
  //
  visualizationWindow_->ResetWindows();
}

void GetWorldLimits( double &xmin, double &ymin, double &xmax, double &ymax )
{
  xmin = xminW_;
  ymin = yminW_;
  xmax = xmaxW_;
  ymax = ymaxW_;
}

void SetClippingBoundary( double xmin, double ymin, double xmax, double ymax )
{
  // the clipping boundary will always be within the world coordinate boundary
  //
  if( xmax-xmin < std::numeric_limits<double>::epsilon() )
    throw EGraphicsError( wxT("SetClippingBoundary: xmax <= xmin") );
  if( ymax-ymin < std::numeric_limits<double>::epsilon() )
    throw EGraphicsError( wxT("SetClippingBoundary: ymax <= ymin") );
  xminClip_ = std::max(xmin,xminW_);
  yminClip_ = std::max(ymin,yminW_);
  xmaxClip_ = std::min(xmax,xmaxW_);
  ymaxClip_ = std::min(ymax,ymaxW_);
}

void GetClippingBoundary( double &xmin, double &ymin, double &xmax, double &ymax )
{
  xmin = xminClip_;
  ymin = yminClip_;
  xmax = xmaxClip_;
  ymax = ymaxClip_;
}

void GetMonitorLimits( int &xmin, int &ymin, int &xmax, int &ymax )
{
  xmin = xminM_;
  ymin = yminM_;
  xmax = xmaxM_;
  ymax = ymaxM_;
}

void SetMonitorLimits( int xmin, int ymin, int xmax, int ymax )
{
  xminM_ = xmin;
  xmaxM_ = xmax;
  yminM_ = ymin;
  ymaxM_ = ymax;
  //
  delete graphicsOutput_;
  graphicsOutput_ = new GRA_wxWidgets( xminM_, yminM_, xmaxM_, ymaxM_ );
}

wxChar GetContinuationCharacter()
{ return continuationCharacter_; }

void SetScriptExtension( wxString const &s )
{ scriptExtension_ = s; }

wxString GetScriptExtension()
{ return scriptExtension_; }

void SetStackExtension( wxString const &s )
{ stackExtension_ = s; }

wxString GetStackExtension()
{ return stackExtension_; }

bool StackIsOn()
{ return stackIsOn_; }

std::ofstream *GetStackStream()
{ return &stackStream_; }

wxString &GetStackFile()
{ return stackFile_; }

void SetStackOff()
{
  stackIsOn_ = false;
  if( stackStream_.is_open() )stackStream_.close();
  stackStream_.clear( std::ios::goodbit );
}

bool SetStackOn( wxString const &filename, bool append )
{
  stackFile_ = filename;
  if( stackStream_.is_open() )stackStream_.close();
  stackStream_.clear( std::ios::goodbit );
  std::ios_base::openmode mode = std::ios::out;
  if( append )mode |= std::ios::app;
  stackStream_.open( stackFile_.mb_str(wxConvUTF8), mode );
  //if( append )stackStream_.open( stackFile_.mb_str(wxConvUTF8), std::ios::out|std::ios::app );
  //else        stackStream_.open( stackFile_.mb_str(wxConvUTF8), std::ios::out );
  stackIsOn_ = stackStream_.is_open();
  return stackIsOn_;
}

void WriteStack( wxString const &s )
{ if( stackIsOn_ )stackStream_ << s.mb_str(wxConvUTF8) << "\n"; }

bool GetEcho()
{ return echo_; }

void SetEcho( bool e )
{ echo_ = e; }

void SetWindowNumber( int n )
{ visualizationWindow_->SetWindowNumber(n); }

int GetWindowNumber()
{ return visualizationWindow_->GetWindowNumber(); }

int GetNumberOfWindows()
{ return visualizationWindow_->GetNumberOfWindows(); }

std::vector<GRA_window*> &GetGraphWindows()
{ return visualizationWindow_->GetGraphWindows(); }

GRA_window *GetGraphWindow()
{ return visualizationWindow_->GetGraphWindow(); }

GRA_window *GetGraphWindow( int n )
{ return visualizationWindow_->GetGraphWindow(n); }

GRA_window *GetGraphWindow( double x, double y )
{ return visualizationWindow_->GetGraphWindow(x,y); }

void AddGraphWindow( GRA_window *gw )
{ visualizationWindow_->AddGraphWindow(gw); }

void DrawGraphWindows( GRA_wxWidgets *graphicsOutput, wxDC &dc )
{ visualizationWindow_->DrawGraphWindows( graphicsOutput, dc ); }
    
void SetMaxHistory( int i )
{ maxHistory_ = i; }

int GetMaxHistory()
{ return maxHistory_; }
    
void SetNHistory( int i )
{ nHistory_ = i; }

int GetNHistory()
{ return nHistory_; }

void ReplotAllWindows()
{ visualizationWindow_->ReplotAllWindows(); }

void ReplotCurrentWindow( bool repaint )
{ visualizationWindow_->ReplotCurrentWindow(repaint); }

void ClearGraphicsMonitor()
{
  wxClientDC dc( visualizationWindow_->GetPage() );
  dc.Clear();
  visualizationWindow_->DisplayBackgrounds( graphicsOutput_, dc );
}

void ClearGraphicsWindow( bool replot, int n )
{
  GRA_window *gw = GetGraphWindow( n );
  SetWindowNumber( n );
  gw->Erase();
  wxClientDC dc( visualizationWindow_->GetPage() );
  gw->DisplayBackground( graphicsOutput_, dc );
  if( replot )gw->Clear();
}
  
void ClearHistory()
{ analysisWindow_->ClearOutput(); }

void WriteOutput( wxString const &s )
{ ::wxGetApp().WriteOutput( s ); }

bool NotInaScript()
{ return (currentScriptNumber_==0); }

Script *GetScript()
{
  if( currentScriptNumber_ == 0 )return 0;
  return scripts_[currentScriptNumber_-1];
}

void SetExecuteCommand( bool b )
{ executeCommand_ = b; }

bool GetExecuteCommand()
{ return executeCommand_; }

bool GetReturnCommand()
{ return returnCommand_; }

void SetReturnCommand( bool b )
{ returnCommand_ = b; }

void SetRestartingScript( bool b )
{ restartingScript_ = b; }

bool GetRestartingScript()
{ return restartingScript_; }

void PrepareToStopScript()
{ prepareToStopScript_ = true; }

void PrepareToExecuteScript()
{ prepareToExecuteScript_ = true; }

void PrepareToPauseScript()
{ prepareToPauseScript_ = true; }

bool GetPausingScript()
{ return pausingScript_; }

void PauseScripts()
{
  if( currentScriptNumber_ > 0 )
  {
    pausingScript_ = true;
    currentScriptNumber_ = 0;
  }
}

void RestartScripts()
{
  if( !pausingScript_ )return;
  pausingScript_ = false;
  restartingScript_ = true;
  currentScriptNumber_ = 1;
  try
  {
    RunScript();
  }
  catch( std::runtime_error const &e )
  {
    ExGlobals::ShowScriptError( e.what() );
    ExGlobals::StopAllScripts();
  }
}

void IncrementScript()
{ ++currentScriptNumber_; }

void SetScript( Script *s )
{
  // stackIsOn_ is set false when executing the top level script
  // so stackSaved_ is used to reset stackIsOn_ after finishing
  // with the top level script
  //
  if( scripts_.empty() )
  {
    stackSaved_ = stackIsOn_;
    stackIsOn_ = false;
  }
  s->SetEcho( echo_ );
  scripts_.push_back( s );
  ++currentScriptNumber_;
}

void RunScript()
{
  scripts_[currentScriptNumber_-1]->Run();
  if( pausingScript_ )return;
  StopScript();
}

void StopScript()
{
  if( scripts_.empty() )return;
  delete scripts_.back();
  scripts_.pop_back();
  if( scripts_.empty() )stackIsOn_ = stackSaved_;
  --currentScriptNumber_;
}

void ShowScriptError( char const *errorMessage )
{
  wxString s( wxT("ERROR in script file: ") );
  s += scripts_[currentScriptNumber_-1]->Filename()+wxT("\n");
  s += wxString(errorMessage,wxConvUTF8)+wxT("\n");
  s += wxT("The following line caused the error:\n");
  s += scripts_[currentScriptNumber_-1]->CurrentLine();
  wxMessageBox( s, wxT("Script error"), wxOK|wxICON_ERROR );
}

void StopAllScripts()
{
  if( scripts_.empty() )return;
  std::vector<Script*>::const_iterator sEnd = scripts_.end();
  for( std::vector<Script*>::const_iterator i=scripts_.begin(); i!=sEnd; ++i )delete *i;
  scripts_.clear();
  stackIsOn_ = stackSaved_;
  currentScriptNumber_ = 0;
}

bool GetExecuteFlag()
{ return executeFlag_; }

void PreParse( wxString &line )
{
  // Cut off everything after encountering the comment character,
  // by default an exclamation mark, !, if not in quotes.
  // A tab is treated like a blank.
  //
  int stateTable[6][4] = {
      { 2,   1,   1, 100 },
      { 3,   5,   4,   4 },   // opening quote
      { 3,   5,   4,   4 },   // opening quote
      { 3,   5,   4,   4 },   // inside quotes
      { 3,   5,   4,   4 },   // closing quote
      { 3,   1,   1, 100 } }; // closing final quote
  //   `    '  othr   !
  //
  int classes[128];
  for( std::size_t i=0; i<128; ++i )classes[i] = 3;
  classes[9] = 5;
  classes[34] = 2;  // double quote
  classes[39] = 2;  // single quote
  classes[96] = 1;  // opening quote
  classes[toascii(Script::GetCommentCharacter())] = 4;
  //
  int currentState, newState = 1;
  int iquote = 0;
  wxString cline;
  std::size_t i = 0;
  std::size_t lineLen = line.length();
  while ( i < lineLen )
  {
    int iascii = toascii( line[i] );
    cline.append( line[i] );
    currentState = newState;
    newState = stateTable[currentState-1][classes[iascii]-1];
    switch ( newState )
    {
      case 2:
      case 3:
        ++iquote;
        break;
      case 5:
        if( --iquote == 0 )newState = 6;
        break;
      case 100:
        line = cline;
        return;
    }
    ++i;
  }
  line = cline;
  return;
}

void ProcessCommand( wxString &commandLine )
{
  if( commandLine[0] == Script::GetCommentCharacter() )return;
  else if( commandLine[0] == wxT('%') )
  {
    int err = system( wxString(commandLine.substr(1)).mb_str(wxConvUTF8) );
    return;
  }
  else if( commandLine[0] == executeCharacter_ )
  {
    commandLine = wxString()<<wxT("EXECUTE ")<<commandLine.substr(1);
  }
  ParseLine p( commandLine );
  try
  {
    p.ParseIt();
  }
  catch( ESyntaxError &e )
  {
    throw std::runtime_error( e.what() );
  }
  wxString commandName;
  if( p.GetNumberOfTokens()>1 && p.IsEqualSign(1) )
  {
    commandName = wxT("EVALUATEEXPRESSION");
  }
  else
  {
    commandName = p.GetString(0).Upper();
    if( !executeFlag_ )
    {
      WriteOutput( commandLine );
      return;
    }
  }
  if( HandleAlias(commandName,commandLine,p) )return;
  Command *command = CommandTable::GetTable()->Get(commandName);
  if( !command )
  {
    wxString s( wxT("unknown command: ") );
    s += commandName;
    throw std::runtime_error( std::string(s.mb_str(wxConvUTF8)) );
  }
  try
  {
    command->Execute( &p );
  }
  catch( ECommandError &e )
  {
    throw std::runtime_error( e.what() );
  }
  if( command->GetWriteStack() )WriteStack( command->GetStackLine() );
  executeCommand_ = command->IsCommand(wxT("EXECUTE")) || prepareToExecuteScript_;
  returnCommand_ = command->IsCommand(wxT("RETURN")) || prepareToStopScript_;
  pausingScript_ = pausingScript_ || command->IsCommand(wxT("PAUSE")) || prepareToPauseScript_;
  prepareToExecuteScript_ = false;
  prepareToStopScript_ = false;
  prepareToPauseScript_ = false;
}

bool HandleAlias( wxString &name, wxString &line, ParseLine &p )
{
  std::map<wxString,wxString>::const_iterator i = alias_.find( name );
  if( i != alias_.end() )
  {
    line = line.size()>name.size() ? (*i).second+wxString(line.substr(name.size())) : (*i).second;
    if( line[0] == Script::GetCommentCharacter() )return true;
    if( line[0] == executeCharacter_ )line = wxString()<<wxT("EXECUTE ")<<line.substr(1);
    ParseLine p2( line );
    try
    {
      p2.ParseIt();
    }
    catch( ESyntaxError const &e )
    {
      throw std::runtime_error( e.what() );
    }
    if( p2.GetNumberOfTokens()>1 && p2.IsEqualSign(1) )
    {
      name = wxT("EVALUATEEXPRESSION");
    }
    else
    {
      name = p2.GetString(0).Upper();
      if( !executeFlag_ )
      {
        WriteOutput( line );
        return true;
      }
    }
    p = p2;
  }
  return false;
}

void AddAlias( wxString const &name, wxString const &value )
{
  std::map<wxString,wxString>::iterator i = alias_.find(name);
  if( i != alias_.end() )alias_.erase( i );
  alias_.insert( std::map<wxString,wxString>::value_type(name,value) );
}

bool RemoveAlias( wxString const &name )
{
  std::map<wxString,wxString>::iterator i = alias_.find(name);
  if( i != alias_.end() )
  {
    alias_.erase( i );
    return true;
  }
  return false;
}

void RemoveAliases()
{ alias_.clear(); }

void DisplayAlias( wxString const &name )
{
  if( name.empty() )return;
  std::map<wxString,wxString>::const_iterator i = alias_.find(name);
  if( i != alias_.end() )WriteOutput( wxString()<<wxT("ALIAS: ")<<name<<wxT(" -> ")<<(*i).second );
  else WriteOutput( wxString()<<name<<wxT(" is not defined as an alias") );
}

void DisplayAliases()
{
  if( alias_.size() == 0 )WriteOutput( wxT("No aliases are currently defined") );
  std::map<wxString,wxString>::const_iterator end = alias_.end();
  for( std::map<wxString,wxString>::const_iterator i=alias_.begin(); i!=end; ++i )
    WriteOutput( wxString()<<wxT("ALIAS: ")<<(*i).first<<wxT(" -> ")<<(*i).second );
}

void RefreshGraphics()
{ visualizationWindow_->RefreshGraphics(); }

void ClearWindows()
{ visualizationWindow_->ClearWindows(); }

void DefineConstants()
{
  wxString name;
  name = wxT("ALPHAMASS");
  NumericVariable::PutVariable( name, 6.64465598e-27, wxT("alpha particle mass (kg)") );
  name = wxT("ALPHAMASSMEV");
  NumericVariable::PutVariable( name, 3727.37904, wxT("alpha particle mass (MeV)") );
  name = wxT("AVOGADRO");
  NumericVariable::PutVariable( name, 6.02214199e23, wxT("Avogadro's constant (1/mol)") );
  name = wxT("BOHRRADIUS");
  NumericVariable::PutVariable( name, 0.5291772083e-10, wxT("Bohr radius (m)") );
  name = wxT("BOLTZMANN");
  NumericVariable::PutVariable( name, 1.3806503e-23, wxT("Boltzmann constant (J/K)") );
  name = wxT("ELECTRONRADIUS");
  NumericVariable::PutVariable( name, 2.817940285e-15, wxT("classical electron radius (m)") );
  name = wxT("COMPTON");
  NumericVariable::PutVariable( name, 2.426310215e-12, wxT("Compton wavelength (m)") );
  name = wxT("DEUTERONMASS");
  NumericVariable::PutVariable( name, 3.34358309e-27, wxT("deuteron particle mass (kg)") );
  name = wxT("DEUTERONMASSMEV");
  NumericVariable::PutVariable( name, 1875.612762, wxT("deuteron particle mass (MeV)") );
  name = wxT("ELECTRONMASS");
  NumericVariable::PutVariable( name, 9.10938188e-31, wxT("electron particle mass (kg)") );
  name = wxT("ELECTRONMASSMEV");
  NumericVariable::PutVariable( name, 0.510998902, wxT("electron particle mass (MeV)") );
  name = wxT("ELECTRONVOLT");
  NumericVariable::PutVariable( name, 1.602176462e-19, wxT("electron volt (J)") );
  name = wxT("ELECTRONCHARGE");
  NumericVariable::PutVariable( name, 1.602176462e-19, wxT("charge on electron (C)") );
  name = wxT("FARADAY");
  NumericVariable::PutVariable( name, 96485.3415, wxT("Faraday constant (C/mol)") );
  name = wxT("FERMICOUPLING");
  NumericVariable::PutVariable( name, 1.16639e-5, wxT("Fermi coupling constant (1/GeV^2)") );
  name = wxT("FINESTRUCTURE");
  NumericVariable::PutVariable( name, 7.297352533e-3, wxT("fine-structure constant ()") );
  name = wxT("MAGNETIC");
  NumericVariable::PutVariable( name, 12.566370614e-7, wxT("magnetic constant (N/A^2)") );
  name = wxT("NEUTRONMASS");
  NumericVariable::PutVariable( name, 1.67492716e-27, wxT("neutron particle mass (kg)") );
  name = wxT("NEUTRONMASSMEV");
  NumericVariable::PutVariable( name, 939.565330, wxT("neutron particle mass (MeV)") );
  name = wxT("GRAVITATION");
  NumericVariable::PutVariable( name, 6.673e-11, wxT("Newtonian constant of gravitation (m^3/kg/s^2)") );
  name = wxT("PLANCK");
  NumericVariable::PutVariable( name, 6.62606876e-34, wxT("Planck's constant (J s)") );
  name = wxT("HBAR");
  NumericVariable::PutVariable( name, 1.05457266e-34, wxT("Planck's constant, reduced (J s)") );
  name = wxT("PROTONMASS");
  NumericVariable::PutVariable( name, 1.67262158e-27, wxT("proton particle mass (kg)") );
  name = wxT("PROTONMASSMEV");
  NumericVariable::PutVariable( name, 938.271998, wxT("proton particle mass (MeV)") );
  name = wxT("RYDBERG");
  NumericVariable::PutVariable( name, 10973731.568549, wxT("Rydberg constant (1/m)") );
  name = wxT("LIGHTSPEED");
  NumericVariable::PutVariable( name, 299792458, wxT("speed of light in a vacuum (m/s)") );
  name = wxT("GRAVITYACC");
  NumericVariable::PutVariable( name, 9.80665, wxT("standard acceleration of gravity (m/s^2)") );
  name = wxT("ATMOSPHERE");
  NumericVariable::PutVariable( name, 101325, wxT("standard atmosphere (Pa)") );
  name = wxT("STEFANBOLTZMANN");
  NumericVariable::PutVariable( name, 5.670400e-8, wxT("Stefan-Boltzmann constant (W/m^2/K^4)") );
  name = wxT("THOMSON");
  NumericVariable::PutVariable( name, 0.665245854e-28, wxT("Thomson cross section (m^2)") );
  name = wxT("AMASS");
  NumericVariable::PutVariable( name, 1.66053873e-27, wxT("unified atomic mass unit (kg)") );
  name = wxT("PI");
  NumericVariable::PutVariable( name, 3.14159265358979323846, wxT("pi") );
  name = wxT("EARTHRADIUS");
  NumericVariable::PutVariable( name, 6.378140e6, wxT("radius of the earth at the equator (m)") );
  name = wxT("AU");
  NumericVariable::PutVariable( name, 1.4959787066e11, wxT("astronomical unit (m)") );
  name = wxT("EARTHMASS");
  NumericVariable::PutVariable( name, 5.97370e24, wxT("mass of the earth (kg)") );
  name = wxT("E");
  NumericVariable::PutVariable( name, 2.718281828459045235, wxT("natural logarithm base") );
  name = wxT("PARSEC");
  NumericVariable::PutVariable( name, 3.0856775807e16, wxT("parsec (m)") );
  name = wxT("SUNMASS");
  NumericVariable::PutVariable( name, 1.98892e30, wxT("mass of the sun (kg)") );
  name = wxT("SUNRADIUS");
  NumericVariable::PutVariable( name, 6.96e8, wxT("radius of the sun at the equator (m)") );
}

void DefaultSize()
{
  xminM_ = xminMDef_;
  yminM_ = yminMDef_;
  xmaxM_ = xmaxMDef_;
  ymaxM_ = ymaxMDef_;
  //
  delete graphicsOutput_;
  graphicsOutput_ = new GRA_wxWidgets( xminM_, yminM_, xmaxM_, ymaxM_ );
  wxClientDC dc( visualizationWindow_->GetPage() );
  dc.SetBackground( *wxWHITE_BRUSH );
  dc.Clear();
  //
  visualizationWindow_->DrawGraphWindows( graphicsOutput_, dc );
}

void ChangeSize( double value )
{
  xminM_ = 0;
  yminM_ = 0;
  xmaxM_ = static_cast<int>(xmaxM_*value+0.5);
  ymaxM_ = static_cast<int>(ymaxM_*value+0.5);
  delete graphicsOutput_;
  graphicsOutput_ = new GRA_wxWidgets( xminM_, yminM_, xmaxM_, ymaxM_ );
  wxClientDC dc( visualizationWindow_->GetPage() );
  dc.SetBackground( *wxWHITE_BRUSH );
  dc.Clear();
  //
  visualizationWindow_->DrawGraphWindows( graphicsOutput_, dc );
}

void InteractiveLegendFrameSetup()
{ 
  //MainGraphicsForm->SetupLegendFrame();
}

void RemoveQuotes( wxString &s )
{
  if( s.size()>1 && (s[0]==wxT('`') || s[0]==wxT('\'') || s[0]==wxT('"')) )
    s.assign( s.substr(1,s.size()-2) );
}

wxString GetFileExtension( wxString &filename )
{
  // get the file extension, if there was one given
  //
  wxString extension;
  std::size_t len = filename.size();
  std::size_t i = filename.find_last_of( wxT('.') );
  if( i != filename.npos )extension = filename.substr(i,len-i);
  return extension;
}

wxString Capitalize( wxString const &input )
{
  return wxString(input.GetChar(0)).Upper()+input.Mid(1);
}

void ToCapitalize( wxString &input )
{
  input = wxString(input.GetChar(0)).Upper()+input.Mid(1).Lower();
}

void RemoveBlanks( wxString &s )
{
  std::size_t i;
  while( (i=s.find(' ')) != s.npos )s.erase( s.begin()+i );
}

wxString IntToHex( int n )
{
  if( n > 255 )throw std::runtime_error("max value for IntToHex is 255");
  if( n < 0 )throw std::runtime_error("min value for IntToHex is 0");
  std::string sout("00");
  std::string const alphabet("0123456789ABCDEF");
  if( n >= 16 )
  {
    int i = n%16;
    sout[1] = alphabet[i];
    n = (n-i)/16;
    sout[0] = alphabet[n];
  }
  else
  {
    sout[1] = alphabet[n];
  }
  return wxString( sout.c_str(), wxConvUTF8 );
}

void SaveSession( wxString &file )
{
  ExXML xml;
  xml.OpenFileForWrite( file );
  std::ofstream *out = xml.GetStream();
  
  *out << "<extrema>\n";
  
  *out << analysisWindow_;
  *out << visualizationWindow_;
  
  *out << *NVariableTable::GetTable();
  *out << *TVariableTable::GetTable();
  
  *out << *GRA_colorControl::GetColorMap();
  
  int nPages = visualizationWindow_->GetNumberOfPages();
  for( int i=1; i<=nPages; ++i )*out << visualizationWindow_->GetPage(i);
  
  *out << "</extrema>" << std::endl;
  out->close();
}

void RestoreSession( wxString &file )
{
  ExXML xml;
  xml.OpenFileForRead( file );
  // first get the analysis window
  //
  if( !xml.GetFirstChild() )
    throw std::runtime_error(
      std::string( (wxString()<<wxT("root node extrema has no children, file: ")<<file).mb_str(wxConvUTF8) ) );

  if( xml.GetName() != wxT("analysiswindow") )
    throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("analysiswindow"),file) );

  visualizationWindow_->DeleteAllPages();
  analysisWindow_->ClearOutput();
  long top, left, height, width;
  xml.GetPropertyValue( wxT("top") ).ToLong( &top );
  xml.GetPropertyValue( wxT("left") ).ToLong( &left );
  xml.GetPropertyValue( wxT("height") ).ToLong( &height );
  xml.GetPropertyValue( wxT("width") ).ToLong( &width );
  analysisWindow_->SetSize( static_cast<int>(left), static_cast<int>(top),
                            static_cast<int>(width), static_cast<int>(height) );
  xml.GetFirstChild(); // get the messages <string> node
  if( xml.GetName() != wxT("string") )
    throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("string"),file) );
  
  analysisWindow_->WriteOutput( xml.GetTextValue() );
  
  xml.GetNextSibling(); // get the <commands> node
  if( xml.GetName() != wxT("commands") )
    throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("commands"),file) );

  long int size;
  xml.GetPropertyValue( wxT("size") ).ToLong( &size );
  for( int i=0; i<static_cast<int>(size); ++i )
  {
    i==0 ? xml.GetFirstChild() : xml.GetNextSibling();
    analysisWindow_->AddCommandString( xml.GetTextValue() );
  }
  if( size > 0L )xml.SetBackToParent();  // set current node back to <commands>
  xml.SetBackToParent(); // set current node back to <analysiswindow>
  //
  // next get the visualization window
  //
  if( !xml.GetNextSibling() )
    throw std::runtime_error(
      std::string( (wxString()<<wxT("expecting node: <visualizationwindow>, file: ")<<file).mb_str(wxConvUTF8) ) );

  if( xml.GetName() != wxT("visualizationwindow") )
    throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("visualizationwindow"),file) );
  
  xml.GetPropertyValue( wxT("top") ).ToLong( &top );
  xml.GetPropertyValue( wxT("left") ).ToLong( &left );
  xml.GetPropertyValue( wxT("height") ).ToLong( &height );
  xml.GetPropertyValue( wxT("width") ).ToLong( &width );
  double aspectRatio;
  xml.GetPropertyValue( wxT("aspectratio") ).ToDouble( &aspectRatio );
  long page;
  xml.GetPropertyValue( wxT("currentpage") ).ToLong( &page );
  visualizationWindow_->SetSize( static_cast<int>(left), static_cast<int>(top),
                                 static_cast<int>(width), static_cast<int>(height) );
  visualizationWindow_->MakeFirstPage();
  SetAspectRatio( aspectRatio );
  visualizationWindow_->ResetPages();
  visualizationWindow_->ResetWindows();
  visualizationWindow_->SetPage( static_cast<int>(page) );

  NVariableTable *nvt = NVariableTable::GetTable();
  TVariableTable *tvt = TVariableTable::GetTable();
  
  nvt->ClearTable(); // delete all numeric variables
  tvt->ClearTable(); // delete all character variables
  //
  // next get the <numericvariables> node
  //
  if( !xml.GetNextSibling() )
    throw std::runtime_error(
      std::string( (wxString()<<wxT("expecting node: <numericvariables>, file: ")<<file).mb_str(wxConvUTF8) ) );

  if( xml.GetName() != wxT("numericvariables") )
    throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("numericvariables"),file) );
  
  xml.GetPropertyValue(wxT("size")).ToLong( &size ); // number of numeric variables
  for( int i=0; i<static_cast<int>(size); ++i )
  {
    i==0 ? xml.GetFirstChild() : xml.GetNextSibling();
    if( xml.GetName() == wxT("scalar") )
    {
      wxString name( xml.GetPropertyValue(wxT("name")) );
      wxString type( xml.GetPropertyValue(wxT("type")) );
      xml.GetFirstChild(); // get <data>
      double value;
      xml.GetTextValue().ToDouble( &value );
      xml.GetNextSibling(); // get <history>
      long int nhist;
      xml.GetPropertyValue(wxT("size")).ToLong(&nhist); // number of history strings
      std::deque<wxString> history;
      for( int j=0; j<static_cast<int>(nhist); ++j )
      {
        j==0 ? xml.GetFirstChild() : xml.GetNextSibling(); // get <string>
        history.push_front( xml.GetTextValue() );
      }
      xml.SetBackToParent();  // set current node back to <history>
      NumericVariable *nv = NumericVariable::PutVariable( name, value, wxString(wxT("")), false );
      nv->SetHistory( history );
      xml.GetNextSibling(); // get <origin>
      wxString origin( xml.GetTextValue() );
      if( !origin.empty() )nv->SetOrigin( origin );
      if( type == wxT("FIT") )nv->GetData().SetFit();
      xml.SetBackToParent();  // set current node back to <scalar>
    }
    else if( xml.GetName() == wxT("vector") )
    {
      wxString name( xml.GetPropertyValue(wxT("name")) );
      wxString orderS( xml.GetPropertyValue(wxT("order")) );
      int order = 0;
      if( orderS == wxT("ASCENDING") )order = 1;
      else if( orderS == wxT("DESCENDING") )order = 2;
      xml.GetFirstChild(); // get <data>
      long int size;
      xml.GetPropertyValue(wxT("size")).ToLong(&size);
      std::vector<double> data( GetDoubleData(xml.GetTextValue(),static_cast<int>(size)) );
      xml.GetNextSibling(); // get <history>
      xml.GetPropertyValue(wxT("size")).ToLong(&size); // number of history strings
      std::deque<wxString> history;
      for( int j=0; j<static_cast<int>(size); ++j )
      {
        j==0 ? xml.GetFirstChild() : xml.GetNextSibling(); // get <string>
        history.push_front( xml.GetTextValue() );
      }
      xml.SetBackToParent();  // set current node back to <history>
      NumericVariable *nv = NumericVariable::PutVariable( name, data, order, wxString(wxT("")), false );
      nv->SetHistory( history );
      xml.GetNextSibling(); // get <origin>
      wxString origin( xml.GetTextValue() );
      if( !origin.empty() )nv->SetOrigin( origin );
      xml.SetBackToParent(); // set current node back to <vector>
    }
    else if( xml.GetName() == wxT("matrix") )
    {
      wxString name( xml.GetPropertyValue(wxT("name")) );
      long int rows, columns, size;
      xml.GetPropertyValue(wxT("rows")).ToLong( &rows );
      xml.GetPropertyValue(wxT("columns")).ToLong( &columns );
      xml.GetFirstChild(); // get <data>
      xml.GetPropertyValue(wxT("size")).ToLong( &size );
      std::vector<double> data( GetDoubleData(xml.GetTextValue(),static_cast<int>(size)) );
      xml.GetNextSibling(); // get <history>
      xml.GetPropertyValue(wxT("size")).ToLong( &size ); // number of history strings
      std::deque<wxString> history;
      for( int j=0; j<static_cast<int>(size); ++j )
      {
        j==0 ? xml.GetFirstChild() : xml.GetNextSibling(); // get <string>
        history.push_front( xml.GetTextValue() );
      }
      xml.SetBackToParent(); // set current node back to history
      NumericVariable *nv = NumericVariable::PutVariable( name, data, static_cast<int>(rows),
                                                          static_cast<int>(columns), wxString(wxT("")),
                                                          false );
      nv->SetHistory( history );
      xml.GetNextSibling(); // get <origin>
      wxString origin( xml.GetTextValue() );
      if( !origin.empty() )nv->SetOrigin( origin );
      xml.SetBackToParent(); // set current node back to <matrix>
    }
    /*
    else if( xml.GetName() == wxT("tensor") )
    {
      wxString name( xml.GetPropertyValue(wxT("name")) );
      long int rows, columns, planes, size;
      xml.GetPropertyValue(wxT("rows")).ToLong( &rows );
      xml.GetPropertyValue(wxT("columns")).ToLong( &columns );
      xml.GetPropertyValue(wxT("planes")).ToLong( &planes );
      xml.GetFirstChild(); // get <data>
      xml.GetPropertyValue(wxT("size")).ToLong( &size );
      std::vector<double> data( GetDoubleData(xml.GetTextValue(),static_cast<int>(size)) );
      xml.GetNextSibling(); // get <history>
      xml.GetPropertyValue(wxT("size")).ToLong( &size ); // number of history strings
      std::deque<wxString> history;
      for( int j=0; j<static_cast<int>(size); ++j )
      {
        j==0 ? xml.GetFirstChild() : xml.GetNextSibling(); // get <string>
        history.push_front( xml.GetTextValue() );
      }
      xml.SetBackToParent(); // set current node back to <history>
      NumericVariable *nv =
      NumericVariable::PutVariable( name, data, static_cast<int>(rows),
      static_cast<int>(columns),
      static_cast<int>(planes), wxT(""), false );
      nv->SetHistory( history );
      xml.GetNextSibling(); // get <origin>
      wxString origin( xml.GetTextValue() );
      if( !origin.empty() )nv->SetOrigin( origin );
      xml.SetBackToParent(); // set current node back to <tensor>
    }
    */
  }
  if( size > 0L )xml.SetBackToParent(); // set node to <numericvariables>
  //
  // next get the <textvariables> node
  //
  if( !xml.GetNextSibling() )
    throw std::runtime_error(
      std::string( (wxString()<<wxT("expecting node: <textvariables>, file: ")<<file).mb_str(wxConvUTF8) ) );

  if( xml.GetName() != wxT("textvariables") )
    throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("textvariables"),file) );
  
  xml.GetPropertyValue(wxT("size")).ToLong( &size ); // number of text variables
  for( int i=0; i<static_cast<int>(size); ++i )
  {
    i==0 ? xml.GetFirstChild() : xml.GetNextSibling();
    if( xml.GetName() == wxT("textscalar") )
    {
      wxString name( xml.GetPropertyValue(wxT("name")) );
      xml.GetFirstChild(); // get <string>
      wxString value = xml.GetTextValue();
      xml.GetNextSibling(); // get <history>
      long int tmp;
      xml.GetPropertyValue(wxT("size")).ToLong( &tmp ); // number of history strings
      std::deque<wxString> history;
      for( int j=0; j<static_cast<int>(tmp); ++j )
      {
        j==0 ? xml.GetFirstChild() : xml.GetNextSibling();
        history.push_front( xml.GetTextValue() );
      }
      xml.SetBackToParent(); // set current node back to <history>
      TextVariable *tv = TextVariable::PutVariable( name, value, wxString(wxT("")), false );
      tv->SetHistory( history );
      xml.GetNextSibling(); // get <origin>
      wxString origin( xml.GetTextValue() );
      if( !origin.empty() )tv->SetOrigin( origin );
      xml.SetBackToParent(); // set current node back to <textscalar>
    }
    else if( xml.GetName() == wxT("textvector") )
    {
      wxString name( xml.GetPropertyValue(wxT("name")) );
      long int tmp;
      xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
      std::vector<wxString> strings;
      for( int j=0; j<static_cast<int>(tmp); ++j )
      {
        j==0 ? xml.GetFirstChild() : xml.GetNextSibling();
        strings.push_back( xml.GetTextValue() );
      }
      xml.GetNextSibling(); // get <history>
      xml.GetPropertyValue(wxT("size")).ToLong( &tmp ); // number of history strings
      std::deque<wxString> history;
      for( int j=0; j<static_cast<int>(tmp); ++j )
      {
        j==0 ? xml.GetFirstChild() : xml.GetNextSibling();
        history.push_front( xml.GetTextValue() );
      }
      xml.SetBackToParent(); // set current node back to <history>
      TextVariable *tv = TextVariable::PutVariable( name, strings, wxString(wxT("")), false );
      tv->SetHistory( history );
      xml.GetNextSibling(); // get <origin>
      wxString origin( xml.GetTextValue() );
      if( !origin.empty() )tv->SetOrigin( origin );
      xml.SetBackToParent(); // set current node back to <textvector>
    }
  }
  if( size > 0L )xml.SetBackToParent(); // set node to <textvariables>
  //
  // next get the <colormap> node
  //
  if( !xml.GetNextSibling() )
    throw std::runtime_error(
      std::string( (wxString()<<wxT("expecting node: <colormap>, file: ")<<file).mb_str(wxConvUTF8) ) );

  if( xml.GetName() != wxT("colormap") )
    throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("colormap"),file) );
  
  wxString name( xml.GetPropertyValue(wxT("name")) );
  if( name == wxT("USERDEFINED") )
  {
    GRA_colorMap *cmap = new GRA_colorMap( wxT("USERDEFINED") );
    xml.GetPropertyValue(wxT("size")).ToLong( &size ); // number of colors
    for( int i=0; i<static_cast<int>(size); ++i )
    {
      i==0 ? xml.GetFirstChild() : xml.GetNextSibling(); // get <color> node
      long int rl, gl, bl;
      xml.GetPropertyValue(wxT("r")).ToLong( &rl );
      xml.GetPropertyValue(wxT("g")).ToLong( &gl );
      xml.GetPropertyValue(wxT("b")).ToLong( &bl );
      cmap->AddColor( new GRA_color(static_cast<int>(rl),static_cast<int>(gl),static_cast<int>(bl)) );
    }
    if( size > 0L )xml.SetBackToParent(); // set node to <colormap>
    GRA_colorControl::SetColorMapUser( cmap );
  }
  else
  {
    try
    {
      GRA_colorControl::SetColorMap( name );
    }
    catch (EGraphicsError const &e)
    {
      throw std::runtime_error( e.what() );
    }
  }
  //
  // get the graphics page(s)
  //
  int pageCounter = 0;
  bool nextPage = xml.GetNextSibling();
  while( nextPage )
  {
    if( xml.GetName() != wxT("graphicspage") )
      throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("graphicspage"),file) );
    //
    // first page already made above
    // only make new page if more than one page
    //
    if( ++pageCounter > 1 )visualizationWindow_->NewPage( pageCounter );
    long int cwl;
    xml.GetPropertyValue(wxT("currentwindow")).ToLong( &cwl );
    int windowNumber = static_cast<int>(cwl);
    bool nextWindow = xml.GetFirstChild();
    while( nextWindow )
    {
      if( xml.GetName() != wxT("graphwindow") )
        throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("graphwindow"),file) );
      
      long int tmp;
      xml.GetPropertyValue(wxT("number")).ToLong( &tmp );
      int number = static_cast<int>(tmp);
      double xLoP;
      xml.GetPropertyValue(wxT("xLoP")).ToDouble( &xLoP );
      double yLoP;
      xml.GetPropertyValue(wxT("yLoP")).ToDouble( &yLoP );
      double xHiP;
      xml.GetPropertyValue(wxT("xHiP")).ToDouble( &xHiP );
      double yHiP;
      xml.GetPropertyValue(wxT("yHiP")).ToDouble( &yHiP );
      double xPrevious;
      xml.GetPropertyValue(wxT("xPrevious")).ToDouble( &xPrevious );
      double yPrevious;
      xml.GetPropertyValue(wxT("yPrevious")).ToDouble( &yPrevious );
      GRA_window *gw = new GRA_window( number, xLoP, yLoP, xHiP, yHiP );
      visualizationWindow_->AddGraphWindow( gw );

      xml.GetFirstChild(); // get <axisc> for x-axis
      SetCharacteristics( xml, gw->GetXAxisCharacteristics(), wxT("axisc"), file );

      xml.GetNextSibling(); // get <axisc> for y-axis
      SetCharacteristics( xml, gw->GetYAxisCharacteristics(), wxT("axisc"), file );

      xml.GetNextSibling(); // get <generalc>
      SetCharacteristics( xml, gw->GetGeneralCharacteristics(), wxT("generalc"), file );

      xml.GetNextSibling(); // get <textc>
      SetCharacteristics( xml, gw->GetTextCharacteristics(), wxT("textc"), file );

      xml.GetNextSibling(); // get <graphlegendc>
      SetCharacteristics( xml, gw->GetGraphLegendCharacteristics(), wxT("graphlegendc"), file );

      xml.GetNextSibling(); // get <datacurvec>
      SetCharacteristics( xml, gw->GetDataCurveCharacteristics(), wxT("datacurvec"), file );

      xml.GetNextSibling(); // get <polarc>
      SetCharacteristics( xml, gw->GetPolarCharacteristics(), wxT("polarc"), file );

      xml.GetNextSibling(); // get <drawableobjects>
      if( xml.GetName() != wxT("drawableobjects") )
        throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("drawableobjects"),file) );
      
      wxClientDC dc( visualizationWindow_->GetPage() );
      bool nextObject = xml.GetFirstChild();
      bool drawableObjects = nextObject;
      while( nextObject )
      {
        if( xml.GetName() == wxT("point") )
        {
          GRA_point *point = GetPoint( xml );
          point->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( point );
        }
        else if( xml.GetName() == wxT("polyline") )
        {
          GRA_polyline *pline = GetPolyline( xml );
          pline->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( pline );
        }
        else if( xml.GetName() == wxT("ellipse") )
        {
          GRA_ellipse *ellipse = GetEllipse( xml );
          ellipse->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( ellipse );
        }
        else if( xml.GetName() == wxT("polygon") )
        {
          GRA_polygon *poly = GetPolygon( xml );
          poly->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( poly );
        }
        else if( xml.GetName() == wxT("multilinefigure") )
        {
          GRA_multiLineFigure *multi = GetMultiLineFigure( xml );
          multi->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( multi );
        }
        else if( xml.GetName() == wxT("plotsymbol") )
        {
          GRA_plotSymbol *ps = GetPlotSymbol( xml );
          ps->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( ps );
        }
        else if( xml.GetName() == wxT("drawabletext") )
        {
          GRA_drawableText *dt = GetDrawableText( xml );
          dt->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( dt );
        }
        else if( xml.GetName() == wxT("cartesianaxes") )
        {
          GRA_cartesianAxes *axes = GetCartesianAxes( xml, file );
          axes->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( axes );
        }
        else if( xml.GetName() == wxT("cartesiancurve") )
        {
          GRA_cartesianCurve *curve = GetCartesianCurve( xml );
          curve->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( curve );
        }
        else if( xml.GetName() == wxT("polaraxes") )
        {
          GRA_polarAxes *axes = GetPolarAxes( xml, file );
          axes->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( axes );
        }
        else if( xml.GetName() == wxT("polarcurve") )
        {
          GRA_polarCurve *curve = GetPolarCurve( xml );
          curve->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( curve );
        }
        else if( xml.GetName() == wxT("graphlegend") )
        {
          GRA_legend *gl = GetGraphLegend( xml, gw );
          gl->Draw( graphicsOutput_, dc );
          //gw->AddDrawableObject( gl );
        }
        else if( xml.GetName() == wxT("contour") )
        {
          GRA_contourLine *contour = GetContour( xml );
          contour->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( contour );
        }
        else if( xml.GetName() == wxT("boxplot") )
        {
          GRA_boxPlot *bp = GetBoxPlot( xml );
          bp->Make();
          bp->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( bp );
        }
        else if( xml.GetName() == wxT("diffusionplot") )
        {
          GRA_diffusionPlot *dp = GetDiffusionPlot( xml );
          dp->Make();
          dp->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( dp );
        }
        else if( xml.GetName() == wxT("ditheringplot") )
        {
          GRA_ditheringPlot *dp = GetDitheringPlot( xml );
          dp->Make();
          dp->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( dp );
        }
        else if( xml.GetName() == wxT("gradientplot") )
        {
          GRA_gradientPlot *gp = GetGradientPlot( xml );
          gp->Make();
          gp->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( gp );
        }
        else if( xml.GetName() == wxT("threedfigure") )
        {
        }
        else
        {
          wxString s(wxT("xml is invalid: unknown "));
          s += wxT("<")+xml.GetName()+wxT("> node encountered");
          throw std::runtime_error( std::string(s.mb_str(wxConvUTF8)) );
        }
        nextObject = xml.GetNextSibling();
      }
      visualizationWindow_->SetWindowNumber( windowNumber );
      visualizationWindow_->GetPage()->Refresh();

      if( drawableObjects )xml.SetBackToParent(); // set back to <drawableobjects>
      xml.SetBackToParent(); // set back to <graphwindow>
      nextWindow = xml.GetNextSibling(); // get next <graphwindow>
    }
    xml.SetBackToParent(); // set back to <graphicspage>
    nextPage = xml.GetNextSibling();
  }
}


std::vector<double> GetDoubleData( wxString const &sdata, int size )
{
  std::vector<double> data(size);
  int jStart = 0;
  for( int j=0; j<size; ++j )
  {
    int jEnd = sdata.find_first_of( wxT(" "), jStart );
    wxString temp( sdata.substr(jStart,jEnd-jStart) );
    temp.ToDouble( &data[j] );
    jStart = jEnd+1;
  }
  return data;
}

std::vector<int> GetIntData( wxString const &sdata, int size )
{
  std::vector<int> data(size);
  int jStart = 0;
  for( int j=0; j<size; ++j )
  {
    int jEnd = sdata.find_first_of( wxT(" "), jStart );
    long int itmp;
    wxString temp( sdata.substr(jStart,jEnd-jStart) );
    temp.ToLong( &itmp );
    data[j] = static_cast<int>( itmp );
    jStart = jEnd+1;
  }
  return data;
}

std::vector<bool> GetBoolData( wxString const &sdata, int size )
{
  std::vector<bool> data(size);
  int jStart = 0;
  for( int j=0; j<size; ++j )
  {
    int jEnd = sdata.find_first_of( wxT(' '), jStart );
    wxString temp( sdata.substr(jStart,jEnd-jStart) );
    std::istringstream iss( std::string(temp.mb_str(wxConvUTF8)) );
    bool tmp;
    iss >> tmp;
    data[j] = tmp;
    jStart = jEnd+1;
  }
  return data;
}

void SetCharacteristics( ExXML &xml, GRA_setOfCharacteristics *characteristics,
                         wxString const &type, wxString const &file )
{
  if( xml.GetName() != type )throw std::runtime_error( InvalidNodeMessage(xml.GetName(),type,file) );
  
  bool nextCharacteristic = xml.GetFirstChild();
  if( !nextCharacteristic )return;
  while( nextCharacteristic )
  {
    if( xml.GetName() != wxT("characteristic") )
      throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("characteristic"),file) );
    
    wxString name( xml.GetPropertyValue(wxT("name")) );
    wxString type( xml.GetPropertyValue(wxT("type")) );
    wxString shape( xml.GetPropertyValue(wxT("shape")) );

    //std::cout << "  characteristic name=|" << name.mb_str(wxConvUTF8) << "| type=|"
    //          << type.mb_str(wxConvUTF8) << "| shape=|" << shape.mb_str(wxConvUTF8) << "|\n";

    if( shape == wxT("VECTOR") )
    {
      xml.GetFirstChild(); // get <data>
      
      long int tmp;
      xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
      int size = static_cast<int>(tmp);
      
      wxString sdata( xml.GetTextValue() );
      if( type == wxT("SIZE") )
        static_cast<GRA_sizeCharacteristic*>(characteristics->Get(name))->SetAsPercent( GetDoubleData(sdata,size) );
      else if( type == wxT("DISTANCE") )
        static_cast<GRA_distanceCharacteristic*>(characteristics->Get(name))->SetAsPercent( GetDoubleData(sdata,size) );
      else if( type == wxT("INT") )
        static_cast<GRA_intCharacteristic*>(characteristics->Get(name))->Set( GetIntData(sdata,size) );
      else if( type == wxT("DOUBLE") )
        static_cast<GRA_doubleCharacteristic*>(characteristics->Get(name))->Set( GetDoubleData(sdata,size) );
      else if( type == wxT("BOOL") )
        static_cast<GRA_boolCharacteristic*>(characteristics->Get(name))->Set( GetBoolData(sdata,size) );
      else if( type == wxT("ANGLE") )
        static_cast<GRA_angleCharacteristic*>(characteristics->Get(name))->Set( GetDoubleData(sdata,size) );
      else if( type == wxT("FONT") )
        static_cast<GRA_fontCharacteristic*>(characteristics->Get(name))->Set( GetIntData(sdata,size) );
      else if( type == wxT("COLOR") )
        static_cast<GRA_colorCharacteristic*>(characteristics->Get(name))->Set( GetIntData(sdata,size) );

      xml.SetBackToParent(); // set back to <characteristic>
    }
    else
    {
      if( type == wxT("SIZE") )
      {
        double tmp;
        xml.GetPropertyValue(wxT("value")).ToDouble( &tmp );
        static_cast<GRA_sizeCharacteristic*>(characteristics->Get(name))->SetAsPercent( tmp );
      }
      else if( type == wxT("DISTANCE") )
      {
        double tmp;
        xml.GetPropertyValue(wxT("value")).ToDouble( &tmp );
        static_cast<GRA_distanceCharacteristic*>(characteristics->Get(name))->SetAsPercent( tmp );
      }
      else if( type == wxT("INT") )
      {
        long int tmp;
        xml.GetPropertyValue(wxT("value")).ToLong( &tmp );
        static_cast<GRA_intCharacteristic*>(characteristics->Get(name))->Set( static_cast<int>(tmp) );
      }
      else if( type == wxT("DOUBLE") )
      {
        double tmp;
        xml.GetPropertyValue(wxT("value")).ToDouble( &tmp );
        static_cast<GRA_doubleCharacteristic*>(characteristics->Get(name))->Set( tmp );
      }
      else if( type == wxT("BOOL") )
      {
        long int tmp;
        xml.GetPropertyValue(wxT("value")).ToLong( &tmp );
        static_cast<GRA_boolCharacteristic*>(characteristics->Get(name))->Set( (tmp!=0L) );
      }
      else if( type == wxT("ANGLE") )
      {
        double tmp;
        xml.GetPropertyValue(wxT("value")).ToDouble( &tmp );
        static_cast<GRA_angleCharacteristic*>(characteristics->Get(name))->Set( tmp );
      }
      else if( type == wxT("FONT") )
      {
        long int tmp;
        xml.GetPropertyValue(wxT("value")).ToLong( &tmp );
        static_cast<GRA_fontCharacteristic*>(characteristics->Get(name))->
            Set( GRA_fontControl::GetFont(static_cast<int>(tmp)) );
      }    
      else if( type == wxT("COLOR") )
      {
        long int tmp;
        xml.GetPropertyValue(wxT("value")).ToLong( &tmp );
        static_cast<GRA_colorCharacteristic*>(characteristics->Get(name))->
            Set( GRA_colorControl::GetColor(static_cast<int>(tmp)) );
      }
    }
    nextCharacteristic = xml.GetNextSibling();
  }
  xml.SetBackToParent();
}

std::string InvalidNodeMessage( wxString const &invalid, wxString const &valid, wxString const &file )
{

  //std::cout << "InvalidNodeMessage: "
  //          << (wxString() << wxT("expected node: ") << valid << wxT(", found: <")
  //              << invalid << wxT(">, file: ") << file).mb_str(wxConvUTF8) << "\n";

  return std::string( (wxString() << wxT("expected node: <") << valid << wxT(">, found: <")
                                  << invalid << wxT(">, file: ") << file).mb_str(wxConvUTF8) );
}

GRA_point *GetPoint( ExXML &xml )
{
  double x;
  xml.GetPropertyValue(wxT("x")).ToDouble( &x );
  double y;
  xml.GetPropertyValue(wxT("y")).ToDouble( &y );
  long int c;
  xml.GetPropertyValue(wxT("color")).ToLong( &c );
  return new GRA_point( x, y, GRA_colorControl::GetColor(static_cast<int>(c)) );
}

GRA_polyline *GetPolyline( ExXML &xml )
{
  long int tmp;
  xml.GetPropertyValue(wxT("linewidth")).ToLong( &tmp );
  int lw = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linetype")).ToLong( &tmp );
  int lt = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linecolor")).ToLong( &tmp );
  int lc = static_cast<int>(tmp);

  xml.GetFirstChild(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  wxString sdata( xml.GetTextValue() );
  std::vector<double> x(size), y(size);
  std::vector<int> pen(size);
  int jStart = 0;
  for( int j=0; j<size; ++j )
  {
    int jEnd = sdata.find_first_of( wxT(" "), jStart );
    wxString temp( sdata.substr(jStart,jEnd-jStart) );
    temp.ToDouble( &x[j] );
    jStart = jEnd+1;
    jEnd = sdata.find_first_of( wxT(" "), jStart );
    temp = sdata.substr(jStart,jEnd-jStart);
    temp.ToDouble( &y[j] );
    jStart = jEnd+1;
    jEnd = sdata.find_first_of( wxT(" "), jStart );
    temp = sdata.substr(jStart,jEnd-jStart);
    temp.ToLong( &tmp );
    pen[j] = static_cast<int>(tmp);
    jStart = jEnd+1;
  }
  xml.SetBackToParent();

  return new GRA_polyline( x, y, pen, lw, lt, GRA_colorControl::GetColor(lc) );
}

GRA_multiLineFigure *GetMultiLineFigure( ExXML &xml )
{
  long int tmp;
  xml.GetPropertyValue(wxT("linewidth")).ToLong( &tmp );
  int lw = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linetype")).ToLong( &tmp );
  int lt = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linecolor")).ToLong( &tmp );
  int lc = static_cast<int>(tmp);

  xml.GetFirstChild(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  wxString sdata( xml.GetTextValue() );
  std::vector<double> x(size), y(size);
  std::vector<int> pen(size);
  int jStart = 0;
  for( int j=0; j<size; ++j )
  {
    int jEnd = sdata.find_first_of( wxT(" "), jStart );
    wxString temp( sdata.substr(jStart,jEnd-jStart) );
    temp.ToDouble( &x[j] );
    jStart = jEnd+1;
    jEnd = sdata.find_first_of( wxT(" "), jStart );
    temp = sdata.substr(jStart,jEnd-jStart);
    temp.ToDouble( &y[j] );
    jStart = jEnd+1;
    jEnd = sdata.find_first_of( wxT(" "), jStart );
    temp = sdata.substr(jStart,jEnd-jStart);
    temp.ToLong( &tmp );
    pen[j] = static_cast<int>(tmp);
    jStart = jEnd+1;
  }
  xml.SetBackToParent();

  return new GRA_multiLineFigure( x, y, pen, GRA_colorControl::GetColor(lc), lw, lt );
}

GRA_errorBar *GetErrorBar( ExXML &xml )
{
  double x;
  xml.GetPropertyValue(wxT("x")).ToDouble( &x );
  double y;
  xml.GetPropertyValue(wxT("y")).ToDouble( &y );
  long int tmp;
  xml.GetPropertyValue(wxT("linewidth")).ToLong( &tmp );
  int lw = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linecolor")).ToLong( &tmp );
  int lc = static_cast<int>(tmp);
  double bottom;
  xml.GetPropertyValue(wxT("bottom")).ToDouble( &bottom );
  double top;
  xml.GetPropertyValue(wxT("top")).ToDouble( &top );
  double footsize;
  xml.GetPropertyValue(wxT("footsize")).ToDouble( &footsize );
  xml.GetPropertyValue(wxT("vertical")).ToLong( &tmp );
  bool vertical = static_cast<bool>(tmp);
  return new GRA_errorBar( x, y, bottom, top, vertical, footsize,
                           GRA_colorControl::GetColor(lc), lw );
}

GRA_polygon *GetPolygon( ExXML &xml )
{
  long int tmp;
  xml.GetPropertyValue(wxT("linewidth")).ToLong( &tmp );
  int lw = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linetype")).ToLong( &tmp );
  int lt = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linecolor")).ToLong( &tmp );
  int lc = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("fillcolor")).ToLong( &tmp );
  int fc = static_cast<int>(tmp);

  xml.GetFirstChild(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<double> x(size), y(size), data(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    x[j] = data[2*j];
    y[j] = data[2*j+1];
  }
  xml.SetBackToParent();

  return new GRA_polygon( x, y, GRA_colorControl::GetColor(lc),
                          GRA_colorControl::GetColor(fc), lw, lt );
}

GRA_ellipse *GetEllipse( ExXML &xml )
{
  double xmin;
  xml.GetPropertyValue(wxT("xmin")).ToDouble( &xmin );
  double ymin;
  xml.GetPropertyValue(wxT("ymin")).ToDouble( &ymin );
  double xmax;
  xml.GetPropertyValue(wxT("xmax")).ToDouble( &xmax );
  double ymax;
  xml.GetPropertyValue(wxT("ymax")).ToDouble( &ymax );
  long int tmp;
  xml.GetPropertyValue(wxT("circle")).ToLong( &tmp );
  bool circle = static_cast<bool>(tmp);  
  xml.GetPropertyValue(wxT("linecolor")).ToLong( &tmp );
  int lc = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("fillcolor")).ToLong( &tmp );
  int fc = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linewidth")).ToLong( &tmp );
  int lw = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linetype")).ToLong( &tmp );
  int lt = static_cast<int>(tmp);
  return new GRA_ellipse( xmin, ymin, xmax, ymax, circle,
                          GRA_colorControl::GetColor(lc), GRA_colorControl::GetColor(fc),
                          lw, lt );
}

GRA_plotSymbol *GetPlotSymbol( ExXML &xml )
{
  double x;
  xml.GetPropertyValue(wxT("x")).ToDouble( &x );
  double y;
  xml.GetPropertyValue(wxT("y")).ToDouble( &y );
  long int tmp;
  xml.GetPropertyValue(wxT("shape")).ToLong( &tmp );
  int code = static_cast<int>(tmp);
  double angle;
  xml.GetPropertyValue(wxT("angle")).ToDouble( &angle );
  double size;
  xml.GetPropertyValue(wxT("size")).ToDouble( &size );
  xml.GetPropertyValue(wxT("color")).ToLong( &tmp );
  int color = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linewidth")).ToLong( &tmp );
  int lw = static_cast<int>(tmp);
  GRA_plotSymbol *ps = new GRA_plotSymbol( code, size, angle,
    GRA_colorControl::GetColor(color), lw );
  ps->SetLocation( x, y );
  return ps;
}

GRA_drawableText *GetDrawableText( ExXML &xml )
{
  double x;
  xml.GetPropertyValue(wxT("x")).ToDouble( &x );
  double y;
  xml.GetPropertyValue(wxT("y")).ToDouble( &y );
  double height;
  xml.GetPropertyValue(wxT("height")).ToDouble( &height );
  double angle;
  xml.GetPropertyValue(wxT("angle")).ToDouble( &angle );
  long int tmp;
  xml.GetPropertyValue(wxT("alignment")).ToLong( &tmp );
  int alignment = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("color")).ToLong( &tmp );
  int color = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("font")).ToLong( &tmp );
  int font = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("graphunits")).ToLong( &tmp );
  bool graphunits = static_cast<bool>(tmp);

  xml.GetFirstChild(); // get <string>
  GRA_drawableText *dt = new GRA_drawableText( xml.GetTextValue(), height,
                                               angle, x, y, alignment, GRA_fontControl::GetFont(font),
                                               GRA_colorControl::GetColor(color) );
  dt->Parse();
  xml.SetBackToParent();

  return dt;
}

GRA_legend *GetGraphLegend( ExXML &xml, GRA_window *gw )
{
  long int tmp;
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("frameon")).ToLong( &tmp );
  bool frameOn = (tmp!=0L);
  xml.GetPropertyValue(wxT("titleon")).ToLong( &tmp );
  bool titleOn = (tmp!=0L);
  double xlo, ylo, xhi, yhi;
  xml.GetPropertyValue(wxT("xlo")).ToDouble( &xlo );
  xml.GetPropertyValue(wxT("ylo")).ToDouble( &ylo );
  xml.GetPropertyValue(wxT("xhi")).ToDouble( &xhi );
  xml.GetPropertyValue(wxT("yhi")).ToDouble( &yhi );
  
  GRA_setOfCharacteristics *legendC = gw->GetGraphLegendCharacteristics();
  static_cast<GRA_distanceCharacteristic*>(legendC->Get(wxT("FRAMEXLO")))->SetAsWorld( xlo );
  static_cast<GRA_distanceCharacteristic*>(legendC->Get(wxT("FRAMEYLO")))->SetAsWorld( ylo );
  static_cast<GRA_distanceCharacteristic*>(legendC->Get(wxT("FRAMEXHI")))->SetAsWorld( xhi );
  static_cast<GRA_distanceCharacteristic*>(legendC->Get(wxT("FRAMEYHI")))->SetAsWorld( yhi );
  
  static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("FRAMEON")))->Set( frameOn );
  static_cast<GRA_boolCharacteristic*>(legendC->Get(wxT("TITLEON")))->Set( titleOn );
  
  GRA_legend *gl = gw->GetGraphLegend();
  double xstart = gl->GetLineStart();
  double xend = gl->GetLineEnd();
  double xlabel = gl->GetXLabel();
  
  for( int j=0; j<size; ++j )
  {
    j==0 ? xml.GetFirstChild() : xml.GetNextSibling(); // get <legendentry>
    long int tmp;
    xml.GetPropertyValue(wxT("nsymbols")).ToLong( &tmp );
    int nSymbols = static_cast<int>(tmp);
    xml.GetPropertyValue(wxT("line")).ToLong( &tmp );
    bool line = (tmp==1L);
    xml.GetPropertyValue(wxT("linetype")).ToLong( &tmp );
    int lt = static_cast<int>(tmp);
    xml.GetPropertyValue(wxT("linewidth")).ToLong( &tmp );
    int lw = static_cast<int>(tmp);
    xml.GetPropertyValue(wxT("linecolor")).ToLong( &tmp );
    int lc = static_cast<int>(tmp);
    static_cast<GRA_intCharacteristic*>(legendC->Get(wxT("SYMBOLS")))->Set( nSymbols );
    GRA_color *lineColor = GRA_colorControl::GetColor( lc );
    
    xml.GetFirstChild(); // get <drawabletext>
    GRA_drawableText *labelDT = GetDrawableText( xml );
    wxString label( labelDT->GetString() );
    double labelHeight = labelDT->GetHeight();
    GRA_font *labelFont = labelDT->GetFont();
    GRA_color *labelColor = labelDT->GetColor();
    
    xml.GetNextSibling(); // get <plotsymbol>
    GRA_plotSymbol *symbol = GetPlotSymbol( xml );
    int symCode = symbol->GetCode();
    double symSize = symbol->GetSize();
    double symAngle = symbol->GetAngle();
    GRA_color *symColor = symbol->GetColor();
    int symLineWidth = symbol->GetLineWidth();
    
    GRA_legendEntry *entry = 0;
    try
    {
      entry = new GRA_legendEntry( gl, label, labelHeight, labelFont, labelColor,
                                   nSymbols, symCode, symSize, symAngle, symColor,
                                   symLineWidth, line, lt, lw, lineColor );
    }
    catch (EGraphicsError const &e)
    {
      delete entry;
      throw std::runtime_error( e.what() );
    }
    gl->AddEntry( entry );
    xml.SetBackToParent();  // set back to <legendentry>
  }
  xml.GetNextSibling(); // get <polygon>  frame
  gl->SetFrame( static_cast<GRA_rectangle*>(GetPolygon(xml)) );
  xml.GetNextSibling(); // get <drawabletext>  title
  gl->SetTitle( GetDrawableText(xml) );
  xml.SetBackToParent();
  
  return gl;
}

GRA_cartesianAxes *GetCartesianAxes( ExXML &xml, wxString const &file )
{
  long int tmp;
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);

  xml.GetFirstChild(); // get x <axis>
  GRA_axis *xaxis = GetAxis( xml, 'X', file );

  xml.GetNextSibling(); // get y <axis>
  GRA_axis *yaxis = GetAxis( xml, 'Y', file );
  GRA_axis *boxX =0, *boxY =0;
  if( size >= 3 )
  {
    xml.GetNextSibling(); // get box x-axis
    boxX = GetAxis( xml, 'X', file );
    if( size == 4 )
    {
      xml.GetNextSibling(); // get box y-axis
      boxY = GetAxis( xml, 'Y', file );
    }
  }
  xml.SetBackToParent();

  GRA_cartesianAxes *axes = new GRA_cartesianAxes();
  axes->SetAxes( xaxis, yaxis, boxX, boxY );
  return axes;
}

GRA_polarAxes *GetPolarAxes( ExXML &xml, wxString const &file )
{
  xml.GetFirstChild(); // get <axis>
  GRA_axis *axis = GetAxis( xml, 'P', file );
  GRA_polarAxes *axes = new GRA_polarAxes();
  axes->SetAxis( axis );
  return axes;
}

GRA_axis *GetAxis( ExXML &xml, wxChar const type, wxString const &file )
{
  double xOrigin;
  xml.GetPropertyValue(wxT("xorigin")).ToDouble( &xOrigin );
  double yOrigin;
  xml.GetPropertyValue(wxT("yorigin")).ToDouble( &yOrigin );
  double length;
  xml.GetPropertyValue(wxT("length")).ToDouble( &length );

  xml.GetFirstChild(); // get <ticcoordinates>
  long int tmp;
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<double> xtics(size), ytics(size), data(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    xtics[j] = data[2*j];
    ytics[j] = data[2*j+1];
  }

  xml.GetNextSibling(); // get <axisc>
  //
  GRA_window *gw = visualizationWindow_->GetGraphWindow();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *polarC = gw->GetPolarCharacteristics();
  GRA_setOfCharacteristics axisc;
  switch (type)
  {
    case 'X':
    {
      axisc = *xAxisC;
      break;
    }
    case 'Y':
    {
      axisc = *yAxisC;
      break;
    }
    case 'P':
    {
      axisc = *polarC;
      break;
    }
  }
  SetCharacteristics( xml, &axisc, wxT("axisc"), file );

  xml.GetNextSibling(); // get <polylines>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<GRA_polyline*> polylines(size);
  for( int j=0; j<size; ++j )
  {
    j==0 ? xml.GetFirstChild() : xml.GetNextSibling(); // get <polyline>
    polylines[j] = GetPolyline(xml);
    if( j == size-1 )xml.SetBackToParent();
  }

  xml.GetNextSibling(); // get <drawabletexts>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<GRA_drawableText*> textVec(size);
  for( int j=0; j<size; ++j )
  {
    j==0 ? xml.GetFirstChild() : xml.GetNextSibling(); // get <drawabletext>
    textVec[j] = GetDrawableText(xml);
    if( j == size-1 )xml.SetBackToParent();
  }
  xml.SetBackToParent();

  GRA_axis *axis = new GRA_axis( xOrigin, yOrigin, length, &axisc );
  axis->SetTicCoordinates( xtics, ytics );
  axis->SetPolylines( polylines );
  axis->SetDrawableText( textVec );
  return axis;
}

GRA_cartesianCurve *GetCartesianCurve( ExXML &xml )
{
  long int tmp;
  xml.GetPropertyValue(wxT("histype")).ToLong( &tmp );
  int histype = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linetype")).ToLong( &tmp );
  int linetype = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linewidth")).ToLong( &tmp );
  int linewidth = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("smooth")).ToLong( &tmp );
  bool smooth = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("linecolor")).ToLong( &tmp );
  GRA_color *linecolor = GRA_colorControl::GetColor(static_cast<int>(tmp));
  xml.GetPropertyValue(wxT("areafillcolor")).ToLong( &tmp );
  int temp = static_cast<int>(tmp);
  GRA_color *areafillcolor = 0;
  if( temp != 0 )areafillcolor = GRA_colorControl::GetColor(temp);
  double xlaxis;
  xml.GetPropertyValue(wxT("xlaxis")).ToDouble( &xlaxis );
  double ylaxis;
  xml.GetPropertyValue(wxT("ylaxis")).ToDouble( &ylaxis );
  double xuaxis;
  xml.GetPropertyValue(wxT("xuaxis")).ToDouble( &xuaxis );
  double yuaxis;
  xml.GetPropertyValue(wxT("yuaxis")).ToDouble( &yuaxis );
  double xmin;
  xml.GetPropertyValue(wxT("xmin")).ToDouble( &xmin );
  double ymin;
  xml.GetPropertyValue(wxT("ymin")).ToDouble( &ymin );
  double xmax;
  xml.GetPropertyValue(wxT("xmax")).ToDouble( &xmax );
  double ymax;
  xml.GetPropertyValue(wxT("ymax")).ToDouble( &ymax );

  xml.GetFirstChild(); // get <areafillcolors>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<GRA_color*> areaFillColors(size);
  std::vector<int> ivec( GetIntData(xml.GetTextValue(),size) );
  for( int j=0; j<size; ++j )areaFillColors[j] = GRA_colorControl::GetColor(ivec[j]);

  xml.GetNextSibling(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> x(size), y(size), data(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    x[j] = data[2*j];
    y[j] = data[2*j+1];
  }

  xml.GetNextSibling(); // get <xe1>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> xe1( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <xe2>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> xe2( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <ye1>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> ye1( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <ye2>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> ye2( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <xycurve>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> xcurve(size), ycurve(size), data2(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    xcurve[j] = data2[2*j];
    ycurve[j] = data2[2*j+1];
  }

  xml.GetNextSibling(); // get <pen>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<int> pen( GetIntData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <plotsymbols>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<GRA_plotSymbol*> symbols(size);
  for( int j=0; j<size; ++j )
  {
    j==0 ? xml.GetFirstChild() : xml.GetNextSibling(); // get <plotsymbol>
    symbols[j] = GetPlotSymbol( xml );
  }
  if( size > 0 )xml.SetBackToParent();

  xml.GetNextSibling(); // get <errorbars>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<GRA_errorBar*> errorbars(size);
  for( int j=0; j<size; ++j )
  {
    j==0 ? xml.GetFirstChild() : xml.GetNextSibling();
    errorbars[j] = GetErrorBar( xml );
  }
  if( size > 0 )xml.SetBackToParent();

  xml.SetBackToParent();

  GRA_cartesianCurve *curve = new GRA_cartesianCurve( x, y, xe1, ye1, xe2, ye2, smooth );
  curve->SetValues( histype, linetype, linewidth, linecolor, areafillcolor, xlaxis, ylaxis,
                    xuaxis, yuaxis, xmin, ymin, xmax, ymax, areaFillColors, xcurve, ycurve,
                    pen, symbols, errorbars );
  return curve;
}

GRA_polarCurve *GetPolarCurve( ExXML &xml )
{
  long int tmp;
  xml.GetPropertyValue(wxT("histype")).ToLong( &tmp );
  int histype = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linetype")).ToLong( &tmp );
  int linetype = static_cast<int>(tmp);
  xml.GetPropertyValue(wxT("linewidth")).ToLong( &tmp );
  int linewidth = static_cast<int>(tmp);
  double xmax;
  xml.GetPropertyValue(wxT("xmax")).ToDouble( &xmax );
  xml.GetPropertyValue(wxT("linecolor")).ToLong( &tmp );
  GRA_color *linecolor = GRA_colorControl::GetColor(static_cast<int>(tmp));
  xml.GetPropertyValue(wxT("areafillcolor")).ToLong( &tmp );
  int temp = static_cast<int>(tmp);
  GRA_color *areafillcolor = 0;
  if( temp != 0 )areafillcolor = GRA_colorControl::GetColor(temp);

  xml.GetFirstChild(); // get <areafillcolors>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<GRA_color*> areaFillColors(size);
  std::vector<int> ivec( GetIntData(xml.GetTextValue(),size) );
  for( int j=0; j<size; ++j )areaFillColors[j] = GRA_colorControl::GetColor(ivec[j]);

  xml.GetNextSibling(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> x(size), y(size), data(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    x[j] = data[2*j];
    y[j] = data[2*j+1];
  }

  xml.GetNextSibling(); // get <xycurve>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> xcurve(size), ycurve(size), data2(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    xcurve[j] = data2[2*j];
    ycurve[j] = data2[2*j+1];
  }

  xml.GetNextSibling(); // get <pen>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<int> pen( GetIntData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <plotsymbols>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<GRA_plotSymbol*> symbols(size);
  for( int j=0; j<size; ++j )
  {
    j==0 ? xml.GetFirstChild() : xml.GetNextSibling(); // get <plotsymbol>
    symbols[j] = GetPlotSymbol( xml );
  }
  if( size > 0 )xml.SetBackToParent();

  xml.SetBackToParent();

  GRA_polarCurve *curve = new GRA_polarCurve( x, y );
  curve->SetValues( histype, linetype, linewidth, xmax, linecolor, areafillcolor,
                    areaFillColors, xcurve, ycurve, pen, symbols );
  return curve;
}

GRA_contourLine *GetContour( ExXML &xml )
{
  /*
  long int tmp;
  xml.GetPropertyValue(wxT("color")).ToLong( &tmp );
  int color = static_cast<int>(tmp);

  xml.GetFirstChild(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  wxString sdata( xml.GetTextValue() );
  std::vector<double> x(size), y(size);
  std::vector<bool> connect(size);
  int jStart = 0;
  for( int j=0; j<size; ++j )
  {
    int jEnd = sdata.find_first_of( wxT(" "), jStart );
    wxString temp( sdata.substr(jStart,jEnd-jStart) );
    temp.ToDouble( &x[j] );
    jStart = jEnd+1;
    jEnd = sdata.find_first_of( wxT(" "), jStart );
    temp = sdata.substr(jStart,jEnd-jStart);
    temp.ToDouble( &y[j] );
    jStart = jEnd+1;
    jEnd = sdata.find_first_of( wxT(" "), jStart );
    temp = sdata.substr(jStart,jEnd-jStart);
    long int tmp;
    temp.ToLong( &tmp );
    connect[j] = static_cast<bool>(tmp);
    jStart = jEnd+1;
  }

  xml.GetNextSibling(); // get <drawabletexts>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<GRA_drawableText*> textVec;
  for( int j=0; j<size; ++j )
  {
    j==0 ? xml.GetFirstChild() : xml.GetNextSibling(); // get <drawabletext>
    textVec.push_back( GetDrawableText(xml) );
    if( j == size-1 )xml.SetBackToParent();
  }
  xml.SetBackToParent();

  GRA_contour *contour = new GRA_contour();
  contour->SetColor( GRA_colorControl::GetColor(color) );
  contour->SetCoords( x, y, connect );
  contour->SetDrawableText( textVec );
  return contour;
  */
  GRA_contourLine *contour = 0;
  return contour;
}

GRA_boxPlot *GetBoxPlot( ExXML &xml )
{
  long int tmp;
  xml.GetPropertyValue(wxT("nrow")).ToLong( &tmp );
  int nrow = static_cast<int>(tmp);
  double fmin;
  xml.GetPropertyValue(wxT("fmin")).ToDouble( &fmin );
  double fmax;
  xml.GetPropertyValue(wxT("fmax")).ToDouble( &fmax );
  double gmin;
  xml.GetPropertyValue(wxT("gmin")).ToDouble( &gmin );
  double gmax;
  xml.GetPropertyValue(wxT("gmax")).ToDouble( &gmax );
  xml.GetPropertyValue(wxT("xprofile")).ToLong( &tmp );
  bool xprofile = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("yprofile")).ToLong( &tmp );
  bool yprofile = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("border")).ToLong( &tmp );
  bool border = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("zoom")).ToLong( &tmp );
  bool zoom = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("axes")).ToLong( &tmp );
  bool axes = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("reset")).ToLong( &tmp );
  bool reset = static_cast<bool>(tmp);
  double bscale;
  xml.GetPropertyValue(wxT("bscale")).ToDouble( &bscale );

  xml.GetFirstChild(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<double> x( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> y( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> z( GetDoubleData(xml.GetTextValue(),size) );
  xml.SetBackToParent();

  return new GRA_boxPlot( x, y, z, nrow, fmin, fmax, gmin, gmax, xprofile, yprofile,
                          border, zoom, axes, reset, bscale );
}

GRA_diffusionPlot *GetDiffusionPlot( ExXML &xml )
{
  long int tmp;
  xml.GetPropertyValue(wxT("nrow")).ToLong( &tmp );
  int nrow = static_cast<int>(tmp);
  double fmin;
  xml.GetPropertyValue(wxT("fmin")).ToDouble( &fmin );
  double fmax;
  xml.GetPropertyValue(wxT("fmax")).ToDouble( &fmax );
  double gmin;
  xml.GetPropertyValue(wxT("gmin")).ToDouble( &gmin );
  double gmax;
  xml.GetPropertyValue(wxT("gmax")).ToDouble( &gmax );
  xml.GetPropertyValue(wxT("xprofile")).ToLong( &tmp );
  bool xprofile = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("yprofile")).ToLong( &tmp );
  bool yprofile = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("border")).ToLong( &tmp );
  bool border = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("zoom")).ToLong( &tmp );
  bool zoom = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("axes")).ToLong( &tmp );
  bool axes = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("reset")).ToLong( &tmp );
  bool reset = static_cast<bool>(tmp);

  xml.GetFirstChild(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<double> x( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> y( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> z( GetDoubleData(xml.GetTextValue(),size) );
  xml.SetBackToParent();

  return new GRA_diffusionPlot( x, y, z, nrow, fmin, fmax, gmin, gmax, xprofile, yprofile,
                                border, zoom, axes, reset );
}

GRA_ditheringPlot *GetDitheringPlot( ExXML &xml )
{
  long int tmp;
  xml.GetPropertyValue(wxT("nrow")).ToLong( &tmp );
  int nrow = static_cast<int>(tmp);
  double fmin;
  xml.GetPropertyValue(wxT("fmin")).ToDouble( &fmin );
  double fmax;
  xml.GetPropertyValue(wxT("fmax")).ToDouble( &fmax );
  double gmin;
  xml.GetPropertyValue(wxT("gmin")).ToDouble( &gmin );
  double gmax;
  xml.GetPropertyValue(wxT("gmax")).ToDouble( &gmax );
  xml.GetPropertyValue(wxT("xprofile")).ToLong( &tmp );
  bool xprofile = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("yprofile")).ToLong( &tmp );
  bool yprofile = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("border")).ToLong( &tmp );
  bool border = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("zoom")).ToLong( &tmp );
  bool zoom = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("axes")).ToLong( &tmp );
  bool axes = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("reset")).ToLong( &tmp );
  bool reset = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("legend")).ToLong( &tmp );
  bool legend = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("equalspaced")).ToLong( &tmp );
  bool equalspaced = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("areas")).ToLong( &tmp );
  bool areas = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("volumes")).ToLong( &tmp );
  bool volumes = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("lines")).ToLong( &tmp );
  bool lines = static_cast<bool>(tmp);

  xml.GetFirstChild(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<int> dither( GetIntData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> contourLevels( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> x( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> y( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> z( GetDoubleData(xml.GetTextValue(),size) );
  xml.SetBackToParent();

  return new GRA_ditheringPlot( x, y, z, nrow, fmin, fmax, gmin, gmax, xprofile, yprofile,
                                border, zoom, axes, reset, legend, equalspaced, areas,
                                volumes, lines, dither, contourLevels );
}

GRA_gradientPlot *GetGradientPlot( ExXML &xml )
{
  long int tmp;
  xml.GetPropertyValue(wxT("nrow")).ToLong( &tmp );
  int nrow = static_cast<int>(tmp);
  double fmin;
  xml.GetPropertyValue(wxT("fmin")).ToDouble( &fmin );
  double fmax;
  xml.GetPropertyValue(wxT("fmax")).ToDouble( &fmax );
  double gmin;
  xml.GetPropertyValue(wxT("gmin")).ToDouble( &gmin );
  double gmax;
  xml.GetPropertyValue(wxT("gmax")).ToDouble( &gmax );
  xml.GetPropertyValue(wxT("xprofile")).ToLong( &tmp );
  bool xprofile = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("yprofile")).ToLong( &tmp );
  bool yprofile = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("border")).ToLong( &tmp );
  bool border = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("zoom")).ToLong( &tmp );
  bool zoom = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("axes")).ToLong( &tmp );
  bool axes = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("reset")).ToLong( &tmp );
  bool reset = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("legend")).ToLong( &tmp );
  bool legend = static_cast<bool>(tmp);
  xml.GetPropertyValue(wxT("linear")).ToLong( &tmp );
  bool linear = static_cast<bool>(tmp);

  xml.GetFirstChild(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<double> x( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> y( GetDoubleData(xml.GetTextValue(),size) );

  xml.GetNextSibling(); // get <data>
  xml.GetPropertyValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> z( GetDoubleData(xml.GetTextValue(),size) );
  xml.SetBackToParent();

  return new GRA_gradientPlot( x, y, z, nrow, fmin, fmax, gmin, gmax, xprofile, yprofile,
                               border, zoom, axes, reset, legend, linear );
}

} // end of namespace

// end of file
