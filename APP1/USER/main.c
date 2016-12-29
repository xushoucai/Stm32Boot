#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "stmflash.h"  
#include "boot.h"

void TIM2_Base_Init(u16 Count); 

int main(void)
{
//	/*TEST*/
//  Set_APP1_Flag();
//	Set_APP2_Flag();
//	/*TEST*/
//	Wipe_APP1_Flag();
//	Wipe_APP2_Flag();
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	
	delay_init();
	
	uart_init(115200);
	
//	Start_load();//��������BOOT����
	TIM2_Base_Init(10000);//1s һ���ж� ��֤ �ж��������Ƿ���ȷ
	printf("This is APP1 !! \r\n");
	
//	{
//	u8 bindata[10]={1,2,3,4,5,6,7,8,9,0};
//	u8 bindata1[10]={0,9,8,7,6,5,4,3,2,1};	
//	/*���Զ�д*/
//	write_bin(10,bindata1,10,1);//����
//	write_bin(0,bindata1,10,1);//����
//	
//	/*���Զ�д*/
//	write_bin(10,bindata1,10,2);//����
//	write_bin(0,bindata1,10,2);//����
//  }
	
	while(1)
	{
		
	}
}


/*-------------------------------------------------------------------------------
�������ƣ�TIM2_Base_Init
������������ʱ��TIM2ͨ�ö�ʱ����
�����������ʱ����װֵ
���ز�������
��    ע����
---------------------------------------------------------------------------------*/
void TIM2_Base_Init(u16 Count)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = Count-1;  //��װֵ��
	TIM_TimeBaseStructure.TIM_Prescaler =7200-1; //��Ƶϵ����72M/7200=10KHz,������������
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);     //��������ֵд���Ӧ�Ĵ���
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);            //ʹ�ܻ���ʧ��ָ����TIM�ж�
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;            //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);                 //��������ֵд���Ӧ�Ĵ�

	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����
							 
}


