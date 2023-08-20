/*
 * 18-Aug-2023
 *
 * main.c
 *
 * Copyright (c) 2023, Jeff Blows
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <curses.h>

#include "main.h"
#include "process_command_line.h"

bool exit_program = false;
void exit_irq(int signal) {
  exit_program = true;
}


int main(int argc, char** argv) {
  signal(SIGINT, exit_irq);
  signal(SIGQUIT, exit_irq);

  initscr();
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);

  uint8_t i = 0;
  mvwprintw(stdscr, 0, 0, "HW %09d\r", i);
  attron(COLOR_PAIR(1));
  while (!exit_program) {
    mvwprintw(stdscr, 0, 3, "%09d\r", i);
    refresh();
    usleep(100000);
    i++;
    if (i % 10 == 0) {
      attroff(COLOR_PAIR(1));
      mvwprintw(stdscr, LINES - 1, 0, "%09d\r", i);
      attron(COLOR_PAIR(1));
    }
  }
  attroff(COLOR_PAIR(1));
  endwin();
  printf("\nexiting now\n");
}

