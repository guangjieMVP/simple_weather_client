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

//KEY����
#define KEY_TASK_PRIO 		8
//�����ջ��С
#define KEY_STK_SIZE		128		
//�����ջ
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//������
void key_task(void *pdata);  

//��LCD����ʾ��ַ��Ϣ����
//�������ȼ�
#define DISPLAY_TASK_PRIO	9
//�����ջ��С
#define DISPLAY_STK_SIZE	2048
//�����ջ
OS_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
//������
void display_task(void *pdata);

//�����ͻ���
//�������ȼ�
#define WAATHER_TASK_PRIO		12
//�����ջ��С
#define WAATHER_STK_SIZE		4096
//�����ջ
OS_STK	WAATHER_TASK_STK[WAATHER_STK_SIZE];
//������
void tcp_weather_client_task(void *pdata);  


//LED����
//�������ȼ�
#define LED_TASK_PRIO		10
//�����ջ��С
#define LED_STK_SIZE		64
//�����ջ
OS_STK	LED_TASK_STK[LED_STK_SIZE];
//������
void led_task(void *pdata);  




//START����
//�������ȼ�
#define START_TASK_PRIO		11
//�����ջ��С
#define START_STK_SIZE		128
//�����ջ
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata); 

//��LCD����ʾ��ַ��Ϣ
//mode:1 ��ʾDHCP��ȡ���ĵ�ַ
//	  ���� ��ʾ��̬��ַ
void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==2)
	{
		sprintf((char*)buf,"DHCP IP :%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"DHCP GW :%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
		LCD_ShowString(30,170,210,16,16,buf); 
		LCD_ShowString(30,190,210,16,16,"Port:8088!"); 
	}
	else 
	{
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
		LCD_ShowString(30,130,210,16,16,buf); 
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
		LCD_ShowString(30,150,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
		LCD_ShowString(30,170,210,16,16,buf); 
		LCD_ShowString(30,190,210,16,16,"Port:8088!"); 
	}	
}

int main(void)
{   
    Stm32_Clock_Init(360, 25, 2, 8);   //����ʱ��,180Mhz   
    HAL_Init();                     //��ʼ��HAL��
    delay_init(180);                //��ʼ����ʱ����
    UART_Init(115200);              //��ʼ��USART
//    usmart_dev.init(90); 		    //��ʼ��USMART	
    LED_Init();                     //��ʼ��LED 
    KEY_Init();                     //��ʼ������
    SDRAM_Init();                   //��ʼ��SDRAM
    LCD_Init();                     //��ʼ��LCD
	LCD_Display_Dir(1);
    PCF8574_Init();                 //��ʼ��PCF8574
    my_mem_init(SRAMIN);		    //��ʼ���ڲ��ڴ��
	my_mem_init(SRAMEX);		    //��ʼ���ⲿ�ڴ��
	my_mem_init(SRAMCCM);		    //��ʼ��CCM�ڴ��
	
	RTC_Init();                     //��ʼ��RTC 
    RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0); //����WAKE UP�ж�,1�����ж�һ�� 
//	tp_dev.init();				    			//��������ʼ�� 
	lv_init();											//lvglϵͳ��ʼ��
	lv_port_disp_init();						//lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ���
//	lv_port_indev_init();						//lvgl����ӿڳ�ʼ��,����lv_init()�ĺ���
	
//	POINT_COLOR = RED; 		        //��ɫ����
//	LCD_ShowString(30,30,200,20,16,"Apollo STM32F4/F7");
//	LCD_ShowString(30,50,200,20,16,"TCP Server NETCONN Test");
//	LCD_ShowString(30,70,200,20,16,"ATOM@ALIENTEK");
//	LCD_ShowString(30,90,200,20,16,"2016/2/24");
    
	OSInit(); 					    //UCOS��ʼ��
	while(lwip_comm_init()) 	    //lwip��ʼ��
	{
//		LCD_ShowString(30,110,200,20,16,"Lwip Init failed! "); 	//lwip��ʼ��ʧ��
		printf("Lwip Init failed!\r\n");
		delay_ms(500);
//		LCD_Fill(30,110,230,150,WHITE);
		delay_ms(500);
	}
//	LCD_ShowString(30,110,200,20,16,"TCP Server Success!  "); 	//tcp�����������ɹ�
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart();      //����UCOS
}

//start����
void start_task(void *pdata)      
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//��ʼ��ͳ������
	OS_ENTER_CRITICAL();  	//���ж�
#if LWIP_DHCP
	lwip_comm_dhcp_creat(); //����DHCP����
#endif
	
	OSTaskCreate(led_task, (void*)0, (OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1], LED_TASK_PRIO);//����LED����
	OSTaskCreate(display_task, (void*)0, (OS_STK*)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE-1], DISPLAY_TASK_PRIO); //��ʾ����
	OSTaskCreate(tcp_weather_client_task, (void*)0, (OS_STK*)&WAATHER_TASK_STK[WAATHER_STK_SIZE-1], WAATHER_TASK_PRIO); 
	OSTaskSuspend(OS_PRIO_SELF); //����start_task����
	OS_EXIT_CRITICAL();  		//���ж�
}

//��ʾ��ַ����Ϣ
void display_task(void *pdata)
{
	gui_app_start();
	
	while(1)
	{ 
//#if LWIP_DHCP									//������DHCP��ʱ��
//		if(lwipdev.dhcpstatus != 0) 			//����DHCP
//		{
//			show_address(lwipdev.dhcpstatus );	//��ʾ��ַ��Ϣ
//			OSTaskSuspend(OS_PRIO_SELF); 		//��ʾ���ַ��Ϣ�������������
//		}
//#else
//		show_address(0); 						//��ʾ��̬��ַ
//		OSTaskSuspend(OS_PRIO_SELF); 		 	//��ʾ���ַ��Ϣ�������������
//#endif //LWIP_DHCP
		lv_task_handler();//lvgl��������
		OSTimeDlyHMSM(0, 0, 0, 20);
	}
}


//led����
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
		OSTimeDlyHMSM(0, 0, 0, 500);  //��ʱ500ms
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
