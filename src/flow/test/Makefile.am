
AM_DEFAULT_SOURCE_EXT = .cc

check_PROGRAMS = \
ha_test

TESTS = $(check_PROGRAMS)

ha_test_CPPFLAGS = @AM_CPPFLAGS@ @CPPUTEST_CPPFLAGS@

ha_test_LDADD = ../libflow.a @CPPUTEST_LDFLAGS@

