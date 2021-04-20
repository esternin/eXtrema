object FitNoviceForm: TFitNoviceForm
  Left = 685
  Top = 428
  Width = 715
  Height = 527
  Caption = 'Fit  (novice mode)'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnActivate = FormActivate
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  PixelsPerInch = 96
  TextHeight = 13
  object DataVecLabel: TLabel
    Left = 58
    Top = 20
    Width = 56
    Height = 13
    Caption = 'Data vector'
  end
  object ExpressionLabel: TLabel
    Left = 18
    Top = 180
    Width = 141
    Height = 13
    Caption = 'Fit expression:   Data vector ='
    Enabled = False
  end
  object IndepVecLabel: TLabel
    Left = 22
    Top = 52
    Width = 93
    Height = 13
    Caption = 'Independent vector'
    Enabled = False
  end
  object FitTypeLabel: TLabel
    Left = 66
    Top = 116
    Width = 47
    Height = 13
    Caption = 'Type of fit'
    Enabled = False
  end
  object NumberLabel: TLabel
    Left = 15
    Top = 148
    Width = 99
    Height = 13
    Caption = 'Degree of polynomial'
    Enabled = False
    ParentShowHint = False
    ShowHint = False
    Visible = False
  end
  object ErrorVecLabel: TLabel
    Left = 60
    Top = 84
    Width = 55
    Height = 13
    Caption = 'Error vector'
    Enabled = False
  end
  object IVMinLabel: TLabel
    Left = 305
    Top = 32
    Width = 17
    Height = 13
    Caption = 'Min'
    Enabled = False
  end
  object IVMaxLabel: TLabel
    Left = 365
    Top = 32
    Width = 20
    Height = 13
    Caption = 'Max'
    Enabled = False
  end
  object FixedLabel: TLabel
    Left = 82
    Top = 211
    Width = 25
    Height = 13
    Caption = 'Fixed'
  end
  object LegendXLabel: TLabel
    Left = 291
    Top = 421
    Width = 19
    Height = 13
    Caption = '%x :'
  end
  object LegendYLabel: TLabel
    Left = 403
    Top = 421
    Width = 19
    Height = 13
    Caption = '%y :'
  end
  object LegendLocLabel: TLabel
    Left = 204
    Top = 421
    Width = 76
    Height = 13
    Caption = 'Legend location'
    ParentShowHint = False
    ShowHint = False
  end
  object DataVecComboBox: TComboBox
    Left = 120
    Top = 16
    Width = 160
    Height = 21
    Hint = 'Choose the vector containing the data to fit'
    Style = csDropDownList
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnChange = DataVecChange
    OnDropDown = UpdateVectors
  end
  object FitResultsGroupBox: TGroupBox
    Left = 424
    Top = 8
    Width = 275
    Height = 129
    Caption = 'Fit Results'
    Enabled = False
    TabOrder = 1
    object ChiSquareLabel: TLabel
      Left = 5
      Top = 28
      Width = 79
      Height = 13
      Caption = 'Total Chi Square'
    end
    object ConfidenceLevelLabel: TLabel
      Left = 5
      Top = 60
      Width = 83
      Height = 13
      Caption = 'Confidence Level'
    end
    object DegFreeLabel: TLabel
      Left = 5
      Top = 92
      Width = 93
      Height = 13
      Caption = 'Degrees of freedom'
    end
    object ChiSquareEdit: TEdit
      Left = 107
      Top = 24
      Width = 150
      Height = 21
      Hint = 'displays the total chi square of the fit'
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 0
    end
    object ConfidenceLevelEdit: TEdit
      Left = 107
      Top = 56
      Width = 150
      Height = 21
      Hint = 'displays the confidence level of the fit'
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 1
    end
    object DegFreeEdit: TEdit
      Left = 107
      Top = 88
      Width = 150
      Height = 21
      Hint = 'displays the number of degrees of freedom for the fit'
      ParentShowHint = False
      ReadOnly = True
      ShowHint = True
      TabOrder = 2
    end
  end
  object DoFitButton: TButton
    Left = 356
    Top = 456
    Width = 80
    Height = 25
    Caption = 'Do the fit'
    TabOrder = 2
    OnClick = DoFitButtonClick
  end
  object CloseButton: TButton
    Left = 442
    Top = 456
    Width = 80
    Height = 25
    Caption = 'Close'
    TabOrder = 3
    OnClick = CloseButtonClick
  end
  object IndepVecComboBox: TComboBox
    Left = 120
    Top = 48
    Width = 160
    Height = 21
    Hint = 'Choose the vector containing the independent data'
    Style = csDropDownList
    Enabled = False
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    OnChange = IndepVecChange
    OnDropDown = UpdateVectors
  end
  object FitTypeComboBox: TComboBox
    Left = 120
    Top = 112
    Width = 160
    Height = 21
    Hint = 'Choose the type of fit to perform'
    Style = csDropDownList
    Enabled = False
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
    OnChange = FitTypeChange
  end
  object NumberComboBox: TComboBox
    Left = 120
    Top = 144
    Width = 57
    Height = 21
    Hint = 'Choose the degree of the polynomial to fit'
    Style = csDropDownList
    Enabled = False
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 6
    Visible = False
    OnChange = NumberChange
    Items.Strings = (
      '1'
      '2'
      '3'
      '4'
      '5'
      '6'
      '7'
      '8')
  end
  object ParameterStringGrid: TStringGrid
    Left = 115
    Top = 208
    Width = 480
    Height = 193
    Hint = 
      'fitting parameters are displayed here, the start values are chan' +
      'geable by the user'
    ColCount = 3
    DefaultColWidth = 172
    DefaultRowHeight = 20
    FixedCols = 0
    RowCount = 9
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 7
    OnSelectCell = SelectCell
    ColWidths = (
      173
      150
      150)
  end
  object TestFitButton: TButton
    Left = 185
    Top = 456
    Width = 80
    Height = 25
    Hint = 
      'click to plot data and the fit expression using the current para' +
      'meter values'
    Caption = 'Test the fit'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 8
    OnClick = TestFitButtonClick
  end
  object ClearButton: TButton
    Left = 271
    Top = 456
    Width = 80
    Height = 25
    Hint = 'click to clear the graphics window'
    Caption = 'Clear graphics'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 9
    OnClick = ClearButtonClick
  end
  object ErrorVecComboBox: TComboBox
    Left = 120
    Top = 80
    Width = 160
    Height = 21
    Hint = 'Choose the vector containing the errors on the independent data'
    Style = csDropDownList
    Enabled = False
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 10
    OnChange = ErrorVecChange
    OnDropDown = UpdateVectors
  end
  object IVMinEdit: TEdit
    Left = 290
    Top = 48
    Width = 50
    Height = 21
    Hint = 
      'enter the independent vector minimum value to change the range o' +
      'f the fit'
    Enabled = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 11
  end
  object IVMaxEdit: TEdit
    Left = 350
    Top = 48
    Width = 50
    Height = 21
    Hint = 
      'enter the independent vector maximum value to change the range o' +
      'f the fit'
    Enabled = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 12
  end
  object FixedCheckBox1: TCheckBox
    Left = 88
    Top = 232
    Width = 17
    Height = 17
    TabOrder = 13
  end
  object FixedCheckBox2: TCheckBox
    Left = 88
    Top = 253
    Width = 17
    Height = 17
    TabOrder = 14
  end
  object FixedCheckBox3: TCheckBox
    Left = 88
    Top = 273
    Width = 17
    Height = 17
    TabOrder = 15
  end
  object FixedCheckBox4: TCheckBox
    Left = 88
    Top = 294
    Width = 17
    Height = 17
    TabOrder = 16
  end
  object FixedCheckBox5: TCheckBox
    Left = 88
    Top = 318
    Width = 17
    Height = 17
    TabOrder = 17
  end
  object FixedCheckBox6: TCheckBox
    Left = 88
    Top = 341
    Width = 17
    Height = 17
    TabOrder = 18
  end
  object FixedCheckBox7: TCheckBox
    Left = 88
    Top = 361
    Width = 17
    Height = 17
    TabOrder = 19
  end
  object FixedCheckBox8: TCheckBox
    Left = 88
    Top = 381
    Width = 17
    Height = 17
    TabOrder = 20
  end
  object LegendXEdit: TEdit
    Left = 311
    Top = 416
    Width = 80
    Height = 21
    Hint = 
      'enter the %x coordinate of the upper left hand corner of the leg' +
      'end'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 21
  end
  object LegendYEdit: TEdit
    Left = 423
    Top = 416
    Width = 80
    Height = 21
    Hint = 
      'enter the %y coordinate of the upper left hand corner of the leg' +
      'end'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 22
  end
  object ExpressionRichEdit: TRichEdit
    Left = 160
    Top = 176
    Width = 465
    Height = 21
    ReadOnly = True
    TabOrder = 23
    WantReturns = False
    WordWrap = False
    OnChange = ExpressionEditChange
  end
end
