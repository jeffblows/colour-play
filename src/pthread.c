/*
 *
 * 24-aug-2023
 *
 * pthread.c
 *
 * Copyright (c) 2023, Jeff Blows
 *
 */


#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <curses.h>

#include "main.h"
#include "process_command_line.h"

static pthread_mutex_t p_screen_lock;


// two threads
//    display tenth of seconds on top line of screen
//    display full seconds on bottom line of screen
static pthread_t update_seconds_thread;
static pthread_t update_tenth_seconds_thread;

/*
 * @brief display a tenth of seconds counter on the top line of the screen
 *
 */
void* update_tenth_seconds(void *arg) {
  int lock_status = 0;
  pthread_mutex_lock(&p_screen_lock);
  attroff(COLOR_PAIR(1));
  mvwprintw(stdscr, 0, 0, "HW %04d\r", time_index);
  attron(COLOR_PAIR(1));
  refresh();
  pthread_mutex_unlock(&p_screen_lock);
  while (!exit_program) {
    if ((lock_status = pthread_mutex_trylock(&p_screen_lock)) == 0)   {
      mvwprintw(stdscr, 0, 3, "%04d\r", time_index);
      refresh();
      pthread_mutex_unlock(&p_screen_lock);
    }
    usleep(100000);
    time_index++;
  }
}


/*
 * @brief display a seconds counter on the bottom line of the screen
 *
 */
void* update_seconds(void *arg) {
  int lock_status = 0;
  int loop_count = 0;
  int last_time_index = -1;
  while (!exit_program) {
    if ((last_time_index != time_index) && (time_index % 10 == 0)) {
      if ((lock_status = pthread_mutex_trylock(&p_screen_lock)) == 0) {
        attroff(COLOR_PAIR(1));
        mvwprintw(stdscr, LINES - 1, 0, "%04d\r", loop_count);
        attron(COLOR_PAIR(1));
        refresh();
        pthread_mutex_unlock(&p_screen_lock);
      }
      last_time_index = time_index;
      if (loop_count == command_line_params.loop_count) {
        raise(SIGINT);
      }
      loop_count++;
    }
    usleep(100000);
  }
}

/*
 * @brief initialise and start the pthreads
 *
 * @return return status of system calls - 0 if success
 *
 */
int pthread_start() {
  int rc = 0;

  if ((rc = pthread_mutex_init(&p_screen_lock, NULL)) != 0) {
    return rc;
  }
  if ((rc = pthread_create(&update_seconds_thread, NULL, update_seconds, NULL)) != 0) {
    return rc;
  }
  if ((rc = pthread_create(&update_tenth_seconds_thread, NULL, update_tenth_seconds, NULL)) != 0) {
    return rc;
  }

  return rc;
}
