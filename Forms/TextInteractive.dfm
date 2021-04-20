object TextCommandForm: TTextCommandForm
  Left = 149
  Top = 628
  Width = 490
  Height = 330
  BorderIcons = []
  Caption = 'Draw Character String'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCloseQuery = FormClose2
  PixelsPerInch = 96
  TextHeight = 13
  object CloseButton: TButton
    Left = 285
    Top = 260
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 0
    OnClick = CloseButtonClick
  end
  object ApplyButton: TButton
    Left = 203
    Top = 260
    Width = 75
    Height = 25
    Caption = 'Draw'
    TabOrder = 1
    OnClick = DrawButtonClick
  end
  object TextPanel1: TPanel
    Left = 1
    Top = 5
    Width = 481
    Height = 245
    TabOrder = 2
    object TextInteractiveLabel: TLabel
      Left = 5
      Top = 20
      Width = 74
      Height = 13
      Caption = 'Character string'
    end
    object XLocLabel: TLabel
      Left = 195
      Top = 45
      Width = 55
      Height = 13
      Caption = '%X location'
    end
    object YLocLabel: TLabel
      Left = 335
      Top = 45
      Width = 55
      Height = 13
      Caption = '%Y location'
    end
    object TextInteractiveEdit: TEdit
      Left = 90
      Top = 15
      Width = 375
      Height = 21
      Hint = 'enter the character string to draw'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
    object TextInteractiveCheckBox: TCheckBox
      Left = 20
      Top = 63
      Width = 160
      Height = 17
      Caption = 'Position string with the cursor'
      TabOrder = 1
      OnClick = TICheckBoxClick
    end
    object TextAlignRadioGroup: TRadioGroup
      Left = 50
      Top = 105
      Width = 380
      Height = 86
      BiDiMode = bdLeftToRight
      Caption = 'String alignment'
      Columns = 3
      ItemIndex = 0
      Items.Strings = (
        'upper left'
        'center left'
        'lower left'
        'upper center'
        'center center'
        'lower center'
        'upper right'
        'center right'
        'lower right')
      ParentBiDiMode = False
      TabOrder = 2
    end
    object XLocEdit: TEdit
      Left = 190
      Top = 60
      Width = 121
      Height = 21
      Hint = 'enter the horizontal location as a % of the width of the window'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
    end
    object YLocEdit: TEdit
      Left = 330
      Top = 60
      Width = 121
      Height = 21
      Hint = 'enter the vertical location as a % of the height of the window'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 4
    end
    object FontButton: TButton
      Left = 203
      Top = 205
      Width = 75
      Height = 25
      Caption = 'Font'
      TabOrder = 5
      OnClick = FontButtonClick
    end
  end
  object EraseButton: TButton
    Left = 122
    Top = 260
    Width = 75
    Height = 25
    Caption = 'Erase'
    TabOrder = 3
    OnClick = EraseButtonClick
  end
end
