#include "ringbuffer.h"

/**
 * @file
 * Implementation of ring buffer functions.
 */

void ring_buffer_init(ring_buffer_t *buffer) {
  buffer->tail_index = 0;
  buffer->head_index = 0;
}

void ring_buffer_queue(ring_buffer_t *buffer, char value) {
  /* Is buffer full? */
  if(ring_buffer_is_full(buffer)) {
    /* Is going to overwrite the oldest byte */
    /* Increase tail index */
    buffer->tail_index = ((buffer->tail_index + 1) & RING_BUFFER_MASK);
  }

  /* Place value in buffer */
  buffer->buffer[buffer->head_index] = value;
  buffer->head_index = ((buffer->head_index + 1) & RING_BUFFER_MASK);
}

void ring_buffer_queue_arr(ring_buffer_t *buffer, const char *value, ring_buffer_size_t size) {
  /* Add bytes; one by one */
  ring_buffer_size_t i;
  for(i = 0; i < size; i++) {
    ring_buffer_queue(buffer, value[i]);
  }
}

ring_buffer_size_t ring_buffer_dequeue(ring_buffer_t *buffer, char *value) {
	if(ring_buffer_is_empty(buffer)) {
		/* No items */
		return 0;
	}

	*value = buffer->buffer[buffer->tail_index];
	buffer->tail_index = ((buffer->tail_index + 1) & RING_BUFFER_MASK);
	return 1;
}

ring_buffer_size_t ring_buffer_dequeue_arr(ring_buffer_t *buffer, char *value, ring_buffer_size_t len) {
	char *value_ptr = value;
	ring_buffer_size_t cnt = 0;

	if(ring_buffer_is_empty(buffer)) {
		/* No items */
		return 0;
	}

	while((cnt < len) && ring_buffer_dequeue(buffer, value_ptr)) {
		cnt++;
		value_ptr++;
	}
	return cnt;
}

ring_buffer_size_t ring_buffer_peek(ring_buffer_t *buffer, char *value, ring_buffer_size_t index) 
{
	/* Add index to pointer */
	ring_buffer_size_t value_index = ((buffer->tail_index + index) & RING_BUFFER_MASK);

	if(index >= ring_buffer_num_items(buffer)) {
		/* No items at index */
		return 0;
	}


	*value = buffer->buffer[value_index];
	return 1;
}



/**
 * Returns whether a ring buffer is empty.
 * @param buffer The buffer for which it should be returned whether it is empty.
 * @return 1 if empty; 0 otherwise.
 */
uint8_t ring_buffer_is_empty(ring_buffer_t* buffer) {
	return (buffer->head_index == buffer->tail_index);
}

/**
 * Returns whether a ring buffer is full.
 * @param buffer The buffer for which it should be returned whether it is full.
 * @return 1 if full; 0 otherwise.
 */
uint8_t ring_buffer_is_full(ring_buffer_t* buffer) {
	return ((buffer->head_index - buffer->tail_index) & RING_BUFFER_MASK) == RING_BUFFER_MASK;
}

/**
 * Returns the number of items in a ring buffer.
 * @param buffer The buffer for which the number of items should be returned.
 * @return The number of items in the ring buffer.
 */
ring_buffer_size_t ring_buffer_num_items(ring_buffer_t* buffer) {
	return ((buffer->head_index - buffer->tail_index) & RING_BUFFER_MASK);
}
