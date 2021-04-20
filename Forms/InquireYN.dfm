object InquireYNForm: TInquireYNForm
  Left = 483
  Top = 539
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsSingle
  Caption = 'Inquire'
  ClientHeight = 85
  ClientWidth = 545
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object InquireLabel: TLabel
    Left = 5
    Top = 8
    Width = 6
    Height = 20
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clMaroon
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object EndButton: TButton
    Left = 310
    Top = 50
    Width = 85
    Height = 25
    Caption = 'Stop all scripts'
    TabOrder = 0
    OnClick = StopScriptsButtonClick
  end
  object YesButton: TButton
    Left = 150
    Top = 50
    Width = 75
    Height = 25
    Caption = 'Yes'
    TabOrder = 1
    OnClick = YesButtonClick
  end
  object NoButton: TButton
    Left = 230
    Top = 50
    Width = 75
    Height = 25
    Caption = 'No'
    TabOrder = 2
    OnClick = NoButtonClick
  end
end
