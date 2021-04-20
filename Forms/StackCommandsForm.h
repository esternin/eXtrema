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
#ifndef StackCommandsFormH
#define StackCommandsFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

class TStackForm : public TForm
{
__published:	// IDE-managed Components
  TOpenDialog *StackDialog;
  TComboBox *FilenameComboBox;
  TButton *CloseButton;
  TButton *BrowseButton;
  TCheckBox *AppendCheckBox;
  TSpeedButton *EmptyListSpeedButton;
  TSpeedButton *RemoveFileSpeedButton;
  TRadioGroup *OnOffRadioGroup;
  TButton *ApplyButton;
  void __fastcall BrowseButtonClick(TObject *Sender);
  void __fastcall CloseButtonClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);
  void __fastcall ClearFileList(TObject *Sender);
  void __fastcall RemoveFilename(TObject *Sender);
  void __fastcall ToggleOnOff(TObject *Sender);
  void __fastcall ApplyButtonClick(TObject *Sender);

private:	// User declarations
  void AddFileToList( AnsiString );

public:		// User declarations
  __fastcall TStackForm(TComponent* Owner);
  __fastcall ~TStackForm();
};

extern PACKAGE TStackForm *StackForm;

#endif
