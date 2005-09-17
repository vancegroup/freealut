# Build, but do not install the following test programs:
noinst_PROGRAMS =		\
	test_errorstuff		\
	test_fileloader		\
	test_memoryloader	\
	test_retrostuff		\
	test_version		\
	test_waveforms

# We need to link against our *own* libalut.
LDADD = ../src/libalut.la

# Specifying the following path is needed to find <AL/alut.h>.
AM_CPPFLAGS = -I$(top_srcdir)/include

# test_retrostuff tests deprecated functions, but we don't want to get compiler
# warnings because of that.
if WNO_DEPRECATED_DECLARATIONS
test_retrostuff_CFLAGS = -Wno-deprecated-declarations
endif
