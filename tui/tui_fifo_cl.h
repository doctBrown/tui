/*
 * tui_fifo_cl.h
 *
 *  Created on: 4 July 2017
 *      Author: doctBrown
 */

#ifndef TUI_FIFO_CL_H_
#define TUI_FIFO_CL_H_
#include "stdint.h"
#include "string.h"
/* FIFO
 * in_p - pointer to the input data
 * out_p - pointer to the output data
 *          Normally in_p - out_p >=0
 *       _______________________________________________
 *      |                        |************|         |
 *      |________________________|____________|_________|
 *      0					   out_p		in_p		  TCP_FIFO_BUF_SIZE-1
 *
 *          If in_p - out_p < 0
 *       _______________________________________________
 *      |*******|                               |*******|
 *      |_______|_______________________________|_______|
 *		0	   in_p							  out_p       TCP_FIFO_BUF_SIZE-1
 *
 *		*- data
 *
 */

template<int32_t FIFO_BUF_SIZE>
class tui_fifo_cl {
public:

	enum {
		FIFO_OK = 0, NOT_ENOUGH_SPACE_IN_FIFO = -1, NOT_ENOUGH_DATA_IN_FIFO = -2, NO_DATA_IN_FIFO = -3,
	};

	int32_t enqueue(uint8_t *data, int32_t len) {

		int32_t len_tales;
		int32_t len_nose;
		int32_t occ_space_in_buf;

		occ_space_in_buf = FIFO.in_p - FIFO.out_p;
		if (occ_space_in_buf < 0) {
			occ_space_in_buf += FIFO_BUF_SIZE;
		}

		if ((occ_space_in_buf + len) > FIFO_BUF_SIZE) {
			return NOT_ENOUGH_SPACE_IN_FIFO;
		}

		if ((FIFO.in_p + len) > FIFO_BUF_SIZE) {
			len_tales = FIFO_BUF_SIZE - FIFO.in_p;
			len_nose = len - len_tales;
			memcpy(&FIFO.buf[FIFO.in_p], data, len_tales);
			memcpy(&FIFO.buf[0], &data[len_tales], len_nose);

		} else {
			memcpy(&FIFO.buf[FIFO.in_p], data, len);

		}

		FIFO.in_p += len;
		FIFO.in_p = FIFO.in_p % FIFO_BUF_SIZE;

		return FIFO_OK;

	}

	int32_t dequeue(uint8_t *data, int32_t len) {

		int32_t byte_in_FIFO, len_tales, len_nose;

		byte_in_FIFO = FIFO.in_p - FIFO.out_p;
		if (byte_in_FIFO < 0) {
			byte_in_FIFO += FIFO_BUF_SIZE;
		}

		if (byte_in_FIFO >= len) {

			if ((FIFO.out_p + len) <= FIFO_BUF_SIZE) {
				memcpy(data, &FIFO.buf[FIFO.out_p], len);
			} else {

				len_tales = FIFO_BUF_SIZE - FIFO.out_p;
				len_nose = len - len_tales;

				memcpy(data, &FIFO.buf[FIFO.out_p], len_tales);
				memcpy(&data[len_tales], &FIFO.buf[0], len_nose);
			}

			FIFO.out_p += len;
			FIFO.out_p = FIFO.out_p % FIFO_BUF_SIZE;

		} else {
			return NOT_ENOUGH_DATA_IN_FIFO;
		}

		return FIFO_OK;
	}

	void rst(void) {
		FIFO.in_p = 0;
		FIFO.out_p = 0;
	}

	int32_t byte_in_FIFO(void) {

		int32_t byte_in_FIFO;

		byte_in_FIFO = FIFO.in_p - FIFO.out_p;
		if (byte_in_FIFO < 0) {
			byte_in_FIFO += FIFO_BUF_SIZE;
		}

		return byte_in_FIFO;

	}
private:

	typedef struct {
		uint8_t buf[FIFO_BUF_SIZE];
		int32_t out_p;
		int32_t in_p;
	} FIFO_t;

	FIFO_t FIFO;
};

#endif /* TUI_FIFO_CL_H_ */

