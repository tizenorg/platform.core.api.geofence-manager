#sbs-git:slp/api/geofence-manager capi-geofence-manager 0.1.0 d1ee09a32e8bc0e9ed48ece37c641a7393c086c5
Name:       capi-geofence-manager
Summary:    A Geofence Manager library in Tizen Native API
Version:    0.3.3
Release:    1
Group:      Location/Libraries
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

%if "%{?profile}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gmodule-2.0)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  pkgconfig(capi-appfw-package-manager)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(capi-system-info)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Provides: capi-geofence-manager-plugin-devel

%description
A Geofence Manager library in Tizen Native API

%prep
%setup -q

%build
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"

MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} -DMAJORVER=${MAJORVER} -DLIBDIR=%{_libdir} \

make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest capi-geofence-manager.manifest
%{_libdir}/libcapi-geofence-manager.so.*

%package devel
Summary: A Geofence Manager library in Tizen Native API (Development)
Group:   Location/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
A Geofence Manager library in Tizen Native API (Development)

%files devel
%{_includedir}/geofence/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-geofence-manager.so
%exclude %{_includedir}/geofence/geofence_module.h
%exclude %{_includedir}/geofence/geofence_data_type.h

%package plugin-devel
Summary:    Geofence Manager Plug-in (Development)
Group:      Location/Libraries
Requires:  %{name} = %{version}-%{release}

%description plugin-devel
This provides interfaces for Geofence Manager plugin. 

%post plugin-devel

%postun plugin-devel

%files plugin-devel
%defattr(-,root,root,-)
%{_includedir}/geofence/geofence_module.h
%{_includedir}/geofence/geofence_data_type.h
