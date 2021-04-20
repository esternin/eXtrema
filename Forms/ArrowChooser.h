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
#ifndef ArrowChooserH
#define ArrowChooserH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Buttons.hpp>

class TArrowChooserForm : public TForm
{
__published:	// IDE-managed Components
  TImage *Image1b;
  TImage *Image1;
  TImage *Image11;
  TImage *Image12;
  TImage *Image13;
  TImage *Image2b;
  TImage *Image2;
  TImage *Image21;
  TImage *Image22;
  TImage *Image23;
  TImage *Image3b;
  TImage *Image3;
  TImage *Image31;
  TImage *Image32;
  TImage *Image33;
  TButton *CloseButton;
  TLabel *ChooseArrowTypeLabel;
  TCheckBox *HeadsCheckBox;

  void __fastcall CloseClick(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall FormClose2(TObject *Sender, bool &CanClose);
  void __fastcall ClickImage1(TObject *Sender);
  void __fastcall ClickImage2(TObject *Sender);
  void __fastcall ClickImage3(TObject *Sender);
  void __fastcall HeadsBothEndsClick(TObject *Sender);

private:	// User declarations
  int arrowType_;
  void __fastcall SwitchImages( TImage *, TImage * );
  void __fastcall PutImage( TImage *, TImage * );

public:		// User declarations
  __fastcall TArrowChooserForm(TComponent* Owner);
  __fastcall ~TArrowChooserForm();
  void CloseForm();
};

extern PACKAGE TArrowChooserForm *ArrowChooserForm;

#endif
