/*
Copyright (C) 2005,...,2009 Joseph L. Chuma

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
#ifndef MainGraphicsWindowH
#define MainGraphicsWindowH

#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ExtDlgs.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <Menus.hpp>
#include <Buttons.hpp>
#include <Classes.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Printers.hpp>
#include <Windows.hpp>

class GRA_polygon;
class GRA_ellipse;
class GRA_drawableText;
class ExString;
class TMainHintForm;
class GRA_page;
class GRA_outputType;
class GRA_window;

class TMainGraphicsForm : public TForm
{
__published:  // IDE-managed Components
  TToolBar *MainGraphicsTopSpeedBar;
  TSpeedButton *ClearSpeedButton;
  TMainMenu *MainGraphicsMenu;
  TPrintDialog *GraphicsPrintDialog;
  TPrinterSetupDialog *GraphicsPrinterSetupDialog;
  TSpeedButton *GraphSpeedButton;
  TStatusBar *MainGraphicsStatusBar;
  TSpeedButton *TextSpeedButton;
  TMenuItem *PostScript1;
  TMenuItem *Zerolines1;
  TMenuItem *ZLHorizontal;
  TMenuItem *ZLVertical;
  TMenuItem *ZLBoth;
  TSaveDialog *SaveDialog;
  TMenuItem *Windowsmetafile1;
  TMenuItem *Windowsenhancedmetafile1;
  TMenuItem *Drawagraph1;
  TSpeedButton *PeakSpeedButton;
  TSpeedButton *FiguresSpeedButton;
  TToolButton *ToolButton2;
  TSpeedButton *PrintSpeedButton;
  TToolButton *ToolButton1;
  TSpeedButton *SaveSpeedButton;
  TMenuItem *PNG1;
  TMenuItem *LoadPNGMenuItem;
  TOpenPictureDialog *LoadPictureDialog;
  TSpeedButton *ImportSpeedButton;
  TMenuItem *Clearcurrentwindow1;
  TMenuItem *ClearCurrentWindowButAllowReplot;
  TPageControl *PageControl1;
  TMenuItem *Pages1;
  TMenuItem *Newpage2;
  TMenuItem *Removelastpage2;
  TToolButton *ToolButton3;
  TSpeedButton *NewPageSpeedButton;
  TSpeedButton *RemovePageSpeedButton;
  TMenuItem *Setaspectratio1;
  TToolButton *ToolButton4;
  TSpeedButton *AspectRatioSpeedButton;
        TSpeedButton *ThreeDPlotSpeedButton;

  void __fastcall GraphClick(TObject *Sender);
  void __fastcall GSubwindowClick(TObject *Sender);
  void __fastcall TextCommandClick(TObject *Sender);
  void __fastcall ClearGraphicsAllowReplotMenuItemClick(TObject *Sender);
  void __fastcall ReplotCurentWindow(TObject *Sender);
  void __fastcall PrintGraphics(TObject *Sender);
  void __fastcall Close1(TObject *Sender, TCloseAction &Action);
  //void __fastcall FillColorClick(TObject *Sender);
  //void __fastcall LineColorClick(TObject *Sender);
  //void __fastcall LineWidthClick(TObject *Sender);
  void __fastcall FiguresSpeedButtonClick(TObject *Sender);
  void __fastcall PeakSpeedButtonClick(TObject *Sender);
  void __fastcall SavePS(TObject *Sender);
  void __fastcall ZLHorizontalClick(TObject *Sender);
  void __fastcall ZLVerticalClick(TObject *Sender);
  void __fastcall ZLBothClick(TObject *Sender);
  void __fastcall StatusBarClick(TObject *Sender);
  void __fastcall SaveMF(TObject *Sender);
  void __fastcall SaveEMF(TObject *Sender);
  void __fastcall SaveAnyClick(TObject *Sender);
  void __fastcall SavePNG(TObject *Sender);
  void __fastcall LoadPNGMenuItemClick(TObject *Sender);
  void __fastcall ImportGraphicsClick(TObject *Sender);
  void __fastcall ClearAll(TObject *Sender);
  void __fastcall ReplotClick(TObject *Sender);
  void __fastcall SetUpPrinter(TObject *Sender);
  void __fastcall DefaultsClick(TObject *Sender);
  void __fastcall CloseQuery1(TObject *Sender, bool &CanClose);
  void __fastcall ClearCurrentWindow(TObject *Sender);
  void __fastcall ClearCurrentWindowAllowReplot(TObject *Sender);
  void __fastcall Windowsize1Click(TObject *Sender);
  void __fastcall NewPageClick(TObject *Sender);
  void __fastcall RemoveLastPageClick(TObject *Sender);
  void __fastcall ChangePage(TObject *Sender);
  void __fastcall AspectRatioClick(TObject *Sender);
        void __fastcall ThreeDPlotClick(TObject *Sender);

public:
  int GetUnitsType() const;
  TCanvas *GetCanvas();
  void SetupLegendFrame();
  void SetImageSize();
  void SetDimensions( int, int );
  TImage *GetCurrentImage();
  void DrawGraphWindows( GRA_outputType * );
  void DisplayBackgrounds( GRA_outputType * );
  void ClearWindows();
  void ReplotCurrentWindow();
  int GetWindowNumber();
  void SetWindowNumber( int );
  int GetNumberOfWindows();
  void HideHintWindow();
  GRA_window *GetGraphWindow();
  GRA_window *GetGraphWindow( int );
  void AddGraphWindow( GRA_window * );
  std::vector<GRA_window*> &GetGraphWindows();
  void ReplotAllWindows();
  void ResetWindows();
  int GetNumberOfPages();
  GRA_page *GetPage( int =-1 );
  void MakeFirstPage();
  void NewPage( int );
  void SetPage( int );
  void SetPage( GRA_page * );
  void InheritPage( int, int );
  void DeleteAllPages();

//protected:
  void __fastcall WMEnterSizeMove( TMessage & );
  void __fastcall WMExitSizeMove( TMessage & );
  BEGIN_MESSAGE_MAP
    MESSAGE_HANDLER( WM_ENTERSIZEMOVE, TMessage, WMEnterSizeMove )
    MESSAGE_HANDLER( WM_EXITSIZEMOVE, TMessage, WMExitSizeMove )
  END_MESSAGE_MAP(TForm)

private:      // User declarations
  AnsiString units_[4];
  int unitsType_; // 0 = graph, 1 = world, 2 = percentages, 3 = pixels
  TMainHintForm *hintWindow_;
  std::vector<GRA_page*> pages_;
  
  void LoadPNG( AnsiString const & );
  
public:       // User declarations
  __fastcall TMainGraphicsForm( TComponent * );
  __fastcall ~TMainGraphicsForm();

  void SetArrowPlacementMode( bool );
  void SetPolygonPlacementMode( bool );
  void SetEllipsePlacementMode( bool );
  void SetTextPlacementMode( GRA_drawableText * );
  void SetInteractiveWindowMode( bool );
  void SetInteractiveLegendMode( bool );
  void SetArrowType( int );
  void SetHeadsBothEnds( bool );
  void SetPolygonType( int );
  void SetDrawCircles( bool );
  void SetPolygonAngle( double );
  void GetImageDimensions( int &, int & );
  void SetStatusBarText( AnsiString &, AnsiString & );

  friend std::ostream &operator<<( std::ostream &, TMainGraphicsForm const & );
};

extern PACKAGE TMainGraphicsForm *MainGraphicsForm;

#endif
