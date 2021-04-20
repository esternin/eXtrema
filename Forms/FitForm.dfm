object FitCommandForm: TFitCommandForm
  Left = 411
  Top = 310
  Width = 783
  Height = 399
  BorderIcons = []
  Caption = 'Fit Command'
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
  object OutputVariablesGroupBox: TGroupBox
    Left = 5
    Top = 5
    Width = 386
    Height = 185
    Caption = 'Output variables'
    Color = clBtnFace
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    ParentShowHint = False
    ShowHint = False
    TabOrder = 0
    object CHISQCheckBox: TCheckBox
      Left = 5
      Top = 20
      Width = 155
      Height = 17
      Hint = 
        'if checked, a new scalar variable, FIT$CHISQ,  will be created c' +
        'ontaining the total chi square for the fit'
      Caption = 'Chi Square'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
    object CLCheckBox: TCheckBox
      Left = 5
      Top = 40
      Width = 155
      Height = 17
      Hint = 
        'if checked, a new scalar variable, FIT$CL,  will be created cont' +
        'aining the confidence level for the fit'
      Caption = 'Confidence Level'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object CORRMATCheckBox: TCheckBox
      Left = 5
      Top = 60
      Width = 155
      Height = 17
      Hint = 
        'if checked, a new matrix variable, FIT$CORR,  will be created co' +
        'ntaining the correlation matrix for the fit'
      Caption = 'Correlation matrix'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
    end
    object COVMATCheckBox: TCheckBox
      Left = 5
      Top = 80
      Width = 155
      Height = 17
      Hint = 
        'if checked, a new matrix variable, FIT$COVM,  will be created co' +
        'ntaining the covariance matrix for the fit'
      Caption = 'Covariance matrix'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
    end
    object E1CheckBox: TCheckBox
      Left = 5
      Top = 100
      Width = 155
      Height = 17
      Hint = 
        'if checked, a new vector variable, FIT$E1,  will be created cont' +
        'aining the root mean square statistical error for each parameter'
      Caption = 'Statistical errors of estimate'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 4
    end
    object E2CheckBox: TCheckBox
      Left = 5
      Top = 120
      Width = 155
      Height = 17
      Hint = 
        'if checked, a new vector variable, FIT$E2,  will be created cont' +
        'aining the root mean square total error of estimate for each par' +
        'ameter'
      Caption = 'Total errors of estimate'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 5
    end
    object VARNAMESCheckBox: TCheckBox
      Left = 5
      Top = 140
      Width = 155
      Height = 17
      Hint = 
        'if this box is checked, a new string array variable, FIT$VAR, wi' +
        'll be created containing the names of the variable parameters us' +
        'ed in the fit'
      Caption = 'Variable names'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 6
    end
    object FREECheckBox: TCheckBox
      Left = 5
      Top = 160
      Width = 155
      Height = 17
      Hint = 
        'if this box is checked, a new scalar variable, FIT$FREE, will be' +
        ' created containing the number of degrees of freedom of the fit'
      Caption = 'Degrees of freedom'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 7
    end
    object CHISQEdit: TEdit
      Left = 175
      Top = 18
      Width = 121
      Height = 21
      Hint = 'this field will contain the total chi square of the fit'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 8
    end
    object CLEdit: TEdit
      Left = 175
      Top = 40
      Width = 121
      Height = 21
      Hint = 'this field will contain the confidence level of the fit'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 9
    end
    object FREEEdit: TEdit
      Left = 175
      Top = 160
      Width = 121
      Height = 21
      Hint = 
        'this field will contain the number of degrees of freedom of the ' +
        'fit'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 10
    end
  end
  object UPDATEPanel: TPanel
    Left = 450
    Top = 250
    Width = 280
    Height = 71
    TabOrder = 1
    object UPDATELabel: TLabel
      Left = 175
      Top = 40
      Width = 94
      Height = 13
      Caption = 'Output vector name'
    end
    object UPDATEEdit: TEdit
      Left = 5
      Top = 35
      Width = 165
      Height = 21
      Hint = 
        'enter the output vector to contain the expression values using t' +
        'he calculated fit parameters'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
    object UPDATEButton: TButton
      Left = 102
      Top = 5
      Width = 75
      Height = 25
      Hint = 
        'click here to evaluate the previously fitted expression for the ' +
        'current parameter values into the output vector entered below'
      Caption = 'Update'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      OnClick = UPDATEButtonClick
    end
  end
  object FitTypeRadioGroup: TRadioGroup
    Left = 270
    Top = 195
    Width = 81
    Height = 66
    Hint = 
      'if your data errors are distributed in a normal distribution, us' +
      'e Normal, but if your data errors are distributed in a Poisson d' +
      'istribution, choose Poisson'
    Caption = 'Fit type'
    Color = clBtnFace
    Items.Strings = (
      'Normal'
      'Poisson')
    ParentColor = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnClick = SetFitType
  end
  object RESETPanel: TPanel
    Left = 5
    Top = 195
    Width = 261
    Height = 121
    TabOrder = 3
    object ITMAXLabel: TLabel
      Left = 130
      Top = 55
      Width = 87
      Height = 13
      Caption = 'Max # of iterations'
    end
    object TOLERANCELabel: TLabel
      Left = 130
      Top = 85
      Width = 58
      Height = 13
      Caption = 'Fit tolerance'
    end
    object RESETCheckBox: TCheckBox
      Left = 5
      Top = 5
      Width = 110
      Height = 17
      Hint = 
        'if this box is checked, the fit parameters will be reset to thei' +
        'r original values after an unsuccessful fit'
      Caption = 'Reset parameters'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
    object ITMAXEdit: TEdit
      Left = 5
      Top = 50
      Width = 121
      Height = 21
      Hint = 'enter the maximum number of iteration steps to allow in a fit'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object TOLERANCEEdit: TEdit
      Left = 5
      Top = 80
      Width = 121
      Height = 21
      Hint = 'enter the fit success tolerance'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
    end
    object ZEROSCheckBox: TCheckBox
      Left = 5
      Top = 25
      Width = 97
      Height = 17
      Hint = 
        'if this box is checked, any zeros of the weight vector will not ' +
        'be used to calculate the number of degrees of freedom'
      Caption = 'Zeros'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
    end
  end
  object MainInputPanel: TPanel
    Left = 395
    Top = 140
    Width = 370
    Height = 106
    TabOrder = 4
    object DataVectorLabel: TLabel
      Left = 130
      Top = 15
      Width = 56
      Height = 13
      Caption = 'Data vector'
    end
    object ExpressionLabel: TLabel
      Left = 310
      Top = 75
      Width = 51
      Height = 13
      Caption = 'Expression'
    end
    object WeightVectorLabel: TLabel
      Left = 130
      Top = 45
      Width = 118
      Height = 13
      Caption = 'Weights vector (optional)'
    end
    object DataVectorEdit: TEdit
      Left = 5
      Top = 10
      Width = 121
      Height = 21
      Hint = 'enter the data vector'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
    object ExpressionEdit: TEdit
      Left = 5
      Top = 70
      Width = 300
      Height = 21
      Hint = 'enter the expression containing the variable fit parameters'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object WeightVectorEdit: TEdit
      Left = 5
      Top = 40
      Width = 121
      Height = 21
      Hint = 'enter the vector of weights (optional)'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
    end
  end
  object ParameterStringGrid: TStringGrid
    Left = 450
    Top = 5
    Width = 267
    Height = 128
    Hint = 'parameters and values after the fit'
    ColCount = 2
    FixedCols = 0
    Font.Charset = OEM_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Terminal'
    Font.Style = []
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
    ColWidths = (
      130
      132)
  end
  object FitButton: TButton
    Left = 345
    Top = 335
    Width = 75
    Height = 25
    Caption = 'Do the Fit'
    TabOrder = 6
    OnClick = DoTheFitClick
  end
  object CloseButton: TButton
    Left = 423
    Top = 335
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 7
    OnClick = CloseClick
  end
  object DefaultsButton: TButton
    Left = 267
    Top = 335
    Width = 75
    Height = 25
    Caption = 'Defaults'
    TabOrder = 8
    OnClick = SetDefaults
  end
  object HelpButton: TButton
    Left = 501
    Top = 335
    Width = 75
    Height = 25
    Caption = 'Help'
    TabOrder = 9
    OnClick = HelpClick
  end
end
