object SplashScreenForm: TSplashScreenForm
  Left = 562
  Top = 434
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'Welcome to Extrema '
  ClientHeight = 276
  ClientWidth = 402
  Color = cl3DLight
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 1
    Top = 8
    Width = 400
    Height = 268
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Color = cl3DLight
    TabOrder = 0
    object VersionLabel: TLabel
      Left = 148
      Top = 88
      Width = 103
      Height = 20
      Caption = 'version 4.4.5'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
      IsControl = True
    end
    object CopyrightLabel: TLabel
      Left = 107
      Top = 120
      Width = 185
      Height = 13
      Caption = 'Copyright © 2010  by Joseph L. Chuma'
      IsControl = True
    end
    object WelcomeLabel: TLabel
      Left = 30
      Top = 40
      Width = 341
      Height = 42
      Caption = 'Welcome to Extrema'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clMaroon
      Font.Height = -30
      Font.Name = 'Arial Black'
      Font.Style = []
      ParentFont = False
    end
    object Label1: TLabel
      Left = 74
      Top = 168
      Width = 252
      Height = 13
      Caption = 'Extrema comes with ABSOLUTELY NO WARRANTY'
    end
    object Label2: TLabel
      Left = 100
      Top = 184
      Width = 201
      Height = 13
      Caption = 'This is free software, and you are welcome'
    end
    object Label3: TLabel
      Left = 107
      Top = 200
      Width = 187
      Height = 13
      Caption = 'to redistribute it under certain conditions'
    end
  end
  object Timer1: TTimer
    Interval = 2000
    OnTimer = Timer1Timer
    Left = 156
    Top = 13
  end
end
