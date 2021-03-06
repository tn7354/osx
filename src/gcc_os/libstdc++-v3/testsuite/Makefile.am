## Makefile for the testsuite subdirectory of the GNU C++ Standard library.
##
## Copyright (C) 2001, 2002, 2003 Free Software Foundation, Inc.
##
## This file is part of the libstdc++ version 3 distribution.
## Process this file with automake to produce Makefile.in.

## This file is part of the GNU ISO C++ Library.  This library is free
## software; you can redistribute it and/or modify it under the
## terms of the GNU General Public License as published by the
## Free Software Foundation; either version 2, or (at your option)
## any later version.

## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.

## You should have received a copy of the GNU General Public License along
## with this library; see the file COPYING.  If not, write to the Free
## Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
## USA.

AUTOMAKE_OPTIONS =  cygnus dejagnu

DEJATOOL = libstdc++-v3

EXPECT = `if [ -f @glibcpp_builddir@/../../expect/expect ] ; then \
            echo @glibcpp_builddir@/../../expect/expect ; \
          else echo expect ; fi`

RUNTEST = `if [ -f @glibcpp_srcdir@/../dejagnu/runtest ] ; then \
	       echo @glibcpp_srcdir@/../dejagnu/runtest ; \
	    else echo runtest; fi`

AM_RUNTESTFLAGS =
RUNTESTFLAGS =

## CXX is actually a "C" compiler. These are real C++ programs.
## Do the same thing as `testsuite_flags --build-cxx`
CXX_fake = @glibcpp_CXX@ 
CXX=`echo $(CXX_fake) | sed 's/xgcc/g++/g'`
CXXLINK = \
	LD_RUN_PATH=${LD_RUN_PATH:+$LD_RUN_PATH:}${glibcpp_builddir}/src/.libs\
	$(LIBTOOL) --tag=CXX --mode=link $(CXX) \
	$(AM_CXXFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@

INCLUDES = \
	-nostdinc++ \
	@GLIBCPP_INCLUDES@ @LIBSUPCXX_INCLUDES@ @TOPLEVEL_INCLUDES@ 

## Build support library.
noinst_LIBRARIES = libv3test.a libv3test-debug.a
libv3test_a_SOURCES = testsuite_hooks.cc testsuite_allocator.cc
libv3test_debug_a_SOURCES = testsuite_hooks_dbg.cc testsuite_allocator_dbg.cc

glibcpp_srcdir=@glibcpp_srcdir@

testsuite_hooks_dbg.cc: ${glibcpp_srcdir}/testsuite/testsuite_hooks.cc
	@LN_S@ ${glibcpp_srcdir}/testsuite/testsuite_hooks.cc testsuite_hooks_dbg.cc
testsuite_allocator_dbg.cc: ${glibcpp_srcdir}/testsuite/testsuite_allocator.cc
	@LN_S@ ${glibcpp_srcdir}/testsuite/testsuite_allocator.cc testsuite_allocator_dbg.cc
testsuite_hooks_dbg.o: testsuite_hooks_dbg.cc
	$(CXXCOMPILE) -D_GLIBCPP_DEBUG -I. -c $<
testsuite_allocator_dbg.o: testsuite_allocator_dbg.cc
	$(CXXCOMPILE) -D_GLIBCPP_DEBUG -I. -c $<

## Build support utilities.
## Only build this as native, as need to find startup files and libc to link.
if GLIBCPP_BUILD_ABI_CHECK
noinst_PROGRAMS = abi_check
else
noinst_PROGRAMS = 
endif
abi_check_SOURCES = abi_check.cc

# Enable wchar_t tests if capable.
if GLIBCPP_TEST_WCHAR_T
all-local: stamp_wchar
else
all-local:
endif

stamp_wchar:
	touch testsuite_wchar_t

# By adding these files here, automake will remove them for 'make clean'
CLEANFILES = *.txt *.tst *.exe core* filebuf_* tmp ostream_* *.log *.sum \
	testsuite_files testsuite_wchar_t \
	site.exp abi_check 
