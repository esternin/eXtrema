object ExecForm: TExecForm
  Left = 589
  Top = 246
  Width = 500
  Height = 148
  BorderIcons = []
  Caption = 'execute a script'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Visible = True
  OnClose = FormClose
  OnCloseQuery = FormClose2
  PixelsPerInch = 96
  TextHeight = 13
  object EnterLabel: TLabel
    Left = 10
    Top = 45
    Width = 59
    Height = 13
    Caption = 'Parameter(s)'
  end
  object EmptyListSpeedButton: TSpeedButton
    Left = 433
    Top = 8
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
    Left = 463
    Top = 8
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
  object ParameterEdit: TEdit
    Left = 87
    Top = 42
    Width = 338
    Height = 21
    Hint = 'enter any parameters expected by the script'
    Anchors = [akLeft, akTop, akRight]
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
  end
  object ChooseFileButton: TButton
    Left = 5
    Top = 8
    Width = 75
    Height = 21
    Hint = 'choose a script file'
    Caption = 'Browse'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    OnClick = ChooseFile
  end
  object FilenameComboBox: TComboBox
    Left = 87
    Top = 8
    Width = 338
    Height = 21
    Hint = 'files used (current file at top)'
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = True
    TabOrder = 2
    OnKeyDown = DeleteFilename
    OnKeyPress = ReturnKeyPress
  end
  object ApplyButton: TButton
    Left = 145
    Top = 78
    Width = 80
    Height = 25
    Hint = 'execute the script displayed above'
    Caption = 'Apply'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    OnClick = ApplyClick
  end
  object CloseButton: TButton
    Left = 230
    Top = 78
    Width = 80
    Height = 25
    Hint = 'close this form'
    Caption = 'Close'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 4
    OnClick = CloseClick
  end
  object ExecuteDialog: TOpenDialog
    Filter = 'script file|*.pcm|stack file|*.stk|any file|*.*'
    Left = 443
    Top = 80
  end
end
