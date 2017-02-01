#include "common.h"
#include "include.h"

#define DATALINE                 CAMERA_H
#define DATACOUNT                CAMERA_W
#define white 255          //255��Ӧͼ���ɫ
#define black 0            //0��Ӧͼ���ɫ
#define hang_max 60



void uint8send(uint8 ch)
{
     //�ȴ����ͻ�������
    while(!(UART_S1_REG(UARTN[FIRE_PORT]) & UART_S1_TDRE_MASK));

    //��������
    UART_D_REG(UARTN[FIRE_PORT]) = (uint8)ch;

}
extern uint8 img[CAMERA_H][CAMERA_W]; //��ά���飬��DMA�ɼ���ͼ������ɶ�ά����
void vcan_sendimg2(uint8 *imgaddr, uint32 imgsize)
{
#define CMD_IMG     1
    uint8 cmdf[2] = {CMD_IMG, ~CMD_IMG};    //ɽ����λ�� ʹ�õ�����
    uint8 cmdr[2] = {~CMD_IMG, CMD_IMG};    //ɽ����λ�� ʹ�õ�����

    uart_putbuff(FIRE_PORT, cmdf, sizeof(cmdf));    //�ȷ�������

    uart_putbuff(FIRE_PORT, imgaddr, imgsize); //�ٷ���ͼ��

    uart_putbuff(FIRE_PORT, cmdr, sizeof(cmdr));    //�ȷ�������
}
//����ͼ����λ����ʾ
//��ͬ����λ������ͬ������
//���ʹ��������λ��������Ҫ�޸Ĵ���

/***************************************************/
/*!
 *  @brief      ��ֵ��ͼ���ѹ���ռ� �� ʱ�� ��ѹ��
 *  @param      dst             ͼ���ѹĿ�ĵ�ַ
 *  @param      src             ͼ���ѹԴ��ַ
 *  @param      srclen          ��ֵ��ͼ���ռ�ÿռ��С
 *  @since      v5.0          
 */
void img_extract(uint8 *dst, uint8 *src, uint32 srclen)
{
    uint8 colour[2] = {white, black}; //0 �� 1 �ֱ��Ӧ����ɫ
    //ע��ɽ�������ͷ 0 ��ʾ ��ɫ��1��ʾ ��ɫ
    uint8 tmpsrc;
    while(srclen --)
    {
        tmpsrc = *src++;
        *dst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
    }
}

//������λ������
void com_pc(uint8 (*img)[CAMERA_W])
{
  uint8 i,j;
  uart_putchar(FIRE_PORT,0xff);
  for(i=0;i<DATALINE;i++)
  {
    for(j=0;j<DATACOUNT;j++)
    {
      if(img[i][j]==0xff)
      {
        img[i][j]=0xfe;
      }
      uart_putchar(FIRE_PORT,img[i][j]);
    }
  }
  
  uart_putchar(FIRE_PORT,0xff);
}

/*!
 *  @brief      ɽ��๦�ܵ���������λ��������ʾ������ʾ����
 *  @param      wareaddr    ����������ʼ��ַ
 *  @param      waresize    ��������ռ�ÿռ�Ĵ�С
 *  @since      v5.0
*  Sample usage:
             �����÷��ο�������:
            ��ɽ�����ϡ������Ǻͼ��ٶ� ��λ����ʾ���� - ���ܳ�������
             http://vcan123.com/forum.php?mod=viewthread&tid=6253&ctid=27
 */
void vcan_sendware(void *wareaddr, uint32_t waresize)
{
#define CMD_WARE     3               //00000011 --------11111100   252
    uint8_t cmdf[2] = {CMD_WARE, ~CMD_WARE};    //���ڵ��� ʹ�õ�ǰ����
    uint8_t cmdr[2] = {~CMD_WARE, CMD_WARE};    //���ڵ��� ʹ�õĺ�����

    uart_putbuff(UART1, cmdf, sizeof(cmdf));    //�ȷ���ǰ����
    uart_putbuff(UART1, (uint8_t *)wareaddr, waresize);    //��������
    uart_putbuff(UART1, cmdr, sizeof(cmdr));    //���ͺ�����

}
void send_img(void)
{
  uint8 i,j;
  uart_putchar(UART3,0xff);
  for(i=0;i<hang_max;i++)
  {
    for(j=0;j<CAMERA_W;j++)
    {
      if(img[i][j]==0xff)
      {
        img[i][j]=0xfe;
      }
      uart_putchar(UART3,img[i][j]);
    }
  }

}
void Up_send(int ch1,int ch2,int ch3)
{
    ch1*=10;
    ch2*=10;
    ch3*=10;
    
    uart_putchar(UART1,0XFF);//����֡ͷ
    uart_putchar(UART1,0XFF);
    
    uart_putchar(UART1,ch1/256);//ͨ��һ
    uart_putchar(UART1,ch1%256);
    
    uart_putchar(UART1,ch2/256);//ͨ����
    uart_putchar(UART1,ch2%256);
    
    uart_putchar(UART1,ch3/256);//ͨ����
    uart_putchar(UART1,ch3%256);
}

void uart_clear()
{
  char dispose;int i;
  for(i=0;i<8;i++)
  {
   if(uart_query (UART0) == 1)     //��ѯ�Ƿ���յ�����
    {
    uart_getchar (UART0,&dispose);  //�ȴ�����һ�����ݣ����浽 ch��
    }
   DELAY_MS(2);
  }
}
