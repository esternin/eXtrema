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
#ifndef extremaMainH
#define extremaMainH

#include <Buttons.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
#include <Clipbrd.hpp>

#include <ostream>
#include "ExString.h"

class TMainForm : public TForm
{
__published:
  TMainMenu *MainMenu;
  TMenuItem *Execute;
  TMenuItem *HelpContentsItem;
  TMenuItem *HelpAboutItem;
  TMenuItem *ReadFileMenuItem;
  TMenuItem *WriteFileMenuItem;
  TMenuItem *N4;
  TMenuItem *ReadVectorsSubMenuItem;
  TMenuItem *ReadScalarsSubMenuItem;
  TMenuItem *ReadMatrixSubMenuItem;
  TMenuItem *ReadTensorSubMenuItem;
  TMenuItem *ReadTextSubMenuItem;
  TMenuItem *WriteVectorsSubMenuItem;
  TMenuItem *WriteScalarsSubMenuItem;
  TMenuItem *WriteMatrixSubMenuItem;
  TMenuItem *WriteTensorSubMenuItem;
  TMenuItem *WriteTextSubMenuItem;
  //
  TLabel *Label1;
  TLabel *Label2;
  TOpenDialog *RestoreSessionDialog;
  //
  TPanel *SpeedBar;
  TSpeedButton *ExecuteSpeedButton;
  TSpeedButton *ExitButton;
  TSpeedButton *HelpCButton;
  TSpeedButton *ClearListBoxButton;
  //
  TStatusBar *StatusLine;
  TEdit *MainEdit;
  TListBox *MainListBox;
  TButton *EntrCmndButton;
  TSpeedButton *ShowSpeedButton;
  TSpeedButton *FitSpeedButton;
  TSpeedButton *GenerateSpeedButton;
  TMenuItem *ExitApp;
  TSpeedButton *PrintListBoxSpeedButton;
  TPrintDialog *ListBoxPrintDialog;
  TSpeedButton *SaveSessionSpeedButton;
  TSpeedButton *RestoreSessionSpeedButton;
  TSaveDialog *SaveSessionDialog;
  TSpeedButton *DefineConstantsSpeedButton;
  TButton *NoviceModeButton;
  TSpeedButton *StackSpeedButton;
  TSaveDialog *MainListBoxSaveDialog;
  //
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall ShowHint(TObject *Sender);
  void __fastcall FileNew(TObject *Sender);
  void __fastcall CreateExecForm(TObject *Sender);
  void __fastcall QuitApp(TObject *Sender);
  void __fastcall HelpContents(TObject *Sender);
  void __fastcall HelpAbout(TObject *Sender);
  void __fastcall CreateVarShowForm(TObject *Sender);
  void __fastcall CreateReadScalForm(TObject *Sender);
  void __fastcall CreateReadVecForm(TObject *Sender);
  void __fastcall CreateReadMatForm(TObject *Sender);
  void __fastcall CreateReadTxtForm(TObject *Sender);
  void __fastcall CreateWriteScalForm(TObject *Sender);
  void __fastcall CreateWriteVecForm(TObject *Sender);
  void __fastcall CreateWriteMatForm(TObject *Sender);
  void __fastcall EntrCmndButtonClick(TObject *Sender);
  void __fastcall ClearMainListBox(TObject *Sender);
  void __fastcall MainEditKeyDown(TObject *Sender,WORD &Key,TShiftState Shift);
  void __fastcall SetMainEditFocus(TObject *Sender);
  void __fastcall CreateFitForm(TObject *Sender);
  void __fastcall CreateWriteTxtForm(TObject *Sender);
  void __fastcall CreateGenerateCommandForm(TObject *Sender);
  void __fastcall PrintMainListBox(TObject *Sender);
  void __fastcall SaveSessionClick(TObject *Sender);
  void __fastcall RestoreSessionClick(TObject *Sender);
  void __fastcall ConstantsClick(TObject *Sender);
  void __fastcall NoviceButtonClick(TObject *Sender);
  void __fastcall MainListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall StackCommandsClick(TObject *Sender);
  void __fastcall MainListBoxMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);

public:
  __fastcall TMainForm(TComponent *Owner);
  __fastcall ~TMainForm();
  //
  ExString GetACommand();
  void QuitApp();
  AnsiString GetHelpFileName() const;
  void HelpContents();
  void __fastcall ClearMainListBox();
  void WriteOutput( ExString const & );
  void SetDimensions( int, int, int, int );
  void AddCommandString( ExString const &s );
  void CloseSubforms();
  void SetTheFocus();

  friend std::ostream &operator<<( std::ostream &, TMainForm const & );

private:
  AnsiString helpFileName_;
  int listBoxWidth_;
  std::vector<ExString> commandStrings_;
  int commandStringsIndex_;

  void __fastcall DoACommand();

  AnsiString GetPreviousCommand()
  {
    AnsiString s;
    if( commandStringsIndex_ >= 0 )s = commandStrings_[commandStringsIndex_--].c_str();
    return s;
  }

  AnsiString GetNextCommand()
  {
    AnsiString s;
    if( commandStringsIndex_ < static_cast<int>(commandStrings_.size())-1 )
      s = commandStrings_[++commandStringsIndex_].c_str();
    return s;
  }
};

extern PACKAGE TMainForm *MainForm;

#endif

