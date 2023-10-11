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
#include <string.h>
#include <curses.h>

#include "main.h"
#include "process_command_line.h"
#include "pthread.h"
#include "thrd.h"

// used between two threads - only updated tenth_seconds thread
uint8_t      time_index     = 0;
const char * verbose_prompt = "Verbose: ";
bool         exit_program   = false;

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
 * @brief initialise curses and build default screen
 *
 * @return none
 */
void setup_terminal(mmask_t * old_mask_ptr) {

  initscr();
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  noecho();
  cbreak();
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, old_mask_ptr);
  keypad(stdscr, TRUE);

  attroff(COLOR_PAIR(1));
  mvwprintw(stdscr, LINES/2, 0, "%s", verbose_prompt);
  attron(COLOR_PAIR(1));
  refresh();

}

/*
 * @brief clear the full specified line and return to the current screen position
 *
 */
void clear_line(int row) {
  int y, x;            // to store where you are
  getyx(stdscr, y, x); // save current pos
  move(row, 0);          // move to begining of line
  clrtoeol();          // clear line
  move(y, x);
}

/*
 * @brief show details about the mouse event
 *
 * @param mouse_mask the event
 *
 */
void show_event_type(int y, mmask_t mouse_mask) {

  if (y < LINES) {
    int report_row = y;
    clear_line(report_row);
    switch (mouse_mask) {
      case BUTTON1_CLICKED: {
        mvprintw(report_row, 1, "Button 1");
        break;
      }
      case BUTTON1_PRESSED: {
        mvprintw(report_row, 1, "Button 1 clicked");
        break;
      }
      case BUTTON1_RELEASED: {
        mvprintw(report_row, 1, "Button 1 released");
        break;
      }
      case BUTTON1_DOUBLE_CLICKED: {
        mvprintw(report_row, 1, "Button 1 Double");
        break;
      }
      case BUTTON3_CLICKED: {
        mvprintw(report_row, 1, "Button 3");
        break;
      }
      default: {
        mvprintw(report_row, 1, "Not Recognised");
        break;
      }
    }
    clear_line(report_row+1);
    mvprintw(report_row+1, 1, "binary: %d", mouse_mask);
  }

}


/*
 * @brief main to create threads and loop until end
 *
 */
int main(int argc, char** argv) {

  mmask_t old_mouse_mask;

  signal(SIGINT, exit_irq);
  signal(SIGQUIT, exit_irq);
  signal(SIGHUP, reset_irq);

  if (PCL_SUCCESS != process_command_line(argc, argv)) {
    exit(EXIT_FAILURE);
  }

  setup_terminal(&old_mouse_mask);

  int thread_status = 0;
  if (command_line_params.pthreads == 1) {
    thread_status = pthread_start();
  } else {
    thread_status = thrd_start();
  }
  if (thread_status != 0) {
    endwin();
    if (command_line_params.verbose != 0) {
      fprintf(stderr, "%s: Thread create failed\n", (command_line_params.pthreads == 1) ? "Pthreads" : "C11");
    }
    return EXIT_FAILURE;
  }

  wtimeout(stdscr, 100000);
  int loop_ch;
  do {

    loop_ch = getch();
    if (loop_ch != ERR) {
      if (command_line_params.pthreads == 1) {
        pthread_update_status(LINES/2, strlen(verbose_prompt) + 1, loop_ch);
      } else {
        thrd_update_status(LINES/2, strlen(verbose_prompt) + 1, loop_ch);
      }
      switch (loop_ch) {
        case 'q': {
          exit_program = loop_ch == 'q';
          break;
        }
        case '1':
        case '2':
        case '3': {
          command_line_params.verbose = loop_ch - '0';
          break;
        }
        default: {
          break;
        }
      }
    }
  } while (!exit_program);

  // TODO wait for thread exits

  attroff(COLOR_PAIR(1));
  endwin();
  printf("\nexiting now\n");
  printf("Verbose level: %d\n", command_line_params.verbose);
}

