# BDE Example Application Makefile

all	     : hello_world

#-----------------------------------------------------------------------------

TARGET	  = DBG_MT

ROOT_LOCN       = ..
INSTALL_LIBDIR  = $(ROOT_LOCN)/lib
INSTALL_INCDIR  = $(ROOT_LOCN)/include

#-----------------------------------------------------------------------------

BDEMAKE_GENERIC_TARGETS = 1
include $(ROOT_LOCN)/Makefile

CXX	     = $(BDE_$(TARGET)_CXX) $(BDE_$(TARGET)_BDE_CXXFLAGS)
CXXLINK	 = $(BDE_$(TARGET)_CXXLINK) $(BDE_$(TARGET)_BDE_LDFLAGS)
BTE_CXXLINK     = $(BTE_$(TARGET)_CXXLINK) $(BTE_$(TARGET)_BDE_LDFLAGS)
ENDLDFLAGS      = $(BDE_$(TARGET)_BDE_ENDLDFLAGS)
BDE_LINKLIB     = $(BDE_$(TARGET)_LINKLIB)
BCE_LINKLIB     = $(BCE_$(TARGET)_LINKLIB)
BTE_LINKLIB     = $(BTE_$(TARGET)_LINKLIB)

ifndef HAVE_SOURCE
HAVE_SOURCE     = $(shell test -d $(BDESCM_LOCN))
endif

ifdef $(HAVE_SOURCE)

BDE_LIBRARY     = $(BDE_INSTLIBDIR)/$(BDE_$(TARGET)_LIBRARY)
BCE_LIBRARY     = $(BDE_INSTLIBDIR)/$(BCE_$(TARGET)_LIBRARY)
BTE_LIBRARY     = $(BDE_INSTLIBDIR)/$(BTE_$(TARGET)_LIBRARY)

$(BDE_INSTINCDIR) : bde.installhdr

$(BCE_INSTINCDIR) : bce.installhdr

$(BTE_INSTINCDIR) : bte.installhdr

endif

#-----------------------------------------------------------------------------

hello_world.o   : hello_world.m.cpp $(BDE_INSTINCDIR)
	$(CXX) -o $@ -c $< -I$(BDE_INSTINCDIR) -I$(BDE_INSTINCDIR)/stlport

hello_world     : hello_world.o $(BDE_LIBRARY)
	$(CXXLINK) -o $@ $< -L$(BDE_INSTLIBDIR) -l$(BDE_LINKLIB) $(ENDLDFLAGS)

clean	   :
	rm -f hello_world hello_world.o

realclean       : clean bde.clean bce.clean bte.clean

