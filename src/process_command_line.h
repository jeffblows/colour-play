/*
 * 19-Aug-2023
 *
 * process_command_line.h
 *
 * Copyright (c) 2023, Jeff Blows
 *
 */

#ifndef _PROCESS_COMMAND_LINE_H_
#define _PROCESS_COMMAND_LINE_H_

#define PCL_SUCCESS (0)

typedef struct {
  int verbose;
} command_line_params_struct;

extern command_line_params_struct command_line_params;

extern int process_command_line(int argc, char** argv);

#endif
