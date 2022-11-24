<h1>Pre-compiled binaries</h2>

A few pre-compiled binaries exist. If you do not wish to compile your own, one of these may work for you "out-of-the-box". They are available from <a href="https://www.physics.brocku.ca/Labs/extrema/">https://www.physics.brocku.ca/Labs/extrema/</a>.
 
* <a href="https://www.physics.brocku.ca/Labs/extrema/Downloads/eXtrema-x86_64.AppImage">eXtrema-x86_64.AppImage</a> - (24MB) an AppImage executable for most Linux distributions, v.4.7.1/wxWidgets 3.2.1. Download, make executable (<code>chmod 755 eXtrema-x86_64.AppImage</code>) and run the AppImage file; it is completely self-contained. This has been tested under several Linux distributions, from RHEL7 to Ubuntu 20.04. You should make sure your system has GNU FreeFonts installed (<code>sudo apt-get install fonts-freefont-ttf</code>). [extrema.desktop](https://www.physics.brocku.ca/Labs/extrema/Downloads/extrema.desktop) should be placed in `/usr/share/applications` and [extrema.svg](https://www.physics.brocku.ca/Labs/extrema/Downloads/icons/hicolor/scalable/apps/extrema.svg) in `/usr/share/icons/hicolor/scalable/apps` or their equivalent locations for your distribution.

* The above AppImage also works on Intel-based Chromebooks, if Linux is enabled.
The default GTK theme that Google uses makes the buttons look a little too wide, but tweaking <code>~/.config/gtk-3.0/settings.ini</code> fixes that:
```
    [Settings]
    gtk-application-prefer-dark-theme=0
    gtk-icon-sizes="panel-menu=12,11:panel=12,11:gtk-menu=12,11:gtk-large-toolbar=12,11:gtk-small-toolbar=12,11:gtk-button=12,11"
```

* <a href="https://www.physics.brocku.ca/Labs/extrema/Downloads/eXtrema-471.zip">eXtrema-471.zip</a> - (23MB) Windows (32-bit, should work on 32- and 64-bit hardware, from XP to Windows 10) binary, wxWidgets 3.2.1. This zip file already contains all of the files you need, including the Windows executable, sample scripts and documentation; unzip this anywhere (e.g. as `C:\Programs\eXtrema-471\`), and double-click on the `extrema.exe` file. One can also make a link on the Desktop to that executable, assign it a custom [icon](https://www.physics.brocku.ca/Labs/extrema/Downloads/icons/extrema.ico) and even drag it to the taskbar. 

You should also install FreeFonts (included, from <a href="https://www.gnu.org/software/freefont/" target="_blank">https://www.gnu.org/software/freefont/</a>) by copying the font files into your system's fonts' directory (instructions vary for different Windows versions, but in general: open Control Panel, then Fonts, then drag the .ttf files into the font folder).
