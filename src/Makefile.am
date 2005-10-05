# The following files should be in our source distribution in addition to the
# standard ones included by automake itself:
EXTRA_DIST = helloworld.wav README

# We want to build a libalut library with libtool.
lib_LTLIBRARIES = libalut.la

# libalut consists of the followin sources and internal headers:
libalut_la_SOURCES =		\
	alutBufferData.c	\
	alutCodec.c		\
	alutError.c		\
	alutInit.c		\
	alutInputStream.c	\
	alutInternal.h		\
	alutLoader.c		\
	alutUtil.c		\
	alutVersion.c		\
	alutWaveform.c

# We use sin and floor, so we might need -lm, autoconf takes care of this.
libalut_la_LIBADD = $(LIBM)

# The following libtool flag is neccessary for building DLLs on Windows.
libalut_la_LDFLAGS = -no-undefined

# Specifying the following path is needed to find <AL/alut.h>.
libalut_la_CPPFLAGS = -I$(top_srcdir)/include
