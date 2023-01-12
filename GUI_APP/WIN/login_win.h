#ifndef __LOGIN_WIN_H__
#define __LOGIN_WIN_H__

#include "sys.h"
#include "lvgl.h"


//º¯ÊıÉêÃ÷
void login_win_create(lv_obj_t * parent);


extern struct daily_weather_ui daily1_weather_ui;
extern struct daily_weather_ui daily2_weather_ui;
extern struct daily_weather_ui daily3_weather_ui;
extern struct now_weather_ui now_weather_ui;

void now_weather_set_temp(struct now_weather_ui *ui, char *temp);
void now_weather_set_logo(struct now_weather_ui *ui, const void *src_img);
void now_weather_set_last_update(struct now_weather_ui *ui, const void *last_update);


void daily_weather_set_date(struct daily_weather_ui *ui, char *date);
void daily_weather_set_weather_text(struct daily_weather_ui *ui, char *text);
void daily_weather_set_temp(struct daily_weather_ui *ui, char *high, char *low);
void daily_weather_set_humi(struct daily_weather_ui *ui, char *humi);
void daily_weather_set_wind_direc(struct daily_weather_ui *ui, char *dir);
void daily_weather_set_wind_speed(struct daily_weather_ui *ui, char *speed);

#endif
