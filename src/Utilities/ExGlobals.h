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
#ifndef EX_GLOBALS
#define EX_GLOBALS

#include <string>
#include <vector>
#include <map>

#include "wx/wx.h"
#include "wx/dcps.h"
#include "wx/image.h"

DECLARE_EVENT_TYPE(myCHW_UPDATE,-1);  // myCHW_UPDATE refers to CommandHistoryWindow

class extrema;
class ExXML;
class Script;
class GRA_color;
class GRA_wxWidgets;
class GRA_window;
class GRA_font;
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
class wxHtmlHelpController;
class wxPrintData;
class VisualizationWindow;
class AnalysisWindow;
class HintForm;
class ParseLine;
class AxisPopup;
class CurvePopup;
class TextPopup;
class LegendPopup;
class GraphicsPage;
class GRA_legend;
class GRA_polarAxes;
class GRA_polarCurve;

namespace ExGlobals
{
  void Initialize();
  wxPrintData *GetPrintData();
  void StartHelp();
  void ExecuteInit();

  void DeleteStuff();

  void SetAnalysisWindow( AnalysisWindow * );
  AnalysisWindow *GetAnalysisWindow();
  void SetVisualizationWindow( VisualizationWindow * );
  VisualizationWindow *GetVisualizationWindow();

  void ShowHint( std::vector<wxString> const & );
  void HideHint();

  bool GetNoviceMode();
  void SetNoviceMode( bool );
  void ToggleNoviceMode();

  wxString GetCurrentPath();
  void SetCurrentPath( wxString const & );
  wxString GetExecutablePath();
  wxString GetHelpPath();
  wxString GetImagePath();

  void SetTension( double );
  double GetTension();

  void SetWorkingColorFlag( bool );
  bool GetWorkingColorFlag();
  void SetWorkingColor( int );
  GRA_color *GetWorkingColor();

  void SetWorkingFontFlag( bool );
  bool GetWorkingFontFlag();
  void SetWorkingFont( int, int, double, double );
  GRA_font *GetWorkingFont();
  GRA_color *GetWorkingFontColor();
  double GetWorkingFontHeight();
  double GetWorkingFontAngle();

  //void SetwxWindow( wxWindow *, int, int );
  wxWindow *GetwxWindow();
  GRA_wxWidgets *GetGraphicsOutput();
  GraphicsPage *GetGraphicsPage();

  void WriteOutput( wxString const & );
  void WarningMessage( wxString const & );

  void ChangeSize( double );
  void SetAspectRatio( double );
  double GetAspectRatio();
  
  void ResetWorldLimits();
  void GetWorldLimits( double &, double &, double &, double & );
  void SetClippingBoundary( double, double, double, double );
  void ResetClippingBoundary();
  void GetClippingBoundary( double &, double &, double &, double & );
  void GetMonitorLimits( int &, int &, int &, int & );
  void SetMonitorLimits( int, int, int, int );

  bool NotInaScript();
  Script *GetScript();
  bool GetExecuteCommand();
  void SetExecuteCommand( bool );
  bool GetReturnCommand();
  void SetReturnCommand( bool );
  bool GetRestartingScript();
  void SetRestartingScript( bool );
  void PauseScripts();
  void RestartScripts();
  void IncrementScript();
  void SetScript( Script * );
  void RunScript();
  void StopScript();
  void ShowScriptError( char const * );
  void StopAllScripts();
  void PrepareToStopScript();
  void PrepareToExecuteScript();
  void PrepareToPauseScript();
  bool GetPausingScript();

  bool GetExecuteFlag();

  void PreParse( wxString & );
  void ProcessCommand( wxString & );
  bool HandleAlias( wxString &, wxString &, ParseLine & );
  void AddAlias( wxString const &, wxString const & );
  bool RemoveAlias( wxString const & );
  void RemoveAliases();
  void DisplayAlias( wxString const & );
  void DisplayAliases();  

  wxChar GetContinuationCharacter();

  void SetScriptExtension( wxString const & );
  wxString GetScriptExtension();

  void SetStackExtension( wxString const & );
  wxString GetStackExtension();

  bool GetEcho();
  void SetEcho( bool );

  GRA_window *GetGraphWindow();
  GRA_window *GetGraphWindow( int );
  GRA_window *GetGraphWindow( double, double );
  void SetWindowNumber( int );
  int GetWindowNumber();
  int GetNumberOfWindows();
  std::vector<GRA_window*> &GetGraphWindows();
  void AddGraphWindow( GRA_window * );
  void DrawGraphWindows( GRA_wxWidgets *, wxDC & );
  void ClearGraphicsWindow( bool, int );

  void SetNHistory( int );
  int GetNHistory();

  void SetMaxHistory( int );
  int GetMaxHistory();

  bool StackIsOn();
  void SetStackOff();
  bool SetStackOn( wxString const &, bool =false );
  std::ofstream *GetStackStream();
  wxString &GetStackFile();
  void WriteStack( wxString const & );

  void RefreshGraphics();
  void ClearWindows();

  void ReplotAllWindows();
  void ReplotCurrentWindow();

  void ClearGraphicsMonitor();
  void ClearHistory();

  void SaveSession( wxString & );
  void RestoreSession( wxString & );
  std::string InvalidNodeMessage( wxString const &, wxString const &, wxString const & );

  void DefineConstants();

  void DefaultSize();
  void InteractiveLegendFrameSetup();

  wxColour GetwxColor( GRA_color * );
  wxColour GetwxColor( int );

  AxisPopup *GetAxisPopup( GraphicsPage * );
  void ZeroAxisPopup();

  CurvePopup *GetCurvePopup( GraphicsPage * );
  void ZeroCurvePopup();
  void DisconnectCurvePopup();

  TextPopup *GetTextPopup( GraphicsPage * );
  void ZeroTextPopup();
  void DisconnectTextPopup();

  LegendPopup *GetLegendPopup( GraphicsPage * );
  void ZeroLegendPopup();
  void DisconnectLegendPopup();

  void QuitApp();

  wxString GetNiceNumber( double );
  
  void RemoveQuotes( wxString & );
  wxString GetFileExtension( wxString & );
  wxString Capitalize( wxString const & );
  void ToCapitalize( wxString & );
  void RemoveBlanks( wxString &s );
  wxString IntToHex( int );

  std::vector<double> GetDoubleData( wxString const &, int );
  std::vector<int> GetIntData( wxString const &, int );
  std::vector<bool> GetBoolData( wxString const &, int );

  void SetCharacteristics( ExXML &, GRA_setOfCharacteristics *,
                           wxString const &, wxString const & );
  GRA_point *GetPoint( ExXML & );
  GRA_polyline *GetPolyline( ExXML & );
  GRA_multiLineFigure *GetMultiLineFigure( ExXML & );
  GRA_polygon *GetPolygon( ExXML & );
  GRA_ellipse *GetEllipse( ExXML & );
  GRA_drawableText *GetDrawableText( ExXML & );
  GRA_plotSymbol *GetPlotSymbol( ExXML & );
  GRA_cartesianAxes *GetCartesianAxes( ExXML &, wxString const & );
  GRA_cartesianCurve *GetCartesianCurve( ExXML & );
  GRA_axis *GetAxis( ExXML &, wxChar const, wxString const & );
  GRA_errorBar *GetErrorBar( ExXML & );
  GRA_contourLine *GetContour( ExXML & );
  GRA_boxPlot *GetBoxPlot( ExXML & );
  GRA_diffusionPlot *GetDiffusionPlot( ExXML & );
  GRA_ditheringPlot *GetDitheringPlot( ExXML & );
  GRA_gradientPlot *GetGradientPlot( ExXML & );
  GRA_legend *GetGraphLegend( ExXML &, GRA_window * );
  GRA_polarAxes *GetPolarAxes( ExXML &, wxString const & );
  GRA_polarCurve *GetPolarCurve( ExXML & );
}

#endif
