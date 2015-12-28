//头文件调用
#include "usart.h"
#include "PICC.h"

//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 0
/*#pragma import(__use_no_semihosting)*/
//标准库需要的支持函数
struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};
/* FILE is typedef’ d in stdio.h. */
FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
_sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int Data, FILE *f)
{
    while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));   //USART_GetFlagStatus：得到发送状态位
    //USART_FLAG_TXE:发送寄存器为空 1：为空；0：忙状态
    USART_SendData(USART1,Data);        //发送一个字符

    return Data;            //返回一个值
}
#endif


USART_HandleTypeDef ConsoleUSART_Handle;
USART_HandleTypeDef RFIDUSART_Handle;
USART_HandleTypeDef LCDUSART_Handle;
#define RFIDUSART_IRQHandler USART2_IRQHandler

HAL_StatusTypeDef USART_SendByte(USART_HandleTypeDef *p_USARTHandle, uint8_t DATA)
{
    __HAL_LOCK((p_USARTHandle));
    WRITE_REG((p_USARTHandle)->Instance->DR, ((DATA) & (uint8_t)0xFF));
    __HAL_UNLOCK((p_USARTHandle));
    return HAL_OK;
}

void HAL_USART_MspInit(USART_HandleTypeDef *husart)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;

    if(husart->Instance == USART1)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();

        GPIO_InitStructure.Pin = GPIO_PIN_9;
        GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.Pin = GPIO_PIN_10;
        GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    }

    if(husart->Instance == USART2)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART2_CLK_ENABLE();
        GPIO_InitStructure.Pin = GPIO_PIN_2;
        GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_InitStructure.Pin = GPIO_PIN_3;
        GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

        HAL_NVIC_SetPriority(USART2_IRQn, 2, 1);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }

    if(husart->Instance == USART3)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_USART3_CLK_ENABLE();

        GPIO_InitStructure.Pin = GPIO_PIN_10;
        GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

        GPIO_InitStructure.Pin = GPIO_PIN_11;
        GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    }
}

void DebugLogConsoleConfig(USART_TypeDef * USART_X)
{
    ConsoleUSART_Handle.Instance            = USART_X;
    ConsoleUSART_Handle.Init.BaudRate       = 115200 ;
    ConsoleUSART_Handle.Init.WordLength     = USART_WORDLENGTH_8B;
    ConsoleUSART_Handle.Init.StopBits       = USART_STOPBITS_1;
    ConsoleUSART_Handle.Init.Parity         = USART_PARITY_NONE;
    ConsoleUSART_Handle.Init.Mode           = USART_MODE_TX_RX;
    HAL_USART_Init(&ConsoleUSART_Handle);
}

void RFIDUSARTConfig(USART_TypeDef * USART_X)
{
    RFIDUSART_Handle.Instance            = USART_X;
    RFIDUSART_Handle.Init.BaudRate       = 9600 ;
    RFIDUSART_Handle.Init.WordLength     = USART_WORDLENGTH_8B;
    RFIDUSART_Handle.Init.StopBits       = USART_STOPBITS_1;
    RFIDUSART_Handle.Init.Parity         = USART_PARITY_NONE;
    RFIDUSART_Handle.Init.Mode           = USART_MODE_TX_RX;
    HAL_USART_Init(&RFIDUSART_Handle);
    __HAL_USART_ENABLE_IT(&RFIDUSART_Handle, USART_IT_RXNE);
}

void LCDUSARTConfig(USART_TypeDef * USART_X)
{
    LCDUSART_Handle.Instance            = USART_X;
    LCDUSART_Handle.Init.BaudRate       = 115200 ;
    LCDUSART_Handle.Init.WordLength     = USART_WORDLENGTH_8B;
    LCDUSART_Handle.Init.StopBits       = USART_STOPBITS_1;
    LCDUSART_Handle.Init.Parity         = USART_PARITY_NONE;
    LCDUSART_Handle.Init.Mode           = USART_MODE_TX_RX;
    HAL_USART_Init(&LCDUSART_Handle);
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: Usart_Configuration
 ** 功能描述: 串口配置函数
 ** 参数描述: BUSART_X表示串口号，baudRate设置波特率
 ** 作  　者: Dream
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Usart_Configuration(USART_TypeDef * USART_X, uint32_t BaudRate)
{
    USART_HandleTypeDef USART_HandleStructure;            //定义一个串口结构体

    USART_HandleStructure.Instance            = USART_X;
    USART_HandleStructure.Init.BaudRate       = BaudRate ;      //波特率115200
    USART_HandleStructure.Init.WordLength     = USART_WORDLENGTH_8B;  //传输过程中使用8位数据
    USART_HandleStructure.Init.StopBits       = USART_STOPBITS_1;   //在帧结尾传输1位停止位
    USART_HandleStructure.Init.Parity         = USART_PARITY_NONE;   //奇偶失能
    USART_HandleStructure.Init.Mode           = USART_MODE_RX | USART_MODE_TX; //接收和发送模式
    HAL_USART_Init(&USART_HandleStructure);        //根据参数初始化串口寄存器
    /*USART_ITConfig(USART_X,USART_IT_RXNE,ENABLE);*/ //FIXME: 使能串口中断
    /*USART_Cmd(USART_X, ENABLE);          //使能串口外设*/
    __HAL_USART_ENABLE(&USART_HandleStructure);
}

/*
 *void USART1_IRQHandler()
 *{
 *  HAL_USART_IRQHandler(&ConsoleUSART_Handle); //TODO: find the USART1_Handler
 *}
 */

 //FIXME
void RFIDUSART_IRQHandler(void)
{
    USART_HandleTypeDef * husart = (USART_HandleTypeDef*) RFIDUSART_IRQHandler; //TODO: movo out of handler
    uint32_t tmp_flag = 0, tmp_it_source = 0;

    static unsigned char bTemp, flag=0;
    static unsigned char sflag = 0;

    /* 串口接收 */
    /*if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)*/
    tmp_flag = __HAL_USART_GET_FLAG(husart, USART_FLAG_RXNE);
    tmp_it_source = __HAL_USART_GET_IT_SOURCE(husart, USART_IT_RXNE);
    if((tmp_flag != RESET) && (tmp_it_source != RESET))
    {
        /* 清串口中断标记 */
        /*bTemp = USART_ReceiveData(USART2);*/
        bTemp = (uint8_t)(husart->Instance->DR & (uint8_t)0x00FF); //TODO
        /* 上条命令处理完成才接收 */
        if(Cmd.ReceiveFlag == 0)
        {
            /* 7F 标记，为 0 表示上一个数据不是7F */
            if(flag == 0)
            {
                /* 上一个标记不是7F，这个是，打上标记 */
                if(bTemp == 0x7F){flag = 1;}
                /* 把值存进接收缓存 */
                Cmd.ReceiveBuffer[Cmd.ReceivePoint++] = bTemp;
            }
            else
            {
                flag = 0;
                /* 上一个是7F，这一个不是表示收到【命令头】 */
                if(bTemp != 0x7F)
                {
                    Cmd.ReceivePoint = 0;
                    Cmd.ReceiveBuffer[Cmd.ReceivePoint++] = bTemp;
                }
                /* 上一个是7F，这一个也是，则忽略当前的7F */
            }
            if(Cmd.ReceivePoint >= 32){Cmd.ReceivePoint = 0;}
            /* 接收指针大于 2 个开始对比命令长度和接收指针，一致表示收到完整命令 */
            if(Cmd.ReceivePoint > 2){if(Cmd.ReceivePoint == Cmd.ReceiveBuffer[0]+1){Cmd.ReceiveFlag = 1;}}
        }
    }
    /* 发送 */
    /*if(USART_GetFlagStatus(USART2, USART_FLAG_TC) == SET)*/
    tmp_flag = __HAL_USART_GET_FLAG(husart, USART_FLAG_TC);
    tmp_it_source = __HAL_USART_GET_IT_SOURCE(husart, USART_IT_TC);
    if((tmp_flag != RESET) && (tmp_it_source != RESET))
    {
        /*USART_ClearFlag(USART2, USART_FLAG_TC);*/
        __HAL_USART_CLEAR_FLAG(husart, USART_FLAG_TC);
        /* 发送指针不为0时继续发送 */
        if(Cmd.SendPoint != 0)
        {
            /* 7F判断 */
            if(sflag == 0)
            {
                Cmd.SendPoint--;
                /*USART_SendData(USART2, Cmd.SendBuffer[Cmd.SendBuffer[0] - Cmd.SendPoint]);*/
                WRITE_REG(husart->Instance->DR, Cmd.SendBuffer[Cmd.SendBuffer[0] - Cmd.SendPoint] & (uint8_t) 0xFF);
                if(Cmd.SendBuffer[Cmd.SendBuffer[0] - Cmd.SendPoint] == 0x7F){sflag = 1;}
            }
            else
            {
                sflag = 0;
                /*USART_SendData(USART2, 0x7F);*/
                WRITE_REG(husart->Instance->DR, 0x7F);
            }
        }
        /* 发送指针为0时打上发送标记表示发送完成 */
        else
        {
            Cmd.SendFlag = 0;
        }
    }
}

/*:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 ** 函数名称: Lcd_Display
 ** 功能描述: LCD显示成像
 ** 参数描述: 无
 ** 作  　者: Dream
 ** 日　  期: 2011年6月20日
 :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::*/
void Lcd_Display(char * buf1)
{
    int length = strlen(buf1);
    HAL_USART_Transmit(&LCDUSART_Handle, (unsigned char *)buf1, length, 5000);
}

