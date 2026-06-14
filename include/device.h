#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

/* Memory-mapped GPU. The emulated program draws ARGB8888 pixels into the
 * framebuffer at GPU_FB, triggers a frame by storing to GPU_FLUSH, and reads
 * the most recent key from GPU_INPUT. The framebuffer itself is ordinary RAM;
 * only the two control registers are intercepted by the device. */
#define GPU_W 160
#define GPU_H 120
#define GPU_FB 0x0F000000u
#define GPU_FLUSH 0x0F0F0000u
#define GPU_INPUT 0x0F0F0004u

/* Each returns 1 if addr is a device register (and was handled), else 0. */
int device_store_word(uint32_t addr, uint32_t value);
int device_load_word(uint32_t addr, uint32_t *out);

#endif /* DEVICE_H */
