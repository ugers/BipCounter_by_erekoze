/*
	BipCounter for Amazfit Bip BipOS
	(C) erekoze  2020 https://myamazfit.ru/members/erekoze.16826/
	
	Приложение Счётчик для BipOS
	
*/
//#define BipEmulator
#ifdef BipEmulator
	#include "libbip.h"
#else
	#include <libbip.h>
#endif
#include "main.h"

//	структура меню экрана - для каждого экрана своя
struct regmenu_ screen_data = {
						55,							//	номер главного экрана, значение 0-255, для пользовательских окон лучше брать от 50 и выше
						1,							//	вспомогательный номер экрана (обычно равен 1)
						0,							//	0
						dispatch_screen,			//	указатель на функцию обработки тача, свайпов, долгого нажатия кнопки
						key_press_screen, 			//	указатель на функцию обработки нажатия кнопки
						screen_job,					//	указатель на функцию-колбэк таймера 
						0,							//	0
						show_screen,				//	указатель на функцию отображения экрана
						0,							//	
						0							//	долгое нажатие кнопки
					};

static int incr_no=0; //переменная инкрисмента

int main(int param0, char** argv){	//	здесь переменная argv не определена
	show_screen((void*) param0);
}

void show_screen (void *param0){
struct app_data_**  app_data_p = (struct app_data_ **)get_ptr_temp_buf_2();    //  указатель на указатель на данные экрана
struct app_data_ *    app_data;                    //    указатель на данные экрана

// проверка источника запуска процедуры
if ( (param0 == *app_data_p) && get_var_menu_overlay()){ // возврат из оверлейного экрана (входящий звонок, уведомление, будильник, цель и т.д.)

    app_data = *app_data_p;                    //    указатель на данные необходимо сохранить для исключения
                                            //    высвобождения памяти функцией reg_menu
	#ifdef BipEmulator
		*app_data_p = (app_data_*)NULL;						//	обнуляем указатель для передачи в функцию reg_menu
	#else
		*app_data_p = NULL;						//	обнуляем указатель для передачи в функцию reg_menu	
	#endif

    //     создаем новый экран, при этом указатель temp_buf_2 был равен 0 и память не была высвобождена 
    reg_menu(&screen_data, 0);                 //     menu_overlay=0
  
    *app_data_p = app_data;                        //    восстанавливаем указатель на данные после функции reg_menu
  
    //   здесь проводим действия при возврате из оверлейного экрана: восстанавливаем данные и т.д.
  
} else { // если запуск функции произошел впервые т.е. из меню

    // создаем экран (регистрируем в системе)
    reg_menu(&screen_data, 0);

    // выделяем необходимую память и размещаем в ней данные, (память по указателю, хранящемуся по адресу temp_buf_2 высвобождается автоматически функцией reg_menu другого экрана)
    *app_data_p = (struct app_data_ *)pvPortMalloc(sizeof(struct app_data_));
    app_data = *app_data_p;        //    указатель на данные
  
    // очистим память под данные
    _memclr(app_data, sizeof(struct app_data_));
  
    //    значение param0 содержит указатель на данные запущенного процесса структура Elf_proc_
	#ifdef BipEmulator
		app_data->proc = (Elf_proc_*)param0;
	#else
		app_data->proc = param0;
	#endif
  
    // запомним адрес указателя на функцию в которую необходимо вернуться после завершения данного экрана
    if ( param0 && app_data->proc->ret_f )             //    если указатель на возврат передан, то возвоащаемся на него
        app_data->ret_f = app_data->proc->elf_finish;
    else                    //    если нет, то на циферблат
        app_data->ret_f = show_watchface;
  
    // здесь проводим действия которые необходимо если функция запущена впервые из меню: заполнение всех структур данных и т.д.
    ElfReadSettings(app_data->proc->index_listed, &app_data->col, 0, sizeof(app_data->col)); // читаем данные из памяти
}

// экран гаснет после таймаута, загорается при нажатии на экран
set_display_state_value(8, 1);
set_display_state_value(2, 1);

		char* tips_string_ru[] = {
				 "-увел/умень", "-выбор/сброс", "-сохр&выйти"};
		char* tips_string_en[] = {
				 "-incr/decr", "-select/reset", "-save&exit"};
		char* tips_string_it[] = {
				"-incr/decr", "-selezi/ripris", "-salva&esci"}; //incremento/decremento selezionare/ripristinare salva/esci
		char* tips_string_fr[] = {
				"-incr/décr", "-sélect/reset", "-sauve&quit"}; //incrémenter/décrémenter sélectionner/réinitialiser sauvegarder/quitter
		char* tips_string_es[] = {
				"-incr/decr", "-selecc/restab", "-guardar&salir"}; //incrementar/decrementar seleccionar/restablecer guardar/salir
		char* tips_string_de[] = {
				"-inkr/dekr", "-auswäh/zurück", "-speic&schl"};//Inkrementieren/Dekrementieren auswählen/zurücksetzen  speichern/schließen
		char* tips_string_gr[] = {
				"-αύξη/μείω", "-επιλέξ/επανα", "-αποθή&έξοδος"}; //αύξηση/μείωση επιλέξτε/επαναφέρετε αποθήκευση/έξοδος
		char** tips_string;
		switch (get_selected_locale()) {
			case locale_ru_RU: {
				tips_string = tips_string_ru;
				break;
			}
			case locale_it_IT: {
				tips_string = tips_string_it;
				break;
			}
			case locale_fr_FR: {
				tips_string = tips_string_fr;
				break;
			}
			case locale_es_ES: {
				tips_string = tips_string_es;
				break;
			}
			case locale_de_DE: {
				tips_string = tips_string_de;
				break;
			}
			case locale_el_GR: {
				tips_string = tips_string_gr;
				break;
			}
			default: {
				tips_string = tips_string_en;
				break;
			}
		};

// здесь выполняем отрисовку интерфейса, обновление (перенос в видеопамять) экрана выполнять не нужно
set_bg_color(COLOR_BLACK); // делаем фон синим
fill_screen_bg(); // заливаем экран фоном
load_font(); // подгружаем шрифты
set_fg_color(COLOR_WHITE); // делаем текст белым
text_out_center("BipCounter v.2.2", 88, 3); // выводим заголовок
show_elf_res_by_id(app_data->proc->index_listed, 7, 40, 21); // рисуем (C)
text_out("AVBurkov", 62, 22); // выводим автора
draw_screen(app_data->col); // перерисоваваем экран
show_elf_res_by_id(app_data->proc->index_listed, 0, 7, 112); // рисуем руку
show_elf_res_by_id(app_data->proc->index_listed, 1, 28, 112); // рисуем стрелку вверх
text_out("/", 49, 115); // выводим разделитель
show_elf_res_by_id(app_data->proc->index_listed, 2, 59, 112); // рисуем стрелку вниз
text_out(tips_string[0], 80, 115); // выводим подсказку
show_elf_res_by_id(app_data->proc->index_listed, 3, 4, 133); // рисуем стрелку влево
text_out("/", 25, 136); // выводим разделитель
show_elf_res_by_id(app_data->proc->index_listed, 4, 35, 133); // рисуем стрелку вправо
text_out(tips_string[1], 55, 136); // выводим подсказку
show_elf_res_by_id(app_data->proc->index_listed, 5, 25, 154); // рисуем кнопку
text_out(tips_string[2], 46, 157); // выводим подсказку
// при необходимости ставим таймер вызова screen_job в мс
set_update_period(1, 3000); // при запуске ставим паузу 3000мс
}

void key_press_screen(){
struct app_data_**  app_data_p = (struct app_data_ **)get_ptr_temp_buf_2();    //  указатель на указатель на данные экрана
struct app_data_ *	app_data = *app_data_p;				//	указатель на данные экрана
ElfWriteSettings(app_data->proc->index_listed, &app_data->col, 0, sizeof(app_data->col)); // пишем данные в память при нажатии кнопки и выходе
// вызываем функцию возврата (обычно это меню запуска), в качестве параметра указываем адрес функции нашего приложения
show_menu_animate(app_data->ret_f, (unsigned int)show_screen, ANIMATE_RIGHT);	
};

void screen_job(){
// при необходимости можно использовать данные экрана в этой функции
struct app_data_**  app_data_p = (struct app_data_ **)get_ptr_temp_buf_2();    //  указатель на указатель на данные экрана
struct app_data_ *	app_data = *app_data_p;				//	указатель на данные экрана

// делаем периодическое действие: анимация, увеличение счетчика, обновление экрана,
// отрисовку интерфейса, обновление (перенос в видеопамять) экрана выполнять нужно
fill_screen_bg(); // заливаем экран фоном
draw_screen(app_data->col); // перерисоваваем экран
// при необходимости заново ставим таймер вызова screen_job
//set_update_period(1, 200); // при запуске ставим паузу после запуска ставим паузу 200мс
}

int dispatch_screen (void *param){
struct app_data_**  app_data_p = (struct app_data_ **)get_ptr_temp_buf_2();    //  указатель на указатель на данные экрана
struct app_data_ *	app_data = *app_data_p;				//	указатель на данные экрана

// в случае отрисовки интерфейса, обновление (перенос в видеопамять) экрана выполнять нужно

#ifdef BipEmulator
	 struct gesture_ *gest = (gesture_*) param;
#else
	 struct gesture_ *gest = param;
#endif
int result = 0;

switch (gest->gesture){
	case GESTURE_CLICK: { // тап по экрану
			// действия при тапе по экрану			
			vibrate(1,70,0); // вибрируем
			if (incr_no==0) app_data->col = app_data->col+1; // если выбрали единицы увеличиваем счетчик касаний на 1
			if (incr_no==1) app_data->col = app_data->col+10; // если выбрали десятки увеличиваем счетчик касаний на 10
			if (incr_no==2) app_data->col = app_data->col+100; // если выбрали сотни увеличиваем счетчик касаний на 100
			if (incr_no==3) app_data->col = app_data->col+1000; // если выбрали тысячи увеличиваем счетчик касаний на 1000
			if (incr_no==4) app_data->col = app_data->col+10000; // если выбрали десятки тысяч увеличиваем счетчик касаний на 10000
			if (incr_no==5) app_data->col = app_data->col+100000; // если выбрали сотни тысяч увеличиваем счетчик касаний на 100000
			if (incr_no==6) app_data->col = app_data->col+1000000; // если выбрали миллионы увеличиваем счетчик касаний на 1000000
			if (app_data->col>9999999) app_data->col = 9999999; // если привысили лимит разрядов, то не увеличиваем
			draw_screen(app_data->col); // перерисоваваем экран
			repaint_screen_lines(40, 110); // обновляем строки экрана
			break;
		};
		case GESTURE_SWIPE_RIGHT: {	//	свайп направо
			// действия при свайпе вправо
			vibrate(1,70,0); // вибрируем
			app_data->col = 0; // сбрасываем счетчик касаний до 0
			draw_screen(app_data->col); // перерисоваваем экран
			repaint_screen_lines(40, 110); // обновляем строки экрана
			break;
		};
		case GESTURE_SWIPE_LEFT: {	// справа налево
			// действия при свайпе влево	
			vibrate(1,70,0); // вибрируем
			incr_no++; // увеличиваем изменяемый разряд
			if (incr_no>6) incr_no=0; // если достигли последнего разряда переходим к первому разряду
			draw_screen(app_data->col); // перерисоваваем экран
			repaint_screen_lines(40, 110); // обновляем строки экрана
			break;
		};
		case GESTURE_SWIPE_UP: {	// свайп вверх
			// действия при свайпе вверх
			vibrate(1,70,0); // вибрируем
			if (incr_no==0) app_data->col = app_data->col+1; // если выбрали единицы увеличиваем счетчик касаний на 1
			if (incr_no==1) app_data->col = app_data->col+10; // если выбрали десятки увеличиваем счетчик касаний на 10
			if (incr_no==2) app_data->col = app_data->col+100; // если выбрали сотни увеличиваем счетчик касаний на 100
			if (incr_no==3) app_data->col = app_data->col+1000; // если выбрали тысячи увеличиваем счетчик касаний на 1000
			if (incr_no==4) app_data->col = app_data->col+10000; // если выбрали десятки тысяч увеличиваем счетчик касаний на 10000
			if (incr_no==5) app_data->col = app_data->col+100000; // если выбрали сотни тысяч увеличиваем счетчик касаний на 100000
			if (incr_no==6) app_data->col = app_data->col+1000000; // если выбрали миллионы увеличиваем счетчик касаний на 1000000
			if (app_data->col>9999999) app_data->col = 9999999; // если привысили лимит разрядов, то не увеличиваем
			draw_screen(app_data->col); // перерисоваваем экран
			repaint_screen_lines(40, 110); // обновляем строки экрана
			break;
		};
		case GESTURE_SWIPE_DOWN: {	// свайп вниз
			// действия при свайпе вниз
			if (incr_no==0) {
				app_data->col = app_data->col-1; // если выбрали единицы уменьшаем счетчик касаний на 1
				vibrate(1,70,0); // вибрируем
				}
			if (incr_no==1) {
				app_data->col = app_data->col-10; // если выбрали десятки уменьшаем счетчик касаний на 10
				vibrate(1,70,0); // вибрируем
				}
			if (incr_no==2) {
				app_data->col = app_data->col-100; // если выбрали сотни уменьшаем счетчик касаний на 100
				vibrate(1,70,0); // вибрируем
				}
			if (incr_no==3) {
				app_data->col = app_data->col-1000; // если выбрали тысячи уменьшаем счетчик касаний на 1000
				vibrate(1,70,0); // вибрируем
				}
			if (incr_no==4) {
				app_data->col = app_data->col-10000; // если выбрали десятки тысяч уменьшаем счетчик касаний на 10000
				vibrate(1,70,0); // вибрируем
				}
			if (incr_no==5) {
				app_data->col = app_data->col-100000; // если выбрали сотни тысяч уменьшаем счетчик касаний на 100000
				vibrate(1,70,0); // вибрируем
				}
			if (incr_no==6) {
				app_data->col = app_data->col-1000000; // если выбрали миллионы уменьшаем счетчик касаний на 1000000
				vibrate(1,70,0); // вибрируем
				}
			if (app_data->col<0||app_data->col==0) {
				app_data->col = 0; // если привысили лимит разрядов, то не увеличиваем
				vibrate(3,300,70); // вибрируем 3 раза когда достигнут 0
				}
			draw_screen(app_data->col); // перерисоваваем экран
			repaint_screen_lines(40, 110); // обновляем строки экрана
			break;
		};		
		default:{	
			// что-то пошло не так...
			break;
		};		
	}
	
	return result;
};

// пользовательская функция
void draw_screen(int col){
	struct app_data_**  app_data_p = (struct app_data_ **)get_ptr_temp_buf_2();    //  указатель на указатель на данные экрана
	struct app_data_ *	app_data = *app_data_p;				//	указатель на данные экрана
	char crnd[8];     // переменная для перевода переменной col типа int в  тип char для печати                               
    _sprintf(crnd, "%07d", col); // конвертируем число int в char лидирующие нули, при необходимости, _sprintf добьёт сам
	set_bg_color(COLOR_BLACK); // делаем фон черным
	draw_filled_rect_bg(0, 43, 176, 110); // стираем предыдущее значение
	show_big_digit(3, crnd, 12, 62, 5); // печатаем результат большими цифрами
	show_elf_res_by_id(app_data->proc->index_listed, 6, 142-(22*incr_no), 47); // выводим индикатор разряда увеличения
	repaint_screen_lines(0, 176); // обновляем строки экрана
};