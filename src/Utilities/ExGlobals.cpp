/*
Copyright (C) 2005,...,2007 Joseph L. Chuma, TRIUMF

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
#include <iostream>
#include <iomanip>
#include <sstream>

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
#include "GRA_multiLineFigure.h"
#include "GRA_ellipse.h"
#include "GRA_drawableText.h"
#include "GRA_cartesianAxes.h"
#include "GRA_cartesianCurve.h"
#include "GRA_axis.h"
#include "GRA_plotSymbol.h"
#include "GRA_legend.h"
#include "GRA_errorBar.h"
#include "GRA_contour.h"
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

void Initialize()
{
  // EX_BINDIR is defined as $(prefix)/share
  // EX_DATADIR is defined as $(prefix)
  //
  if( !wxGetEnv(wxT("EXTREMA_DIR"),&executablePath_) )
  {
    executablePath_ = wxT(EX_BINDIR);
    imagePath_ = wxString(wxT(EX_DATADIR)) + wxT("/extrema/Images");
    helpPath_ = wxString(wxT(EX_DATADIR)) + wxT("/extrema/Help");
  }
  else
  {
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
  if( wxTheApp->argc >= 2 )fname = wxTheApp->argv[1];
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
    ParseLine p( wxString(wxT("EXECUTE '"))+fname+wxT("'") );
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
  int r =0, g =0, b =0;
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
  xmaxW_ = xmaxWDef_;
  ymaxW_ = xmaxWDef_*r;
  //
  xminClip_ = xminW_ + pageBorder_;
  yminClip_ = yminW_ + pageBorder_;
  xmaxClip_ = xmaxW_ - pageBorder_;
  ymaxClip_ = ymaxW_ - pageBorder_;
  //
  xminM_ = xminMDef_;
  yminM_ = yminMDef_;
  xmaxM_ = xmaxMDef_;
  ymaxM_ = static_cast<int>(xmaxMDef_*r+0.5);
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
  try
  {
    scripts_[currentScriptNumber_-1]->Run();
    if( pausingScript_ )return;
    StopScript();
  }
  catch( std::runtime_error const &e )
  {
    throw;
  }
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
  std::vector<Script*>().swap( scripts_ );
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
  try
  {
    if( HandleAlias(commandName,commandLine,p) )return;
  }
  catch( std::runtime_error const &e )
  {
    throw;
  }
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
    line = line.size()>name.size() ? (*i).second+line.substr(name.size()) : (*i).second;
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
  dc.SetBackground( wxBrush(wxT("WHITE"),wxSOLID) );
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
  dc.SetBackground( wxBrush(wxT("WHITE"),wxSOLID) );
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

void SaveSession( wxString &fileName )
{
  ExXML xml;
  try
  {
    xml.OpenFileForWrite( fileName );
  }
  catch ( std::runtime_error &e )
  {
    throw;
  }
  std::ostream &out( xml.GetStream() );
  out << "<extrema>\n";
  //
  out << analysisWindow_;
  out << visualizationWindow_;
  //
  out << *NVariableTable::GetTable();
  out << *TVariableTable::GetTable();
  //
  out << *GRA_colorControl::GetColorMap();
  //
  int nPages = visualizationWindow_->GetNumberOfPages();
  for( int i=1; i<=nPages; ++i )out << visualizationWindow_->GetPage(i);
  out << "</extrema>" << std::endl;
  xml.CloseFile();
}

void RestoreSession( wxString &fileName )
{
  ExXML xml;
  try
  {
    xml.OpenFileForRead( fileName );
  }
  catch ( std::runtime_error const &e )
  {
    throw;
  }
  // NextElementNode() returns 0 if no more nodes
  //                   returns -1 if an error occurred
  //
  if( xml.NextElementNode() == -1 )return; // this should be the <extrema> node
  std::ostream &out( xml.GetStream() );
  //
  visualizationWindow_->DeleteAllPages();
  //
  xml.NextElementNode();                         // get <analysiswindow>
  if( xml.GetName() != wxT("analysiswindow") )
    throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("<analysiswindow>")) );
  
  analysisWindow_->ClearOutput();
  
  long top, left, height, width;
  xml.GetAttributeValue( wxT("top") ).ToLong( &top );
  xml.GetAttributeValue( wxT("left") ).ToLong( &left );
  xml.GetAttributeValue( wxT("height") ).ToLong( &height );
  xml.GetAttributeValue( wxT("width") ).ToLong( &width );
  analysisWindow_->SetSize( static_cast<int>(left), static_cast<int>(top),
                            static_cast<int>(width), static_cast<int>(height) );

  xml.NextElementNode(); // get the messages <string> node
  bool messagesFound = false;
  if( xml.GetName() == wxT("string") )
  {
    messagesFound = true;
    analysisWindow_->WriteOutput( xml.GetTextValue() );
  }
  if( messagesFound )xml.NextElementNode();
  bool commandsFound = false;
  if( xml.GetName() == wxT("commands") )
  {
    commandsFound = true;
    long size;
    xml.GetAttributeValue( wxT("size") ).ToLong( &size );
    for( long i=0; i<size; ++i )
    {
      xml.NextElementNode();  // get <string>
      analysisWindow_->AddCommandString( xml.GetTextValue() );
    }
  }
  if( commandsFound )xml.NextElementNode();
  //
  if( xml.GetName() != wxT("visualizationwindow") )
    throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("<visualizationwindow>")) );

  xml.GetAttributeValue( wxT("top") ).ToLong( &top );
  xml.GetAttributeValue( wxT("left") ).ToLong( &left );
  xml.GetAttributeValue( wxT("height") ).ToLong( &height );
  xml.GetAttributeValue( wxT("width") ).ToLong( &width );
  visualizationWindow_->SetSize( static_cast<int>(left), static_cast<int>(top),
                                 static_cast<int>(width), static_cast<int>(height) );
  double aspectRatio;
  xml.GetAttributeValue( wxT("aspectratio") ).ToDouble( &aspectRatio );

  long tmp;
  xml.GetAttributeValue( wxT("currentpage") ).ToLong( &tmp );

  visualizationWindow_->MakeFirstPage();
  SetAspectRatio( aspectRatio );
  visualizationWindow_->ResetPages();
  visualizationWindow_->ResetWindows();

  NVariableTable *nvt = NVariableTable::GetTable();
  TVariableTable *tvt = TVariableTable::GetTable();
  
  nvt->ClearTable(); // delete all numeric variables
  tvt->ClearTable(); // delete all character variables
  
  xml.NextElementNode(); // get the <numericvariables> node
  bool numericVariablesFound = false;
  if( xml.GetName() == wxT("numericvariables") )
  {
    numericVariablesFound = true;
    long int tmp;
    xml.GetAttributeValue(wxT("size")).ToLong(&tmp); // number of numeric variables
    int size = static_cast<int>(tmp);
    for( int i=0; i<size; ++i )
    {
      xml.NextElementNode();
      if( xml.GetName() == wxT("scalar") )
      {
        wxString name( xml.GetAttributeValue(wxT("name")) );
        wxString type( xml.GetAttributeValue(wxT("type")) );
        xml.NextElementNode(); // get <data>
        double value;
        xml.GetTextValue().ToDouble( &value );
        xml.NextElementNode(); // get <history>
        xml.GetAttributeValue(wxT("size")).ToLong(&tmp); // number of history strings
        int nHist = static_cast<int>(tmp);
        std::deque<wxString> history;
        for( int j=0; j<nHist; ++j )
        {
          xml.NextElementNode(); // get <string>
          history.push_front( xml.GetTextValue() );
        }
        NumericVariable *nv = NumericVariable::PutVariable( name, value, wxString(wxT("")), false );
        nv->SetHistory( history );
        xml.NextElementNode(); // get <origin>
        wxString origin( xml.GetTextValue() );
        if( !origin.empty() )nv->SetOrigin( origin );
        if( type == wxT("FIT") )nv->GetData().SetFit();
      }
      else if( xml.GetName() == wxT("vector") )
      {
        wxString name( xml.GetAttributeValue(wxT("name")) );
        wxString orderS( xml.GetAttributeValue(wxT("order")) );
        int order = 0;
        if( orderS == wxT("ASCENDING") )order = 1;
        else if( orderS == wxT("DESCENDING") )order = 2;
        xml.NextElementNode(); // get <data>
        long int tmp;
        xml.GetAttributeValue(wxT("size")).ToLong(&tmp);
        int size = static_cast<int>(tmp);
        std::vector<double> data( GetDoubleData(xml.GetTextValue(),size) );
        xml.NextElementNode(); // get <history>
        xml.GetAttributeValue(wxT("size")).ToLong(&tmp); // number of history strings
        int nHist = static_cast<int>(tmp);
        std::deque<wxString> history;
        for( int j=0; j<nHist; ++j )
        {
          xml.NextElementNode(); // get <string>
          history.push_front( xml.GetTextValue() );
        }
        NumericVariable *nv = NumericVariable::PutVariable( name, data, order, wxString(wxT("")), false );
        nv->SetHistory( history );
        xml.NextElementNode(); // get <origin>
        wxString origin( xml.GetTextValue() );
        if( !origin.empty() )nv->SetOrigin( origin );
      }
      else if( xml.GetName() == wxT("matrix") )
      {
        wxString name( xml.GetAttributeValue(wxT("name")) );
        long int tmp;
        xml.GetAttributeValue(wxT("rows")).ToLong( &tmp );
        int rows = static_cast<int>(tmp);
        xml.GetAttributeValue(wxT("columns")).ToLong( &tmp );
        int columns = static_cast<int>(tmp);
        xml.NextElementNode(); // get <data>
        xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
        int size = static_cast<int>(tmp);
        std::vector<double> data( GetDoubleData(xml.GetTextValue(),size) );
        xml.NextElementNode(); // get <history>
        xml.GetAttributeValue(wxT("size")).ToLong( &tmp ); // number of history strings
        int nHist = static_cast<int>(tmp);
        std::deque<wxString> history;
        for( int j=0; j<nHist; ++j )
        {
          xml.NextElementNode(); // get <string>
          history.push_front( xml.GetTextValue() );
        }
        NumericVariable *nv =
          NumericVariable::PutVariable( name, data, rows, columns, wxString(wxT("")), false );
        nv->SetHistory( history );
        xml.NextElementNode(); // get <origin>
        wxString origin( xml.GetTextValue() );
        if( !origin.empty() )nv->SetOrigin( origin );
      }
      else if( xml.GetName() == wxT("tensor") )
      {
        wxString name( xml.GetAttributeValue(wxT("name")) );
        long int tmp;
        xml.GetAttributeValue(wxT("rows")).ToLong( &tmp );
        int rows = static_cast<int>(tmp);
        xml.GetAttributeValue(wxT("columns")).ToLong( &tmp );
        int columns = static_cast<int>(tmp);
        xml.GetAttributeValue(wxT("planes")).ToLong( &tmp );
        int planes = static_cast<int>(tmp);
        xml.NextElementNode(); // get <data>
        xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
        int size = static_cast<int>(tmp);
        std::vector<double> data( GetDoubleData(xml.GetTextValue(),size) );
        xml.NextElementNode(); // get <history>
        xml.GetAttributeValue(wxT("size")).ToLong( &tmp ); // number of history strings
        int nHist = static_cast<int>(tmp);
        std::deque<wxString> history;
        for( int j=0; j<nHist; ++j )
        {
          xml.NextElementNode(); // get <string>
          history.push_front( xml.GetTextValue() );
        }
        //NumericVariable *nv = NumericVariable::PutVariable( name, data, rows, columns, planes, wxT(""), false );
        //nv->SetHistory( history );
        //xml.NextElementNode(); // get <origin>
        //wxString origin( xml.GetTextValue() );
        //if( !origin.empty() )nv->SetOrigin( origin );
      }
    }
  }
  if( numericVariablesFound )xml.NextElementNode(); // get <textvariables>
  bool textVariablesFound = false;
  if( xml.GetName() == wxT("textvariables") )
  {
    textVariablesFound = true;
    long int tmp;
    xml.GetAttributeValue(wxT("size")).ToLong( &tmp ); // number of text variables
    int size = static_cast<int>(tmp);
    for( int i=0; i<size; ++i )
    {
      xml.NextElementNode();
      if( xml.GetName() == wxT("textscalar") )
      {
        wxString name( xml.GetAttributeValue(wxT("name")) );
        xml.NextElementNode(); // get <string>
        wxString value = xml.GetTextValue();
        xml.NextElementNode(); // get <history>
        xml.GetAttributeValue(wxT("size")).ToLong( &tmp ); // number of history strings
        int nHist = static_cast<int>(tmp);
        std::deque<wxString> history;
        for( int j=0; j<nHist; ++j )
        {
          xml.NextElementNode(); // get <string>
          history.push_front( xml.GetTextValue() );
        }
        TextVariable *tv = TextVariable::PutVariable( name, value, wxString(wxT("")), false );
        tv->SetHistory( history );
        xml.NextElementNode(); // get <origin>
        wxString origin( xml.GetTextValue() );
        if( !origin.empty() )tv->SetOrigin( origin );
      }
      else if( xml.GetName() == wxT("textvector") )
      {
        wxString name( xml.GetAttributeValue(wxT("name")) );
        xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
        int size = static_cast<int>(tmp);
        std::vector<wxString> strings(size);
        for( int j=0; j<size; ++j )
        {
          xml.NextElementNode(); // get <string>
          strings[j] = xml.GetTextValue();
        }
        xml.NextElementNode(); // get <history>
        xml.GetAttributeValue(wxT("size")).ToLong( &tmp ); // number of history strings
        int nHist = static_cast<int>(tmp);
        std::deque<wxString> history;
        for( int j=0; j<nHist; ++j )
        {
          xml.NextElementNode(); // get <string>
          history.push_front( xml.GetTextValue() );
        }
        TextVariable *tv = TextVariable::PutVariable( name, strings, wxString(wxT("")), false );
        tv->SetHistory( history );
        xml.NextElementNode(); // get <origin>
        wxString origin( xml.GetTextValue() );
        if( !origin.empty() )tv->SetOrigin( origin );
      }
    }
  }
  if( textVariablesFound )xml.NextElementNode();
  if( xml.GetName() != wxT("colormap") )
    throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("<colormap>")) );

  wxString name( xml.GetAttributeValue(wxT("name")) );
  if( name == wxT("USERDEFINED") )
  {
    GRA_colorMap *tmp = new GRA_colorMap( wxT("USERDEFINED") );
    long int itmp;
    xml.GetAttributeValue(wxT("size")).ToLong( &itmp ); // number of colors
    int size = static_cast<int>(itmp);
    for( int i=0; i<size; ++i )
    {
      xml.NextElementNode(); // get <color> node
      long int rl;
      xml.GetAttributeValue(wxT("r")).ToLong( &rl );
      long int gl;
      xml.GetAttributeValue(wxT("g")).ToLong( &gl );
      long int bl;
      xml.GetAttributeValue(wxT("b")).ToLong( &bl );
      tmp->AddColor( new GRA_color(static_cast<int>(rl),static_cast<int>(gl),static_cast<int>(bl)) );
    }
    GRA_colorControl::SetColorMapUser( tmp );
  }
  else
  {
    try
    {
      GRA_colorControl::SetColorMap( name );
    }
    catch (EGraphicsError &e)
    {
      throw std::runtime_error( e.what() );
    }
  }
  int nodeType = xml.NextElementNode(); // get <graphicspage>
  int pageCounter = 0;
  while( nodeType == 1 )
  {
    if( xml.GetName() == wxT("") )break;
    if( xml.GetName() != wxT("graphicspage") )
      throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("<graphicspage>")) );
    // first page already made above
    // only make new page if more than one page
    //
    if( ++pageCounter > 1 )visualizationWindow_->NewPage( pageCounter );
    long int cwl;
    xml.GetAttributeValue(wxT("currentwindow")).ToLong( &cwl );
    int windowNumber = static_cast<int>(cwl);
    nodeType = xml.NextElementNode();
    while( nodeType == 1 )
    {
      if( xml.GetName() == wxT("graphicspage") )break;
      if( xml.GetName() != wxT("graphwindow") )
        throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("<graphwindow>")) );
      
      long int tmp;
      xml.GetAttributeValue(wxT("number")).ToLong( &tmp );
      int number = static_cast<int>(tmp);
      double xLoP;
      xml.GetAttributeValue(wxT("xLoP")).ToDouble( &xLoP );
      double yLoP;
      xml.GetAttributeValue(wxT("yLoP")).ToDouble( &yLoP );
      double xHiP;
      xml.GetAttributeValue(wxT("xHiP")).ToDouble( &xHiP );
      double yHiP;
      xml.GetAttributeValue(wxT("yHiP")).ToDouble( &yHiP );
      double xPrevious;
      xml.GetAttributeValue(wxT("xPrevious")).ToDouble( &xPrevious );
      double yPrevious;
      xml.GetAttributeValue(wxT("yPrevious")).ToDouble( &yPrevious );
      GRA_window *gw = new GRA_window( number, xLoP, yLoP, xHiP, yHiP );
      visualizationWindow_->AddGraphWindow( gw );
      xml.NextElementNode(); // get <axisc>
      if( xml.GetName() != wxT("axisc") )
        throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("<axisc>")) );
      nodeType = SetCharacteristics( xml, gw->GetXAxisCharacteristics() );
      if( xml.GetName() != wxT("axisc") )
        throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("<axisc>")) );
      nodeType = SetCharacteristics( xml, gw->GetYAxisCharacteristics() );
      if( xml.GetName() != wxT("generalc") )
        throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("<generalc>")) );
      nodeType = SetCharacteristics( xml, gw->GetGeneralCharacteristics() );
      if( xml.GetName() != wxT("textc") )
        throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("<textc>")) );
      nodeType = SetCharacteristics( xml, gw->GetTextCharacteristics() );
      if( xml.GetName() != wxT("graphlegendc") )
        throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("<graphlegendc>")) );
      nodeType = SetCharacteristics( xml, gw->GetGraphLegendCharacteristics() );
      if( xml.GetName() != wxT("datacurvec") )
        throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("<datacurvec>")) );
      nodeType = SetCharacteristics( xml, gw->GetDataCurveCharacteristics() );
      if( xml.GetName() != wxT("drawableobjects") )
        throw std::runtime_error( InvalidNodeMessage(xml.GetName(),wxT("<drawableobjects>")) );
      xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
      int size = static_cast<int>(tmp);
      wxClientDC dc( visualizationWindow_->GetPage() );
      for( int i=0; i<size; ++i )
      {
        nodeType = xml.NextElementNode();
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
          GRA_cartesianAxes *axes = GetCartesianAxes( xml );
          axes->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( axes );
        }
        else if( xml.GetName() == wxT("cartesiancurve") )
        {
          GRA_cartesianCurve *curve = GetCartesianCurve( xml );
          curve->Draw( graphicsOutput_, dc );
          gw->AddDrawableObject( curve );
        }
        else if( xml.GetName() == wxT("graphlegend") )
        {
          GRA_legend *gl = gw->GetLegend();
          long int tmp;
          xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
          int size = static_cast<int>(tmp);
          for( int j=0; j<size; ++j )
          {
            xml.NextElementNode(); // get <entry>
            xml.GetAttributeValue(wxT("line")).ToLong( &tmp );
            bool line = tmp?true:false;
            xml.GetAttributeValue(wxT("linetype")).ToLong( &tmp );
            int lt = static_cast<int>(tmp);
            xml.GetAttributeValue(wxT("linewidth")).ToLong( &tmp );
            int lw = static_cast<int>(tmp);
            xml.GetAttributeValue(wxT("linecolor")).ToLong( &tmp );
            int lc = static_cast<int>(tmp);
            xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
            int nSymbols = static_cast<int>(tmp);
            xml.NextElementNode(); // get <string>
            wxString label( xml.GetTextValue() );
            std::vector<GRA_plotSymbol*> symbols(nSymbols);
            for( int k=0; k<nSymbols; ++k )
            {
              xml.NextElementNode(); // get <plotsymbol>
              symbols[k] = GetPlotSymbol( xml );
            }
            gl->AddEntry( label, line, GRA_colorControl::GetColor(lc), lt, lw, symbols, graphicsOutput_, dc );
          }
        }
        else if( xml.GetName() == wxT("contour") )
        {
          GRA_contour *contour = GetContour( xml );
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
      }
      visualizationWindow_->SetWindowNumber( windowNumber );
      dynamic_cast<GraphicsPage*>(visualizationWindow_->GetPage())->Paint();
      nodeType = xml.NextElementNode(); // get <graphwindow>
    }
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

int SetCharacteristics( ExXML &xml, GRA_setOfCharacteristics *characteristics )
{
  int nodeType;
  xml.NextElementNode();
  while( xml.GetName() == wxT("characteristic") )
  {
    wxString name( xml.GetAttributeValue(wxT("name")) );
    wxString type( xml.GetAttributeValue(wxT("type")) );
    wxString shape( xml.GetAttributeValue(wxT("shape")) );
    if( shape == wxT("VECTOR") )
    {
      xml.NextElementNode(); // get <data>
      int size;
      std::istringstream( (char const*)xml.GetAttributeValue(wxT("size")).mb_str(wxConvUTF8) ) >> size;
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
    }
    else
    {
      if( type == wxT("SIZE") )
      {
        double tmp;
        std::istringstream( (char const*)xml.GetAttributeValue(wxT("value")).mb_str(wxConvUTF8) ) >> tmp;
        static_cast<GRA_sizeCharacteristic*>(characteristics->Get(name))->SetAsPercent( tmp );
      }
      else if( type == wxT("DISTANCE") )
      {
        double tmp;
        std::istringstream( (char const*)xml.GetAttributeValue(wxT("value")).mb_str(wxConvUTF8) ) >> tmp;
        static_cast<GRA_distanceCharacteristic*>(characteristics->Get(name))->SetAsPercent( tmp );
      }
      else if( type == wxT("INT") )
      {
        int tmp;
        std::istringstream( (char const*)xml.GetAttributeValue(wxT("value")).mb_str(wxConvUTF8) ) >> tmp;
        static_cast<GRA_intCharacteristic*>(characteristics->Get(name))->Set( tmp );
      }
      else if( type == wxT("DOUBLE") )
      {
        double tmp;
        std::istringstream( (char const*)xml.GetAttributeValue(wxT("value")).mb_str(wxConvUTF8) ) >> tmp;
        static_cast<GRA_doubleCharacteristic*>(characteristics->Get(name))->Set( tmp );
      }
      else if( type == wxT("BOOL") )
      {
        bool tmp;
        std::istringstream( (char const*)xml.GetAttributeValue(wxT("value")).mb_str(wxConvUTF8) ) >> tmp;
        static_cast<GRA_boolCharacteristic*>(characteristics->Get(name))->Set( tmp );
      }
      else if( type == wxT("ANGLE") )
      {
        double tmp;
        std::istringstream( (char const*)xml.GetAttributeValue(wxT("value")).mb_str(wxConvUTF8) ) >> tmp;
        static_cast<GRA_angleCharacteristic*>(characteristics->Get(name))->Set( tmp );
      }
      else if( type == wxT("FONT") )
      {
        int tmp;
        std::istringstream( (char const*)xml.GetAttributeValue(wxT("value")).mb_str(wxConvUTF8) ) >> tmp;
        static_cast<GRA_fontCharacteristic*>(characteristics->
                                             Get(name))->Set( GRA_fontControl::GetFont(tmp) );
      }    
      else if( type == wxT("COLOR") )
      {
        int tmp;
        std::istringstream( (char const*)xml.GetAttributeValue(wxT("value")).mb_str(wxConvUTF8) ) >> tmp;
        static_cast<GRA_colorCharacteristic*>(characteristics->
                                              Get(name))->Set( GRA_colorControl::GetColor(tmp) );
      }
    }
    nodeType = xml.NextElementNode();
  }
  return nodeType;
}

std::string InvalidNodeMessage( wxString const &invalid, wxString const &valid )
{
  wxString s;
  s << wxT("xml node \"<") << invalid << wxT(">\" is invalid: node should be ") << valid;
  return std::string(s.mb_str(wxConvUTF8));
}

GRA_point *GetPoint( ExXML &xml )
{
  double x;
  xml.GetAttributeValue(wxT("x")).ToDouble( &x );
  double y;
  xml.GetAttributeValue(wxT("y")).ToDouble( &y );
  long int c;
  xml.GetAttributeValue(wxT("color")).ToLong( &c );
  return new GRA_point( x, y, GRA_colorControl::GetColor(static_cast<int>(c)) );
}

GRA_polyline *GetPolyline( ExXML &xml )
{
  long int tmp;
  xml.GetAttributeValue(wxT("linewidth")).ToLong( &tmp );
  int lw = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("linetype")).ToLong( &tmp );
  int lt = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("linecolor")).ToLong( &tmp );
  int lc = static_cast<int>(tmp);
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
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
  return new GRA_polyline( x, y, pen, lw, lt, GRA_colorControl::GetColor(lc) );
}

GRA_multiLineFigure *GetMultiLineFigure( ExXML &xml )
{
  long int tmp;
  xml.GetAttributeValue(wxT("linewidth")).ToLong( &tmp );
  int lw = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("linetype")).ToLong( &tmp );
  int lt = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("linecolor")).ToLong( &tmp );
  int lc = static_cast<int>(tmp);
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
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
  return new GRA_multiLineFigure( x, y, pen, GRA_colorControl::GetColor(lc), lw, lt );
}

GRA_errorBar *GetErrorBar( ExXML &xml )
{
  double x;
  xml.GetAttributeValue(wxT("x")).ToDouble( &x );
  double y;
  xml.GetAttributeValue(wxT("y")).ToDouble( &y );
  long int tmp;
  xml.GetAttributeValue(wxT("linewidth")).ToLong( &tmp );
  int lw = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("linecolor")).ToLong( &tmp );
  int lc = static_cast<int>(tmp);
  double bottom;
  xml.GetAttributeValue(wxT("bottom")).ToDouble( &bottom );
  double top;
  xml.GetAttributeValue(wxT("top")).ToDouble( &top );
  double footsize;
  xml.GetAttributeValue(wxT("footsize")).ToDouble( &footsize );
  xml.GetAttributeValue(wxT("vertical")).ToLong( &tmp );
  bool vertical = static_cast<bool>(tmp);
  return new GRA_errorBar( x, y, bottom, top, vertical, footsize,
                           GRA_colorControl::GetColor(lc), lw );
}

GRA_polygon *GetPolygon( ExXML &xml )
{
  long int tmp;
  xml.GetAttributeValue(wxT("linewidth")).ToLong( &tmp );
  int lw = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("linetype")).ToLong( &tmp );
  int lt = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("linecolor")).ToLong( &tmp );
  int lc = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("fillcolor")).ToLong( &tmp );
  int fc = static_cast<int>(tmp);
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<double> x(size), y(size), data(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    x[j] = data[2*j];
    y[j] = data[2*j+1];
  }
  return new GRA_polygon( x, y, GRA_colorControl::GetColor(lc),
                          GRA_colorControl::GetColor(fc), lw, lt );
}

GRA_ellipse *GetEllipse( ExXML &xml )
{
  double xmin;
  xml.GetAttributeValue(wxT("xmin")).ToDouble( &xmin );
  double ymin;
  xml.GetAttributeValue(wxT("ymin")).ToDouble( &ymin );
  double xmax;
  xml.GetAttributeValue(wxT("xmax")).ToDouble( &xmax );
  double ymax;
  xml.GetAttributeValue(wxT("ymax")).ToDouble( &ymax );
  long int tmp;
  xml.GetAttributeValue(wxT("circle")).ToLong( &tmp );
  bool circle = static_cast<bool>(tmp);  
  xml.GetAttributeValue(wxT("linecolor")).ToLong( &tmp );
  int lc = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("fillcolor")).ToLong( &tmp );
  int fc = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("linewidth")).ToLong( &tmp );
  int lw = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("linetype")).ToLong( &tmp );
  int lt = static_cast<int>(tmp);
  return new GRA_ellipse( xmin, ymin, xmax, ymax, circle,
                          GRA_colorControl::GetColor(lc), GRA_colorControl::GetColor(fc),
                          lw, lt );
}

GRA_plotSymbol *GetPlotSymbol( ExXML &xml )
{
  double x;
  xml.GetAttributeValue(wxT("x")).ToDouble( &x );
  double y;
  xml.GetAttributeValue(wxT("y")).ToDouble( &y );
  long int tmp;
  xml.GetAttributeValue(wxT("shape")).ToLong( &tmp );
  int code = static_cast<int>(tmp);
  double angle;
  xml.GetAttributeValue(wxT("angle")).ToDouble( &angle );
  double size;
  xml.GetAttributeValue(wxT("size")).ToDouble( &size );
  xml.GetAttributeValue(wxT("color")).ToLong( &tmp );
  int color = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("linewidth")).ToLong( &tmp );
  int lw = static_cast<int>(tmp);
  GRA_plotSymbol *ps = new GRA_plotSymbol( code, size, angle,
    GRA_colorControl::GetColor(color), lw );
  ps->SetLocation( x, y );
  return ps;
}

GRA_drawableText *GetDrawableText( ExXML &xml )
{
  double x;
  xml.GetAttributeValue(wxT("x")).ToDouble( &x );
  double y;
  xml.GetAttributeValue(wxT("y")).ToDouble( &y );
  double height;
  xml.GetAttributeValue(wxT("height")).ToDouble( &height );
  double angle;
  xml.GetAttributeValue(wxT("angle")).ToDouble( &angle );
  long int tmp;
  xml.GetAttributeValue(wxT("alignment")).ToLong( &tmp );
  int alignment = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("color")).ToLong( &tmp );
  int color = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("font")).ToLong( &tmp );
  int font = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("graphunits")).ToLong( &tmp );
  bool graphunits = static_cast<bool>(tmp);
  xml.NextElementNode(); // get <string>
  GRA_drawableText *dt = new GRA_drawableText( xml.GetTextValue(), height,
    angle, x, y, alignment, GRA_fontControl::GetFont(font),
    GRA_colorControl::GetColor(color) );
  dt->Parse();
  return dt;
}

GRA_cartesianAxes *GetCartesianAxes( ExXML &xml )
{
  long int tmp;
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  xml.NextElementNode(); // get x <axis>
  GRA_axis *xaxis = GetAxis( xml, 'X' );
  xml.NextElementNode(); // get y <axis>
  GRA_axis *yaxis = GetAxis( xml, 'Y' );
  GRA_axis *bottom =0, *top =0, *left =0, *right =0;
  if( size == 6 )
  {
    xml.NextElementNode(); // get bottom <axis>
    bottom = GetAxis( xml, 'X' );
    xml.NextElementNode(); // get top <axis>
    top = GetAxis( xml, 'X' );
    xml.NextElementNode(); // get left <axis>
    left = GetAxis( xml, 'Y' );
    xml.NextElementNode(); // get right <axis>
    right = GetAxis( xml, 'Y' );
  }
  GRA_cartesianAxes *axes = new GRA_cartesianAxes();
  axes->SetAxes( xaxis, yaxis, bottom, top, left, right );
  return axes;
}

GRA_axis *GetAxis( ExXML &xml, wxChar const type )
{
  double xOrigin;
  xml.GetAttributeValue(wxT("xorigin")).ToDouble( &xOrigin );
  double yOrigin;
  xml.GetAttributeValue(wxT("yorigin")).ToDouble( &yOrigin );
  xml.NextElementNode(); // get <ticcoordinates>
  long int tmp;
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<double> xtics(size), ytics(size), data(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    xtics[j] = data[2*j];
    ytics[j] = data[2*j+1];
  }
  xml.NextElementNode(); // get <axisc>
  //
  GRA_window *gw = visualizationWindow_->GetGraphWindow();
  GRA_setOfCharacteristics *generalC = gw->GetGeneralCharacteristics();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  double const xlaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
  double const ylaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("LOWERAXIS")))->GetAsWorld();
  double const xuaxis = static_cast<GRA_distanceCharacteristic*>(xAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
  double const yuaxis = static_cast<GRA_distanceCharacteristic*>(yAxisC->Get(wxT("UPPERAXIS")))->GetAsWorld();
  double const xlwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XLOWERWINDOW")))->GetAsWorld();
  double const xuwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("XUPPERWINDOW")))->GetAsWorld();
  double const ylwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("YLOWERWINDOW")))->GetAsWorld();
  double const yuwind = static_cast<GRA_distanceCharacteristic*>(generalC->Get(wxT("YUPPERWINDOW")))->GetAsWorld();
  int const gridLineType = static_cast<GRA_intCharacteristic*>(generalC->Get(wxT("GRIDLINETYPE")))->Get();
  GRA_setOfCharacteristics axisc;
  switch (type)
  {
    case 'X':
    {
      axisc = *xAxisC;
      axisc.AddDistance( wxT("XLOWERWINDOW"), xlwind, false, xminW_, xmaxW_ );
      axisc.AddDistance( wxT("YLOWERWINDOW"), ylwind, false, yminW_, ymaxW_ );
      axisc.AddDistance( wxT("XUPPERWINDOW"), xuwind, false, xminW_, xmaxW_ );
      axisc.AddDistance( wxT("YUPPERWINDOW"), yuwind, false, yminW_, ymaxW_ );
      axisc.AddDistance(wxT("OLAXIS"),ylaxis,false,ylwind,yuwind);
      axisc.AddDistance(wxT("OUAXIS"),yuaxis,false,ylwind,yuwind);
      axisc.AddNumber(wxT("GRIDLINETYPE"),gridLineType);
      break;
    }
    case 'Y':
    {
      axisc = *yAxisC;
      axisc.AddDistance( wxT("XLOWERWINDOW"), xlwind, false, xminW_, xmaxW_ );
      axisc.AddDistance( wxT("YLOWERWINDOW"), ylwind, false, yminW_, ymaxW_ );
      axisc.AddDistance( wxT("XUPPERWINDOW"), xuwind, false, xminW_, xmaxW_ );
      axisc.AddDistance( wxT("YUPPERWINDOW"), yuwind, false, yminW_, ymaxW_ );
      axisc.AddDistance(wxT("OLAXIS"),xlaxis,false,ylwind,yuwind);
      axisc.AddDistance(wxT("OUAXIS"),xuaxis,false,ylwind,yuwind);
      axisc.AddNumber(wxT("GRIDLINETYPE"),gridLineType);
      break;
    }
  }
  SetCharacteristics( xml, &axisc );
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<GRA_polyline*> polylines(size);
  for( int j=0; j<size; ++j )
  {
    xml.NextElementNode(); // get <polyline>
    polylines[j] = GetPolyline(xml);
  }
  xml.NextElementNode(); // get <drawabletexts>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<GRA_drawableText*> textVec(size);
  for( int j=0; j<size; ++j )
  {
    xml.NextElementNode(); // get <drawabletext>
    textVec[j] = GetDrawableText(xml);
  }
  GRA_axis *axis = new GRA_axis( xOrigin, yOrigin, &axisc );
  axis->SetTicCoordinates( xtics, ytics );
  axis->SetPolylines( polylines );
  axis->SetDrawableText( textVec );
  return axis;
}

GRA_cartesianCurve *GetCartesianCurve( ExXML &xml )
{
  long int tmp;
  xml.GetAttributeValue(wxT("histype")).ToLong( &tmp );
  int histype = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("linetype")).ToLong( &tmp );
  int linetype = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("linewidth")).ToLong( &tmp );
  int linewidth = static_cast<int>(tmp);
  xml.GetAttributeValue(wxT("smooth")).ToLong( &tmp );
  bool smooth = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("linecolor")).ToLong( &tmp );
  GRA_color *linecolor = GRA_colorControl::GetColor(static_cast<int>(tmp));
  xml.GetAttributeValue(wxT("areafillcolor")).ToLong( &tmp );
  int temp = static_cast<int>(tmp);
  GRA_color *areafillcolor = 0;
  if( temp != 0 )areafillcolor = GRA_colorControl::GetColor(temp);
  double xlaxis;
  xml.GetAttributeValue(wxT("xlaxis")).ToDouble( &xlaxis );
  double ylaxis;
  xml.GetAttributeValue(wxT("ylaxis")).ToDouble( &ylaxis );
  double xuaxis;
  xml.GetAttributeValue(wxT("xuaxis")).ToDouble( &xuaxis );
  double yuaxis;
  xml.GetAttributeValue(wxT("yuaxis")).ToDouble( &yuaxis );
  double xmin;
  xml.GetAttributeValue(wxT("xmin")).ToDouble( &xmin );
  double ymin;
  xml.GetAttributeValue(wxT("ymin")).ToDouble( &ymin );
  double xmax;
  xml.GetAttributeValue(wxT("xmax")).ToDouble( &xmax );
  double ymax;
  xml.GetAttributeValue(wxT("ymax")).ToDouble( &ymax );
  xml.NextElementNode(); // get <areafillcolors>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<GRA_color*> areaFillColors(size);
  std::vector<int> ivec( GetIntData(xml.GetTextValue(),size) );
  for( int j=0; j<size; ++j )
    areaFillColors[j] = GRA_colorControl::GetColor(ivec[j]);
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> x(size), y(size), data(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    x[j] = data[2*j];
    y[j] = data[2*j+1];
  }
  xml.NextElementNode(); // get <xe1>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> xe1( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <xe2>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> xe2( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <ye1>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> ye1( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <ye2>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> ye2( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <xycurve>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> xcurve(size), ycurve(size), data2(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    xcurve[j] = data2[2*j];
    ycurve[j] = data2[2*j+1];
  }
  xml.NextElementNode(); // get <pen>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<int> pen( GetIntData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <plotsymbols>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<GRA_plotSymbol*> symbols(size);
  for( int j=0; j<size; ++j )
  {
    xml.NextElementNode(); // get <plotsymbol>
    symbols[j] = GetPlotSymbol( xml );
  }
  xml.NextElementNode(); // get <errorbars>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<GRA_errorBar*> errorbars(size);
  for( int j=0; j<size; ++j )
  {
    xml.NextElementNode(); // get <plotsymbol>
    errorbars[j] = GetErrorBar( xml );
  }
  GRA_cartesianCurve *curve = new GRA_cartesianCurve( x, y, xe1, ye1, xe2, ye2, smooth );
  curve->SetValues( histype, linetype, linewidth, linecolor, areafillcolor, xlaxis, ylaxis,
                    xuaxis, yuaxis, xmin, ymin, xmax, ymax, areaFillColors, xcurve, ycurve,
                    pen, symbols, errorbars );
  return curve;
}

GRA_contour *GetContour( ExXML &xml )
{
  long int tmp;
  xml.GetAttributeValue(wxT("color")).ToLong( &tmp );
  int color = static_cast<int>(tmp);
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
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
  xml.NextElementNode(); // get <drawabletexts>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<GRA_drawableText*> textVec(size);
  for( int j=0; j<size; ++j )
  {
    xml.NextElementNode(); // get <drawabletext>
    textVec[j] = GetDrawableText(xml);
  }
  GRA_contour *contour = new GRA_contour();
  contour->SetColor( GRA_colorControl::GetColor(color) );
  contour->SetCoords( x, y, connect );
  contour->SetDrawableText( textVec );
  return contour;
}

GRA_boxPlot *GetBoxPlot( ExXML &xml )
{
  long int tmp;
  xml.GetAttributeValue(wxT("nrow")).ToLong( &tmp );
  int nrow = static_cast<int>(tmp);
  double fmin;
  xml.GetAttributeValue(wxT("fmin")).ToDouble( &fmin );
  double fmax;
  xml.GetAttributeValue(wxT("fmax")).ToDouble( &fmax );
  double gmin;
  xml.GetAttributeValue(wxT("gmin")).ToDouble( &gmin );
  double gmax;
  xml.GetAttributeValue(wxT("gmax")).ToDouble( &gmax );
  xml.GetAttributeValue(wxT("xprofile")).ToLong( &tmp );
  bool xprofile = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("yprofile")).ToLong( &tmp );
  bool yprofile = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("border")).ToLong( &tmp );
  bool border = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("zoom")).ToLong( &tmp );
  bool zoom = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("axes")).ToLong( &tmp );
  bool axes = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("reset")).ToLong( &tmp );
  bool reset = static_cast<bool>(tmp);
  double bscale;
  xml.GetAttributeValue(wxT("bscale")).ToDouble( &bscale );
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<double> x( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> y( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> z( GetDoubleData(xml.GetTextValue(),size) );
  return new GRA_boxPlot( x, y, z, nrow, fmin, fmax, gmin, gmax, xprofile, yprofile,
                          border, zoom, axes, reset, bscale );
}

GRA_diffusionPlot *GetDiffusionPlot( ExXML &xml )
{
  long int tmp;
  xml.GetAttributeValue(wxT("nrow")).ToLong( &tmp );
  int nrow = static_cast<int>(tmp);
  double fmin;
  xml.GetAttributeValue(wxT("fmin")).ToDouble( &fmin );
  double fmax;
  xml.GetAttributeValue(wxT("fmax")).ToDouble( &fmax );
  double gmin;
  xml.GetAttributeValue(wxT("gmin")).ToDouble( &gmin );
  double gmax;
  xml.GetAttributeValue(wxT("gmax")).ToDouble( &gmax );
  xml.GetAttributeValue(wxT("xprofile")).ToLong( &tmp );
  bool xprofile = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("yprofile")).ToLong( &tmp );
  bool yprofile = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("border")).ToLong( &tmp );
  bool border = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("zoom")).ToLong( &tmp );
  bool zoom = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("axes")).ToLong( &tmp );
  bool axes = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("reset")).ToLong( &tmp );
  bool reset = static_cast<bool>(tmp);
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<double> x( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> y( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> z( GetDoubleData(xml.GetTextValue(),size) );
  return new GRA_diffusionPlot( x, y, z, nrow, fmin, fmax, gmin, gmax, xprofile, yprofile,
                                border, zoom, axes, reset );
}

GRA_ditheringPlot *GetDitheringPlot( ExXML &xml )
{
  long int tmp;
  xml.GetAttributeValue(wxT("nrow")).ToLong( &tmp );
  int nrow = static_cast<int>(tmp);
  double fmin;
  xml.GetAttributeValue(wxT("fmin")).ToDouble( &fmin );
  double fmax;
  xml.GetAttributeValue(wxT("fmax")).ToDouble( &fmax );
  double gmin;
  xml.GetAttributeValue(wxT("gmin")).ToDouble( &gmin );
  double gmax;
  xml.GetAttributeValue(wxT("gmax")).ToDouble( &gmax );
  xml.GetAttributeValue(wxT("xprofile")).ToLong( &tmp );
  bool xprofile = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("yprofile")).ToLong( &tmp );
  bool yprofile = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("border")).ToLong( &tmp );
  bool border = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("zoom")).ToLong( &tmp );
  bool zoom = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("axes")).ToLong( &tmp );
  bool axes = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("reset")).ToLong( &tmp );
  bool reset = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("legend")).ToLong( &tmp );
  bool legend = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("equalspaced")).ToLong( &tmp );
  bool equalspaced = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("areas")).ToLong( &tmp );
  bool areas = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("volumes")).ToLong( &tmp );
  bool volumes = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("lines")).ToLong( &tmp );
  bool lines = static_cast<bool>(tmp);
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<int> dither( GetIntData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> contourLevels( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> x( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> y( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> z( GetDoubleData(xml.GetTextValue(),size) );
  return new GRA_ditheringPlot( x, y, z, nrow, fmin, fmax, gmin, gmax, xprofile, yprofile,
                                border, zoom, axes, reset, legend, equalspaced, areas,
                                volumes, lines, dither, contourLevels );
}

GRA_gradientPlot *GetGradientPlot( ExXML &xml )
{
  long int tmp;
  xml.GetAttributeValue(wxT("nrow")).ToLong( &tmp );
  int nrow = static_cast<int>(tmp);
  double fmin;
  xml.GetAttributeValue(wxT("fmin")).ToDouble( &fmin );
  double fmax;
  xml.GetAttributeValue(wxT("fmax")).ToDouble( &fmax );
  double gmin;
  xml.GetAttributeValue(wxT("gmin")).ToDouble( &gmin );
  double gmax;
  xml.GetAttributeValue(wxT("gmax")).ToDouble( &gmax );
  xml.GetAttributeValue(wxT("xprofile")).ToLong( &tmp );
  bool xprofile = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("yprofile")).ToLong( &tmp );
  bool yprofile = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("border")).ToLong( &tmp );
  bool border = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("zoom")).ToLong( &tmp );
  bool zoom = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("axes")).ToLong( &tmp );
  bool axes = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("reset")).ToLong( &tmp );
  bool reset = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("legend")).ToLong( &tmp );
  bool legend = static_cast<bool>(tmp);
  xml.GetAttributeValue(wxT("linear")).ToLong( &tmp );
  bool linear = static_cast<bool>(tmp);
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  int size = static_cast<int>(tmp);
  std::vector<double> x( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> y( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  xml.GetAttributeValue(wxT("size")).ToLong( &tmp );
  size = static_cast<int>(tmp);
  std::vector<double> z( GetDoubleData(xml.GetTextValue(),size) );
  return new GRA_gradientPlot( x, y, z, nrow, fmin, fmax, gmin, gmax, xprofile, yprofile,
                               border, zoom, axes, reset, legend, linear );
}

} // end of namespace

// end of file
