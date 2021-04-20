object SelectColorForm: TSelectColorForm
  Left = 722
  Top = 392
  Width = 455
  Height = 229
  BorderIcons = []
  Caption = 'Color selector'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object ColorMapLabel: TLabel
    Left = 10
    Top = 65
    Width = 50
    Height = 13
    Caption = 'Color map:'
  end
  object ColorDrawGrid: TDrawGrid
    Left = 5
    Top = 11
    Width = 205
    Height = 130
    Hint = 'Click on a color from the current color map'
    Color = clWhite
    ColCount = 8
    DefaultColWidth = 20
    DefaultRowHeight = 20
    FixedCols = 0
    RowCount = 6
    FixedRows = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goThumbTracking]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnClick = GridClick
    OnDrawCell = OnDrawCell
  end
  object NamedColorRadioGroup: TRadioGroup
    Left = 230
    Top = 2
    Width = 205
    Height = 140
    Hint = 'choose one of the fixed named colors'
    Caption = 'Named colors'
    Columns = 3
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Arial'
    Font.Style = []
    Items.Strings = (
      'Black'
      'Red'
      'Green'
      'Blue'
      'Purple'
      'Yellow'
      'Cyan'
      'Brown'
      'Coral'
      'Salmon'
      'Sienna'
      'Tan'
      'Fuchsia'
      'Lime'
      'Maroon'
      'Navy'
      'Olive'
      'Silver'
      'Teal')
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    OnClick = NamedColorRadioClick
  end
  object OKButton: TButton
    Left = 144
    Top = 158
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 2
    OnClick = OKButtonClick
  end
  object CancelButton: TButton
    Left = 225
    Top = 158
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 3
    OnClick = CancelButtonClick
  end
end
