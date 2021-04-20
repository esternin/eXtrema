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
#ifndef ReadMatrixFormH
#define ReadMatrixFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>

class TReadMatForm : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TEdit *MatrixNameEdit;
  TLabel *MatrixNameLabel;
  TEdit *NumRowsEdit;
  TLabel *NumRowsLabel;
  TEdit *NumColmsEdit;
  TLabel *NumColmsLabel;
  TPanel *FormatPanel;
  TCheckBox *FormatCheckBox;
  TEdit *FormatEdit;
  TPanel *OptionsPanel;
  TShape *Shape3;
  TCheckBox *DisplayMessagesCheckBox;
  TCheckBox *CloseFirstCheckBox;
  TCheckBox *CloseAfterCheckBox;
  TPanel *LineRangePanel;
  TLabel *LineRangeLabel;
  TEdit *LineRangeEdit;
  TButton *ChooseFileButton;
  TComboBox *FilenameComboBox;
  TOpenDialog *ReadDialog;
  TButton *ApplyButton;
  TButton *CloseButton;
  void __fastcall ApplyClick(TObject *Sender);
  void __fastcall ChooseFileClick(TObject *Sender);
  void __fastcall FilenameComboBoxClick(TObject *Sender);
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);

private:	// User declarations

public:		// User declarations
  __fastcall TReadMatForm(TComponent* Owner);
  __fastcall ~TReadMatForm();

};

extern PACKAGE TReadMatForm *ReadMatForm;

#endif
