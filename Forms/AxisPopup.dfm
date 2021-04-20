object AxisPopupForm: TAxisPopupForm
  Left = 954
  Top = 264
  Width = 260
  Height = 375
  Caption = 'Axis Popup Editor'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  PixelsPerInch = 96
  TextHeight = 13
  object CloseButton: TButton
    Left = 88
    Top = 312
    Width = 75
    Height = 25
    Hint = 'click to close this form'
    Caption = 'Close'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnClick = OnCloseClick
  end
  object TopPageControl: TPageControl
    Left = 2
    Top = 2
    Width = 250
    Height = 303
    ActivePage = NumbersTabSheet
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object AxisTabSheet: TTabSheet
      Caption = 'Axis'
      object AxisColorLabel: TLabel
        Left = 8
        Top = 72
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
      object AxisColorShape: TShape
        Left = 8
        Top = 84
        Width = 25
        Height = 33
        Hint = 'click to set the axis color'
        ParentShowHint = False
        Shape = stRoundSquare
        ShowHint = True
        OnMouseDown = OnAxisColorMouseDown
      end
      object AxisLinewidthLabel: TLabel
        Left = 51
        Top = 72
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
      object LTicLenLabel: TLabel
        Left = 21
        Top = 232
        Width = 89
        Height = 13
        Caption = 'Major tic length (%)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object NsincsLabel: TLabel
        Left = 131
        Top = 176
        Width = 57
        Height = 13
        Caption = '# minor incs'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object STicLenLabel: TLabel
        Left = 131
        Top = 232
        Width = 89
        Height = 13
        Caption = 'Minor tic length (%)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object Bevel1: TBevel
        Left = 16
        Top = 128
        Width = 217
        Height = 9
        Shape = bsTopLine
      end
      object AxisOnCheckBox: TCheckBox
        Left = 5
        Top = 20
        Width = 65
        Height = 17
        Hint = 'toggle display of the axis'
        Caption = 'Draw axis'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        OnClick = OnDrawAxisClick
      end
      object DrawGraphBoxCheckBox: TCheckBox
        Left = 5
        Top = 44
        Width = 97
        Height = 17
        Caption = 'Draw graph box'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        OnClick = OnDrawGraphBoxClick
      end
      object AxisLinewidthEdit: TEdit
        Left = 51
        Top = 88
        Width = 35
        Height = 21
        Hint = 'set the line width of the axis'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        Text = '1'
        OnChange = OnAxisLinewidthChange
      end
      object AxisLinewidthUpDown: TUpDown
        Left = 86
        Top = 88
        Width = 16
        Height = 21
        Associate = AxisLinewidthEdit
        Min = 1
        Max = 10
        Position = 1
        TabOrder = 3
        Wrap = False
        OnClick = OnAxisLinewidthUpDownClick
      end
      object TicsOnCheckBox: TCheckBox
        Left = 80
        Top = 140
        Width = 81
        Height = 17
        Hint = 'toggle display of the tic marks'
        Caption = 'Display tics'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 4
        OnClick = OnDisplayTics
      end
      object LTicLenEdit: TEdit
        Left = 21
        Top = 248
        Width = 57
        Height = 21
        Hint = 'set the large tic length (as a percentage of the window)'
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
        OnChange = OnLTicLenChange
      end
      object LTicLenUpDown: TUpDown
        Left = 78
        Top = 248
        Width = 17
        Height = 21
        Min = 0
        Max = 1000
        Position = 0
        TabOrder = 6
        Wrap = False
        OnClick = OnLTicLenClick
      end
      object NsincsEdit: TEdit
        Left = 131
        Top = 192
        Width = 40
        Height = 21
        Hint = 'set the number of minor divisions (between major tics)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 7
        Text = '1'
        OnChange = OnNsincsChange
      end
      object NsincsUpDown: TUpDown
        Left = 171
        Top = 192
        Width = 16
        Height = 21
        Associate = NsincsEdit
        Min = 1
        Max = 50
        Position = 1
        TabOrder = 8
        Wrap = False
        OnClick = OnNsincsClick
      end
      object STicLenEdit: TEdit
        Left = 131
        Top = 248
        Width = 57
        Height = 21
        Hint = 'set the minor tic length (as a percentage of the window)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 9
        Text = '0'
        OnChange = OnSTicLenChange
      end
      object STicLenUpDown: TUpDown
        Left = 188
        Top = 248
        Width = 17
        Height = 21
        Min = 0
        Max = 1000
        Position = 0
        TabOrder = 10
        Wrap = False
        OnClick = OnSTicLenClick
      end
      object Panel1: TPanel
        Left = 120
        Top = 10
        Width = 105
        Height = 100
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 11
        object GridLineTypeLabel: TLabel
          Left = 22
          Top = 56
          Width = 61
          Height = 13
          Caption = 'Grid line type'
        end
        object GridNoneRB: TRadioButton
          Left = 5
          Top = 5
          Width = 90
          Height = 17
          Hint = 'choose this for no grid lines for this axis'
          Caption = 'No grid lines'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 0
          OnClick = OnNoGridLinesClick
        end
        object GridMajorRB: TRadioButton
          Left = 5
          Top = 20
          Width = 90
          Height = 17
          Hint = 'choose this for grid lines at every major tic for this axis'
          Caption = 'At major tics'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          OnClick = OnMajorTicGridLinesClick
        end
        object GridAllRB: TRadioButton
          Left = 5
          Top = 35
          Width = 90
          Height = 17
          Hint = 'choose this for grid lines at major and minor tics for this axis'
          Caption = 'At all tics'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 2
          OnClick = OnAllTicsGridLinesClick
        end
        object GridLineTypeEdit: TEdit
          Left = 22
          Top = 72
          Width = 42
          Height = 21
          Hint = 'set the line type for the grid lines '
          ParentShowHint = False
          ShowHint = True
          TabOrder = 3
          Text = '1'
          OnChange = OnGridLineTypeEditChange
        end
        object GridLineTypeUpDown: TUpDown
          Left = 64
          Top = 72
          Width = 16
          Height = 21
          Associate = GridLineTypeEdit
          Min = 1
          Max = 10
          Position = 1
          TabOrder = 4
          Wrap = False
          OnClick = OnGridLineTypeUpDownClick
        end
      end
      object TicRadioGroup: TRadioGroup
        Left = 16
        Top = 160
        Width = 73
        Height = 65
        Caption = 'Tics'
        Items.Strings = (
          'Inside'
          'Outside'
          'Both')
        TabOrder = 12
        OnClick = OnTicDirectionClick
      end
    end
    object NumbersTabSheet: TTabSheet
      Caption = 'Numbering'
      ImageIndex = 1
      object NumbersFontLabel: TLabel
        Left = 18
        Top = 16
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
      object NumbersColorLabel: TLabel
        Left = 18
        Top = 64
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
      object NumbersColorShape: TShape
        Left = 18
        Top = 76
        Width = 25
        Height = 33
        Hint = 'click to set the axis numbering color'
        ParentShowHint = False
        Shape = stRoundSquare
        ShowHint = True
        OnMouseDown = OnNumbersColorMouseDown
      end
      object NumbersHeightLabel: TLabel
        Left = 61
        Top = 64
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
      object NumbersAngleLabel: TLabel
        Left = 149
        Top = 64
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
      object NumbersOnCheckBox: TCheckBox
        Left = 68
        Top = 4
        Width = 105
        Height = 17
        Hint = 'toggle display of the axis numbering'
        Caption = 'Draw axis values'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        OnClick = OnDisplayNumbersClick
      end
      object NumbersFontComboBox: TComboBox
        Left = 15
        Top = 28
        Width = 212
        Height = 21
        Hint = 'choose the axis numbering font'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 13
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        OnChange = OnSelectNumbersFont
      end
      object NumbersHeightEdit: TEdit
        Left = 61
        Top = 80
        Width = 57
        Height = 21
        Hint = 'set the axis numbering height (as a percentage of the window)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        Text = '0'
        OnChange = OnNumbersHeightEditChange
      end
      object NumbersHeightUpDown: TUpDown
        Left = 118
        Top = 80
        Width = 17
        Height = 21
        Min = 0
        Max = 1000
        Position = 0
        TabOrder = 3
        Wrap = False
        OnClick = OnNumbersHeightUpDownClick
      end
      object NumbersAngleEdit: TEdit
        Left = 149
        Top = 80
        Width = 57
        Height = 21
        Hint = 'set the angle of the axis numbering (from 0 to 360 degrees)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 4
        Text = '0'
        OnChange = OnNumbersAngleChange
      end
      object NumbersAngleUpDown: TUpDown
        Left = 206
        Top = 80
        Width = 16
        Height = 21
        Associate = NumbersAngleEdit
        Min = 0
        Max = 360
        Position = 0
        TabOrder = 5
        Wrap = False
        OnClick = OnNumbersAngleUpDownClick
      end
      object LogStyleCheckBox: TCheckBox
        Left = 49
        Top = 120
        Width = 144
        Height = 17
        Hint = 'toggle decimal or exponential style numbers'
        Caption = 'Exponential style numbers'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 6
        OnClick = OnLogStyleClick
      end
    end
    object LabelTabSheet: TTabSheet
      Caption = 'Label'
      ImageIndex = 2
      object LabelLabel: TLabel
        Left = 15
        Top = 32
        Width = 48
        Height = 13
        Caption = 'Axis Label'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object LabelFontLabel: TLabel
        Left = 15
        Top = 71
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
      object LabelColorLabel: TLabel
        Left = 15
        Top = 112
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
      object LabelColorShape: TShape
        Left = 15
        Top = 124
        Width = 25
        Height = 33
        Hint = 'click to set the axis label color'
        ParentShowHint = False
        Shape = stRoundSquare
        ShowHint = True
        OnMouseDown = OnLabelColorMouseDown
      end
      object LabelHeightLabel: TLabel
        Left = 61
        Top = 112
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
      object LabelOnCheckBox: TCheckBox
        Left = 84
        Top = 12
        Width = 73
        Height = 17
        Hint = 'toggle display of the axis label'
        Caption = 'Draw label'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        OnClick = OnDisplayLabelClick
      end
      object LabelEdit: TEdit
        Left = 15
        Top = 46
        Width = 209
        Height = 21
        Hint = 'enter the axis label (hit Enter key to accept label)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
        OnExit = OnAxisLabelEditExit
        OnKeyPress = OnAxisLabelEditKeyPress
      end
      object LabelFontComboBox: TComboBox
        Left = 15
        Top = 84
        Width = 212
        Height = 21
        Hint = 'choose the axis label font'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ItemHeight = 13
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        OnChange = OnSelectLabelFont
      end
      object LabelHeightEdit: TEdit
        Left = 61
        Top = 128
        Width = 57
        Height = 21
        Hint = 'set the axis label height (as a percentage of the window)'
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
        OnChange = OnLabelHeightEditChange
      end
      object LabelHeightUpDown: TUpDown
        Left = 118
        Top = 128
        Width = 17
        Height = 21
        Min = 0
        Max = 1000
        Position = 0
        TabOrder = 4
        Wrap = False
        OnClick = OnLabelHeightUpDownClick
      end
    end
    object LocationTabSheet: TTabSheet
      Caption = 'Location'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ImageIndex = 3
      ParentFont = False
      object LocationLowLabel: TLabel
        Left = 34
        Top = 20
        Width = 67
        Height = 13
        Caption = 'Lower axis (%)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object LocationUpLabel: TLabel
        Left = 133
        Top = 20
        Width = 67
        Height = 13
        Caption = 'Upper axis (%)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object LocationLowEdit: TEdit
        Left = 34
        Top = 36
        Width = 57
        Height = 21
        Hint = 'set the axis lower location (as a percentage of the window)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        Text = '0'
        OnChange = OnLocationLowEditChange
      end
      object LocationLowUpDown: TUpDown
        Left = 91
        Top = 36
        Width = 17
        Height = 21
        Min = 0
        Max = 1000
        Position = 0
        TabOrder = 1
        Wrap = False
        OnClick = OnLocationLowUpDownClick
      end
      object LocationUpEdit: TEdit
        Left = 133
        Top = 36
        Width = 57
        Height = 21
        Hint = 'set the axis upper location (as a percentage of the window)'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 2
        Text = '0'
        OnChange = OnLocationUpEditChange
      end
      object LocationUpUpDown: TUpDown
        Left = 190
        Top = 36
        Width = 17
        Height = 21
        Min = 0
        Max = 1000
        Position = 0
        TabOrder = 3
        Wrap = False
        OnClick = OnLocationUpUpDownClick
      end
    end
    object ScalesTabSheet: TTabSheet
      Caption = 'Scales'
      ImageIndex = 4
      object MinLabel: TLabel
        Left = 14
        Top = 25
        Width = 16
        Height = 13
        Caption = 'min'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object MaxLabel: TLabel
        Left = 126
        Top = 25
        Width = 19
        Height = 13
        Caption = 'max'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
      end
      object MinEdit: TEdit
        Left = 34
        Top = 22
        Width = 80
        Height = 21
        Hint = 'change the axis minimum value'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
      end
      object MaxEdit: TEdit
        Left = 148
        Top = 22
        Width = 80
        Height = 21
        Hint = 'change the axis maximum value'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
      end
      object LogRadioGroup: TRadioGroup
        Left = 68
        Top = 50
        Width = 105
        Height = 105
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        Items.Strings = (
          'linear axis'
          'log base 2'
          'log base e'
          'log base 10')
        ParentFont = False
        TabOrder = 2
        OnClick = OnLogClick
      end
      object RescaleButton: TButton
        Left = 71
        Top = 165
        Width = 100
        Height = 25
        Hint = 'click to apply the scaling'
        Caption = 'Apply rescaling'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
        OnClick = OnApplyRescaling
      end
    end
  end
end
