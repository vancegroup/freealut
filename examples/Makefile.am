bin_PROGRAMS = hello_world

hello_world_SOURCES       = hello_world.c

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/../include
LDADD = ../src/lib@LIBRARY@.la ../../linux/src/libopenal.so -lpthread
