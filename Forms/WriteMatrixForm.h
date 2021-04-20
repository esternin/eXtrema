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
#ifndef WriteMatrixFormH
#define WriteMatrixFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <CheckLst.hpp>
#include <Dialogs.hpp>

class TWriteMatForm : public TForm
{
__published:	// IDE-managed Components
  TCheckListBox *NamesCheckListBox;
  TButton *ChooseFileButton;
  TComboBox *FilenameComboBox;
  TCheckBox *AppendCheckBox;
  TButton *ApplyButton;
  TButton *CloseButton;
  TOpenDialog *WriteDialog;
  TButton *UpdateButton;
  void __fastcall ApplyClick(TObject *Sender);
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall ChooseFileClick(TObject *Sender);
  void __fastcall UpdateClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);

private:	// User declarations
  void __fastcall FillOutListBox();

public:		// User declarations
  __fastcall TWriteMatForm(TComponent* Owner);
  __fastcall ~TWriteMatForm();

};

extern PACKAGE TWriteMatForm *WriteMatForm;

#endif
