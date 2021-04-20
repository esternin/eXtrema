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
#ifndef SelectFontH
#define SelectFontH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>

class GRA_font;

class TSelectFontForm : public TForm
{
__published:	// IDE-managed Components
  TButton *OKButton;
  TPanel *Panel1;
  TComboBox *FontsComboBox;
  TRichEdit *FontsRichEdit;
  TLabel *FontSelectLabel;
  TLabel *FontSampleLabel;
  TButton *CancelButton;
  TLabel *HeightLabel;
  TEdit *HeightEdit;
  TLabel *AngleLabel;
  TEdit *AngleEdit;
  TPageControl *FontColorPageControl;
  TTabSheet *NamedColorTabSheet;
  TTabSheet *ColorMapTabSheet;
  TRadioGroup *NamedColorRadioGroup;
  TDrawGrid *FontColorDrawGrid;
  void __fastcall OKClick(TObject *Sender);
  void __fastcall FontsComboBoxClick(TObject *Sender);
  void __fastcall OnDrawCell(TObject *Sender, int ACol,
                             int ARow, TRect &Rect, TGridDrawState State);
  void __fastcall FontsGridClick(TObject *Sender);
  void __fastcall CancelClick(TObject *Sender);
  void __fastcall NamedColorRadioClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall HeightChange(TObject *Sender);
  void __fastcall AngleChange(TObject *Sender);

private:	// User declarations
  bool gridClick_, namedColorClick_, heightChanged_, angleChanged_;
  int colorCode_;

public:		// User declarations
  __fastcall TSelectFontForm(TComponent* Owner);
  __fastcall ~TSelectFontForm();

  void __fastcall SetFont( GRA_font const * );
  void __fastcall SetHeight( double );
  void __fastcall SetAngle( double );
  void __fastcall DisableAngle();

};

extern PACKAGE TSelectFontForm *SelectFontForm;

#endif
