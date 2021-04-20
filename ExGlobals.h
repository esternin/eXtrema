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
#ifndef EX_GLOBALS
#define EX_GLOBALS

#include <map>
#include <vector>

#include "ExString.h"
#include "GRA_font.h"
#include "NumericVariable.h"
#include "GRA_outputType.h"
#include "extremaMain.h"
#include "ExXML.h"

class GRA_window;
class Script;
class GRA_setOfCharacteristics;
class GRA_point;
class GRA_polyline;
class GRA_multiLineFigure;
class GRA_polygon;
class GRA_ellipse;
class GRA_drawableText;
class GRA_plotSymbol;
class GRA_cartesianAxes;
class GRA_cartesianCurve;
class GRA_axis;
class GRA_errorBar;
class GRA_contourLine;
class GRA_boxPlot;
class GRA_diffusionPlot;
class GRA_ditheringPlot;
class GRA_gradientPlot;
class ParseLine;
class GRA_polarAxes;
class GRA_polarCurve;

namespace ExGlobals
{
  extern double splineTension_;
  extern GRA_outputType *screenOutput_;
  extern double xminW_, yminW_, xmaxW_, ymaxW_;             // world coordinates
  extern double xminClip_, yminClip_, xmaxClip_, ymaxClip_; // clipping boundary
  extern int xminM_, yminM_, xmaxM_, ymaxM_;             // monitor coordinates
  extern ExString scriptExtension_, stackExtension_;
  extern char continuationCharacter_, executeCharacter_;
  extern bool echo_, stackIsOn_, stackSaved_, executeFlag_;
  extern int nHistory_, maxHistory_, currentScriptNumber_, scriptNumberSave_;
  extern std::vector<Script*> scripts_;
  extern bool workingColorFlag_, workingFontFlag_, workingFontColorFlag_;
  extern bool workingFontHeightFlag_, workingFontAngleFlag_;
  extern double workingFontHeight_, workingFontAngle_;
  extern GRA_color *workingColor_, *workingFontColor_;
  extern GRA_font *workingFont_;
  extern ExString currentPath_, executablePath_, stackFile_;
  extern bool noviceMode_, executeCommand_, returnCommand_, pausingScript_, restartingScript_;
  extern bool prepareToStopScript_, prepareToExecuteScript_, prepareToPauseScript_;
  extern std::ofstream stackStream_;
  extern std::map<ExString,ExString> alias_;

  void Initialize();

  void ExecuteInit();
  void DeleteStuff();

  void AddGraphWindow( GRA_window * );
  std::vector<GRA_window*> &GetGraphWindows();
  GRA_window *GetGraphWindow();
  GRA_window *GetGraphWindow( int );
  void SetWindowNumber( int );
  int GetWindowNumber();
  int GetNumberOfWindows();

  void ClearWindows();
  void ReplotCurrentWindow();
  void ReplotAllWindows();

  bool GetNoviceMode();
  void SetNoviceMode( bool );
  void ToggleNoviceMode();

  void SetCurrentPath( ExString );
  ExString GetCurrentPath();
  ExString GetExecutablePath();

  void SetTension( double );
  double GetTension();

  void SetWorkingColorFlag( bool );
  bool GetWorkingColorFlag();
  void SetWorkingColor( GRA_color * );
  GRA_color *GetWorkingColor();
  void SetWorkingFontFlags( bool );
  bool GetWorkingFontFlag();
  void SetWorkingFontFlag( bool );
  void SetWorkingFont( GRA_font * );
  GRA_font *GetWorkingFont();
  bool GetWorkingFontColorFlag();
  void SetWorkingFontColorFlag( bool );
  void SetWorkingFontColor( GRA_color * );
  GRA_color *GetWorkingFontColor();
  bool GetWorkingFontHeightFlag();
  void SetWorkingFontHeightFlag( bool );
  void SetWorkingFontHeight( double height );
  double GetWorkingFontHeight();
  bool GetWorkingFontAngleFlag();
  void SetWorkingFontAngleFlag( bool );
  void SetWorkingFontAngle( double );
  double GetWorkingFontAngle();

  GRA_outputType *GetScreenOutput();

  void WriteOutput( ExString const & );
  void WarningMessage( ExString const & );

  void DefaultSize();
  void ChangeSize( double );
  void ChangeAspectRatio( double );
  double GetAspectRatio();

  void SetWorldLimits( double, double, double, double );
  void ResetWorldLimits();
  void GetWorldLimits( double &, double &, double &, double & );

  void SetClippingBoundary( double, double, double, double );
  void ResetClippingBoundary();
  void GetClippingBoundary( double &, double &, double &, double & );

  void GetMonitorLimits( int &, int &, int &, int & );
  void SetMonitorLimits( int, int, int, int );
  void ResetMonitorLimits();

  bool GetExecuteFlag();
  char GetContinuationCharacter();
  void SetScriptExtension( ExString const & );
  ExString GetScriptExtension();
  void SetStackExtension( ExString const & );
  ExString GetStackExtension();

  void WriteStack( ExString const & );
  bool StackIsOn();
  std::ofstream *GetStackStream();
  ExString &GetStackFile();
  void SetStackOff();
  bool SetStackOn( ExString const &, bool =false );

  bool GetEcho();
  void SetEcho( bool );

  void SetNHistory( int );
  int GetNHistory();
  void SetMaxHistory( int );
  int GetMaxHistory();
  void ClearHistory();

  bool GetExecuteCommand();
  void SetExecuteCommand( bool );
  bool GetReturnCommand();
  void SetReturnCommand( bool );
  void SetRestartingScript( bool );
  bool GetRestartingScript();
  void PauseScripts();
  void RestartScripts();
  void IncrementScript();

  bool NotInaScript();
  Script *GetScript();
  void SetScript( Script * );
  void RunScript();
  void StopScript();
  void ShowScriptError( char const * );
  void StopAllScripts();
  void PrepareToStopScript();
  void PrepareToExecuteScript();
  void PrepareToPauseScript();
  bool GetPausingScript();

  void PreParse( ExString & );
  void ProcessCommand( ExString & );
  bool HandleAlias( ExString &, ExString &, ParseLine & );
  void AddAlias( ExString const &, ExString const & );
  bool RemoveAlias( ExString const & );
  void RemoveAliases();
  void DisplayAlias( ExString const & );
  void DisplayAliases();

  void ClearGraphicsMonitor();
  void SaveSession( ExString & );
  void RestoreSession( ExString & );
  void DefineConstants();

  void DrawGraphWindows( GRA_outputType * =screenOutput_ );

  void HelpContents();

  void QuitApp();

  std::vector<double> GetDoubleData( ExString const &, int );
  std::vector<int> GetIntData( ExString const &, int );
  std::vector<bool> GetBoolData( ExString const &, int );

  int SetCharacteristics( ExXML &, GRA_setOfCharacteristics * );
  GRA_point *GetPoint( ExXML & );
  GRA_polyline *GetPolyline( ExXML & );
  GRA_multiLineFigure *GetMultiLineFigure( ExXML & );
  GRA_polygon *GetPolygon( ExXML & );
  GRA_ellipse *GetEllipse( ExXML & );
  GRA_drawableText *GetDrawableText( ExXML & );
  GRA_plotSymbol *GetPlotSymbol( ExXML & );
  GRA_cartesianAxes *GetCartesianAxes( ExXML & );
  GRA_cartesianCurve *GetCartesianCurve( ExXML & );
  GRA_axis *GetAxis( ExXML &, char const );
  GRA_errorBar *GetErrorBar( ExXML & );
  GRA_contourLine *GetContour( ExXML & );
  GRA_boxPlot *GetBoxPlot( ExXML & );
  GRA_diffusionPlot *GetDiffusionPlot( ExXML & );
  GRA_ditheringPlot *GetDitheringPlot( ExXML & );
  GRA_gradientPlot *GetGradientPlot( ExXML & );
  GRA_polarAxes *GetPolarAxes( ExXML & );
  GRA_polarCurve *GetPolarCurve( ExXML & );
}
#endif
