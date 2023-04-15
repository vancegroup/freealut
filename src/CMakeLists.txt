
add_definitions(-DHAVE_CONFIG_H=1)

# Without __NO_CTYPE tolower and friends are macros which introduce a GLIBC 2.3
# dependency. By defining this identifier we are currently backwards compatible
# to GLIBC 2.1.3, which is a good thing. In addition, the macros lead to code
# which triggers warnings with -Wunreachable-code.
if(HAVE_GLIBC)
	add_definitions(-D__NO_CTYPE=1)
endif()


if(HAVE_GCC_VISIBILITY)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
	add_definitions(-DHAVE_GCC_VISIBILITY)
endif()

# So our static library can be linked into shared libraries (for instance,
# gcc on amd64 needs -fPIC)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_SHARED_LIBRARY_C_FLAGS}")

set(ALUT_SOURCES
	alutBufferData.c
	alutCodec.c
	alutError.c
	alutInit.c
	alutInputStream.c
	alutLoader.c
	alutOutputStream.c
	alutUtil.c
	alutVersion.c
	alutWaveform.c)

set(ALUT_INTERNAL_HEADERS
	alutInternal.h)
source_group(InternalHeader FILES ${ALUT_INTERNAL_HEADERS})

set(ALUT_HEADERS
	../include/AL/alut.h)
source_group(APIHeader FILES ${ALUT_HEADERS})

add_library(alut ${ALUT_SOURCES} ${ALUT_INTERNAL_HEADERS} ${ALUT_HEADERS})
set_property(TARGET
	alut
	PROPERTY
	PUBLIC_HEADER
	${ALUT_HEADERS})
set_target_properties(alut
	PROPERTIES
	VERSION
	${MAJOR_VERSION}
	SOVERSION
	${MAJOR_VERSION})
target_link_libraries(alut ${OPENAL_LIBRARY})
if(UNIX)
	target_link_libraries(alut m)
endif()	

install(TARGETS alut
	LIBRARY DESTINATION lib${LIB_SUFFIX}
	ARCHIVE DESTINATION lib${LIB_SUFFIX}
	RUNTIME DESTINATION bin
	PUBLIC_HEADER DESTINATION include/AL)
