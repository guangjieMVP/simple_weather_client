#include "login_win.h"
#include "gui_app.h"
#include "main_win.h"
#include <string.h>

struct now_weather_ui {
	lv_obj_t *temperature_label;
	lv_obj_t *weather_logo;
	lv_obj_t *city_name_label;
	lv_obj_t *last_update_label;
};

struct date_time_ui {
	lv_obj_t *date_label;
	lv_obj_t *time_label;
	lv_obj_t *week;
};

struct daily_weather_ui {
	lv_obj_t *date_label;
	
	lv_obj_t *temperature_label;
	lv_obj_t *temperature_logo;
	
	lv_obj_t *wind_direction_label;
	lv_obj_t *wind_direction_logo;
	
	lv_obj_t *humidity_label;
	lv_obj_t *humidity_logo;
	
	lv_obj_t *wind_speed_label;
	lv_obj_t *wind_speed_logo;
	
	lv_obj_t *weather_label;
	lv_obj_t *weather_logo;
	
	lv_obj_t *wind_scale_label;
	lv_obj_t *wind_scale_logo;
};



#define LINE1_POS_Y      180

#define LINE2_POS_X      266
#define LINE2_POS_Y      440

#define LINE3_POS_X      533
#define LINE3_POS_Y      440


#define LINE0_POS_X      220
#define LINE0_POS_Y      150

#define LCD_WIDTH        800

#define TEXT_COLOR      0xffffff

LV_IMG_DECLARE(fair_weather);//ͼƬ����
LV_IMG_DECLARE(fair_weather_white);//ͼƬ����  
LV_IMG_DECLARE(weather48x48);
LV_IMG_DECLARE(temperature32x32);               
LV_IMG_DECLARE(humidity32x32);
LV_IMG_DECLARE(wind_direation32x32);  
LV_IMG_DECLARE(wind_speed32x32);
LV_IMG_DECLARE(weather32x32);  
LV_IMG_DECLARE(sun128x128);

LV_FONT_DECLARE(myFont_Foshan);   //��������   
LV_FONT_DECLARE(myFontNumber);  
LV_FONT_DECLARE(myFontDailyWeather);

static lv_obj_t *login_win; 

static lv_point_t line0_points[] = { {LINE0_POS_X, LINE0_POS_Y-120}, {LINE0_POS_X, LINE0_POS_Y} };
static lv_point_t line1_points[] = { {0, LINE1_POS_Y }, { 800, LINE1_POS_Y} };
static lv_point_t line2_points[] = { {LINE2_POS_X, LINE1_POS_Y+30 }, { LINE2_POS_X, LINE2_POS_Y}};
static lv_point_t line3_points[] = { {LINE3_POS_X, LINE1_POS_Y+30 }, { LINE3_POS_X, LINE3_POS_Y}};

struct daily_weather_ui daily1_weather_ui;
struct daily_weather_ui daily2_weather_ui;
struct daily_weather_ui daily3_weather_ui;
struct now_weather_ui now_weather_ui;

struct date_time_ui *datimeweek;

void date_time_ui_init(struct date_time_ui *ui)
{
	
}

void now_weather_set_temp(struct now_weather_ui *ui, char *temp)
{
	char buf[120] = {0};
	sprintf(buf, "%s °", temp);
	lv_label_set_text(ui->temperature_label, buf);
}

void now_weather_set_logo(struct now_weather_ui *ui, const void *src_img)
{
	lv_img_set_src(ui->weather_logo, src_img);
}

void now_weather_set_last_update(struct now_weather_ui *ui, const void *last_update)
{
	lv_label_set_text(ui->last_update_label, last_update);
}

static void now_weather_ui_init(struct now_weather_ui *ui, lv_obj_t *parent)
{
	static lv_style_t my_style; 
	static lv_style_t temp_style;
	
	lv_style_copy(&my_style, &lv_style_pretty_color);//��ʽ���� 
	my_style.text.font = &myFont_Foshan;//����ʽ��ʹ������ 
	my_style.text.color = LV_COLOR_BLACK;
	
	ui->city_name_label = lv_label_create(parent, NULL);
	lv_obj_set_pos(ui->city_name_label, LINE0_POS_X-160, LINE0_POS_Y-105);
//	lv_obj_set_size(ui->city_name_label, 100, 100);
	lv_obj_set_style(ui->city_name_label, &my_style);
//	lv_label_set_recolor(ui->city_name_label, true); 
	lv_label_set_text(ui->city_name_label, "佛山");
//	lv_label_set_style(ui->city_name_label, LV_LABEL_STYLE_MAIN, &my_style);//������ʽ 
	
	
	ui->last_update_label = lv_label_create(parent, NULL);
	lv_obj_set_pos(ui->last_update_label, LINE0_POS_X+20, LINE0_POS_Y-130);
	lv_obj_set_size(ui->last_update_label, 100, 60);
	lv_label_set_text(ui->last_update_label, "2021-03-23T09:50:00+08:00");
	
	ui->weather_logo = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->weather_logo, LINE0_POS_X+20, LINE0_POS_Y-110);            
	lv_img_set_src(ui->weather_logo, &sun128x128);
	
	lv_style_copy(&temp_style, &lv_style_pretty_color);
	temp_style.text.font = &myFontNumber; 
	temp_style.text.color = LV_COLOR_BLACK;
	ui->temperature_label = lv_label_create(parent, NULL);
	lv_obj_set_style(ui->temperature_label, &temp_style);
	lv_obj_set_pos(ui->temperature_label, LINE0_POS_X+180, LINE0_POS_Y-80);
	lv_label_set_text(ui->temperature_label, "99 °");
}

static lv_style_t daily_weather_style; 

void daily_weather_set_date(struct daily_weather_ui *ui, char *date)
{
	lv_label_set_text(ui->date_label, date);
}

void daily_weather_set_weather_text(struct daily_weather_ui *ui, char *text)
{
	char buf[120] = {0};
	sprintf(buf, "天气: %s", text);
	lv_label_set_text(ui->weather_label, buf);
}

void daily_weather_set_temp(struct daily_weather_ui *ui, char *high, char *low)
{
	char buf[120] = {0};
	sprintf(buf, "温度: %s~%s °", low, high);
	lv_label_set_text(ui->temperature_label, buf);
}

void daily_weather_set_humi(struct daily_weather_ui *ui, char *humi)
{
	char buf[120] = {0};
	sprintf(buf, "湿度: %s\%RH", humi);
	lv_label_set_text(ui->humidity_label, buf);
}

void daily_weather_set_wind_direc(struct daily_weather_ui *ui, char *dir)
{
	char buf[120] = {0};
	sprintf(buf, "风向: %s", dir);
	lv_label_set_text(ui->wind_direction_label, buf);
}

void daily_weather_set_wind_speed(struct daily_weather_ui *ui, char *speed)
{
	char buf[120] = {0};
	sprintf(buf, "风速: %skm/h", speed);
	lv_label_set_text(ui->wind_speed_label, buf);
}


void daily1_weather_ui_init(struct daily_weather_ui *ui, lv_obj_t *parent)
{
	lv_style_copy(&daily_weather_style, &lv_style_pretty_color);//��ʽ���� 
	daily_weather_style.text.font = &myFontDailyWeather;//����ʽ��ʹ������ 
	daily_weather_style.text.color = LV_COLOR_BLACK;
	
	ui->date_label =  lv_label_create(parent, NULL);
	lv_obj_set_pos(ui->date_label, LINE2_POS_X-210, LINE1_POS_Y+5);
	lv_obj_set_style(ui->date_label, &daily_weather_style);
	lv_label_set_text(ui->date_label, "2021-03-24");
	lv_obj_set_size(ui->date_label, 60, 60);
	

	ui->weather_logo         = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->weather_logo, LINE2_POS_X-260, LINE1_POS_Y+40);            
	lv_img_set_src(ui->weather_logo, &weather32x32);
	ui->weather_label        = lv_label_create(parent, NULL);
	lv_obj_set_size(ui->weather_label, 60, 30);
	lv_label_set_text(ui->weather_label, "天气: 多云");
	lv_obj_set_style(ui->weather_label, &daily_weather_style);
	lv_obj_set_pos(ui->weather_label, LINE2_POS_X-260+50, LINE1_POS_Y+45);
//	lv_obj_align(ui->weather_label, ui->weather_logo, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);     //���ö��뷽ʽ
	

	ui->temperature_logo  = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->temperature_logo, LINE2_POS_X-260, LINE1_POS_Y+90);
	lv_img_set_src(ui->temperature_logo, &temperature32x32);
	ui->temperature_label = lv_label_create(parent, NULL);
	lv_label_set_text(ui->temperature_label, "温度: 20°");
	lv_obj_set_style(ui->temperature_label, &daily_weather_style);
	lv_obj_set_pos(ui->temperature_label, LINE2_POS_X-260+50, LINE1_POS_Y+95);
//	lv_obj_align(ui->temperature_label, ui->temperature_logo, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);     //���ö��뷽ʽ

	ui->humidity_logo  = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->humidity_logo, LINE2_POS_X-260, LINE1_POS_Y+140);
	lv_img_set_src(ui->humidity_logo, &humidity32x32);
	ui->humidity_label = lv_label_create(parent, NULL);
	lv_label_set_text(ui->humidity_label, "湿度: 70\%RH");
	lv_obj_set_style(ui->humidity_label, &daily_weather_style);
	lv_obj_set_pos(ui->humidity_label, LINE2_POS_X-260+50, LINE1_POS_Y+145);
	
	ui->wind_direction_logo  = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->wind_direction_logo, LINE2_POS_X-260, LINE1_POS_Y+190);
	lv_img_set_src(ui->wind_direction_logo, &wind_direation32x32);
	ui->wind_direction_label = lv_label_create(parent, NULL);
	lv_label_set_text(ui->wind_direction_label, "风向: 东");
	lv_obj_set_style(ui->wind_direction_label, &daily_weather_style);
	lv_obj_set_pos(ui->wind_direction_label, LINE2_POS_X-260+50, LINE1_POS_Y+195);
	
	ui->wind_speed_logo  = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->wind_speed_logo, LINE2_POS_X-260, LINE1_POS_Y+240);
	lv_img_set_src(ui->wind_speed_logo, &wind_speed32x32);
	ui->wind_speed_label = lv_label_create(parent, NULL);
	lv_obj_set_style(ui->wind_speed_label, &daily_weather_style);
	lv_label_set_text(ui->wind_speed_label, "风速: 2km/h");
	lv_obj_set_pos(ui->wind_speed_label, LINE2_POS_X-260+50, LINE1_POS_Y+245);
				 
//	ui->wind_scale_label     = lv_label_create(parent, NULL);
//	ui->wind_scale_logo      = lv_img_create(parent, NULL);
}

void daily2_weather_ui_init(struct daily_weather_ui *ui, lv_obj_t *parent)
{
	ui->date_label =  lv_label_create(parent, NULL);
	lv_obj_set_pos(ui->date_label, LINE3_POS_X-210, LINE1_POS_Y+5);
	lv_label_set_text(ui->date_label, "2021-03-25");
	lv_obj_set_style(ui->date_label, &daily_weather_style);
	lv_obj_set_size(ui->date_label, 60, 60);
	
	ui->weather_logo         = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->weather_logo, LINE3_POS_X-260, LINE1_POS_Y+40);
	lv_img_set_src(ui->weather_logo, &weather32x32);
	ui->weather_label        = lv_label_create(parent, NULL);
	lv_obj_set_size(ui->weather_label, 60, 30);
	lv_label_set_text(ui->weather_label, "天气: 多云");
	lv_obj_set_style(ui->weather_label, &daily_weather_style);
	lv_obj_set_pos(ui->weather_label, LINE3_POS_X-260+50, LINE1_POS_Y+45);
//	lv_obj_align(ui->weather_label, ui->weather_logo, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);     //���ö��뷽ʽ
	

	ui->temperature_logo  = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->temperature_logo, LINE3_POS_X-260, LINE1_POS_Y+90);
	lv_img_set_src(ui->temperature_logo, &temperature32x32);
	ui->temperature_label = lv_label_create(parent, NULL);
	lv_label_set_text(ui->temperature_label, "温度: 20°");
	lv_obj_set_style(ui->temperature_label, &daily_weather_style);
	lv_obj_set_pos(ui->temperature_label, LINE3_POS_X-260+50, LINE1_POS_Y+95);
//	lv_obj_align(ui->temperature_label, ui->temperature_logo, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);     //���ö��뷽ʽ

	ui->humidity_logo  = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->humidity_logo, LINE3_POS_X-260, LINE1_POS_Y+140);
	lv_img_set_src(ui->humidity_logo, &humidity32x32);
	ui->humidity_label = lv_label_create(parent, NULL);
	lv_label_set_text(ui->humidity_label, "湿度: 70\%RH");
	lv_obj_set_style(ui->humidity_label, &daily_weather_style);
	lv_obj_set_pos(ui->humidity_label, LINE3_POS_X-260+50, LINE1_POS_Y+145);
	
	ui->wind_direction_logo  = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->wind_direction_logo, LINE3_POS_X-260, LINE1_POS_Y+190);
	lv_img_set_src(ui->wind_direction_logo, &wind_direation32x32);
	ui->wind_direction_label = lv_label_create(parent, NULL);
	lv_label_set_text(ui->wind_direction_label, "风向: 东");
	lv_obj_set_style(ui->wind_direction_label, &daily_weather_style);
	lv_obj_set_pos(ui->wind_direction_label, LINE3_POS_X-260+50, LINE1_POS_Y+195);
	
	ui->wind_speed_logo  = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->wind_speed_logo, LINE3_POS_X-260, LINE1_POS_Y+240);
	lv_img_set_src(ui->wind_speed_logo, &wind_speed32x32);
	ui->wind_speed_label = lv_label_create(parent, NULL);
	lv_obj_set_style(ui->wind_speed_label, &daily_weather_style);
	lv_label_set_text(ui->wind_speed_label, "风速: 2km/h");
	lv_obj_set_pos(ui->wind_speed_label, LINE3_POS_X-260+50, LINE1_POS_Y+245);
}

void daily3_weather_ui_init(struct daily_weather_ui *ui, lv_obj_t *parent)
{
	ui->date_label =  lv_label_create(parent, NULL);
	lv_obj_set_pos(ui->date_label, LCD_WIDTH-210, LINE1_POS_Y+5);
	lv_label_set_text(ui->date_label, "2021-03-26");
	lv_obj_set_style(ui->date_label, &daily_weather_style);
	lv_obj_set_size(ui->date_label, 60, 60);
	
	
	ui->weather_logo         = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->weather_logo, LCD_WIDTH-260, LINE1_POS_Y+40);
	lv_img_set_src(ui->weather_logo, &weather32x32);
	ui->weather_label        = lv_label_create(parent, NULL);
	lv_obj_set_size(ui->weather_label, 60, 30);
	lv_label_set_text(ui->weather_label, "天气: 多云");
	lv_obj_set_style(ui->weather_label, &daily_weather_style);
	lv_obj_set_pos(ui->weather_label, LCD_WIDTH-260+50, LINE1_POS_Y+45);
//	lv_obj_align(ui->weather_label, ui->weather_logo, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);     //���ö��뷽ʽ
	

	ui->temperature_logo  = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->temperature_logo, LCD_WIDTH-260, LINE1_POS_Y+90);
	lv_img_set_src(ui->temperature_logo, &temperature32x32);
	ui->temperature_label = lv_label_create(parent, NULL);
	lv_label_set_text(ui->temperature_label, "温度: 20°");
	lv_obj_set_style(ui->temperature_label, &daily_weather_style);
	lv_obj_set_pos(ui->temperature_label, LCD_WIDTH-260+50, LINE1_POS_Y+95);
//	lv_obj_align(ui->temperature_label, ui->temperature_logo, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);     //���ö��뷽ʽ

	ui->humidity_logo  = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->humidity_logo, LCD_WIDTH-260, LINE1_POS_Y+140);
	lv_img_set_src(ui->humidity_logo, &humidity32x32);
	ui->humidity_label = lv_label_create(parent, NULL);
	lv_label_set_text(ui->humidity_label, "湿度: 70\%RH");
	lv_obj_set_style(ui->humidity_label, &daily_weather_style);
	lv_obj_set_pos(ui->humidity_label, LCD_WIDTH-260+50, LINE1_POS_Y+145);
	
	ui->wind_direction_logo  = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->wind_direction_logo, LCD_WIDTH-260, LINE1_POS_Y+190);
	lv_img_set_src(ui->wind_direction_logo, &wind_direation32x32);
	ui->wind_direction_label = lv_label_create(parent, NULL);
	lv_label_set_text(ui->wind_direction_label, "风向: 东");
	lv_obj_set_style(ui->wind_direction_label, &daily_weather_style);
	lv_obj_set_pos(ui->wind_direction_label, LCD_WIDTH-260+50, LINE1_POS_Y+195);
	
	ui->wind_speed_logo  = lv_img_create(parent, NULL);
	lv_obj_set_pos(ui->wind_speed_logo, LCD_WIDTH-260, LINE1_POS_Y+240);
	lv_img_set_src(ui->wind_speed_logo, &wind_speed32x32);
	ui->wind_speed_label = lv_label_create(parent, NULL);
	lv_obj_set_style(ui->wind_speed_label, &daily_weather_style);
	lv_label_set_text(ui->wind_speed_label, "风速: 2km/h");
	lv_obj_set_pos(ui->wind_speed_label, LCD_WIDTH-260+50, LINE1_POS_Y+245);
}

//������¼����
void login_win_create(lv_obj_t * parent)
{
	uint16_t parent_width,parent_height;
	
	//��ȡ������Ŀ��͸�
	parent_width = lv_obj_get_width(parent);
	parent_height = lv_obj_get_height(parent);
	//������¼����
	login_win = lv_cont_create(parent, NULL);//��������ģ�ⴰ��
	lv_obj_set_size(login_win, parent_width, parent_height);//���ô���Ĵ�С
	lv_cont_set_layout(login_win, LV_LAYOUT_OFF);//�����޲��ַ�ʽ
//	
	lv_obj_t *line0 = lv_line_create(parent, NULL);
	lv_line_set_points(line0, line0_points, 2);
	
	lv_obj_t *line1 = lv_line_create(parent, NULL);
	lv_line_set_points(line1, line1_points, 2);
	
	lv_obj_t *line2 = lv_line_create(parent, NULL);
	lv_line_set_points(line2, line2_points, 2);
	
	lv_obj_t *line3 = lv_line_create(parent, NULL);
	lv_line_set_points(line3, line3_points, 2);
	
	now_weather_ui_init(&now_weather_ui, parent);
	
	daily1_weather_ui_init(&daily1_weather_ui, parent);
	daily2_weather_ui_init(&daily2_weather_ui, parent);
	daily3_weather_ui_init(&daily3_weather_ui, parent);
//	//����һ���ֻ��������
//	phone_ta = lv_ta_create(login_win, NULL);
//	lv_obj_set_size(phone_ta, parent_width*0.7f, 30);//���ô�С
//	lv_obj_align(phone_ta, logo_img,LV_ALIGN_OUT_BOTTOM_MID, 0, 20);//���ö��뷽ʽ
//	lv_ta_set_text(phone_ta, "");//���Ĭ�ϵ��ı�����
//	lv_ta_set_placeholder_text(phone_ta,"Input phone");//����placeholder��ʾ�ı�
//	lv_ta_set_one_line(phone_ta, true);//ʹ�ܵ���ģʽ
//	lv_obj_set_event_cb(phone_ta, event_handler);//�����¼��ص�����
//	//����һ�����������
//	password_ta = lv_ta_create(login_win, phone_ta);//��phone_ta�Ͻ��п���
//	lv_ta_set_pwd_mode(password_ta,true);//ʹ�����뱣��ģʽ
//	lv_obj_align(password_ta,phone_ta, LV_ALIGN_OUT_BOTTOM_MID,0,10);
//	lv_ta_set_cursor_type(password_ta, LV_CURSOR_LINE|LV_CURSOR_HIDDEN);//�����ع��
//	lv_ta_set_placeholder_text(password_ta, "Input password");//����placeholder��ʾ�ı�
//	//����һ����½��ť
//	login_btn = lv_btn_create(login_win,NULL);
//	lv_obj_set_size(login_btn,parent_width*0.7f,40);//���ô�С
//	lv_obj_align(login_btn,password_ta,LV_ALIGN_OUT_BOTTOM_MID,0,20);//���ö��뷽ʽ
//	lv_obj_t * btn_label = lv_label_create(login_btn,NULL);
//	lv_label_set_text(btn_label,"login");//���ð�ť�ϵı���
//	lv_obj_set_event_cb(login_btn, event_handler);//�����¼��ص�����
}















