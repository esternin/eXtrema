object ProgressForm: TProgressForm
  Left = 745
  Top = 487
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = 'Loading...'
  ClientHeight = 21
  ClientWidth = 242
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object ProgressBar1: TProgressBar
    Left = 0
    Top = 0
    Width = 242
    Height = 21
    Align = alClient
    Min = 0
    Max = 100
    TabOrder = 0
  end
end
