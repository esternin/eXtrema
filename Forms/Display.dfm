object DisplayForm: TDisplayForm
  Left = 489
  Top = 275
  Width = 500
  Height = 237
  BorderIcons = []
  Caption = 'Display'
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
  object CloseButton: TButton
    Left = 159
    Top = 175
    Width = 80
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Close'
    TabOrder = 0
    OnClick = CloseButtonClick
  end
  object DisplayRichEdit: TRichEdit
    Left = 0
    Top = 0
    Width = 491
    Height = 169
    Anchors = [akLeft, akTop, akRight]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 1
  end
  object EndButton: TButton
    Left = 248
    Top = 175
    Width = 85
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Stop all scripts'
    TabOrder = 2
    OnClick = StopScriptsButtonClick
  end
end
