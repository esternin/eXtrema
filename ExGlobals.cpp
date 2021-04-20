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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <dir.h>
#include <stdexcept>

#include "ExGlobals.h"

#include "ECommandError.h"
#include "EGraphicsError.h"
#include "EExpressionError.h"
#include "Script.h"
#include "extremaMain.h"
#include "MainGraphicsWindow.h"
#include "ParseLine.h"
#include "Command.h"
#include "CommandTable.h"
#include "NumericVariable.h"
#include "TextVariable.h"
#include "NVariableTable.h"
#include "TVariableTable.h"
#include "GRA_window.h"
#include "GRA_borlandMonitor.h"
#include "GRA_colorControl.h"
#include "GRA_fontControl.h"
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
#include "GRA_contourLine.h"
#include "GRA_boxPlot.h"
#include "GRA_diffusionPlot.h"
#include "GRA_ditheringPlot.h"
#include "GRA_gradientPlot.h"
#include "GRA_page.h"
#include "GRA_rectangle.h"
#include "GRA_polarAxes.h"
#include "GRA_polarCurve.h"

namespace ExGlobals
{
extern double const xminWDef_ = 0.0;
extern double const yminWDef_ = 0.0;
extern double const xmaxWDef_ = 11.0;
extern double const ymaxWDef_ = 8.5;
extern int const xminMDef_ = 0;
extern int const yminMDef_ = 0;
extern int const xmaxMDef_ = 647;
extern int const ymaxMDef_ = 500;
extern double const pageBorder_ = 0.25; // edge gap of graphics page (inch)

double splineTension_;
GRA_outputType *screenOutput_;
double xminW_, yminW_, xmaxW_, ymaxW_;
double xminClip_, yminClip_, xmaxClip_, ymaxClip_;
int xminM_, yminM_, xmaxM_, ymaxM_;
ExString scriptExtension_, stackExtension_;
char continuationCharacter_, executeCharacter_;
bool echo_, stackIsOn_, stackSaved_, executeFlag_;
int nHistory_, maxHistory_, currentScriptNumber_, scriptNumberSave_;
std::vector<Script*> scripts_;
bool workingColorFlag_, workingFontFlag_, workingFontColorFlag_;
bool workingFontHeightFlag_, workingFontAngleFlag_;
double workingFontHeight_, workingFontAngle_;
GRA_color *workingColor_, *workingFontColor_;
GRA_font *workingFont_;
ExString workingFontName_, currentPath_, executablePath_, stackFile_;
bool noviceMode_, executeCommand_, returnCommand_, pausingScript_, restartingScript_;
bool prepareToStopScript_, prepareToExecuteScript_, prepareToPauseScript_;
std::ofstream stackStream_;
std::map<ExString,ExString> alias_;

void Initialize()
{
  executablePath_ = ExString(_argv[0]).ExtractFilePath() + "/";
  //
  // getcwd returns the current working directory
  char buf[256];
  if( !getcwd(buf,256) )currentPath_ = executablePath_;
  else                  currentPath_ = ExString( buf ) + "/";
  //
  splineTension_ = 0.0;
  //
  workingColorFlag_ = false;
  workingFontFlag_ = false;
  workingFontColorFlag_ = false;
  workingFontHeightFlag_ = false;
  workingFontAngleFlag_ = false;
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
  screenOutput_ = new GRA_borlandMonitor();
  //
  executeFlag_ = true;
  scriptExtension_ = "pcm";
  stackExtension_ = "stk";
  currentScriptNumber_ = 0;
  scriptNumberSave_ = 0;
  continuationCharacter_ = '-';
  executeCharacter_ = '@';
  nHistory_ = 1;
  maxHistory_ = 5;
  echo_ = false;
  stackIsOn_ = false;
  stackSaved_ = false;
  //
  noviceMode_ = true;
  //
  executeCommand_ = false;
  returnCommand_ = false;
  pausingScript_ = false;
  restartingScript_ = false;
  prepareToStopScript_ = false;
  prepareToExecuteScript_ = false;
  prepareToPauseScript_ = false;
}

void ExecuteInit()
{
  Command *cp = CommandTable::GetTable()->Get("EXECUTE");
  if( !cp )throw runtime_error( "unknown command: EXECUTE" );
  ExString fname( "extremaInit.pcm" );
  ExString parameters;
  if( _argc > 1 )
  {
    std::string arg1( _argv[1] );
    if( arg1.size()>2 && arg1.substr(0,2)=="--" )
    {
      std::string command( fname.c_str() );
      if( arg1.substr(2)=="script" && _argc>2 )command = _argv[2];
      std::size_t b = command.find(" ");
      fname = command.substr(0,b);
      if( b != command.npos )parameters = command.substr(b);
    }
  }
  std::ifstream in;
  in.open( fname.c_str() );
  if( !in.is_open() )
  {
    if( fname.ExtractFilePath().empty() )
    {
      ExString fname2( ExGlobals::GetCurrentPath() + fname );
      in.open( fname2.c_str() );
      if( in.is_open() )fname = fname2;
      else
      {
        fname2 = ExGlobals::GetExecutablePath() + fname;
        in.open( fname2.c_str() );
        if( !in.is_open() )return;
        fname = fname2;
      }
    }
    else return;
  }
  in.close();
  try
  {
    ParseLine p( ExString("EXECUTE '")+fname+"'"+parameters );
    try
    {
      p.ParseIt();
    }
    catch( ESyntaxError &e )
    {
      throw runtime_error( e.what() );
    }
    cp->Execute( &p );
  }
  catch( ECommandError &e )
  {
    throw runtime_error( e.what() );
  }
  try
  {
    RunScript();
  }
  catch ( runtime_error &e )
  {
    ShowScriptError( e.what() );
    StopAllScripts();
  }
}

void DeleteStuff()
{
  while( !scripts_.empty() )
  {
    delete scripts_.back();
    scripts_.pop_back();
  }
  TVariableTable::GetTable()->ClearTable();
  NVariableTable::GetTable()->ClearTable();
}

bool GetNoviceMode()
{ return noviceMode_; }

void SetNoviceMode( bool mode )
{ noviceMode_ = mode; }

void ToggleNoviceMode()
{ noviceMode_ = !noviceMode_; }

void SetCurrentPath( ExString path )
{ currentPath_ = path; }

ExString GetCurrentPath()
{ return currentPath_; }

ExString GetExecutablePath()
{ return executablePath_; }

void SetTension( double t )
{ splineTension_ = t; }

double GetTension()
{ return splineTension_; }

void SetWorkingColorFlag( bool b )
{ workingColorFlag_ = b; }

bool GetWorkingColorFlag()
{ return workingColorFlag_; }

void SetWorkingColor( GRA_color *c )
{ workingColor_=c; workingColorFlag_=true; }

GRA_color *GetWorkingColor()
{ return workingColor_; }

void SetWorkingFontFlags( bool b )
{
  workingFontFlag_ = b;
  workingFontColorFlag_ = b;
  workingFontHeightFlag_ = b;
  workingFontAngleFlag_ = b;
}

bool GetWorkingFontFlag()
{ return workingFontFlag_; }

void SetWorkingFontFlag( bool b )
{ workingFontFlag_ = b; }

void SetWorkingFont( GRA_font *font )
{ workingFont_=font; workingFontFlag_=true; }

GRA_font *GetWorkingFont()
{ return workingFont_; }

bool GetWorkingFontColorFlag()
{ return workingFontColorFlag_; }

void SetWorkingFontColorFlag( bool b )
{ workingFontColorFlag_ = b; }

void SetWorkingFontColor( GRA_color *c )
{ workingFontColor_=c; workingFontColorFlag_=true; }

GRA_color *GetWorkingFontColor()
{ return workingFontColor_; }

bool GetWorkingFontHeightFlag()
{ return workingFontHeightFlag_; }

void SetWorkingFontHeightFlag( bool b )
{ workingFontHeightFlag_ = b; }

void SetWorkingFontHeight( double height )
{ workingFontHeight_=height; workingFontHeightFlag_=true; }

double GetWorkingFontHeight()
{ return workingFontHeight_; }

bool GetWorkingFontAngleFlag()
{ return workingFontAngleFlag_; }

void SetWorkingFontAngleFlag( bool b )
{ workingFontAngleFlag_ = b; }

void SetWorkingFontAngle( double angle )
{ workingFontAngle_=angle; workingFontAngleFlag_=true; }

double GetWorkingFontAngle()
{ return workingFontAngle_; }

GRA_outputType *GetScreenOutput()
{ return screenOutput_; }

void WarningMessage( ExString const &s )
{ WriteOutput( ExString("Warning: ")+s ); }

void QuitApp()
{ MainForm->QuitApp(); }

void AddGraphWindow( GRA_window *gw )
{ MainGraphicsForm->AddGraphWindow(gw); }

std::vector<GRA_window*> &GetGraphWindows()
{ return MainGraphicsForm->GetGraphWindows(); }

GRA_window *GetGraphWindow()
{ return MainGraphicsForm->GetGraphWindow(); }

GRA_window *GetGraphWindow( int n )
{ return MainGraphicsForm->GetGraphWindow(n); }

void ClearWindows()
{ MainGraphicsForm->ClearWindows(); }

void ReplotCurrentWindow()
{ MainGraphicsForm->ReplotCurrentWindow(); }

int GetWindowNumber()
{ return MainGraphicsForm->GetWindowNumber(); }

int GetNumberOfWindows()
{ return MainGraphicsForm->GetNumberOfWindows(); }

void DrawGraphWindows( GRA_outputType *outputType )
{ MainGraphicsForm->DrawGraphWindows( outputType ); }

void SetWindowNumber( int n )
{ MainGraphicsForm->SetWindowNumber(n); }

void ReplotAllWindows()
{ MainGraphicsForm->ReplotAllWindows(); }

void ResetClippingBoundary()
{
  xminClip_ = xminW_ + pageBorder_;
  yminClip_ = yminW_ + pageBorder_;
  xmaxClip_ = xmaxW_ - pageBorder_;
  ymaxClip_ = ymaxW_ - pageBorder_;
}

void GetWorldLimits( double &xmin, double &ymin, double &xmax, double &ymax )
{
  xmin = xminW_;
  ymin = yminW_;
  xmax = xmaxW_;
  ymax = ymaxW_;
}

void SetWorldLimits( double xmin, double ymin, double xmax, double ymax )
{
  if( xmax-xmin < std::numeric_limits<double>::epsilon() )
    throw EGraphicsError( "SetWorldLimits: xmax <= xmin" );
  if( ymax-ymin < std::numeric_limits<double>::epsilon() )
    throw EGraphicsError( "SetWorldLimits: ymax <= ymin" );
  xminW_ = xmin;
  yminW_ = ymin;
  xmaxW_ = xmax;
  ymaxW_ = ymax;
  //
  xminClip_ = xminW_ + pageBorder_;
  yminClip_ = yminW_ + pageBorder_;
  xmaxClip_ = xmaxW_ - pageBorder_;
  ymaxClip_ = ymaxW_ - pageBorder_;
  //
  delete screenOutput_;
  screenOutput_ = new GRA_borlandMonitor();
  static_cast<GRA_borlandMonitor*>(screenOutput_)->SetCanvas( MainGraphicsForm->GetCanvas() );
  //
  MainGraphicsForm->SetImageSize();
  MainGraphicsForm->DrawGraphWindows( screenOutput_ );
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

bool GetExecuteFlag()
{ return executeFlag_; }

char GetContinuationCharacter()
{ return continuationCharacter_; }

void SetScriptExtension( ExString const &s )
{ scriptExtension_ = s; }

ExString GetScriptExtension()
{ return scriptExtension_; }

void SetStackExtension( ExString const &s )
{ stackExtension_ = s; }

ExString GetStackExtension()
{ return stackExtension_; }

bool StackIsOn()
{ return stackIsOn_; }

std::ofstream *GetStackStream()
{ return &stackStream_; }

ExString &GetStackFile()
{ return stackFile_; }

void SetStackOff()
{
  stackIsOn_ = false;
  if( stackStream_.is_open() )stackStream_.close();
  stackStream_.clear( std::ios::goodbit );
}

bool GetEcho()
{ return echo_; }

void SetEcho( bool e )
{ echo_ = e; }

void SetNHistory( int i )
{ nHistory_ = i; }

int GetNHistory()
{ return nHistory_; }

void SetMaxHistory( int i )
{ maxHistory_ = i; }

int GetMaxHistory()
{ return maxHistory_; }

void WriteStack( ExString const &s )
{ if( stackIsOn_ )stackStream_ << s.c_str() << "\n"; }

bool NotInaScript()
{ return (currentScriptNumber_==0); }

bool GetExecuteCommand()
{ return executeCommand_; }

void SetExecuteCommand( bool b )
{ executeCommand_ = b; }

bool GetReturnCommand()
{ return returnCommand_; }

void SetReturnCommand( bool b )
{ returnCommand_ = b; }

void SetRestartingScript( bool b )
{ restartingScript_ = b; }

bool GetRestartingScript()
{ return restartingScript_; }

void ClearGraphicsMonitor()
{
  screenOutput_->ClearGraphicsCanvas();
  MainGraphicsForm->DisplayBackgrounds( screenOutput_ );
}

void ClearHistory()
{ MainForm->ClearMainListBox(); }

void ChangeAspectRatio( double r ) // r = height/width
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
  ymaxM_ = xmaxMDef_*r;
  //
  delete screenOutput_;
  screenOutput_ = new GRA_borlandMonitor();
  static_cast<GRA_borlandMonitor*>(screenOutput_)->SetCanvas( MainGraphicsForm->GetCanvas() );
  //
  MainGraphicsForm->SetImageSize();
  //
  MainGraphicsForm->ResetWindows();
}

double GetAspectRatio()
{ return (ymaxW_-yminW_)/(xmaxW_-xminW_); }

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
  delete screenOutput_;
  screenOutput_ = new GRA_borlandMonitor();
  static_cast<GRA_borlandMonitor*>(screenOutput_)->SetCanvas( MainGraphicsForm->GetCanvas() );
  //
  MainGraphicsForm->SetImageSize();
  MainGraphicsForm->DrawGraphWindows( screenOutput_ );
}

void SetClippingBoundary( double xmin, double ymin, double xmax, double ymax )
{
  // the clipping boundary will always be within the world coordinate boundary
  //
  if( xmax-xmin < std::numeric_limits<double>::epsilon() )
    throw EGraphicsError( "SetClippingBoundary: xmax <= xmin" );
  if( ymax-ymin < std::numeric_limits<double>::epsilon() )
    throw EGraphicsError( "SetClippingBoundary: ymax <= ymin" );
  xminClip_ = std::max(xmin,xminW_);
  yminClip_ = std::max(ymin,yminW_);
  xmaxClip_ = std::min(xmax,xmaxW_);
  ymaxClip_ = std::min(ymax,ymaxW_);
}

void SetMonitorLimits( int xmin, int ymin, int xmax, int ymax )
{
  xminM_ = xmin;
  yminM_ = ymin;
  xmaxM_ = xmax;
  ymaxM_ = ymax;
  //
  delete screenOutput_;
  screenOutput_ = new GRA_borlandMonitor();
  static_cast<GRA_borlandMonitor*>(screenOutput_)->SetCanvas( MainGraphicsForm->GetCanvas() );
  //
  MainGraphicsForm->SetImageSize();
}

void ResetMonitorLimits()
{
  xminM_ = xminMDef_;
  yminM_ = yminMDef_;
  xmaxM_ = xmaxMDef_;
  ymaxM_ = ymaxMDef_;
  //
  delete screenOutput_;
  screenOutput_ = new GRA_borlandMonitor();
  static_cast<GRA_borlandMonitor*>(screenOutput_)->SetCanvas( MainGraphicsForm->GetCanvas() );
}

void DefaultSize()
{
  ResetMonitorLimits();
  MainGraphicsForm->SetImageSize();
  MainGraphicsForm->DrawGraphWindows( screenOutput_ );
}

void ChangeSize( double value )
{
  int width, height;
  MainGraphicsForm->GetImageDimensions( width, height );
  SetMonitorLimits( 0, 0,
    static_cast<int>(value*width+0.5), static_cast<int>(value*height+0.5) );
  MainGraphicsForm->SetImageSize();
  MainGraphicsForm->DrawGraphWindows( screenOutput_ );
}

void WriteOutput( ExString const &s )
{ MainForm->WriteOutput( s.c_str() ); }

bool SetStackOn( ExString const &filename, bool append )
{
  stackFile_ = filename;
  if( stackStream_.is_open() )stackStream_.close();
  stackStream_.clear( std::ios::goodbit );
  if( append )stackStream_.open( stackFile_.c_str(), std::ios::out|std::ios::app );
  else        stackStream_.open( stackFile_.c_str(), std::ios::out );
  stackIsOn_ = stackStream_.is_open();
  return stackIsOn_;
}

Script *GetScript()
{
  if( currentScriptNumber_ == 0 )return 0;
  return scripts_[currentScriptNumber_-1];
}

bool GetPausingScript()
{ return pausingScript_; }

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

void StopScript()
{
  if( scripts_.empty() )return;
  delete scripts_.back();
  scripts_.pop_back();
  if( scripts_.empty() )stackIsOn_ = stackSaved_;
  --currentScriptNumber_;
}

void PrepareToStopScript()
{ prepareToStopScript_ = true; }

void PrepareToExecuteScript()
{ prepareToExecuteScript_ = true; }

void PrepareToPauseScript()
{ prepareToPauseScript_ = true; }

void ShowScriptError( char const *errorMessage )
{
  ExString s = ExString("ERROR in script file: ")+scripts_[currentScriptNumber_-1]->Filename()+"\n";
  s += ExString(errorMessage)+"\n";
  s += "The following line caused the error:\n";
  s += scripts_[currentScriptNumber_-1]->CurrentLine();
  Application->MessageBox( s.c_str(), "Script error", MB_OK );
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

void PauseScripts()
{
  if( currentScriptNumber_ > 0 )
  {
    pausingScript_ = true;
    currentScriptNumber_ = 0;
  }
}

void RunScript()
{
  try
  {
    scripts_[currentScriptNumber_-1]->Run();
    if( pausingScript_ )return;
    StopScript();
  }
  catch ( runtime_error &e )
  {
    throw; 
  }
}

void RestartScripts()
{
  if( pausingScript_ )
  {
    pausingScript_ = false;
    restartingScript_ = true;
    currentScriptNumber_ = 1;
    try
    {
      RunScript();
    }
    catch ( runtime_error &e )
    {
      ExGlobals::ShowScriptError( e.what() );
      ExGlobals::StopAllScripts();
    }
  }
}

void IncrementScript()
{ ++currentScriptNumber_; }

void PreParse( ExString &line )
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
  ExString cline;
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

void ProcessCommand( ExString &commandLine )
{
  if( commandLine[0] == Script::GetCommentCharacter() )return;
  if( commandLine[0] == executeCharacter_ )
  {
    ExString s( ExString("EXECUTE ")+commandLine.substr(1,commandLine.size()-1) );
    commandLine = s;
  }
  ParseLine p( commandLine );
  try
  {
    p.ParseIt();
  }
  catch( ESyntaxError &e )
  {
    throw runtime_error( e.what() );
  }
  ExString commandName;
  if( p.GetNumberOfTokens()>1 && p.IsEqualSign(1) )
  {
    commandName = ExString("EVALUATEEXPRESSION");
  }
  else
  {
    commandName = p.GetString(0).UpperCase();
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
  catch( runtime_error const &e )
  {
    throw;
  }
  Command *command = CommandTable::GetTable()->Get(commandName);
  if( !command )
  {
    ExString s("unknown command: ");
    s += commandName;
    throw runtime_error( s.c_str() );
  }
  try
  {
    command->Execute( &p );
  }
  catch( ECommandError &e )
  {
    throw runtime_error( e.what() );
  }
  catch( EExpressionError &e )
  {
    throw runtime_error( e.what() );
  }
  if( command->GetWriteStack() )WriteStack( command->GetStackLine() );
  executeCommand_ = command->IsCommand("EXECUTE") || prepareToExecuteScript_;
  returnCommand_ = command->IsCommand("RETURN") || prepareToStopScript_;
  pausingScript_ = pausingScript_ || command->IsCommand("PAUSE") || prepareToPauseScript_;
  prepareToStopScript_ = false;
  prepareToExecuteScript_ = false;
  prepareToPauseScript_ = false;
}

bool HandleAlias( ExString &name, ExString &line, ParseLine &p )
{
  std::map<ExString,ExString>::const_iterator i = alias_.find( name );
  if( i != alias_.end() )
  {
    line = line.size()>name.size() ? (*i).second+line.substr(name.size()) : (*i).second;
    if( line[0] == Script::GetCommentCharacter() )return true;
    if( line[0] == executeCharacter_ )line = ExString("EXECUTE ")+line.substr(1);
    ParseLine p2( line );
    try
    {
      p2.ParseIt();
    }
    catch( ESyntaxError &e )
    {
      throw runtime_error( e.what() );
    }
    if( p2.GetNumberOfTokens()>1 && p2.IsEqualSign(1) )
    {
      name = ExString("EVALUATEEXPRESSION");
    }
    else
    {
      name = p2.GetString(0).UpperCase();
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

void AddAlias( ExString const &name, ExString const &value )
{
  std::map<ExString,ExString>::iterator i = alias_.find(name);
  if( i != alias_.end() )alias_.erase( i );
  alias_.insert( std::map<ExString,ExString>::value_type(name,value) );
}

bool RemoveAlias( ExString const &name )
{
  std::map<ExString,ExString>::iterator i = alias_.find(name);
  if( i != alias_.end() )
  {
    alias_.erase( i );
    return true;
  }
  return false;
}

void RemoveAliases()
{ alias_.clear(); }

void DisplayAlias( ExString const &name )
{
  if( name.empty() )return;
  std::map<ExString,ExString>::const_iterator i = alias_.find(name);
  if( i != alias_.end() )WriteOutput( ExString("ALIAS: ")+name+" -> "+(*i).second );
  else WriteOutput( name+" is not defined as an alias" );
}

void DisplayAliases()
{
  if( alias_.size() == 0 )WriteOutput("No aliases are currently defined");
  std::map<ExString,ExString>::const_iterator end = alias_.end();
  for( std::map<ExString,ExString>::const_iterator i=alias_.begin(); i!=end; ++i )
    WriteOutput( ExString("ALIAS: ")+(*i).first+" -> "+(*i).second );
}

void DefineConstants()
{
  NumericVariable::PutVariable( "alphamass", 6.64465598e-27, "alpha particle mass (kg)" );
  NumericVariable::PutVariable( "alphamassmev", 3727.37904, "alpha particle mass (MeV)" );
  NumericVariable::PutVariable( "avogadro", 6.02214199e23, "Avogadro's constant (1/mol)" );
  NumericVariable::PutVariable( "bohrradius", 0.5291772083e-10, "Bohr radius (m)" );
  NumericVariable::PutVariable( "boltzmann", 1.3806503e-23, "Boltzmann constant (J/K)" );
  NumericVariable::PutVariable( "electronradius", 2.817940285e-15, "classical electron radius (m)" );
  NumericVariable::PutVariable( "compton", 2.426310215e-12, "Compton wavelength (m)" );
  NumericVariable::PutVariable( "deuteronmass", 3.34358309e-27, "deuteron particle mass (kg)" );
  NumericVariable::PutVariable( "deuteronmassmev", 1875.612762, "deuteron particle mass (MeV)" );
  NumericVariable::PutVariable( "electronmass", 9.10938188e-31, "electron particle mass (kg)" );
  NumericVariable::PutVariable( "electronmassmev", 0.510998902, "electron particle mass (MeV)" );
  NumericVariable::PutVariable( "electronvolt", 1.602176462e-19, "electron volt (J)" );
  NumericVariable::PutVariable( "electroncharge", 1.602176462e-19, "charge on electron (C)" );
  NumericVariable::PutVariable( "faraday", 96485.3415, "Faraday constant (C/mol)" );
  NumericVariable::PutVariable( "fermicoupling", 1.16639e-5, "Fermi coupling constant (1/GeV^2)" );
  NumericVariable::PutVariable( "finestructure", 7.297352533e-3, "fine-structure constant ()" );
  NumericVariable::PutVariable( "magnetic", 12.566370614e-7, "magnetic constant (N/A^2)" );
  NumericVariable::PutVariable( "neutronmass", 1.67492716e-27, "neutron particle mass (kg)" );
  NumericVariable::PutVariable( "neutronmassmev", 939.565330, "neutron particle mass (MeV)" );
  NumericVariable::PutVariable( "gravitation", 6.673e-11, "Newtonian constant of gravitation (m^3/kg/s^2)" );
  NumericVariable::PutVariable( "planck", 6.62606876e-34, "Planck's constant (J s)" );
  NumericVariable::PutVariable( "hbar", 1.05457266e-34, "Planck's constant, reduced (J s)" );
  NumericVariable::PutVariable( "protonmass", 1.67262158e-27, "proton particle mass (kg)" );
  NumericVariable::PutVariable( "protonmassmev", 938.271998, "proton particle mass (MeV)" );
  NumericVariable::PutVariable( "rydberg", 10973731.568549, "Rydberg constant (1/m)" );
  NumericVariable::PutVariable( "lightspeed", 299792458, "speed of light in a vacuum (m/s)" );
  NumericVariable::PutVariable( "gravityacc", 9.80665, "standard acceleration of gravity (m/s^2)" );
  NumericVariable::PutVariable( "atmosphere", 101325, "standard atmosphere (Pa)" );
  NumericVariable::PutVariable( "stefanboltzmann", 5.670400e-8, "Stefan-Boltzmann constant (W/m^2/K^4)" );
  NumericVariable::PutVariable( "thomson", 0.665245854e-28, "Thomson cross section (m^2)" );
  NumericVariable::PutVariable( "amass", 1.66053873e-27, "unified atomic mass unit (kg)" );
  NumericVariable::PutVariable( "pi", 3.14159265358979323846, "pi" );
  NumericVariable::PutVariable( "earthradius", 6.378140e6, "radius of the earth at the equator (m)" );
  NumericVariable::PutVariable( "au", 1.4959787066e11, "astronomical unit (m)" );
  NumericVariable::PutVariable( "earthmass", 5.97370e24, "mass of the earth (kg)" );
  NumericVariable::PutVariable( "e", 2.718281828459045235, "natural logarithm base" );
  NumericVariable::PutVariable( "parsec", 3.0856775807e16, "parsec (m)" );
  NumericVariable::PutVariable( "sunmass", 1.98892e30, "mass of the sun (kg)" );
  NumericVariable::PutVariable( "sunradius", 6.96e8, "radius of the sun at the equator (m)" );
}

void HelpContents()
{ MainForm->HelpContents(); }

void SaveSession( ExString &fileName )
{
  ExXML xml;
  try
  {
    xml.OpenFileForWrite( fileName );
  }
  catch (...)
  {
    throw;
  }
  std::ostream &out( xml.GetStream() );
  out << "<extrema>\n";
  out << *MainForm;
  out << *MainGraphicsForm;
  out << *NVariableTable::GetTable();
  out << *TVariableTable::GetTable();
  out << *GRA_colorControl::GetColorMap();
  //
  int nPages = MainGraphicsForm->GetNumberOfPages();
  for( int i=0; i<nPages; ++i )
  {
    GRA_page *page = MainGraphicsForm->GetPage(i);
    out << *page;
  }
  out << "</extrema>" << std::endl;
}

std::vector<double> GetDoubleData( ExString const &sdata, int size )
{
  std::vector<double> data(size);
  int jStart = 0;
  for( int j=0; j<size; ++j )
  {
    int jEnd = sdata.find_first_of( " ", jStart );
    data[j] = sdata.substr(jStart,jEnd-jStart).ToDouble();
    jStart = jEnd+1;
  }
  return data;
}

std::vector<int> GetIntData( ExString const &sdata, int size )
{
  std::vector<int> data(size);
  int jStart = 0;
  for( int j=0; j<size; ++j )
  {
    int jEnd = sdata.find_first_of( " ", jStart );
    data[j] = sdata.substr(jStart,jEnd-jStart).ToInt();
    jStart = jEnd+1;
  }
  return data;
}

std::vector<bool> GetBoolData( ExString const &sdata, int size )
{
  std::vector<bool> data(size);
  int jStart = 0;
  for( int j=0; j<size; ++j )
  {
    int jEnd = sdata.find_first_of( " ", jStart );
    data[j] = sdata.substr(jStart,jEnd-jStart).ToBool();
    jStart = jEnd+1;
  }
  return data;
}

int SetCharacteristics( ExXML &xml, GRA_setOfCharacteristics *characteristics )
{
  int nodeType;
  xml.NextElementNode();
  while( xml.GetName() == "characteristic" )
  {
    ExString name( xml.GetAttributeValue("name") );
    ExString type( xml.GetAttributeValue("type") );
    ExString shape( xml.GetAttributeValue("shape") );
    if( shape == "VECTOR" )
    {
      xml.NextElementNode(); // get <data>
      int size = xml.GetAttributeValue("size").ToInt();
      ExString sdata( xml.GetTextValue() );
      if( type == "SIZE" )
        static_cast<GRA_sizeCharacteristic*>(characteristics->Get(name))->SetAsPercent( GetDoubleData(sdata,size) );
      else if( type == "DISTANCE" )
        static_cast<GRA_distanceCharacteristic*>(characteristics->Get(name))->SetAsPercent( GetDoubleData(sdata,size) );
      else if( type == "INT" )
        static_cast<GRA_intCharacteristic*>(characteristics->Get(name))->Set( GetIntData(sdata,size) );
      else if( type == "DOUBLE" )
        static_cast<GRA_doubleCharacteristic*>(characteristics->Get(name))->Set( GetDoubleData(sdata,size) );
      else if( type == "BOOL" )
        static_cast<GRA_boolCharacteristic*>(characteristics->Get(name))->Set( GetBoolData(sdata,size) );
      else if( type == "ANGLE" )
        static_cast<GRA_angleCharacteristic*>(characteristics->Get(name))->Set( GetDoubleData(sdata,size) );
      else if( type == "FONT" )
        static_cast<GRA_fontCharacteristic*>(characteristics->Get(name))->Set( GetIntData(sdata,size) );
      else if( type == "COLOR" )
        static_cast<GRA_colorCharacteristic*>(characteristics->Get(name))->Set( GetIntData(sdata,size) );
    }
    else
    {
      if( type == "SIZE" )
        static_cast<GRA_sizeCharacteristic*>(characteristics->
          Get(name))->SetAsPercent( xml.GetAttributeValue("value").ToDouble() );
      else if( type == "DISTANCE" )
        static_cast<GRA_distanceCharacteristic*>(characteristics->
          Get(name))->SetAsPercent( xml.GetAttributeValue("value").ToDouble() );
      else if( type == "INT" )
        static_cast<GRA_intCharacteristic*>(characteristics->
          Get(name))->Set( xml.GetAttributeValue("value").ToInt() );
      else if( type == "DOUBLE" )
        static_cast<GRA_doubleCharacteristic*>(characteristics->
          Get(name))->Set( xml.GetAttributeValue("value").ToDouble() );
      else if( type == "BOOL" )
        static_cast<GRA_boolCharacteristic*>(characteristics->
          Get(name))->Set( xml.GetAttributeValue("value").ToBool() );
      else if( type == "ANGLE" )
        static_cast<GRA_angleCharacteristic*>(characteristics->
          Get(name))->Set( xml.GetAttributeValue("value").ToDouble() );
      else if( type == "FONT" )
        static_cast<GRA_fontCharacteristic*>(characteristics->
          Get(name))->Set( GRA_fontControl::GetFont(xml.GetAttributeValue("value").ToInt()) );
      else if( type == "COLOR" )
        static_cast<GRA_colorCharacteristic*>(characteristics->
          Get(name))->Set( GRA_colorControl::GetColor(xml.GetAttributeValue("value").ToInt()) );
    }
    nodeType = xml.NextElementNode();
  }
  return nodeType;
}

void RestoreSession( ExString &fileName )
{
  ExXML xml;
  try
  {
    xml.OpenFileForRead( fileName );
  }
  catch (...)
  {
    throw;
  }
  // NextElementNode() returns 0 if no more nodes
  //                   returns -1 if an error occurred
  //
  if( xml.NextElementNode() == -1 )return; // this should be the <extrema> node
  std::ostream &out( xml.GetStream() );
  //
  MainGraphicsForm->DeleteAllPages();
  MainForm->ClearMainListBox();
  MainForm->CloseSubforms();
  //
  xml.NextElementNode(); // get <analysiswindow>
  if( xml.GetName() != "analysiswindow" )
    throw runtime_error("xml is invalid: node should be <analysiswindow>");
  MainForm->SetDimensions( xml.GetAttributeValue("top").ToInt(),
                           xml.GetAttributeValue("left").ToInt(),
                           xml.GetAttributeValue("height").ToInt(),
                           xml.GetAttributeValue("width").ToInt() );
  SetNoviceMode( xml.GetAttributeValue("novicemode").ToBool() );
  if( xml.GetAttributeValue("varshow").ToBool() )MainForm->CreateVarShowForm( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("readvec").ToBool() )MainForm->CreateReadVecForm( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("readscal").ToBool() )MainForm->CreateReadScalForm( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("readmat").ToBool() )MainForm->CreateReadMatForm( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("readtxt").ToBool() )MainForm->CreateReadTxtForm( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("exec").ToBool() )MainForm->CreateExecForm( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("writevec").ToBool() )MainForm->CreateWriteVecForm( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("writescal").ToBool() )MainForm->CreateWriteScalForm( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("writemat").ToBool() )MainForm->CreateWriteMatForm( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("writetxt").ToBool() )MainForm->CreateWriteTxtForm( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("fit").ToBool() )MainForm->CreateFitForm( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("generate").ToBool() )MainForm->CreateGenerateCommandForm( reinterpret_cast<TObject*>(0) );
  //
  xml.NextElementNode(); // get the <messages> node
  bool messagesFound = false;
  if( xml.GetName() == "messages" )
  {
    messagesFound = true;
    int size = xml.GetAttributeValue("size").ToInt();
    for( int i=0; i<size; ++i )
    {
      xml.NextElementNode();  // get <string>
      MainForm->WriteOutput( xml.GetTextValue() );
    }
  }
  if( messagesFound )xml.NextElementNode();
  bool commandsFound = false;
  if( xml.GetName() == "commands" )
  {
    commandsFound = true;
    int size = xml.GetAttributeValue("size").ToInt();
    for( int i=0; i<size; ++i )
    {
      xml.NextElementNode();  // get <string>
      MainForm->AddCommandString( xml.GetTextValue() );
    }
  }
  if( commandsFound )xml.NextElementNode(); // get the <visualizationwindow> node
  if( xml.GetName() != "visualizationwindow" )
    throw runtime_error("xml is invalid: node should be <visualizationwindow>");
  MainGraphicsForm->SetDimensions( xml.GetAttributeValue("top").ToInt(),
                                   xml.GetAttributeValue("left").ToInt() );
  //
  int width = xml.GetAttributeValue("width").ToInt();
  int height = xml.GetAttributeValue("height").ToInt();
  int currentPage = xml.GetAttributeValue("currentpage").ToInt();
  double aspectRatio = xml.GetAttributeValue("aspectratio").ToDouble();
  MainGraphicsForm->MakeFirstPage();
  SetMonitorLimits( 0, 0, width, height );
  ChangeAspectRatio( aspectRatio );
  //MainGraphicsForm->SetImageSize();
  if( xml.GetAttributeValue("figure").ToBool() )MainGraphicsForm->FiguresSpeedButtonClick( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("peak").ToBool() )MainGraphicsForm->PeakSpeedButtonClick( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("graph").ToBool() )MainGraphicsForm->GraphClick( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("subw").ToBool() )MainGraphicsForm->GSubwindowClick( reinterpret_cast<TObject*>(0) );
  if( xml.GetAttributeValue("text").ToBool() )MainGraphicsForm->TextCommandClick( reinterpret_cast<TObject*>(0) );
  //
  NVariableTable *nvt = NVariableTable::GetTable();
  TVariableTable *tvt = TVariableTable::GetTable();
  //
  nvt->ClearTable(); // delete all numeric variables
  tvt->ClearTable(); // delete all character variables
  //
  xml.NextElementNode(); // get the <numericvariables> node
  bool numericVariablesFound = false;
  if( xml.GetName() == "numericvariables" )
  {
    numericVariablesFound = true;
    int size = xml.GetAttributeValue("size").ToInt(); // number of numeric variables
    for( int i=0; i<size; ++i )
    {
      xml.NextElementNode();
      if( xml.GetName() == "scalar" )
      {
        ExString name( xml.GetAttributeValue("name") );
        ExString type( xml.GetAttributeValue("type") );
        xml.NextElementNode(); // get <data>
        double value = xml.GetTextValue().ToDouble();
        xml.NextElementNode(); // get <history>
        int nHist = xml.GetAttributeValue("size").ToInt(); // number of history strings
        std::deque<ExString> history;
        for( int j=0; j<nHist; ++j )
        {
          xml.NextElementNode(); // get <string>
          history.push_front( xml.GetTextValue() );
        }
        NumericVariable *nv = NumericVariable::PutVariable( name, value, ExString(""), false );
        nv->SetHistory( history );
        xml.NextElementNode(); // get <origin>
        ExString origin( xml.GetTextValue() );
        if( !origin.empty() )nv->SetOrigin( origin );
        if( type == "FIT" )nv->GetData().SetFit();
      }
      else if( xml.GetName() == "vector" )
      {
        ExString name( xml.GetAttributeValue("name") );
        ExString orderS( xml.GetAttributeValue("order") );
        int order = 0;
        if( orderS == "ASCENDING" )order = 1;
        else if( orderS == "DESCENDING" )order = 2;
        xml.NextElementNode(); // get <data>
        int size = xml.GetAttributeValue("size").ToInt();
        std::vector<double> data( GetDoubleData(xml.GetTextValue(),size) );
        xml.NextElementNode(); // get <history>
        int nHist = xml.GetAttributeValue("size").ToInt(); // number of history strings
        std::deque<ExString> history;
        for( int j=0; j<nHist; ++j )
        {
          xml.NextElementNode(); // get <string>
          history.push_front( xml.GetTextValue() );
        }
        NumericVariable *nv = NumericVariable::PutVariable( name, data, order, ExString(""), false );
        nv->SetHistory( history );
        xml.NextElementNode(); // get <origin>
        ExString origin( xml.GetTextValue() );
        if( !origin.empty() )nv->SetOrigin( origin );
      }
      else if( xml.GetName() == "matrix" )
      {
        ExString name( xml.GetAttributeValue("name") );
        int rows = xml.GetAttributeValue("rows").ToInt();
        int columns = xml.GetAttributeValue("columns").ToInt();
        xml.NextElementNode(); // get <data>
        int size = xml.GetAttributeValue("size").ToInt();
        std::vector<double> data( GetDoubleData(xml.GetTextValue(),size) );
        xml.NextElementNode(); // get <history>
        int nHist = xml.GetAttributeValue("size").ToInt(); // number of history strings
        std::deque<ExString> history;
        for( int j=0; j<nHist; ++j )
        {
          xml.NextElementNode(); // get <string>
          history.push_front( xml.GetTextValue() );
        }
        NumericVariable *nv = NumericVariable::PutVariable( name, data, rows, columns, ExString(""), false );
        nv->SetHistory( history );
        xml.NextElementNode(); // get <origin>
        ExString origin( xml.GetTextValue() );
        if( !origin.empty() )nv->SetOrigin( origin );
      }
      else if( xml.GetName() == "tensor" )
      {
        ExString name( xml.GetAttributeValue("name") );
        int rows = xml.GetAttributeValue("rows").ToInt();
        int columns = xml.GetAttributeValue("columns").ToInt();
        int planes = xml.GetAttributeValue("planes").ToInt();
        xml.NextElementNode(); // get <data>
        int size = xml.GetAttributeValue("size").ToInt();
        std::vector<double> data( GetDoubleData(xml.GetTextValue(),size) );
        xml.NextElementNode(); // get <history>
        int nHist = xml.GetAttributeValue("size").ToInt(); // number of history strings
        std::deque<ExString> history;
        for( int j=0; j<nHist; ++j )
        {
          xml.NextElementNode(); // get <string>
          history.push_front( xml.GetTextValue() );
        }
        //NumericVariable *nv = NumericVariable::PutVariable( name, data, rows, columns, planes, "", false );
        //nv->SetHistory( history );
        //xml.NextElementNode(); // get <origin>
        //ExString origin( xml.GetTextValue() );
        //if( !origin.empty() )nv->SetOrigin( origin );
      }
    }
  }
  if( numericVariablesFound )xml.NextElementNode(); // get <textvariables>
  bool textVariablesFound = false;
  if( xml.GetName() == "textvariables" )
  {
    textVariablesFound = true;
    int size = xml.GetAttributeValue("size").ToInt(); // number of text variables
    for( int i=0; i<size; ++i )
    {
      xml.NextElementNode();
      if( xml.GetName() == "textscalar" )
      {
        ExString name( xml.GetAttributeValue("name") );
        xml.NextElementNode(); // get <string>
        ExString value = xml.GetTextValue();
        xml.NextElementNode(); // get <history>
        int nHist = xml.GetAttributeValue("size").ToInt(); // number of history strings
        std::deque<ExString> history;
        for( int j=0; j<nHist; ++j )
        {
          xml.NextElementNode(); // get <string>
          history.push_front( xml.GetTextValue() );
        }
        TextVariable *tv = TextVariable::PutVariable( name, value, ExString(""), false );
        tv->SetHistory( history );
        xml.NextElementNode(); // get <origin>
        ExString origin( xml.GetTextValue() );
        if( !origin.empty() )tv->SetOrigin( origin );
      }
      else if( xml.GetName() == "textvector" )
      {
        ExString name( xml.GetAttributeValue("name") );
        int size = xml.GetAttributeValue("size").ToInt();
        std::vector<ExString> strings(size);
        for( int j=0; j<size; ++j )
        {
          xml.NextElementNode(); // get <string>
          strings[j] = xml.GetTextValue();
        }
        xml.NextElementNode(); // get <history>
        int nHist = xml.GetAttributeValue("size").ToInt(); // number of history strings
        std::deque<ExString> history;
        for( int j=0; j<nHist; ++j )
        {
          xml.NextElementNode(); // get <string>
          history.push_front( xml.GetTextValue() );
        }
        TextVariable *tv = TextVariable::PutVariable( name, strings, ExString(""), false );
        tv->SetHistory( history );
        xml.NextElementNode(); // get <origin>
        ExString origin( xml.GetTextValue() );
        if( !origin.empty() )tv->SetOrigin( origin );
      }
    }
  }
  if( textVariablesFound )xml.NextElementNode();
  if( xml.GetName() != "colormap" )
    throw runtime_error("xml is invalid: node should be <colormap>");
  ExString name( xml.GetAttributeValue("name") );
  if( name == "USERDEFINED" )
  {
    GRA_colorMap *tmp = new GRA_colorMap( "USERDEFINED" );
    int size = xml.GetAttributeValue("size").ToInt(); // number of colors
    for( int i=0; i<size; ++i )
    {
      xml.NextElementNode(); // get <color> node
      tmp->AddColor( new GRA_color(xml.GetAttributeValue("r").ToInt(),
                                   xml.GetAttributeValue("g").ToInt(),
                                   xml.GetAttributeValue("b").ToInt()) );
    }
    GRA_colorControl::SetColorMapUser( tmp );
  }
  else
  {
    try
    {
      GRA_colorControl::SetColorMap( name );
    }
    catch (EGraphicsError const &e)
    {
      throw runtime_error( e.what() );
    }
  }
  int nodeType = xml.NextElementNode(); // get <graphicspage>
  int pageCounter = 0;
  while( nodeType == 1 )
  {
    if( xml.GetName() == "" )break;
    if( xml.GetName() != "graphicspage" )
    {
      ExString s("xml is invalid: ");
      s += "<"+xml.GetName()+"> node encountered, expecting <graphicspage>";
      throw runtime_error( s.c_str() );
    }
    // first page already made above
    // only make new page if more than one page
    //
    if( ++pageCounter > 1 )MainGraphicsForm->NewPage( pageCounter );
    int windowNumber = xml.GetAttributeValue("currentwindow").ToInt();
    nodeType = xml.NextElementNode(); // get <graphwindow>
    while( nodeType == 1 )
    {
      if( xml.GetName() == "graphicspage" )break;
      if( xml.GetName() != "graphwindow" )
      {
        ExString s("xml is invalid: ");
        s += "<"+xml.GetName()+"> node encountered, expecting <graphwindow>";
        throw runtime_error( s.c_str() );
      }
      int number = xml.GetAttributeValue("number").ToInt();
      double xLoP = xml.GetAttributeValue("xLoP").ToDouble();
      double yLoP = xml.GetAttributeValue("yLoP").ToDouble();
      double xHiP = xml.GetAttributeValue("xHiP").ToDouble();
      double yHiP = xml.GetAttributeValue("yHiP").ToDouble();
      double xPrevious = xml.GetAttributeValue("xPrevious").ToDouble();
      double yPrevious = xml.GetAttributeValue("yPrevious").ToDouble();
      GRA_window *gw = new GRA_window( number, xLoP, yLoP, xHiP, yHiP );
      MainGraphicsForm->AddGraphWindow( gw );
      xml.NextElementNode(); // get <axisc>
      if( xml.GetName() != "axisc" )
        throw runtime_error("xml is invalid: node should be <axisc>");
      nodeType = SetCharacteristics( xml, gw->GetXAxisCharacteristics() );
      if( xml.GetName() != "axisc" )
        throw runtime_error("xml is invalid: node should be <axisc>");
      nodeType = SetCharacteristics( xml, gw->GetYAxisCharacteristics() );
      if( xml.GetName() != "generalc" )
        throw runtime_error("xml is invalid: node should be <generalc>");
      nodeType = SetCharacteristics( xml, gw->GetGeneralCharacteristics() );
      if( xml.GetName() != "textc" )
        throw runtime_error("xml is invalid: node should be <textc>");
      nodeType = SetCharacteristics( xml, gw->GetTextCharacteristics() );
      if( xml.GetName() != "graphlegendc" )
        throw runtime_error("xml is invalid: node should be <graphlegendc>");
      nodeType = SetCharacteristics( xml, gw->GetGraphLegendCharacteristics() );
      if( xml.GetName() != "datacurvec" )
        throw runtime_error("xml is invalid: node should be <datacurvec>");
      nodeType = SetCharacteristics( xml, gw->GetDataCurveCharacteristics() );
      if( xml.GetName() != "polarc" )
        throw runtime_error("xml is invalid: node should be <polarc>");
      nodeType = SetCharacteristics( xml, gw->GetPolarCharacteristics() );
      if( xml.GetName() != "drawableobjects" )
        throw runtime_error("xml is invalid: node should be <drawableobjects>");
      int size = xml.GetAttributeValue("size").ToInt();
      for( int i=0; i<size; ++i )
      {
        nodeType = xml.NextElementNode();
        if( xml.GetName() == "point" )
        {
          GRA_point *point = GetPoint( xml );
          screenOutput_->Draw( point );
          gw->AddDrawableObject( point );
        }
        else if( xml.GetName() == "polyline" )
        {
          GRA_polyline *pline = GetPolyline( xml );
          screenOutput_->Draw( pline );
          gw->AddDrawableObject( pline );
        }
        else if( xml.GetName() == "ellipse" )
        {
          GRA_ellipse *ellipse = GetEllipse( xml );
          screenOutput_->Draw( ellipse );
          gw->AddDrawableObject( ellipse );
        }
        else if( xml.GetName() == "polygon" )
        {
          GRA_polygon *poly = GetPolygon( xml );
          screenOutput_->Draw( poly );
          gw->AddDrawableObject( poly );
        }
        else if( xml.GetName() == "multilinefigure" )
        {
          GRA_multiLineFigure *multi = GetMultiLineFigure( xml );
          screenOutput_->Draw( multi );
          gw->AddDrawableObject( multi );
        }
        else if( xml.GetName() == "plotsymbol" )
        {
          GRA_plotSymbol *ps = GetPlotSymbol( xml );
          screenOutput_->Draw( ps );
          gw->AddDrawableObject( ps );
        }
        else if( xml.GetName() == "drawabletext" )
        {
          GRA_drawableText *dt = GetDrawableText( xml );
          screenOutput_->Draw( dt );
          gw->AddDrawableObject( dt );
        }
        else if( xml.GetName() == "cartesianaxes" )
        {
          GRA_cartesianAxes *axes = GetCartesianAxes( xml );
          axes->Draw( screenOutput_ );
          gw->AddDrawableObject( axes );
        }
        else if( xml.GetName() == "cartesiancurve" )
        {
          GRA_cartesianCurve *curve = GetCartesianCurve( xml );
          curve->Draw( screenOutput_ );
          gw->AddDrawableObject( curve );
        }
        else if( xml.GetName() == "polaraxes" )
        {
          GRA_polarAxes *axes = GetPolarAxes( xml );
          axes->Draw( screenOutput_ );
          gw->AddDrawableObject( axes );
        }
        else if( xml.GetName() == "polarcurve" )
        {
          GRA_polarCurve *curve = GetPolarCurve( xml );
          curve->Draw( screenOutput_ );
          gw->AddDrawableObject( curve );
        }
        else if( xml.GetName() == "graphlegend" )
        {
          // this really needs to be fixed up
          //
          int size = xml.GetAttributeValue("size").ToInt();
          bool frameOn = xml.GetAttributeValue("frameon").ToBool();
          bool titleOn = xml.GetAttributeValue("titleon").ToBool();
          double xlo = xml.GetAttributeValue("xlo").ToDouble();
          double ylo = xml.GetAttributeValue("ylo").ToDouble();
          double xhi = xml.GetAttributeValue("xhi").ToDouble();
          double yhi = xml.GetAttributeValue("yhi").ToDouble();

          GRA_setOfCharacteristics *legendC = gw->GetGraphLegendCharacteristics();
          static_cast<GRA_distanceCharacteristic*>(legendC->Get("FRAMEXLO"))->SetAsWorld( xlo );
          static_cast<GRA_distanceCharacteristic*>(legendC->Get("FRAMEYLO"))->SetAsWorld( ylo );
          static_cast<GRA_distanceCharacteristic*>(legendC->Get("FRAMEXHI"))->SetAsWorld( xhi );
          static_cast<GRA_distanceCharacteristic*>(legendC->Get("FRAMEYHI"))->SetAsWorld( yhi );

          static_cast<GRA_boolCharacteristic*>(legendC->Get("FRAMEON"))->Set( frameOn );
          static_cast<GRA_boolCharacteristic*>(legendC->Get("TITLEON"))->Set( titleOn );

          GRA_legend *gl = gw->GetGraphLegend();
          double xstart = gl->GetLineStart();
          double xend = gl->GetLineEnd();
          double xlabel = gl->GetXLabel();

          for( int j=0; j<size; ++j )
          {
            xml.NextElementNode(); // get <legendentry>
            int nSymbols = xml.GetAttributeValue("nsymbols").ToInt();
            bool line = xml.GetAttributeValue("line").ToBool();
            int lt = xml.GetAttributeValue("linetype").ToInt();
            int lw = xml.GetAttributeValue("linewidth").ToInt();
            int lc = xml.GetAttributeValue("linecolor").ToInt();
            static_cast<GRA_intCharacteristic*>(legendC->Get("SYMBOLS"))->Set( nSymbols );
            GRA_color *lineColor = GRA_colorControl::GetColor( lc );

            xml.NextElementNode(); // get <label>
            GRA_drawableText *labelDT = GetDrawableText( xml );
            ExString label( labelDT->GetString() );
            double labelHeight = labelDT->GetHeight();
            GRA_font *labelFont = labelDT->GetFont();
            GRA_color *labelColor = labelDT->GetColor();

            xml.NextElementNode(); // get <plotsymbol>
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
              throw;
            }
            gl->AddEntry( entry );
          }
          xml.NextElementNode(); // get <frame>
          gl->SetFrame( static_cast<GRA_rectangle*>(GetPolygon(xml)) );
          xml.NextElementNode(); // get <title>
          gl->SetTitle( GetDrawableText(xml) );
          gl->Draw( screenOutput_ );
        }
        else if( xml.GetName() == "contour" )
        {
          GRA_contourLine *contour = GetContour( xml );
          contour->Draw( screenOutput_ );
          gw->AddDrawableObject( contour );
        }
        else if( xml.GetName() == "boxplot" )
        {
          GRA_boxPlot *bp = GetBoxPlot( xml );
          bp->Make();
          bp->Draw( screenOutput_ );
          gw->AddDrawableObject( bp );
        }
        else if( xml.GetName() == "diffusionplot" )
        {
          GRA_diffusionPlot *dp = GetDiffusionPlot( xml );
          dp->Make();
          dp->Draw( screenOutput_ );
          gw->AddDrawableObject( dp );
        }
        else if( xml.GetName() == "ditheringplot" )
        {
          GRA_ditheringPlot *dp = GetDitheringPlot( xml );
          dp->Make();
          dp->Draw( screenOutput_ );
          gw->AddDrawableObject( dp );
        }
        else if( xml.GetName() == "gradientplot" )
        {
          GRA_gradientPlot *gp = GetGradientPlot( xml );
          gp->Make();
          gp->Draw( screenOutput_ );
          gw->AddDrawableObject( gp );
        }
        else if( xml.GetName() == "threedfigure" )
        {
        }
        else
        {
          ExString s("xml is invalid: unknown ");
          s += "<"+xml.GetName()+"> node encountered";
          throw runtime_error( s.c_str() );
        }
      }
      MainGraphicsForm->SetWindowNumber( windowNumber );
      nodeType = xml.NextElementNode(); // get <graphwindow>
    }
  }
}

GRA_point *GetPoint( ExXML &xml )
{
  double x = xml.GetAttributeValue("x").ToDouble();
  double y = xml.GetAttributeValue("y").ToDouble();
  int c = xml.GetAttributeValue("color").ToInt();
  return new GRA_point( x, y, GRA_colorControl::GetColor(c) );
}

GRA_polyline *GetPolyline( ExXML &xml )
{
  int lw = xml.GetAttributeValue("linewidth").ToInt();
  int lt = xml.GetAttributeValue("linetype").ToInt();
  int lc = xml.GetAttributeValue("linecolor").ToInt();
  xml.NextElementNode(); // get <data>
  int size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> x(size), y(size), data(GetDoubleData(xml.GetTextValue(),3*size));
  std::vector<int> pen(size);
  for( int j=0; j<size; ++j )
  {
    x[j] = data[3*j];
    y[j] = data[3*j+1];
    pen[j] = static_cast<int>(data[3*j+2]);
  }
  return new GRA_polyline( x, y, pen, lw, lt, GRA_colorControl::GetColor(lc) );
}

GRA_multiLineFigure *GetMultiLineFigure( ExXML &xml )
{
  int lw = xml.GetAttributeValue("linewidth").ToInt();
  int lt = xml.GetAttributeValue("linetype").ToInt();
  int lc = xml.GetAttributeValue("linecolor").ToInt();
  xml.NextElementNode(); // get <data>
  int size = xml.GetAttributeValue("size").ToInt();
  ExString sdata( xml.GetTextValue() );
  std::vector<double> x(size), y(size);
  std::vector<int> pen(size);
  int jStart = 0;
  for( int j=0; j<size; ++j )
  {
    int jEnd = sdata.find_first_of( " ", jStart );
    x[j] = sdata.substr(jStart,jEnd-jStart).ToDouble();
    jStart = jEnd+1;
    jEnd = sdata.find_first_of( " ", jStart );
    y[j] = sdata.substr(jStart,jEnd-jStart).ToDouble();
    jStart = jEnd+1;
    jEnd = sdata.find_first_of( " ", jStart );
    pen[j] = sdata.substr(jStart,jEnd-jStart).ToInt();
    jStart = jEnd+1;
  }
  return new GRA_multiLineFigure( x, y, pen, GRA_colorControl::GetColor(lc), lw, lt );
}

GRA_errorBar *GetErrorBar( ExXML &xml )
{
  double x = xml.GetAttributeValue("x").ToDouble();
  double y = xml.GetAttributeValue("y").ToDouble();
  int lw = xml.GetAttributeValue("linewidth").ToInt();
  int lc = xml.GetAttributeValue("linecolor").ToInt();
  double bottom = xml.GetAttributeValue("bottom").ToDouble();
  double top = xml.GetAttributeValue("top").ToDouble();
  double footsize = xml.GetAttributeValue("footsize").ToDouble();
  bool vertical = xml.GetAttributeValue("vertical").ToBool();
  return new GRA_errorBar( x, y, bottom, top, vertical, footsize,
                           GRA_colorControl::GetColor(lc), lw );
}

GRA_polygon *GetPolygon( ExXML &xml )
{
  int lw = xml.GetAttributeValue("linewidth").ToInt();
  int lt = xml.GetAttributeValue("linetype").ToInt();
  int lc = xml.GetAttributeValue("linecolor").ToInt();
  int fc = xml.GetAttributeValue("fillcolor").ToInt();
  xml.NextElementNode(); // get <data>
  int size = xml.GetAttributeValue("size").ToInt();
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
  double xmin = xml.GetAttributeValue("xmin").ToDouble();
  double ymin = xml.GetAttributeValue("ymin").ToDouble();
  double xmax = xml.GetAttributeValue("xmax").ToDouble();
  double ymax = xml.GetAttributeValue("ymax").ToDouble();
  bool circle = xml.GetAttributeValue("circle").ToBool();
  int lc = xml.GetAttributeValue("linecolor").ToInt();
  int fc = xml.GetAttributeValue("fillcolor").ToInt();
  int lw = xml.GetAttributeValue("linewidth").ToInt();
  int lt = xml.GetAttributeValue("linetype").ToInt();
  return new GRA_ellipse( xmin, ymin, xmax, ymax, circle,
                          GRA_colorControl::GetColor(lc), GRA_colorControl::GetColor(fc),
                          lw, lt );
}

GRA_plotSymbol *GetPlotSymbol( ExXML &xml )
{
  double x = xml.GetAttributeValue("x").ToDouble();
  double y = xml.GetAttributeValue("y").ToDouble();
  int code = xml.GetAttributeValue("shape").ToInt();
  double angle = xml.GetAttributeValue("angle").ToDouble();
  double size = xml.GetAttributeValue("size").ToDouble();
  int color = xml.GetAttributeValue("color").ToInt();
  int lw = xml.GetAttributeValue("linewidth").ToInt();
  GRA_plotSymbol *ps = new GRA_plotSymbol( code, size, angle,
    GRA_colorControl::GetColor(color), lw );
  ps->SetLocation( x, y );
  return ps;
}

GRA_drawableText *GetDrawableText( ExXML &xml )
{
  double x = xml.GetAttributeValue("x").ToDouble();
  double y = xml.GetAttributeValue("y").ToDouble();
  double height = xml.GetAttributeValue("height").ToDouble();
  double angle = xml.GetAttributeValue("angle").ToDouble();
  int alignment = xml.GetAttributeValue("alignment").ToInt();
  int color = xml.GetAttributeValue("color").ToInt();
  int font = xml.GetAttributeValue("font").ToInt();
  bool graphunits = xml.GetAttributeValue("graphunits").ToBool();
  xml.NextElementNode(); // get <string>
  GRA_drawableText *dt = new GRA_drawableText( xml.GetTextValue(), height,
    angle, x, y, alignment, GRA_fontControl::GetFont(font),
    GRA_colorControl::GetColor(color) );
  dt->Parse();
  return dt;
}

GRA_cartesianAxes *GetCartesianAxes( ExXML &xml )
{
  int size = xml.GetAttributeValue("size").ToInt();
  xml.NextElementNode(); // get x <axis>
  GRA_axis *xaxis = GetAxis( xml, 'X' );
  xml.NextElementNode(); // get y <axis>
  GRA_axis *yaxis = GetAxis( xml, 'Y' );
  GRA_axis *boxX =0, *boxY =0;
  if( size >= 3 )
  {
    xml.NextElementNode(); // get box x-axis
    boxX = GetAxis( xml, 'X' );
    if( size == 4 )
    {
      xml.NextElementNode(); // get box y-axis
      boxY = GetAxis( xml, 'Y' );
    }
  }
  GRA_cartesianAxes *axes = new GRA_cartesianAxes();
  axes->SetAxes( xaxis, yaxis, boxX, boxY );
  return axes;
}

GRA_polarAxes *GetPolarAxes( ExXML &xml )
{
  xml.NextElementNode(); // get <axis>
  GRA_axis *axis = GetAxis( xml, 'X' );
  GRA_polarAxes *axes = new GRA_polarAxes();
  axes->SetAxes( axis );
  return axes;
}

GRA_axis *GetAxis( ExXML &xml, char const type )
{
  double const xOrigin = xml.GetAttributeValue("xorigin").ToDouble();
  double const yOrigin = xml.GetAttributeValue("yorigin").ToDouble();
  double const length = xml.GetAttributeValue("length").ToDouble();
  xml.NextElementNode(); // get <ticcoordinates>
  int size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> xtics(size), ytics(size), data(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    xtics[j] = data[2*j];
    ytics[j] = data[2*j+1];
  }
  xml.NextElementNode(); // get <axisc>
  //
  GRA_window *gw = MainGraphicsForm->GetGraphWindow();
  GRA_setOfCharacteristics *xAxisC = gw->GetXAxisCharacteristics();
  GRA_setOfCharacteristics *yAxisC = gw->GetYAxisCharacteristics();
  GRA_setOfCharacteristics *polarC = gw->GetPolarCharacteristics();
  GRA_setOfCharacteristics axisc;
  switch (type)
  {
    case 'X':
      axisc = *xAxisC;
      break;
    case 'Y':
      axisc = *yAxisC;
      break;
    case 'P':
      axisc = *polarC;
      break;
  }
  SetCharacteristics( xml, &axisc );
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<GRA_polyline*> polylines(size);
  for( int j=0; j<size; ++j )
  {
    xml.NextElementNode(); // get <polyline>
    polylines[j] = GetPolyline(xml);
  }
  xml.NextElementNode(); // get <drawabletexts>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<GRA_drawableText*> textVec(size);
  for( int j=0; j<size; ++j )
  {
    xml.NextElementNode(); // get <drawabletext>
    textVec[j] = GetDrawableText(xml);
  }
  GRA_axis *axis = new GRA_axis( xOrigin, yOrigin, length, &axisc );
  axis->SetTicCoordinates( xtics, ytics );
  axis->SetPolylines( polylines );
  axis->SetDrawableText( textVec );
  return axis;
}

GRA_cartesianCurve *GetCartesianCurve( ExXML &xml )
{
  int histype = xml.GetAttributeValue("histype").ToInt();
  int linetype = xml.GetAttributeValue("linetype").ToInt();
  int linewidth = xml.GetAttributeValue("linewidth").ToInt();
  bool smooth = xml.GetAttributeValue("smooth").ToBool();
  GRA_color *linecolor = GRA_colorControl::GetColor(xml.GetAttributeValue("linecolor").ToInt());
  int tmp = xml.GetAttributeValue("areafillcolor").ToInt();
  GRA_color *areafillcolor = 0;
  if( tmp != 0 )areafillcolor = GRA_colorControl::GetColor(tmp);
  double xlaxis = xml.GetAttributeValue("xlaxis").ToDouble();
  double ylaxis = xml.GetAttributeValue("ylaxis").ToDouble();
  double xuaxis = xml.GetAttributeValue("xuaxis").ToDouble();
  double yuaxis = xml.GetAttributeValue("yuaxis").ToDouble();
  double xmin = xml.GetAttributeValue("xmin").ToDouble();
  double ymin = xml.GetAttributeValue("ymin").ToDouble();
  double xmax = xml.GetAttributeValue("xmax").ToDouble();
  double ymax = xml.GetAttributeValue("ymax").ToDouble();
  xml.NextElementNode(); // get <areafillcolors>
  int size = xml.GetAttributeValue("size").ToInt();
  std::vector<GRA_color*> areaFillColors(size);
  std::vector<int> ivec( GetIntData(xml.GetTextValue(),size) );
  for( int j=0; j<size; ++j )
    areaFillColors[j] = GRA_colorControl::GetColor(ivec[j]);
  xml.NextElementNode(); // get <data>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> x(size), y(size), data(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    x[j] = data[2*j];
    y[j] = data[2*j+1];
  }
  xml.NextElementNode(); // get <xe1>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> xe1( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <xe2>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> xe2( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <ye1>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> ye1( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <ye2>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> ye2( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <xycurve>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> xcurve(size), ycurve(size), data2(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    xcurve[j] = data2[2*j];
    ycurve[j] = data2[2*j+1];
  }
  xml.NextElementNode(); // get <pen>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<int> pen( GetIntData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <plotsymbols>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<GRA_plotSymbol*> symbols(size);
  for( int j=0; j<size; ++j )
  {
    xml.NextElementNode(); // get <plotsymbol>
    symbols[j] = GetPlotSymbol( xml );
  }
  xml.NextElementNode(); // get <errorbars>
  size = xml.GetAttributeValue("size").ToInt();
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

GRA_polarCurve *GetPolarCurve( ExXML &xml )
{
  int histype = xml.GetAttributeValue("histype").ToInt();
  int linetype = xml.GetAttributeValue("linetype").ToInt();
  int linewidth = xml.GetAttributeValue("linewidth").ToInt();
  GRA_color *linecolor = GRA_colorControl::GetColor(xml.GetAttributeValue("linecolor").ToInt());
  int tmp = xml.GetAttributeValue("areafillcolor").ToInt();
  GRA_color *areafillcolor = 0;
  if( tmp != 0 )areafillcolor = GRA_colorControl::GetColor(tmp);
  double xmax = xml.GetAttributeValue("xmax").ToDouble();
  xml.NextElementNode(); // get <areafillcolors>
  int size = xml.GetAttributeValue("size").ToInt();
  std::vector<GRA_color*> areaFillColors(size);
  std::vector<int> ivec( GetIntData(xml.GetTextValue(),size) );
  for( int j=0; j<size; ++j )
    areaFillColors[j] = GRA_colorControl::GetColor(ivec[j]);
  xml.NextElementNode(); // get <data>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> x(size), y(size), data(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    x[j] = data[2*j];
    y[j] = data[2*j+1];
  }
  xml.NextElementNode(); // get <xycurve>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> xcurve(size), ycurve(size), data2(GetDoubleData(xml.GetTextValue(),2*size));
  for( int j=0; j<size; ++j )
  {
    xcurve[j] = data2[2*j];
    ycurve[j] = data2[2*j+1];
  }
  xml.NextElementNode(); // get <pen>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<int> pen( GetIntData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <plotsymbols>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<GRA_plotSymbol*> symbols(size);
  for( int j=0; j<size; ++j )
  {
    xml.NextElementNode(); // get <plotsymbol>
    symbols[j] = GetPlotSymbol( xml );
  }
  GRA_polarCurve *curve = new GRA_polarCurve( x, y );
  curve->SetValues( histype, linetype, linewidth, xmax, linecolor, areafillcolor,
                    areaFillColors, xcurve, ycurve, pen, symbols );
  return curve;
}

GRA_contourLine *GetContour( ExXML &xml )
{
  /*
  int color = xml.GetAttributeValue("color").ToInt();
  xml.NextElementNode(); // get <data>
  int size = xml.GetAttributeValue("size").ToInt();
  ExString sdata( xml.GetTextValue() );
  std::vector<double> x(size), y(size);
  std::vector<bool> connect(size);
  int jStart = 0;
  for( int j=0; j<size; ++j )
  {
    int jEnd = sdata.find_first_of( " ", jStart );
    x[j] = sdata.substr(jStart,jEnd-jStart).ToDouble();
    jStart = jEnd+1;
    jEnd = sdata.find_first_of( " ", jStart );
    y[j] = sdata.substr(jStart,jEnd-jStart).ToDouble();
    jStart = jEnd+1;
    jEnd = sdata.find_first_of( " ", jStart );
    connect[j] = sdata.substr(jStart,jEnd-jStart).ToBool();
    jStart = jEnd+1;
  }
  xml.NextElementNode(); // get <drawabletexts>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<GRA_drawableText*> textVec(size);
  for( int j=0; j<size; ++j )
  {
    xml.NextElementNode(); // get <drawabletext>
    textVec[j] = GetDrawableText(xml);
  }
  GRA_contourLine *contour = new GRA_contour();
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
  int nrow = xml.GetAttributeValue("nrow").ToInt();
  double fmin = xml.GetAttributeValue("fmin").ToDouble();
  double fmax = xml.GetAttributeValue("fmax").ToDouble();
  double gmin = xml.GetAttributeValue("gmin").ToDouble();
  double gmax = xml.GetAttributeValue("gmax").ToDouble();
  bool xprofile = xml.GetAttributeValue("xprofile").ToBool();
  bool yprofile = xml.GetAttributeValue("yprofile").ToBool();
  bool border = xml.GetAttributeValue("border").ToBool();
  bool zoom = xml.GetAttributeValue("zoom").ToBool();
  bool axes = xml.GetAttributeValue("axes").ToBool();
  bool reset = xml.GetAttributeValue("reset").ToBool();
  double bscale = xml.GetAttributeValue("bscale").ToDouble();
  xml.NextElementNode(); // get <data>
  int size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> x( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> y( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> z( GetDoubleData(xml.GetTextValue(),size) );
  return new GRA_boxPlot( x, y, z, nrow, fmin, fmax, gmin, gmax, xprofile, yprofile,
                          border, zoom, axes, reset, bscale );
}

GRA_diffusionPlot *GetDiffusionPlot( ExXML &xml )
{
  int nrow = xml.GetAttributeValue("nrow").ToInt();
  double fmin = xml.GetAttributeValue("fmin").ToDouble();
  double fmax = xml.GetAttributeValue("fmax").ToDouble();
  double gmin = xml.GetAttributeValue("gmin").ToDouble();
  double gmax = xml.GetAttributeValue("gmax").ToDouble();
  bool xprofile = xml.GetAttributeValue("xprofile").ToBool();
  bool yprofile = xml.GetAttributeValue("yprofile").ToBool();
  bool border = xml.GetAttributeValue("border").ToBool();
  bool zoom = xml.GetAttributeValue("zoom").ToBool();
  bool axes = xml.GetAttributeValue("axes").ToBool();
  bool reset = xml.GetAttributeValue("reset").ToBool();
  xml.NextElementNode(); // get <data>
  int size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> x( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> y( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> z( GetDoubleData(xml.GetTextValue(),size) );
  return new GRA_diffusionPlot( x, y, z, nrow, fmin, fmax, gmin, gmax, xprofile, yprofile,
                                border, zoom, axes, reset );
}

GRA_ditheringPlot *GetDitheringPlot( ExXML &xml )
{
  int nrow = xml.GetAttributeValue("nrow").ToInt();
  double fmin = xml.GetAttributeValue("fmin").ToDouble();
  double fmax = xml.GetAttributeValue("fmax").ToDouble();
  double gmin = xml.GetAttributeValue("gmin").ToDouble();
  double gmax = xml.GetAttributeValue("gmax").ToDouble();
  bool xprofile = xml.GetAttributeValue("xprofile").ToBool();
  bool yprofile = xml.GetAttributeValue("yprofile").ToBool();
  bool border = xml.GetAttributeValue("border").ToBool();
  bool zoom = xml.GetAttributeValue("zoom").ToBool();
  bool axes = xml.GetAttributeValue("axes").ToBool();
  bool reset = xml.GetAttributeValue("reset").ToBool();
  bool legend = xml.GetAttributeValue("legend").ToBool();
  bool equalspaced = xml.GetAttributeValue("equalspaced").ToBool();
  bool areas = xml.GetAttributeValue("areas").ToBool();
  bool volumes = xml.GetAttributeValue("volumes").ToBool();
  bool lines = xml.GetAttributeValue("lines").ToBool();
  xml.NextElementNode(); // get <data>
  int size = xml.GetAttributeValue("size").ToInt();
  std::vector<int> dither( GetIntData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> contourLevels( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> x( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> y( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> z( GetDoubleData(xml.GetTextValue(),size) );
  return new GRA_ditheringPlot( x, y, z, nrow, fmin, fmax, gmin, gmax, xprofile, yprofile,
                                border, zoom, axes, reset, legend, equalspaced, areas,
                                volumes, lines, dither, contourLevels );
}

GRA_gradientPlot *GetGradientPlot( ExXML &xml )
{
  int nrow = xml.GetAttributeValue("nrow").ToInt();
  double fmin = xml.GetAttributeValue("fmin").ToDouble();
  double fmax = xml.GetAttributeValue("fmax").ToDouble();
  double gmin = xml.GetAttributeValue("gmin").ToDouble();
  double gmax = xml.GetAttributeValue("gmax").ToDouble();
  bool xprofile = xml.GetAttributeValue("xprofile").ToBool();
  bool yprofile = xml.GetAttributeValue("yprofile").ToBool();
  bool border = xml.GetAttributeValue("border").ToBool();
  bool zoom = xml.GetAttributeValue("zoom").ToBool();
  bool axes = xml.GetAttributeValue("axes").ToBool();
  bool reset = xml.GetAttributeValue("reset").ToBool();
  bool legend = xml.GetAttributeValue("legend").ToBool();
  bool linear = xml.GetAttributeValue("linear").ToBool();
  xml.NextElementNode(); // get <data>
  int size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> x( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> y( GetDoubleData(xml.GetTextValue(),size) );
  xml.NextElementNode(); // get <data>
  size = xml.GetAttributeValue("size").ToInt();
  std::vector<double> z( GetDoubleData(xml.GetTextValue(),size) );
  return new GRA_gradientPlot( x, y, z, nrow, fmin, fmax, gmin, gmax, xprofile, yprofile,
                               border, zoom, axes, reset, legend, linear );
}

} // end of namespace

// end of file
