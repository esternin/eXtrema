object SelectFontForm: TSelectFontForm
  Left = 145
  Top = 513
  Width = 255
  Height = 425
  BorderIcons = []
  Caption = 'Font selector'
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
  object OKButton: TButton
    Left = 44
    Top = 358
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 0
    OnClick = OKClick
  end
  object Panel1: TPanel
    Left = 2
    Top = 2
    Width = 242
    Height = 349
    TabOrder = 1
    object FontSelectLabel: TLabel
      Left = 10
      Top = 15
      Width = 50
      Height = 13
      Caption = 'Font name'
    end
    object FontSampleLabel: TLabel
      Left = 10
      Top = 60
      Width = 35
      Height = 13
      Caption = 'Sample'
    end
    object HeightLabel: TLabel
      Left = 5
      Top = 320
      Width = 39
      Height = 13
      Caption = '%Height'
    end
    object AngleLabel: TLabel
      Left = 130
      Top = 320
      Width = 27
      Height = 13
      Caption = 'Angle'
    end
    object FontsComboBox: TComboBox
      Left = 5
      Top = 30
      Width = 230
      Height = 21
      Hint = 'select a font name'
      ItemHeight = 13
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnChange = FontsComboBoxClick
    end
    object FontsRichEdit: TRichEdit
      Left = 5
      Top = 75
      Width = 230
      Height = 21
      Hint = 'font sample'
      Lines.Strings = (
        'FontsRichEdit')
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object HeightEdit: TEdit
      Left = 50
      Top = 315
      Width = 60
      Height = 21
      TabOrder = 2
      OnChange = HeightChange
    end
    object AngleEdit: TEdit
      Left = 163
      Top = 315
      Width = 60
      Height = 21
      TabOrder = 3
      OnChange = AngleChange
    end
    object FontColorPageControl: TPageControl
      Left = 5
      Top = 110
      Width = 230
      Height = 193
      ActivePage = NamedColorTabSheet
      TabOrder = 4
      object NamedColorTabSheet: TTabSheet
        Caption = 'Named colors'
        object NamedColorRadioGroup: TRadioGroup
          Left = 8
          Top = 15
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
            'White'
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
          TabOrder = 0
          OnClick = NamedColorRadioClick
        end
      end
      object ColorMapTabSheet: TTabSheet
        Caption = 'Color map colours'
        ImageIndex = 1
        object FontColorDrawGrid: TDrawGrid
          Left = 8
          Top = 15
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
          OnClick = FontsGridClick
          OnDrawCell = OnDrawCell
        end
      end
    end
  end
  object CancelButton: TButton
    Left = 125
    Top = 358
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 2
    OnClick = CancelClick
  end
end
