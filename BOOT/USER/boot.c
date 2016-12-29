#include "sys.h"
#include "stmflash.h"  
#include "boot.h"
#include "usart.h"
u8 App1_Flag[10]={0X00,0X11,0X22,0X33,0X44,0X55,0X66,0X77,0X88,0X99};
u8 App2_Flag[10]={0X99,0X88,0X77,0X66,0X55,0X44,0X33,0X22,0X11,0X00};
/*stm32f103vet6 flash �ռ�滮  256K*/
/*
����                         ��ʼ��ַ
BOOT��         50K           0x8000000
APP1��         100K          0x800C800
APP2��         100K          0x8025800
������������ 6K            0x803E800
*/


/*
void write_bin(u32 offset_addr,u8 *bin_data , u32 number ,u8 app);
����APP����
����˵��:
offset_addr�� APP bin ƫ�Ƶ�ַ
bin_data   �� bin����ָ��
number     �� ��Ҫд���bin���ݸ���
app        �� 1 ��APP1  2��APP2
*/
void write_bin(u32 offset_addr,void *bin_data , u32 number ,u8 app)
{
	u32 addr=0;
	
	if(app==1)
		addr=BOOT_APP1;
	else if(app==2)
		addr=BOOT_APP2;
	
	STMFLASH_Write(addr+offset_addr,bin_data,number/2);
}


#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif	
void Wipe_App1(void)
{
	u32 secpos;	   //������ַ
	FLASH_Unlock();		
	for(secpos=50/2;secpos<150/2;secpos++)
	FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);
	FLASH_Lock();//����
}


void Wipe_App2(void)
{
	u32 secpos;	   //������ַ
	FLASH_Unlock();		
	for(secpos=150/2;secpos<250/2;secpos++)
	FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);
	FLASH_Lock();//����
}



/*����*/
u8 Boot_App1(void)
{
		if(((*(vu32*)(BOOT_APP1+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
		{	 
			iap_load_app(BOOT_APP1);//ִ��FLASH APP����
			return  1;
		}
    else
		{			
      printf("APP1  ����ʧ�ܣ��� \r\n");	//�����������˵������ʧ��	
		  return 0;
		}
}



u8 Boot_App2(void)
{
		if(((*(vu32*)(BOOT_APP2+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
		{	 
			iap_load_app(BOOT_APP2);//ִ��FLASH APP����
			return  1;
		}
    else
		{			
      printf("APP2  ����ʧ�ܣ��� \r\n");	//�����������˵������ʧ��	
		  return 0;
		}
}


u8 Boot_Boot(void)
{
		if(((*(vu32*)(BOOT_APP2+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
		{	 
			iap_load_app(BOOT_ADDR);//ִ��FLASH APP����
			return  1;
		}
    else
		{			
      printf("BOOT  ����ʧ�ܣ��� \r\n");	//�����������˵������ʧ��	
		  return 0;
		}
}



/*
д��APP1��־
ע��:��BOOT������  ���鲻Ҫ��BOOT��ʹ��
������������󣬽���APP1ʹ��  д���־
*/
void Set_APP1_Flag(void)
{
	write_bin(0x803E800,App1_Flag,10,0);
}



/*
д��APP2��־
ע��:��BOOT������  ���鲻Ҫ��BOOT��ʹ��
������������󣬽���APP2ʹ��  д���־
*/
void Set_APP2_Flag(void)
{
	write_bin(0x803E800+10,App2_Flag,10,0);
}



/*
����APP1������ɱ�־
*/
void Wipe_APP1_Flag(void)
{
	u8 Temp[10]={0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF};
	write_bin(0x803E800,Temp,10,0);
}



/*
����APP2������ɱ�־
*/
void Wipe_APP2_Flag(void)
{
	u8 Temp[10]={0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF};
	write_bin(0x803E800+10,Temp,10,0);
}



/*
BOOT������֤��������
������֤APP1�Ƿ�������APP2
*/
char Start_load(void)
{
	u8 app_flag_f,i;
	u16 pBuffer[5];
	STMFLASH_Read(0x803E800,pBuffer,5);
	app_flag_f=0;
	for(i=0;i<10;i++)
	{
	if(((pBuffer[i/2]>>((i%2)*8))&0xFF)!=App1_Flag[i])
		app_flag_f=1;
  }
	if(app_flag_f==0)
	{
		printf("Bootloader App1 !!\r\n");
		Boot_App1();
	}
		else if(app_flag_f==1)
	printf("None App1 !!\r\n");
	
		
	STMFLASH_Read(0x803E800+10,pBuffer,5);
	app_flag_f=0;
	for(i=0;i<10;i++)
	{
	if(((pBuffer[i/2]>>((i%2)*8))&0xFF)!=App2_Flag[i])
		app_flag_f=1;
  }
	if(app_flag_f==0)
	{
		printf("Bootloader App2 !!\r\n");
		Boot_App2();
	}
	else if(app_flag_f==1)
	printf("None App2 !!\r\n");
	
	return 0;
}



//��ת��Ӧ�ó����
//appxaddr:�û�������ʼ��ַ.
iapfun jump2app; 
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(vu32*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}
}	
