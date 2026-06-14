#include "device.h"

#include "utils.h"

extern struct global *glob;

#ifdef HAVE_SDL

#include <SDL2/SDL.h>
#include <stdlib.h>

#define SCALE 4 /* on-screen pixels per framebuffer pixel */

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static int ready = 0;
static uint32_t last_key = 0;

static void gpu_shutdown(void)
{
  if (texture)
    SDL_DestroyTexture(texture);
  if (renderer)
    SDL_DestroyRenderer(renderer);
  if (window)
    SDL_DestroyWindow(window);
  SDL_Quit();
}

/* Lazily create the window the first time a program flushes a frame, so
 * non-graphical programs never open one. */
static int gpu_init(void)
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    return 0;
  window = SDL_CreateWindow("my_mips", SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, GPU_W * SCALE, GPU_H * SCALE,
                            SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1,
                                SDL_RENDERER_ACCELERATED
                                    | SDL_RENDERER_PRESENTVSYNC);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING, GPU_W, GPU_H);
  if (!window || !renderer || !texture)
    return 0;
  atexit(gpu_shutdown);
  ready = 1;
  return 1;
}

static void pump_events(void)
{
  SDL_Event e;
  while (SDL_PollEvent(&e))
  {
    if (e.type == SDL_QUIT)
      last_key = 'q';
    else if (e.type == SDL_KEYDOWN)
      last_key = (uint32_t)(e.key.keysym.sym & 0xFF);
  }
}

static void gpu_flush(void)
{
  if (!ready && !gpu_init())
    return;
  SDL_UpdateTexture(texture, NULL, (uint8_t *)glob->memory + GPU_FB, GPU_W * 4);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer); /* PRESENTVSYNC paces the frame */
  pump_events();
}

int device_store_word(uint32_t addr, uint32_t value)
{
  (void)value;
  if (addr == GPU_FLUSH)
  {
    gpu_flush();
    return 1;
  }
  return 0;
}

int device_load_word(uint32_t addr, uint32_t *out)
{
  if (addr == GPU_INPUT)
  {
    *out = last_key;
    last_key = 0; /* a key is reported once */
    return 1;
  }
  return 0;
}

#else /* built without SDL: accept the registers but do nothing */

int device_store_word(uint32_t addr, uint32_t value)
{
  (void)value;
  return addr == GPU_FLUSH;
}

int device_load_word(uint32_t addr, uint32_t *out)
{
  if (addr == GPU_INPUT)
  {
    *out = 0;
    return 1;
  }
  return 0;
}

#endif /* HAVE_SDL */
