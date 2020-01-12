/*
 * tui_common.h
 *
 *  Created on: 23 dec 2019
 *      Author: doctBrown
 */

#ifndef TUI_TUI_COMMON_H_
#define TUI_TUI_COMMON_H_

#include "stdint.h"

const static int32_t TUI_TX_FIFO_LEN = 100000;

enum class COLORS {
	BLACK = 0,
	RED = 1,
	GREEN = 2,
	YELLOW = 3,
	BLUE = 4,
	MAGENTA = 5,
	CYAN = 6,
	WHITE = 7,
	BRIGHT_BLACK = 60,
	BRIGHT_RED = 61,
	BRIGHT_GREEN = 62,
	BRIGHT_YELLOW = 63,
	BRIGHT_BLUE = 64,
	BRIGHT_MAGENTA = 65,
	BRIGHT_CYAN = 66,
	BRIGHT_WHITE = 67,
};

enum class FORMAT {
	RESET_ALL = 0, BOLD = 1, UNDERLINE = 4, SLOW_BLINK = 5, RAPID_BLINK = 6, REVERSE = 7, HIDDEN = 8, DEFAULT = 10, NOT_REVERSE = 27,
};

inline int32_t rst_scr2(char *buf) {
	buf[0] = ('\033');
	buf[1] = ('[');
	buf[2] = ('0');
	buf[3] = ('m');
	return 4;
}

inline int32_t clrscr2(char *buf) {
	buf[0] = ('\033');
	buf[1] = ('[');
	buf[2] = ('2');
	buf[3] = ('J');
	return 4;
}

inline int32_t clreol2(char *buf) {
	buf[0] = ('\033');
	buf[1] = ('[');
	buf[2] = ('K');
	return 3;
}

inline int32_t cursoron2(char *buf) {
	buf[0] = ('\033');
	buf[1] = ('[');
	buf[2] = ('2');
	buf[3] = ('5');
	buf[4] = ('h');
	return 5;
}

inline int32_t cursoroff2(char *buf) {
	buf[0] = ('\033');
	buf[1] = ('[');
	buf[2] = ('?');
	buf[3] = ('2');
	buf[4] = ('5');
	buf[5] = ('l');
	return 6;
}

inline int32_t set_text_color2(uint8_t color, char *buf) {

	uint8_t temp_color = color + 30;

	if (temp_color >= 100) {
		buf[0] = '\033';
		buf[1] = '[';
		buf[2] = (temp_color / 100) % 10 + 0x30;
		buf[3] = (temp_color / 10) % 10 + 0x30;
		buf[4] = (temp_color % 10) + 0x30;
		buf[5] = 'm';
		return 6;
	} else {
		buf[0] = '\033';
		buf[1] = '[';
		buf[2] = (temp_color / 10) + 0x30;
		buf[3] = (temp_color % 10) + 0x30;
		buf[4] = 'm';
		return 5;
	}

}

inline int32_t set_bg2(uint8_t color, char *buf) {

	uint8_t temp_color = color + 40;

	if (temp_color >= 100) {
		buf[0] = '\033';
		buf[1] = '[';
		buf[2] = (temp_color / 100) % 10 + 0x30;
		buf[3] = (temp_color / 10) % 10 + 0x30;
		buf[4] = (temp_color % 10) + 0x30;
		buf[5] = 'm';
		return 6;
	} else {
		buf[0] = '\033';
		buf[1] = '[';
		buf[2] = (temp_color / 10) + 0x30;
		buf[3] = (temp_color % 10) + 0x30;
		buf[4] = 'm';
		return 5;
	}

}

inline int32_t set_format2(uint8_t format, char *buf) {
	buf[0] = '\033';
	buf[1] = '[';
	buf[2] = (format / 10) + 0x30;
	buf[3] = (format % 10) + 0x30;
	buf[4] = 'm';
	return 5;
}

inline int32_t gotoxy2(char x, char y, char *buf) {

	uint8_t x_tmp = x + 1;
	uint8_t y_tmp = y + 1;

	buf[0] = '\033';
	buf[1] = '[';
	buf[2] = (y_tmp / 100) + 0x30;
	buf[3] = ((y_tmp / 10) % 10) + 0x30;
	buf[4] = (y_tmp % 10) + 0x30;
	buf[5] = ';';
	buf[6] = (x_tmp / 100) + 0x30;
	buf[7] = ((x_tmp / 10) % 10) + 0x30;
	buf[8] = (x_tmp % 10) + 0x30;
	buf[9] = 'f';
	return 10;
}

const char gDigitsLut[200] = { '0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9', '1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6',
		'1', '7', '1', '8', '1', '9', '2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9', '3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5',
		'3', '6', '3', '7', '3', '8', '3', '9', '4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9', '5', '0', '5', '1', '5', '2', '5', '3', '5', '4',
		'5', '5', '5', '6', '5', '7', '5', '8', '5', '9', '6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9', '7', '0', '7', '1', '7', '2', '7', '3',
		'7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9', '8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9', '9', '0', '9', '1', '9', '2',
		'9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9' };

// Branching for different cases (forward)
// Use lookup table of two digits
inline int32_t u32toa_branchlut(uint32_t value, char* buffer) {
	int32_t amnt = 0;
	if (value < 10000) {
		const uint32_t d1 = (value / 100) << 1;
		const uint32_t d2 = (value % 100) << 1;

		if (value >= 1000) {
			*buffer++ = gDigitsLut[d1];
			amnt++;
		}
		if (value >= 100) {
			*buffer++ = gDigitsLut[d1 + 1];
			amnt++;
		}
		if (value >= 10) {
			*buffer++ = gDigitsLut[d2];
			amnt++;
		}
		*buffer++ = gDigitsLut[d2 + 1];
		amnt++;
	} else if (value < 100000000) {
		// value = bbbbcccc
		const uint32_t b = value / 10000;
		const uint32_t c = value % 10000;

		const uint32_t d1 = (b / 100) << 1;
		const uint32_t d2 = (b % 100) << 1;

		const uint32_t d3 = (c / 100) << 1;
		const uint32_t d4 = (c % 100) << 1;

		if (value >= 10000000) {
			*buffer++ = gDigitsLut[d1];
			amnt++;
		}
		if (value >= 1000000) {
			*buffer++ = gDigitsLut[d1 + 1];
			amnt++;
		}
		if (value >= 100000) {
			*buffer++ = gDigitsLut[d2];
			amnt++;
		}
		*buffer++ = gDigitsLut[d2 + 1];

		*buffer++ = gDigitsLut[d3];
		*buffer++ = gDigitsLut[d3 + 1];
		*buffer++ = gDigitsLut[d4];
		*buffer++ = gDigitsLut[d4 + 1];
		amnt += 5;
	} else {
		// value = aabbbbcccc in decimal

		const uint32_t a = value / 100000000; // 1 to 42
		value %= 100000000;

		if (a >= 10) {
			const unsigned i = a << 1;
			*buffer++ = gDigitsLut[i];
			*buffer++ = gDigitsLut[i + 1];
			amnt += 2;
		} else {
			*buffer++ = '0' + static_cast<char>(a);
			amnt++;
		}
		const uint32_t b = value / 10000; // 0 to 9999
		const uint32_t c = value % 10000; // 0 to 9999

		const uint32_t d1 = (b / 100) << 1;
		const uint32_t d2 = (b % 100) << 1;

		const uint32_t d3 = (c / 100) << 1;
		const uint32_t d4 = (c % 100) << 1;

		*buffer++ = gDigitsLut[d1];
		*buffer++ = gDigitsLut[d1 + 1];
		*buffer++ = gDigitsLut[d2];
		*buffer++ = gDigitsLut[d2 + 1];
		*buffer++ = gDigitsLut[d3];
		*buffer++ = gDigitsLut[d3 + 1];
		*buffer++ = gDigitsLut[d4];
		*buffer++ = gDigitsLut[d4 + 1];

		amnt += 8;

	}
	//*buffer++ = '\0';
	// amnt++;
	return amnt;
}

inline int32_t i32toa(int32_t value, char* buffer) {
	int32_t amnt = 0;
	uint32_t u = static_cast<uint32_t>(value);
	if (value < 0) {
		*buffer++ = '-';
		u = ~u + 1;
		amnt++;
	} else {
//		*buffer++ = ' ';
//		amnt++;
	}

	amnt += u32toa_branchlut(u, buffer);

	return amnt;
}

inline void u64toa_branchlut(uint64_t value, char* buffer) {
	if (value < 100000000) {
		uint32_t v = static_cast<uint32_t>(value);
		if (v < 10000) {
			const uint32_t d1 = (v / 100) << 1;
			const uint32_t d2 = (v % 100) << 1;

			if (v >= 1000)
				*buffer++ = gDigitsLut[d1];
			if (v >= 100)
				*buffer++ = gDigitsLut[d1 + 1];
			if (v >= 10)
				*buffer++ = gDigitsLut[d2];
			*buffer++ = gDigitsLut[d2 + 1];
		} else {
			// value = bbbbcccc
			const uint32_t b = v / 10000;
			const uint32_t c = v % 10000;

			const uint32_t d1 = (b / 100) << 1;
			const uint32_t d2 = (b % 100) << 1;

			const uint32_t d3 = (c / 100) << 1;
			const uint32_t d4 = (c % 100) << 1;

			if (value >= 10000000)
				*buffer++ = gDigitsLut[d1];
			if (value >= 1000000)
				*buffer++ = gDigitsLut[d1 + 1];
			if (value >= 100000)
				*buffer++ = gDigitsLut[d2];
			*buffer++ = gDigitsLut[d2 + 1];

			*buffer++ = gDigitsLut[d3];
			*buffer++ = gDigitsLut[d3 + 1];
			*buffer++ = gDigitsLut[d4];
			*buffer++ = gDigitsLut[d4 + 1];
		}
	} else if (value < 10000000000000000) {
		const uint32_t v0 = static_cast<uint32_t>(value / 100000000);
		const uint32_t v1 = static_cast<uint32_t>(value % 100000000);

		const uint32_t b0 = v0 / 10000;
		const uint32_t c0 = v0 % 10000;

		const uint32_t d1 = (b0 / 100) << 1;
		const uint32_t d2 = (b0 % 100) << 1;

		const uint32_t d3 = (c0 / 100) << 1;
		const uint32_t d4 = (c0 % 100) << 1;

		const uint32_t b1 = v1 / 10000;
		const uint32_t c1 = v1 % 10000;

		const uint32_t d5 = (b1 / 100) << 1;
		const uint32_t d6 = (b1 % 100) << 1;

		const uint32_t d7 = (c1 / 100) << 1;
		const uint32_t d8 = (c1 % 100) << 1;

		if (value >= 1000000000000000)
			*buffer++ = gDigitsLut[d1];
		if (value >= 100000000000000)
			*buffer++ = gDigitsLut[d1 + 1];
		if (value >= 10000000000000)
			*buffer++ = gDigitsLut[d2];
		if (value >= 1000000000000)
			*buffer++ = gDigitsLut[d2 + 1];
		if (value >= 100000000000)
			*buffer++ = gDigitsLut[d3];
		if (value >= 10000000000)
			*buffer++ = gDigitsLut[d3 + 1];
		if (value >= 1000000000)
			*buffer++ = gDigitsLut[d4];
		if (value >= 100000000)
			*buffer++ = gDigitsLut[d4 + 1];

		*buffer++ = gDigitsLut[d5];
		*buffer++ = gDigitsLut[d5 + 1];
		*buffer++ = gDigitsLut[d6];
		*buffer++ = gDigitsLut[d6 + 1];
		*buffer++ = gDigitsLut[d7];
		*buffer++ = gDigitsLut[d7 + 1];
		*buffer++ = gDigitsLut[d8];
		*buffer++ = gDigitsLut[d8 + 1];
	} else {
		const uint32_t a = static_cast<uint32_t>(value / 10000000000000000); // 1 to 1844
		value %= 10000000000000000;

		if (a < 10)
			*buffer++ = '0' + static_cast<char>(a);
		else if (a < 100) {
			const uint32_t i = a << 1;
			*buffer++ = gDigitsLut[i];
			*buffer++ = gDigitsLut[i + 1];
		} else if (a < 1000) {
			*buffer++ = '0' + static_cast<char>(a / 100);

			const uint32_t i = (a % 100) << 1;
			*buffer++ = gDigitsLut[i];
			*buffer++ = gDigitsLut[i + 1];
		} else {
			const uint32_t i = (a / 100) << 1;
			const uint32_t j = (a % 100) << 1;
			*buffer++ = gDigitsLut[i];
			*buffer++ = gDigitsLut[i + 1];
			*buffer++ = gDigitsLut[j];
			*buffer++ = gDigitsLut[j + 1];
		}

		const uint32_t v0 = static_cast<uint32_t>(value / 100000000);
		const uint32_t v1 = static_cast<uint32_t>(value % 100000000);

		const uint32_t b0 = v0 / 10000;
		const uint32_t c0 = v0 % 10000;

		const uint32_t d1 = (b0 / 100) << 1;
		const uint32_t d2 = (b0 % 100) << 1;

		const uint32_t d3 = (c0 / 100) << 1;
		const uint32_t d4 = (c0 % 100) << 1;

		const uint32_t b1 = v1 / 10000;
		const uint32_t c1 = v1 % 10000;

		const uint32_t d5 = (b1 / 100) << 1;
		const uint32_t d6 = (b1 % 100) << 1;

		const uint32_t d7 = (c1 / 100) << 1;
		const uint32_t d8 = (c1 % 100) << 1;

		*buffer++ = gDigitsLut[d1];
		*buffer++ = gDigitsLut[d1 + 1];
		*buffer++ = gDigitsLut[d2];
		*buffer++ = gDigitsLut[d2 + 1];
		*buffer++ = gDigitsLut[d3];
		*buffer++ = gDigitsLut[d3 + 1];
		*buffer++ = gDigitsLut[d4];
		*buffer++ = gDigitsLut[d4 + 1];
		*buffer++ = gDigitsLut[d5];
		*buffer++ = gDigitsLut[d5 + 1];
		*buffer++ = gDigitsLut[d6];
		*buffer++ = gDigitsLut[d6 + 1];
		*buffer++ = gDigitsLut[d7];
		*buffer++ = gDigitsLut[d7 + 1];
		*buffer++ = gDigitsLut[d8];
		*buffer++ = gDigitsLut[d8 + 1];
	}

	*buffer = '\0';
}

inline void i64toa_branchlut(int64_t value, char* buffer) {
	uint64_t u = static_cast<uint64_t>(value);
	if (value < 0) {
		*buffer++ = '-';
		u = ~u + 1;
	}

	u64toa_branchlut(u, buffer);
}

#endif /* TUI_TUI_COMMON_H_ */
