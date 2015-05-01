# You may need to set the following variables:
#
# CXX   - path to clang++  3.6
# CC    - path to clang 3.6
# AR    - path to clang 3.6 archiver; needed for -flto
# WAF   - path to waf (https://github.com/Bloomberg/bde-tools)
# OPTIM - remove -flto if LLVM gold plug in is not configured
# BSLRC - location of BDE directory to use

CXX = clang++
CC = clang
AR = llvm-ar
BSLSRC = ../..
WAF = waf
OPTIM = -O -flto -DBSLS_ASSERT_LEVEL_NONE

BSL = $(BSLSRC)/groups/bsl
BDL = $(BSLSRC)/groups/bdl
BSLINC = $(INST)/include/bsl
BDLINC = $(INST)/include/bdl
BSLLIB = $(BSLSRC)/build/groups/bsl
BDLLIB = $(BSLSRC)/build/groups/bdl
GROWTH_SIZE = 20  # use 16 or less for testing
STDLIB = -stdlib=libc++
DEBUG = -g3 -Wall

INCLUDES = \
   -I$(BSL)/bsls -I$(BSL)/bslma -I$(BSL)/bslscm -I$(BSL)/bslh \
   -I$(BSL)/bsl+bslhdrs -I$(BSL)/bslstl -I$(BSL)/bslmf -I$(BSL)/bslalg \
   -I$(BDL)/bdlscm -I$(BDL)/bdlma

DEFS = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -DBSLS_IDENT_OFF

# Change this if not using libc++:
LIBCPP = -lc++ -lc++abi

LIBS = \
   -Wl,-Bstatic \
     -L$(BDLLIB) -lbdl -L$(BSLLIB) -lbsl $(LIBCPP) \
   -Wl,-Bdynamic \
   -lpthread

CFLAGS_LOCAL = $(DEBUG) $(OPTIM) $(TARG) $(DEFS) $(CFLAGS) -std=c99
CXXFLAGS_LOCAL = $(DEBUG) $(OPTIM) $(TARG) $(DEFS) $(STDLIB)
CXXFLAGS_ALL = $(CXXFLAGS_LOCAL) -std=c++14 $(INCLUDES) $(CXXFLAGS)
LDFLAGS_ALL = $(LIBS) $(LDFLAGS)

all: run

BINARIES = growth \
           locality-AS1 locality-AS7 locality-AS9 locality-AS13 \
           zation tention

$(BINARIES): bde-tag

OUTPUT =  *-result growth-*-*

clean:
	rm -f $(BINARIES) $(OUTPUT) bde-tag

bde-tag: Makefile
	( cd $(BSLSRC) && \
      export CC='$(CC)' CFLAGS='$(CFLAGS_LOCAL)' && \
      export CXX='$(CXX)' CXXFLAGS='$(CXXFLAGS_LOCAL) -Wno-narrowing' && \
      export AR=llvm-ar && \
      $(WAF) configure \
         --abi-bits=64 --cpp11 --build-type release --assert-level=none && \
      $(WAF) build -v; )
	touch bde-tag

# section 7
growth: growth.cc allocont.h bde-tag
	$(CXX) -o $@ $(CXXFLAGS_ALL) -Wno-unused-variable $< $(LDFLAGS_ALL)
growth-orig: growth-orig.cc allocont.h bde-tag
	$(CXX) -o $@ $(CXXFLAGS_ALL) -Wno-unused-variable $< $(LDFLAGS_ALL)

# section 8
locality-AS1: locality.cc allocont.h
	$(CXX) -DSTDALLOC -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)
locality-AS7: locality.cc allocont.h
	$(CXX) -DCTMULTI -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)
locality-AS9: locality.cc allocont.h
	$(CXX) -DRTMULTI -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)
locality-AS13: locality.cc allocont.h
	$(CXX) -DRTMULTIMONO -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)

# section 9
zation: zation.cc
	$(CXX) -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)

# section 10
tention: tention.cc
	$(CXX) -o $@ $(CXXFLAGS_ALL) $< $(LDFLAGS_ALL)

run: run-growth run-locality run-zation run-tention

run-growth: growth
	@echo \"test-growth 20\" may take a full day to complete:
	./test-growth $(GROWTH_SIZE)
	cat growth-*-* >growth-result

run-growth-orig: growth-orig
	./test-growth-orig $(GROWTH_SIZE)
	cat growth-*-* >growth-orig-result

run-locality: locality-AS1 locality-AS7 locality-AS9 locality-AS13
	@( \
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
