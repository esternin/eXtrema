object VarShowForm: TVarShowForm
  Left = 437
  Top = 274
  Width = 768
  Height = 277
  Caption = 'Known variables'
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
  object VarShowPanel: TPanel
    Left = 0
    Top = 0
    Width = 760
    Height = 200
    Caption = 'VarShowPanel'
    TabOrder = 0
    object VarShowStringGrid: TStringGrid
      Left = 1
      Top = 1
      Width = 758
      Height = 198
      Hint = 'click on a variable to show its details'
      Align = alClient
      DefaultColWidth = 150
      FixedCols = 0
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnClick = DisplayInfo
    end
  end
  object VarShowRefreshBtn: TButton
    Left = 299
    Top = 208
    Width = 75
    Height = 25
    Caption = 'Refresh'
    TabOrder = 1
    OnClick = RefreshStringGrid
  end
  object VarShowCloseBtn: TButton
    Left = 386
    Top = 208
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 2
    OnClick = CloseClick
  end
end
