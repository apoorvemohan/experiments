
LN_S=ln -s -f
MKDIR_P=/bin/mkdir -p
CC=gcc
M32=0
PACKAGE=dmtcp
PACKAGE_TARNAME=dmtcp-2.3.1
VERSION=2.3.1
prefix=/usr/local
exec_prefix=${prefix}
datarootdir=${prefix}/share
bindir=${exec_prefix}/bin
libdir=${exec_prefix}/lib
docdir=${datarootdir}/doc/${PACKAGE_TARNAME}
includedir=${prefix}/include
mandir=${datarootdir}/man
infodir=${datarootdir}/man

plugins = pid ipc alloc dl timer

default:
	for p in $(plugins); do (cd $$p && $(MAKE)); done

all: default

libs: default

$(plugins):
	cd $@ && $(MAKE)

tidy:
	rm -rf dmtcp-autotest-* ckpt_*_files
	rm -f ckpt_*.dmtcp dmtcp_restart_script* \
	  dmtcp-shared-memory.* dmtcp-test-typescript.tmp core*
	rm -rf ckpt_*

clean: tidy
	for p in $(plugins); do (cd $$p && $(MAKE) clean); done

distclean: clean
	for p in $(plugins); do (cd $$p && $(MAKE) distclean); done
	rm -f Makefile

install-libs: all
	test -e $(DESTDIR)$(libdir) || $(MKDIR_P) $(DESTDIR)$(libdir)
	for p in $(plugins); do (cd $$p && $(MAKE) install-libs); done

install: all
	test -e $(DESTDIR)$(bindir) || $(MKDIR_P) $(DESTDIR)$(bindir)
	test -e $(DESTDIR)$(libdir) || $(MKDIR_P) $(DESTDIR)$(libdir)
	test -e $(DESTDIR)$(includedir) || $(MKDIR_P) $(DESTDIR)$(includedir)
	for p in $(plugins); do (cd $$p && $(MAKE) install); done

uninstall:
	for p in $(plugins); do (cd $$p && $(MAKE) uninstall); done

uninstall-libs:
	for p in $(plugins); do (cd $$p && $(MAKE) uninstall-libs); done

install-exec-hook:
uninstall-hook:

.PHONY: default all pid ipc alloc dl timer \
	tidy clean distclean install uninstall install-exec-hook uninstall-hook
