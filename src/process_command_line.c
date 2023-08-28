/*
 * 18-Aug-2023
 *
 * process_command_line.c
 *
 * Copyright (c) 2023, Jeff Blows
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#include "process_command_line.h"


command_line_params_struct command_line_params = {.verbose = 0, .loop_count = 0, .pthreads = 0};

/**
 * @brief display a short message for command usage
 *
 */
void show_usage(char * command_name) {
  fprintf(stderr, "Usage: %s [-v verbose_level] [-p] <loop_count>\n", command_name);
  fprintf(stderr, "\t-v <n> verbose error message level\n");
  fprintf(stderr, "\t-p thread model to use. Default is C11 threads. Option is pthreads.\n");
}

int process_command_line(int argc, char** argv) {
  int opt;
  bool usage_error = false;

  while ((opt = getopt(argc, argv, "pv:")) != -1) {
    switch (opt) {
      case 'v': {
        long int lv = strtol(optarg, NULL, 0);
        if ((errno == ERANGE) || (errno == EINVAL)) {
          show_usage(argv[0]);
          fprintf(stderr, "Verbose option invalid:\n");
          usage_error = true;
          continue;
        }
        command_line_params.verbose = lv % MAX_VERBOSE_LEVEL;
        break;
      }
      case 'p': {
        command_line_params.pthreads = 1;
        break;
      }
      default: {
        show_usage(argv[0]);
        usage_error = true;
      }
    }
  }
  if (usage_error) {
    return PCL_BAD_OPTION;
  }
  if (optind >= argc) {
    // we have run out of parameters without getting our counter
    show_usage(argv[0]);
    return PCL_MISSING_OPTION;
  }
  command_line_params.loop_count = strtoul(argv[optind], NULL, 0);

  return PCL_SUCCESS;
}
