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
#ifndef ReadVectorsFormH
#define ReadVectorsFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Buttons.hpp>

class TReadVecForm : public TForm
{
__published:	// IDE-managed Components
  TOpenDialog *ReadDialog;
  TButton *ChooseFileButton;
  TPanel *LineRangePanel;
  TEdit *LineRange1Edit;
  TLabel *LineRange1Label;
  TEdit *LineRange2Edit;
  TLabel *LineRange2Label;
  TEdit *LineRange3Edit;
  TLabel *LineRange3Label;
  TStringGrid *NameColumnStringGrid;
  TPanel *OptionsPanel;
  TCheckBox *MakeNewCheckBox;
  TCheckBox *AppendCheckBox;
  TCheckBox *DisplayMessagesCheckBox;
  TCheckBox *ErrorStopCheckBox;
  TCheckBox *ErrorFillCheckBox;
  TEdit *ErrorFillEdit;
  TCheckBox *CloseFirstCheckBox;
  TCheckBox *CloseAfterCheckBox;
  TButton *ApplyButton;
  TButton *CloseButton;
  TComboBox *FilenameComboBox;
  TCheckBox *LineRangeCheckBox;
  TPanel *FormatPanel;
  TCheckBox *FormatCheckBox;
  TEdit *FormatEdit;
  TShape *Shape1;
  TShape *Shape2;
  TShape *Shape3;
  TSpeedButton *EmptyListSpeedButton;
  TSpeedButton *RemoveFileSpeedButton;
  void __fastcall ChooseFile(TObject *Sender);
  void __fastcall MakeNewClick(TObject *Sender);
  void __fastcall StopOnErrorsClick(TObject *Sender);
  void __fastcall ErrorFillClick(TObject *Sender);
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall ApplyClick(TObject *Sender);
  void __fastcall NameColumnSetEditText(TObject *Sender, int ACol,
                                        int ARow, const AnsiString Value);
  void __fastcall LineRangeCheckBoxClicked(TObject *Sender);
  void __fastcall FormatCheckBoxClicked(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);
  void __fastcall ClearFileList(TObject *Sender);
  void __fastcall RemoveFilename(TObject *Sender);
  void __fastcall DeleteFilename(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall ReturnKeyPress(TObject *Sender, char &Key);

  //void __fastcall FilenameComboBoxClick(TObject *Sender);

private:	// User declarations

public:		// User declarations
  AnsiString currentPath_;
  __fastcall TReadVecForm(TComponent* Owner);
  __fastcall ~TReadVecForm();
};

extern PACKAGE TReadVecForm *ReadVecForm;

#endif
