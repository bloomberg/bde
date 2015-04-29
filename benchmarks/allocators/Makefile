# The following variables likely need to be configured for your machine.
# CXX   = path to clang++  3.6
# CC    = path to clang 3.6
# WAF   = path to waf (https://github.com/Bloomberg/bde-tools)
# OPTIM = remove -flto if LLVM gold plug in is not configured

CXX = clang++
CC = clang
WAF = waf
OPTIM = -O -flto -DBSLS_ASSERT_LEVEL_NONE 


BSLROOT = ../..
BSL = $(BSLROOT)/groups/bsl
BDL = $(BSLROOT)/groups/bdl
BSLINC = $(INST)/include/bsl
BDLINC = $(INST)/include/bdl
BSLLIB = $(BSLROOT)/build/groups/bsl
BDLLIB = $(BSLROOT)/build/groups/bdl
GROWTH_SIZE = 16
STDLIB = -stdlib=libc++
DEBUG = -g3 -Wall


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
	rm -f growth locality-AS? locality-AS1? zation tention *-result growth-*-*

bde: Makefile
	( cd $(BSLROOT) && \
      export CC='$(CC)' CFLAGS='$(CFLAGS_LOCAL)' && \
      export CXX='$(CXX)' CXXFLAGS='$(CXXFLAGS_LOCAL) -Wno-narrowing' && \
      $(WAF) configure \
         --abi-bits=64 --cpp11 --build-type release --assert-level=none && \
      $(WAF) build -v; )
	touch bde

# section 7
growth: growth.cc allocont.h bde
	$(CXX) -o $@ $(CXXFLAGS_ALL) -Wno-unused-variable $< $(LDFLAGS_ALL)

# section 8
locality-AS1: locality.cc allocont.h bde
	$(CXX) -DSTDALLOC -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)
locality-AS7: locality.cc allocont.h bde
	$(CXX) -DCTMULTI -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)
locality-AS9: locality.cc allocont.h bde
	$(CXX) -DRTMULTI -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)
locality-AS13: locality.cc allocont.h bde
	$(CXX) -DRTMULTIMONO -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)

# section 9
zation: zation.cc bde
	$(CXX) -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)

# section 10
tention: tention.cc bde
	$(CXX) -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)

run: run-growth run-locality run-zation run-tention

run-growth: growth
	./test-growth 20
	cat growth-*-* >growth-result

run-locality: locality-AS1 locality-AS7 locality-AS9 locality-AS13
	( \
      echo "********** using AS1 default std::allocator:"; \
	  ./test-locality ./locality-AS1 2>&1; \
      echo "********** using AS7 compile-time-bound multipool"; \
	  ./test-locality ./locality-AS7 2>&1; \
      echo "********** using AS9 polymorphic multipool:"; \
	  ./test-locality ./locality-AS9 2>&1; \
	  echo "********** using AS13 polymorphic multipool backed by monotonic:"; \
	  ./test-locality ./locality-AS13 2>&1; \
    ) | tee locality-result

run-zation: zation
	time ./test-zation | tee zation-result

run-tention: tention
	time ./test-tention | tee tention-result
