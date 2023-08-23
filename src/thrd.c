/*
 * 23-AUG-2023
 *
 * thrd.c
 *
 * Copyright (c) 2023, Jeff Blows
 *
 */

#include <stdio.h>
#include <threads.h>
#include <ncurses.h>

#include "main.h"
#include "process_command_line.h"
#include "thrd.h"

mtx_t t_screen_lock;

/*
 * @brief display a tenth of seconds counter on the top line of the screen
 *
 */
int t_update_tenth_seconds(void *arg) {
  mtx_lock(&t_screen_lock);
  attroff(COLOR_PAIR(1));
  mvwprintw(stdscr, 0, 0, "HW %04d\r", time_index);
  attron(COLOR_PAIR(1));
  refresh();
  mtx_unlock(&t_screen_lock);
  while (!exit_program) {
    mtx_lock(&t_screen_lock);
    mvwprintw(stdscr, 0, 3, "%04d\r", time_index);
    refresh();
    mtx_unlock(&t_screen_lock);
    usleep(100000);
    time_index++;
  }
}


/*
 * @brief display a seconds counter on the bottom line of the screen
 *
 */
int t_update_seconds(void *arg) {
  int loop_count = 0;
  while (!exit_program) {
    if (time_index % 10 == 0) {
      mtx_lock(&t_screen_lock);
      attroff(COLOR_PAIR(1));
      mvwprintw(stdscr, LINES - 1, 0, "%04d\r", loop_count);
      attron(COLOR_PAIR(1));
      refresh();
      mtx_unlock(&t_screen_lock);
      if (loop_count == command_line_params.loop_count) {
        raise(SIGINT);
      }
      loop_count++;
    }
    usleep(100000);
  }
}


/*
 * @brief initialise thrd
 *
 */
void thrd_init() {
  mtx_init(&t_screen_lock, mtx_plain);
  thrd_create(&update_seconds_thrd, t_update_seconds, NULL);
  thrd_create(&update_tenth_seconds_thrd, t_update_tenth_seconds, NULL);
}
