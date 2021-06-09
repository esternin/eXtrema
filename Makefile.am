# Copyright (C) 2010 Joseph L. Chuma <joe.chuma@gmail.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# autoreconf suggested we move m4 files to a subdirectory, need to know this in Makefile.am ES
ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = ./fonts ./Images ./Help ./Scripts ./doc ./demo .gitignore reconf

# list immediate subdirectories only
SUBDIRS = src

applicationsdir = $(datadir)/applications
dist_applications_DATA = extrema.desktop

# Note that our icons are already in icons/hicolor subdirectory, so we don't
# include it in iconsdir itself, as would be typical.
iconsdir = $(datadir)
nobase_dist_icons_DATA = \
    icons/hicolor/16x16/apps/extrema.png \
    icons/hicolor/32x32/apps/extrema.png \
    icons/hicolor/64x64/apps/extrema.png \
    icons/hicolor/512x512/apps/extrema.png \
    icons/hicolor/scalable/apps/extrema.svg

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	gtk-update-icon-cache --force --ignore-theme-index $(DESTDIR)$(iconsdir)/icons/hicolor

myimagesdir = $(datadir)/extrema/Images
myimages_DATA = $(abs_top_srcdir)/Images/*.GIF $(abs_top_srcdir)/Images/*.bmp

myhelpdir = $(datadir)/extrema/Help
myhelp_DATA = $(abs_top_srcdir)/Help/extrema.hhc $(abs_top_srcdir)/Help/extrema.hhk $(abs_top_srcdir)/Help/extrema.hhp $(abs_top_srcdir)/Help/*.gif $(abs_top_srcdir)/Help/extremaMain.htm

myCommandsdir = $(myhelpdir)/Commands
myCommands_DATA = $(abs_top_srcdir)/Help/Commands/*.htm $(abs_top_srcdir)/Help/Commands/*.png $(abs_top_srcdir)/Help/Commands/*.gif $(abs_top_srcdir)/Help/Commands/*.jpg

myBasicTasksdir = $(myhelpdir)/BasicTasks
myBasicTasks_DATA = $(abs_top_srcdir)/Help/BasicTasks/*.htm $(abs_top_srcdir)/Help/BasicTasks/*.gif $(abs_top_srcdir)/Help/BasicTasks/*.dat $(abs_top_srcdir)/Help/BasicTasks/*.pcm

myCharacteristicsdir = $(myhelpdir)/Characteristics
myCharacteristics_DATA = $(abs_top_srcdir)/Help/Characteristics/*.htm $(abs_top_srcdir)/Help/Characteristics/*.png $(abs_top_srcdir)/Help/Characteristics/*.jpg

myFunctionsdir = $(myhelpdir)/Functions
myFunctions_DATA = $(abs_top_srcdir)/Help/Functions/*.htm $(abs_top_srcdir)/Help/Functions/*.png $(abs_top_srcdir)/Help/Functions/*.gif

myIntroductiondir = $(myhelpdir)/Introduction
myIntroduction_DATA = $(abs_top_srcdir)/Help/Introduction/*.htm $(abs_top_srcdir)/Help/Introduction/*.png

myLicensedir = $(myhelpdir)/License
myLicense_DATA = $(abs_top_srcdir)/Help/License/*.htm

myOperatorsdir = $(myhelpdir)/Operators
myOperators_DATA = $(abs_top_srcdir)/Help/Operators/*.htm
