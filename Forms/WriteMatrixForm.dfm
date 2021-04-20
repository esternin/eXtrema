object WriteMatForm: TWriteMatForm
  Left = 546
  Top = 236
  Width = 323
  Height = 280
  Caption = 'Write matrix'
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
    Top = 1
    Width = 312
    Height = 125
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    TabOrder = 0
  end
  object ChooseFileButton: TButton
    Left = 5
    Top = 136
    Width = 75
    Height = 21
    Anchors = [akLeft]
    Caption = 'Browse'
    TabOrder = 1
    OnClick = ChooseFileClick
  end
  object FilenameComboBox: TComboBox
    Left = 85
    Top = 136
    Width = 224
    Height = 21
    Anchors = [akLeft, akRight]
    ItemHeight = 13
    TabOrder = 2
  end
  object AppendCheckBox: TCheckBox
    Left = 114
    Top = 175
    Width = 87
    Height = 17
    Hint = 'toggle append data to an existing file'
    Anchors = [akLeft]
    Caption = 'Append to file'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
  end
  object ApplyButton: TButton
    Left = 120
    Top = 213
    Width = 75
    Height = 25
    Anchors = [akLeft]
    Caption = 'Apply'
    TabOrder = 4
    OnClick = ApplyClick
  end
  object CloseButton: TButton
    Left = 200
    Top = 213
    Width = 75
    Height = 25
    Anchors = [akLeft]
    Caption = 'Close'
    TabOrder = 5
    OnClick = CloseClick
  end
  object UpdateButton: TButton
    Left = 40
    Top = 214
    Width = 75
    Height = 25
    Anchors = [akLeft]
    Caption = 'Update'
    TabOrder = 6
    OnClick = UpdateClick
  end
  object WriteDialog: TOpenDialog
    Left = 30
    Top = 35
  end
end
