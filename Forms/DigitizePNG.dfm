object DigitizePNGForm: TDigitizePNGForm
  Left = 569
  Top = 125
  BorderStyle = bsSingle
  Caption = 'Digitize PNG'
  ClientHeight = 271
  ClientWidth = 397
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
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 398
    Height = 231
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
    object XoutLabel: TLabel
      Left = 10
      Top = 15
      Width = 102
      Height = 13
      Caption = 'X output vector name'
    end
    object YoutLabel: TLabel
      Left = 10
      Top = 45
      Width = 102
      Height = 13
      Caption = 'Y output vector name'
    end
    object PointsLabel1: TLabel
      Left = 10
      Top = 75
      Width = 105
      Height = 14
      Caption = 'First point  : '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object PointsLabel2: TLabel
      Left = 10
      Top = 95
      Width = 105
      Height = 14
      Caption = 'Second point : '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object PointsLabel3: TLabel
      Left = 10
      Top = 115
      Width = 105
      Height = 14
      Caption = 'Third point  : '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object NPointsLabel: TLabel
      Left = 10
      Top = 135
      Width = 203
      Height = 14
      Caption = 'Number of digitized points : '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object XYLabel1: TLabel
      Left = 120
      Top = 75
      Width = 56
      Height = 14
      Caption = 'XYLabel1'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object XYLabel2: TLabel
      Left = 120
      Top = 95
      Width = 56
      Height = 14
      Caption = 'XYLabel2'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object XYLabel3: TLabel
      Left = 120
      Top = 115
      Width = 56
      Height = 14
      Caption = 'XYLabel3'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object NPLabel: TLabel
      Left = 215
      Top = 135
      Width = 49
      Height = 14
      Caption = 'NPLabel'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object LastPointLabel: TLabel
      Left = 10
      Top = 155
      Width = 203
      Height = 14
      Caption = 'Last point digitized       : '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object LPLabel: TLabel
      Left = 215
      Top = 155
      Width = 49
      Height = 14
      Caption = 'LPLabel'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object XoutEdit: TEdit
      Left = 150
      Top = 10
      Width = 175
      Height = 21
      Hint = 
        'enter the name of the vector which will hold the x digitized val' +
        'ues'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
    end
    object YoutEdit: TEdit
      Left = 150
      Top = 40
      Width = 175
      Height = 21
      Hint = 
        'enter the name of the vector which will hold the y digitized val' +
        'ues'
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object StartStopButton: TButton
      Left = 149
      Top = 192
      Width = 100
      Height = 25
      Caption = 'Start digitizing'
      TabOrder = 2
      OnClick = StartStopClick
    end
  end
  object CloseButton: TButton
    Left = 148
    Top = 240
    Width = 100
    Height = 25
    Caption = 'Close'
    TabOrder = 1
    OnClick = CloseClick
  end
end
