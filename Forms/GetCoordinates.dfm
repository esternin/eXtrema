object GetCoordinatesForm: TGetCoordinatesForm
  Left = 762
  Top = 438
  BorderIcons = []
  BorderStyle = bsSingle
  ClientHeight = 99
  ClientWidth = 155
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object XLabel: TLabel
    Left = 5
    Top = 5
    Width = 16
    Height = 16
    Caption = 'x :'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object YLabel: TLabel
    Left = 5
    Top = 35
    Width = 17
    Height = 16
    Caption = 'y :'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object XEdit: TEdit
    Left = 25
    Top = 3
    Width = 121
    Height = 21
    Hint = 'enter the abscissa here'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
  end
  object YEdit: TEdit
    Left = 25
    Top = 33
    Width = 121
    Height = 21
    Hint = 'enter the ordinate here'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
  end
  object OKButton: TButton
    Left = 45
    Top = 65
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 2
    OnClick = OKClick
  end
end
