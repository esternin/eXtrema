object GraphicsSubwindowForm: TGraphicsSubwindowForm
  Left = 593
  Top = 248
  BorderStyle = bsSingle
  Caption = 'Graphics sub-windows'
  ClientHeight = 416
  ClientWidth = 406
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
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 406
    Height = 371
    ActivePage = ChoosePredefinedWindowTabSheet
    TabOrder = 0
    object ChoosePredefinedWindowTabSheet: TTabSheet
      Caption = 'Choose a pre-defined window'
      object InheritLabel: TLabel
        Left = 71
        Top = 270
        Width = 200
        Height = 13
        Caption = 'Inherit characteristics from window number'
      end
      object WindowCoordStringGrid: TStringGrid
        Left = 25
        Top = 10
        Width = 348
        Height = 230
        Hint = 'click on a row to select a sub-window'
        Anchors = [akLeft, akTop, akRight]
        FixedColor = cl3DLight
        RowCount = 16
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goDrawFocusSelected, goRowSelect]
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        OnMouseUp = StringGridMouseUp
      end
      object InheritEdit: TEdit
        Left = 281
        Top = 265
        Width = 45
        Height = 21
        TabOrder = 1
      end
      object ApplyButton: TButton
        Left = 162
        Top = 305
        Width = 75
        Height = 25
        Caption = 'Apply'
        TabOrder = 2
        OnClick = ApplyClick
      end
    end
    object DefineOneWindowTabSheet: TTabSheet
      Caption = 'Define a new window'
      ImageIndex = 2
      object DOWPanel: TPanel
        Left = 5
        Top = 10
        Width = 391
        Height = 176
        TabOrder = 0
        object WindowNumberLabel: TLabel
          Left = 20
          Top = 19
          Width = 77
          Height = 13
          Caption = 'Window number'
        end
        object xloLabel: TLabel
          Left = 190
          Top = 52
          Width = 21
          Height = 13
          Caption = '%xlo'
        end
        object yloLabel: TLabel
          Left = 295
          Top = 52
          Width = 21
          Height = 13
          Caption = '%ylo'
        end
        object xupLabel: TLabel
          Left = 190
          Top = 91
          Width = 25
          Height = 13
          Caption = '%xup'
        end
        object yupLabel: TLabel
          Left = 295
          Top = 91
          Width = 25
          Height = 13
          Caption = '%yup'
        end
        object DOWInheritLabel: TLabel
          Left = 15
          Top = 140
          Width = 200
          Height = 13
          Caption = 'Inherit characteristics from window number'
        end
        object WindowNumberEdit: TEdit
          Left = 105
          Top = 15
          Width = 45
          Height = 21
          Hint = 'enter window number'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 0
          OnExit = WNEditExit
        end
        object InteractiveCheckBox: TCheckBox
          Left = 19
          Top = 70
          Width = 157
          Height = 17
          Hint = 'if checked, use graphics cursor to choose window'
          Caption = 'Define window interactively '
          ParentShowHint = False
          ShowHint = True
          TabOrder = 5
          OnClick = InteractiveCheckBoxClicked
        end
        object xloEdit: TEdit
          Left = 220
          Top = 50
          Width = 45
          Height = 21
          Hint = 'enter x-coordinate of lower left corner'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          OnExit = xloEditExit
        end
        object yloEdit: TEdit
          Left = 325
          Top = 50
          Width = 45
          Height = 21
          Hint = 'enter y-coordinate of lower left corner'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 2
          OnExit = yloEditExit
        end
        object xupEdit: TEdit
          Left = 220
          Top = 85
          Width = 45
          Height = 21
          Hint = 'x-coordinate of upper right corner'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 3
          OnExit = xupEditExit
        end
        object yupEdit: TEdit
          Left = 325
          Top = 85
          Width = 45
          Height = 21
          Hint = 'enter y-coordinate of upper right corner'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 4
          OnExit = yupEditExit
        end
        object DOWInheritEdit: TEdit
          Left = 225
          Top = 135
          Width = 45
          Height = 21
          TabOrder = 6
        end
      end
      object DOWApplyButton: TButton
        Left = 157
        Top = 195
        Width = 75
        Height = 25
        Caption = 'Apply'
        TabOrder = 1
        OnClick = DOWApplyClick
      end
    end
    object DefineMultipleWindowsTabSheet: TTabSheet
      Caption = 'Define multiple windows'
      ImageIndex = 2
      object Panel1: TPanel
        Left = 5
        Top = 10
        Width = 386
        Height = 126
        TabOrder = 0
        object NWHLabel: TLabel
          Left = 15
          Top = 20
          Width = 148
          Height = 13
          Caption = 'Number of windows horizontally'
        end
        object NWVLabel: TLabel
          Left = 15
          Top = 55
          Width = 137
          Height = 13
          Caption = 'Number of windows vertically'
        end
        object TilingStartLabel: TLabel
          Left = 15
          Top = 90
          Width = 113
          Height = 13
          Caption = 'Starting window number'
        end
        object TilingStartEdit: TEdit
          Left = 175
          Top = 85
          Width = 45
          Height = 21
          Hint = 'enter starting window number'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 2
          OnExit = TilingStartEditExit
        end
        object NWHEdit: TEdit
          Left = 175
          Top = 15
          Width = 45
          Height = 21
          TabOrder = 0
          OnExit = NWHEditExit
        end
        object NWVEdit: TEdit
          Left = 175
          Top = 50
          Width = 45
          Height = 21
          TabOrder = 1
          OnExit = NWVEditExit
        end
      end
      object DMWApplyButton: TButton
        Left = 161
        Top = 150
        Width = 75
        Height = 25
        Caption = 'Apply'
        TabOrder = 1
        OnClick = DMWApplyClick
      end
    end
  end
  object CloseButton: TButton
    Left = 165
    Top = 385
    Width = 75
    Height = 25
    Caption = 'Close'
    TabOrder = 1
    OnClick = CloseClick
  end
end
