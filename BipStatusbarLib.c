/*
	BipStatusbarLibrary for Amazfit Bip BipOS

	(C) ugers 2020
	
	Библиотека статусбар для BipOS
	
*/
#include "BipStatusbarLib.h"
//#define BipEmulator
#ifdef BipEmulator
	#include "libbip.h"
#else
	#include <libbip.h>
#endif

//ОПЦИИ
#define BATTERY_TEXT
#define BATTERY_ICON
#define TIME
#define DATA
#define BLUETOOTH

void show_statusbar(int y, int bgColor,int fgColor){
	set_bg_color(bgColor); // делаем фон
	set_fg_color(fgColor); // делаем фон
	draw_filled_rect_bg(0, 0, 176, y + 20);
	//БАТАРЕЯ
		if (get_fw_version() != 11536)
		{
			//set_bg_color(COLOR_YELLOW);
			//draw_filled_rect_bg(x + 2, y + 2, x + 21, y + 10);
		}else{
			#ifdef BipEmulator
					word battery_percentage = 80;
			#else
					word battery_percentage = *((word*)(0x20000334));
			#endif
				#ifdef BATTERY_ICON
				set_bg_color(COLOR_BLACK); // делаем фон
				draw_filled_rect_bg(148, y + 1, 172, y + 15); // сама батарейка
				draw_filled_rect_bg(170, y + 4, 174, y + 12); //кончик
				set_bg_color(COLOR_WHITE); // делаем фон
				draw_filled_rect_bg(149, y + 2, 171, y + 14); // сама батарейка
				draw_filled_rect_bg(171, y + 5, 173, y + 11); //кончик
				//Цвет индикатора батареи
				char r_count = battery_percentage / 20;
				r_count = r_count > 4 ? 4 : r_count < 1 ? 1 : r_count;
					if (battery_percentage > 20) {
						set_bg_color(battery_percentage <= 60 ? COLOR_YELLOW : COLOR_GREEN);
					}else if (battery_percentage <= 20) {
						set_bg_color(COLOR_RED);
					}

					for (char i = 0; i < r_count; i++)
					{
						draw_filled_rect_bg(151 + i * 5, y + 4, 154 + i * 5, y + 12);
					}
				#endif
			#ifdef BATTERY_TEXT
			//Проценты батареи
			char d1[4];     // переменная для перевода переменной для печати                               
			_sprintf(d1, "%01d", battery_percentage); // конвертируем
			set_bg_color(bgColor); // делаем фон
				#ifdef BATTERY_ICON
					text_out_center(d1, 134, y);
				#else
					text_out_center(d1, 146, y);
					text_out("%", 162, y);
				#endif
			#endif
		}
set_bg_color(bgColor); // делаем фон
#ifdef TIME
	// ВРЕМЯ
	char clock_time[8]; 			//	текст время		12:34
	struct datetime_ dt;
	get_current_date_time(&dt);
	_sprintf(clock_time, "%02d:%02d", dt.hour, dt.min);
	text_out_center(clock_time, 88,y);		// печатаем результат(время)
#endif
#ifdef DATA
	char data[12]; 			//	текст дата		день.месяц
	#ifndef TIME
		struct datetime_ dt;
		get_current_date_time(&dt);
	#endif
	_sprintf(data, "%02d.%02d", dt.day, dt.month);
	text_out(data,4,y);		// печатаем день недели
#endif
#ifdef BLUETOOTH
	// БЛЮТУЗ
	char last_bt_con;
	#ifdef BipEmulator
		last_bt_con = 1;
	#else
		last_bt_con = check_app_state(0x200);
	#endif
		set_bg_color(COLOR_BLACK);
		draw_filled_rect_bg(48, y-1, 62, y+17);
	if (last_bt_con){
		//paint bluetooth ON
		set_bg_color(COLOR_BLUE);
		set_fg_color(COLOR_WHITE);
	}else{			
		//paint bluetooth OFF
		set_bg_color(COLOR_RED);
		set_fg_color(COLOR_BLACK);
	}
	draw_filled_rect_bg(49, y, 61, y+16);
	text_out_center("ᛒ", 55, y);
#endif
// РАЗДЕЛИТЕЛЬ
	set_fg_color(fgColor); // делаем фон
	draw_horizontal_line(y+19, 0, 176);
	repaint_screen_lines(0, y+19); // обновляем строки экрана
}