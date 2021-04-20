/*
Copyright (C) 2007 Joseph L. Chuma, TRIUMF

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
#ifndef LegendPopupH
#define LegendPopupH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

class GRA_page;
class GRA_window;
class GRA_setOfCharacteristics;
class GRA_legend;

class TLegendPopupForm : public TForm
{
__published:	// IDE-managed Components
  TCheckBox *DrawLegendCheckBox;
  TGroupBox *TitleGroupBox;
  TLabel *TitleLabel;
  TEdit *TitleEdit;
  TLabel *TitleColorLabel;
  TShape *TitleColorShape;
  TLabel *TitleHeightLabel;
  TEdit *TitleHeightEdit;
  TUpDown *TitleHeightUpDown;
  TLabel *TitleFontLabel;
  TComboBox *TitleFontComboBox;
  TCheckBox *DrawTitleCheckBox;
  TGroupBox *FrameGroupBox;
  TLabel *FrameColorLabel;
  TShape *FrameColorShape;
  TCheckBox *DrawFrameCheckBox;
  TButton *CloseButton;
  TLabel *MoveLabel;
  TLabel *MoveHorizontalLabel;
  TTrackBar *MoveHorizontalTrackBar;
  TLabel *MoveVerticalLabel;
  TTrackBar *MoveVerticalTrackBar;
  TLabel *ExpandHorizontalLabel;
  TTrackBar *ExpandHorizontalTrackBar;
  TLabel *ExpandVerticalLabel;
  TTrackBar *ExpandVerticalTrackBar;
  TLabel *Label1;
  TLabel *MoveHLabel;
  TLabel *MoveVLabel;
  TLabel *ExpandHLabel;
  TLabel *ExpandVLabel;
  void __fastcall OnDrawLegendClick(TObject *Sender);
  void __fastcall OnDrawFrameClick(TObject *Sender);
  void __fastcall OnFrameColorMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall OnDrawTitleClick(TObject *Sender);
  void __fastcall OnTitleKeyPress(TObject *Sender, char &Key);
  void __fastcall OnTitleColorMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall OnTitleHeightUpDownClick(TObject *Sender,
          TUDBtnType Button);
  void __fastcall OnTitleHeightChange(TObject *Sender);
  void __fastcall OnTitleFontChange(TObject *Sender);
  void __fastcall OnCloseClick(TObject *Sender);
  void __fastcall OnCloseForm(TObject *Sender, TCloseAction &Action);
  void __fastcall OnCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall OnMoveHorizontally(TObject *Sender);
  void __fastcall OnMoveVertically(TObject *Sender);
  void __fastcall OnExpandHorizontally(TObject *Sender);
  void __fastcall OnExpandVertically(TObject *Sender);

private:	// User declarations
  GRA_page *page_;
  GRA_window *window_;
  GRA_setOfCharacteristics *legChars_;
  GRA_legend *legend_;
  bool setup_;

public:		// User declarations
  __fastcall TLegendPopupForm( TComponent* Owner );
  __fastcall ~TLegendPopupForm();

  void Disconnect();
  void Setup( GRA_page *, GRA_window *, GRA_legend * );
  void ReDraw();
};

extern PACKAGE TLegendPopupForm *LegendPopupForm;

#endif
