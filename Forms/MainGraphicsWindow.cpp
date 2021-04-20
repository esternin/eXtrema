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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "MainGraphicsWindow.h"

#include "ExGlobals.h"
#include "ParseLine.h"
#include "GraphForm.h"
#include "GraphNovice.h"
#include "TextInteractive.h"
#include "SetAspectRatio.h"
#include "extremaMain.h"
#include "GRA_window.h"
#include "CMD_defaults.h"
#include "FigureDraw.h"
#include "GRA_polygon.h"
#include "GRA_rectangle.h"
#include "GRA_Star5pt.h"
#include "GRA_ellipse.h"
#include "SelectColor.h"
#include "SelectLineWidth.h"
#include "PeakFinding.h"
#include "ECommandError.h"
#include "EGraphicsError.h"
#include "PNGDisplay.h"
#include "GRA_outputType.h"
#include "GraphicsSubwindow.h"
#include "GRA_borlandW32.h"
#include "GRA_borlandPrinter.h"
#include "GRA_borlandMetafile.h"
#include "GRA_borlandMonitor.h"
#include "GRA_postscript.h"
#include "GRA_gd.h"
#include "GRA_setOfCharacteristics.h"
#include "HintForm.h"
#include "Script.h"
#include "GRA_drawableText.h"
#include "GRA_page.h"
#include "ThreeDPlot.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TMainGraphicsForm *MainGraphicsForm;

__fastcall TMainGraphicsForm::TMainGraphicsForm(TComponent* Owner) : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "MainGraphicsForm", "Top", 10 );
  Left = ini->ReadInteger( "MainGraphicsForm", "Left", 10 );
  //int width = ini->ReadInteger( "MainGraphicsForm", "Width", 552 );
  //int height = ini->ReadInteger( "MainGraphicsForm", "Height", 386 );
  double xmax = ini->ReadFloat( "MainGraphicsForm", "Xmax", 11.0 );
  double ymax = ini->ReadFloat( "MainGraphicsForm", "Ymax", 8.5 );
  double aspectRatio = ini->ReadFloat( "MainGraphicsForm", "AspectRatio", ExGlobals::GetAspectRatio() );
  delete ini;
  //
  MakeFirstPage();
  ExGlobals::ChangeAspectRatio( aspectRatio );
  //
  ExGlobals::ClearGraphicsMonitor();
  unitsType_ = 0;
  units_[0] = "  graph units";
  units_[1] = "  world units";
  units_[2] = "  percentages";
  units_[3] = "  pixels";
  MainGraphicsStatusBar->Panels->Items[0]->Text = units_[0];
  //
  double const eps = 0.00001;
  PeakFindingForm = 0;
  //
  hintWindow_ = new TMainHintForm( MainGraphicsForm );
  hintWindow_->HideIt();
  //
  ExGlobals::ExecuteInit();
  ExGlobals::SetWorldLimits( 0.0, 0.0, xmax, ymax );
}

__fastcall TMainGraphicsForm::~TMainGraphicsForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "MainGraphicsForm", "Top", Top );
  ini->WriteInteger( "MainGraphicsForm", "Left", Left );
  /*
  ini->WriteInteger( "MainGraphicsForm", "Width",
    pages_[PageControl1->ActivePageIndex]->GetImage()->Width );
  ini->WriteInteger( "MainGraphicsForm", "Height",
    pages_[PageControl1->ActivePageIndex]->GetImage()->Height );
  */
  double xmin, ymin, xmax, ymax;
  ExGlobals::GetWorldLimits( xmin, ymin, xmax, ymax );
  ini->WriteFloat( "MainGraphicsForm", "Xmax", xmax );
  ini->WriteFloat( "MainGraphicsForm", "Ymax", ymax );
  ini->WriteFloat( "MainGraphicsForm", "AspectRatio", ExGlobals::GetAspectRatio() );
  delete ini;
  if( hintWindow_ )delete hintWindow_;
  DeleteAllPages();
}

void TMainGraphicsForm::DeleteAllPages()
{
  while( !pages_.empty() )
  {
    delete pages_.back();
    pages_.pop_back();
  }
}

void TMainGraphicsForm::SetDimensions( int top, int left )
{
  Top = top;
  Left = left;
}

std::ostream &operator<<( std::ostream &out, TMainGraphicsForm const &mgf )
{
  bool b = (GraphCommandForm!=0);
  b = b||(GraphNoviceForm!=0);
  return out << "<visualizationwindow top=\"" << mgf.Top << "\" left=\""
             << mgf.Left << "\" width=\""
             << mgf.pages_[mgf.PageControl1->ActivePageIndex]->GetImage()->Width
             << "\" height=\""
             << mgf.pages_[mgf.PageControl1->ActivePageIndex]->GetImage()->Height
             << "\" aspectratio=\"" << ExGlobals::GetAspectRatio()
             << "\" currentpage=\"" << mgf.PageControl1->ActivePageIndex+1
             << "\" figure=\"" << (FigureDrawForm!=0)
             << "\" peak=\"" << (PeakFindingForm!=0)
             << "\" graph=\"" << b
             << "\" subw=\"" << (GraphicsSubwindowForm!=0)
             << "\" text=\"" << (TextCommandForm!=0)
             << "\"/>\n";
}

void __fastcall TMainGraphicsForm::WMEnterSizeMove( TMessage &message )
{ /* nothing here */ }

void __fastcall TMainGraphicsForm::WMExitSizeMove( TMessage &message )
{
  int height = pages_[PageControl1->ActivePageIndex]->GetImage()->Height;
  int width = pages_[PageControl1->ActivePageIndex]->GetImage()->Width;
  double ratio = static_cast<double>(height)/static_cast<double>(width);
  double aspectRatio = ExGlobals::GetAspectRatio();
  if( ratio > aspectRatio )height = static_cast<int>(aspectRatio*width+0.5);
  else                     width = static_cast<int>(height/aspectRatio+0.5);
  ExGlobals::SetMonitorLimits( 0, 0, width, height );
  SetImageSize();
  pages_[PageControl1->ActivePageIndex]->DrawGraphWindows( ExGlobals::GetScreenOutput() );
}

void TMainGraphicsForm::DrawGraphWindows( GRA_outputType *outputType )
{
  pages_[PageControl1->ActivePageIndex]->DrawGraphWindows( outputType );
}

void TMainGraphicsForm::DisplayBackgrounds( GRA_outputType *outputType )
{
  pages_[PageControl1->ActivePageIndex]->DisplayBackgrounds( outputType );
}

void TMainGraphicsForm::ClearWindows()
{
  pages_[PageControl1->ActivePageIndex]->ClearWindows();
}

void TMainGraphicsForm::ReplotCurrentWindow()
{
  pages_[PageControl1->ActivePageIndex]->ReplotCurrentWindow();
}

void TMainGraphicsForm::ReplotAllWindows()
{
  pages_[PageControl1->ActivePageIndex]->ReplotAllWindows();
}

int TMainGraphicsForm::GetWindowNumber()
{
  return pages_[PageControl1->ActivePageIndex]->GetWindowNumber();
}

int TMainGraphicsForm::GetNumberOfWindows()
{
  return pages_[PageControl1->ActivePageIndex]->GetNumberOfWindows();
}

int TMainGraphicsForm::GetNumberOfPages()
{ return PageControl1->PageCount; }

GRA_page *TMainGraphicsForm::GetPage( int n )
{
  if( n == -1 )return pages_[PageControl1->ActivePageIndex];
  return pages_[n];
}

void TMainGraphicsForm::ResetWindows()
{
  pages_[PageControl1->ActivePageIndex]->ResetWindows();
}

void __fastcall TMainGraphicsForm::ClearAll( TObject *Sender )
{
  ExGlobals::ClearGraphicsMonitor();
  pages_[PageControl1->ActivePageIndex]->ClearWindows();
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack("CLEAR");
}

void __fastcall TMainGraphicsForm::ClearGraphicsAllowReplotMenuItemClick( TObject *Sender )
{ ExGlobals::ClearGraphicsMonitor(); }

void __fastcall TMainGraphicsForm::ClearCurrentWindow( TObject *Sender )
{
  GRA_window *gw = pages_[PageControl1->ActivePageIndex]->GetGraphWindow();
  gw->Erase(); // erase the canvas
  gw->Clear(); // destroy drawable objects in window
}

void __fastcall TMainGraphicsForm::ClearCurrentWindowAllowReplot( TObject *Sender )
{ pages_[PageControl1->ActivePageIndex]->GetGraphWindow()->Erase(); } // erase the canvas

void __fastcall TMainGraphicsForm::ZLHorizontalClick( TObject *Sender )
{ pages_[PageControl1->ActivePageIndex]->GetGraphWindow()->DrawZerolines( true, false ); }

void __fastcall TMainGraphicsForm::ZLVerticalClick( TObject *Sender )
{ pages_[PageControl1->ActivePageIndex]->GetGraphWindow()->DrawZerolines( false, true ); }

void __fastcall TMainGraphicsForm::ZLBothClick( TObject *Sender )
{ pages_[PageControl1->ActivePageIndex]->GetGraphWindow()->DrawZerolines( true, true ); }

int TMainGraphicsForm::GetUnitsType() const
{ return unitsType_; }

void __fastcall TMainGraphicsForm::StatusBarClick( TObject *Sender )
{
  if( ++unitsType_ == 4 )unitsType_ = 0;
  MainGraphicsStatusBar->Panels->Items[0]->Text = units_[unitsType_];
}

void TMainGraphicsForm::SetStatusBarText( AnsiString &s1, AnsiString &s2 )
{
  MainGraphicsStatusBar->Panels->Items[1]->Text = s1;
  MainGraphicsStatusBar->Panels->Items[2]->Text = s2;
}

void __fastcall TMainGraphicsForm::PrintGraphics( TObject *Sender )
{
  GraphicsPrintDialog->Options << poPrintToFile << poWarning;
  if( GraphicsPrintDialog->Execute() )
  {
    TPrinter *printer = Printer();
    printer->BeginDoc();
    try
    {
      pages_[PageControl1->ActivePageIndex]->DrawGraphWindows( &GRA_borlandPrinter(printer) );
    }
    catch (EGraphicsError &e)
    {
      Application->MessageBox(e.what(),"error",MB_OK);
    }
    printer->EndDoc();
  }
}

void TMainGraphicsForm::SetImageSize()
{
  // check out GetSystemMetrics for info on border size, etc.
  //
  int xmin, ymin, xmax, ymax;
  ExGlobals::GetMonitorLimits( xmin, ymin, xmax, ymax );
  Height = ymax - ymin + 120;
  Width = xmax - xmin + 50;
  //
  pages_[PageControl1->ActivePageIndex]->Reset();
  //
  MainGraphicsStatusBar->Width = Width;
  MainGraphicsStatusBar->Panels->Items[0]->Width = Width/5;
  MainGraphicsStatusBar->Panels->Items[1]->Width = 2*Width/5;
  MainGraphicsStatusBar->Panels->Items[2]->Width = 2*Width/5;
  ExGlobals::ClearGraphicsMonitor();
}

void TMainGraphicsForm::GetImageDimensions( int &width, int &height )
{
  width = pages_[PageControl1->ActivePageIndex]->GetImage()->Width;
  height = pages_[PageControl1->ActivePageIndex]->GetImage()->Height;
}

TImage *TMainGraphicsForm::GetCurrentImage()
{ return pages_[PageControl1->ActivePageIndex]->GetImage(); }

void __fastcall TMainGraphicsForm::Close1( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TMainGraphicsForm::ReplotClick( TObject *Sender )
{ ReplotAllWindows(); }

void __fastcall TMainGraphicsForm::ReplotCurentWindow( TObject *Sender )
{ ExGlobals::ReplotCurrentWindow(); }

void __fastcall TMainGraphicsForm::SetUpPrinter( TObject *Sender )
{ GraphicsPrinterSetupDialog->Execute(); }

void __fastcall TMainGraphicsForm::DefaultsClick( TObject *Sender )
{
  ParseLine *p = 0;
  CMD_defaults::Definition()->Execute( p );
}

void __fastcall TMainGraphicsForm::CloseQuery1( TObject *Sender, bool &CanClose )
{ CanClose = false; }

void TMainGraphicsForm::SetWindowNumber( int n )
{ pages_[PageControl1->ActivePageIndex]->SetWindowNumber(n); }

TCanvas *TMainGraphicsForm::GetCanvas()
{ return pages_[PageControl1->ActivePageIndex]->GetImage()->Canvas; }

void TMainGraphicsForm::SetArrowPlacementMode( bool b )
{ pages_[PageControl1->ActivePageIndex]->SetArrowPlacementMode(b); }

void TMainGraphicsForm::SetPolygonPlacementMode( bool b )
{ pages_[PageControl1->ActivePageIndex]->SetPolygonPlacementMode(b); }

void TMainGraphicsForm::SetEllipsePlacementMode( bool b )
{ pages_[PageControl1->ActivePageIndex]->SetEllipsePlacementMode(b); }

void TMainGraphicsForm::SetTextPlacementMode( GRA_drawableText *dt )
{
  pages_[PageControl1->ActivePageIndex]->SetTextPlacementMode( dt );
  Show();
  SetFocus();
  Update();

  if( !hintWindow_ )hintWindow_ = new TMainHintForm( MainGraphicsForm );
  std::vector<ExString> lines;
  lines.push_back( ExString("text: ")+dt->GetString() );
  lines.push_back( " " );
  lines.push_back( "click in the visualization window to place the string" );
  hintWindow_->ActivateHint( lines );
}

void TMainGraphicsForm::SetInteractiveWindowMode( bool b )
{ pages_[PageControl1->ActivePageIndex]->SetInteractiveWindowMode(b); }

void TMainGraphicsForm::SetInteractiveLegendMode( bool b )
{ pages_[PageControl1->ActivePageIndex]->SetInteractiveLegendMode(b); }

void TMainGraphicsForm::SetArrowType( int i )
{ pages_[PageControl1->ActivePageIndex]->SetArrowType(i); }

void TMainGraphicsForm::SetHeadsBothEnds( bool b )
{ pages_[PageControl1->ActivePageIndex]->SetHeadsBothEnds(b); }

void TMainGraphicsForm::SetPolygonType( int i )
{ pages_[PageControl1->ActivePageIndex]->SetPolygonType(i); }

void TMainGraphicsForm::SetDrawCircles( bool b )
{ pages_[PageControl1->ActivePageIndex]->SetDrawCircles(b); }

void TMainGraphicsForm::SetPolygonAngle( double a )
{ pages_[PageControl1->ActivePageIndex]->SetPolygonAngle(a); }

void __fastcall TMainGraphicsForm::FiguresSpeedButtonClick( TObject *Sender )
{
  if( FigureDrawForm )
  {
    FigureDrawForm->WindowState = wsNormal;
    FigureDrawForm->Visible = true;
    FigureDrawForm->BringToFront();
  }
  else
  {
    FigureDrawForm = new TFigureDrawForm(this);
    FigureDrawForm->Show();
  }
}

void __fastcall TMainGraphicsForm::PeakSpeedButtonClick( TObject *Sender )
{
  if( PeakFindingForm )
  {
    PeakFindingForm->WindowState = wsNormal;
    PeakFindingForm->Visible = true;
    PeakFindingForm->BringToFront();
  }
  else
  {
    PeakFindingForm = new TPeakFindingForm(this);
    PeakFindingForm->Show();
  }
}

void __fastcall TMainGraphicsForm::GraphClick( TObject *Sender )
{
  if( ExGlobals::GetNoviceMode() )
  {
    if( GraphNoviceForm )
    {
      GraphNoviceForm->WindowState = wsNormal;
      GraphNoviceForm->Visible = true;
      GraphNoviceForm->BringToFront();
    }
    else
    {
      GraphNoviceForm = new TGraphNoviceForm(this);
      GraphNoviceForm->Show();
    }
  }
  else
  {
    if( GraphCommandForm )
    {
      GraphCommandForm->WindowState = wsNormal;
      GraphCommandForm->Visible = true;
      GraphCommandForm->BringToFront();
    }
    else
    {
      GraphCommandForm = new TGraphCommandForm(this);
      GraphCommandForm->Show();
    }
  }
}

void __fastcall TMainGraphicsForm::ThreeDPlotClick(TObject *Sender)
{
  if( ThreeDPlotForm )
  {
    ThreeDPlotForm->WindowState = wsNormal;
    ThreeDPlotForm->Visible = true;
    ThreeDPlotForm->BringToFront();
  }
  else
  {
    ThreeDPlotForm = new TThreeDPlotForm(this);
    ThreeDPlotForm->Show();
  }
}

void __fastcall TMainGraphicsForm::GSubwindowClick( TObject *Sender )
{
  if( GraphicsSubwindowForm )
  {
    GraphicsSubwindowForm->WindowState = wsNormal;
    GraphicsSubwindowForm->Visible = true;
    GraphicsSubwindowForm->BringToFront();
  }
  else
  {
    GraphicsSubwindowForm = new TGraphicsSubwindowForm(this);
    GraphicsSubwindowForm->Show();
  }
}

void __fastcall TMainGraphicsForm::TextCommandClick( TObject *Sender )
{
  if( TextCommandForm )
  {
    TextCommandForm->WindowState = wsNormal;
    TextCommandForm->Visible = true;
    TextCommandForm->BringToFront();
  }
  else
  {
    TextCommandForm = new TTextCommandForm(this);
    TextCommandForm->Show();
  }
}

void __fastcall TMainGraphicsForm::AspectRatioClick( TObject *Sender )
{
  if( SetAspectRatioForm )
  {
    SetAspectRatioForm->WindowState = wsNormal;
    SetAspectRatioForm->Visible = true;
    SetAspectRatioForm->BringToFront();
  }
  else
  {
    SetAspectRatioForm = new TSetAspectRatioForm(this);
    SetAspectRatioForm->Show();
  }
}

void __fastcall TMainGraphicsForm::SaveAnyClick( TObject *Sender )
{
  bool temp = ForceCurrentDirectory;
  ForceCurrentDirectory = true;
  SaveDialog->Filter = "Encapsulated PostScript (*.eps)|*.eps"
      "|PNG file (*.png)|*.png|Windows metafile (*.wmf)|*.wmf"
      "|Windows enhanced metafile (*.emf)|*.emf";
  if( SaveDialog->Execute() )
  {
    AnsiString filename( SaveDialog->FileName );
    AnsiString extension( ExtractFileExt(filename) );
    if( extension.IsEmpty() )
    {
      switch (SaveDialog->FilterIndex)
      {
        case 1:
          extension = ".eps";
          break;
        case 2:
          extension = ".png";
          break;
        case 3:
          extension = ".wmf";
          break;
        case 4:
          extension = ".emf";
          break;
      }
      filename += extension;
    }
    std::ofstream outFile;
    outFile.open( filename.c_str(), ios_base::out );
    if( !outFile.is_open() )
    {
      AnsiString s( "unable to open " );
      s += filename;
      Application->MessageBox(s.c_str(),"error",MB_OK);
      return;
    }
    outFile.close();
    TCursor saveCursor = Screen->Cursor;
    Screen->Cursor = crHourGlass;    // show hourglass cursor
    if( extension == ".eps" )
    {
      std::auto_ptr<GRA_postscript> ps( new GRA_postscript(filename.c_str()) );
      ps->Draw();
      if( ExGlobals::StackIsOn() )
        ExGlobals::WriteStack( ExString("HARDCOPY\\POSTSCRIPT ")+filename.c_str() );
    }
    else if( extension == ".png" )
    {
      try
      {
        std::auto_ptr<GRA_gd> png( new GRA_gd(filename.c_str()) );
        png->Draw();
      }
      catch (EGraphicsError &e)
      {
        Application->MessageBox(e.what(),"error",MB_OK);
      }
      if( ExGlobals::StackIsOn() )
        ExGlobals::WriteStack( ExString("HARDCOPY\\PNG ")+filename.c_str() );
    }
    else if( extension == ".wmf" )
    {
      try
      {
        std::auto_ptr<GRA_borlandMetafile> bm( new GRA_borlandMetafile(false,filename.c_str()) );
        bm->DrawAndSave();
      }
      catch (EGraphicsError &e)
      {
        Application->MessageBox(e.what(),"error",MB_OK);
      }
      if( ExGlobals::StackIsOn() )
        ExGlobals::WriteStack( ExString("HARDCOPY\\WMF ")+filename.c_str() );
    }
    else if( extension == ".emf" )
    {
      try
      {
        std::auto_ptr<GRA_borlandMetafile> bm( new GRA_borlandMetafile(true,filename.c_str()) );
        bm->DrawAndSave();
      }
      catch (EGraphicsError &e)
      {
        Application->MessageBox(e.what(),"error",MB_OK);
      }
      if( ExGlobals::StackIsOn() )
        ExGlobals::WriteStack( ExString("HARDCOPY\\EMF ")+filename.c_str() );
    }
    Screen->Cursor = saveCursor;
  }
  ForceCurrentDirectory = temp;
}

void __fastcall TMainGraphicsForm::SavePS( TObject *Sender )
{
  bool temp = ForceCurrentDirectory;
  ForceCurrentDirectory = true;
  SaveDialog->Filter = "Encapsulated PostScript (*.eps)|*.eps";
  if( SaveDialog->Execute() )
  {
    TCursor saveCursor = Screen->Cursor;
    Screen->Cursor = crHourGlass;    // show hourglass cursor
    std::auto_ptr<GRA_postscript> ps( new GRA_postscript(SaveDialog->FileName.c_str()) );
    ps->Draw();
    Screen->Cursor = saveCursor;
  }
  ForceCurrentDirectory = temp;
}

void __fastcall TMainGraphicsForm::SavePNG( TObject *Sender )
{
  bool temp = ForceCurrentDirectory;
  ForceCurrentDirectory = true;
  SaveDialog->Filter = "PNG file (*.png)|*.png";
  if( SaveDialog->Execute() )
  {
    TCursor saveCursor = Screen->Cursor;
    Screen->Cursor = crHourGlass;    // show hourglass cursor
    try
    {
      std::auto_ptr<GRA_gd> png( new GRA_gd(SaveDialog->FileName.c_str()) );
      png->Draw();
    }
    catch (EGraphicsError &e)
    {
      Application->MessageBox(e.what(),"error",MB_OK);
    }
    Screen->Cursor = saveCursor;
  }
  ForceCurrentDirectory = temp;
}

void __fastcall TMainGraphicsForm::SaveMF( TObject *Sender )
{
  bool temp = ForceCurrentDirectory;
  ForceCurrentDirectory = true;
  SaveDialog->Filter = "Windows metafile (*.wmf)|*.wmf";
  if( SaveDialog->Execute() )
  {
    TCursor saveCursor = Screen->Cursor;
    Screen->Cursor = crHourGlass;    // show hourglass cursor
    try
    {
      std::auto_ptr<GRA_borlandMetafile> bm( new GRA_borlandMetafile(false,SaveDialog->FileName.c_str()) );
      bm->DrawAndSave();
    }
    catch (EGraphicsError &e)
    {
      Application->MessageBox(e.what(),"error",MB_OK);
    }
    Screen->Cursor = saveCursor;
  }
  ForceCurrentDirectory = temp;
}

void __fastcall TMainGraphicsForm::SaveEMF( TObject *Sender )
{
  bool temp = ForceCurrentDirectory;
  ForceCurrentDirectory = true;
  SaveDialog->Filter = "Windows enhanced metafile (*.emf)|*.emf";
  if( SaveDialog->Execute() )
  {
    TCursor saveCursor = Screen->Cursor;
    Screen->Cursor = crHourGlass;    // show hourglass cursor
    try
    {
      std::auto_ptr<GRA_borlandMetafile> bm( new GRA_borlandMetafile(true,SaveDialog->FileName.c_str()) );
      bm->DrawAndSave();
    }
    catch (EGraphicsError &e)
    {
      Application->MessageBox(e.what(),"error",MB_OK);
    }
    Screen->Cursor = saveCursor;
  }
  ForceCurrentDirectory = temp;
}

void __fastcall TMainGraphicsForm::LoadPNGMenuItemClick( TObject *Sender )
{
  bool temp = ForceCurrentDirectory;
  ForceCurrentDirectory = true;
  if( LoadPictureDialog->Execute() )
  {
    ForceCurrentDirectory = temp;
    AnsiString fileName( LoadPictureDialog->FileName );
    LoadPNG( fileName );
  }
}

void TMainGraphicsForm::LoadPNG( AnsiString const &fileName )
{
  FILE *f = fopen(fileName.c_str(),"rb");
  if( !f )
  {
    AnsiString s("unable to open PNG file [");
    s += fileName + AnsiString("]");
    Application->MessageBox(s.c_str(),"error",MB_OK);
    return;
  }
  if( PNGForm )
  {
    PNGForm->WindowState = wsNormal;
    PNGForm->Visible = true;
    PNGForm->BringToFront();
  }
  else
  {
    PNGForm = new TPNGForm(this);
    PNGForm->Show();
  }
  TCursor saveCursor = Screen->Cursor;
  Screen->Cursor = crHourGlass;
  PNGForm->LoadPNG( f );
  fclose(f);
  Screen->Cursor = saveCursor;
}

void __fastcall TMainGraphicsForm::ImportGraphicsClick( TObject *Sender )
{
  bool temp = ForceCurrentDirectory;
  ForceCurrentDirectory = true;
  LoadPictureDialog->Filter = "PNG file (*.png)|*.png";
  if( LoadPictureDialog->Execute() )
  {
    ForceCurrentDirectory = temp;
    AnsiString fileName( LoadPictureDialog->FileName );
    LoadPNG( fileName );
  }
}
  
void TMainGraphicsForm::SetupLegendFrame()
{
  Application->MessageBox(
  "open a rectangle in the graphics window for the legend frame", "INFORMATION", MB_OK );
  SetFocus();
  Show();
  Update();
  pages_[PageControl1->ActivePageIndex]->SetInteractiveLegendMode( true );
}

void TMainGraphicsForm::HideHintWindow()
{ hintWindow_->HideIt(); }

void TMainGraphicsForm::AddGraphWindow( GRA_window *gw )
{ return pages_[PageControl1->ActivePageIndex]->AddGraphWindow(gw); }

std::vector<GRA_window*> &TMainGraphicsForm::GetGraphWindows()
{ return pages_[PageControl1->ActivePageIndex]->GetGraphWindows(); }

GRA_window *TMainGraphicsForm::GetGraphWindow()
{ return pages_[PageControl1->ActivePageIndex]->GetGraphWindow(); }

GRA_window *TMainGraphicsForm::GetGraphWindow( int n )
{ return pages_[PageControl1->ActivePageIndex]->GetGraphWindow(n); }

void __fastcall TMainGraphicsForm::Windowsize1Click( TObject *Sender )
{ ExGlobals::DefaultSize(); }

void __fastcall TMainGraphicsForm::NewPageClick( TObject *Sender )
{
  GRA_page *page = new GRA_page( PageControl1 );
  pages_.push_back( page );
  SetImageSize();
}

void TMainGraphicsForm::MakeFirstPage()
{
  pages_.push_back( new GRA_page(PageControl1) );
  SetImageSize();
}

void TMainGraphicsForm::NewPage( int n )
{
  // this function is only called for n > maxPage
  //
  int maxPage = pages_.size();
  int i = maxPage-1;
  while( ++i < n )
  {
    pages_.push_back( new GRA_page(PageControl1) );
    PageControl1->ActivePageIndex = i-1;
    ChangePage( (TObject*)0 );
  }
  PageControl1->ActivePageIndex = n-1;
  ChangePage( (TObject*)0 );
}

void TMainGraphicsForm::SetPage( int n )
{
  PageControl1->ActivePageIndex = n-1;
  static_cast<GRA_borlandMonitor*>(ExGlobals::GetScreenOutput())->SetCanvas(
    pages_[n-1]->GetImage()->Canvas );
}

void TMainGraphicsForm::SetPage( GRA_page *p )
{
  int np = pages_.size();
  for( int i=0; i<np; ++i )
  {
    if( pages_[i] == p )
    {
      PageControl1->ActivePageIndex = i;
      static_cast<GRA_borlandMonitor*>(ExGlobals::GetScreenOutput())->SetCanvas(
        pages_[i]->GetImage()->Canvas );
      return;
    }
  }
}

void TMainGraphicsForm::InheritPage( int n, int m )
{ pages_[n-1]->InheritFrom( pages_[m-1] ); }

void __fastcall TMainGraphicsForm::RemoveLastPageClick( TObject *Sender )
{
  int nPages = PageControl1->PageCount;
  if( nPages > 1 )
  {
    delete pages_[nPages-1];
    pages_.pop_back();
  }
}

void __fastcall TMainGraphicsForm::ChangePage( TObject *Sender )
{
  int activePage = PageControl1->ActivePageIndex;
  static_cast<GRA_borlandMonitor*>(ExGlobals::GetScreenOutput())->SetCanvas(
    pages_[activePage]->GetImage()->Canvas );
  SetImageSize();
  pages_[PageControl1->ActivePageIndex]->DrawGraphWindows( ExGlobals::GetScreenOutput() );
}

