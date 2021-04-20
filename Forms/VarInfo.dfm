object VarInfoForm: TVarInfoForm
  Left = 409
  Top = 308
  Width = 553
  Height = 329
  BorderIcons = [biMinimize, biMaximize]
  Caption = 'variable information'
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
  object VarInfoPanel: TPanel
    Left = 0
    Top = 0
    Width = 545
    Height = 257
    Align = alTop
    Caption = 'VarInfoPanel'
    TabOrder = 0
    object VarInfoMemo: TMemo
      Left = 1
      Top = 1
      Width = 543
      Height = 255
      Align = alClient
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Fixedsys'
      Font.Style = []
      ParentFont = False
      ScrollBars = ssBoth
      TabOrder = 0
    end
  end
  object VarInfoCloseBtn: TButton
    Left = 278
    Top = 264
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 1
    OnClick = CloseClick
  end
  object VarInfoRefreshBtn: TButton
    Left = 192
    Top = 264
    Width = 75
    Height = 25
    Caption = 'Refresh'
    TabOrder = 2
    OnClick = RefreshInfo
  end
end
