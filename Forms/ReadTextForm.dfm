object ReadTxtForm: TReadTxtForm
  Left = 699
  Top = 435
  Width = 440
  Height = 258
  Caption = 'Read text strings'
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
  object OptionsPanel: TPanel
    Left = 20
    Top = 50
    Width = 191
    Height = 81
    TabOrder = 0
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
    Left = 252
    Top = 10
    Width = 176
    Height = 121
    TabOrder = 1
    object LineRange1Label: TLabel
      Left = 78
      Top = 32
      Width = 74
      Height = 13
      Caption = 'First line to read'
      Enabled = False
    end
    object LineRange2Label: TLabel
      Left = 78
      Top = 60
      Width = 75
      Height = 13
      Caption = 'Last line to read'
      Enabled = False
    end
    object LineRange3Label: TLabel
      Left = 78
      Top = 85
      Width = 47
      Height = 13
      Caption = 'Increment'
      Enabled = False
    end
    object LineRange1Edit: TEdit
      Left = 23
      Top = 30
      Width = 50
      Height = 21
      Hint = 'skip records before this value'
      Enabled = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      Text = '1'
    end
    object LineRange2Edit: TEdit
      Left = 23
      Top = 55
      Width = 50
      Height = 21
      Hint = 'leave blank to read to end of file'
      Enabled = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object LineRange3Edit: TEdit
      Left = 23
      Top = 80
      Width = 50
      Height = 21
      Hint = 'record increment'
      Enabled = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      Text = '1'
    end
    object LineRangeCheckBox: TCheckBox
      Left = 38
      Top = 5
      Width = 100
      Height = 17
      Hint = 'toggle line range'
      Caption = 'Use line range'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
      OnClick = LineRangeCheckBoxClicked
    end
  end
  object ChooseFileButton: TButton
    Left = 87
    Top = 145
    Width = 75
    Height = 21
    Hint = 'pick a file to read'
    Caption = 'Browse'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnClick = ChooseFile
  end
  object FilenameComboBox: TComboBox
    Left = 167
    Top = 145
    Width = 175
    Height = 21
    Hint = 'files used (current file at top)'
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = FilenameComboBoxClick
  end
  object ApplyButton: TButton
    Left = 137
    Top = 180
    Width = 75
    Height = 25
    Caption = 'Apply'
    TabOrder = 4
    OnClick = ApplyClick
  end
  object CloseButton: TButton
    Left = 217
    Top = 180
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 5
    OnClick = CloseClick
  end
  object Panel1: TPanel
    Left = 5
    Top = 5
    Width = 241
    Height = 40
    TabOrder = 6
    object TextNameLabel: TLabel
      Left = 135
      Top = 15
      Width = 96
      Height = 13
      Caption = 'String variable name'
    end
    object TextNameEdit: TEdit
      Left = 5
      Top = 10
      Width = 121
      Height = 21
      TabOrder = 0
    end
  end
  object ReadDialog: TOpenDialog
    Left = 35
    Top = 155
  end
end
