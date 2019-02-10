//*****************************************************************************
//
// cmdline.c - Functions to help with processing command lines.
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

//*****************************************************************************
// Process a command line string into arguments and execute the command.
//
// pcCmdLine points to a string that contains a command line that was
// obtained by an application by some means.
//
// This function will take the supplied command line string and break it up
// into individual arguments.  The first argument is treated as a command and
// is searched for in the command table.  If the command is found, then the
// command function is called and all of the command line arguments are passed
// in the normal argc, argv form.
//
// The command table is contained in an array named g_psCmdTable
// containing tCmdLineEntry structures which must be provided by the
// application.  The array must be terminated with an entry whose pcCmd
// field contains a NULL pointer.
//
// Returns CMDLINE_BAD_CMD if the command is not found,
// CMDLINE_TOO_MANY_ARGS if there are more arguments than can be parsed.
// Otherwise it returns the code that was returned by the command function.
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "cmdline.h"
#include "CustomDefinitions.h"
#include "cmd_interface.h"

#define CMDLINE_MAX_ARGS        8

// Structure for an entry in the command list table.
typedef struct{
    const char *pcCmd;  // A pointer to a string containing the name of the command.
    pfnCmdLine pfnCmd;  // A function pointer to the implementation of the command.
    const char *pcHelp; // A pointer to a string of brief help text for the command.
}tCmdLineEntry;


tCmdLineEntry g_psCmdTable[] = {
    {"message",&CMD_Message,"Message <device> <line> <string> [number]"},
};

static char *g_ppcArgv[CMDLINE_MAX_ARGS + 1];

int CmdLineProcess(char *pcCmdLine){
    char *pcChar;
    uint_fast8_t ui8Argc;
    bool bFindArg = true;
    tCmdLineEntry *psCmdEntry;

    ui8Argc = 0;  // Initialize the argument counter, and point to the beginning of the command line string.
    pcChar = pcCmdLine;

    while(*pcChar){// Advance through the command line until a zero character is found.
        if(*pcChar == ' '){ // If there is a space, then replace it with a zero, and set the flag to search for the next argument.
            *pcChar = 0;
            bFindArg = true;
        }else{ // Otherwise it is not a space, so it must be a character that is part of an argument.
            if(bFindArg){ // If bFindArg is set, then that means we are looking for the start of the next argument.
                // As long as the maximum number of arguments has not been
                // reached, then save the pointer to the start of this new arg
                // in the argv array, and increment the count of args, argc.
                if(ui8Argc < CMDLINE_MAX_ARGS){
                    g_ppcArgv[ui8Argc] = pcChar;
                    ui8Argc++;
                    bFindArg = false;
                }else{ // The maximum number of arguments has been reached so return the error.
                    return(CMDLINE_TOO_MANY_ARGS);
                }
            }
        }
        pcChar++; // Advance to the next character in the command line.
    }

    if(ui8Argc){ // If one or more arguments was found, then process the command.
        psCmdEntry = &g_psCmdTable[0]; // Start at the beginning of the command table, to look for a matching command.
        while(psCmdEntry->pcCmd){ // Search through the command table until a null command string is found, which marks the end of the table.
            // If this command entry command string matches argv[0], then call
            // the function for this command, passing the command line arguments.
            if(!strcmp(g_ppcArgv[0], psCmdEntry->pcCmd)){
                return(psCmdEntry->pfnCmd(ui8Argc, g_ppcArgv));
            }
            psCmdEntry++; // Not found, so advance to the next entry.
        }
    }
    return(CMDLINE_BAD_CMD); // Fall through to here means that no matching command was found, so return an error.
}
