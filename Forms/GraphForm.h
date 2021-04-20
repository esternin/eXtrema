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
#ifndef GraphFormH
#define GraphFormH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>

class TGraphCommandForm : public TForm
{
__published:	// IDE-managed Components
  TPageControl *PageControl1;
  TTabSheet *GraphTabSheet;
  TTabSheet *PlotSymbolTabSheet;
  TTabSheet *DataTabSheet;
  TTabSheet *XAxisTabSheet;
  TTabSheet *YAxisTabSheet;
  TLabel *DataDescriptionLabel;
  TLabel *DepVarLabel;
  TLabel *DepVarEBar1Label;
  TLabel *DepVarEBar2Label;
  TLabel *IndepVarLabel;
  TLabel *IndepVarEBar1Label;
  TLabel *IndepVarEBar2Label;
  TLabel *HistogramTypeLabel;
  TCheckBox *AxesOnlyCheckBox;
  TCheckBox *OverlayCheckBox;
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label3;
  TRadioGroup *AutoscaleRadioGroup;
  TLabel *PlotSymbolLabel;
  TComboBox *PlotSymbolComboBox;
  TLabel *PlotSymbolAngleLabel;
  TEdit *PlotSymbolAngleEdit;
  TLabel *PlotSymbolSizeLabel;
  TEdit *PlotSymbolSizeEdit;
  TCheckBox *PlotSymbolConnectCheckBox;
  TPanel *XPanel;
  TLabel *XMinLabel;
  TLabel *XVMinLabel;
  TLabel *XMaxLabel;
  TLabel *XVMaxLabel;
  TLabel *XLogBaseLabel;
  TEdit *XMinEdit;
  TEdit *XVMinEdit;
  TEdit *XMaxEdit;
  TEdit *XVMaxEdit;
  TEdit *XLogBaseEdit;
  TCheckBox *XLogStyleCheckBox;
  TPanel *LocPanel;
  TLabel *XLAxisLabel;
  TLabel *XUAxisLabel;
  TLabel *XAxisAngleLabel;
  TEdit *XLAxisEdit;
  TEdit *XUAxisEdit;
  TCheckBox *XAxisOnCheckBox;
  TEdit *XAxisAngleEdit;
  TButton *XAxisColorButton;
  TCheckBox *XForceCrossCheckBox;
  TPanel *TicPanel;
  TLabel *XLTicLenLabel;
  TLabel *XSTicLenLabel;
  TLabel *XTicAngleLabel;
  TEdit *XLTicLenEdit;
  TEdit *XSTicLenEdit;
  TEdit *XTicAngleEdit;
  TCheckBox *XTicsCheckBox;
  TCheckBox *XTicsBothSidesCheckBox;
  TPanel *NumberPanel;
  TLabel *XNDigLabel;
  TLabel *XNDecLabel;
  TLabel *XNumDistLabel;
  TLabel *XNumAxisAngleLabel;
  TCheckBox *XNumbersCheckBox;
  TEdit *XNDigEdit;
  TEdit *XNDecEdit;
  TEdit *XNumDistEdit;
  TEdit *XNumAxisAngleEdit;
  TButton *XNumericLabelsFontButton;
  TPanel *TextLabelPanel;
  TLabel *XTextLabel;
  TEdit *XTextLabelEdit;
  TButton *XTextLabelFontButton;
  TCheckBox *XTextLabelCheckBox;
  TPanel *Panel1;
  TLabel *YMinLabel;
  TLabel *YVMinLabel;
  TLabel *YMaxLabel;
  TLabel *YVMaxLabel;
  TLabel *YLogBaseLabel;
  TEdit *YMinEdit;
  TEdit *YVMinEdit;
  TEdit *YMaxEdit;
  TEdit *YVMaxEdit;
  TEdit *YLogBaseEdit;
  TCheckBox *YLogStyleCheckBox;
  TPanel *Panel2;
  TLabel *YLAxisLabel;
  TLabel *YUAxisLabel;
  TLabel *YAxisAngleLabel;
  TEdit *YLAxisEdit;
  TEdit *YUAxisEdit;
  TCheckBox *YAxisOnCheckBox;
  TEdit *YAxisAngleEdit;
  TButton *YAxisColorButton;
  TCheckBox *YForceCrossCheckBox;
  TPanel *Panel3;
  TLabel *YLTicLenLabel;
  TLabel *YSTicLenLabel;
  TLabel *YTicAngleLabel;
  TEdit *YLTicLenEdit;
  TEdit *YSTicLenEdit;
  TEdit *YTicAngleEdit;
  TCheckBox *YTicsCheckBox;
  TCheckBox *YTicsBothSidesCheckBox;
  TPanel *Panel4;
  TLabel *YNDigLabel;
  TLabel *YNDecLabel;
  TLabel *YNumDistLabel;
  TLabel *YNumAxisAngleLabel;
  TCheckBox *YNumbersCheckBox;
  TEdit *YNDigEdit;
  TEdit *YNDecEdit;
  TEdit *YNumDistEdit;
  TEdit *YNumAxisAngleEdit;
  TButton *YNumericLabelsFontButton;
  TPanel *Panel5;
  TLabel *YTextLabel;
  TEdit *YTextLabelEdit;
  TButton *YTextLabelFontButton;
  TCheckBox *YTextLabelCheckBox;
  TCheckBox *YOnRightCheckBox;
  TCheckBox *XOnTopCheckBox;
  TButton *ClearButton;
  TButton *ReplotButton;
  TButton *DrawGraphButton;
  TButton *DefaultsButton;
  TButton *CloseButton;
  TButton *PlotSymbolColorButton;
  TCheckBox *VirtualCheckBox;
  TCheckBox *XVirtualCheckBox;
  TCheckBox *YVirtualCheckBox;
  TLabel *NLYincLabel;
  TEdit *NLYincEdit;
  TLabel *NSYincLabel;
  TEdit *NSYincEdit;
  TLabel *NLXincLabel;
  TEdit *NLXincEdit;
  TLabel *NSXincLabel;
  TEdit *NSXincEdit;
  TComboBox *DepVarComboBox;
  TComboBox *DepVarErrorComboBox;
  TComboBox *DepVarError2ComboBox;
  TComboBox *IndepVarErrorComboBox;
  TComboBox *IndepVarError2ComboBox;
  TComboBox *IndepVarComboBox;
  TCheckBox *SmoothCheckBox;
  TImage *HT1Image;
  TImage *HT2Image;
  TImage *HT3Image;
  TImage *HT4Image;
  TImage *HT0Image;
  TImage *Image6;
  TImage *HT0bImage;
  TImage *HT1bImage;
  TImage *HT3bImage;
  TImage *HT4bImage;
  TLabel *Label4;
  TLabel *Label5;
  TLabel *Label6;
  TLabel *Label7;
  TLabel *Label8;
  TImage *HT2bImage;
  TImage *HT0aImage;
  TImage *HT1aImage;
  TImage *HT2aImage;
  TImage *HT3aImage;
  TImage *HT4aImage;
  void __fastcall PlotSymbolColorButtonClick(TObject *Sender);
  void __fastcall CloseButtonClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);
  void __fastcall ReplotButtonClick(TObject *Sender);
  void __fastcall ClearGraphicsButtonClick(TObject *Sender);
  void __fastcall AxesOnlyClick(TObject *Sender);
  void __fastcall OverlayCurveClick(TObject *Sender);
  void __fastcall DefaultsButtonClick(TObject *Sender);
  void __fastcall AutoscaleClick(TObject *Sender);
  void __fastcall DrawGraphButtonClick(TObject *Sender);
  void __fastcall XAxisColorButtonClick(TObject *Sender);
  void __fastcall YAxisColorButtonClick(TObject *Sender);
  void __fastcall XNumericFontButtonClick(TObject *Sender);
  void __fastcall YNumericFontButtonClick(TObject *Sender);
  void __fastcall YTextFontButtonClick(TObject *Sender);
  void __fastcall XTextFontButtonClick(TObject *Sender);
  void __fastcall H0Click(TObject *Sender);
  void __fastcall H1Click(TObject *Sender);
  void __fastcall H2Click(TObject *Sender);
  void __fastcall H3Click(TObject *Sender);
  void __fastcall H4Click(TObject *Sender);

private:	// User declarations
  bool wasCommensurate_;
  int histogramType_;
  //
  void __fastcall DrawGraph();
  void __fastcall FillOutForm();
  void __fastcall PutImage( TImage *, TImage * );

public:		// User declarations
  __fastcall TGraphCommandForm(TComponent* Owner);
  __fastcall ~TGraphCommandForm();
};

extern PACKAGE TGraphCommandForm *GraphCommandForm;

#endif
