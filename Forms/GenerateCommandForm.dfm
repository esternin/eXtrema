object GenerateCmndForm: TGenerateCmndForm
  Left = 139
  Top = 539
  Width = 468
  Height = 239
  BorderIcons = []
  Caption = 'Generate a vector'
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
  object NameLabel: TLabel
    Left = 242
    Top = 10
    Width = 112
    Height = 13
    Caption = 'Generated vector name'
  end
  object Panel1: TPanel
    Left = 200
    Top = 30
    Width = 246
    Height = 126
    TabOrder = 0
    object MinLabel: TLabel
      Left = 150
      Top = 8
      Width = 70
      Height = 13
      Caption = 'Minimum value'
    end
    object IncLabel: TLabel
      Left = 150
      Top = 40
      Width = 47
      Height = 13
      Caption = 'Increment'
    end
    object MaxLabel: TLabel
      Left = 150
      Top = 68
      Width = 73
      Height = 13
      Caption = 'Maximum value'
    end
    object NumLabel: TLabel
      Left = 150
      Top = 100
      Width = 83
      Height = 13
      Caption = 'Number of values'
    end
    object MinEdit: TEdit
      Left = 5
      Top = 5
      Width = 140
      Height = 21
      Hint = 'enter the minimum value'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
    object IncEdit: TEdit
      Left = 5
      Top = 35
      Width = 140
      Height = 21
      Hint = 'enter the increment'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object MaxEdit: TEdit
      Left = 5
      Top = 65
      Width = 140
      Height = 21
      Hint = 'enter the maximum value'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
    end
    object NumEdit: TEdit
      Left = 5
      Top = 95
      Width = 140
      Height = 21
      Hint = 'enter the number of values to generate'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
    end
  end
  object NameEdit: TEdit
    Left = 97
    Top = 5
    Width = 140
    Height = 21
    Hint = 'enter the name of the vector to generate'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
  end
  object ChoicesRadioGroup: TRadioGroup
    Left = 5
    Top = 35
    Width = 185
    Height = 105
    Hint = 'choose how to generate the vector'
    Caption = 'Choices'
    ItemIndex = 1
    Items.Strings = (
      'random values'
      'minimum increment maximum'
      'minimum increment number'
      'minimum maximum number')
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnClick = ChoicesClick
  end
  object DefaultsButton: TButton
    Left = 108
    Top = 170
    Width = 75
    Height = 25
    Hint = 'clear all fields'
    Caption = 'Defaults'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = DefaultsClick
  end
  object CloseButton: TButton
    Left = 268
    Top = 170
    Width = 75
    Height = 25
    Hint = 'close this form'
    Caption = 'Close'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    OnClick = CloseClick
  end
  object ApplyButton: TButton
    Left = 188
    Top = 170
    Width = 75
    Height = 25
    Hint = 'create the vector'
    Caption = 'Apply'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
    OnClick = ApplyClick
  end
end
