/*
 * tui_cl.h
 *
 *  Created on: 24 dec 2019
 *      Author: doctBrown
 */

#ifndef TUI_CL_H_
#define TUI_CL_H_

#include "tui_par_cl.h"
#include "tui_wind_cl.h"
#include "stdint.h"
#include "tui_common.h"
#include "string.h"
#include <new>
#include <vector>
#include "tui_fifo_cl.h"

/* Text User Interface or TUI is simple library for construction windows-text-like interface, consisted of layers, windows and parameters.
 * Originally this library created like simple, self-sufficient, fast enough and old-school solution for debugging and interaction with an embedded device through UART or telnet terminal.
 * The library don't use any third party library, only couple function for num to string conversation from Milo Yip see https://github.com/miloyip/itoa-benchmark (don't hit hard, there MIT)
 * and can be run probably in any OS or without it (we use ARM cortex-A9 (Zynq-7000) without any OS and work well). This feature is pretty useful, because you can construct and
 * debug interface in f. ex. in Windows (through Windows cmd.exe terminal) and then transfer code in your embedded system.
 *
 * TUI includes parameters, windows and layers. Parameters is minimal displaying unit, that can be numeric (integer or real), textual or boolean (displaying as 0 or 1).
 * Parameters registered in windows. Windows registered in layer. In one moment displaying only one layer with some windows with some parameters.
 * Sending graphic data performs through integrated FIFO, user should periodically check data availability in FIFO and send them to the terminal.
 * Receiving keystroke data perform through simple buffer, method that perform this task try to decode first bytes of input buffer (but not more than 10 bytes)
 * into the pressing corresponding key and ignore all another bytes.
 *
 * Also the library includes  engine that perform some tasks under layers, windows and parameters:
 *
 * - keystroke handling;
 * - displaying of active layer with its windows and parameters;
 * - monitors parameter changes and displaying only parameters, that had changed (for data flow decreasing);
 * - etc.
 *
 * Library features:
 * - under construction.
 *
 * Library support maximum 10 layer, but you can change this parameter, see const MAX_N_LAYERS in tui_cl.
 *
 * How to use (see main example):
 * Using tui library performed through inheritance of tui_cl class.
 * In inheritor you should declare all needed windows (tui_wind_cl) and parameters (tui_par_cl).
 * Parameter is binded to the window through method reg_par(...). Recommended first initialize windows and parameters and then perform registration.
 * Parameter can be registered only in one window.
 * Windows registered on the layer can be registered only in one layer.
 * Every layer has his own layer_id from 0 to 9 (for MAX_N_LAYERS = 10)
 * Layer with layer_id = 0 displayed in the first order by default.
 * In one layer can be registered any number of windows.
 * Interaction between main program and tui performs through direct access to windows and parameters declared in public region of inheritor.
 * For every parameter can be assigned some callback-functions for elaboration events such as "Enter", "Esc", "F1" - pressing, changing of
 * parameter and selecting (cursor set), see main_example.
 *
 */

class tui_cl {

public:

	enum class KB_DECODER_TYPE {
		WINDOWS = 0, TELNET = 1,
	};

	virtual ~tui_cl() { // under development
	}

	const static int32_t MAX_N_LAYERS = 10;

	/*should be called before any another methods*/
	void init();

protected:
	/* Method for registering windows, should be called last in  constract_tui(...) for every used tui_wind_cl declaration*/
	void reg_wind(tui_wind_cl* p, int32_t layer_id);
public:
	/* Responsible for displaying parameters */
	void disp_cntxt();

	/* Responsible for handling keystrokes
	 * You can sand any number of byte, but amount byte corresponding of 1 any keyboard key is more preferable
	 * Method will elaborate only one key, and if there is more than "1 key" byte, these byte will be ignored
	 */
	void kb_cntxt(char *buf, int32_t len);

	/* Check sometimes, because FIFO limited in deep (about 10 K) */
	int32_t byte_in_tx_buf(void);

	/* Copy len byte in *ptr
	 * Don't request more than byte_in_tx_buf(...)
	 */
	int32_t get_tx_data(uint8_t* ptr, int32_t len);

	/* This function will try to negotiate with remote console
	 * This function will write necessary command in tx_fifo_.
	 * Call this function just after new connection
	 */
	void req_telnet_negot(void);

	/* Set keyboard decoder type, call after init, or decoder will be always for windows stdio
	 *
	 */
	void set_kb_decoder(KB_DECODER_TYPE t) {
		kb_decoder_type_ = t;
	}

private:

	enum class KB_CODES {
		UNDEF = -1,
		NUM_0 = 0,
		NUM_1 = 1,
		NUM_2 = 2,
		NUM_3 = 3,
		NUM_4 = 4,
		NUM_5 = 5,
		NUM_6 = 6,
		NUM_7 = 7,
		NUM_8 = 8,
		NUM_9 = 9,
		ESC = 10,
		TAB = 11,
		ENTER = 12,
		SPACE = 13,
		MINUS = 14,
		PLUS = 15,
		BSPACE = 16,
		TILDE = 17,
		ARROW_LEFT = 18,
		ARROW_RIGHT = 19,
		ARROW_DOWN = 20,
		ARROW_UP = 21,
		F1_HELP = 22,
	};

	std::vector<tui_wind_cl*> wind_vec_[MAX_N_LAYERS];
	tui_wind_cl wind_tmp_;
	tui_wind_cl *cursor_wind_ = &wind_tmp_;
	int32_t act_layer_id_;

	int32_t cursor_wind_ind_;
	bool fl_layer_changed_;

	int32_t protect_cnt_;
	int32_t digit_weight_;

	tui_fifo_cl<TUI_TX_FIFO_LEN> tx_fifo_;

	char tmp_buf_[50];

	KB_DECODER_TYPE kb_decoder_type_;

	bool kb_decoder(int32_t* code, char *buf, int32_t len);
	bool kb_decoder_win(int32_t* code, char *buf, int32_t len);
};

inline void tui_cl::disp_cntxt() {
	int32_t n, buf_ind;
	//Check all layers and windows for activation request
	for (int32_t i = 0; i < MAX_N_LAYERS; i++) {
		//TODO not optimal solution, think about it
		for (uint64_t j = 0; j < wind_vec_[i].size(); j++) {
			if (wind_vec_[i][j]->chk_activ()) {
				wind_vec_[i][j]->rst_req_act();
				fl_layer_changed_ = true;
				act_layer_id_ = (int32_t) (i);
				cursor_wind_->desel_par();
				cursor_wind_ind_ = 0;
				cursor_wind_ = wind_vec_[act_layer_id_][cursor_wind_ind_];

				cursor_wind_->sel_act_par();

				break;
			}
			if (fl_layer_changed_) {
				break;
			}
		}
	}
	//Layer displaying
	if (fl_layer_changed_) {
		fl_layer_changed_ = false;
		for (uint64_t i = 0; i < wind_vec_[act_layer_id_].size(); i++) {

			buf_ind = 0;
			n = rst_scr2(&tmp_buf_[buf_ind]);
			buf_ind += n;
			n = cursoroff2(&tmp_buf_[buf_ind]);
			buf_ind += n;
			tx_fifo_.enqueue((uint8_t*) tmp_buf_, buf_ind);

			wind_vec_[act_layer_id_][i]->disp_frame();
			wind_vec_[act_layer_id_][i]->req_disp_all_par();
			wind_vec_[act_layer_id_][i]->disp_par();
		}
	} else {
		for (uint64_t i = 0; i < wind_vec_[act_layer_id_].size(); i++) {
			wind_vec_[act_layer_id_][i]->disp_par();
		}
	}

}

inline void tui_cl::kb_cntxt(char *buf, int32_t len) {
	int32_t code;
	bool ret = false;

	if (kb_decoder_type_ == KB_DECODER_TYPE::TELNET) {
		ret = kb_decoder(&code, buf, len);
	} else if (kb_decoder_type_ == KB_DECODER_TYPE::WINDOWS) {
		ret = kb_decoder_win(&code, buf, len);
	}

	if (ret) {
		if ((code <= 9) && (code >= 0)) {
			cursor_wind_->set_digit_in_par(code);
		} else {
			switch (code) {
			case (int32_t) (KB_CODES::ESC):
				cursor_wind_->invoke_esc();
				break;
			case (int32_t) (KB_CODES::TAB):
				cursor_wind_->desel_par();
				cursor_wind_ind_++;
				if (cursor_wind_ind_ >= (int32_t) ((wind_vec_[act_layer_id_].size()))) {
					cursor_wind_ind_ = 0;
				}
				//printf("sel_wind_ind_: %d\n", cursor_wind_ind_);
				cursor_wind_ = wind_vec_[act_layer_id_][cursor_wind_ind_];
				cursor_wind_->sel_tab_par();

				break;
			case (int32_t) (KB_CODES::ENTER):
				cursor_wind_->invoke_enter();
				break;
			case (int32_t) (KB_CODES::SPACE):
				cursor_wind_->change_bool_num();
				break;
			case (int32_t) (KB_CODES::MINUS):
				cursor_wind_->decr_num();
				break;
			case (int32_t) (KB_CODES::PLUS):
				cursor_wind_->incr_num();
				break;
			case (int32_t) (KB_CODES::BSPACE):
				cursor_wind_->reset_par();
				break;
			case (int32_t) (KB_CODES::TILDE):
				int32_t n, buf_ind;
				buf_ind = 0;
				n = rst_scr2(&tmp_buf_[buf_ind]);
				buf_ind += n;
				n = cursoroff2(&tmp_buf_[buf_ind]);
				buf_ind += n;
				n = clrscr2(&tmp_buf_[buf_ind]);
				buf_ind += n;
				tx_fifo_.enqueue((uint8_t*) tmp_buf_, buf_ind);

				for (uint64_t i = 0; i < wind_vec_[act_layer_id_].size(); i++) {
					wind_vec_[act_layer_id_][i]->disp_frame();
					wind_vec_[act_layer_id_][i]->req_disp_all_par();
				}

				break;
			case (int32_t) (KB_CODES::ARROW_LEFT):
				cursor_wind_->sel_left_par();
				break;
			case (int32_t) (KB_CODES::ARROW_RIGHT):
				cursor_wind_->sel_right_par();
				break;
			case (int32_t) (KB_CODES::ARROW_DOWN):
				cursor_wind_->sel_down_par();
				break;
			case (int32_t) (KB_CODES::ARROW_UP):
				cursor_wind_->sel_up_par();
				break;
			case (int32_t) (KB_CODES::F1_HELP):
				cursor_wind_->invoke_help();
				break;
			default:
				break;
			}
		}
	}
}

inline int32_t tui_cl::byte_in_tx_buf(void) {
	return tx_fifo_.byte_in_FIFO();
}

inline int32_t tui_cl::get_tx_data(uint8_t* ptr, int32_t len) {
	return tx_fifo_.dequeue(ptr, len);
}

inline void tui_cl::req_telnet_negot(void) {
	int32_t buf_ind;
	/*ELNET protocol negotiation*/
	buf_ind = 0;
	tmp_buf_[buf_ind] = 255;
	buf_ind++;
	tmp_buf_[buf_ind] = 253;
	buf_ind++;
	tmp_buf_[buf_ind] = 3;
	buf_ind++;
	tx_fifo_.enqueue((uint8_t*) (tmp_buf_), buf_ind);
	buf_ind = 0;
	tmp_buf_[buf_ind] = 255;
	buf_ind++;
	tmp_buf_[buf_ind] = 254;
	buf_ind++;
	tmp_buf_[buf_ind] = 1;
	buf_ind++;
	tx_fifo_.enqueue((uint8_t*) (tmp_buf_), buf_ind);
	buf_ind = 0;
	tmp_buf_[buf_ind] = 255;
	buf_ind++;
	tmp_buf_[buf_ind] = 254;
	buf_ind++;
	tmp_buf_[buf_ind] = 34;
	buf_ind++;
	tx_fifo_.enqueue((uint8_t*) (tmp_buf_), buf_ind);
	buf_ind = 0;
	tmp_buf_[buf_ind] = 253;
	buf_ind++;
	tmp_buf_[buf_ind] = 254;
	buf_ind++;
	tmp_buf_[buf_ind] = 45;
	buf_ind++;
	tx_fifo_.enqueue((uint8_t*) (tmp_buf_), buf_ind);
	int32_t n;
	buf_ind = 0;
	n = rst_scr2(&tmp_buf_[buf_ind]);
	buf_ind += n;
	n = cursoroff2(&tmp_buf_[buf_ind]);
	buf_ind += n;
	n = clrscr2(&tmp_buf_[buf_ind]);
	buf_ind += n;
	tx_fifo_.enqueue((uint8_t*) (tmp_buf_), buf_ind);
	for (uint64_t i = 0; i < wind_vec_[act_layer_id_].size(); i++) {
		wind_vec_[act_layer_id_][i]->disp_frame();
		wind_vec_[act_layer_id_][i]->req_disp_all_par();
	}
}

inline bool tui_cl::kb_decoder(int32_t* code, char *buf, int32_t len) {
	*code = (int32_t) (KB_CODES::UNDEF);
	int32_t amnt_byte = 0, byte_extract = 0;
	uint8_t rx_byte[5]; //буффер на прием
	amnt_byte = len;
	if (amnt_byte > 0) {
		if (amnt_byte > 5) {
			byte_extract = 5;
			memcpy(rx_byte, buf, byte_extract);
		} else {
			//if (protect_cnt_ >= 5) {
			//	protect_cnt_ = 0;
			memcpy(rx_byte, buf, amnt_byte);
			byte_extract = amnt_byte;
			//					printf("%d: ", amnt_byte);
			//					for (int i = 0; i < amnt_byte; i++) {
			//						printf("0x%X : ", rx_byte[i]);
			//					}
			//
			//					printf("\n\r");
			//} else {
			//	protect_cnt_++;
			//	return false;
			//}
		}
		//rx_buf_reset();
		if (byte_extract == 1) {
			if ((rx_byte[0] == 0x1B) && (byte_extract == 1)) {
				//printf("ESC\n");
				*code = (int32_t) (KB_CODES::ESC);
			} else if ((rx_byte[0] == 0x9) && (byte_extract == 1)) {
				//printf("TAB\n");
				*code = (int32_t) (KB_CODES::TAB);
			} else if ((rx_byte[0] == 0xD) && (byte_extract == 1)) {
				//printf("ENTER1\n");
				*code = (int32_t) (KB_CODES::ENTER);
			} else if ((rx_byte[0] == 0x20) && (byte_extract == 1)) {
				//printf("SPACE\n");
				*code = (int32_t) (KB_CODES::SPACE);
			} else if ((rx_byte[0] == 0x2D)) {
				//printf("-\n");
				*code = (int32_t) (KB_CODES::MINUS);
			} else if ((rx_byte[0] == 0x3D) || (rx_byte[0] == 0x2B)) {
				//printf("+\n");
				*code = (int32_t) (KB_CODES::PLUS);
			} else if ((rx_byte[0] >= 0x30) && (rx_byte[0] <= 0x39)) {
				//printf("%d\n", rx_byte[0] - 0x30);
				*code = (int32_t) ((rx_byte[0] - 0x30));
			} else if (rx_byte[0] == 0x08) {
				//printf("backspace\n");
				*code = (int32_t) (KB_CODES::BSPACE);
			} else if (rx_byte[0] == 0x60) {
				//printf("tilde\n");
				*code = (int32_t) (KB_CODES::TILDE);
			}
		} else {

			if ((rx_byte[0] == 0xD) && (byte_extract == 2)) {
				*code = (int32_t) (KB_CODES::ENTER);
				//printf("ENTER2\n");
			}

			if ((rx_byte[0] == 0x1B) && (rx_byte[1] == 0x5B) && (byte_extract >= 3)) {
				if (rx_byte[2] == 0x44) {
					//printf("<-\n");
					*code = (int32_t) (KB_CODES::ARROW_LEFT);
				} else if (rx_byte[2] == 0x43) {
					//printf("->\n");
					*code = (int32_t) (KB_CODES::ARROW_RIGHT);
				} else if (rx_byte[2] == 0x42) {
					//printf("down\n");
					*code = (int32_t) (KB_CODES::ARROW_DOWN);
				} else if (rx_byte[2] == 0x41) {
					//printf("^\n");
					*code = (int32_t) (KB_CODES::ARROW_UP);
				}
			}
		}
	}
	return true;
}

inline bool tui_cl::kb_decoder_win(int32_t* code, char *buf, int32_t len) {
	*code = (int32_t) (KB_CODES::UNDEF);
	int32_t amnt_byte = 0, byte_extract = 0;
	uint8_t rx_byte[5];
	amnt_byte = len;
	if (amnt_byte > 0) {
		if (amnt_byte > 5) {
			byte_extract = 5;
			memcpy(rx_byte, buf, byte_extract);
		} else {
			//if (protect_cnt_ >= 5) {
			//	protect_cnt_ = 0;
			memcpy(rx_byte, buf, amnt_byte);
			byte_extract = amnt_byte;
			//					printf("%d: ", amnt_byte);
			//					for (int i = 0; i < amnt_byte; i++) {
			//						printf("0x%X : ", rx_byte[i]);
			//					}
			//
			//					printf("\n\r");
			//} else {
			//	protect_cnt_++;
			//	return false;
			//}
		}
		//rx_buf_reset();
		if (byte_extract == 1) {
			if ((rx_byte[0] == 0x1B) && (byte_extract == 1)) {
				//	printf("ESC\n");
				*code = (int32_t) (KB_CODES::ESC);
			} else if ((rx_byte[0] == 0x9) && (byte_extract == 1)) {
				//printf("TAB\n");
				*code = (int32_t) (KB_CODES::TAB);
			} else if ((rx_byte[0] == 0xD) && (byte_extract == 1)) {
				//printf("ENTER\n");
				*code = (int32_t) (KB_CODES::ENTER);
			} else if ((rx_byte[0] == 0x20) && (byte_extract == 1)) {
				//printf("SPACE\n");
				*code = (int32_t) (KB_CODES::SPACE);
			} else if ((rx_byte[0] == 0x2D)) {
				//printf("-\n");
				*code = (int32_t) (KB_CODES::MINUS);
			} else if ((rx_byte[0] == 0x3D) || (rx_byte[0] == 0x2B)) {
				//printf("+\n");
				*code = (int32_t) (KB_CODES::PLUS);
			} else if ((rx_byte[0] >= 0x30) && (rx_byte[0] <= 0x39)) {
				//printf("%d\n", rx_byte[0] - 0x30);
				*code = (int32_t) ((rx_byte[0] - 0x30));
			} else if (rx_byte[0] == 0x08) {
				//printf("backspace\n");
				*code = (int32_t) (KB_CODES::BSPACE);
			} else if (rx_byte[0] == 0x60) {
				//printf("tilde\n");
				*code = (int32_t) (KB_CODES::TILDE);
			}
		} else {
			if ((rx_byte[0] == 224)) {
				if (rx_byte[1] == 75) {
					//printf("<-\n");
					*code = (int32_t) (KB_CODES::ARROW_LEFT);
				} else if (rx_byte[1] == 77) {
					//printf("->\n");
					*code = (int32_t) (KB_CODES::ARROW_RIGHT);
				} else if (rx_byte[1] == 80) {
					//printf("down\n");
					*code = (int32_t) (KB_CODES::ARROW_DOWN);
				} else if (rx_byte[1] == 72) {
					//printf("^\n");
					*code = (int32_t) (KB_CODES::ARROW_UP);
				}
			} else if ((rx_byte[0] == 0)) {
				if (rx_byte[1] == 0x3B) {
					//	printf("F1\n");
					*code = (int32_t) (KB_CODES::F1_HELP);
				}
			}

		}
	}
	return true;
}

inline void tui_cl::init() {
	act_layer_id_ = 0;
	protect_cnt_ = 0;
	cursor_wind_ind_ = 0;
	fl_layer_changed_ = false;
	for (int32_t i = 0; i < MAX_N_LAYERS; i++) {
		wind_vec_[i].clear();
	}
	tx_fifo_.rst();
	kb_decoder_type_ = KB_DECODER_TYPE::WINDOWS;
}

inline void tui_cl::reg_wind(tui_wind_cl* p, int32_t layer_id) {
	if (layer_id <= MAX_N_LAYERS) {
		wind_vec_[layer_id].push_back(p);
		if (layer_id == 0) {
			cursor_wind_ = p;
		}

		p->set_tx_fifo_ptr(&tx_fifo_);

		p->calc_sel_tbl();
	}
}

#endif /* TUI_CL_H_ */
