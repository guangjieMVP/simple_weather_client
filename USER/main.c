#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "sdram.h"
#include "lan8720.h"
#include "timer.h"
#include "pcf8574.h"
#include "usmart.h"
#include "malloc.h"
#include "lwip/netif.h"
#include "lwip_comm.h"
#include "lwipopts.h"
#include "tcp_server_demo.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "gui_app.h" 
#include "rtc.h"

//KEY任务
#define KEY_TASK_PRIO 		8
//任务堆栈大小
#define KEY_STK_SIZE		128		
//任务堆栈
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//任务函数
void key_task(void *pdata);  

//在LCD上显示地址信息任务
//任务优先级
#define DISPLAY_TASK_PRIO	9
//任务堆栈大小
#define DISPLAY_STK_SIZE	2048
//任务堆栈
OS_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
//任务函数
void display_task(void *pdata);

//天气客户端
//任务优先级
#define WAATHER_TASK_PRIO		12
//任务堆栈大小
#define WAATHER_STK_SIZE		4096
//任务堆栈
OS_STK	WAATHER_TASK_STK[WAATHER_STK_SIZE];
//任务函数
void tcp_weather_client_task(void *pdata);  


//LED任务
//任务优先级
#define LED_TASK_PRIO		10
//任务堆栈大小
#define LED_STK_SIZE		64
//任务堆栈
OS_STK	LED_TASK_STK[LED_STK_SIZE];
//任务函数
void led_task(void *pdata);  




//START任务
//任务优先级
#define START_TASK_PRIO		11
//任务堆栈大小
#define START_STK_SIZE		128
//任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata); 

//在LCD上显示地址信息
//mode:1 显示DHCP获取到的地址
//	  其他 显示静态地址
void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==2)
	{
		sprintf((char*)buf,"DHCP IP :%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"DHCP GW :%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
		LCD_ShowString(30,170,210,16,16,buf); 
		LCD_ShowString(30,190,210,16,16,"Port:8088!"); 
	}
	else 
	{
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
		LCD_ShowString(30,170,210,16,16,buf); 
		LCD_ShowString(30,190,210,16,16,"Port:8088!"); 
	}	
}

int main(void)
{   
    Stm32_Clock_Init(360, 25, 2, 8);   //设置时钟,180Mhz   
    HAL_Init();                     //初始化HAL库
    delay_init(180);                //初始化延时函数
    UART_Init(115200);              //初始化USART
//    usmart_dev.init(90); 		    //初始化USMART	
    LED_Init();                     //初始化LED 
    KEY_Init();                     //初始化按键
    SDRAM_Init();                   //初始化SDRAM
    LCD_Init();                     //初始化LCD
	LCD_Display_Dir(1);
    PCF8574_Init();                 //初始化PCF8574
    my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMEX);		    //初始化外部内存池
	my_mem_init(SRAMCCM);		    //初始化CCM内存池
	
	RTC_Init();                     //初始化RTC 
    RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0); //配置WAKE UP中断,1秒钟中断一次 
//	tp_dev.init();				    			//触摸屏初始化 
	lv_init();											//lvgl系统初始化
	lv_port_disp_init();						//lvgl显示接口初始化,放在lv_init()的后面
//	lv_port_indev_init();						//lvgl输入接口初始化,放在lv_init()的后面
	
//	POINT_COLOR = RED; 		        //红色字体
//	LCD_ShowString(30,30,200,20,16,"Apollo STM32F4/F7");
//	LCD_ShowString(30,50,200,20,16,"TCP Server NETCONN Test");
//	LCD_ShowString(30,70,200,20,16,"ATOM@ALIENTEK");
//	LCD_ShowString(30,90,200,20,16,"2016/2/24");
    
	OSInit(); 					    //UCOS初始化
	while(lwip_comm_init()) 	    //lwip初始化
	{
//		LCD_ShowString(30,110,200,20,16,"Lwip Init failed! "); 	//lwip初始化失败
		printf("Lwip Init failed!\r\n");
		delay_ms(500);
//		LCD_Fill(30,110,230,150,WHITE);
		delay_ms(500);
	}
//	LCD_ShowString(30,110,200,20,16,"TCP Server Success!  "); 	//tcp服务器创建成功
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart();      //开启UCOS
}

//start任务
void start_task(void *pdata)      
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//初始化统计任务
	OS_ENTER_CRITICAL();  	//关中断
#if LWIP_DHCP
	lwip_comm_dhcp_creat(); //创建DHCP任务
#endif
	
	OSTaskCreate(led_task, (void*)0, (OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1], LED_TASK_PRIO);//创建LED任务
	OSTaskCreate(display_task, (void*)0, (OS_STK*)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE-1], DISPLAY_TASK_PRIO); //显示任务
	OSTaskCreate(tcp_weather_client_task, (void*)0, (OS_STK*)&WAATHER_TASK_STK[WAATHER_STK_SIZE-1], WAATHER_TASK_PRIO); 
	OSTaskSuspend(OS_PRIO_SELF); //挂起start_task任务
	OS_EXIT_CRITICAL();  		//开中断
}

//显示地址等信息
void display_task(void *pdata)
{
	gui_app_start();
	
	while(1)
	{ 
//#if LWIP_DHCP									//当开启DHCP的时候
//		if(lwipdev.dhcpstatus != 0) 			//开启DHCP
//		{
//			show_address(lwipdev.dhcpstatus );	//显示地址信息
//			OSTaskSuspend(OS_PRIO_SELF); 		//显示完地址信息后挂起自身任务
//		}
//#else
//		show_address(0); 						//显示静态地址
//		OSTaskSuspend(OS_PRIO_SELF); 		 	//显示完地址信息后挂起自身任务
//#endif //LWIP_DHCP
		lv_task_handler();//lvgl的事务处理
		OSTimeDlyHMSM(0, 0, 0, 20);
	}
}


//led任务
void led_task(void *pdata)
{
//	RTC_TimeTypeDef RTC_TimeStruct;
//    RTC_DateTypeDef RTC_DateStruct;
	
	while(1)
	{
//		HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct, RTC_FORMAT_BIN);
////		sprintf((char*)tbuf,"Time:%02d:%02d:%02d",RTC_TimeStruct.Hours,RTC_TimeStruct.Minutes,RTC_TimeStruct.Seconds); 
//		HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct, RTC_FORMAT_BIN);
////		sprintf((char*)tbuf,"Date:20%02d-%02d-%02d",RTC_DateStruct.Year,RTC_DateStruct.Month,RTC_DateStruct.Date); 	
////		sprintf((char*)tbuf,"Week:%d",RTC_DateStruct.WeekDay); 
		LED0 = !LED0;
		OSTimeDlyHMSM(0, 0, 0, 500);  //延时500ms
 	}
}

void tcp_weather_client_task(void *pdata)
{
	ServerAddr_Init();
	
	while (1)
	{
//		printf("weather running\r\n");
		OSTimeDlyHMSM(0, 0, 5, 1000);
		while (1)
		{
			WeatherHandler();
			OSTimeDlyHMSM(0, 0, 0, 10000);
		}
	}
}
