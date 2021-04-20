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
#ifndef FigureDrawH
#define FigureDrawH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Buttons.hpp>
#include <Dialogs.hpp>

class TFigureDrawForm : public TForm
{
__published:	// IDE-managed Components
  TPageControl *FigureDrawPageControl;
  TTabSheet *ArrowTabSheet;
  TLabel *ChooseArrowTypeLabel;
  TImage *Image11;
  TImage *Image21;
  TImage *Image31;
  TImage *Image12;
  TImage *Image1;
  TImage *Image3;
  TImage *Image2;
  TImage *Image13;
  TImage *Image22;
  TImage *Image23;
  TImage *Image32;
  TImage *Image33;
  TImage *Image1b;
  TImage *Image2b;
  TImage *Image3b;
  TCheckBox *HeadsCheckBox;
  TButton *CloseButton;
  TTabSheet *RectanglesTabSheet;
  TButton *ArrowDrawButton;
  TButton *PolygonDrawButton;
  TTabSheet *EllipseTabSheet;
  TButton *EllipseDrawButton;
  TPanel *Panel1;
  TSpeedButton *LineWidthSpeedButton;
  TSpeedButton *LineColorSpeedButton;
  TSpeedButton *FillColorSpeedButton;
  TCheckBox *CircleCheckBox;
  TImage *RectangleImage;
  TImage *SquareImage;
  TImage *Rectangle1bImage;
  TImage *Square1bImage;
  TImage *Rectangle1Image;
  TImage *Square1Image;
  TEdit *PolygonAngleEdit;
  TLabel *PolygonAngleLabel;
  TImage *Star1bImage;
  TImage *StarImage;
  TImage *Star1Image;
  void __fastcall ClickImage1(TObject *Sender);
  void __fastcall ClickImage2(TObject *Sender);
  void __fastcall ClickImage3(TObject *Sender);
  void __fastcall CloseClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);
  void __fastcall HeadBothEndsClick(TObject *Sender);
  void __fastcall DrawEllipseClick(TObject *Sender);
  void __fastcall DrawPolygonClick(TObject *Sender);
  void __fastcall DrawArrowClick(TObject *Sender);
  void __fastcall LineWidthClick(TObject *Sender);
  void __fastcall LineColorClick(TObject *Sender);
  void __fastcall FillColorClick(TObject *Sender);
  void __fastcall RectangleImageClick(TObject *Sender);
  void __fastcall SquareImageClick(TObject *Sender);
  void __fastcall StarImageClick(TObject *Sender);

private:	// User declarations
  int arrowType_;
  int polygonType_;
  double polygonAngle_;
  void __fastcall PutImage( TImage *, TImage * );

public:		// User declarations
  __fastcall TFigureDrawForm(TComponent* Owner);
  __fastcall ~TFigureDrawForm();
  void CloseForm();
};

extern PACKAGE TFigureDrawForm *FigureDrawForm;

#endif
