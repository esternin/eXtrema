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
#ifndef AxisPopupH
#define AxisPopupH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

class GRA_page;
class GRA_window;
class GRA_setOfCharacteristics;

class TAxisPopupForm : public TForm
{
__published:	// IDE-managed Components
  TButton *CloseButton;
  TPageControl *TopPageControl;
  TTabSheet *AxisTabSheet;
  TCheckBox *AxisOnCheckBox;
  TCheckBox *DrawGraphBoxCheckBox;
  TLabel *AxisColorLabel;
  TShape *AxisColorShape;
  TLabel *AxisLinewidthLabel;
  TEdit *AxisLinewidthEdit;
  TUpDown *AxisLinewidthUpDown;
  TCheckBox *TicsOnCheckBox;
  TLabel *LTicLenLabel;
  TEdit *LTicLenEdit;
  TUpDown *LTicLenUpDown;
  TLabel *NsincsLabel;
  TEdit *NsincsEdit;
  TUpDown *NsincsUpDown;
  TLabel *STicLenLabel;
  TEdit *STicLenEdit;
  TUpDown *STicLenUpDown;
  TPanel *Panel1;
  TLabel *GridLineTypeLabel;
  TRadioButton *GridNoneRB;
  TRadioButton *GridMajorRB;
  TRadioButton *GridAllRB;
  TEdit *GridLineTypeEdit;
  TUpDown *GridLineTypeUpDown;
  TTabSheet *NumbersTabSheet;
  TCheckBox *NumbersOnCheckBox;
  TLabel *NumbersFontLabel;
  TComboBox *NumbersFontComboBox;
  TLabel *NumbersColorLabel;
  TShape *NumbersColorShape;
  TLabel *NumbersHeightLabel;
  TEdit *NumbersHeightEdit;
  TUpDown *NumbersHeightUpDown;
  TLabel *NumbersAngleLabel;
  TEdit *NumbersAngleEdit;
  TUpDown *NumbersAngleUpDown;
  TTabSheet *LabelTabSheet;
  TCheckBox *LabelOnCheckBox;
  TLabel *LabelLabel;
  TEdit *LabelEdit;
  TLabel *LabelFontLabel;
  TComboBox *LabelFontComboBox;
  TLabel *LabelColorLabel;
  TShape *LabelColorShape;
  TLabel *LabelHeightLabel;
  TEdit *LabelHeightEdit;
  TUpDown *LabelHeightUpDown;
  TTabSheet *LocationTabSheet;
  TLabel *LocationLowLabel;
  TEdit *LocationLowEdit;
  TUpDown *LocationLowUpDown;
  TLabel *LocationUpLabel;
  TEdit *LocationUpEdit;
  TUpDown *LocationUpUpDown;
  TCheckBox *LogStyleCheckBox;
  TTabSheet *ScalesTabSheet;
  TLabel *MinLabel;
  TEdit *MinEdit;
  TLabel *MaxLabel;
  TEdit *MaxEdit;
  TRadioGroup *LogRadioGroup;
  TButton *RescaleButton;
  TRadioGroup *TicRadioGroup;
  TBevel *Bevel1;
  void __fastcall OnCloseClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
  void __fastcall OnAxisColorMouseDown(TObject *Sender,TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall OnNumbersColorMouseDown(TObject *Sender,TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall OnApplyRescaling(TObject *Sender);
  void __fastcall OnLabelColorMouseDown(TObject *Sender,TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall OnSelectNumbersFont(TObject *Sender);
  void __fastcall OnNumbersAngleUpDownClick(TObject *Sender, TUDBtnType Button);
  void __fastcall OnNumbersAngleChange(TObject *Sender);
  void __fastcall OnNumbersHeightUpDownClick(TObject *Sender, TUDBtnType Button);
  void __fastcall OnNumbersHeightEditChange(TObject *Sender);
  void __fastcall OnDisplayNumbersClick(TObject *Sender);
  void __fastcall OnDrawAxisClick(TObject *Sender);
  void __fastcall OnAxisLinewidthUpDownClick(TObject *Sender,TUDBtnType Button);
  void __fastcall OnAxisLinewidthChange(TObject *Sender);
  void __fastcall OnNsincsChange(TObject *Sender);
  void __fastcall OnNsincsClick(TObject *Sender, TUDBtnType Button);
  void __fastcall OnLTicLenChange(TObject *Sender);
  void __fastcall OnLTicLenClick(TObject *Sender, TUDBtnType Button);
  void __fastcall OnSTicLenChange(TObject *Sender);
  void __fastcall OnSTicLenClick(TObject *Sender, TUDBtnType Button);
  void __fastcall OnDisplayTics(TObject *Sender);
  void __fastcall OnDisplayLabelClick(TObject *Sender);
  void __fastcall OnSelectLabelFont(TObject *Sender);
  void __fastcall OnLabelHeightEditChange(TObject *Sender);
  void __fastcall OnLabelHeightUpDownClick(TObject *Sender,TUDBtnType Button);
  void __fastcall OnLocationLowEditChange(TObject *Sender);
  void __fastcall OnLocationLowUpDownClick(TObject *Sender,TUDBtnType Button);
  void __fastcall OnLocationUpEditChange(TObject *Sender);
  void __fastcall OnLocationUpUpDownClick(TObject *Sender,TUDBtnType Button);
  void __fastcall OnAxisLabelEditExit(TObject *Sender);
  void __fastcall OnAxisLabelEditKeyPress(TObject *Sender, char &Key);
  void __fastcall OnDrawGraphBoxClick(TObject *Sender);
  void __fastcall OnNoGridLinesClick(TObject *Sender);
  void __fastcall OnMajorTicGridLinesClick(TObject *Sender);
  void __fastcall OnAllTicsGridLinesClick(TObject *Sender);
  void __fastcall OnGridLineTypeEditChange(TObject *Sender);
  void __fastcall OnGridLineTypeUpDownClick(TObject *Sender,TUDBtnType Button);
  void __fastcall OnLogClick(TObject *Sender);
  void __fastcall OnLogStyleClick(TObject *Sender);
  void __fastcall OnTicDirectionClick(TObject *Sender);

private:	// User declarations
  GRA_page *page_;
  GRA_window *window_;
  GRA_setOfCharacteristics *axisChars_;
  int xOffset_, yOffset_;
  bool setup_;
  char which_;

  void ReDraw();

public:		// User declarations
  __fastcall TAxisPopupForm(TComponent* Owner);
  __fastcall ~TAxisPopupForm();

  void Setup( GRA_page *, GRA_window *, char const );
};

extern PACKAGE TAxisPopupForm *AxisPopupForm;

#endif
