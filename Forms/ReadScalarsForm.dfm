object ReadScalForm: TReadScalForm
  Left = 633
  Top = 444
  Width = 488
  Height = 353
  Caption = 'Read scalars'
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
  object NameColumnStringGrid: TStringGrid
    Left = 4
    Top = 5
    Width = 273
    Height = 162
    Hint = 'scalar names and column numbers'
    ColCount = 2
    DefaultColWidth = 50
    DefaultRowHeight = 15
    FixedCols = 0
    RowCount = 50
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goAlwaysShowEditor]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnSetEditText = NameColumnSetEditText
    ColWidths = (
      201
      50)
  end
  object LineRangePanel: TPanel
    Left = 70
    Top = 175
    Width = 126
    Height = 41
    TabOrder = 1
    object LineRangeLabel: TLabel
      Left = 58
      Top = 13
      Width = 56
      Height = 13
      Caption = 'Line to read'
    end
    object LineRangeEdit: TEdit
      Left = 5
      Top = 10
      Width = 50
      Height = 21
      Hint = 'file record from which to read scalars'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      Text = '1'
    end
  end
  object FormatPanel: TPanel
    Left = 292
    Top = 30
    Width = 176
    Height = 65
    TabOrder = 2
    object FormatCheckBox: TCheckBox
      Left = 50
      Top = 10
      Width = 76
      Height = 17
      Hint = 'toggle use format for read'
      Caption = 'Use format'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnClick = UseFormatClick
    end
    object FormatEdit: TEdit
      Left = 13
      Top = 30
      Width = 150
      Height = 21
      Hint = 'C style format'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
  end
  object ChooseFileButton: TButton
    Left = 3
    Top = 225
    Width = 75
    Height = 21
    Hint = 'pick a file to read'
    Caption = 'Browse'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = ChooseFileClick
  end
  object FilenameComboBox: TComboBox
    Left = 88
    Top = 225
    Width = 175
    Height = 21
    Hint = 'files used (current file at top)'
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    OnClick = FilenameComboBoxClick
  end
  object ApplyButton: TButton
    Left = 173
    Top = 270
    Width = 75
    Height = 25
    Caption = 'Apply'
    TabOrder = 5
    OnClick = ApplyClick
  end
  object CloseButton: TButton
    Left = 253
    Top = 270
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 6
    OnClick = CloseClick
  end
  object OptionsPanel: TPanel
    Left = 285
    Top = 115
    Width = 191
    Height = 111
    TabOrder = 7
    object Shape1: TShape
      Left = 5
      Top = 83
      Width = 181
      Height = 2
    end
    object Shape3: TShape
      Left = 5
      Top = 48
      Width = 181
      Height = 2
    end
    object DisplayMessagesCheckBox: TCheckBox
      Left = 5
      Top = 90
      Width = 105
      Height = 17
      Hint = 'toggle warning/informational messages'
      Caption = 'Display messages'
      Checked = True
      ParentShowHint = False
      ShowHint = True
      State = cbChecked
      TabOrder = 0
    end
    object ErrorFillCheckBox: TCheckBox
      Left = 5
      Top = 58
      Width = 97
      Height = 17
      Hint = 'toggle fill data read errors'
      Caption = 'Error Fill'
      Enabled = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      OnClick = ErrorFillClick
    end
    object ErrorFillEdit: TEdit
      Left = 110
      Top = 55
      Width = 75
      Height = 21
      Hint = 'value used for error fill'
      Enabled = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      Text = '0'
      Visible = False
    end
    object CloseFirstCheckBox: TCheckBox
      Left = 5
      Top = 5
      Width = 145
      Height = 17
      Caption = 'Close File Before Reading'
      Checked = True
      State = cbChecked
      TabOrder = 3
    end
    object CloseAfterCheckBox: TCheckBox
      Left = 5
      Top = 25
      Width = 135
      Height = 17
      Caption = 'Close File After Reading'
      Checked = True
      State = cbChecked
      TabOrder = 4
    end
  end
  object ReadDialog: TOpenDialog
    Filter = 
      'data files (*.dat)|*.dat|text files (*.txt)|*.txt|all files (*.*' +
      ')|*.*'
    Left = 340
    Top = 270
  end
end
