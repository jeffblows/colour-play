/*
 * 19-Aug-2023
 *
 * main.h
 *
 * Copyright (c) 2023, Jeff Blows
 *
 */

#ifndef _MAIN_H_
#define _MAIN_H_

/**
 * @brief Debug level logging macro.
 *
 * Macro to expose function, line number as well as desired log message.
 *
 * Usage: _DEBUG_PRINT("Payload: %s", post_json);
 */
#ifdef ENABLE_DEBUG_PRINT
  #define _DEBUG_PRINT(...)
    {
      printf("DEBUG:   %s L#%d ", __func__, __LINE__);
      printf(__VA_ARGS__);
      printf("\n");
    }
#else
#define _DEBUG_PRINT(...)
#endif

extern bool exit_program;

#endif
