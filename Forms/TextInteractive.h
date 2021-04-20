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
#ifndef TextInteractiveH
#define TextInteractiveH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <Buttons.hpp>
#include <Grids.hpp>

class ExString;

class TTextCommandForm : public TForm
{
__published:	// IDE-managed Components
  TButton *CloseButton;
  TButton *ApplyButton;
  TPanel *TextPanel1;
  TLabel *TextInteractiveLabel;
  TLabel *XLocLabel;
  TLabel *YLocLabel;
  TEdit *TextInteractiveEdit;
  TCheckBox *TextInteractiveCheckBox;
  TRadioGroup *TextAlignRadioGroup;
  TEdit *XLocEdit;
  TEdit *YLocEdit;
  TButton *FontButton;
  TButton *EraseButton;
  void __fastcall CloseButtonClick(TObject *Sender);
  void __fastcall DrawButtonClick(TObject *Sender);
  void __fastcall TICheckBoxClick(TObject *Sender);
  void __fastcall FontButtonClick(TObject *Sender);
  void __fastcall EraseButtonClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);

private:	// User declarations
  void __fastcall SetUp();

  enum {SIZE = 9};
  static int gwToRadio_[SIZE];
  static int radioToGw_[SIZE];

  bool fromSet_;

public:		// User declarations
  __fastcall TTextCommandForm(TComponent* Owner);
  __fastcall ~TTextCommandForm();

  void Set( ExString const &, bool, double, double, int, ExString const & );
};

extern PACKAGE TTextCommandForm *TextCommandForm;

#endif
