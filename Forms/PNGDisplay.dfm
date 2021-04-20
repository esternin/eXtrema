object PNGForm: TPNGForm
  Left = 555
  Top = 280
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'PNG'
  ClientHeight = 473
  ClientWidth = 492
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
  object PNGImage: TImage
    Left = 0
    Top = 30
    Width = 492
    Height = 418
    Cursor = crCross
    Align = alClient
    OnMouseMove = ImageMouseMove
    OnMouseUp = ImageMouseUp
  end
  object PNGToolBar: TToolBar
    Left = 0
    Top = 0
    Width = 492
    Height = 30
    ButtonHeight = 24
    Caption = 'PNGToolBar'
    TabOrder = 0
    object DigitizeSpeedButton: TSpeedButton
      Left = 0
      Top = 2
      Width = 50
      Height = 24
      Hint = 'digitize points off the png image'
      Caption = 'Digitize'
      ParentShowHint = False
      ShowHint = True
      OnClick = DigitizeClick
    end
  end
  object PNGStatusBar: TStatusBar
    Left = 0
    Top = 448
    Width = 492
    Height = 25
    Panels = <
      item
        Width = 164
      end
      item
        Width = 164
      end>
    SimplePanel = False
  end
end
