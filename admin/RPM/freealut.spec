#
# spec file for package alut (Version 1.0.0)
#

# norootforbuild
# neededforbuild  SDL-devel-packages alsa alsa-devel arts arts-devel audiofile esound esound-devel gcc-c++ glib2 glib2-devel gpp libgpp libogg libogg-devel libvorbis libvorbis-devel smpeg smpeg-devel x-devel-packages

BuildRequires: aaa_base acl attr bash bind-utils bison bzip2 coreutils cpio cpp cracklib cvs cyrus-sasl db devs diffutils e2fsprogs file filesystem fillup findutils flex gawk gdbm-devel glibc glibc-devel glibc-locale gpm grep groff gzip info insserv klogd less libacl libattr libgcc libnscd libselinux libstdc++ libxcrypt libzio m4 make man mktemp module-init-tools ncurses ncurses-devel net-tools netcfg openldap2-client openssl pam pam-modules patch permissions popt procinfo procps psmisc pwdutils rcs readline sed strace syslogd sysvinit tar tcpd texinfo timezone unzip util-linux vim zlib zlib-devel SDL SDL-devel aalib aalib-devel alsa alsa-devel arts arts-devel audiofile autoconf automake binutils esound esound-devel expat fontconfig fontconfig-devel gcc gcc-c++ gdbm gettext glib2 glib2-devel gnome-filesystem libogg libogg-devel libstdc++-devel libtool libvorbis libvorbis-devel perl resmgr rpm slang slang-devel xorg-x11-devel xorg-x11-libs

Name:         freealut
License:      LGPL
Group:        System/Libraries
Autoreqprov:  on
Version:      1.0.0
Release:      1
URL:          http://www.openal.org/
Summary:      Open Audio Library Utility Toolkit
Source:       freealut-%{version}.tar.gz
BuildRoot:    %{_tmppath}/%{name}-%{version}-build

%description
freealut is a highly portable Open Source implementation of ALUT, a
utility toolkit for OpenAL. ALUT makes managing of OpenAL contexts,
loading sounds in various formats and creating waveforms very easy.



Authors:
--------
    Erik Hofman <erik@ehofman.com>
    Steve Baker <sjbaker1@airmail.net>
    Sven Panne <sven.panne@aedion.de>

%package devel
Summary:      Static libraries, header files and tests for the freealut library
Requires:     freealut = %{version}
Group:        Development/Libraries/C and C++

%description devel
freealut is a highly portable Open Source implementation of ALUT, a
utility toolkit for OpenAL. ALUT makes managing of OpenAL contexts,
loading sounds in various formats and creating waveforms very easy.



Authors:
--------
    Erik Hofman <erik@ehofman.com>
    Steve Baker <sjbaker1@airmail.net>
    Sven Panne <sven.panne@aedion.de>

%debug_package
%prep
%setup -q

%build
%{?suse_update_config:%{suse_update_config -f admin/autotools}}
test -f configure || ./autogen.sh
export CFLAGS="$RPM_OPT_FLAGS"
./configure --prefix=%{_prefix}
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

# documentation
install -m 755 -d $RPM_BUILD_ROOT%{_defaultdocdir}/%{name}
install -m 644 AUTHORS COPYING ChangeLog NEWS README doc/alut.css doc/alut.html \
	       $RPM_BUILD_ROOT%{_defaultdocdir}/%{name}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc %{_defaultdocdir}/%{name}/AUTHORS
%doc %{_defaultdocdir}/%{name}/COPYING
%doc %{_defaultdocdir}/%{name}/ChangeLog
%doc %{_defaultdocdir}/%{name}/NEWS
%doc %{_defaultdocdir}/%{name}/README
%{_libdir}/libalut.so.*

%files devel
%defattr(-,root,root)
%doc %{_defaultdocdir}/%{name}/alut.html
%doc %{_defaultdocdir}/%{name}/alut.css
%{_includedir}/AL/alut.h
%{_libdir}/libalut.a
%{_libdir}/libalut.la
%{_libdir}/libalut.so

%changelog -n freealut
* Mon Oct 10 2005 - sven.panne@aedion.de
- Initial version
