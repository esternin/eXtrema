object PeakFindingForm: TPeakFindingForm
  Left = 790
  Top = 212
  BorderIcons = [biMinimize, biMaximize]
  BorderStyle = bsSingle
  Caption = 'Peak Finding'
  ClientHeight = 186
  ClientWidth = 333
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
  object PeakFindPanel: TPanel
    Left = 1
    Top = 1
    Width = 330
    Height = 141
    TabOrder = 0
    object ScanLeftSpeedButton: TSpeedButton
      Left = 98
      Top = 35
      Width = 23
      Height = 22
      Hint = 'scan to the left'
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000120B0000120B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        3333333333333333333333333333333333333333333333333333333333333333
        3333333333333FF3333333333333003333333333333F77F33333333333009033
        333333333F7737F333333333009990333333333F773337FFFFFF330099999000
        00003F773333377777770099999999999990773FF33333FFFFF7330099999000
        000033773FF33777777733330099903333333333773FF7F33333333333009033
        33333333337737F3333333333333003333333333333377333333333333333333
        3333333333333333333333333333333333333333333333333333333333333333
        3333333333333333333333333333333333333333333333333333}
      NumGlyphs = 2
      ParentShowHint = False
      ShowHint = True
      OnClick = ScanLeftClick
    end
    object ScanRightSpeedButton: TSpeedButton
      Left = 125
      Top = 35
      Width = 23
      Height = 22
      Hint = 'scan to the right'
      Glyph.Data = {
        76010000424D7601000000000000760000002800000020000000100000000100
        04000000000000010000120B0000120B00001000000000000000000000000000
        800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        3333333333333333333333333333333333333333333333333333333333333333
        3333333333333333333333333333333333333333333FF3333333333333003333
        3333333333773FF3333333333309003333333333337F773FF333333333099900
        33333FFFFF7F33773FF30000000999990033777777733333773F099999999999
        99007FFFFFFF33333F7700000009999900337777777F333F7733333333099900
        33333333337F3F77333333333309003333333333337F77333333333333003333
        3333333333773333333333333333333333333333333333333333333333333333
        3333333333333333333333333333333333333333333333333333}
      NumGlyphs = 2
      ParentShowHint = False
      ShowHint = True
      OnClick = ScanRightClick
    end
    object XPeakValLabel: TLabel
      Left = 15
      Top = 90
      Width = 86
      Height = 13
      Caption = 'Peak x-coordinate'
    end
    object YPeakValLabel: TLabel
      Left = 15
      Top = 115
      Width = 86
      Height = 13
      Caption = 'Peak y-coordinate'
    end
    object MaxMinRadioGroup: TRadioGroup
      Left = 163
      Top = 5
      Width = 70
      Height = 66
      Caption = 'Search for'
      ItemIndex = 0
      Items.Strings = (
        'Maxima'
        'Minima')
      TabOrder = 0
    end
    object XPeakValEdit: TEdit
      Left = 110
      Top = 85
      Width = 121
      Height = 21
      TabOrder = 1
    end
    object YPeakValEdit: TEdit
      Left = 110
      Top = 110
      Width = 121
      Height = 21
      TabOrder = 2
    end
    object SaveButton: TButton
      Left = 245
      Top = 85
      Width = 75
      Height = 46
      Hint = 
        'save the currently chosen peak'#39's coordinates to vectors PEAK$X a' +
        'nd PEAK$Y'
      Caption = 'Save'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
      OnClick = RecordCoordinates
    end
  end
  object CloseButton: TButton
    Left = 173
    Top = 150
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 1
    OnClick = CloseClick
  end
  object InitializeButton: TButton
    Left = 86
    Top = 150
    Width = 75
    Height = 25
    Hint = 'start again with the latest curve drawn'
    Caption = 'Initialize'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnClick = NewCurve
  end
end
