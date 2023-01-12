#include "stdio.h"
#include "string.h"
#include "lwip/opt.h" 
#include "lwip/lwip_sys.h"
#include "lwip/api.h"
#include "includes.h"
#include <lwip/sockets.h> 
#include "Utf8ToGbk.h"
#include "login_win.h"

#include "cJSON.h"

#define XINZHI_WEATHER_IP   "116.62.81.138"    //芯知天气服务IP

//#define XINZHI_WEATHER_SERVER   "www.seniverse.com"      //心知天气域名

//http 请求指令
#define GET_WEATHER_PACK    \
    "GET https://api.seniverse.com/v3/weather/%s.json?key=%s&location=%s&language=zh-Hans&unit=c\r\n\r\n"

#define PUBLICJ_KEY   "POB2txaak6FjMohJV"     //公钥
#define PRIVATEJ_KEY  "SuS5skvpfEOLSuNP4"     //私钥

#define LOCATION     "foshan"

#define  NOW_JSON_PACK     "now"              //获取当前天气
#define  DAILY_JSON_PACK   "daily"            //获取最新的天气

#define SERVER_HTTP_PORT    80

typedef struct weather_data {
    char id[32];
    char name[32];
    char country[32];
    char path[32];
    char timezone[32];
    char timezone_offset[32];
    char text[32];
    char code[32];
    char temperature[32];
	char last_update[64];

    char date[3][64];              //日期
    char text_day[3][64];
    char text_night[3][64];        //晚上
    char high[3][64];              //最低温
    char low[3][64];                //最高温        将二位数组改成64位就解决了hardfault问题
    char wind_speed[3][64];        //风速
    char wind_direction[3][64];    //风向
    char wind_scale[3][64];        //风力等级
    char humidity[3][64];          //湿度
}weather_t;


static int socketfd = -1;
static struct sockaddr_in server_addr; 

static void _now_weather_json_parser(weather_t *wdata, char *jsondata)
{
    int index;
    cJSON *json, *arrayItem, *item, *subject, *object;

    json = cJSON_Parse(jsondata);
    if (json == NULL)
    {
		printf("json parser error");
        return;
    }
    else
    {
        if ((arrayItem = cJSON_GetObjectItem(json, "results")) != NULL)
        {
            // int arrsize = cJSON_GetArraySize(results);
            if ((object = cJSON_GetArrayItem(arrayItem, 0)) != NULL)
            {
                if ((subject = cJSON_GetObjectItem(object, "location")) != NULL)
                {
                    if ((item = cJSON_GetObjectItem(subject, "id")) != NULL)
                    {
                        memcpy(wdata->id, item->valuestring, strlen(item->valuestring));
                    }
                    if ((item = cJSON_GetObjectItem(subject, "name")) != NULL)
                    {
                        memcpy(wdata->name, item->valuestring, strlen(item->valuestring));
                    }
                    if ((item = cJSON_GetObjectItem(subject, "path")) != NULL)
                    {
                        memcpy(wdata->path, item->valuestring, strlen(item->valuestring));
                    }
                    if ((item = cJSON_GetObjectItem(subject, "country")) != NULL)
                    {
                        memcpy(wdata->country, item->valuestring, strlen(item->valuestring));
                    }
                }
                else
                {
//                    fprintf(stderr, "don't get location obj\r\n");
                }

                /* 匹配子对象2：今天的天气情况 */
				if((subject = cJSON_GetObjectItem(object, "now")) != NULL)
				{
					// 匹配天气现象文字
					if((item = cJSON_GetObjectItem(subject, "text")) != NULL)
					{
						memcpy(wdata->text, item->valuestring, strlen(item->valuestring));
					}
					// 匹配天气现象代码
					if((item = cJSON_GetObjectItem(subject, "code")) != NULL)
					{
						memcpy(wdata->code, item->valuestring, strlen(item->valuestring)); 
					}
					// 匹配气温
					if((item = cJSON_GetObjectItem(subject, "temperature")) != NULL) 
					{
						memcpy(wdata->temperature, item->valuestring, strlen(item->valuestring));   
					}	
				}
				if((subject = cJSON_GetObjectItem(object, "last_update")) != NULL)
				{
					memcpy(wdata->last_update, subject->valuestring, strlen(subject->valuestring));
				}
            }
            else
            {
//                fprintf(stderr, "don't get arrayItem 0\r\n");
            }
        }
        else
        {
//            fprintf(stderr, "don't get arrayItem\r\n");
        }
    }
//	cJSON_Delete(item);
//	cJSON_Delete(arrayItem);
//	cJSON_Delete(object);
//	cJSON_Delete(subject);
    cJSON_Delete(json);
//	free(json);
}

static void _daily_weather_json_parser(weather_t *wdata, char *jsondata)
{
    cJSON *json = NULL;
    cJSON *results = NULL;
    cJSON *object = NULL;
    cJSON *location = NULL;
    cJSON *daily = NULL;
    cJSON *arritem = NULL;
    cJSON *item = NULL;
	
	printf("jsondata len = %d\r\n", strlen(jsondata));
    json = cJSON_Parse(jsondata);
    if (json == NULL)
    {
        printf("cJSON_Parse error\r\n");
       return;
    }

//    results = cJSON_GetObjectItem(json, "results");
//    if (results == NULL)
//    {
//        printf("cJSON_GetObject result error\r\n");
//        goto daily_end;
//    }

//    object = cJSON_GetArrayItem(results, 0);
//    if (object == NULL)
//    {
//        printf("get results arrayItem error\r\n");
//        goto daily_end;
//    }

//    location = cJSON_GetObjectItem(object, "location"); 
//    if (location == NULL)
//    {
//        printf("get location object error\r\n");
//        goto daily_end;
//    }
	if ((results = cJSON_GetObjectItem(json, "results")) != NULL)
	{
		if ((object = cJSON_GetArrayItem(results, 0)) != NULL)
		{
			if ((location = cJSON_GetObjectItem(object, "location")) != NULL)
			{
				if ((daily = cJSON_GetObjectItem(object, "daily")) != NULL)
				{
					int size = cJSON_GetArraySize(daily);
					// printf("daily array size = %d\r\n", size);
					for (int i = 0; i < size; i++)
					{   
						arritem = cJSON_GetArrayItem(daily, i);
						if (arritem == NULL)
						{
							printf("get arritem error %d\r\n", i);
							continue;
						}
						if ((item = cJSON_GetObjectItem(arritem, "date")) != NULL)
						{
							memcpy(&wdata->date[i][32], item->valuestring, strlen(item->valuestring));
							// printf("date %s\r\n", &wdata->date[i][32]);
						}
						if ((item = cJSON_GetObjectItem(arritem, "text_day")) != NULL)
						{
							memcpy(&wdata->text_day[i][32], item->valuestring, strlen(item->valuestring));
							// printf("text_day %s\r\n", &wdata->text_day[i][32]);
						}
						if ((item = cJSON_GetObjectItem(arritem, "text_night")) != NULL)
						{
							memcpy(&wdata->text_night[i][32], item->valuestring, strlen(item->valuestring));
							// printf("text_night %s\r\n", &wdata->text_night[i][32]);
						}
						 if ((item = cJSON_GetObjectItem(arritem, "high")) != NULL)
						{
							memcpy(&wdata->high[i][32], item->valuestring, strlen(item->valuestring));
						}
						if ((item = cJSON_GetObjectItem(arritem, "low")) != NULL)
						{
							memcpy(&wdata->low[i][32], item->valuestring, strlen(item->valuestring));
						}
						if ((item = cJSON_GetObjectItem(arritem, "wind_direction")) != NULL)
						{
							memcpy(&wdata->wind_direction[i][32], item->valuestring, strlen(item->valuestring));
						}
						if ((item = cJSON_GetObjectItem(arritem, "wind_speed")) != NULL)
						{
							memcpy(&wdata->wind_speed[i][32], item->valuestring, strlen(item->valuestring));
						}
						if ((item = cJSON_GetObjectItem(arritem, "wind_scale")) != NULL)
						{
							memcpy(&wdata->wind_scale[i][32], item->valuestring, strlen(item->valuestring));
						}
						if ((item = cJSON_GetObjectItem(arritem, "humidity")) != NULL)
						{
							memcpy(&wdata->humidity[i][32], item->valuestring, strlen(item->valuestring));
						}
					}
				}
			}
		}
	}
//daily_end:
    cJSON_Delete(json);
//	free(json);
}

static int get_weather(weather_t *wdata, char *which_json, char *loacation)
{
    int ret;
    char json_buffer[1024];
    char request[1024] = {0};

    memset(json_buffer, 0, 1024);

    //新建立套接字
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		printf("socket() err\r\n");
		return -1;
    }
    if (connect(socketfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0)   //发起连接
    {
		printf("connect() err\r\n");
        close(socketfd);
		return -1;
    }
    sprintf(request, GET_WEATHER_PACK, which_json, PRIVATEJ_KEY, loacation);
    ret = send(socketfd, request, strlen(request), 0);
    if (ret < 0)
    {
		printf("request weather error\r\n");
    }
    if (recv(socketfd, json_buffer, sizeof(json_buffer), 0) > 0)
    {
         printf("%s", json_buffer);
    }
    else
    {
//        fprintf(stderr, "recv errr %s\r\n", strerror(errno));
    }
    close(socketfd);
	
	int len;
//	utf82gbk(&jsonptr, json_buffer, strlen(json_buffer));
//	SwitchToGbk(json_buffer, strlen(json_buffer), json_buffer, &len);
湛江
    if (strcmp(which_json, NOW_JSON_PACK) == 0)
    {
        // printf("parser now json\r\n");
        _now_weather_json_parser(wdata, json_buffer);
    }
    if (strcmp(which_json, DAILY_JSON_PACK) == 0)
    {
        // printf("parser daily json\r\n");
        _daily_weather_json_parser(wdata, json_buffer);
    }
}

//格式化输出打印天气信息
static void format_weather_print(weather_t *wdata, char *location)
{
	now_weather_set_temp(&now_weather_ui, wdata->temperature);
	now_weather_set_last_update(&now_weather_ui, wdata->last_update);
    printf("\r\n");
    printf("================%s 现在天气如下====================\r\n", wdata->name);
    printf("天气:  %s\r\n", wdata->text);
    printf("温度: %s ℃\r\n", wdata->temperature);
    printf("地区: %s\r\n", wdata->path);
    printf("城市: %s\r\n",  wdata->name);
	printf("last update : %s\r\n", wdata->last_update);
	if (strcmp(wdata->text, "晴") == 0)
	{
		printf("Sunny\r\n");
	}
    printf("================%s未来三天天气如下===================\r\n", wdata->name);

    for (int i = 0; i < 3; i++)
    {
		if (i == 0)
		{
			daily_weather_set_date(&daily1_weather_ui, &wdata->date[i][32]);
			daily_weather_set_weather_text(&daily1_weather_ui, &wdata->text_day[i][32]);
			daily_weather_set_temp(&daily1_weather_ui, &wdata->high[i][32], &wdata->low[i][32]);
			daily_weather_set_humi(&daily1_weather_ui, &wdata->humidity[i][32]);
			daily_weather_set_wind_direc(&daily1_weather_ui, &wdata->wind_direction[i][32]);
			daily_weather_set_wind_speed(&daily1_weather_ui, &wdata->wind_speed[i][32]);
		}
		else if (i == 1)
		{
			daily_weather_set_date(&daily2_weather_ui, &wdata->date[i][32]);
			daily_weather_set_weather_text(&daily1_weather_ui, &wdata->text_day[i][32]);
			daily_weather_set_temp(&daily2_weather_ui, &wdata->high[i][32], &wdata->low[i][32]);
			daily_weather_set_humi(&daily2_weather_ui, &wdata->humidity[i][32]);
			daily_weather_set_wind_direc(&daily2_weather_ui, &wdata->wind_direction[i][32]);
			daily_weather_set_wind_speed(&daily2_weather_ui, &wdata->wind_speed[i][32]);
		}
		else 
		{
			daily_weather_set_date(&daily3_weather_ui, &wdata->date[i][32]);
			daily_weather_set_weather_text(&daily3_weather_ui, &wdata->text_day[i][32]);
			daily_weather_set_temp(&daily3_weather_ui, &wdata->high[i][32], &wdata->low[i][32]);
			daily_weather_set_humi(&daily3_weather_ui, &wdata->humidity[i][32]);
			daily_weather_set_wind_direc(&daily3_weather_ui, &wdata->wind_direction[i][32]);
			daily_weather_set_wind_speed(&daily3_weather_ui, &wdata->wind_speed[i][32]);
		}
		
        printf("[%s]\r\n", &wdata->date[i][32]);
        printf("text_day: %s\r\n", &wdata->text_day[i][32]);
        printf("text_night: %s\r\n", &wdata->text_night[i][32]);
        printf("temperute: %s ~ %s ℃\r\n", &wdata->low[i][32], &wdata->high[i][32]);
        printf("wind_direction: %s\r\n", &wdata->wind_direction[i][32]);
        printf("wind_speed: %s km/h\r\n", &wdata->wind_speed[i][32]);
        printf("wind_scale: %s\r\n", &wdata->wind_scale[i][32]);
        printf("humidity: %s\r\n", &wdata->humidity[i][32]);
        printf("\r\n");
    }
    printf("\r\n");
}

void WeatherHandler(void)
{
	weather_t wdata;
	char location[20] = {"foshan"};
	printf("sizeof(wdata) = %d\r\n", sizeof(weather_t));
	
	memset(&wdata, 0, sizeof(weather_t));
	get_weather(&wdata, NOW_JSON_PACK, location);
	get_weather(&wdata, DAILY_JSON_PACK, location); 
	format_weather_print(&wdata, location);
	printf("get weather data\r\n");
}


void ServerAddr_Init(void)
{
	memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_HTTP_PORT);
    server_addr.sin_addr.s_addr = inet_addr(XINZHI_WEATHER_IP);

}
//int main1(int argc, char **argv)
//{
//    addr = *((struct in_addr *)host->h_addr);
//    printf("host name : %s\r\n", host->h_name);     //主机名
//    printf("host ip :%s\r\n", inet_ntoa(addr));     //IP地址
//    
//    printf("port : %d\r\n", SERVER_HTTP_PORT);

//	memset(&server_addr, 0, sizeof(server_addr));
//    server_addr.sin_family = AF_INET;
//    server_addr.sin_port = htons(SERVER_HTTP_PORT);
//    server_addr.sin_addr.s_addr = inet_addr(XINZHI_WEATHER_IP);

//    weather_t wdata;
//   
//    char location[80] = {0};
//    int count = 0; 
//    while (1)
//    {
//        printf("\r\n请输入城市名称查询天气,如北京,输入beijing : ");
//        scanf("%s", location);
//        printf("\r\n");

//        memset(&wdata, 0, sizeof(weather_t));
//        get_weather(&wdata, NOW_JSON_PACK, location);
//        get_weather(&wdata, DAILY_JSON_PACK, location); 

//        format_weather_print(&wdata, location);
//        // printf("\r\n %d\r\n", count++);
//        // sleep(3);
//    }
//}