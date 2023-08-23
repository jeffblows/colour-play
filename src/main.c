/*
 * 18-Aug-2023
 *
 * main.c
 *
 * Copyright (c) 2023, Jeff Blows
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <curses.h>
#include <pthread.h>

#include "main.h"
#include "process_command_line.h"


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

// used between two threads - only updated part_seconds thread
uint8_t time_index;
pthread_mutex_t screen_lock;

// two threads
//    display tenth of seconds on top line of screen
//    display full seconds on bottom line of screen
pthread_t update_seconds_thread;
pthread_t update_tenth_seconds_thread;


/*
 * @brief display a tenth of seconds counter on the top line of the screen
 *
 */
void* update_tenth_seconds(void *arg) {
  pthread_mutex_lock(&screen_lock);
  attroff(COLOR_PAIR(1));
  mvwprintw(stdscr, 0, 0, "HW %04d\r", time_index);
  attron(COLOR_PAIR(1));
  refresh();
  pthread_mutex_unlock(&screen_lock);
  while (!exit_program) {
    pthread_mutex_lock(&screen_lock);
    mvwprintw(stdscr, 0, 3, "%04d\r", time_index);
    refresh();
    pthread_mutex_unlock(&screen_lock);
    usleep(100000);
    time_index++;
  }
}


/*
 * @brief display a seconds counter on the bottom line of the screen
 *
 */
void* update_seconds(void *arg) {
  int loop_count = 0;
  while (!exit_program) {
    if (time_index % 10 == 0) {
      pthread_mutex_lock(&screen_lock);
      attroff(COLOR_PAIR(1));
      mvwprintw(stdscr, LINES - 1, 0, "%04d\r", loop_count);
      attron(COLOR_PAIR(1));
      refresh();
      pthread_mutex_unlock(&screen_lock);
      exit_program = (loop_count == command_line_params.loop_count);
      loop_count++;
    }
    usleep(100000);
  }
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

  pthread_create(&update_seconds_thread, NULL, update_seconds, NULL);
  pthread_create(&update_tenth_seconds_thread, NULL, update_tenth_seconds, NULL);

  do {
  } while (!exit_program);

  attroff(COLOR_PAIR(1));
  usleep(1000000);
  endwin();
  printf("\nexiting now\n");
  printf("Verbose level: %d\n", command_line_params.verbose);
}

