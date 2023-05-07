VERSION = 0.1.0
COMMIT_ID = $(shell git rev-list -1 HEAD)
TESTS := $(wildcard tests/*.sh)
G_TESTS := $(wildcard tests/resource/*.sh)
SHELL := /bin/bash


build:
	@xmake project -k cmake
	@xmake -v
	@ln -sf ./build/linux/x86_64/release/rvld ld

$(TESTS): %:
	sh $*

test: build
	@xmake config -m debug
	@xmake project -k cmake
	@xmake -v
	@ln -sf ./build/linux/x86_64/debug/rvld ld
	@$(SHELL) tests/hello.sh

gtest:
	$(foreach f, $(G_TESTS), \
        $(SHELL) $(f); \
    )
	@xmake config -m debug
	@xmake project -k cmake
	@xmake -v
	@ln -sf ./build/linux/x86_64/debug/rvld ld
	xmake run test_main

clean:
	rm ./ld
	rm -rf ./tests/out
	xmake clean

.PHONY: build clean test