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
#ifndef FitFormH
#define FitFormH
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <StdCtrls.hpp>

#include <string>

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>

class TFitCommandForm : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *OutputVariablesGroupBox;
  TPanel *UPDATEPanel;
  TRadioGroup *FitTypeRadioGroup;
  TCheckBox *CHISQCheckBox;
  TCheckBox *CLCheckBox;
  TCheckBox *CORRMATCheckBox;
  TCheckBox *COVMATCheckBox;
  TCheckBox *E1CheckBox;
  TCheckBox *E2CheckBox;
  TCheckBox *VARNAMESCheckBox;
  TCheckBox *FREECheckBox;
  TEdit *UPDATEEdit;
  TPanel *RESETPanel;
  TCheckBox *RESETCheckBox;
  TEdit *ITMAXEdit;
  TLabel *UPDATELabel;
  TLabel *ITMAXLabel;
  TEdit *TOLERANCEEdit;
  TLabel *TOLERANCELabel;
  TCheckBox *ZEROSCheckBox;
  TPanel *MainInputPanel;
  TEdit *DataVectorEdit;
  TLabel *DataVectorLabel;
  TEdit *ExpressionEdit;
  TLabel *ExpressionLabel;
  TEdit *CHISQEdit;
  TEdit *CLEdit;
  TEdit *FREEEdit;
  TStringGrid *ParameterStringGrid;
  TEdit *WeightVectorEdit;
  TLabel *WeightVectorLabel;
  TButton *FitButton;
  TButton *CloseButton;
  TButton *DefaultsButton;
  TButton *UPDATEButton;
  TButton *HelpButton;
  void __fastcall SetDefaults(TObject *Sender);
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall SetFitType(TObject *Sender);
  void __fastcall UPDATEButtonClick(TObject *Sender);
  void __fastcall DoTheFitClick(TObject *Sender);
  void __fastcall HelpClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);

private:	// User declarations
  ExString expression_;

public:		// User declarations
  __fastcall TFitCommandForm(TComponent* Owner);
  __fastcall ~TFitCommandForm();
};

extern PACKAGE TFitCommandForm *FitCommandForm;

#endif
