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

libalut_la_LIBADD  = $(LIBM)

AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)
