# Copyright (C) 2009 Joseph L. Chuma <joe.chuma@gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = reconf configure extrema.spec extrema.desktop extrema_icons.tar.bz2 ./Images ./Help ./Scripts ./doc ./src/wxForms/ExColorCtrl.h ./src/wxForms/ExSpinCtrlD.h ./src/wxForms/ExSpinCtrlI.h ./src/wxForms/ExSlider.h

# list immediate subdirectories only
SUBDIRS = src

myimagesdir = $(datadir)/extrema/Images
myimages_DATA = ./Images/*.GIF ./Images/*.bmp

myhelpdir = $(datadir)/extrema/Help
myhelp_DATA = ./Help/extrema.hhc ./Help/extrema.hhk ./Help/extrema.hhp ./Help/*.gif ./Help/extremaMain.htm

myCommandsdir = $(myhelpdir)/Commands
myCommands_DATA = ./Help/Commands/*.htm ./Help/Commands/*.png ./Help/Commands/*.gif  ./Help/Commands/*.jpg

myBasicTasksdir = $(myhelpdir)/BasicTasks
myBasicTasks_DATA = ./Help/BasicTasks/*.htm ./Help/BasicTasks/*.gif

myCharacteristicsdir = $(myhelpdir)/Characteristics
myCharacteristics_DATA = ./Help/Characteristics/*.htm ./Help/Characteristics/*.png ./Help/Characteristics/*.jpg

myFunctionsdir = $(myhelpdir)/Functions
myFunctions_DATA = ./Help/Functions/*.htm ./Help/Functions/*.png ./Help/Functions/*.gif

myIntroductiondir = $(myhelpdir)/Introduction
myIntroduction_DATA = ./Help/Introduction/*.htm ./Help/Introduction/*.png

myLicensedir = $(myhelpdir)/License
myLicense_DATA = ./Help/License/*.htm

myOperatorsdir = $(myhelpdir)/Operators
myOperators_DATA = ./Help/Operators/*.htm
