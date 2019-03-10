#******************************************************************************
#
# Makefile - Rules for building the project example.
#
# Copyright (c) 2013-2017 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#
#   Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
#   Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the
#   distribution.
#
#   Neither the name of Texas Instruments Incorporated nor the names of
#   its contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# This is part of revision 2.1.4.178 of the Tiva Firmware Development Package.
#
#******************************************************************************

#******************************************************************************
# Last modified February 13, 2019 by Clara Schaertl Short <clarity@utexas.edu>
#******************************************************************************

# Enable debug symbols.
ifndef DEBUG
export DEBUG=1
endif

all: # The default rule

#
# Defines the part type that this project uses.
#
PART=TM4C123GH6PM
BOARD=ek-tm4c123gxl

DOC_ROOT=doc

#
# Fetches and builds all dependencies, and installs Git hooks.
#
GIT_HOOKS=$(wildcard .githooks/*)
deps:
	rm -rf .git/hooks
	cd .git && ln -s ../.githooks hooks
	-brew bundle

#
# Include the common make definitions.
#
include makedefs

#
# Where to find header files that do not live in the source directory.
#
IPATH=. include

#
# The default rule, which causes the project example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/project.axf
all: ${COMPILER}/project.bin
	@echo Binary size: $(shell stat -f "%z" $<) bytes.

#
# The rules to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER}/* doc *.log

realclean: clean
	@$(MAKE) -C driverlib clean

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the project.
#
C_SOURCES=$(wildcard src/*.c)
ASM_SOURCES=$(wildcard src/*_gcc.S)
OBJECTS=${C_SOURCES:src/%.c=${COMPILER}/%.o} \
      ${ASM_SOURCES:src/%.S=${COMPILER}/%.o}

LIBDRIVER=driverlib/${COMPILER}/libdriver.a
LDSCRIPT=project.ld

${LIBDRIVER}:
	$(MAKE) -C driverlib all

${COMPILER}/project.axf: ${OBJECTS} ${LIBDRIVER} ${LDSCRIPT}
${COMPILER}/project.bin: ${COMPILER}/project.axf

SCATTERgcc_project=${LDSCRIPT}
ENTRY_project=Reset_Handler
CFLAGSgcc=-DTARGET_IS_TM4C123_RB1

#
# Rules for debugging the project.
#
LM4FLASH=lm4flash
OPENOCD=openocd
GDB=${PREFIX}-gdb

flash: ${COMPILER}/project.bin
	$(LM4FLASH) $<

debug: ${COMPILER}/project.axf
	$(GDB) $<

#
# Rules for building documentation.
#
#DOC_MARKDOWN=$(wildcard *.md)
#DOC_SOURCES=$(filter-out src/startup_gcc.c,${C_SOURCES})
#DOC_HEADERS=$(wildcard include/*.h)
#
#MARKDOWN_PDFS=${DOC_MARKDOWN:%.md=doc/%.pdf}
#
#docs: doc/source.pdf doc/headers.pdf ${MARKDOWN_PDFS}
#
#${DOC_MARKDOWN:%.md=doc/%.3}: doc/%.3: %.md
#	doxygen
#
#doc/%.3: src/% Doxyfile
#	doxygen
#
#doc/%.3: include/% Doxyfile
#	doxygen
#
#doc/headers.pdf: ${DOC_HEADERS:include/%=doc/%.3}
#	@echo "  MAN   ${@}"
#	@man -t $^ | pstopdf -o $@
#
#doc/source.pdf: ${DOC_SOURCES:src/%=doc/%.3}
#	@echo "  MAN   ${@}"
#	@man -t $^ | pstopdf -o $@
#
#doc/%.pdf: doc/%.3
#	@echo "  MAN   ${@}"
#	@man -t $^ | pstopdf -o $@
#
#
# Include the automatically generated dependency files.
#
#ifneq (${MAKECMDGOALS},clean)
#-include ${wildcard ${COMPILER}/*.d} __dummy__
#endif
