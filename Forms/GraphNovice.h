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
#ifndef GraphNoviceH
#define GraphNoviceH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "ExString.h"

class TGraphNoviceForm : public TForm
{
__published:	// IDE-managed Components
  TComboBox *DepVarComboBox;
  TComboBox *IndepVarComboBox;
  TLabel *DepVarLabel;
  TLabel *IndepVarLabel;
  TComboBox *ErrorsComboBox;
  TLabel *ErrorsLabel;
  TButton *DrawGraphButton;
  TButton *ClearButton;
  TButton *CloseButton;
  TLabel *PlotSymbolLabel;
  TComboBox *PlotSymbolComboBox;
  TLabel *IVMinLabel;
  TLabel *IVMaxLabel;
  TEdit *IVMinEdit;
  TEdit *IVMaxEdit;
  TCheckBox *ConnectCheckBox;
  void __fastcall DrawGraphClick(TObject *Sender);
  void __fastcall ClearClick(TObject *Sender);
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall UpdateVars(TObject *Sender);
  void __fastcall IndepVarChange(TObject *Sender);
  void __fastcall PlotSymbolChange(TObject *Sender);

private:	// User declarations
  int color_;
  std::vector<int> symArray_;
  void __fastcall FillOutForm();

public:		// User declarations
  __fastcall TGraphNoviceForm( TComponent* Owner );
  __fastcall ~TGraphNoviceForm();
  void Set( ExString const &, ExString const &, double, double,
            ExString const &, int, ExString const & );
};

extern PACKAGE TGraphNoviceForm *GraphNoviceForm;

#endif
