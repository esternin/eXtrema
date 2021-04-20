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
#ifndef ExecuteFormH
#define ExecuteFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <Buttons.hpp>

class ExString;

class TExecForm : public TForm
{
__published:	// IDE-managed Components
  TOpenDialog *ExecuteDialog;
  TLabel *EnterLabel;
  TEdit *ParameterEdit;
  TButton *ChooseFileButton;
  TComboBox *FilenameComboBox;
  TButton *ApplyButton;
  TButton *CloseButton;
  TSpeedButton *EmptyListSpeedButton;
  TSpeedButton *RemoveFileSpeedButton;
  void __fastcall ChooseFile(TObject *Sender);
  void __fastcall ApplyClick(TObject *Sender);
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall ClearFileList(TObject *Sender);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);
  void __fastcall RemoveFilename(TObject *Sender);
  void __fastcall DeleteFilename(TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall ReturnKeyPress(TObject *Sender, char &Key);

private:	// User declarations
  AnsiString currentPath_;
  void SetFile( AnsiString );

public:		// User declarations
  __fastcall TExecForm(TComponent* Owner);
  __fastcall ~TExecForm();
  void Set( ExString const &, ExString const &, ExString const & );
};

extern PACKAGE TExecForm *ExecForm;

#endif
