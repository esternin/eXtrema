object ReadMatForm: TReadMatForm
  Left = 533
  Top = 274
  Width = 460
  Height = 291
  Caption = 'Read matrix'
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
  object Panel1: TPanel
    Left = 0
    Top = 5
    Width = 241
    Height = 101
    TabOrder = 0
    object MatrixNameLabel: TLabel
      Left = 135
      Top = 15
      Width = 57
      Height = 13
      Caption = 'Matrix name'
    end
    object NumRowsLabel: TLabel
      Left = 135
      Top = 45
      Width = 74
      Height = 13
      Caption = 'Number of rows'
    end
    object NumColmsLabel: TLabel
      Left = 135
      Top = 75
      Width = 91
      Height = 13
      Caption = 'Number of columns'
    end
    object MatrixNameEdit: TEdit
      Left = 5
      Top = 10
      Width = 121
      Height = 21
      TabOrder = 0
    end
    object NumRowsEdit: TEdit
      Left = 5
      Top = 40
      Width = 121
      Height = 21
      TabOrder = 1
    end
    object NumColmsEdit: TEdit
      Left = 5
      Top = 70
      Width = 121
      Height = 21
      TabOrder = 2
    end
  end
  object FormatPanel: TPanel
    Left = 257
    Top = 5
    Width = 176
    Height = 65
    TabOrder = 1
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
  object OptionsPanel: TPanel
    Left = 250
    Top = 75
    Width = 191
    Height = 86
    TabOrder = 2
    object Shape3: TShape
      Left = 5
      Top = 48
      Width = 181
      Height = 2
    end
    object DisplayMessagesCheckBox: TCheckBox
      Left = 5
      Top = 60
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
    object CloseFirstCheckBox: TCheckBox
      Left = 5
      Top = 5
      Width = 145
      Height = 17
      Caption = 'Close File Before Reading'
      Checked = True
      State = cbChecked
      TabOrder = 1
    end
    object CloseAfterCheckBox: TCheckBox
      Left = 5
      Top = 25
      Width = 135
      Height = 17
      Caption = 'Close File After Reading'
      Checked = True
      State = cbChecked
      TabOrder = 2
    end
  end
  object LineRangePanel: TPanel
    Left = 70
    Top = 115
    Width = 126
    Height = 41
    TabOrder = 3
    object LineRangeLabel: TLabel
      Left = 58
      Top = 13
      Width = 55
      Height = 13
      Caption = 'Starting line'
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
  object ChooseFileButton: TButton
    Left = 3
    Top = 185
    Width = 75
    Height = 21
    Hint = 'pick a file to read'
    Caption = 'Browse'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    OnClick = ChooseFileClick
  end
  object FilenameComboBox: TComboBox
    Left = 88
    Top = 185
    Width = 353
    Height = 21
    Hint = 'files used (current file at top)'
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
    OnClick = FilenameComboBoxClick
  end
  object ApplyButton: TButton
    Left = 141
    Top = 220
    Width = 75
    Height = 25
    Caption = 'Apply'
    TabOrder = 6
    OnClick = ApplyClick
  end
  object CloseButton: TButton
    Left = 221
    Top = 220
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 7
    OnClick = CloseClick
  end
  object ReadDialog: TOpenDialog
    Filter = 
      'data files (*.dat)|*.dat|text files (*.txt)|*.txt|all files (*.*' +
      ')|*.*'
    Left = 15
    Top = 115
  end
end
