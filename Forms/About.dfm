object AboutBox: TAboutBox
  Left = 774
  Top = 588
  Width = 306
  Height = 279
  BorderIcons = []
  Caption = 'about Extrema'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 8
    Top = 13
    Width = 281
    Height = 188
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Color = cl3DLight
    TabOrder = 0
    object ProductLabel: TLabel
      Left = 103
      Top = 15
      Width = 75
      Height = 24
      Alignment = taCenter
      Caption = 'Extrema '
      Color = cl3DLight
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clMaroon
      Font.Height = -20
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsUnderline]
      ParentColor = False
      ParentFont = False
      IsControl = True
    end
    object VersionLabel: TLabel
      Left = 110
      Top = 48
      Width = 61
      Height = 13
      Alignment = taCenter
      Caption = 'version 4.4.5'
      IsControl = True
    end
    object CopyrightLabel: TLabel
      Left = 46
      Top = 65
      Width = 188
      Height = 13
      Caption = 'Copyright © 2010  by  Joseph L. Chuma'
      IsControl = True
    end
    object WebLabel: TLabel
      Left = 55
      Top = 170
      Width = 171
      Height = 13
      Cursor = crHandPoint
      Caption = 'http://exsitewebware.com/extrema/'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlue
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      OnClick = WebClick
    end
    object Label2: TLabel
      Left = 15
      Top = 105
      Width = 252
      Height = 13
      Caption = 'Extrema comes with ABSOLUTELY NO WARRANTY'
    end
    object Label3: TLabel
      Left = 40
      Top = 120
      Width = 201
      Height = 13
      Caption = 'This is free software, and you are welcome'
    end
    object Label4: TLabel
      Left = 46
      Top = 136
      Width = 190
      Height = 13
      Caption = 'to redistribute it under certain conditions.'
    end
  end
  object OKButton: TButton
    Left = 111
    Top = 212
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 1
  end
end
