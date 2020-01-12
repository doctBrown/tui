/*
 * par_cl.h
 *
 *  Created on: 24 dec 2019
 *      Author: doctBrown
 */

#ifndef TUI_PAR_CL_H_
#define TUI_PAR_CL_H_

#include "tui_fifo_cl.h"
#include "stdint.h"
#include "tui_common.h"
#include "string.h"
#include <new>
#include <cmath>

class tui_par_cl {
public:
	/*Possible option of parameter*/
	enum CFG {
		NUM = 0, TXT = 1, SLCTBL = 2, N_SLCTBL = 0, CNTRLBL = 4, N_CNTRLBL = 0, BOOLT = 16, N_BOOLT = 0, REALT = 32, INT32T = 0,
	};

	/*Callback function*/
	typedef void (*cbf_t)(int32_t num_, void *par);

	/* METHODES RECOMMENDED FOR USING BY USER */

	/*should be called before any another methods*/
	bool init(int32_t x, int32_t y, int32_t buf_len, uint32_t cfg);

	/* Set integer number in inner buffer, if parameter is INT32, number will be displayed after invoking method disp();
	 * Be careful, if number changes, will be called callback function cbf_change_(), if specified.
	 * Sets flag upd_req_
	 */
	void set_num(int32_t p);

	/* Get integer num from inner buffer */
	int32_t get_num(void);

	/* Set text data in inner buffer, len should be <=  buf_len in init(...), or excess text will not be displayed.
	 * Sets flag upd_req_.
	 * Text displays if cfg in init(...) is equal TEXT
	 */
	void set_text(char* p, int32_t len);

	/* Sets callback function when pressed "Enter" and this parameter is selected
	 */
	bool set_enter_cb(cbf_t f, void *par);

	/* Sets callback function when pressed "Esc" and this parameter is selected
	 */
	bool set_esc_cb(cbf_t f, void *par);

	/* Sets callback function when number in parameter is changed.
	 * Be careful, because, callback-function should not consist any operation that change the integer number, or there will be an infinite loop
	 */
	bool set_change_cb(cbf_t f, void* par);

	/* Sets callback function when pressed "F1" and this parameter is selected
	 */
	bool set_help_cb(cbf_t f, void* par);

	/* Sets callback function for select event
	 */
	bool set_sel_cb(cbf_t f, void* par);

	/* Set (true) this parameter as default for cursor position or unset (false)
	 */
	void set_def_sel(bool p);

	/* Get (true) this parameter as default for cursor position or unset (false)
	 */
	bool get_def_sel(void);

	/* Return whether integer number changed (true) or not (false) since the last call of this method.
	 */
	bool is_changed(void);

	/* Set allowable range for integer number, inclusively
	 */
	void set_min_max(int32_t min, int32_t max);

	/* Set parameter for displaying real number
	 */
	void set_real_num_par(const int32_t int_len, const int32_t frac_len);

	/* Set real number in inner buffer, if parameter is REAL, number will be displayed after invoking method disp();
	 * Sets flag upd_req_
	 */
	void set_real_num(double p);

	/*Add 0 before significant number with length buf_len, see init(...), i.e.*/
	void set_trailing_zeros(bool p);

	/* METHODS ARE NOT RECOMMENDED FOR USING BY USER, BECAUSE THEY USUALLY USED ИН TUI SUBSYSTEM  */

	/*Set inversion, i.e. parameter will look like as a "selected" in terminal*/
	void set_inv(bool p);

	/*display params (send graphic data to output buffer) if raised flat upd_req_*/
	bool disp(int32_t x, int32_t y, uint8_t bg_colors, uint8_t text_colors);

	/* Sets flag upd_req_.
	 */
	void disp_req(void);

	/* Intentionally causes callback-function for "Enter"-press event
	 */
	bool invoke_enter_cb(void);

	/* Intentionally causes callback-function for "Esc"-press event
	 */
	bool invoke_esc_cb(void);

	/* Intentionally causes callback-function for changing event
	 */
	bool invoke_change_cb(void);

	/* Intentionally causes callback-function for "F1"-press event
	 */
	bool invoke_help_cb(void);

	/* Increment integer number
	 * Sets flag upd_req_.
	 */
	void incr_num(void);

	/* Decrement integer number number
	 * Sets flag upd_req_.
	 */
	void decr_num(void);

	/* Change sign of integer number
	 */
	void change_sigh(void);

	/* Get current configuration mask
	 */
	uint32_t get_cfg(void);

	/* Get current x-coordinate of parameter
	 */
	int32_t x_coord(void);

	/* Get current y-coordinate of parameter
	 */
	int32_t y_coord(void);

	/* Set TX-FIFO pointer. Tx-FIFO should be assigned before disp(...), or content of buffer will be lost.
	 */
	void set_tx_fifo_ptr(tui_fifo_cl<TUI_TX_FIFO_LEN>* t);

	/* Force set integer number to 1
	 * Sets flag upd_req_.
	 */
	void set_num_to_1(void);

	/* Force set integer number to 0
	 * Sets flag upd_req_.
	 */
	void set_num_to_0(void);

private:

	cbf_t cbf_enter_;
	void *cbf_enter_par_;
	cbf_t cbf_esc_;
	void *cbf_esc_par_;
	cbf_t cbf_change_;
	void *cbf_change_par_;
	cbf_t cbf_help_;
	void *cbf_help_par_;
	cbf_t cbf_sel_;
	void *cbf_sel_par_;

	int32_t num_;
	int32_t max_num_;
	int32_t min_num_;
	uint32_t config_;
	uint8_t x_;
	uint8_t y_;
	bool fl_inv_;
	char *buf_;
	int32_t buf_len_;

	char num2str_buf_[20];

	bool upd_req_;
	int32_t text_len_;

	tui_fifo_cl<TUI_TX_FIFO_LEN> *tx_fifo_;

	static const int32_t TMP_BUF_LEN = 100;
	char tmp_buf_[TMP_BUF_LEN];

	bool def_4_cursor_;
	bool changed_;
	int32_t min_, max_;

	double num_real_;
	int32_t frac_len_;
	int32_t int_len_;
	bool trail_zeros;

	int32_t i2s_conv(int32_t num, int32_t buf_len, char *buf) {
		int32_t amnt_d;

		if (buf == nullptr) {
			return 0;
		}

		amnt_d = i32toa(num, num2str_buf_);
		if (amnt_d >= 10) {
			return 0;
		}
		if (amnt_d >= buf_len) {
			memcpy(buf, num2str_buf_, buf_len);
		} else {

			if ((buf_len - amnt_d) < 0) {
				return 0;
			}
			if (trail_zeros) {
				memset(buf, '0', (buf_len - amnt_d));

				if (num >= 0) {
					memcpy(&buf[(buf_len - amnt_d)], num2str_buf_, amnt_d);
				} else {
					memcpy(&buf[(buf_len - amnt_d) + 1], num2str_buf_ + 1, amnt_d - 1); //TOOD
					buf[0] = '-';
				}

			} else {
				memset(buf, ' ', (buf_len - amnt_d));
				memcpy(&buf[(buf_len - amnt_d)], num2str_buf_, amnt_d);
			}

		}

		return amnt_d;
	}

	int32_t d2s_conv(double num, const int32_t int_len, const int32_t frac_len, int32_t buf_len, char *buf) {

		if (buf == 0) {
			return 0;
		}

		if ((int_len + frac_len + 1) > buf_len) {
			if (buf_len >= 3) {
				buf[0] = 'N';
				buf[1] = 'A';
				buf[2] = 'N';
				return 3;
			} else {
				return 0;
			}
		}

		const double frac_factor = pow(10, frac_len);
		const int64_t frac_factor_int = (int64_t) (frac_factor);

		int32_t int_part;
		int32_t frac_part;
		bool fl_inv = false;

		if (num < 0) {
			num = -num;
			fl_inv = true;
		}

		int64_t int_part_tmp = (int64_t) ((num * frac_factor) + 0.5);
		int_part = (int32_t) (int_part_tmp / frac_factor_int);
		frac_part = (int32_t) (int_part_tmp % frac_factor_int);

		int32_t amnt_d;
		int32_t ind = 0;
		memset(buf, ' ', int_len);

		if (fl_inv) {
			buf[0] = '-';
			ind++;
		}

		amnt_d = i32toa(int_part, num2str_buf_);
		if (amnt_d > int_len) {
			memcpy(buf, "NAN", 3);
			ind += 3;
			return 3;
		} else {
			memcpy(&buf[ind], num2str_buf_, amnt_d);
			ind += amnt_d;
		}

		buf[ind] = '.';
		ind += 1;
		memset(buf + ind, '0', frac_len);

		amnt_d = i32toa(frac_part, num2str_buf_);
		if (amnt_d >= frac_len) {
			memcpy(&buf[ind], num2str_buf_, frac_len);
			ind += frac_len;
		} else {
			int32_t bias = (frac_len - amnt_d);
			memcpy(&buf[ind + bias], num2str_buf_, frac_len);
			ind += frac_len;
		}

		return ind;
	}

};

inline bool tui_par_cl::init(int32_t x, int32_t y, int32_t buf_len, uint32_t cfg) {
	cbf_enter_ = nullptr;
	cbf_esc_ = nullptr;
	cbf_change_ = nullptr;
	cbf_enter_par_ = nullptr;
	cbf_esc_par_ = nullptr;
	cbf_change_par_ = nullptr;
	cbf_help_ = nullptr;
	cbf_help_par_ = nullptr;
	cbf_sel_ = nullptr;
	cbf_sel_par_ = nullptr;

	if (x < 0) {
		x_ = 0;
	} else {
		x_ = x;
	}
	if (y < 0) {
		y_ = 0;
	} else {
		y_ = y;
	}

	num_ = 0;
	buf_len_ = buf_len;
	buf_ = new (std::nothrow) char[buf_len_];
	if (buf_ == 0) {
		return true;
	}
	memset(buf_, ' ', buf_len_);
	//my_int32_sprintf(par, data_len, data);
	fl_inv_ = false;
	upd_req_ = true;
	config_ = (uint32_t) cfg;
	text_len_ = 0;
	tx_fifo_ = nullptr;
	def_4_cursor_ = false;
	changed_ = false;
	min_ = -2147483647;
	max_ = 2147483647;
	num_real_ = 0;

	num_real_ = 0;
	frac_len_ = 3;
	int_len_ = 1;
	trail_zeros = false;
	return false;
}

inline void tui_par_cl::set_inv(bool p) {
	if (p == true) {
		if (cbf_sel_ != nullptr) {
			cbf_sel_(num_, cbf_sel_par_);
		}
	}
	fl_inv_ = p;
}

inline bool tui_par_cl::disp(int32_t x, int32_t y, uint8_t bg_colors, uint8_t text_colors) {

	if (tx_fifo_ == nullptr) {
		return true;
	}

	int32_t n, buf_ind, x_ind = 0;
	if (upd_req_) {

		buf_ind = 0;
		n = gotoxy2(x_ + x, y_ + y, &tmp_buf_[buf_ind]);
		buf_ind += n;

		n = set_text_color2(text_colors, &tmp_buf_[buf_ind]);
		buf_ind += n;

		n = set_bg2((uint8_t) bg_colors, &tmp_buf_[buf_ind]);
		buf_ind += n;

		if ((config_ & CFG::SLCTBL) == CFG::SLCTBL) {
			if (fl_inv_) {
				n = set_format2((uint8_t) (FORMAT::REVERSE), &tmp_buf_[buf_ind]);
				buf_ind += n;
			} else {
				n = set_format2((uint8_t) (FORMAT::NOT_REVERSE), &tmp_buf_[buf_ind]);
				buf_ind += n;
			}
		} else {
			n = set_format2((uint8_t) (FORMAT::NOT_REVERSE), &tmp_buf_[buf_ind]);
			buf_ind += n;
		}
		tx_fifo_->enqueue((uint8_t*) tmp_buf_, buf_ind);

		buf_ind = 0;

		if ((config_ & CFG::TXT) == CFG::TXT) {

			int32_t bias = 0;

			for (int i = 0; i < text_len_; i++) {
				if (buf_[i] == '\r') {
					n = gotoxy2(x_ + x, y_ + y + bias, &tmp_buf_[buf_ind]);
					buf_ind += n;

					tx_fifo_->enqueue((uint8_t*) tmp_buf_, buf_ind);

					buf_ind = 0;
				} else if (buf_[i] == '\n') {
					bias++;
					x_ind--;
					if (x_ind < 0) {
						x_ind = 0;
					}
					n = gotoxy2(x_ + x + x_ind, y_ + y + bias, &tmp_buf_[buf_ind]);
					buf_ind += n;

					tx_fifo_->enqueue((uint8_t*) tmp_buf_, buf_ind);

					buf_ind = 0;
				} else {

					tmp_buf_[buf_ind] = buf_[i];
					buf_ind++;
					x_ind++;
				}
			}

			tx_fifo_->enqueue((uint8_t*) tmp_buf_, buf_ind);

			buf_ind = 0;

		} else if ((config_ & CFG::TXT) == CFG::NUM) {
			if ((config_ & CFG::REALT) == CFG::REALT) {
				int32_t n;
				n = d2s_conv(num_real_, int_len_, frac_len_, buf_len_, buf_);

				tx_fifo_->enqueue((uint8_t*) buf_, n);

			} else {
				i2s_conv(num_, buf_len_, buf_);

				tx_fifo_->enqueue((uint8_t*) buf_, buf_len_);

			}
		}
		upd_req_ = false;
	}
	return false;
}

inline void tui_par_cl::set_num(int32_t p) {

	int32_t num_prev = num_;

	if (p > max_) {
		num_ = max_;
	} else {
		num_ = p;
	}

	if (num_ < min_) {
		num_ = min_;
	}

	if (num_ != num_prev) {
		changed_ = true;
		if (cbf_change_ != nullptr) {
			cbf_change_(num_, cbf_change_par_);
		}

	}

	upd_req_ = true;
}

inline int32_t tui_par_cl::get_num(void) {
	return num_;
}
/* Attention, input string can include only \n (from all control characters)  that will be interpreted like a newline
 * Length of one line should not exceed TMP_BUF_LEN = 100 characters, or be ready for memory corruption )
 */
inline void tui_par_cl::set_text(char* p, int32_t len) {
	if (len <= buf_len_) {
		memcpy(buf_, p, len);
		text_len_ = len;
	} else {
		memcpy(buf_, p, buf_len_);
		text_len_ = buf_len_;
	}
	upd_req_ = true;

}

inline void tui_par_cl::disp_req(void) {
	upd_req_ = true;
}

inline bool tui_par_cl::set_enter_cb(cbf_t f, void *par) {
	if (f == nullptr) {
		return true;
	}
	cbf_enter_par_ = par;
	cbf_enter_ = f;
	return false;
}

inline bool tui_par_cl::set_esc_cb(cbf_t f, void *par) {
	if (f == nullptr) {
		return true;
	}
	cbf_esc_par_ = par;
	cbf_esc_ = f;
	return false;
}

inline bool tui_par_cl::set_change_cb(cbf_t f, void* par) {
	if (f == nullptr) {
		return true;
	}
	cbf_change_par_ = par;
	cbf_change_ = f;
	return false;
}

inline bool tui_par_cl::set_help_cb(cbf_t f, void* par) {
	if (f == nullptr) {
		return true;
	}
	cbf_help_par_ = par;
	cbf_help_ = f;
	return false;
}

inline bool tui_par_cl::set_sel_cb(cbf_t f, void* par) {
	if (f == nullptr) {
		return true;
	}
	cbf_sel_par_ = par;
	cbf_sel_ = f;
	return false;
}

inline bool tui_par_cl::invoke_enter_cb(void) {
	if (cbf_enter_ == nullptr) {
		return true;
	}
	cbf_enter_(num_, cbf_enter_par_);
	return false;
}

inline bool tui_par_cl::invoke_esc_cb(void) {
	if (cbf_esc_ == nullptr) {
		return true;
	}
	cbf_esc_(num_, cbf_esc_par_);
	return false;
}

inline bool tui_par_cl::invoke_change_cb(void) {
	if (cbf_change_ == nullptr) {
		return true;
	}
	cbf_change_(num_, cbf_change_par_);
	return false;
}

inline bool tui_par_cl::invoke_help_cb(void) {
	if (cbf_help_ == nullptr) {
		return true;
	}
	cbf_help_(num_, cbf_help_par_);
	return false;
}

inline void tui_par_cl::incr_num(void) {
	int32_t num_prev = num_;
	if ((config_ & (CFG::TXT | CFG::CNTRLBL | CFG::BOOLT)) == (CFG::NUM | CFG::CNTRLBL | CFG::N_BOOLT)) {
		num_++;
		if (num_ > max_) {
			num_ = max_;
		}
		if (num_ != num_prev) {
			changed_ = true;
			if (cbf_change_ != nullptr) {
				cbf_change_(num_, cbf_change_par_);
			}
		}
	}
}

inline void tui_par_cl::decr_num(void) {
	int32_t num_prev = num_;
	if ((config_ & (CFG::TXT | CFG::CNTRLBL | CFG::BOOLT)) == (CFG::NUM | CFG::CNTRLBL | CFG::N_BOOLT)) {
		num_--;
		if (num_ < min_) {
			num_ = min_;
		}

		if (num_ != num_prev) {
			changed_ = true;
			if (cbf_change_ != nullptr) {
				cbf_change_(num_, cbf_change_par_);
			}
		}

	}
}

inline void tui_par_cl::change_sigh(void) {
	int32_t num_prev = num_;
	if ((config_ & (CFG::TXT | CFG::CNTRLBL | CFG::BOOLT)) == (CFG::NUM | CFG::CNTRLBL | CFG::N_BOOLT)) {
		num_ = -num_;

		if (num_ < min_) {
			num_ = min_;
		}

		if (num_ != num_prev) {
			changed_ = true;
			if (cbf_change_ != nullptr) {
				cbf_change_(num_, cbf_change_par_);
			}
		}
	}
}

inline uint32_t tui_par_cl::get_cfg(void) {
	return config_;
}

inline int32_t tui_par_cl::x_coord(void) {
	return x_;
}

inline int32_t tui_par_cl::y_coord(void) {
	return y_;
}

inline void tui_par_cl::set_tx_fifo_ptr(tui_fifo_cl<TUI_TX_FIFO_LEN>* t) {
	tx_fifo_ = t;
}

inline void tui_par_cl::set_def_sel(bool p) {
	def_4_cursor_ = p;
}

inline bool tui_par_cl::get_def_sel(void) {
	return def_4_cursor_;
}

inline void tui_par_cl::set_num_to_1(void) {
	int32_t num_prev = num_;
	if ((config_ & (CFG::TXT | CFG::CNTRLBL | CFG::BOOLT)) == (CFG::NUM | CFG::CNTRLBL | CFG::BOOLT)) {
		num_ = 1;
		if (num_ != num_prev) {
			changed_ = true;
			if (cbf_change_ != nullptr) {
				cbf_change_(num_, cbf_change_par_);
			}
		}
	}
}

inline void tui_par_cl::set_num_to_0(void) {
	int32_t num_prev = num_;
	if ((config_ & (CFG::TXT | CFG::CNTRLBL | CFG::BOOLT)) == (CFG::NUM | CFG::CNTRLBL | CFG::BOOLT)) {
		num_ = 0;
		if (num_ != num_prev) {
			changed_ = true;
			if (cbf_change_ != nullptr) {
				cbf_change_(num_, cbf_change_par_);
			}
		}
	}
}

inline bool tui_par_cl::is_changed(void) {
	if (changed_) {
		changed_ = false;
		return true;
	}
	return false;
}

inline void tui_par_cl::set_min_max(int32_t min, int32_t max) {
	min_ = min;
	max_ = max;
}

inline void tui_par_cl::set_real_num_par(const int32_t int_len, const int32_t frac_len) {
	frac_len_ = frac_len;
	int_len_ = int_len;
}

inline void tui_par_cl::set_real_num(double p) {
	num_real_ = p;
	upd_req_ = true;
}

inline void tui_par_cl::set_trailing_zeros(bool p) {
	trail_zeros = p;
}

#endif /* TUI_PAR_CL_H_ */
