object SelectLineWidthForm: TSelectLineWidthForm
  Left = 661
  Top = 468
  Width = 126
  Height = 155
  BorderIcons = []
  Caption = 'Set line width'
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
  object LineWidthLabel: TLabel
    Left = 35
    Top = 20
    Width = 48
    Height = 13
    Caption = 'Line width'
  end
  object LineWidthComboBox: TComboBox
    Left = 19
    Top = 40
    Width = 80
    Height = 21
    Hint = 'select the line width -- units are pts (72 pt = 1 inch)'
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnChange = LineWidthChange
    Items.Strings = (
      '1'
      '2'
      '3'
      '4'
      '5'
      '6'
      '7'
      '8'
      '9'
      '10'
      '11'
      '12'
      '13'
      '14'
      '15'
      '16'
      '17'
      '18'
      '19'
      '20')
  end
  object CloseButton: TButton
    Left = 21
    Top = 80
    Width = 75
    Height = 25
    Hint = 'close this form'
    Caption = 'Close'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    OnClick = CloseClick
  end
end
