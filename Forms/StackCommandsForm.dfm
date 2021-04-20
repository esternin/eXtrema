object StackForm: TStackForm
  Left = 649
  Top = 532
  Width = 425
  Height = 190
  Caption = 'Record commands'
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
    Left = 352
    Top = 69
    Width = 22
    Height = 22
    Hint = 'remove all files from the list'
    Anchors = [akTop]
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
    Left = 384
    Top = 69
    Width = 22
    Height = 22
    Hint = 'remove the displayed file from the list'
    Anchors = [akTop]
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
  object FilenameComboBox: TComboBox
    Left = 95
    Top = 69
    Width = 245
    Height = 21
    Hint = 'files used (current file at top)'
    Anchors = [akTop]
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
  end
  object BrowseButton: TButton
    Left = 10
    Top = 69
    Width = 75
    Height = 21
    Hint = 'browse folders'
    Anchors = [akTop]
    Caption = 'Browse'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    OnClick = BrowseButtonClick
  end
  object CloseButton: TButton
    Left = 212
    Top = 125
    Width = 75
    Height = 25
    Hint = 'close this form'
    Anchors = [akTop]
    Caption = 'Close'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnClick = CloseButtonClick
  end
  object AppendCheckBox: TCheckBox
    Left = 164
    Top = 96
    Width = 89
    Height = 17
    Hint = 
      'if checked, commands will be appended to the end of the above fi' +
      'le'
    Anchors = [akTop]
    Caption = 'Append to file'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
  end
  object OnOffRadioGroup: TRadioGroup
    Left = 116
    Top = 9
    Width = 185
    Height = 49
    Caption = 'Command recording'
    Columns = 2
    Items.Strings = (
      'ON'
      'OFF')
    TabOrder = 4
    OnClick = ToggleOnOff
  end
  object ApplyButton: TButton
    Left = 129
    Top = 125
    Width = 75
    Height = 25
    Hint = 'click to apply the command recording settings'
    Caption = 'Apply'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
    OnClick = ApplyButtonClick
  end
  object StackDialog: TOpenDialog
    Filter = 'stack file (*.stk)|*.stk|any file (*.*)|*.*'
    Left = 16
    Top = 120
  end
end
