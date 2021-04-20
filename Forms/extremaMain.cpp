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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "extremaMain.h"
#include "ExGlobals.h"
#include "GRA_colorControl.h"
#include "Htmlhelp.h"
#include "About.h"
#include "MainGraphicsWindow.h"
#include "VarShow.h"
#include "VarInfo.h"
#include "ReadVectorsForm.h"
#include "ReadScalarsForm.h"
#include "ReadMatrixForm.h"
#include "ReadTextForm.h"
#include "WriteVectorsForm.h"
#include "WriteScalarsForm.h"
#include "WriteMatrixForm.h"
#include "WriteTextForm.h"
#include "GraphForm.h"
#include "GraphNovice.h"
#include "FitForm.h"
#include "FitNovice.h"
#include "GenerateCommandForm.h"
#include "GraphicsSubwindow.h"
#include "TextInteractive.h"
#include "ExecuteForm.h"
#include "OperatorTable.h"
#include "FunctionTable.h"
#include "CommandTable.h"
#include "SelectLineWidth.h"
#include "FigureDraw.h"
#include "PeakFinding.h"
#include "StackCommandsForm.h"
#include "Script.h"
#include "SetAspectRatio.h"
#include "ThreeDPlot.h"
//
#pragma resource "*.dfm"

TMainForm *MainForm;

__fastcall TMainForm::TMainForm( TComponent *Owner ) : TForm(Owner)
{
  helpFileName_ = AnsiString("extrema.chm");
  listBoxWidth_ = MainListBox->Width;
  //
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "MainForm", "Top", 500 );
  Left= ini->ReadInteger( "MainForm", "Left", 500 );
  Height = ini->ReadInteger( "MainForm", "Height", 500 );
  Width = ini->ReadInteger( "MainForm", "Width", 700 );
  delete ini;
  //
  commandStringsIndex_ = -1;
  ExGlobals::Initialize();
}

__fastcall TMainForm::~TMainForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "MainForm", "Top", Top );
  ini->WriteInteger( "MainForm", "Left", Left );
  ini->WriteInteger( "MainForm", "Height", Height );
  ini->WriteInteger( "MainForm", "Width", Width );
  delete ini;
  delete OperatorTable::GetTable();
  delete FunctionTable::GetTable();
  delete CommandTable::GetTable();
  delete ExGlobals::GetScreenOutput();
}

void TMainForm::SetDimensions( int top, int left, int height, int width )
{
  Top = top;
  Left = left;
  Height = height;
  Width = width;
}

void TMainForm::WriteOutput( ExString const &s )
{
  AnsiString as( s.c_str() );
  int w = 1.75*MainListBox->Canvas->TextWidth( as );
  if( w > listBoxWidth_ )
  {
    listBoxWidth_ = w;
    MainListBox->Perform(LB_SETHORIZONTALEXTENT, w, 0);
  }
  MainListBox->Items->Add( as );
  MainListBox->TopIndex = MainListBox->Items->Count - 1;
}

void __fastcall TMainForm::FormCreate( TObject *Sender )
{ Application->OnHint = ShowHint; }

void __fastcall TMainForm::ShowHint( TObject *Sender )
{ StatusLine->SimpleText = Application->Hint; }

void __fastcall TMainForm::FileNew( TObject *Sender )
{ /*--- Add code to create a new file --- */ }

void __fastcall TMainForm::QuitApp( TObject *Sender )
{
  if( Application->MessageBox("Do you really want to quit the program?","?",MB_OKCANCEL) == IDOK )
  {
    if( ExGlobals::StackIsOn() )ExGlobals::WriteStack("QUIT");
    QuitApp();
  }
}

void TMainForm::QuitApp()
{
  GRA_colorControl::DeleteStuff();
  ExGlobals::DeleteStuff();
  //
  CloseSubforms();
  MainGraphicsForm->Close();
  MainForm->Close();
}

void TMainForm::CloseSubforms()
{
  if( VarShowForm )VarShowForm->Close();
  if( VarInfoForm )VarInfoForm->Close();
  if( ReadVecForm )ReadVecForm->Close();
  if( ReadScalForm )ReadScalForm->Close();
  if( ReadMatForm )ReadMatForm->Close();
  if( ReadTxtForm )ReadTxtForm->Close();
  if( ExecForm )ExecForm->Close();
  if( WriteVecForm )WriteVecForm->Close();
  if( WriteScalForm )WriteScalForm->Close();
  if( WriteMatForm )WriteMatForm->Close();
  if( WriteTxtForm )WriteTxtForm->Close();
  if( FitCommandForm )FitCommandForm->Close();
  if( FitNoviceForm )FitNoviceForm->Close();
  if( SelectLineWidthForm )SelectLineWidthForm->Close();
  if( FigureDrawForm )FigureDrawForm->Close();
  if( PeakFindingForm )PeakFindingForm->Close();
  if( GraphCommandForm )GraphCommandForm->Close();
  if( GraphNoviceForm )GraphNoviceForm->Close();
  if( GraphicsSubwindowForm )GraphicsSubwindowForm->Close();
  if( TextCommandForm )TextCommandForm->Close();
  if( GenerateCmndForm )GenerateCmndForm->Close();
  if( StackForm )StackForm->Close();
  if( SetAspectRatioForm )SetAspectRatioForm->Close();
  if( ThreeDPlotForm )ThreeDPlotForm->Close();
}

void __fastcall TMainForm::HelpContents( TObject *Sender )
{
  HelpContents();
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack("HELP");
}

AnsiString TMainForm::GetHelpFileName() const
{ return helpFileName_; }

void TMainForm::HelpContents()
{
  std::ifstream tmp;
  AnsiString path( ExtractFilePath(Application->ExeName) );
  AnsiString helpFile = path + helpFileName_;
  tmp.open( helpFile.c_str(), ios_base::in );
  if( !tmp )
  {
    AnsiString s( "unable to open help file: " );
    s += helpFile;
    Application->MessageBox( s.c_str(), "ERROR", MB_OK );
  }
  else
  {
    tmp.close();
    HtmlHelp( MainForm->Handle, helpFile.c_str(), HH_DISPLAY_TOPIC, NULL );
  }
}

void __fastcall TMainForm::HelpAbout( TObject *Sender )
{
  std::auto_ptr<TAboutBox> aboutBox( new TAboutBox(this) );
  aboutBox->ShowModal();
}

void __fastcall TMainForm::EntrCmndButtonClick( TObject *Sender )
{ DoACommand(); }

void __fastcall TMainForm::MainEditKeyDown( TObject *Sender, WORD &Key, TShiftState Shift )
{
  if( Key == VK_UP )  // display previous command
  {
    AnsiString temp( GetPreviousCommand() );
    if( !temp.IsEmpty() )
    {
      temp += ' ';
      MainEdit->SetTextBuf( const_cast<char*>(temp.c_str()) );
      MainEdit->SelStart = temp.Length();
    }
  }
  else if( Key == VK_DOWN )
  {
    AnsiString temp( GetNextCommand() );
    if( temp.IsEmpty() )
      MainEdit->Clear();
    else
    {
      MainEdit->SetTextBuf( const_cast<char*>(temp.c_str()) );
      MainEdit->SelStart = temp.Length();
    }
  }
  else if( Key == VK_RETURN )
  {
    DoACommand();
  }
}

void __fastcall TMainForm::DoACommand()
{
  // this function is called only by MainForm
  //
  ExString commandLine;
  commandLine = GetACommand();
  if( commandLine.empty() || commandLine==" " )
  {
    if( ExGlobals::GetPausingScript() )ExGlobals::RestartScripts();
    return;
  }
  commandStrings_.push_back( commandLine );
  commandStringsIndex_ = commandStrings_.size()-1;
  try
  {
    ExGlobals::ProcessCommand( commandLine );
  }
  catch( runtime_error &e )
  {
    Application->MessageBox( e.what(), "ERROR", MB_OK );
  }
  if( ExGlobals::GetExecuteCommand() )
  {
    // the script run here must be the top level script
    // since it is run interactively
    //
    try
    {
      ExGlobals::RunScript();
    }
    catch ( runtime_error &e )
    {
      ExGlobals::ShowScriptError( e.what() );
      ExGlobals::StopAllScripts();
    }
  }
}

ExString TMainForm::GetACommand()
{
  ExString commandLine;
  bool flag = true;
  int inputLineLen;
  while ( flag )
  {
    inputLineLen = MainEdit->GetTextLen();
    if( inputLineLen == 0 )
    {
      if( !ExGlobals::GetExecuteFlag() )WriteOutput(" ");
      MainEdit->Clear();
      commandLine.clear();
      return commandLine;
    }
    char *c = new char[++inputLineLen];
    MainEdit->GetTextBuf( c, inputLineLen );
    ExString inputLine(c);
    delete [] c;
    WriteOutput( inputLine );
    //
    // trim leading and trailing blanks
    //
    std::size_t rTemp = inputLine.find_last_not_of( ' ' );
    if( rTemp != inputLine.npos() )
    {
      std::size_t lTemp = inputLine.find_first_not_of(' ');
      if( lTemp != inputLine.npos() )inputLine = inputLine.substr( lTemp, rTemp-lTemp+1 );
    }
    else inputLine = ExString(" ");
    inputLineLen = inputLine.length();
    //
    flag = false;
    if( inputLine[inputLineLen-1] == ExGlobals::GetContinuationCharacter() )
    {
      if( --inputLineLen == 0 )
      {
        MainEdit->Clear();
        throw runtime_error( "continuation line is empty" );
      }
      inputLine.erase( inputLine.length()-1, 1 );  // erase continuation character
      flag = true;
    }
    commandLine += inputLine;
  }
  if( commandLine.empty() || commandLine==" " && !ExGlobals::GetExecuteFlag() )WriteOutput(" ");
  ExGlobals::PreParse( commandLine );
  MainEdit->Clear();
  return commandLine;
}

void __fastcall TMainForm::ClearMainListBox( TObject *Sender )
{
  ClearMainListBox();
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack("CLEAR\\HISTORY");
}

void __fastcall TMainForm::PrintMainListBox( TObject *Sender )
{
  if( ListBoxPrintDialog->Execute() )
  {
    int count = MainListBox->Items->Count;
    TPrinter *p = Printer();
    p->BeginDoc();
    p->Canvas->Font = MainListBox->Font;
    int y = 200;
    for( int i=0; i<count; ++i )
    {
      AnsiString s( MainListBox->Items->Strings[i] );
      int h = p->Canvas->TextHeight(s);
      y += h+5;
      if( y > p->PageHeight-400 )
      {
        p->NewPage();
        y = 200+h;
      }
      p->Canvas->TextOut(200,y,s);
    }
    p->EndDoc();
  }
}

std::ostream &operator<<( std::ostream &out, TMainForm const &mf )
{
  bool b = (FitCommandForm!=0)||(FitNoviceForm!=0);
  out << "<analysiswindow top=\"" << mf.Top << "\" left=\"" << mf.Left << "\" height=\"" << mf.Height
      << "\" width=\"" << mf.Width << "\" novicemode=\"" << ExGlobals::GetNoviceMode()
      << "\" varshow=\"" << (VarShowForm!=0)
      << "\" readvec=\"" << (ReadVecForm!=0) << "\" readscal=\"" << (ReadScalForm!=0)
      << "\" readmat=\"" << (ReadMatForm!=0) << "\" readtxt=\"" << (ReadTxtForm!=0)
      << "\" exec=\"" << (ExecForm!=0) << "\" writevec=\"" << (WriteVecForm!=0)
      << "\" writescal=\"" << (WriteScalForm!=0) << "\" writemat=\"" << (WriteMatForm!=0)
      << "\" writetxt=\"" << (WriteTxtForm!=0)
      << "\" fit=\"" << b
      << "\" generate=\"" << (GenerateCmndForm!=0) << "\">\n";
  //
  int count = mf.MainListBox->Items->Count;
  if( count > 0 )
  {
    out << "<messages size=\"" << count << "\">\n";
    for( int i=0; i<count; ++i )
      out << "<string><![CDATA[" << ExString(mf.MainListBox->Items->Strings[i].c_str())
          << "]]></string>\n";
    out << "</messages>\n";
  }
  count = mf.commandStrings_.size();
  if( count > 0 )
  {
    out << "<commands size=\"" << count << "\" index=\"" << mf.commandStringsIndex_ << "\">\n";
    for( int i=0; i<count; ++i )
      out << "<string><![CDATA[" << mf.commandStrings_[i] << "]]></string>\n";
    out << "</commands>\n";
  }
  return out << "</analysiswindow>\n";
}

void TMainForm::AddCommandString( ExString const &s )
{
  commandStrings_.push_back( s );
  commandStringsIndex_ = commandStrings_.size()-1;
}

void __fastcall TMainForm::ClearMainListBox()
{
  MainListBox->Clear();
  listBoxWidth_ = MainListBox->Width*9/10;
  MainListBox->Perform( LB_SETHORIZONTALEXTENT, listBoxWidth_, 0 );
  //std::vector<ExString>().swap( commandStrings_ );
  //commandStringsIndex_ = -1;
}

void __fastcall TMainForm::CreateVarShowForm( TObject *Sender )
{
  if( VarShowForm )
  {
    VarShowForm->RefreshStringGrid();
    VarShowForm->WindowState = wsNormal;
    VarShowForm->Visible = true;
    VarShowForm->BringToFront();
  }
  else
  {
    VarShowForm = new TVarShowForm(this);
    VarShowForm->Show();
  }
}

void __fastcall TMainForm::CreateReadVecForm( TObject *Sender )
{
  if( ReadVecForm )
  {
    ReadVecForm->WindowState = wsNormal;
    ReadVecForm->Visible = true;
    ReadVecForm->BringToFront();
  }
  else
  {
    ReadVecForm = new TReadVecForm(this);
    ReadVecForm->Show();
  }
}

void __fastcall TMainForm::CreateReadScalForm( TObject *Sender )
{
  if( ReadScalForm )
  {
    ReadScalForm->WindowState = wsNormal;
    ReadScalForm->Visible = true;
    ReadScalForm->BringToFront();
  }
  else
  {
    ReadScalForm = new TReadScalForm(this);
    ReadScalForm->Show();
  }
}

void __fastcall TMainForm::CreateReadMatForm( TObject *Sender )
{
  if( ReadMatForm )
  {
    ReadMatForm->WindowState = wsNormal;
    ReadMatForm->Visible = true;
    ReadMatForm->BringToFront();
  }
  else
  {
    ReadMatForm = new TReadMatForm(this);
    ReadMatForm->Show();
  }
}

void __fastcall TMainForm::CreateReadTxtForm( TObject *Sender )
{
  if( ReadTxtForm )
  {
    ReadTxtForm->WindowState = wsNormal;
    ReadTxtForm->Visible = true;
    ReadTxtForm->BringToFront();
  }
  else
  {
    ReadTxtForm = new TReadTxtForm(this);
    ReadTxtForm->Show();
  }
}

void TMainForm::SetTheFocus()
{ MainEdit->SetFocus(); }

void __fastcall TMainForm::SetMainEditFocus( TObject *Sender )
{ MainEdit->SetFocus(); }

void __fastcall TMainForm::CreateWriteVecForm( TObject *Sender )
{
  if( WriteVecForm )
  {
    WriteVecForm->WindowState = wsNormal;
    WriteVecForm->Visible = true;
    WriteVecForm->BringToFront();
  }
  else
  {
    WriteVecForm = new TWriteVecForm(this);
    WriteVecForm->Show();
  }
}

void __fastcall TMainForm::CreateWriteScalForm( TObject *Sender )
{
  if( WriteScalForm )
  {
    WriteScalForm->WindowState = wsNormal;
    WriteScalForm->Visible = true;
    WriteScalForm->BringToFront();
  }
  else
  {
    WriteScalForm = new TWriteScalForm(this);
    WriteScalForm->Show();
  }
}

void __fastcall TMainForm::CreateWriteMatForm( TObject *Sender )
{
  if( WriteMatForm )
  {
    WriteMatForm->WindowState = wsNormal;
    WriteMatForm->Visible = true;
    WriteMatForm->BringToFront();
  }
  else
  {
    WriteMatForm = new TWriteMatForm(this);
    WriteMatForm->Show();
  }
}

void __fastcall TMainForm::CreateWriteTxtForm( TObject *Sender )
{
  if( WriteTxtForm )
  {
    WriteTxtForm->WindowState = wsNormal;
    WriteTxtForm->Visible = true;
    WriteTxtForm->BringToFront();
  }
  else
  {
    WriteTxtForm = new TWriteTxtForm(this);
    WriteTxtForm->Show();
  }
}

void __fastcall TMainForm::CreateFitForm( TObject *Sender )
{
  if( ExGlobals::GetNoviceMode() )
  {
    if( FitNoviceForm )
    {
      FitNoviceForm->WindowState = wsNormal;
      FitNoviceForm->Visible = true;
      FitNoviceForm->BringToFront();
    }
    else
    {
      FitNoviceForm = new TFitNoviceForm(this);
      FitNoviceForm->Show();
    }
  }
  else
  {
    if( FitCommandForm )
    {
      FitCommandForm->WindowState = wsNormal;
      FitCommandForm->Visible = true;
      FitCommandForm->BringToFront();
    }
    else
    {
      FitCommandForm = new TFitCommandForm(this);
      FitCommandForm->Show();
    }
  }
}

void __fastcall TMainForm::CreateGenerateCommandForm( TObject *Sender )
{
  if( GenerateCmndForm )
  {
    GenerateCmndForm->WindowState = wsNormal;
    GenerateCmndForm->Visible = true;
    GenerateCmndForm->BringToFront();
  }
  else
  {
    GenerateCmndForm = new TGenerateCmndForm(this);
    GenerateCmndForm->Show();
  }
}

void __fastcall TMainForm::CreateExecForm( TObject *Sender )
{
  if( ExecForm )
  {
    ExecForm->WindowState = wsNormal;
    ExecForm->Visible = true;
    ExecForm->BringToFront();
  }
  else
  {
    ExecForm = new TExecForm(this);
    ExecForm->Show();
  }
}

void __fastcall TMainForm::SaveSessionClick( TObject *Sender )
{
  bool temp = ForceCurrentDirectory;
  ForceCurrentDirectory = true;
  if( SaveSessionDialog->Execute() )
  {
    ForceCurrentDirectory = temp;
    ExString fileName( SaveSessionDialog->FileName.c_str() );
    ExGlobals::SaveSession( fileName );
  }
}

void __fastcall TMainForm::RestoreSessionClick( TObject *Sender )
{
  bool temp = ForceCurrentDirectory;
  ForceCurrentDirectory = true;
  if( RestoreSessionDialog->Execute() )
  {
    ForceCurrentDirectory = temp;
    ExString fileName( RestoreSessionDialog->FileName.c_str() );
    try
    {
      ExGlobals::RestoreSession( fileName );
    }
    catch (runtime_error &e)
    {
      Application->MessageBox( e.what(), "ERROR", MB_OK );
    }
  }
}

void __fastcall TMainForm::StackCommandsClick( TObject *Sender )
{
  if( StackForm )
  {
    StackForm->WindowState = wsNormal;
    StackForm->Visible = true;
    StackForm->BringToFront();
  }
  else
  {
    StackForm = new TStackForm( this );
    StackForm->Show();
  }
}

void __fastcall TMainForm::ConstantsClick( TObject *Sender )
{
  ExGlobals::DefineConstants();
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack("DEFINE\\CONSTANTS");
}

void __fastcall TMainForm::NoviceButtonClick(TObject *Sender)
{
  ExGlobals::ToggleNoviceMode();
  NoviceModeButton->Caption = ExGlobals::GetNoviceMode() ? "Expert mode is OFF" : "Expert mode is ON";
}

void __fastcall TMainForm::MainListBoxKeyDown( TObject *Sender, WORD &Key, TShiftState Shift )
{
  if( Shift.Contains(ssCtrl) && Key=='C' )
  {
    AnsiString s;
    int count = MainListBox->Items->Count;
    for( int i=0; i<count; ++i )
    {
      if( MainListBox->Selected[i] )
      {
        s += MainListBox->Items->Strings[i];
        s += "\r\n";
      }
    }
    Clipboard()->SetTextBuf( s.c_str() );
  }
  else if( Shift.Contains(ssCtrl) && Key=='A' )
  {
    int count = MainListBox->Items->Count;
    for( int i=0; i<count; ++i )
      MainListBox->Selected[i] = true;
  }
}

void __fastcall TMainForm::MainListBoxMouseDown( TObject *Sender,
       TMouseButton Button, TShiftState Shift, int X, int Y )
{
  if( Button == mbRight )
  {
    bool temp = ForceCurrentDirectory;
    ForceCurrentDirectory = true;
    MainListBoxSaveDialog->Title = "Save command history"; 
    if( MainListBoxSaveDialog->Execute() )
    {
      ForceCurrentDirectory = temp;
      ExString filename( MainListBoxSaveDialog->FileName.c_str() );
      std::ofstream out;
      std::ios_base::openmode mode = std::ios_base::out|std::ios_base::trunc;
      out.open( filename.c_str(), mode );
      if( !out )
      {
        ExString tmp("Unable to open file ");
        tmp += filename;
        Application->MessageBox( tmp.c_str(), "ERROR", MB_OK );
      }
      int count = MainListBox->Items->Count;
      for( int i=0; i<count; ++i )out << MainListBox->Items->Strings[i].c_str() << "\n";
      out.close();
    }
  }
}

// end of file

