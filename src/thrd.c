/*
 * 23-AUG-2023
 *
 * thrd.c
 *
 * Copyright (c) 2023, Jeff Blows
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <threads.h>
#include <ncurses.h>

#include "main.h"
#include "process_command_line.h"
#include "thrd.h"

static mtx_t t_screen_lock;
static thrd_t update_seconds_thrd;
static thrd_t update_tenth_seconds_thrd;

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
  int last_time_index = -1;

  while (!exit_program) {
    if ((last_time_index != time_index) && (time_index % 10 == 0)) {
      if (mtx_trylock(&t_screen_lock) == 0) {
        attroff(COLOR_PAIR(1));
        mvwprintw(stdscr, LINES - 1, 0, "%04d\r", loop_count);
        attron(COLOR_PAIR(1));
        refresh();
        mtx_unlock(&t_screen_lock);
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
 * @brief put a character to the screen
 *
 * @return  nothing
 *
 */
void thrd_update_status(int y, int x, int ch) {

  mtx_lock(&t_screen_lock);
  attroff(COLOR_PAIR(1));
  if (ch == KEY_MOUSE) {
    MEVENT event;
    if (getmouse(&event) == OK) {
      mvwprintw(stdscr, y+1, 1, "X %4d",event.x);
      mvwprintw(stdscr, y+2, 1, "Y %4d",event.y);
      show_event_type(y+3, event.bstate);
      //mvwprintw(stdscr, 0, 0, "Mouse Event!");
    }
  } else {
    mvwprintw(stdscr, y, x, "%c", ch);
  }
  attron(COLOR_PAIR(1));
  refresh();
  mtx_unlock(&t_screen_lock);

}

/*
 * @brief initialise thrd
 *
 */
int thrd_start() {
  int rc = 0;
  if ((rc = mtx_init(&t_screen_lock, mtx_plain)) != 0) {
    return rc;
  }
  if ((rc = thrd_create(&update_seconds_thrd, t_update_seconds, NULL)) != 0) {
    return rc;
  }
  if ((rc = thrd_create(&update_tenth_seconds_thrd, t_update_tenth_seconds, NULL)) != 0) {
    return rc;
  }

  return rc;
}
