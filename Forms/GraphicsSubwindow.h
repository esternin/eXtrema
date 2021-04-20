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
#ifndef GraphicsSubwindowH
#define GraphicsSubwindowH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>

class TGraphicsSubwindowForm : public TForm
{
__published:	// IDE-managed Components
  TPageControl *PageControl1;
  TTabSheet *ChoosePredefinedWindowTabSheet;
  TStringGrid *WindowCoordStringGrid;
  TTabSheet *DefineOneWindowTabSheet;
  TPanel *DOWPanel;
  TLabel *WindowNumberLabel;
  TLabel *xloLabel;
  TLabel *yloLabel;
  TLabel *xupLabel;
  TLabel *yupLabel;
  TEdit *WindowNumberEdit;
  TCheckBox *InteractiveCheckBox;
  TEdit *xloEdit;
  TEdit *yloEdit;
  TEdit *xupEdit;
  TEdit *yupEdit;
  TTabSheet *DefineMultipleWindowsTabSheet;
  TPanel *Panel1;
  TLabel *NWHLabel;
  TLabel *NWVLabel;
  TLabel *TilingStartLabel;
  TEdit *TilingStartEdit;
  TEdit *NWHEdit;
  TEdit *NWVEdit;
  TButton *DOWApplyButton;
  TButton *CloseButton;
  TButton *DMWApplyButton;
  TLabel *DOWInheritLabel;
  TEdit *DOWInheritEdit;
  TLabel *InheritLabel;
  TEdit *InheritEdit;
  TButton *ApplyButton;
  void __fastcall InteractiveCheckBoxClicked(TObject *Sender);
  void __fastcall DOWApplyClick(TObject *Sender);
  void __fastcall DMWApplyClick(TObject *Sender);
  void __fastcall StringGridMouseUp(TObject *Sender, TMouseButton Button,
                                    TShiftState Shift, int X, int Y);
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);
  void __fastcall ApplyClick(TObject *Sender);
  void __fastcall WNEditExit(TObject *Sender);
  void __fastcall xloEditExit(TObject *Sender);
  void __fastcall yloEditExit(TObject *Sender);
  void __fastcall xupEditExit(TObject *Sender);
  void __fastcall yupEditExit(TObject *Sender);
  void __fastcall NWHEditExit(TObject *Sender);
  void __fastcall NWVEditExit(TObject *Sender);
  void __fastcall TilingStartEditExit(TObject *Sender);

private:	// User declarations
  int chosenWindow_;
  void __fastcall FillStringGrid();

public:		// User declarations
  __fastcall TGraphicsSubwindowForm(TComponent* Owner);
  __fastcall ~TGraphicsSubwindowForm();
  void __fastcall RefreshStringGrid();
  void __fastcall SetUp( double, double, double, double );
};

extern PACKAGE TGraphicsSubwindowForm *GraphicsSubwindowForm;

#endif
