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
#ifndef DigitizeInfoH
#define DigitizeInfoH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

class TDigitizeInfoForm : public TForm
{
__published:	// IDE-managed Components
  TLabel *InstructionLabel1;
  TLabel *InstructionLabel2;
  TLabel *InstructionLabel3;
  TButton *AbortButton;

  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall AbortClick(TObject *Sender);

private:	// User declarations
  int n_;

public:		// User declarations
  __fastcall TDigitizeInfoForm(TComponent* Owner);
  __fastcall ~TDigitizeInfoForm();
  void __fastcall Next();
};

extern PACKAGE TDigitizeInfoForm *DigitizeInfoForm;

#endif
