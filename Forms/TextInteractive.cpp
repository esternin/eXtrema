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

#include "TextInteractive.h"

#include "ExString.h"
#include "ExGlobals.h"
#include "GRA_window.h"
#include "MainGraphicsWindow.h"
#include "SelectFont.h"
#include "GRA_setOfCharacteristics.h"
#include "GRA_drawableText.h"
#include "EGraphicsError.h"
#include "GRA_borlandW32.h"
#include "GRA_font.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

TTextCommandForm *TextCommandForm;

int TTextCommandForm::gwToRadio_[] = {2,5,8,1,4,7,0,3,6};
int TTextCommandForm::radioToGw_[] = {7,4,1,8,5,2,9,6,3};

__fastcall TTextCommandForm::TTextCommandForm(TComponent* Owner)
  : TForm(Owner)
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  Top = ini->ReadInteger( "TextCommandForm", "Top", 213 );
  Left = ini->ReadInteger( "TextCommandForm", "Left", 510 );
  Width = ini->ReadInteger( "TextCommandForm", "Width", 490 );
  Height = ini->ReadInteger( "TextCommandForm", "Height", 330 );
  TextInteractiveEdit->Text = ini->ReadString( "TextCommandForm", "String", "" );
  TextInteractiveCheckBox->Checked = ini->ReadBool( "TextCommandForm", "Interactive", true );
  XLocEdit->Text = ini->ReadString( "TextCommandForm", "Xloc", "" );
  YLocEdit->Text = ini->ReadString( "TextCommandForm", "Yloc", "" );
  TextAlignRadioGroup->ItemIndex = gwToRadio_[ini->ReadInteger("TextCommandForm","Alignment",1)-1];
  delete ini;
  //
  fromSet_ = false;
  SetUp();
}

__fastcall TTextCommandForm::~TTextCommandForm()
{
  TIniFile *ini = new TIniFile( ChangeFileExt( Application->ExeName, ".INI" ) );
  ini->WriteInteger( "TextCommandForm", "Top", Top );
  ini->WriteInteger( "TextCommandForm", "Left", Left );
  ini->WriteInteger( "TextCommandForm", "Width", Width );
  ini->WriteInteger( "TextCommandForm", "Height", Height );
  ini->WriteString( "TextCommandForm", "String", TextInteractiveEdit->Text );
  ini->WriteBool( "TextCommandForm", "Interactive", TextInteractiveCheckBox->Checked );
  ini->WriteString( "TextCommandForm", "Xloc", XLocEdit->Text );
  ini->WriteString( "TextCommandForm", "Yloc", YLocEdit->Text );
  ini->WriteInteger( "TextCommandForm", "Alignment", radioToGw_[TextAlignRadioGroup->ItemIndex] );
  delete ini;
}

void __fastcall TTextCommandForm::DrawButtonClick( TObject *Sender )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *textC = gw->GetTextCharacteristics();
  //
  static_cast<GRA_intCharacteristic*>(textC->Get("ALIGNMENT"))->Set( radioToGw_[TextAlignRadioGroup->ItemIndex] );
  //
  bool interactive = TextInteractiveCheckBox->Checked;
  static_cast<GRA_boolCharacteristic*>(textC->Get("INTERACTIVE"))->Set( interactive );
  if( !interactive )
  {
    double xloc, yloc;
    try
    {
      xloc = XLocEdit->Text.ToDouble();
    }
    catch (EConvertError &e)
    {
      Application->MessageBox( "invalid value entered for the x location", "Error", MB_OK );
      return;
    }
    try
    {
      yloc = YLocEdit->Text.ToDouble();
    }
    catch (EConvertError &e)
    {
      Application->MessageBox( "invalid value entered for the y location", "Error", MB_OK );
      return;
    }
    static_cast<GRA_distanceCharacteristic*>(textC->Get("XLOCATION"))->SetAsPercent( xloc );
    static_cast<GRA_distanceCharacteristic*>(textC->Get("YLOCATION"))->SetAsPercent( yloc );
  }
  if( !fromSet_ || !interactive )
  {
    ExString text( TextInteractiveEdit->Text.c_str() );
    GRA_drawableText *dt = new GRA_drawableText( text, false );
    try
    {
      dt->Parse();
    }
    catch ( EGraphicsError &e )
    {
      delete dt;
      Application->MessageBox( e.what(), "Error", MB_OK );
      return;
    }
    if( interactive )MainGraphicsForm->SetTextPlacementMode( dt );
    else
    {
      ExGlobals::GetScreenOutput()->Draw( dt );
      gw->AddDrawableObject( dt );
    }
  }
  //
  if( ExGlobals::StackIsOn() )
  {
    ExString line( "FORM TEXT,'" );
    int psc = TextInteractiveCheckBox->Checked ? 1 : 0;
    line += ExString(TextInteractiveEdit->Text.c_str()) + "'," + ExString(psc) + "," +
            ExString(XLocEdit->Text.c_str()) + "," + ExString(YLocEdit->Text.c_str()) + "," +
            ExString( radioToGw_[TextAlignRadioGroup->ItemIndex] ) + ",DRAW";
    ExGlobals::WriteStack( line );
  }
}

void __fastcall TTextCommandForm::TICheckBoxClick( TObject *Sender )
{ SetUp(); }

void __fastcall TTextCommandForm::SetUp()
{
  if( TextInteractiveCheckBox->Checked )
  {
    XLocEdit->Enabled = false;
    XLocLabel->Enabled = false;
    YLocEdit->Enabled = false;
    YLocLabel->Enabled = false;
  }
  else
  {
    XLocEdit->Enabled = true;
    XLocLabel->Enabled = true;
    YLocEdit->Enabled = true;
    YLocLabel->Enabled = true;
  }
}

void __fastcall TTextCommandForm::FontButtonClick( TObject *Sender )
{
  GRA_window *gw = ExGlobals::GetGraphWindow();
  GRA_setOfCharacteristics *textChars = gw->GetTextCharacteristics();
  //
  // these flags need to be set false, to indicate if a name,
  // color, etc. have been selected in the font selector form
  //
  ExGlobals::SetWorkingFontFlags( false );
  //
  std::auto_ptr<TSelectFontForm> f( new TSelectFontForm(this) );
  f->SetFont( static_cast<GRA_fontCharacteristic*>(textChars->Get("FONT"))->Get() );
  f->Caption = AnsiString("Text Font");
  f->SetHeight( static_cast<GRA_sizeCharacteristic*>(textChars->Get("HEIGHT"))->GetAsPercent() );
  f->SetAngle( static_cast<GRA_angleCharacteristic*>(textChars->Get("ANGLE"))->Get() );
  f->ShowModal();
  //
  if( ExGlobals::GetWorkingFontFlag() )
    static_cast<GRA_fontCharacteristic*>(textChars->Get("FONT"))->Set( ExGlobals::GetWorkingFont() );
  if( ExGlobals::GetWorkingFontColorFlag() )
    static_cast<GRA_colorCharacteristic*>(textChars->Get("COLOR"))->Set( ExGlobals::GetWorkingFontColor() );
  if( ExGlobals::GetWorkingFontHeightFlag() )
    static_cast<GRA_sizeCharacteristic*>(textChars->Get("HEIGHT"))->SetAsPercent( ExGlobals::GetWorkingFontHeight() );
  if( ExGlobals::GetWorkingFontAngleFlag() )
    static_cast<GRA_angleCharacteristic*>(textChars->Get("ANGLE"))->Set( ExGlobals::GetWorkingFontAngle() );
}

void __fastcall TTextCommandForm::EraseButtonClick( TObject *Sender )
{
  ExGlobals::GetGraphWindow()->RemoveLastTextString();
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( "FORM TEXT,ERASE" );
}

void __fastcall TTextCommandForm::CloseButtonClick( TObject *Sender )
{
  Close();
  TextCommandForm = 0;
  if( ExGlobals::StackIsOn() )ExGlobals::WriteStack( "FORM TEXT,CLOSE" );
}

void __fastcall TTextCommandForm::FormClose( TObject *Sender, TCloseAction &Action )
{ Action = caFree; }

void __fastcall TTextCommandForm::FormClose2( TObject *Sender, bool &CanClose )
{ TextCommandForm = 0; }

void TTextCommandForm::Set( ExString const &text, bool psc,
                            double xloc, double yloc, int alignment,
                            ExString const &action )
{
  TextInteractiveEdit->Text = AnsiString( text.c_str() );
  TextInteractiveCheckBox->Checked = psc;
  XLocEdit->Text = AnsiString( xloc );
  YLocEdit->Text = AnsiString( yloc );
  TextAlignRadioGroup->ItemIndex = gwToRadio_[alignment-1];
  if( action == "CLOSE" )
  {
    CloseButtonClick( static_cast<TObject*>(0) );
  }
  else if( action == "ERASE" )
  {
    EraseButtonClick( static_cast<TObject*>(0) );
  }
  else if( action == "DRAW" )
  {
    fromSet_ = true;
    DrawButtonClick( static_cast<TObject*>(0) );
    fromSet_ = false;
  }
}
// end of file
