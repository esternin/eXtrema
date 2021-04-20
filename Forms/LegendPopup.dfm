object LegendPopupForm: TLegendPopupForm
  Left = 621
  Top = 266
  Width = 270
  Height = 496
  Caption = 'Legend Popup'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = OnCloseForm
  OnCloseQuery = OnCloseQuery
  PixelsPerInch = 96
  TextHeight = 13
  object DrawLegendCheckBox: TCheckBox
    Left = 88
    Top = 8
    Width = 85
    Height = 17
    Hint = 'toggle the graph legend on/off'
    Caption = 'Draw legend'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnClick = OnDrawLegendClick
  end
  object TitleGroupBox: TGroupBox
    Left = 5
    Top = 240
    Width = 250
    Height = 185
    Caption = 'Legend Title'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlue
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object TitleLabel: TLabel
      Left = 16
      Top = 40
      Width = 20
      Height = 13
      Caption = 'Title'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object TitleColorLabel: TLabel
      Left = 45
      Top = 83
      Width = 24
      Height = 13
      Caption = 'Color'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object TitleColorShape: TShape
      Left = 46
      Top = 96
      Width = 25
      Height = 33
      Hint = 'click to set the legend title color'
      ParentShowHint = False
      Shape = stRoundSquare
      ShowHint = True
      OnMouseDown = OnTitleColorMouseDown
    end
    object TitleHeightLabel: TLabel
      Left = 120
      Top = 84
      Width = 48
      Height = 13
      Caption = 'Height (%)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object TitleFontLabel: TLabel
      Left = 16
      Top = 132
      Width = 21
      Height = 13
      Caption = 'Font'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object TitleEdit: TEdit
      Left = 15
      Top = 54
      Width = 220
      Height = 21
      Hint = 'enter the new legend title (hit Enter key to accept it)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnKeyPress = OnTitleKeyPress
    end
    object TitleHeightEdit: TEdit
      Left = 113
      Top = 100
      Width = 57
      Height = 21
      Hint = 'set the title height (as a percentage of the window)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      Text = '0'
      OnChange = OnTitleHeightChange
    end
    object TitleHeightUpDown: TUpDown
      Left = 170
      Top = 100
      Width = 17
      Height = 21
      Min = 0
      Max = 1000
      Position = 0
      TabOrder = 2
      Wrap = False
      OnClick = OnTitleHeightUpDownClick
    end
    object TitleFontComboBox: TComboBox
      Left = 15
      Top = 146
      Width = 220
      Height = 21
      Hint = 'choose the title font'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ItemHeight = 13
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
      OnChange = OnTitleFontChange
    end
    object DrawTitleCheckBox: TCheckBox
      Left = 90
      Top = 20
      Width = 70
      Height = 17
      Caption = 'Draw title'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 4
      OnClick = OnDrawTitleClick
    end
  end
  object FrameGroupBox: TGroupBox
    Left = 5
    Top = 32
    Width = 250
    Height = 201
    Caption = 'Legend Frame'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlue
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    object FrameColorLabel: TLabel
      Left = 39
      Top = 20
      Width = 24
      Height = 13
      Caption = 'Color'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object FrameColorShape: TShape
      Left = 40
      Top = 32
      Width = 25
      Height = 33
      Hint = 'click to set the frame color'
      ParentShowHint = False
      Shape = stRoundSquare
      ShowHint = True
      OnMouseDown = OnFrameColorMouseDown
    end
    object MoveLabel: TLabel
      Left = 16
      Top = 80
      Width = 27
      Height = 13
      Caption = 'Move'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object MoveHorizontalLabel: TLabel
      Left = 77
      Top = 62
      Width = 47
      Height = 13
      Caption = 'Horizontal'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object MoveVerticalLabel: TLabel
      Left = 176
      Top = 62
      Width = 35
      Height = 13
      Caption = 'Vertical'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object ExpandHorizontalLabel: TLabel
      Left = 77
      Top = 126
      Width = 47
      Height = 13
      Caption = 'Horizontal'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object ExpandVerticalLabel: TLabel
      Left = 176
      Top = 126
      Width = 35
      Height = 13
      Caption = 'Vertical'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object Label1: TLabel
      Left = 16
      Top = 144
      Width = 36
      Height = 13
      Caption = 'Expand'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object MoveHLabel: TLabel
      Left = 95
      Top = 104
      Width = 6
      Height = 13
      Caption = '()'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object MoveVLabel: TLabel
      Left = 190
      Top = 104
      Width = 6
      Height = 13
      Caption = '()'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object ExpandHLabel: TLabel
      Left = 95
      Top = 170
      Width = 6
      Height = 13
      Caption = '()'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object ExpandVLabel: TLabel
      Left = 190
      Top = 170
      Width = 6
      Height = 13
      Caption = '()'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object DrawFrameCheckBox: TCheckBox
      Left = 100
      Top = 35
      Width = 112
      Height = 17
      Hint = 'toggle drawing the legend frame'
      Caption = 'Draw legend frame'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnClick = OnDrawFrameClick
    end
    object MoveHorizontalTrackBar: TTrackBar
      Left = 56
      Top = 74
      Width = 90
      Height = 30
      Hint = 'shift the legend left or right'
      Max = 100
      Orientation = trHorizontal
      ParentShowHint = False
      Frequency = 10
      Position = 0
      SelEnd = 0
      SelStart = 0
      ShowHint = True
      TabOrder = 1
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = OnMoveHorizontally
    end
    object MoveVerticalTrackBar: TTrackBar
      Left = 152
      Top = 74
      Width = 90
      Height = 30
      Hint = 'shift the legend up or down'
      Max = 100
      Orientation = trHorizontal
      ParentShowHint = False
      Frequency = 10
      Position = 0
      SelEnd = 0
      SelStart = 0
      ShowHint = True
      TabOrder = 2
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = OnMoveVertically
    end
    object ExpandHorizontalTrackBar: TTrackBar
      Left = 56
      Top = 138
      Width = 90
      Height = 30
      Hint = 'expand or shrink the legend horizontally'
      Max = 100
      Orientation = trHorizontal
      ParentShowHint = False
      Frequency = 10
      Position = 0
      SelEnd = 0
      SelStart = 0
      ShowHint = True
      TabOrder = 3
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = OnExpandHorizontally
    end
    object ExpandVerticalTrackBar: TTrackBar
      Left = 152
      Top = 138
      Width = 90
      Height = 30
      Hint = 'expand or shrink the legend vertically'
      Max = 100
      Orientation = trHorizontal
      ParentShowHint = False
      Frequency = 10
      Position = 0
      SelEnd = 0
      SelStart = 0
      ShowHint = True
      TabOrder = 4
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = OnExpandVertically
    end
  end
  object CloseButton: TButton
    Left = 93
    Top = 432
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 3
    OnClick = OnCloseClick
  end
end
