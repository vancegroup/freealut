set(ALUT_INTERNAL_HEADERS alutInternal.h)
source_group(InternalHeader FILES ${ALUT_INTERNAL_HEADERS})

set(ALUT_HEADERS ../include/AL/alut.h)
source_group(APIHeader FILES ${ALUT_HEADERS})

# STATIC or SHARED not specified so controlled by BUILD_SHARED_LIBS
add_library(
	alut
	alutBufferData.c
	alutCodec.c
	alutError.c
	alutInit.c
	alutInputStream.c
	alutLoader.c
	alutOutputStream.c
	alutUtil.c
	alutVersion.c
	alutWaveform.c
	${ALUT_INTERNAL_HEADERS}
	${ALUT_HEADERS})

set_target_properties(
	alut
	PROPERTIES PUBLIC_HEADER "${ALUT_HEADERS}"
			   VERSION ${MAJOR_VERSION}
			   SOVERSION ${MAJOR_VERSION})
target_compile_definitions(alut PRIVATE HAVE_CONFIG_H=1)
target_include_directories(
	alut
	PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
		   "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>"
		   "${OPENAL_INCLUDE_DIR}")
target_link_libraries(alut PUBLIC ${OPENAL_LIBRARY})
if(UNIX)
	target_link_libraries(alut PRIVATE m)
endif()
if(NOT BUILD_SHARED_LIBS)
	target_compile_definitions(alut PRIVATE ALUT_BUILD_STATIC)
endif()

if(HAVE_GCC_VISIBILITY)
	target_compile_options(alut PRIVATE "-fvisibility=hidden")
	target_compile_definitions(alut PRIVATE HAVE_GCC_VISIBILITY)
endif()

# Without __NO_CTYPE tolower and friends are macros which introduce a GLIBC 2.3
# dependency. By defining this identifier we are currently backwards compatible
# to GLIBC 2.1.3, which is a good thing. In addition, the macros lead to code
# which triggers warnings with -Wunreachable-code.
if(HAVE_GLIBC)
	target_compile_definitions(alut PRIVATE __NO_CTYPE=1)
endif()

install(
	TARGETS alut
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/AL)
