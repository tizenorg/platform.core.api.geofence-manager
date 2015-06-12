#sbs-git:slp/api/geofence-manager capi-geofence-manager 0.1.0 d1ee09a32e8bc0e9ed48ece37c641a7393c086c5
Name:       capi-geofence-manager
Summary:    A Geofence Manager library in Tizen Native API
Version:    0.2.2
Release:    1
Group:      Location/API
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz

%if "%{?tizen_profile_name}" == "tv"
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
BuildRequires:  pkgconfig(privacy-manager-client)

Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description


%prep
%setup -q


%build
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"

MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
cmake . -DCMAKE_INSTALL_PREFIX=/usr -DFULLVER=%{version} -DMAJORVER=${MAJORVER} -DLIBDIR=%{_libdir}\
%if 0%{?model_build_feature_location_geofence}
    -DENABLE_GEOFENCE=YES
%endif

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
Summary:  A Geofence Manager library in Tizen Native API (Development)
Group:    Location/API
Requires: %{name} = %{version}-%{release}

%description devel


%files devel
%{_includedir}/geofence/*.h
%{_libdir}/pkgconfig/*.pc
%{_libdir}/libcapi-geofence-manager.so

