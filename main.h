/*
	BipCounter for Amazfit Bip BipOS
	(C) erekoze 2020 https://myamazfit.ru/members/erekoze.16826/
	(C) ugers 2020
	
	Приложение Счётчик для BipOS
	
*/

#include "libbip.h"
#ifndef __APP_TEMPLATE_H__
#define __APP_TEMPLATE_H__

// структура данных для нашего экрана
struct app_data_ {
		void* 	ret_f;					//	адрес функции возврата
		Elf_proc_* 	proc;				//	указатель на данные запущенного процесса
		int		col;
		int		timeout;
};


void 	show_screen (void *return_screen);
void 	key_press_screen();
int 	dispatch_screen (void *param);
void 	screen_job();
void	draw_screen(int col);
#endif