# Test executables

#Verbose
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_test REQUIRED glib-2.0)
FOREACH(flag ${pkgs_test_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -Wl,-zdefs" )
SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} -fvisibility=hidden")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_C_FLAGS_RELEASE "-O2")

ADD_EXECUTABLE(appmgr_tool main.c)
TARGET_LINK_LIBRARIES(appmgr_tool capi-appfw-app-manager ${pkgs_test_LDFLAGS})
INSTALL(TARGETS appmgr_tool DESTINATION bin)

