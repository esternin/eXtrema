object SetAspectRatioForm: TSetAspectRatioForm
  Left = 667
  Top = 404
  Width = 240
  Height = 190
  Caption = 'Set Aspect Ratio'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object TopPanel: TPanel
    Left = 0
    Top = 0
    Width = 232
    Height = 156
    Align = alClient
    TabOrder = 0
    object HeightLabel: TLabel
      Left = 31
      Top = 28
      Width = 46
      Height = 16
      Caption = 'Height'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object WidthLabel: TLabel
      Left = 37
      Top = 68
      Width = 40
      Height = 16
      Caption = 'Width'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Shape1: TShape
      Left = 30
      Top = 53
      Width = 180
      Height = 2
    end
    object HeightEdit: TEdit
      Left = 81
      Top = 24
      Width = 121
      Height = 21
      TabOrder = 0
    end
    object WidthEdit: TEdit
      Left = 81
      Top = 64
      Width = 121
      Height = 21
      TabOrder = 1
    end
    object OKButton: TButton
      Left = 39
      Top = 112
      Width = 75
      Height = 25
      Caption = 'OK'
      TabOrder = 2
      OnClick = OKClick
    end
    object CancelButton: TButton
      Left = 119
      Top = 112
      Width = 75
      Height = 25
      Caption = 'Cancel'
      TabOrder = 3
      OnClick = CancelClick
    end
  end
end
