//*****************************************************************************
//
// cmdline.h - Prototypes for command line processing functions.
//
// Copyright (c) 2007-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the Tiva Utility Library.
//
//*****************************************************************************

#include "CustomDefinitions.h"

#ifndef __CMDLINE_H__
#define __CMDLINE_H__

#define CMDLINE_BAD_CMD         (-1)
#define CMDLINE_TOO_MANY_ARGS   (-2)
#define CMDLINE_TOO_FEW_ARGS   (-3)
#define CMDLINE_INVALID_ARG   (-4)

// Command line function callback type.
typedef int (*pfnCmdLine)(int argc, char *argv[]);

int CmdLineProcess(char *pcCmdLine);

#endif // __CMDLINE_H__
