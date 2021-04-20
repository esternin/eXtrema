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
#ifndef PH_VARSHOW
#define PH_VARSHOW

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>

class TVarShowForm : public TForm
{
__published:	// IDE-managed Components
  TPanel *VarShowPanel;
  TStringGrid *VarShowStringGrid;
  TButton *VarShowRefreshBtn;
  TButton *VarShowCloseBtn;
  void __fastcall RefreshStringGrid(TObject *Sender);
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall DisplayInfo(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);

private:	// User declarations
  void __fastcall FillStringGrid();
  void __fastcall SetRows();

public:		// User declarations
  __fastcall TVarShowForm(TComponent* Owner);
  __fastcall ~TVarShowForm();
  void RefreshStringGrid();
};

extern PACKAGE TVarShowForm *VarShowForm;

#endif
