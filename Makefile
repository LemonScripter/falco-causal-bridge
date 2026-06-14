# DCC Falco Bridge Makefile

CXX=g++
CXXFLAGS=-O2 -Wall -std=c++17 -I./include
LDFLAGS=-lbpf -lelf -lz

all: build

build:
	@echo "Building Falco DCC Bridge (libsinsp extension)..."
	$(CXX) $(CXXFLAGS) -c src/sinsp_bridge.cpp -o sinsp_bridge.o

test-integration:
	@echo "Running Logic Verification (Python)..."
	python3 tests/verify_falco.py

clean:
	rm -f *.o

.PHONY: all build test-integration clean
