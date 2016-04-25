Name:       capi-appfw-app-manager
Summary:    Application Manager API
Version:    0.2.4
Release:    1
Group:      Application Framework/API
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(aul)
BuildRequires:  pkgconfig(pkgmgr)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(cynara-client)
BuildRequires:  pkgconfig(pkgmgr)
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
The Application Manager API provides functions to get information about running applications.

%package devel
Summary:  Application Manager API (Development)
Group:    Application Framework/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
The Application Manager API provides functions to get information about running applications. (DEV)


%prep
%setup -q


%build
MAJORVER=`echo %{version} | awk 'BEGIN {FS="."}{print $1}'`
%cmake . -DFULLVER=%{version} -DMAJORVER=${MAJORVER}
%__make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%{_libdir}/libcapi-appfw-app-manager.so.*
%{_bindir}/appmgr_tool
%manifest %{name}.manifest
%license LICENSE

%files devel
%{_includedir}/appfw/*.h
%{_libdir}/libcapi-appfw-app-manager.so
%{_libdir}/pkgconfig/*.pc


