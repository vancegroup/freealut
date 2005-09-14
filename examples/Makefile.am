# Build, but do not install the following test program:
noinst_PROGRAMS = hello_world

# We need to link against our *own* libalut.
LDADD = ../src/libalut.la

# Specifying the following path is needed to find <AL/alut.h>.
AM_CPPFLAGS = -I$(top_srcdir)/include
