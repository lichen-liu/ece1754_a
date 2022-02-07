# Example Makefile for ROSE users
# This makefile is provided as an example of how to use ROSE 

# Location of root directory for ROSE
ROSE_ROOT = /u/course/ece1754/rose

# Location of install directory after "make install"
ROSE_INSTALL_DIR = $(ROSE_ROOT)/ROSE_INSTALL

# Location of include directory after "make install"
ROSE_INCLUDE_DIR = $(ROSE_INSTALL_DIR)/include

# Location of bin directory after "make install"
ROSE_BIN_DIR = $(ROSE_INSTALL_DIR)/bin

# Location of library directory after "make install"
ROSE_LIB_DIR = $(ROSE_ROOT)/ROSE_INSTALL/lib

# Location of Boost include directory
#BOOST_CPPFLAGS = -pthread -I$(ROSE_ROOT)/BOOST_INSTALL/include
BOOST_CPPFLAGS = -pthread -I/usr/include/boost

CC                    = gcc
CXX                   = g++
CPPFLAGS              = 
CXXFLAGS              =  -std=c++11 -g
LDFLAGS               = 

ROSE_LIBS = $(ROSE_LIB_DIR)/librose.la

# Example suffix rule for more experienced makefile users
#.C.o:
#	g++ -c -I$(ROSE_INCLUDE_DIR) -o $@ $(@:.o=.C)


# Default make rule to use
all: MyFirstRosePass

MyFirstRosePass.lo:	MyFirstRosePass.C
	/bin/sh $(ROSE_BIN_DIR)/libtool --mode=compile $(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) -I$(ROSE_INCLUDE_DIR)/rose $(BOOST_CPPFLAGS) -c -o MyFirstRosePass.lo MyFirstRosePass.C

MyFirstRosePass: MyFirstRosePass.lo
	/bin/sh $(ROSE_BIN_DIR)/libtool --mode=link $(CXX) $(CXXFLAGS) $(LDFLAGS) -o MyFirstRosePass MyFirstRosePass.lo $(ROSE_LIBS)

# Rule used by make installcheck to verify correctness of installed libraries
check:
	./MyFirstRosePass testCode.C

