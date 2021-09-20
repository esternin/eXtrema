#!/bin/sh

if [ -e /tmp/AppDir ]; then
  echo "  makeAppImage: /tmp/AppDir already exists, remove or rename to save it, aborting"
  exit 0
fi

mkdir /tmp/AppDir

make install DESTDIR=/tmp/AppDir
strip /tmp/AppDir/usr/local/lib/libextrema.a

if [ ! -e linuxdeploy-x86_64.AppImage ]; then
  echo "  makeAppImage: downloading linuxdeploy-x86_64.AppImage"
  wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
  chmod 755 linuxdeploy-x86_64.AppImage 
fi
./linuxdeploy-x86_64.AppImage --appdir /tmp/AppDir --executable src/extrema --desktop-file ../extrema.desktop --icon-file ../icons/hicolor/scalable/apps/extrema.svg 

mkdir -p /tmp/AppDir/usr/local/share/fonts/freefont
BASEDIR=$(dirname $(readlink -f "$0"))
install -m 644 $BASEDIR/fonts/* /tmp/AppDir/usr/local/share/fonts/freefont

cat <<'EOT' > /tmp/AppDir/AppRun
#!/bin/sh

# Absolute path to this script's location
BASEDIR=$(dirname $(readlink -f "$0"))

export EXTREMA_DIR=$BASEDIR/usr/local/share/extrema
LD_LIBRARY_PATH=$BASEDIR/usr/lib $BASEDIR/usr/local/bin/extrema
EOT

chmod 755 /tmp/AppDir/AppRun 

mkdir /tmp/AppDir/usr/local/share/metainfo
cat <<EOT > /tmp/AppDir/usr/local/share/metainfo/ca.brocku.physics.extrema.metainfo.xml
<?xml version="1.0" encoding="UTF-8"?>
<component type="desktop-application">
  <id>ca.brocku.physics.extrema</id>
  
  <name>eXtrema</name>
  <summary>Data visualization and analysis</summary>
  
  <metadata_license>FSFAP</metadata_license>
  <project_license>GPL-2.0-or-later</project_license>
  
  <recommends>
    <control>pointing</control>
    <control>keyboard</control>
    <control>touch</control>
  </recommends>
  
  <description>
    <p>
      eXtrema is a powerful visualization and data analysis tool that enables researchers to quickly distill their large, complex data sets into meaningful information. Its flexibility, sophistication, and power allow you to easily develop your own commands and create highly customized graphs.
    </p>
  </description>
  
  <launchable type="desktop-id">extrema.desktop</launchable>
  <screenshots>
    <screenshot type="default">
      <image>https://www.physics.brocku.ca/Labs/extrema/images/extrema_title_3.gif</image>
    </screenshot>
    <screenshot>
      <image>https://www.physics.brocku.ca/Labs/extrema/images/fitexample2.png</image>
    </screenshot>
    <screenshot>
      <image>https://www.physics.brocku.ca/Labs/extrema/images/contourdensity4.png</image>
    </screenshot>
  </screenshots>
</component>
EOT

if [ ! -e appimagetool-x86_64.AppImage ]; then
  echo "  makeAppImage: downloading appimagetool-x86_64.AppImage"
  wget https://github.com/AppImage/AppImageKit/releases/download/13/appimagetool-x86_64.AppImage
  chmod 755 appimagetool-x86_64.AppImage
fi
./appimagetool-x86_64.AppImage /tmp/AppDir 

echo "  makeAppImage: eXtrema-x86_64.AppImage in this directory is ready."

