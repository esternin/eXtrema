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
#ifndef InquireH
#define InquireH
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>

#include <string>

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

class CMD_inquire;

class TInquireForm : public TForm
{
__published:	// IDE-managed Components
  TLabel *InquireLabel;
  TEdit *InquireEdit;
  TButton *OKButton;
  TButton *EndButton;
  void __fastcall OKButtonClick( TObject *Sender );
  void __fastcall StopScriptsButtonClick( TObject *Sender );
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall ReturnKeyPress(TObject *Sender, char &Key);

public:		// User declarations
  __fastcall TInquireForm( TComponent * );
  __fastcall ~TInquireForm();
  void SetLabel( ExString const & );
  void __fastcall ApplyIt();
};

extern PACKAGE TInquireForm *InquireForm;

#endif
