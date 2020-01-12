/*
 * window_cl.h
 *
 *  Created on: 23 dec 2019
 *      Author: doctBrown
 */

#ifndef TUI_WIND_CL_H_
#define TUI_WIND_CL_H_

#include "tui_fifo_cl.h"
#include "tui_par_cl.h"
#include "stdint.h"
#include "tui_common.h"
#include "string.h"
#include <new>
#include <cstdint>
#include <vector>

class tui_wind_cl {

public:

	/* METHODES RECOMMENDED FOR USING BY USER */

	/*should be called before any another methods*/
	bool init(int32_t x, int32_t y, int32_t width, int32_t height, const char* head, int8_t head_len);

	/* Reset selected (with cursor) parameter to default state, if options tui_par_cl::NUM, tui_par_cl::CFG::CNTRLBL, tui_par_cl::CFG::N_BOOLT is set.
	 * Reset virtual cursor to the start digit and set number in parameter to 0;
	 * Sets flag upd_req_.
	 */
	void reset_par(void);

	/*set background color of the window*/
	void set_bg_color(COLORS p);

	/*set parameter (text on num) color*/
	void set_txt_color(COLORS p);

	/*Register parameter in window. Attention, parameter can be registered only in one window */
	void reg_par(tui_par_cl* p);

	/* On/Off shadow of window*/
	void shadow(bool s);

	/* Behavior cursor when activate this window (on new layer)
	 *  p = false - set cursor on last active parameter, else set it on default parameter
	 *  default = false;
	 */
	void set_tab_sel_def(bool p);

	/* Behavior cursor when activate this window through TAB key
	 *  p = false - set cursor on last active parameter, else set it on default parameter
	 *  default = true;
	 */
	void set_act_sel_def(bool p);

	bool get_tab_sel_def(void);

	bool get_act_sel_def(void);

	/* On/Off frame of window*/
	void framing(bool f);

	/* Requires window activation, set activation request of the window
	 */
	void req_act(void);

	/* Return true if request of activation the window is raised
	 */
	bool chk_activ(void);

	/* METHODS ARE NOT RECOMMENDED FOR USING BY USER, BECAUSE THEY USUALLY USED IN TUI SUBSYSTEM  */

	/*display main body of the window (send graphic data to output buffer)*/
	bool disp_frame();

	/*display all parameters registered in the window (send graphic data to output buffer)
	 * if flag upd_req_ of parameter is raised
	 */
	void disp_par();

	/* Select (put cursor) parameter below */
	void sel_down_par(void);

	/* Select (put cursor) parameter to the left */
	void sel_left_par(void);

	/* Select (put cursor) parameter to the right */
	void sel_right_par(void);

	/* Select (put cursor) parameter after jumping to the new window through the "TAB".
	 * If before was called set_tab_sel_def(...) with true option, after jumping to the new window will be
	 * selected default parameter, else will be selected last active parameter. Default parameter is set through set_def_sel(...);
	 */
	void sel_tab_par(void);

	/* Select (put cursor) parameter after activate new window (usually when new layer is activated).
	 * If before was called set_act_sel_def(...) with true option, after jumping to the new window will be
	 * selected default parameter, else will be selected last active parameter. Default parameter is set through set_def_sel(...);
	 */
	void sel_act_par(void);

	/* unselect current parameter*/
	void sel_up_par(void);

	/* Select (put cursor) parameter above */
	void desel_par(void);

	/* Increment selected (with cursor) parameter if option of parameter is set with options tui_par_cl::CFG::NUM and tui_par_cl::CFG::INT32T
	 * Sets flag upd_req_.
	 */
	void incr_num(void);

	/* Decrement selected (with cursor) parameter if option of parameter is set with options tui_par_cl::CFG::NUM and tui_par_cl::CFG::INT32T
	 * Sets flag upd_req_.
	 */
	void decr_num(void);

	/* Set number in digit of selected (with cursor) parameter if option of parameter is set with options tui_par_cl::CFG::NUM and tui_par_cl::CFG::INT32T.
	 * After calling this method virtual cursor moves to the left for one digit. Reset to the start digit performed after select new parameter (cursor shift) of reset parameter.
	 * Sets flag upd_req_.
	 */
	void set_digit_in_par(int32_t digit);

	/* Forcibly display all parameters of window, usually used when new layer is activated
	 */
	void req_disp_all_par(void);

	/* Reset request of activation the window
	 */
	void rst_req_act(void);

	/* Intentionally causes callback-function for "Enter"-press event for selected (with cursor) parameter
	 */
	bool invoke_enter(void);

	/* Intentionally causes callback-function for "Esc"-press event for selected (with cursor) parameter
	 */
	bool invoke_esc(void);

	/* Intentionally causes callback-function for "F1"-press event for selected (with cursor) parameter
	 */
	bool invoke_help(void);

	/* Calculate transitions table, that determined new cursor position for every parameter in window when arrow keys using*/
	void calc_sel_tbl(void);

	/* Set pointer to output fifo-buffer*/
	void set_tx_fifo_ptr(tui_fifo_cl<TUI_TX_FIFO_LEN>* t);

	/* Invert parameter if parameter is set with option tui_par_cl::CFG::BOOLT
	 */
	void change_bool_num(void);

private:
	int32_t x_, y_, width_, height_;
	COLORS bg_colors_;
	COLORS text_colors_;
	uint8_t *head_buf_;
	uint8_t head_len_;
	std::vector<tui_par_cl*> par_vec_;
	std::vector<int32_t> sel_tbl_right_;
	std::vector<int32_t> sel_tbl_left_;
	std::vector<int32_t> sel_tbl_up_;
	std::vector<int32_t> sel_tbl_down_;

	int32_t sel_par_ind_;
	int32_t digit_weight_;
	bool prohibit_idecr_;
	bool activate_fl_;
	int32_t def_sel_ind_;
	bool shadow_;
	tui_fifo_cl<TUI_TX_FIFO_LEN> *tx_fifo_;

	static const int32_t TMP_BUF_LEN = 1024;
	char tmp_buf_[TMP_BUF_LEN];
	bool tab_sel_def_ = false;
	bool act_sel_def_ = true;
	bool framing_ = true;
};

/* Should be called before any another method calls */
inline bool tui_wind_cl::init(int32_t x, int32_t y, int32_t width, int32_t height, const char* head, int8_t head_len) {
	x_ = x;
	y_ = y;
	width_ = width;
	height_ = height;
	head_len_ = head_len;
	head_buf_ = new (std::nothrow) uint8_t[head_len_];
	if (head_buf_ == 0) {
		return true;
	}
	memcpy(head_buf_, head, head_len);
	bg_colors_ = COLORS::BLUE;
	text_colors_ = COLORS::WHITE;
	par_vec_.clear();
	sel_par_ind_ = 0;
	digit_weight_ = 0;
	activate_fl_ = false;
	def_sel_ind_ = 0;
	shadow_ = false;
	tx_fifo_ = nullptr;
	tab_sel_def_ = false;
	act_sel_def_ = true;
	framing_ = true;
	prohibit_idecr_ = false;
	return false;
}

inline void tui_wind_cl::set_bg_color(COLORS p) {
	bg_colors_ = p;
}

inline void tui_wind_cl::set_txt_color(COLORS p) {
	text_colors_ = p;
}

inline bool tui_wind_cl::disp_frame() {

	if (tx_fifo_ == nullptr) {
		return true;
	}

	char ACS_ULCORNER = 32; //47;//(201); /* upper left corner */
	char ACS_LLCORNER = 32; //92;//(200); /* lower left corner */
	char ACS_URCORNER = 32; //92; /* upper right corner */
	char ACS_LRCORNER = 32; //47; /* lower right corner */
	char ACS_HLINE = '-'; //(205); /* horizontal line */
	char ACS_VLINE = '|'; //(186); /* vertical line */

	if (framing_) {

		ACS_ULCORNER = 32; //47;//(201); /* upper left corner */
		ACS_LLCORNER = 32; //92;//(200); /* lower left corner */
		ACS_URCORNER = 32; //92; /* upper right corner */
		ACS_LRCORNER = 32; //47; /* lower right corner */
		ACS_HLINE = '-'; //(205); /* horizontal line */
		ACS_VLINE = '|'; //(186); /* vertical line */

	} else {
		ACS_ULCORNER = 32; //47;//(201); /* upper left corner */
		ACS_LLCORNER = 32; //92;//(200); /* lower left corner */
		ACS_URCORNER = 32; //92; /* upper right corner */
		ACS_LRCORNER = 32; //47; /* lower right corner */
		ACS_HLINE = 32; //(205); /* horizontal line */
		ACS_VLINE = 32; //(186); /* vertical line */

	}

	if ((width_ > 256) || (height_ > 256)) {
		return true;
	}

	int32_t k = 0, n, buf_ind;
	int32_t head_pos = width_ / 2 - head_len_ / 2;
	if (head_pos < 0) {
		head_pos = 0;
	}

	buf_ind = 0;
	n = gotoxy2(x_, y_, &tmp_buf_[buf_ind]);
	buf_ind += n;

	n = set_text_color2((uint8_t) text_colors_, &tmp_buf_[buf_ind]);
	buf_ind += n;

	n = set_bg2((uint8_t) bg_colors_, &tmp_buf_[buf_ind]);
	buf_ind += n;
	tx_fifo_->enqueue((uint8_t*) tmp_buf_, buf_ind);
	buf_ind = 0;

	tmp_buf_[buf_ind] = ACS_ULCORNER;
	buf_ind++;

	for (int32_t i = 0; i < width_; i++) {

		if ((i >= head_pos) && (k < head_len_)) {
			tmp_buf_[buf_ind] = head_buf_[k];
			buf_ind++;
			k++;
		} else {
			tmp_buf_[buf_ind] = ACS_HLINE;
			buf_ind++;
		}
	}

	tmp_buf_[buf_ind] = ACS_URCORNER;
	buf_ind++;
	tx_fifo_->enqueue((uint8_t*) tmp_buf_, buf_ind);
	buf_ind = 0;

	for (int32_t i = 0; i < height_; i++) {

		n = gotoxy2(x_, y_ + i + 1, tmp_buf_);
		buf_ind += n;

		tmp_buf_[buf_ind] = ACS_VLINE;
		buf_ind++;

		memset(&tmp_buf_[buf_ind], ' ', width_);
		buf_ind += width_;

		tmp_buf_[buf_ind] = ACS_VLINE;
		buf_ind++;

		tx_fifo_->enqueue((uint8_t*) tmp_buf_, buf_ind);
		buf_ind = 0;

	}

	n = gotoxy2(x_, y_ + height_ + 1, tmp_buf_);
	buf_ind += n;

	tmp_buf_[buf_ind] = ACS_LLCORNER;
	buf_ind++;

	memset(&tmp_buf_[buf_ind], ACS_HLINE, width_);
	buf_ind += width_;

	tmp_buf_[buf_ind] = ACS_LRCORNER;
	buf_ind++;

	tx_fifo_->enqueue((uint8_t*) tmp_buf_, buf_ind);
	buf_ind = 0;

	if (shadow_) {

		n = set_bg2((uint8_t) COLORS::BLACK, &tmp_buf_[buf_ind]);
		buf_ind += n;

		n = gotoxy2(x_ + 1, y_ + height_ + 2, &tmp_buf_[buf_ind]);
		buf_ind += n;

		for (int32_t i = 0; i < height_ + 1; i++) {

			n = gotoxy2(x_ + width_ + 2, y_ + i + 1, &tmp_buf_[buf_ind]);
			buf_ind += n;

			tmp_buf_[buf_ind] = ' ';
			buf_ind++;

		}

		n = gotoxy2(x_ + 1, y_ + height_ + 2, &tmp_buf_[buf_ind]);
		buf_ind += n;

		memset(&tmp_buf_[buf_ind], ' ', width_ + 2);
		buf_ind += (width_ + 2);

		tx_fifo_->enqueue((uint8_t*) tmp_buf_, buf_ind);
		buf_ind = 0;

	}

	return false;

}

inline void tui_wind_cl::disp_par() {
	int32_t n;

	int32_t buf_ind = 0;
	n = set_text_color2((uint8_t) text_colors_, &tmp_buf_[buf_ind]);
	buf_ind += n;
	n = set_bg2((uint8_t) bg_colors_, &tmp_buf_[buf_ind]);
	buf_ind += n;
	tx_fifo_->enqueue((uint8_t*) tmp_buf_, buf_ind);
	buf_ind = 0;

	for (uint64_t i = 0; i < par_vec_.size(); i++) {
		if (par_vec_[i] != nullptr) {
			par_vec_[i]->disp(x_, y_, (uint8_t) (bg_colors_), (uint8_t) (text_colors_));
		}
	}
}

inline void tui_wind_cl::reg_par(tui_par_cl* p) {
	par_vec_.push_back(p);
}

inline void tui_wind_cl::sel_down_par(void) {
	if (par_vec_.size() == 0) {
		return;
	}

	par_vec_[sel_par_ind_]->set_inv(false);
	par_vec_[sel_par_ind_]->disp_req();
	sel_par_ind_ = sel_tbl_down_[sel_par_ind_];
	par_vec_[sel_par_ind_]->set_inv(true);
	par_vec_[sel_par_ind_]->disp_req();
	digit_weight_ = 0;
	prohibit_idecr_ = false;
}

inline void tui_wind_cl::sel_left_par(void) {
	if (par_vec_.size() == 0) {
		return;
	}
	par_vec_[sel_par_ind_]->set_inv(false);
	par_vec_[sel_par_ind_]->disp_req();
	sel_par_ind_ = sel_tbl_left_[sel_par_ind_];
	par_vec_[sel_par_ind_]->set_inv(true);
	par_vec_[sel_par_ind_]->disp_req();
	digit_weight_ = 0;
	prohibit_idecr_ = false;
}

inline void tui_wind_cl::sel_right_par(void) {
	if (par_vec_.size() == 0) {
		return;
	}
	par_vec_[sel_par_ind_]->set_inv(false);
	par_vec_[sel_par_ind_]->disp_req();
	sel_par_ind_ = sel_tbl_right_[sel_par_ind_];
	par_vec_[sel_par_ind_]->set_inv(true);
	par_vec_[sel_par_ind_]->disp_req();
	digit_weight_ = 0;
	prohibit_idecr_ = false;
}

inline void tui_wind_cl::sel_tab_par(void) {
	if (par_vec_.size() == 0) {
		return;
	}

	if (tab_sel_def_) {
		sel_par_ind_ = def_sel_ind_;
	}

	par_vec_[sel_par_ind_]->set_inv(true);
	par_vec_[sel_par_ind_]->disp_req();
	digit_weight_ = 0;
	prohibit_idecr_ = false;

}

inline void tui_wind_cl::sel_act_par(void) {
	if (par_vec_.size() == 0) {
		return;
	}
	if (act_sel_def_) {
		sel_par_ind_ = def_sel_ind_;
	}
	par_vec_[sel_par_ind_]->set_inv(true);
	par_vec_[sel_par_ind_]->disp_req();
	digit_weight_ = 0;
	prohibit_idecr_ = false;
}

inline void tui_wind_cl::sel_up_par(void) {
	if (par_vec_.size() == 0) {
		return;
	}
	par_vec_[sel_par_ind_]->set_inv(false);
	par_vec_[sel_par_ind_]->disp_req();

	sel_par_ind_ = sel_tbl_up_[sel_par_ind_];

	par_vec_[sel_par_ind_]->set_inv(true);
	par_vec_[sel_par_ind_]->disp_req();
	digit_weight_ = 0;
	prohibit_idecr_ = false;
}

inline void tui_wind_cl::desel_par(void) {
	if (par_vec_.size() == 0) {
		return;
	}
	par_vec_[sel_par_ind_]->set_inv(false);
	par_vec_[sel_par_ind_]->disp_req();
	digit_weight_ = 0;
	prohibit_idecr_ = false;
}

inline void tui_wind_cl::incr_num(void) {
	if (par_vec_.size() == 0) {
		return;
	}

	if (!prohibit_idecr_) {
		par_vec_[sel_par_ind_]->incr_num();
		par_vec_[sel_par_ind_]->disp_req();
	}
}

inline void tui_wind_cl::decr_num(void) {
	if (par_vec_.size() == 0) {
		return;
	}
	if (!prohibit_idecr_) {
		par_vec_[sel_par_ind_]->decr_num();
		par_vec_[sel_par_ind_]->disp_req();
	} else {
		par_vec_[sel_par_ind_]->change_sigh();
		par_vec_[sel_par_ind_]->disp_req();
	}
}

inline void tui_wind_cl::set_digit_in_par(int32_t digit) {
	if (par_vec_.size() == 0) {
		return;
	}

	if ((par_vec_[sel_par_ind_]->get_cfg() & (tui_par_cl::CFG::TXT | tui_par_cl::CFG::CNTRLBL | tui_par_cl::CFG::BOOLT)) == (tui_par_cl::NUM | tui_par_cl::CFG::CNTRLBL | tui_par_cl::CFG::N_BOOLT)) {
		par_vec_[sel_par_ind_]->set_num(par_vec_[sel_par_ind_]->get_num() * digit_weight_ + digit);
		par_vec_[sel_par_ind_]->disp_req();
		digit_weight_ = 10;
		prohibit_idecr_ = true;
	}

}

inline void tui_wind_cl::reset_par(void) {
	if (par_vec_.size() == 0) {
		return;
	}
	if ((par_vec_[sel_par_ind_]->get_cfg() & (tui_par_cl::CFG::TXT | tui_par_cl::CFG::CNTRLBL | tui_par_cl::CFG::BOOLT)) == (tui_par_cl::NUM | tui_par_cl::CFG::CNTRLBL | tui_par_cl::CFG::N_BOOLT)) {
		par_vec_[sel_par_ind_]->set_num(0);
		par_vec_[sel_par_ind_]->disp_req();
		digit_weight_ = 0;
		prohibit_idecr_ = false;
	}
}

inline void tui_wind_cl::req_disp_all_par(void) {
	//printf("par_vec_.size(): %d\n", par_vec_.size());
	for (int32_t i = 0; i < (int32_t) (par_vec_.size()); i++) {
		par_vec_[i]->disp_req();
	}
}

inline void tui_wind_cl::req_act(void) {
	activate_fl_ = true;
}

inline bool tui_wind_cl::chk_activ(void) {
	return activate_fl_;
}

inline void tui_wind_cl::rst_req_act(void) {
	activate_fl_ = false;
}

inline bool tui_wind_cl::invoke_enter(void) {
	if (par_vec_.size() == 0) {
		return true;
	}
	return par_vec_[sel_par_ind_]->invoke_enter_cb();
}

inline bool tui_wind_cl::invoke_esc(void) {
	if (par_vec_.size() == 0) {
		return true;
	}
	return par_vec_[sel_par_ind_]->invoke_esc_cb();
}

inline bool tui_wind_cl::invoke_help(void) {
	if (par_vec_.size() == 0) {
		return true;
	}
	return par_vec_[sel_par_ind_]->invoke_help_cb();
}

inline void tui_wind_cl::calc_sel_tbl(void) {
	//create table, that help to move cursor to the new parameter field
	sel_tbl_right_.clear();
	int32_t next_ind = 0;
	int32_t delta_x = 0, delta_prev = 2147483647;
	int32_t delta_y = 0, delta_1_prev = 2147483647;
	for (uint64_t i = 0; i < par_vec_.size(); i++) {
		next_ind = i;
		delta_prev = 2147483647;
		delta_1_prev = 2147483647;
		for (uint64_t j = 0; j < par_vec_.size(); j++) {
			if ((j != i) && ((par_vec_[j]->get_cfg() & tui_par_cl::CFG::SLCTBL) == tui_par_cl::CFG::SLCTBL)) {
				if (par_vec_[j]->x_coord() > par_vec_[i]->x_coord()) {
					delta_x = par_vec_[j]->x_coord() - par_vec_[i]->x_coord();
					delta_y = par_vec_[j]->y_coord() - par_vec_[i]->y_coord();
					int32_t vec_len = delta_y * delta_y + delta_x * delta_x;

					if (vec_len < delta_1_prev) {
						delta_1_prev = vec_len;
						next_ind = j;
					}
				}
			}
		}
		sel_tbl_right_.push_back(next_ind);
		//printf("right_.next_ind: %d, fl: %d\n\r", next_ind, fl);
	}

	for (uint64_t i = 0; i < par_vec_.size(); i++) {
		next_ind = i;
		delta_prev = 2147483647;
		delta_1_prev = 2147483647;
		for (uint64_t j = 0; j < par_vec_.size(); j++) {
			if ((j != i) && ((par_vec_[j]->get_cfg() & tui_par_cl::CFG::SLCTBL) == tui_par_cl::CFG::SLCTBL)) {
				if (par_vec_[j]->x_coord() > par_vec_[i]->x_coord()) {
					delta_x = par_vec_[j]->x_coord() - par_vec_[i]->x_coord();
					delta_y = par_vec_[j]->y_coord() - par_vec_[i]->y_coord();
					int32_t vec_len = delta_y * delta_y + delta_x * delta_x;

					if (delta_y == 0) {
						if (vec_len < delta_1_prev) {
							delta_1_prev = vec_len;
							next_ind = j;
							sel_tbl_right_[i] = next_ind;
						}
					}

				}
			}
		}
		//printf("right_.next_ind: %d, fl: %d\n\r", next_ind, fl);
	}
	//------------------------------------------------------------------
	sel_tbl_left_.clear();
	for (uint64_t i = 0; i < par_vec_.size(); i++) {
		next_ind = i;
		delta_prev = 2147483647;
		delta_1_prev = 2147483647;
		for (uint64_t j = 0; j < par_vec_.size(); j++) {
			if ((j != i) && ((par_vec_[j]->get_cfg() & tui_par_cl::CFG::SLCTBL) == tui_par_cl::CFG::SLCTBL)) {
				if (par_vec_[j]->x_coord() < par_vec_[i]->x_coord()) {
					delta_x = par_vec_[j]->x_coord() - par_vec_[i]->x_coord();
					delta_y = par_vec_[j]->y_coord() - par_vec_[i]->y_coord();
					int32_t vec_len = delta_y * delta_y + delta_x * delta_x;
					if (vec_len < delta_1_prev) {
						delta_1_prev = vec_len;
						next_ind = j;
					}
				}
			}
		}
		sel_tbl_left_.push_back(next_ind);
		//printf("left_.next_ind: %d, fl: %d\n\r", next_ind, fl);
	}
	for (uint64_t i = 0; i < par_vec_.size(); i++) {
		next_ind = i;
		delta_prev = 2147483647;
		delta_1_prev = 2147483647;
		for (uint64_t j = 0; j < par_vec_.size(); j++) {
			if ((j != i) && ((par_vec_[j]->get_cfg() & tui_par_cl::CFG::SLCTBL) == tui_par_cl::CFG::SLCTBL)) {
				if (par_vec_[j]->x_coord() < par_vec_[i]->x_coord()) {
					delta_x = par_vec_[j]->x_coord() - par_vec_[i]->x_coord();
					delta_y = par_vec_[j]->y_coord() - par_vec_[i]->y_coord();
					int32_t vec_len = delta_y * delta_y + delta_x * delta_x;
					if (delta_y == 0) {
						if (vec_len < delta_1_prev) {
							delta_1_prev = vec_len;
							next_ind = j;
							sel_tbl_left_[i] = next_ind;
						}
					}
				}
			}
		}
		//printf("left_.next_ind: %d, fl: %d\n\r", next_ind, fl);
	}
	//------------------------------------------------------------------
	sel_tbl_up_.clear();
	for (uint64_t i = 0; i < par_vec_.size(); i++) {
		next_ind = i;
		delta_prev = 2147483647;
		delta_1_prev = 2147483647;
		for (uint64_t j = 0; j < par_vec_.size(); j++) {
			if ((j != i) && ((par_vec_[j]->get_cfg() & tui_par_cl::CFG::SLCTBL) == tui_par_cl::CFG::SLCTBL)) {
				if (par_vec_[j]->y_coord() < par_vec_[i]->y_coord()) {
					delta_x = par_vec_[j]->x_coord() - par_vec_[i]->x_coord();
					delta_y = par_vec_[j]->y_coord() - par_vec_[i]->y_coord();
					int32_t vec_len = delta_y * delta_y + delta_x * delta_x;
					if (vec_len < delta_1_prev) {
						delta_1_prev = vec_len;
						next_ind = j;
					}
				}
			}
		}
		sel_tbl_up_.push_back(next_ind);
		//printf("up.next_ind: %d, fl: %d\n\r", next_ind, fl);
	}
	for (uint64_t i = 0; i < par_vec_.size(); i++) {
		next_ind = i;
		delta_prev = 2147483647;
		delta_1_prev = 2147483647;
		for (uint64_t j = 0; j < par_vec_.size(); j++) {
			if ((j != i) && ((par_vec_[j]->get_cfg() & tui_par_cl::CFG::SLCTBL) == tui_par_cl::CFG::SLCTBL)) {
				if (par_vec_[j]->y_coord() < par_vec_[i]->y_coord()) {
					delta_x = par_vec_[j]->x_coord() - par_vec_[i]->x_coord();
					delta_y = par_vec_[j]->y_coord() - par_vec_[i]->y_coord();
					int32_t vec_len = delta_y * delta_y + delta_x * delta_x;
					if (delta_x == 0) {
						if (vec_len < delta_1_prev) {
							delta_1_prev = vec_len;
							next_ind = j;
							sel_tbl_up_[i] = next_ind;
						}
					}
				}
			}
		}

		//printf("up.next_ind: %d, fl: %d\n\r", next_ind, fl);
	}
	//------------------------------------------------------------------
	sel_tbl_down_.clear();
	for (uint64_t i = 0; i < par_vec_.size(); i++) {
		next_ind = i;
		delta_prev = 2147483647;
		delta_1_prev = 2147483647;
		for (uint64_t j = 0; j < par_vec_.size(); j++) {
			if ((j != i) && ((par_vec_[j]->get_cfg() & tui_par_cl::CFG::SLCTBL) == tui_par_cl::CFG::SLCTBL)) {
				if (par_vec_[j]->y_coord() > par_vec_[i]->y_coord()) {
					delta_x = par_vec_[j]->x_coord() - par_vec_[i]->x_coord();
					delta_y = par_vec_[j]->y_coord() - par_vec_[i]->y_coord();
					int32_t vec_len = delta_y * delta_y + delta_x * delta_x;
					if (vec_len < delta_1_prev) {
						delta_1_prev = vec_len;
						next_ind = j;
					}
				}
			}
		}
		sel_tbl_down_.push_back(next_ind);
		//printf("down.next_ind: %d, fl: %d\n\r", next_ind, fl);
	}
	for (uint64_t i = 0; i < par_vec_.size(); i++) {
		next_ind = i;
		delta_prev = 2147483647;
		delta_1_prev = 2147483647;
		for (uint64_t j = 0; j < par_vec_.size(); j++) {
			if ((j != i) && ((par_vec_[j]->get_cfg() & tui_par_cl::CFG::SLCTBL) == tui_par_cl::CFG::SLCTBL)) {
				if (par_vec_[j]->y_coord() > par_vec_[i]->y_coord()) {
					delta_x = par_vec_[j]->x_coord() - par_vec_[i]->x_coord();
					delta_y = par_vec_[j]->y_coord() - par_vec_[i]->y_coord();
					int32_t vec_len = delta_y * delta_y + delta_x * delta_x;
					if (delta_x == 0) {
						if (vec_len < delta_1_prev) {
							delta_1_prev = vec_len;
							next_ind = j;
							sel_tbl_down_[i] = next_ind;
						}
					}
				}
			}
		}
		//printf("down.next_ind: %d, fl: %d\n\r", next_ind, fl);
	}
	//-------------------------------------------------
	//Select default parameter for selecting
	delta_prev = 2147483647;
	for (uint64_t i = 0; i < par_vec_.size(); i++) {
		if ((par_vec_[i]->get_cfg() & tui_par_cl::CFG::SLCTBL) == tui_par_cl::CFG::SLCTBL) {
			delta_x = par_vec_[i]->x_coord() + par_vec_[i]->y_coord();
			if (delta_x < delta_prev) {
				delta_prev = delta_x;
				def_sel_ind_ = i;
				sel_par_ind_ = def_sel_ind_;
			}
		}
	}
	//Reselect default parameter for selecting if if set outside
	for (uint64_t i = 0; i < par_vec_.size(); i++) {
		if ((par_vec_[i]->get_cfg() & tui_par_cl::CFG::SLCTBL) == tui_par_cl::CFG::SLCTBL) {

			if (par_vec_[i]->get_def_sel()) {
				def_sel_ind_ = i;
				sel_par_ind_ = def_sel_ind_;
				break;
			}

		}
	}

}

inline void tui_wind_cl::shadow(bool s) {
	shadow_ = s;
}

inline void tui_wind_cl::set_tx_fifo_ptr(tui_fifo_cl<TUI_TX_FIFO_LEN>* t) {
	tx_fifo_ = t;
	for (int32_t i = 0; i < (int32_t) ((par_vec_.size())); i++) {
		par_vec_[i]->set_tx_fifo_ptr(t);
	}
}

inline void tui_wind_cl::set_tab_sel_def(bool p) {
	tab_sel_def_ = p;
}

inline void tui_wind_cl::set_act_sel_def(bool p) {
	act_sel_def_ = p;
}

inline bool tui_wind_cl::get_tab_sel_def(void) {
	return tab_sel_def_;
}

inline bool tui_wind_cl::get_act_sel_def(void) {
	return act_sel_def_;
}

inline void tui_wind_cl::change_bool_num(void) {
	if (par_vec_.size() == 0) {
		return;
	}
	if (par_vec_[sel_par_ind_]->get_num() >= 1) {
		par_vec_[sel_par_ind_]->set_num_to_0();
		par_vec_[sel_par_ind_]->disp_req();
	} else {
		par_vec_[sel_par_ind_]->set_num_to_1();
		par_vec_[sel_par_ind_]->disp_req();
	}
}

inline void tui_wind_cl::framing(bool f) {
	framing_ = f;
}

#endif /* TUI_WIND_CL_H_ */
