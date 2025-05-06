#pragma once

#include <stdint.h>

typedef struct _rt_ctx {
  // A pointer to the framebuffer.
  void *framebuffer;

  // The framebuffer width, in pixels.
  uint32_t framebuffer_width;

  // The framebuffer height, in pixels.
  uint32_t framebuffer_height;

  // Set this to 1 if you prefer using DOS line endings to UNIX line endings.
  int use_crlf_ending;

  // Do we need to show a cursor after printing text?
  int show_cursor;

  // The background color
  uint32_t bg_color;

  // The foreground color
  uint32_t fg_color;

  /*
   * Do NOT modify everything below me!
   */
  // The terminal's width, in columns of 8 pixels.
  int term_width;
  // The terminal's height, in rows of 16 pixels.
  int term_height;
  // The X position of the cursor, in columns of 8 pixels.
  int x;
  // The Y position of the cursor, in rows of 16 pixels.
  int y;
} rt_context;

void rt_init(rt_context ctx);
void rt_print(char *str);