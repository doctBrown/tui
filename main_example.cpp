#include <iostream>
#include "stdio.h"
#include "conio.h"
#include <stdio.h>
#include <tui_cl.h>
#include <tui_demo.h>
#include <wchar.h>
#include <windows.h>
#include <wincon.h>

/* For delay, see https://www.c-plusplus.net/forum/topic/109539/usleep-unter-windows */
void usleep(__int64 usec) {
	HANDLE timer;
	LARGE_INTEGER ft;
	ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time
	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}

tui_demo gui;
bool cnt_en = false;
int32_t cnt_int = 0;
double cnt_real = 0;
bool fl_prog_done = false;

/*Some callback functions*/
void start_cnt_cbf(int32_t par, void *p) {
	cnt_en = true;
}

void stop_cnt_cbf(int32_t par, void *p) {
	cnt_en = false;
}

void rst_cnt_cbf(int32_t par, void *p) {
	gui.WIND_RST.req_act();
}

void rst_cnt_cancel_cbf(int32_t par, void *p) {
	gui.WIND_0.req_act();
}

void rst_cnt_ok_cbf(int32_t par, void *p) {
	cnt_int = 0;
	gui.WIND_0.req_act();
}

void act_help_cbf(int32_t num, void *par) {
	gui.HELP_0_w.req_act();
}

void dact_help_cbf(int32_t num, void *par) {
	gui.WIND_0.req_act();
}

void exit_cbf(int32_t num, void *par) {
	fl_prog_done = true;
}

const static int32_t BUF_SIZE = 1000;
uint8_t buf_tx[BUF_SIZE];
int32_t amnt_tx = 0;
uint8_t buf_rx[BUF_SIZE];
int32_t amnt_rx = 0;

int main() {
	gui.init();
	gui.constract_tui();

	/* Just in case, request activation of main window - WIND_0.
	 * This will result in displaying all windows in layer, which include WIND_0
	 */
	gui.WIND_0.req_act();

	/* Set some callback-function for "Enter" and "Esc" elaboration
	 */
	gui.BUT_START_p.set_enter_cb(start_cnt_cbf, nullptr);
	gui.BUT_STOP_p.set_enter_cb(stop_cnt_cbf, nullptr);
	gui.BUT_RST_p.set_enter_cb(rst_cnt_cbf, nullptr);
	gui.CANCEL_p.set_enter_cb(rst_cnt_cancel_cbf, nullptr);
	gui.CANCEL_p.set_esc_cb(rst_cnt_cancel_cbf, nullptr);
	gui.OK_p.set_enter_cb(rst_cnt_ok_cbf, nullptr);
	gui.OK_p.set_esc_cb(rst_cnt_cancel_cbf, nullptr);

	/*We can set its own help for every selectable parameter
	 * In this example lets set one help for all parameters
	 */
	gui.HELP_str_0_p.set_help_cb(act_help_cbf, nullptr);
	gui.BUT_START_p.set_help_cb(act_help_cbf, nullptr);
	gui.BUT_STOP_p.set_help_cb(act_help_cbf, nullptr);
	gui.BUT_RST_p.set_help_cb(act_help_cbf, nullptr);
	gui.INT_NUM_p.set_help_cb(act_help_cbf, nullptr);
	gui.BOOL_NUM_p.set_help_cb(act_help_cbf, nullptr);
	gui.EXIT_p.set_help_cb(act_help_cbf, nullptr);

	gui.HELP_0_p.set_esc_cb(dact_help_cbf, nullptr);

	/* Set callback-function for for exit from program
	 */
	gui.EXIT_p.set_enter_cb(exit_cbf, nullptr);

	uint8_t tx_buf[4];

	while (1) {

		/* Context method of tui, the method must be called periodically
		 * In fact, this method monitors all changes in parameters and windows state (activate/deactivate),
		 * displays an active layer, etc.
		 */
		gui.disp_cntxt();

		/*Form buffer with keystroke data */
		for (int i = 0; i < 10; i++) {
			if (_kbhit()) {
				if (amnt_rx < BUF_SIZE) {
					buf_rx[amnt_rx] = getch();
					amnt_rx++;
				} else {
					amnt_rx = 0;
				}
			}
		}

		/* If there are some bytes in rx buffer */
		if (amnt_rx > 0) {
			if (amnt_rx <= 100) { //100 bytes? Too many keystrokes, ignore it
				gui.kb_cntxt((char*) buf_rx, amnt_rx);
//				printf("%d: ", amnt_rx);
//				for (int i = 0; i < amnt_rx; i++) {
//					printf("0x%X : ", buf_rx[i]);
//				}
//
//				printf("\n\r");
			}
			amnt_rx = 0;
		}

		/*Check if there is any bytes in output buffer*/
		amnt_tx = gui.byte_in_tx_buf();
		if (amnt_tx > 0) {
			for (int i = 0; i < amnt_tx; i++) {
				/*Since tui use FIFO we can extract any number of bytes (<= byte_in_tx_buf) in one function call*/
				gui.get_tx_data(tx_buf, 1);
				putchar((char) tx_buf[0]);
			}
		}

		/* Our main program, that count the counter with enable and reset)*/
		if (cnt_en) {
			cnt_int++;
		}
		gui.CNT_p.set_num(cnt_int);
		cnt_real = (double) cnt_int / 1000.0;
		gui.CNTr_p.set_real_num(cnt_real);

		/*Check if exit from program*/
		if (fl_prog_done) {
			break;
		}

		/*10 ms delay*/
		usleep(10000);

	}
	return 0;
}
