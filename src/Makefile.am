EXTRA_DIST = helloworld.wav README

lib_LTLIBRARIES = libalut.la

libalut_la_SOURCES =	\
	alutError.c	\
	alutError.h	\
	alutInit.c	\
	alutInit.h	\
	alutLoader.c	\
	alutUtil.c	\
	alutVersion.c	\
	alutWaveform.c

# We use sin and floor, so we might need -lm, autoconf takes care of this.
libalut_la_LIBADD = $(LIBM)

# The following libtool flag is neccessary for building DLLs on Windows.
libalut_la_LDFLAGS = -no-undefined

# Specifying the following path is needed to find <AL/alut.h>.
libalut_la_CPPFLAGS = -I$(top_srcdir)/include
