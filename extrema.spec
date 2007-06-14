Name: extrema
Version: 4.1.6
Release: 1
License: GPL
Vendor: TRIUMF
Source0: ftp://csftp.triumf.ca/pub/CompServ/extrema/%{name}-%{version}.tar.gz
Patch0: extrema.desktop.patch
Icon: %{name}.gif
Requires: wxGTK >= 2.6.3
BuildRequires: wxGTK-devel >= 2.6.3, dos2unix
BuildRoot: /var/tmp/%{name}-root

Group: Applications/Math
Summary: Extrema is a powerful visualization and data analysis tool

%description
Extrema is a powerful visualization and data analysis tool that enables
researchers to quickly distill their large, complex data sets into meaningful 
information. Its flexibility, sophistication, and power allow you to easily 
develop your own commands and create highly customized graphs.

%prep
%setup -q
# clean up cvs directores if they exist
find . -name CVS -type d | xargs -r rm -rf

# Patch desktop file and convert to unix linefeeds
%patch0 -p0
dos2unix extrema.desktop

%build

%configure --exec-prefix=%{_prefix} --datarootdir=%{_datadir} --with-wxGTK=%{_prefix} --disable-static

make

%install

rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT

mkdir -p $RPM_BUILD_ROOT%{_datadir}/%{name}/Scripts
cp -fR $RPM_BUILD_DIR/%{name}-%{version}/Scripts/* $RPM_BUILD_ROOT%{_datadir}/%{name}/Scripts

# icons
mkdir -p $RPM_BUILD_ROOT%{_datadir}/icons/crystalsvg
tar --bzip2 -xf %{name}_icons.tar.bz2 -C $RPM_BUILD_ROOT%{_datadir}/icons/crystalsvg

mkdir -p $RPM_BUILD_ROOT%{_datadir}/pixmaps
install -m 644 Images/%{name}.gif $RPM_BUILD_ROOT%{_datadir}/pixmaps

# desktop menu entry
mkdir -p $RPM_BUILD_ROOT%{_datadir}/applications
install -m 644 %{name}.desktop $RPM_BUILD_ROOT%{_datadir}/applications

%clean
rm -rf $RPM_BUILD_ROOT

%post

# Update the desktop database
update-desktop-database /usr/share/applications

%postun

# If we are erasing then update the desktop database-file.
if [ "$1" = 0 ]; then
    update-desktop-database /usr/share/applications
fi

%files

%defattr(-, root, root, -)

%doc AUTHORS ChangeLog COPYING INSTALL NEWS README THANKS
%{_datadir}/applications/*
%attr(0755,root,root) %{_datadir}/icons/crystalsvg/*
%{_bindir}/extrema
%{_libdir}/libextrema*
%{_datadir}/%{name}/*
%{_datadir}/pixmaps/%{name}.gif

%changelog
