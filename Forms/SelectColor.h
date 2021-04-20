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
#ifndef SelectColorH
#define SelectColorH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>

class GRA_color;

class TSelectColorForm : public TForm
{
__published:	// IDE-managed Components
  TDrawGrid *ColorDrawGrid;
  TLabel *ColorMapLabel;
  TRadioGroup *NamedColorRadioGroup;
  TButton *OKButton;
  TButton *CancelButton;
  void __fastcall GridClick(TObject *Sender);
  void __fastcall OnDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
  void __fastcall NamedColorRadioClick(TObject *Sender);
  void __fastcall OKButtonClick(TObject *Sender);
  void __fastcall CancelButtonClick(TObject *Sender);

private:	// User declarations
  bool gridClick_;
  bool namedColorClick_;

public:		// User declarations
  __fastcall TSelectColorForm(TComponent* Owner);
  __fastcall ~TSelectColorForm();

  void __fastcall SetColor( GRA_color * );
};

extern PACKAGE TSelectColorForm *SelectColorForm;

#endif
