PLUGIN=timer
CC = gcc
M32=0
CFLAGS = -g -O2
CPPFLAGS = 
CXX = g++
CXXFLAGS = -g -O2
LDFLAGS = 

# Allow the user to specify the install program.
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}

MKDIR_P = /bin/mkdir -p
PACKAGE = dmtcp
PACKAGE_BUGREPORT = dmtcp-forum@lists.sourceforge.net
PACKAGE_NAME = DMTCP
PACKAGE_STRING = DMTCP 2.3.1
PACKAGE_TARNAME = dmtcp
PACKAGE_URL = http://dmtcp.sourceforge.net
PACKAGE_VERSION = 2.3.1
prefix=/usr/local
exec_prefix=${prefix}
datarootdir=${prefix}/share
bindir = ${exec_prefix}/bin
docdir = ${datarootdir}/doc/${PACKAGE_TARNAME}
includedir = ${prefix}/include
infodir = ${datarootdir}/info
libdir = ${exec_prefix}/lib
pkglibdir = $(libdir)/dmtcp
top_builddir = ../../..
top_srcdir = ../../..
srcdir = .

targetdir = $(top_builddir)

ifeq ($(M32),1)
  targetdir = $(top_builddir)/lib/$(PACKAGE)/32
endif

LIBNAME=$(targetdir)/lib/${PACKAGE}/libdmtcp_${PLUGIN}.so

DMTCP_INCLUDE_PATH = $(top_srcdir)/include
JALIB_PATH = $(top_srcdir)/jalib

INCLUDES = -I$(JALIB_PATH) -I$(DMTCP_INCLUDE_PATH) -I$(srcdir)

override CFLAGS += -fPIC
override CXXFLAGS += -fPIC
ifeq ($(M32),1)
CFLAGS += -m32 -Wa,--32
CXXFLAGS += -m32 -Wa,--32
LDFLAGS += -m32 -Wl,-m32 -Wl,-melf_i386 -Wa,--32
endif

CXXCOMPILE = $(CXX) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS) -c -o $@
CXXLINK = $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@

COMMON_HEADERS = $(DMTCP_INCLUDE_PATH)/util.h \
		 $(DMTCP_INCLUDE_PATH)/dmtcp.h \
		 $(DMTCP_INCLUDE_PATH)/shareddata.h \
		 $(DMTCP_INCLUDE_PATH)/dmtcpalloc.h \
		 $(DMTCP_INCLUDE_PATH)/protectedfds.h \
		 $(DMTCP_INCLUDE_PATH)/virtualidtable.h \
		 $(DMTCP_INCLUDE_PATH)/protectedfds.h \
		 $(DMTCP_INCLUDE_PATH)/procmapsarea.h \
		 $(JALIB_PATH)/jassert.h $(JALIB_PATH)/jalloc.h \
		 $(JALIB_PATH)/jalib.h $(JALIB_PATH)/jbuffer.h \
		 $(JALIB_PATH)/jconvert.h $(JALIB_PATH)/jfilesystem.h \
		 $(JALIB_PATH)/jserialize.h $(JALIB_PATH)/jsocket.h

HEADERS = timerwrappers.h timerlist.h

OBJS = timerlist.o timerwrappers.o timer_create.o

LIB_LDFLAGS = -shared -Wl,--unresolved-symbols=ignore-all

LDADD = -ldl -lpthread -lrt

all: default

default: build


build: ${LIBNAME}

tidy:
	rm -rf dmtcp-autotest-* ckpt_*_files
	rm -f ckpt_*.dmtcp dmtcp_restart_script* core*

clean: tidy
	-rm -f *.o
	-rm -f $(LIBNAME)

distclean: clean
	rm -f Makefile

install-libs: all
	${INSTALL_PROGRAM} ${LIBNAME} $(DESTDIR)$(pkglibdir)

uninstall-libs:
	rm -f $(DESTDIR)$(pkglibdir)/$(LIBNAME)

install: install-libs

uninstall: uninstall-libs

${LIBNAME}: $(OBJS)
	@rm -f ${LIBNAME}
	$(CXXLINK) $(OBJS) $(LIB_LDFLAGS) $(LDADD)

%.o: %.cpp ${COMMON_HEADERS} ${HEADERS}
	$(CXXCOMPILE)  $<

.PHONY: default all build tidy clean distclean install uninstall
