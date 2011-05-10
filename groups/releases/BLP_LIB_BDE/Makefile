# BDE Universal Makefile
# * This file is machine generated *

# Locations
ifndef ROOT_LOCN
  ROOT_LOCN = .
endif
ifndef BASE_SUBDIR
  BASE_SUBDIR = groups
endif
ifndef BASE_LOCN
  ifdef BASE_SUBDIR
    BASE_LOCN = $(ROOT_LOCN)/$(BASE_SUBDIR)
  else
    BASE_LOCN = $(ROOT_LOCN)/groups
  endif
endif
ifndef DEPARTMENT_SUBDIR
  DEPARTMENT_SUBDIR = departments/index
endif
ifndef DEPARTMENT_LOCN
  ifdef DEPARTMENT_SUBDIR
    DEPARTMENT_LOCN = $(ROOT_LOCN)/$(DEPARTMENT_SUBDIR)
  else
    DEPARTMENT_LOCN = $(ROOT_LOCN)/departments/index
  endif
endif
ifndef ADAPTER_SUBDIR
  ADAPTER_SUBDIR = adapters
endif
ifndef ADAPTER_LOCN
  ifdef ADAPTER_SUBDIR
    ADAPTER_LOCN = $(ROOT_LOCN)/$(ADAPTER_SUBDIR)
  else
    ADAPTER_LOCN = $(ROOT_LOCN)/adapters
  endif
endif
ifndef WRAPPER_SUBDIR
  WRAPPER_SUBDIR = wrappers
endif
ifndef WRAPPER_LOCN
  ifdef WRAPPER_SUBDIR
    WRAPPER_LOCN = $(ROOT_LOCN)/$(WRAPPER_SUBDIR)
  else
    WRAPPER_LOCN = $(ROOT_LOCN)/wrappers
  endif
endif
ifndef FUNCTION_SUBDIR
  FUNCTION_SUBDIR = functions/index
endif
ifndef FUNCTION_LOCN
  ifdef FUNCTION_SUBDIR
    FUNCTION_LOCN = $(ROOT_LOCN)/$(FUNCTION_SUBDIR)
  else
    FUNCTION_LOCN = $(ROOT_LOCN)/functions/index
  endif
endif
ifndef ENTERPRISE_SUBDIR
  ENTERPRISE_SUBDIR = enterprise
endif
ifndef ENTERPRISE_LOCN
  ifdef ENTERPRISE_SUBDIR
    ENTERPRISE_LOCN = $(ROOT_LOCN)/$(ENTERPRISE_SUBDIR)
  else
    ENTERPRISE_LOCN = $(ROOT_LOCN)/enterprise
  endif
endif
ifndef APPLICATION_SUBDIR
  APPLICATION_SUBDIR = applications
endif
ifndef APPLICATION_LOCN
  ifdef APPLICATION_SUBDIR
    APPLICATION_LOCN = $(ROOT_LOCN)/$(APPLICATION_SUBDIR)
  else
    APPLICATION_LOCN = $(ROOT_LOCN)/applications
  endif
endif
ifndef INSTALL_LIBDIR
  INSTALL_LIBDIR = $(ROOT_LOCN)/lib
endif
ifndef INSTALL_INCDIR
  INSTALL_INCDIR = $(ROOT_LOCN)/include
endif

# Build type (PRODUCTION or STANDALONE)
ifndef BUILD
  BUILD = STANDALONE
endif
ifeq ($(BUILD),PRODUCTION)
  ifndef BDEMAKE_INCLUDED_BB
    include /bbsrc/mkincludes/machindepx.lib
    include /bb/source/mkincludes/machdep
  SUN_SPECIFIC_CPP_FLAGS=
  BDEMAKE_INCLUDED_BB = 1
  endif

  COMMAND_PRE = $(COMPILE) $<
  COMMAND_POST =
  AS_COMMAND_POST =
else
  COMMAND_PRE =
  CC_COMMAND_POST = $< -c
  NC_COMMAND_POST = $<
endif

# Assign dependencies to $(OBJS) for archive stages
ifndef OBJS
  OBJS = $^
endif

# Generic targets
ifndef BDEMAKE_GENERIC_TARGETS
  BDEMAKE_GENERIC_TARGETS = 1

.PHONY:: all build install installlib installhdr uninstall uninstalllib uninstallhdr checksum verify clean distclean realclean

all: bas.all

build: bas.build

install: bas.install

installlib: bas.installlib

installhdr: bas.installhdr

uninstall: bas.uninstall

uninstalllib: bas.uninstalllib

uninstallhdr: bas.uninstallhdr

checksum: bas.checksum

verify: bas.verify

clean: bas.clean

distclean: bas.distclean

realclean: bas.realclean

endif

# Dependent package groups
ifndef BDEMAKE_INCLUDED_BAS_GROUP
  BDEMAKE_INCLUDED_BAS_GROUP = 1
  include $(BASE_LOCN)/bas/group/bas.mk
endif

ifndef RELDIR
  CWD         := $(shell (cd $(ROOT_LOCN) && pwd))
  RELDIR      := $(shell test -d $(GROUPS_LOCN)/releases && echo '$(GROUPS_LOCN)/releases/BLP_LIB_BDE/')
  ifndef RELDIR
    RELDIR = $(CWD)/
  endif
  RELFILES    := Makefile Makefile.nmk MANIFEST README README.INSTALL examples/Makefile examples/Makefile.nmk examples/*.c* README.EXAMPLES README.PLATFORMS VERSION
  ABSRELFILES := $(RELFILES:%=$(RELDIR)%)
  VERSION     := $(shell cat $(RELDIR)VERSION)
  VERRELFILES := $(RELFILES:%=$(VERSION)/%)
endif

.PHONY:: version dist srcdist bindist

version: bas.version
	@echo release $(VERSION)

bindist: $(ROOT_LOCN)/$(VERSION).$(PLATFORM).tar.gz

$(ROOT_LOCN)/$(VERSION).$(PLATFORM).tar.gz: $(ABSRELFILES) $(BAS_BINDISTFILES) $(BAS_ABSMKDISTFILES)
	(cd $(RELDIR) && rm -f $(VERSION) && ln -s . $(VERSION) && tar cvf $(CWD)/$(VERSION).$(PLATFORM).tar $(VERRELFILES) && rm -f $(VERSION))
	(cd $(BAS_LIBDIR)/.. && rm -f $(VERSION) && ln -s . $(VERSION) && tar rvf $(CWD)/$(VERSION).$(PLATFORM).tar $(VERSION)/lib/$(PLATFORM)/libbas.* && rm -f $(VERSION))
	(cd $(BAS_INCDIR)/.. && rm -f $(VERSION) && ln -s . $(VERSION) && tar rvf $(CWD)/$(VERSION).$(PLATFORM).tar $(VERSION)/include/bas/*.h && rm -f $(VERSION))
	(cd $(ROOT_LOCN) && rm -f $(VERSION) && ln -s . $(VERSION) && tar rvf $(CWD)/$(VERSION).$(PLATFORM).tar $(BAS_VERMKDISTFILES) && rm -f $(VERSION))
	gzip $(ROOT_LOCN)/$(VERSION).$(PLATFORM).tar
	@echo '** created $(PLATFORM) binary distribution'

srcdist: $(ROOT_LOCN)/$(VERSION).src.tar.gz $(ROOT_LOCN)/$(VERSION).src.zip

$(ROOT_LOCN)/$(VERSION).src.tar.gz: $(ABSRELFILES) $(BAS_ABSSRCDISTFILES)
	(cd $(RELDIR) && rm -f $(VERSION) && ln -s . $(VERSION) && tar cvf $(CWD)/$(VERSION).src.tar $(VERRELFILES) && rm -f $(VERSION))
	(cd $(ROOT_LOCN) && rm -f $(VERSION) && ln -s . $(VERSION) && tar rvf $(CWD)/$(VERSION).src.tar $(BAS_VERSRCDISTFILES) && rm -f $(VERSION))
	gzip $(ROOT_LOCN)/$(VERSION).src.tar
	@echo '** created source distribution (tar.gz)'

$(ROOT_LOCN)/$(VERSION).src.zip: $(ABSRELFILES) $(BAS_ABSSRCDISTFILES)
	(cd $(RELDIR) && rm -f $(VERSION) && ln -s . $(VERSION) && zip $(CWD)/$(VERSION).src.zip $(VERRELFILES) && rm -f $(VERSION))
	(cd $(ROOT_LOCN) && rm -f $(VERSION) && ln -s . $(VERSION) && zip -u $(CWD)/$(VERSION).src.zip $(BAS_VERSRCDISTFILES) && rm -f $(VERSION))
	@echo '** created source distribution (zip)'

dist: srcdist bindist

