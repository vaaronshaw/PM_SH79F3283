#ifndef RINGBUFFER_H
#define RINGBUFFER_H
/**
 * @file
 * Prototypes and structures for the ring buffer module.
 */

#include "defines.h"
/**
 * The size of a ring buffer.
 * Due to the design only <tt> RING_BUFFER_SIZE-1 </tt> items
 * can be contained in the buffer.
 * The buffer size must be a power of two.
*/
#define RING_BUFFER_SIZE 50

#if (RING_BUFFER_SIZE & (RING_BUFFER_SIZE - 1)) != 0
//#error "RING_BUFFER_SIZE must be a power of two"
#endif

/**
 * The type which is used to hold the size
 * and the indicies of the buffer.
 * Must be able to fit \c RING_BUFFER_SIZE .
 */
typedef unsigned char uint8_t;
typedef uint8_t ring_buffer_size_t;

/**
 * Used as a modulo operator
 * as <tt> a % b = (a & (b 鈭� 1)) </tt>
 * where \c a is a positive index in the buffer and
 * \c b is the (power of two) size of the buffer.
 */
#define RING_BUFFER_MASK (RING_BUFFER_SIZE-1)

/**
 * Simplifies the use of <tt>struct ring_buffer_t</tt>.
 */
typedef struct ring_buffer_t ring_buffer_t;

/**
 * Structure which holds a ring buffer.
 * The buffer contains a buffer array
 * as well as metavalue for the ring buffer.
 */
struct ring_buffer_t {
  /** Buffer memory. */
  char buffer[RING_BUFFER_SIZE];
  /** Index of tail. */
  ring_buffer_size_t tail_index;
  /** Index of head. */
  ring_buffer_size_t head_index;
};

/**
 * Initializes the ring buffer pointed to by <em>buffer</em>.
 * This function can also be used to empty/reset the buffer.
 * @param buffer The ring buffer to initialize.
 */
void ring_buffer_init(ring_buffer_t *buffer);

/**
 * Adds a byte to a ring buffer.
 * @param buffer The buffer in which the value should be placed.
 * @param value The byte to place.
 */
void ring_buffer_queue(ring_buffer_t *buffer, char value);

/**
 * Adds an array of bytes to a ring buffer.
 * @param buffer The buffer in which the value should be placed.
 * @param value A pointer to the array of bytes to place in the queue.
 * @param size The size of the array.
 */
void ring_buffer_queue_arr(ring_buffer_t *buffer, const char *value, ring_buffer_size_t size);

/**
 * Returns the oldest byte in a ring buffer.
 * @param buffer The buffer from which the value should be returned.
 * @param value A pointer to the location at which the value should be placed.
 * @return 1 if value was returned; 0 otherwise.
 */
uint8_t ring_buffer_dequeue(ring_buffer_t *buffer, char *value);

/**
 * Returns the <em>len</em> oldest bytes in a ring buffer.
 * @param buffer The buffer from which the value should be returned.
 * @param value A pointer to the array at which the value should be placed.
 * @param len The maximum number of bytes to return.
 * @return The number of bytes returned.
 */
uint8_t ring_buffer_dequeue_arr(ring_buffer_t *buffer, char *value, ring_buffer_size_t len);
/**
 * Peeks a ring buffer, i.e. returns an element without removing it.
 * @param buffer The buffer from which the value should be returned.
 * @param value A pointer to the location at which the value should be placed.
 * @param index The index to peek.
 * @return 1 if value was returned; 0 otherwise.
 */
uint8_t ring_buffer_peek(ring_buffer_t *buffer, char *value, ring_buffer_size_t index);

extern uint8_t ring_buffer_is_empty(ring_buffer_t* buffer);
extern uint8_t ring_buffer_is_full(ring_buffer_t* buffer);
extern uint8_t ring_buffer_num_items(ring_buffer_t* buffer);

#endif /* RINGBUFFER_H */
