object CurvePopupForm: TCurvePopupForm
  Left = 692
  Top = 300
  Width = 245
  Height = 390
  Caption = 'Curve Popup Editor'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = OnCloseForm
  OnCloseQuery = OnCloseQueryForm
  PixelsPerInch = 96
  TextHeight = 13
  object CurveColorLabel: TLabel
    Left = 52
    Top = 88
    Width = 54
    Height = 13
    Caption = 'Curve color'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object CurveColorShape: TShape
    Left = 66
    Top = 100
    Width = 25
    Height = 33
    Hint = 'click to set the curve color'
    ParentShowHint = False
    Shape = stRoundSquare
    ShowHint = True
    OnMouseDown = OnCurveColorMouseDown
  end
  object CurveLinewidthLabel: TLabel
    Left = 55
    Top = 136
    Width = 48
    Height = 13
    Caption = 'Line width'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object CurveLineTypeLabel: TLabel
    Left = 122
    Top = 136
    Width = 43
    Height = 13
    Caption = 'Line type'
  end
  object AreaFillColorLabel: TLabel
    Left = 124
    Top = 88
    Width = 60
    Height = 13
    Caption = 'Area fill color'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object AreaFillColorShape: TShape
    Left = 138
    Top = 100
    Width = 25
    Height = 33
    Hint = 'click to set the curve color'
    ParentShowHint = False
    Shape = stRoundSquare
    ShowHint = True
    OnMouseDown = OnAreaFillColorMouseDown
  end
  object HistogramRG: TRadioGroup
    Left = 50
    Top = 5
    Width = 137
    Height = 73
    Caption = 'Curve type'
    Items.Strings = (
      'non-histogram'
      'histogram without tails'
      'histogram with tails')
    TabOrder = 0
    OnClick = OnCurveTypeClick
  end
  object CurveLinewidthEdit: TEdit
    Left = 55
    Top = 152
    Width = 35
    Height = 21
    Hint = 'set the line width of the curve'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    Text = '1'
    OnChange = OnCurveLinewidthChange
  end
  object CurveLinewidthUpDown: TUpDown
    Left = 90
    Top = 152
    Width = 16
    Height = 21
    Associate = CurveLinewidthEdit
    Min = 1
    Max = 10
    Position = 1
    TabOrder = 2
    Wrap = False
    OnClick = OnCurveLinewidthClick
  end
  object CurveLineTypeEdit: TEdit
    Left = 122
    Top = 152
    Width = 42
    Height = 21
    Hint = 'set the line type for the curve '
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    Text = '1'
    OnChange = OnCurveLineTypeChange
  end
  object CurveLineTypeUpDown: TUpDown
    Left = 164
    Top = 152
    Width = 16
    Height = 21
    Associate = CurveLineTypeEdit
    Min = 1
    Max = 10
    Position = 1
    TabOrder = 4
    Wrap = False
    OnClick = OnCurveLinetypeClick
  end
  object PlotsymbolGroupBox: TGroupBox
    Left = 6
    Top = 184
    Width = 225
    Height = 129
    Caption = 'Plotsymbols'
    TabOrder = 5
    object PSCodeLabel: TLabel
      Left = 62
      Top = 22
      Width = 25
      Height = 13
      Caption = 'Code'
    end
    object PSSizeLabel: TLabel
      Left = 32
      Top = 76
      Width = 37
      Height = 13
      Caption = 'Size (%)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object PSColorLabel: TLabel
      Left = 8
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
    object PSColorShape: TShape
      Left = 8
      Top = 32
      Width = 25
      Height = 33
      Hint = 'click to set the plotsymbol color'
      ParentShowHint = False
      Shape = stRoundSquare
      ShowHint = True
      OnMouseDown = OnPSColorMouseDown
    end
    object PSAngleLabel: TLabel
      Left = 120
      Top = 76
      Width = 74
      Height = 13
      Caption = 'Angle (degrees)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object PSCodeEdit: TEdit
      Left = 62
      Top = 36
      Width = 42
      Height = 21
      Hint = 'set the plotsymbol code (1 - 18) '
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      Text = '1'
      OnChange = OnPSCodeChange
    end
    object PSCodeUpDown: TUpDown
      Left = 104
      Top = 36
      Width = 16
      Height = 21
      Associate = PSCodeEdit
      Min = 0
      Max = 18
      Position = 1
      TabOrder = 1
      Wrap = False
      OnClick = OnPSCodeClick
    end
    object PSConnectCheckBox: TCheckBox
      Left = 152
      Top = 40
      Width = 65
      Height = 17
      Hint = 'toggle connecting the plotsymbols'
      Caption = 'Connect'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      OnClick = OnPSCodeChange
    end
    object PSSizeEdit: TEdit
      Left = 32
      Top = 92
      Width = 57
      Height = 21
      Hint = 'set the plotsymbol size (as a percentage of the window)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
      Text = '0'
      OnChange = OnPSSizeChange
    end
    object PSSizeUpDown: TUpDown
      Left = 89
      Top = 92
      Width = 17
      Height = 21
      Min = 0
      Max = 1000
      Position = 0
      TabOrder = 4
      Wrap = False
      OnClick = OnPSSizeClick
    end
    object PSAngleEdit: TEdit
      Left = 120
      Top = 92
      Width = 57
      Height = 21
      Hint = 'set the plotsymbol angle (from 0 to 360 degrees)'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 5
      Text = '0'
      OnChange = OnPSAngleChange
    end
    object PSAngleUpDown: TUpDown
      Left = 177
      Top = 92
      Width = 16
      Height = 21
      Associate = PSAngleEdit
      Min = 0
      Max = 360
      Position = 0
      TabOrder = 6
      Wrap = False
      OnClick = OnPSAngleClick
    end
  end
  object CloseButton: TButton
    Left = 81
    Top = 322
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 6
    OnClick = OnCloseClick
  end
end
