# -----------------------------------------------------------------
# Version: 2.0
# Date: "Nov, 2007" 
# -----------------------------------------------------------------
# Programmer: Mukesh Kumar (muk139@psu.edu)@ PSU
# -----------------------------------------------------------------
# Makefile for PIHM 
#
# cvode/pihm/Makefile.  
# -----------------------------------------------------------------

SHELL = /bin/sh

srcdir       = .
builddir     = .
top_builddir = ../../
top_builddir = ../../
prefix       = /usr/local
exec_prefix  = ${prefix}
includedir   = ${prefix}/include
libdir       = ${exec_prefix}/lib



CPP      = cc -E
CPPFLAGS = 
CC       = gcc
CFLAGS   = -Wall -g -O0 -fbounds-check -fno-delete-null-pointer-checks -fexceptions -fnon-call-exceptions -fstack-protector-all
# -fstack-check
#-Wl,--stack=268435457,--heap=268435456 
#-DSUNDIALS_240
#-fstack-protector-all
# -fstack-check
#
#-fmudflap
#67108864
#16777216
# -mtune=native -fomit-frame-pointer -mfpmath=sse -msse3
#CFLAGS   = -O3 -mtune=native -fomit-frame-pointer -mfpmath=sse -msse3
LDFLAGS  = 
LIBS     = -lm
SRC    = pihm.c f.c read_alloc.c initialize.c is_sm_et.c update.c print.c
OBJS     = $(SRC:%.o=%.c)

COMPILER_PREFIX = 
LINKER_PREFIX   = 

SUNDIALS_INC_DIR = $(includedir)
SUNDIALS_LIB_DIR = $(libdir)
SUNDIALS_LIBS    = -static -lsundials_cvode -static -lsundials_nvecserial
#SUNDIALS_LIBS    = -lsundials_cvode.dll -lsundials_nvecserial.dll
#SUNDIALS_LIBS := $(shell sundials-config -t s -l c -m cvode)

# EXEC_FILES = cvdx cvdxe cvbx cvkx cvkxb cvdemd cvdemk

all:
	@(echo)
	@(echo '       make pihm     - make pihm        ')
	@(echo '       make clean    - remove all executable files')
	@(echo)

#	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(SUNDIALS_LIBS) $(LIBS)

pihm: $(OBJS)
	@echo '...Compiling PIHM ...'
	$(CC) $(CFLAGS) -I$(SUNDIALS_INC_DIR) -L$(SUNDIALS_LIB_DIR) -o $(builddir)/pihm $(SRC) $(SUNDIALS_LIBS) $(LIBS)

clean:
	@rm -f *.o
	@rm -f pihm

