bin_PROGRAMS = hello_world

hello_world_SOURCES = hello_world.c

AM_CPPFLAGS = -I$(top_srcdir)/include

# ToDo: -lpthread should be detected via autoconf magic
LDADD = ../src/lib@LIBRARY@.la -lopenal -lpthread
