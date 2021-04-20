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
#include "ProjectHeader.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "MainGraphicsWindow.h"
#include "ArrowChooser.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TArrowChooserForm *ArrowChooserForm;

__fastcall TArrowChooserForm::TArrowChooserForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  this->Top = ini->ReadInteger( "ArrowChooserForm", "Top", 220 );
  this->Left = ini->ReadInteger( "ArrowChooserForm", "Left", 850 );
  arrowType_ = ini->ReadInteger( "ArrowChooserForm", "ArrowType", 1 );
  bool headsBothEnds = ini->ReadBool( "ArrowChooserForm", "HeadsBothEnds", false );
  delete ini;
  //
  switch( arrowType_ )
  {
    case 1:
      if( headsBothEnds )
      {
        PutImage( Image1b, Image13 );
        PutImage( Image2b, Image22 );
        PutImage( Image3b, Image32 );
      }
      else
      {
        PutImage( Image1b, Image11 );
        PutImage( Image2b, Image2  );
        PutImage( Image3b, Image3  );
      }
      break;
    case 2:
      if( headsBothEnds )
      {
        PutImage( Image1b, Image12 );
        PutImage( Image2b, Image23 );
        PutImage( Image3b, Image32 );
      }
      else
      {
        PutImage( Image1b, Image1  );
        PutImage( Image2b, Image21 );
        PutImage( Image3b, Image3  );
      }
      break;
    case 3:
      if( headsBothEnds )
      {
        PutImage( Image1b, Image12 );
        PutImage( Image2b, Image22 );
        PutImage( Image3b, Image33 );
      }
      else
      {
        PutImage( Image1b, Image1  );
        PutImage( Image2b, Image2  );
        PutImage( Image3b, Image31 );
      }
      break;
  }
  HeadsCheckBox->Checked = headsBothEnds;
  MainGraphicsForm->SetArrowType( arrowType_ );
}

__fastcall TArrowChooserForm::~TArrowChooserForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "ArrowChooserForm", "Top", this->Top );
  ini->WriteInteger( "ArrowChooserForm", "Left", this->Left );
  ini->WriteInteger( "ArrowChooserForm", "ArrowType", arrowType_ );
  ini->WriteBool( "ArrowChooserForm", "HeadsBothEnds", HeadsCheckBox->Checked );
  delete ini;
}

void __fastcall TArrowChooserForm::CloseClick(TObject *Sender)
{ CloseForm(); }

void __fastcall TArrowChooserForm::FormClose(TObject *Sender, TCloseAction &Action)
{ Action = caFree; }

void __fastcall TArrowChooserForm::FormClose2(TObject *Sender, bool &CanClose)
{ ArrowChooserForm = 0; }

void __fastcall TArrowChooserForm::PutImage( TImage *i1, TImage *i2 )
{ i1->Picture->Assign(i2->Picture); }

void TArrowChooserForm::CloseForm()
{
  Close();
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TArrowChooserForm::ClickImage1(TObject *Sender)
{
  switch( arrowType_ )
  {
    case 1:
      break;
    case 2:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image13 );
        PutImage( Image2b, Image22 );
      }
      else
      {
        PutImage( Image1b, Image11 );
        PutImage( Image2b, Image2  );
      }
      break;
    case 3:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image13 );
        PutImage( Image3b, Image32 );
      }
      else
      {
        PutImage( Image1b, Image11 );
        PutImage( Image3b, Image3  );
      }
      break;
  }
  arrowType_ = 1;
  MainGraphicsForm->SetArrowType( arrowType_ );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TArrowChooserForm::ClickImage2(TObject *Sender)
{
  switch( arrowType_ )
  {
    case 1:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image12 );
        PutImage( Image2b, Image23 );
      }
      else
      {
        PutImage( Image1b, Image1  );
        PutImage( Image2b, Image21 );
      }
      break;
    case 2:
      break;
    case 3:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image3b, Image32 );
        PutImage( Image2b, Image23 );
      }
      else
      {
        PutImage( Image3b, Image3  );
        PutImage( Image2b, Image21 );
      }
      break;
  }
  arrowType_ = 2;
  MainGraphicsForm->SetArrowType( arrowType_ );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TArrowChooserForm::ClickImage3(TObject *Sender)
{
  switch( arrowType_ )
  {
    case 1:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image12 );
        PutImage( Image3b, Image33 );
      }
      else
      {
        PutImage( Image1b, Image1  );
        PutImage( Image3b, Image31 );
      }
      break;
    case 2:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image2b, Image22 );
        PutImage( Image3b, Image33 );
      }
      else
      {
        PutImage( Image2b, Image2  );
        PutImage( Image3b, Image31 );
      }
      break;
    case 3:
      break;
  }
  arrowType_ = 3;
  MainGraphicsForm->SetArrowType( arrowType_ );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

void __fastcall TArrowChooserForm::SwitchImages( TImage *i1, TImage *i2 )
{
  TPicture *p = new TPicture();
  p->Assign(i1->Picture);
  i1->Picture->Assign(i2->Picture);
  i2->Picture->Assign(p);
  delete p;
}

void __fastcall TArrowChooserForm::HeadsBothEndsClick(TObject *Sender)
{
  switch( arrowType_ )
  {
    case 1:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image13 );
        PutImage( Image2b, Image22 );
        PutImage( Image3b, Image32 );
      }
      else
      {
        PutImage( Image1b, Image11 );
        PutImage( Image2b, Image2  );
        PutImage( Image3b, Image3  );
      }
      break;
    case 2:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image12 );
        PutImage( Image2b, Image23 );
        PutImage( Image3b, Image32 );
      }
      else
      {
        PutImage( Image1b, Image1  );
        PutImage( Image2b, Image21 );
        PutImage( Image3b, Image3  );
      }
      break;
    case 3:
      if( HeadsCheckBox->Checked )
      {
        PutImage( Image1b, Image12 );
        PutImage( Image2b, Image22 );
        PutImage( Image3b, Image33 );
      }
      else
      {
        PutImage( Image1b, Image1  );
        PutImage( Image2b, Image2  );
        PutImage( Image3b, Image31 );
      }
      break;
  }
  MainGraphicsForm->SetHeadsBothEnds( HeadsCheckBox->Checked );
  MainGraphicsForm->SetFocus();
  MainGraphicsForm->Show();
  MainGraphicsForm->Update();
}

//---------------------------------------------------------------------------

