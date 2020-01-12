/*
 * tui_demo.h
 *
 *  Created on: 26 dec 2019
 *      Author: doctBrown
 */

#ifndef TUI_DEMO_H_
#define TUI_DEMO_H_

#include "tui/tui_cl.h"
#include "tui/tui_par_cl.h"

/* How to use (see main example):
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
 */
class tui_demo: public tui_cl {

public:

	/*Declare windows and parameters in public region of this class*/
	tui_wind_cl WIND_0;
	tui_wind_cl WIND_1;
	tui_wind_cl WIND_RST;
	tui_par_cl CNT_str_p, CNT_p, CNTr_p, BUT_START_p, BUT_STOP_p, BUT_RST_p;
	tui_par_cl OK_p, CANCEL_p, INFO_p;
	tui_par_cl TEXT1_p;
	tui_par_cl TEXT2_p;
	tui_par_cl INT_NUM_p;
	tui_par_cl BOOL_NUM_p;

	tui_wind_cl HELP_0_w;
	tui_par_cl HELP_0_p;
	tui_par_cl HELP_str_0_p;
	tui_par_cl EXIT_p;

	/*Construct your TUI here, you can declare any number of parameters and windows, but amount of layer is limited by MAX_N_LAYERS = 10*/
	void constract_tui() {

		/* First of all, set decode type*/
		set_kb_decoder(KB_DECODER_TYPE::WINDOWS);

		/* Lets customize some windows with few parameters*/
		WIND_0.init(0, 0, 40, 10, "COUNTER CONTROL", 15);
		WIND_0.set_bg_color(COLORS::BLUE);
		WIND_0.set_txt_color(COLORS::WHITE);
		WIND_0.set_act_sel_def(false);

		HELP_0_w.init(1, 1, 70, 14, "HELP", 4);
		HELP_0_w.framing(true);
		HELP_0_w.shadow(true);
		HELP_0_w.set_bg_color(COLORS::CYAN);
		HELP_0_w.set_txt_color(COLORS::BLACK);
		HELP_0_p.init(1, 1, 355, tui_par_cl::CFG::TXT);

		HELP_0_p.set_text((char*)"Use \"Arrows\" for navigation insight window.\n\r"
				"Use \"TAB\" for navigation between windows.\n\r"
				"Use \"SPACE\" for changing parameters type of bool.\n\r"
				"Use \"+\" or \"-\" or digital keys for changing parameters type of.\n\r"
				"integer\n\r"
				"Use \"ENTER\" for applying selected parameter or menu item.\n\r"
				"Use \"`\" for refreshing current layer of windows.\n\r"
				"Press \"ESC\" to close this window.", 355);

		HELP_0_w.reg_par(&HELP_0_p);

		CNT_str_p.init(10, 3, 22, tui_par_cl::CFG::TXT);
		CNT_str_p.set_text((char*)"INT CNT :\n\r"
								"REAL CNT:\n\r", 22);
		WIND_0.reg_par(&CNT_str_p);

		CNT_p.init(20, 3, 5, tui_par_cl::NUM);
		WIND_0.reg_par(&CNT_p);

		CNTr_p.init(20, 4, 8, tui_par_cl::NUM | tui_par_cl::REALT);
		CNTr_p.set_real_num_par(3, 4);
		WIND_0.reg_par(&CNTr_p);

		BUT_START_p.init(5, 6, 5, tui_par_cl::CFG::TXT | tui_par_cl::CFG::SLCTBL);
		BUT_START_p.set_text((char*)"START:", 5);
		WIND_0.reg_par(&BUT_START_p);

		BUT_STOP_p.init(15, 6, 5, tui_par_cl::CFG::TXT | tui_par_cl::CFG::SLCTBL);
		BUT_STOP_p.set_text((char*)"STOP", 4);
		WIND_0.reg_par(&BUT_STOP_p);

		BUT_RST_p.init(25, 6, 5, tui_par_cl::CFG::TXT | tui_par_cl::CFG::SLCTBL);
		BUT_RST_p.set_text((char*)"RESET:", 5);
		WIND_0.reg_par(&BUT_RST_p);

		HELP_str_0_p.init(1, 1, 20, tui_par_cl::CFG::TXT | tui_par_cl::CFG::SLCTBL);
		HELP_str_0_p.set_text((char*)"Press F1 to get help", 20);
		WIND_0.reg_par(&HELP_str_0_p);

		WIND_RST.init(33, 3, 16, 5, "RESET CNTL ?", 12);
		WIND_RST.set_bg_color(COLORS::RED);
		WIND_RST.shadow(true);
		OK_p.init(2, 4, 5, tui_par_cl::CFG::TXT | tui_par_cl::CFG::SLCTBL);
		OK_p.set_text((char*)"OK:", 2);
		WIND_RST.reg_par(&OK_p);

		CANCEL_p.init(7, 4, 6, tui_par_cl::CFG::TXT | tui_par_cl::CFG::SLCTBL);
		CANCEL_p.set_text((char*)"CANCEL", 6);
		CANCEL_p.set_def_sel(true);
		WIND_RST.reg_par(&CANCEL_p);

		INFO_p.init(2, 2, 15, tui_par_cl::CFG::TXT);
		INFO_p.set_text((char*)"RESET COUNTER ?", 15);
		WIND_RST.reg_par(&INFO_p);

		WIND_1.init(42, 0, 40, 10, "CONTROLABLE PARAMETERS", 22);

		TEXT1_p.init(1, 1, 107, tui_par_cl::CFG::TXT);
		TEXT1_p.set_text((char*)"You can enter integer number with help\n\r"
								"of number keys or \"+\", \"-\" keys   \n\r"
							    "Use \"SPACE\" to change bool type", 107);
		WIND_1.reg_par(&TEXT1_p);

		TEXT2_p.init(2, 5, 20, tui_par_cl::CFG::TXT);
		TEXT2_p.set_text((char*)"Integer:\n\r"
				"Boolean:\n\r", 20);
		WIND_1.reg_par(&TEXT2_p);

		INT_NUM_p.init(10, 5, 5, tui_par_cl::NUM | tui_par_cl::CFG::SLCTBL | tui_par_cl::CFG::CNTRLBL);
		WIND_1.reg_par(&INT_NUM_p);

		BOOL_NUM_p.init(10, 6, 5, tui_par_cl::BOOLT | tui_par_cl::SLCTBL | tui_par_cl::CFG::CNTRLBL);
		WIND_1.reg_par(&BOOL_NUM_p);

		EXIT_p.init(2, 8, 17, tui_par_cl::CFG::TXT | tui_par_cl::SLCTBL);
		EXIT_p.set_text((char*)"Exit from program", 17);
		WIND_0.reg_par(&EXIT_p);


		//call registration only after all parameter are registered
		reg_wind(&WIND_0, 0);
		reg_wind(&WIND_1, 0);
		reg_wind(&WIND_RST, 1);
		reg_wind(&HELP_0_w, 2);

	}

};

#endif /* TUI_DEMO_H_ */
