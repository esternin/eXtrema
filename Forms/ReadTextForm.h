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
#ifndef ReadTextFormH
#define ReadTextFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>

class TReadTxtForm : public TForm
{
__published:	// IDE-managed Components
  TPanel *OptionsPanel;
  TShape *Shape3;
  TCheckBox *DisplayMessagesCheckBox;
  TCheckBox *CloseFirstCheckBox;
  TCheckBox *CloseAfterCheckBox;
  TPanel *LineRangePanel;
  TLabel *LineRange1Label;
  TLabel *LineRange2Label;
  TLabel *LineRange3Label;
  TEdit *LineRange1Edit;
  TEdit *LineRange2Edit;
  TEdit *LineRange3Edit;
  TCheckBox *LineRangeCheckBox;
  TButton *ChooseFileButton;
  TComboBox *FilenameComboBox;
  TButton *ApplyButton;
  TButton *CloseButton;
  TPanel *Panel1;
  TLabel *TextNameLabel;
  TEdit *TextNameEdit;
  TOpenDialog *ReadDialog;
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall ChooseFile(TObject *Sender);
  void __fastcall FilenameComboBoxClick(TObject *Sender);
  void __fastcall ApplyClick(TObject *Sender);
  void __fastcall LineRangeCheckBoxClicked(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);

private:	// User declarations

public:		// User declarations
  __fastcall TReadTxtForm(TComponent* Owner);
  __fastcall ~TReadTxtForm();

};

extern PACKAGE TReadTxtForm *ReadTxtForm;

#endif
