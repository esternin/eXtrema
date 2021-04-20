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
#ifndef GenerateCommandFormH
#define GenerateCommandFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

class ExString;

class TGenerateCmndForm : public TForm
{
__published:	// IDE-managed Components
  TPanel *Panel1;
  TEdit *MinEdit;
  TLabel *MinLabel;
  TEdit *NameEdit;
  TLabel *NameLabel;
  TEdit *IncEdit;
  TLabel *IncLabel;
  TEdit *MaxEdit;
  TLabel *MaxLabel;
  TEdit *NumEdit;
  TLabel *NumLabel;
  TRadioGroup *ChoicesRadioGroup;
  TButton *DefaultsButton;
  TButton *CloseButton;
  TButton *ApplyButton;
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall DefaultsClick(TObject *Sender);
  void __fastcall ApplyClick(TObject *Sender);
  void __fastcall ChoicesClick(TObject *Sender);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);

private:	// User declarations
  void __fastcall SetUp();
  
public:		// User declarations
  __fastcall TGenerateCmndForm(TComponent* Owner);
  __fastcall ~TGenerateCmndForm();
  void Set( ExString const &, int, double, double, double, int, ExString const & );

};

extern PACKAGE TGenerateCmndForm *GenerateCmndForm;

#endif
