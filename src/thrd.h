/*
 * 23-aug-2023
 *
 * thrd.h
 *
 * Copyright (c) 2023, Jeff Blows
 *
 */

#ifndef _THRD_H
#define _THRD_H

int t_update_tenth_seconds(void *arg);
int t_update_seconds(void *arg);
void thrd_init();

#endif
