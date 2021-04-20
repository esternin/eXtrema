object WriteScalForm: TWriteScalForm
  Left = 560
  Top = 229
  Width = 319
  Height = 297
  Caption = 'Write scalars'
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
  object NamesCheckListBox: TCheckListBox
    Left = 1
    Top = 0
    Width = 309
    Height = 100
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    TabOrder = 0
  end
  object ChooseFileButton: TButton
    Left = 5
    Top = 125
    Width = 75
    Height = 21
    Anchors = [akLeft]
    Caption = 'Browse'
    TabOrder = 1
    OnClick = ChooseFileClick
  end
  object FilenameComboBox: TComboBox
    Left = 85
    Top = 125
    Width = 220
    Height = 21
    Anchors = [akLeft, akRight]
    ItemHeight = 13
    TabOrder = 2
  end
  object ApplyButton: TButton
    Left = 118
    Top = 225
    Width = 75
    Height = 25
    Anchors = [akLeft]
    Caption = 'Apply'
    TabOrder = 3
    OnClick = ApplyClick
  end
  object CloseButton: TButton
    Left = 198
    Top = 225
    Width = 75
    Height = 25
    Anchors = [akLeft]
    Caption = 'Close'
    TabOrder = 4
    OnClick = CloseClick
  end
  object AppendCheckBox: TCheckBox
    Left = 112
    Top = 170
    Width = 87
    Height = 17
    Hint = 'toggle append data to an existing file'
    Anchors = [akLeft]
    Caption = 'Append to file'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
  end
  object UpdateButton: TButton
    Left = 38
    Top = 225
    Width = 75
    Height = 25
    Anchors = [akLeft]
    Caption = 'Update'
    TabOrder = 6
    OnClick = UpdateClick
  end
  object WriteDialog: TOpenDialog
    Filter = 
      'data files (*.dat)|*.dat|text files (*.txt)|*.txt|all files (*.*' +
      ')|*.*'
    Left = 45
    Top = 35
  end
end
