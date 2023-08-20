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

#include "main.h"
#include "process_command_line.h"

bool exit_program = false;
void exit_irq(int signal) {
  exit_program = true;
}


int main(int argc, char** argv) {
  signal(SIGINT, exit_irq);
  signal(SIGQUIT, exit_irq);

  uint8_t i = 0;
  while (!exit_program) {
    printf("HW %09d\r", i);
    fflush(stdout);
    usleep(100000);
    i++;
  }
  printf("\nexiting now\n");
}

