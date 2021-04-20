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
#ifndef CurvePopupH
#define CurvePopupH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

class GRA_page;
class GRA_window;
class GRA_setOfCharacteristics;
class GRA_cartesianCurve;

class TCurvePopupForm : public TForm
{
__published:	// IDE-managed Components
  TRadioGroup *HistogramRG;
  TLabel *CurveColorLabel;
  TShape *CurveColorShape;
  TLabel *CurveLinewidthLabel;
  TEdit *CurveLinewidthEdit;
  TUpDown *CurveLinewidthUpDown;
  TLabel *CurveLineTypeLabel;
  TEdit *CurveLineTypeEdit;
  TUpDown *CurveLineTypeUpDown;
  TGroupBox *PlotsymbolGroupBox;
  TLabel *PSCodeLabel;
  TEdit *PSCodeEdit;
  TUpDown *PSCodeUpDown;
  TCheckBox *PSConnectCheckBox;
  TLabel *PSSizeLabel;
  TEdit *PSSizeEdit;
  TUpDown *PSSizeUpDown;
  TLabel *PSColorLabel;
  TShape *PSColorShape;
  TLabel *PSAngleLabel;
  TEdit *PSAngleEdit;
  TUpDown *PSAngleUpDown;
  TButton *CloseButton;
  TLabel *AreaFillColorLabel;
  TShape *AreaFillColorShape;

  void __fastcall OnCurveColorMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnCurveLinewidthChange(TObject *Sender);
  void __fastcall OnCurveLinewidthClick(TObject *Sender,
          TUDBtnType Button);
  void __fastcall OnCurveLinetypeClick(TObject *Sender, TUDBtnType Button);
  void __fastcall OnCurveLineTypeChange(TObject *Sender);
  void __fastcall OnCloseClick(TObject *Sender);
  void __fastcall OnCloseForm(TObject *Sender, TCloseAction &Action);
  void __fastcall OnCloseQueryForm(TObject *Sender, bool &CanClose);
  void __fastcall OnCurveTypeClick(TObject *Sender);
  void __fastcall OnPSCodeChange(TObject *Sender);
  void __fastcall OnPSColorMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
  void __fastcall OnPSSizeChange(TObject *Sender);
  void __fastcall OnPSSizeClick(TObject *Sender, TUDBtnType Button);
  void __fastcall OnPSAngleChange(TObject *Sender);
  void __fastcall OnPSAngleClick(TObject *Sender, TUDBtnType Button);
  void __fastcall OnPSCodeClick(TObject *Sender, TUDBtnType Button);
  void __fastcall OnAreaFillColorMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);

private:	// User declarations
  GRA_page *page_;
  GRA_window *window_;
  GRA_cartesianCurve *curve_;
  GRA_setOfCharacteristics *curveChars_;
  bool setup_;

  void ReDraw();

public:		// User declarations
  __fastcall TCurvePopupForm( TComponent * );
  __fastcall ~TCurvePopupForm();

  void Disconnect();
  void Setup( GRA_page *, GRA_window *, GRA_cartesianCurve * );
};

extern PACKAGE TCurvePopupForm *CurvePopupForm;

#endif
