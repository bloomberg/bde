BSLROOT = ../..
BSL = $(BSLROOT)/groups/bsl
BDL = $(BSLROOT)/groups/bdl
BSLINC = $(INST)/include/bsl
BDLINC = $(INST)/include/bdl
BSLLIB = $(BSLROOT)/build/groups/bsl
BDLLIB = $(BSLROOT)/build/groups/bdl
CXX = clang++
CC = clang
DEBUG = -g3 -Wall
OPTIM = -O -flto -DBSLS_ASSERT_LEVEL_NONE 
GROWTH_SIZE = 16
STDLIB = -stdlib=libc++

INCLUDES = \
   -I$(BSL)/bsls -I$(BSL)/bslma -I$(BSL)/bslscm -I$(BSL)/bslh \
   -I$(BSL)/bsl+bslhdrs -I$(BSL)/bslstl -I$(BSL)/bslmf -I$(BSL)/bslalg \
   -I$(BDL)/bdlscm -I$(BDL)/bdlma

DEFS = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -DBSLS_IDENT_OFF

LIBS = \
   -Wl,-Bstatic \
     -L$(BDLLIB) -lbdl -L$(BSLLIB) -lbsl /usr/lib/x86_64-linux-gnu/libc++.a \
   -Wl,-Bdynamic \
   -lpthread -lrt

CFLAGS_LOCAL = $(DEBUG) $(OPTIM) $(TARG) $(DEFS) $(CFLAGS) -std=c99
CXXFLAGS_LOCAL = $(DEBUG) $(OPTIM) $(TARG) $(DEFS) $(STDLIB) 
CXXFLAGS_ALL = $(CXXFLAGS_LOCAL) -std=c++14 $(INCLUDES) $(CXXFLAGS)
LDFLAGS_ALL = $(LIBS) $(LDFLAGS)

all: bde run

clean:
	rm -f growth locality? eigen thready *.result

bde: Makefile
	( cd $(BSLROOT) && \
      export CFLAGS='$(CFLAGS_LOCAL)' && \
      CXX='$(CXX)' CXXFLAGS='$(CXXFLAGS_LOCAL) -Wno-narrowing' && \
      ../bde-tools/bin/waf configure \
         --abi-bits=64 --cpp11 --build-type release --assert-level=none && \
      ../bde-tools/bin/waf build -v; )
	touch bde

growth: growth.cc allocont.h bde
	$(CXX) -o $@ $(CXXFLAGS_ALL) -Wno-unused-variable $< $(LDFLAGS_ALL)

locality1: locality.cc allocont.h bde
	$(CXX) -DSTDALLOC -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)
locality2: locality.cc allocont.h bde
	$(CXX) -DCTMULTI -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)
locality3: locality.cc allocont.h bde
	$(CXX) -DRTMULTI -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)
locality4: locality.cc allocont.h bde
	$(CXX) -DRTMULTIMONO -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)

eigen: eigen.cc bde
	$(CXX) -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)
thready: thready.cc bde
	$(CXX) -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)

run: run-growth run-locality run-eigen run-thready

run-growth: growth
	for i in 4 6 8 10 12 14 16; do \
        echo ./growth $(GROWTH_SIZE) $$i - ; \
        ./growth $(GROWTH_SIZE) $$i - | tee growth-$(GROWTH_SIZE)-$$i ; \
    done

run-locality: locality1 locality2 locality3 locality4
	( \
      echo "***************** using default std::allocator:"; \
	  ./test-locality ./locality1; \
      echo "***************** using compile-time-bound multipool"; \
	  ./test-locality ./locality2; \
      echo "***************** using polymorphic multipool:"; \
	  ./test-locality ./locality3; \
	  echo "***************** using polymorphic multipool backed by monotonic:"; \
	  ./test-locality ./locality4; \
    ) | tee locality-result

run-eigen: eigen
	time ./eigen | tee eigen-result

run-thready: thready
	time ./thready | tee thready-result

