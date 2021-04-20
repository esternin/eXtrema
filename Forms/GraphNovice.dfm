object GraphNoviceForm: TGraphNoviceForm
  Left = 340
  Top = 714
  Width = 495
  Height = 230
  Caption = 'Graph Form (novice mode)'
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
  object DepVarLabel: TLabel
    Left = 35
    Top = 20
    Width = 141
    Height = 13
    Caption = 'Dependent Variable (required)'
  end
  object IndepVarLabel: TLabel
    Left = 29
    Top = 52
    Width = 147
    Height = 13
    Caption = 'Independent Variable (optional)'
  end
  object ErrorsLabel: TLabel
    Left = 103
    Top = 84
    Width = 73
    Height = 13
    Caption = 'Errors (optional)'
  end
  object PlotSymbolLabel: TLabel
    Left = 122
    Top = 116
    Width = 53
    Height = 13
    Caption = 'Plot symbol'
  end
  object IVMinLabel: TLabel
    Left = 369
    Top = 32
    Width = 17
    Height = 13
    Caption = 'Min'
    Enabled = False
  end
  object IVMaxLabel: TLabel
    Left = 429
    Top = 32
    Width = 20
    Height = 13
    Caption = 'Max'
    Enabled = False
  end
  object DepVarComboBox: TComboBox
    Left = 187
    Top = 16
    Width = 145
    Height = 21
    Hint = 'choose the dependent variable vector (required)'
    Style = csDropDownList
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnDropDown = UpdateVars
  end
  object IndepVarComboBox: TComboBox
    Left = 187
    Top = 48
    Width = 145
    Height = 21
    Hint = 'choose the independent variablt vector (optional)'
    Style = csDropDownList
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    OnChange = IndepVarChange
    OnDropDown = UpdateVars
  end
  object ErrorsComboBox: TComboBox
    Left = 187
    Top = 80
    Width = 145
    Height = 21
    Hint = 'choose the error vector (optional)'
    Style = csDropDownList
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnDropDown = UpdateVars
  end
  object DrawGraphButton: TButton
    Left = 122
    Top = 160
    Width = 75
    Height = 25
    Hint = 'plot the vectors chosen above'
    Caption = 'Draw'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = DrawGraphClick
  end
  object ClearButton: TButton
    Left = 206
    Top = 160
    Width = 75
    Height = 25
    Hint = 'clear the graphics'
    Caption = 'Clear'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    OnClick = ClearClick
  end
  object CloseButton: TButton
    Left = 290
    Top = 160
    Width = 75
    Height = 25
    Hint = 'close this form'
    Caption = 'Close'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
    OnClick = CloseClick
  end
  object PlotSymbolComboBox: TComboBox
    Left = 187
    Top = 111
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 6
    OnChange = PlotSymbolChange
    Items.Strings = (
      '<none>'
      'square'
      'square filled'
      'cross'
      'square with cross'
      'plus'
      'diamond'
      'diamond with plus'
      'diamond filled'
      'asterix'
      'triangle'
      'triangle filled'
      'circle'
      'circle filled'
      'star'
      'star filled'
      'point'
      'arrow starting at data'
      'arrow centred at data')
  end
  object IVMinEdit: TEdit
    Left = 354
    Top = 48
    Width = 50
    Height = 21
    Hint = 
      'enter the independent vector minimum value to change the range o' +
      'f the fit'
    Enabled = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 7
  end
  object IVMaxEdit: TEdit
    Left = 414
    Top = 48
    Width = 50
    Height = 21
    Hint = 
      'enter the independent vector maximum value to change the range o' +
      'f the fit'
    Enabled = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 8
  end
  object ConnectCheckBox: TCheckBox
    Left = 344
    Top = 113
    Width = 120
    Height = 17
    Hint = 'check to connect the plotsymbols with line segments'
    Caption = 'Connect plot symbols'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 9
  end
end
