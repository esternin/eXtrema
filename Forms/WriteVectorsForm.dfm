object WriteVecForm: TWriteVecForm
  Left = 541
  Top = 349
  Width = 369
  Height = 285
  Caption = 'Write vectors'
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
  object ChooseFileButton: TButton
    Left = 7
    Top = 151
    Width = 75
    Height = 21
    Anchors = [akLeft, akBottom]
    Caption = 'Browse'
    TabOrder = 0
    OnClick = ChooseFileClick
  end
  object FilenameComboBox: TComboBox
    Left = 87
    Top = 150
    Width = 269
    Height = 21
    Anchors = [akLeft, akBottom]
    ItemHeight = 13
    TabOrder = 1
  end
  object ApplyButton: TButton
    Left = 144
    Top = 214
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Apply'
    TabOrder = 2
    OnClick = ApplyClick
  end
  object CloseButton: TButton
    Left = 225
    Top = 214
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Close'
    TabOrder = 3
    OnClick = CloseClick
  end
  object NamesCheckListBox: TCheckListBox
    Left = 2
    Top = 0
    Width = 357
    Height = 105
    Anchors = [akLeft, akTop, akRight, akBottom]
    ItemHeight = 13
    TabOrder = 4
  end
  object AppendCheckBox: TCheckBox
    Left = 139
    Top = 180
    Width = 87
    Height = 17
    Hint = 'toggle append data to an existing file'
    Anchors = [akLeft, akBottom]
    Caption = 'Append to file'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
  end
  object UpdateButton: TButton
    Left = 63
    Top = 214
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Update'
    TabOrder = 6
    OnClick = UpdateClick
  end
  object SelectAllButton: TButton
    Left = 103
    Top = 112
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Select All'
    TabOrder = 7
    OnClick = SelectAllClick
  end
  object ClearAllButton: TButton
    Left = 183
    Top = 112
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Clear All'
    TabOrder = 8
    OnClick = ClearAllClick
  end
  object WriteDialog: TOpenDialog
    Filter = 
      'data files (*.dat)|*.dat|text files (*.txt)|*.txt|all files (*.*' +
      ')|*.*'
    Left = 60
    Top = 40
  end
end
