/*
 * 18-Aug-2023
 *
 * main.c
 *
 * Copyright (c) 2023, Jeff Blows
 *
 * Collection of common methods
 *  command line porecessing
 *  threads
 *  mutex lock
 *  interupt handling
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <curses.h>

#include "main.h"
#include "process_command_line.h"
#include "pthread.h"
#include "thrd.h"

// used between two threads - only updated tenth_seconds thread
uint8_t time_index;

bool exit_program = false;
/*
 * @brief set the exit flag when an interupt is received
 *
 */
void exit_irq(int signal) {
  exit_program = true;
}

/*
 * @brief change config parameters and reread settings if applicable
 *
 */
void reset_irq(int signal) {
  command_line_params.verbose = (command_line_params.verbose + 1) % MAX_VERBOSE_LEVEL;
}

/*
 * @brief main to create threads and loop until end
 *
 */
int main(int argc, char** argv) {

  signal(SIGINT, exit_irq);
  signal(SIGQUIT, exit_irq);
  signal(SIGHUP, reset_irq);

  if (PCL_SUCCESS != process_command_line(argc, argv)) {
    exit(EXIT_FAILURE);
  }

  initscr();
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);

  time_index = 0;

  if (command_line_params.pthreads == 1) {
    if (pthread_start() != 0) {
      if (command_line_params.verbose != 0) {
        fprintf(stderr, "Pthread create failed\n");
      }
      return EXIT_FAILURE;
    }
  } else {
    thrd_start();
  }

  do {
    usleep(200000);
  } while (!exit_program);

  attroff(COLOR_PAIR(1));
  usleep(1000000);
  endwin();
  printf("\nexiting now\n");
  printf("Verbose level: %d\n", command_line_params.verbose);
}

