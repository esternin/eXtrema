object DigitizeInfoForm: TDigitizeInfoForm
  Left = 530
  Top = 123
  BorderIcons = []
  BorderStyle = bsSingle
  Caption = 'Digitize Information'
  ClientHeight = 166
  ClientWidth = 492
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  PixelsPerInch = 96
  TextHeight = 13
  object InstructionLabel1: TLabel
    Left = 8
    Top = 8
    Width = 100
    Height = 16
    Caption = 'Enter instructions'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object InstructionLabel2: TLabel
    Left = 8
    Top = 32
    Width = 100
    Height = 16
    Caption = 'Enter instructions'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object InstructionLabel3: TLabel
    Left = 8
    Top = 56
    Width = 100
    Height = 16
    Caption = 'Enter instructions'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object AbortButton: TButton
    Left = 209
    Top = 135
    Width = 75
    Height = 25
    Caption = 'Abort'
    TabOrder = 0
    OnClick = AbortClick
  end
end
