NAME    = rvalue
WAF     = LD_RUN_PATH=/opt/swt/lib64 waf
GCCPATH = /opt/swt/install/gcc-4.9.2
WAF11   = LD_RUN_PATH=$(GCCPATH)/lib64 PATH=$(GCCPATH)/bin:$(PATH) waf
RUNTEST = bde_runtest.py


default: check

.PHONY: build
build:
	$(WAF) build

.PHONY: install
install:
	sudo $(MAKE) do-install

.PHONY: do-install
do-install:
	$(WAF) install
	sed -e '/^#/d' < groups/bsl/bsl+stdhdrs/package/bsl+stdhdrs.pub | \
         (cd /usr/local/include/bsl; xargs $(RM))

.PHONY: check
check:
	$(WAF) build --test build --targets=bslmf_$(NAME).t
	$(RUNTEST) build/groups/bsl/bslmf/bslmf_$(NAME).t

.PHONY: check-cxx11
check-cxx11 :
	$(WAF11) build --test build --targets=bslmf_$(NAME).t
	$(RUNTEST) build/groups/bsl/bslmf/bslmf_$(NAME).t

.PHONY: verify
verify:
	bde_verify groups/bsl/bslmf/bslmf_$(NAME).cpp
	bde_verify groups/bsl/bslmf/bslmf_$(NAME).t.cpp

.PHONY: test
test:
	$(WAF) build --test run

.PHONY: configure-cxx11
configure-cxx11:
	$(WAF11) configure --cpp11 --build-type=release --assert-level=none 

.PHONY: configure
configure:
	$(WAF) configure --build-type=release --assert-level=none 

.PHONY: clean
clean:
	$(WAF) clean
	$(RM) mkerr olderr *~
