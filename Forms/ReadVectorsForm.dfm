object ReadVecForm: TReadVecForm
  Left = 691
  Top = 387
  Width = 390
  Height = 515
  Anchors = []
  Caption = 'Read vectors'
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
  object EmptyListSpeedButton: TSpeedButton
    Left = 320
    Top = 13
    Width = 22
    Height = 22
    Hint = 'remove all files from the list'
    Anchors = [akTop, akRight]
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      04000000000000010000120B0000120B00001000000000000000000000000000
      800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333000000000
      3333333777777777F3333330F777777033333337F3F3F3F7F3333330F0808070
      33333337F7F7F7F7F3333330F080707033333337F7F7F7F7F3333330F0808070
      33333337F7F7F7F7F3333330F080707033333337F7F7F7F7F3333330F0808070
      333333F7F7F7F7F7F3F33030F080707030333737F7F7F7F7F7333300F0808070
      03333377F7F7F7F773333330F080707033333337F7F7F7F7F333333070707070
      33333337F7F7F7F7FF3333000000000003333377777777777F33330F88877777
      0333337FFFFFFFFF7F3333000000000003333377777777777333333330777033
      3333333337FFF7F3333333333000003333333333377777333333}
    NumGlyphs = 2
    ParentShowHint = False
    ShowHint = True
    OnClick = ClearFileList
  end
  object RemoveFileSpeedButton: TSpeedButton
    Left = 350
    Top = 13
    Width = 22
    Height = 22
    Hint = 'remove the displayed file from the list'
    Anchors = [akTop, akRight]
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      04000000000000010000130B0000130B00001000000000000000000000000000
      800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      333333333333333333FF33333333333330003333333333333777333333333333
      300033FFFFFF3333377739999993333333333777777F3333333F399999933333
      3300377777733333337733333333333333003333333333333377333333333333
      3333333333333333333F333333333333330033333F33333333773333C3333333
      330033337F3333333377333CC3333333333333F77FFFFFFF3FF33CCCCCCCCCC3
      993337777777777F77F33CCCCCCCCCC399333777777777737733333CC3333333
      333333377F33333333FF3333C333333330003333733333333777333333333333
      3000333333333333377733333333333333333333333333333333}
    NumGlyphs = 2
    ParentShowHint = False
    ShowHint = True
    OnClick = RemoveFilename
  end
  object ChooseFileButton: TButton
    Left = 7
    Top = 13
    Width = 75
    Height = 21
    Hint = 'pick a file to read'
    Caption = 'Browse'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnClick = ChooseFile
  end
  object LineRangePanel: TPanel
    Left = 200
    Top = 228
    Width = 176
    Height = 121
    TabOrder = 1
    object LineRange1Label: TLabel
      Left = 22
      Top = 32
      Width = 74
      Height = 13
      Caption = 'First line to read'
      Enabled = False
    end
    object LineRange2Label: TLabel
      Left = 22
      Top = 60
      Width = 75
      Height = 13
      Caption = 'Last line to read'
      Enabled = False
    end
    object LineRange3Label: TLabel
      Left = 52
      Top = 85
      Width = 47
      Height = 13
      Caption = 'Increment'
      Enabled = False
    end
    object LineRange1Edit: TEdit
      Left = 103
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
      Left = 103
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
      Left = 103
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
  object NameColumnStringGrid: TStringGrid
    Left = 54
    Top = 45
    Width = 273
    Height = 162
    Hint = 'vector names and column numbers'
    ColCount = 2
    DefaultColWidth = 50
    DefaultRowHeight = 15
    FixedCols = 0
    RowCount = 50
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goAlwaysShowEditor]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnSetEditText = NameColumnSetEditText
    ColWidths = (
      201
      50)
  end
  object OptionsPanel: TPanel
    Left = 5
    Top = 226
    Width = 191
    Height = 195
    TabOrder = 3
    object Shape1: TShape
      Left = 5
      Top = 155
      Width = 181
      Height = 2
    end
    object Shape2: TShape
      Left = 5
      Top = 100
      Width = 181
      Height = 2
    end
    object Shape3: TShape
      Left = 5
      Top = 48
      Width = 181
      Height = 2
    end
    object MakeNewCheckBox: TCheckBox
      Left = 5
      Top = 55
      Width = 124
      Height = 17
      Hint = 'toggle allowing for new vectors longer than existing vectors'
      Caption = 'Create new vectors'
      Checked = True
      ParentShowHint = False
      ShowHint = True
      State = cbChecked
      TabOrder = 0
      OnClick = MakeNewClick
    end
    object AppendCheckBox: TCheckBox
      Left = 5
      Top = 75
      Width = 97
      Height = 17
      Hint = 'toggle append data to end of existing vectors'
      Caption = 'Append data'
      Enabled = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object DisplayMessagesCheckBox: TCheckBox
      Left = 5
      Top = 165
      Width = 105
      Height = 17
      Hint = 'toggle warning/informational messages'
      Caption = 'Display messages'
      Checked = True
      ParentShowHint = False
      ShowHint = True
      State = cbChecked
      TabOrder = 2
    end
    object ErrorStopCheckBox: TCheckBox
      Left = 5
      Top = 110
      Width = 100
      Height = 17
      Hint = 'toggle skip over data read errors'
      Caption = 'Stop on Error'
      Checked = True
      ParentShowHint = False
      ShowHint = True
      State = cbChecked
      TabOrder = 3
      OnClick = StopOnErrorsClick
    end
    object ErrorFillCheckBox: TCheckBox
      Left = 5
      Top = 130
      Width = 97
      Height = 17
      Hint = 'toggle fill data read errors'
      Caption = 'Error fill'
      Enabled = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 4
      OnClick = ErrorFillClick
    end
    object ErrorFillEdit: TEdit
      Left = 110
      Top = 127
      Width = 75
      Height = 21
      Hint = 'value used for error fill'
      Enabled = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 5
      Text = '0'
    end
    object CloseFirstCheckBox: TCheckBox
      Left = 5
      Top = 5
      Width = 145
      Height = 17
      Caption = 'Close file before reading'
      Checked = True
      State = cbChecked
      TabOrder = 6
    end
    object CloseAfterCheckBox: TCheckBox
      Left = 5
      Top = 25
      Width = 135
      Height = 17
      Caption = 'Close file after reading'
      Checked = True
      State = cbChecked
      TabOrder = 7
    end
  end
  object ApplyButton: TButton
    Left = 113
    Top = 448
    Width = 75
    Height = 25
    Caption = 'Apply'
    TabOrder = 4
    OnClick = ApplyClick
  end
  object CloseButton: TButton
    Left = 193
    Top = 448
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 5
    OnClick = CloseClick
  end
  object FilenameComboBox: TComboBox
    Left = 93
    Top = 13
    Width = 220
    Height = 21
    Hint = 'files used (current file at top)'
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 6
    OnKeyDown = DeleteFilename
    OnKeyPress = ReturnKeyPress
  end
  object FormatPanel: TPanel
    Left = 200
    Top = 355
    Width = 176
    Height = 65
    TabOrder = 7
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
      OnClick = FormatCheckBoxClicked
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
  object ReadDialog: TOpenDialog
    Filter = 
      'data files (*.dat)|*.dat|text files (*.txt)|*.txt|all files (*.*' +
      ')|*.*'
    Left = 10
    Top = 61
  end
end
