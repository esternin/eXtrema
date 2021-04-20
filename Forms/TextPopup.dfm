object TextPopupForm: TTextPopupForm
  Left = 725
  Top = 260
  Width = 255
  Height = 275
  Caption = 'Text Popup'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = OnCloseForm
  OnCloseQuery = OnCloseQueryForm
  PixelsPerInch = 96
  TextHeight = 13
  object StringLabel: TLabel
    Left = 24
    Top = 8
    Width = 27
    Height = 13
    Caption = 'String'
  end
  object AngleLabel: TLabel
    Left = 153
    Top = 60
    Width = 74
    Height = 13
    Caption = 'Angle (degrees)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object ColorLabel: TLabel
    Left = 21
    Top = 59
    Width = 24
    Height = 13
    Caption = 'Color'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object ColorShape: TShape
    Left = 22
    Top = 71
    Width = 25
    Height = 33
    Hint = 'click to set the text color'
    ParentShowHint = False
    Shape = stRoundSquare
    ShowHint = True
    OnMouseDown = OnColorMouseDown
  end
  object HeightLabel: TLabel
    Left = 65
    Top = 60
    Width = 48
    Height = 13
    Caption = 'Height (%)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object FontLabel: TLabel
    Left = 21
    Top = 112
    Width = 21
    Height = 13
    Caption = 'Font'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object XLocLabel: TLabel
    Left = 38
    Top = 156
    Width = 64
    Height = 13
    Caption = 'X-location (%)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object YLocLabel: TLabel
    Left = 137
    Top = 156
    Width = 64
    Height = 13
    Caption = 'Y-location (%)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object StringEdit: TEdit
    Left = 24
    Top = 26
    Width = 200
    Height = 21
    Hint = 'enter the new text string (hit Enter key to accept it)'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    OnKeyPress = OnStringEditKeyPress
  end
  object AngleEdit: TEdit
    Left = 153
    Top = 76
    Width = 57
    Height = 21
    Hint = 'set the text angle (from 0 to 360 degrees)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 1
    Text = '0'
    OnChange = OnAngleChange
  end
  object AngleUpDown: TUpDown
    Left = 210
    Top = 76
    Width = 16
    Height = 21
    Associate = AngleEdit
    Min = 0
    Max = 360
    Position = 0
    TabOrder = 2
    Wrap = False
    OnClick = OnAngleClick
  end
  object HeightEdit: TEdit
    Left = 65
    Top = 76
    Width = 57
    Height = 21
    Hint = 'set the text height (as a percentage of the window)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 3
    Text = '0'
    OnChange = OnHeightChange
  end
  object HeightUpDown: TUpDown
    Left = 122
    Top = 76
    Width = 17
    Height = 21
    Min = 0
    Max = 1000
    Position = 0
    TabOrder = 4
    Wrap = False
    OnClick = OnHeightClick
  end
  object FontComboBox: TComboBox
    Left = 18
    Top = 124
    Width = 212
    Height = 21
    Hint = 'choose the text font'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ItemHeight = 13
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 5
    OnChange = OnFontChange
  end
  object CloseButton: TButton
    Left = 87
    Top = 208
    Width = 75
    Height = 25
    Hint = 'close this form'
    Caption = 'Close'
    ParentShowHint = False
    ShowHint = True
    TabOrder = 6
    OnClick = OnCloseClick
  end
  object XLocEdit: TEdit
    Left = 38
    Top = 172
    Width = 57
    Height = 21
    Hint = 'set the text x-location (as a percentage of the window)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 7
    Text = '0'
    OnChange = OnXLocChange
  end
  object XLocUpDown: TUpDown
    Left = 95
    Top = 172
    Width = 17
    Height = 21
    Min = 0
    Max = 1000
    Position = 0
    TabOrder = 8
    Wrap = False
    OnClick = OnXLocClick
  end
  object YLocEdit: TEdit
    Left = 137
    Top = 172
    Width = 57
    Height = 21
    Hint = 'set the text y-location (as a percentage of the window)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    ParentShowHint = False
    ShowHint = True
    TabOrder = 9
    Text = '0'
    OnChange = OnYLocChange
  end
  object YLocUpDown: TUpDown
    Left = 194
    Top = 172
    Width = 17
    Height = 21
    Min = 0
    Max = 1000
    Position = 0
    TabOrder = 10
    Wrap = False
    OnClick = OnYLocClick
  end
end
