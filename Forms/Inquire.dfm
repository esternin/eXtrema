object InquireForm: TInquireForm
  Left = 449
  Top = 359
  Width = 528
  Height = 146
  BorderIcons = [biMinimize, biMaximize]
  Caption = 'Inquire'
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
    Left = 8
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
  object InquireEdit: TEdit
    Left = 5
    Top = 35
    Width = 508
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
    OnKeyPress = ReturnKeyPress
  end
  object OKButton: TButton
    Left = 185
    Top = 72
    Width = 85
    Height = 25
    Caption = 'OK'
    TabOrder = 1
    OnClick = OKButtonClick
  end
  object EndButton: TButton
    Left = 275
    Top = 72
    Width = 85
    Height = 25
    Caption = 'Stop all scripts'
    TabOrder = 2
    OnClick = StopScriptsButtonClick
  end
end
