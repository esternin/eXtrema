object HistogramTypeForm: THistogramTypeForm
  Left = 802
  Top = 105
  BorderStyle = bsSingle
  Caption = 'Histogram Type'
  ClientHeight = 161
  ClientWidth = 250
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object HistogramTypeRadioGroup: TRadioGroup
    Left = 0
    Top = 0
    Width = 250
    Height = 126
    Align = alTop
    Caption = 'Histogram Type'
    Color = cl3DLight
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    Items.Strings = (
      'non-histogram'
      'horizontal histogram without tails'
      'horizontal histogram with tails'
      'vertical histogram without tails'
      'vertical histogram with tails')
    ParentColor = False
    ParentFont = False
    TabOrder = 0
  end
  object CloseButton: TButton
    Left = 165
    Top = 136
    Width = 75
    Height = 25
    Hint = 'close this form (does not set histogram value)'
    Caption = 'Close'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    OnClick = CloseForm
  end
  object ApplyButton: TButton
    Left = 88
    Top = 136
    Width = 75
    Height = 25
    Hint = 'apply the histogram type setting'
    Caption = 'Apply'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnClick = ApplyClick
  end
  object DefaultsButton: TButton
    Left = 11
    Top = 136
    Width = 75
    Height = 25
    Hint = 'set default histogram value'
    Caption = 'Defaults'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = SetDefaultsClick
  end
end
