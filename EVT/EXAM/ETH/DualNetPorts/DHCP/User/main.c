/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/02/05
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
DHCP example, demonstrating that DHCP automatically obtains an IP address.
 */
#include "string.h"
#include "eth_driver.h"
#include "CH390.h"
#define HTONL(x) ((((x) & (uint32_t)0x000000ffUL) << 24) | \
                     (((x) & (uint32_t)0x0000ff00UL) <<  8) | \
                     (((x) & (uint32_t)0x00ff0000UL) >>  8) | \
                     (((x) & (uint32_t)0xff000000UL) >> 24))

u8 MACAddr[2][6];                                                   //MAC address
u8 IPAddr[2][4]   = {{0, 0, 0, 0}, {0, 0, 0, 0}};         //IP address
u8 GWIPAddr[2][4] = {{192, 168, 1, 1}, {192, 168, 1, 1}};           //Gateway IP address
u8 IPMask[2][4]   = {{255, 255, 255, 0}, {255, 255, 255, 0}};       //subnet mask
u8 DESIP[2][4]    = {{192, 168, 1, 100}, {192, 168, 1, 100}};       //destination IP address

struct _WCH_IPv6_Addr IPv6Addr = {
        {0x0,0x0,0x0,0x0},
        0
};

struct _WCH_IPv6_Addr DesIPv6Addr = {
        {0xFE800000,0x00000000,0x58b5a5f1,0x062cd266},
        0
};

struct _WCH_IPv6_Addr DesIPv6Addr1 = {
        {0xFE800000,0x00000000,0x58b5a5f1,0x062cd267},
        0
};
u16 desport = 1000;                                                 //destination port,Each time it is used, it will increment by one
u16 srcport = 1000;                                                 //source port,Each time it is used, it will increment by one

u8 SocketId1[NETIF_NUM], SocketId2[NETIF_NUM];
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];  //socket receive buffer
u8 MyBuf[RECE_BUF_LEN];

/*********************************************************************
 * @fn      mStopIfError
 *
 * @brief   check if error.
 *
 * @param   iError - error constants.
 *
 * @return  none
 */
void mStopIfError(u8 iError)
{
    if (iError == WCHNET_ERR_SUCCESS)
        return;
    printf("Error: %02X\r\n", (u16) iError);
}

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Initializes TIM2.
 *
 * @return  none
 */
void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = { 0 };

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = SystemCoreClock / 1000000;
    TIM_TimeBaseStructure.TIM_Prescaler = WCHNETTIMERPERIOD * 1000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    NVIC_EnableIRQ(TIM2_IRQn);
}

/*********************************************************************
 * @fn      WCHNET_CreateTcpSocket
 *
 * @brief   Create TCP Socket
 *
 * @return  none
 */
void WCHNET_CreateTcpSocket_IPv4(uint8_t NetIfIndex, const uint8_t *DesIPv4)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    TmpSocketInf.DesPort = desport++;
    TmpSocketInf.SourPort = srcport++;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    TmpSocketInf.netindex = NetIfIndex;
    memcpy((void *) TmpSocketInf.IPv4DesAddr, DesIPv4, 4);
    TmpSocketInf.IPVersion = IP_V4;
    i = WCHNET_SocketCreat(&SocketId1[NetIfIndex], &TmpSocketInf);
    mStopIfError(i);
    if (i != WCHNET_ERR_SUCCESS) {
        return;
    }
    printf("SocketId %d\r\n", SocketId1[NetIfIndex]);
    i = WCHNET_SocketConnect(SocketId1[NetIfIndex]);                        //make a TCP connection
    mStopIfError(i);
}

/*********************************************************************
 * @fn      WCHNET_CreateTcpSocket_IPv6
 *
 * @brief   Create TCP Socket based on IPv6
 *
 * @return  none
 */
void WCHNET_CreateTcpSocket_IPv6(uint8_t NetIfIndex, uint8_t AddrIndex, const uint32_t *DesIPv6)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    TmpSocketInf.DesPort = desport++;
    TmpSocketInf.SourPort = srcport++;
    TmpSocketInf.ProtoType = PROTO_TYPE_TCP;
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    TmpSocketInf.netindex = NetIfIndex;
    memcpy((void *) TmpSocketInf.IPv6DesAddr, DesIPv6, 16);
    TmpSocketInf.IPVersion = IP_V6;
    TmpSocketInf.IPv6SrcAddrIndex = AddrIndex;
    i = WCHNET_SocketCreat(&SocketId2[NetIfIndex], &TmpSocketInf);
	mStopIfError(i);
	if (i != WCHNET_ERR_SUCCESS) {
        return;
    }
    printf("SocketId %d\r\n", SocketId2[NetIfIndex]);
    i = WCHNET_SocketConnect(SocketId2[NetIfIndex]);                        //make a TCP connection
    mStopIfError(i);
}
/*********************************************************************
 * @fn      WCHNET_DataLoopback
 *
 * @brief   Data loopback function.
 *
 * @param   id - socket id.
 *
 * @return  none
 */
void WCHNET_DataLoopback(u8 id)
{
#if 1
    u8 i;
    u32 len;
    u32 endAddr = SocketInf[id].RecvStartPoint + SocketInf[id].RecvBufLen;       //Receive buffer end address

    if ((SocketInf[id].RecvReadPoint + SocketInf[id].RecvRemLen) > endAddr) {    //Calculate the length of the received data
        len = endAddr - SocketInf[id].RecvReadPoint;
    }
    else {
        len = SocketInf[id].RecvRemLen;
    }
    if(len)
    {
        i = WCHNET_SocketSend(id, (u8 *) SocketInf[id].RecvReadPoint, &len);         //send data
        if (i == WCHNET_ERR_SUCCESS) {
            WCHNET_SocketRecv(id, NULL, &len);                                       //Clear sent data
        }
    }

#else
    u32 len, totallen;
    u8 *p = MyBuf, TransCnt = 255;

    len = WCHNET_SocketRecvLen(id, NULL);                                //query length
    printf("Receive Len = %lu\r\n", (unsigned long) len);
    totallen = len;
    WCHNET_SocketRecv(id, MyBuf, &len);                                  //Read the data of the receive buffer into MyBuf
    while(1){
        len = totallen;
        WCHNET_SocketSend(id, p, &len);                                  //Send the data
        totallen -= len;                                                 //Subtract the sent length from the total length
        p += len;                                                        //offset buffer pointer
        if( !--TransCnt )  break;                                        //Timeout exit
        if(totallen) continue;                                           //If the data is not sent, continue to send
        break;                                                           //After sending, exit
    }
#endif
}

/*********************************************************************
 * @fn      WCHNET_HandleSockInt
 *
 * @brief   Socket Interrupt Handle
 *
 * @param   socketid - socket id.
 *          intstat - interrupt status
 *
 * @return  none
 */
void WCHNET_HandleSockInt(u8 socketid, u8 intstat)
{
    if (intstat & SINT_STAT_RECV)                                               //receive data
    {
        WCHNET_DataLoopback(socketid);                                          //Data loopback
    }
    if (intstat & SINT_STAT_CONNECT)                                            //connect successfully
    {
        WCHNET_ModifyRecvBuf(socketid, (u32) SocketRecvBuf[socketid], RECE_BUF_LEN);
        printf("TCP Connect Success\r\n");
    }
    if (intstat & SINT_STAT_DISCONNECT)                                         //disconnect
    {
        printf("TCP Disconnect\r\n");
        // WCHNET_ExchangeNetIfTxRxHandlers();
        WCHNET_CreateTcpSocket_IPv4(SocketInf[socketid].netindex, DESIP[0]);    //Create a TCP connection
    }
    if (intstat & SINT_STAT_TIM_OUT)                                            //timeout disconnect
    {
        printf("TCP Timeout\r\n");
        WCHNET_CreateTcpSocket_IPv4(SocketInf[socketid].netindex, DESIP[0]); 
    }
}

/*********************************************************************
 * @fn      WCHNET_HandleGlobalInt
 *
 * @brief   Global Interrupt Handle
 *
 * @return  none
 */
void WCHNET_HandleGlobalInt(void)
{
    u8 intstat;
    u16 i;
    u8 socketint;

    for(uint8_t netif_indx = 0; netif_indx < NETIF_NUM; netif_indx++)
    {
        intstat = WCHNET_GetGlobalInt(netif_indx);                            //get global interrupt flag
        if (intstat & GINT_STAT_UNREACH)                            //Unreachable interrupt
        {
            printf("GINT_STAT_UNREACH\r\n");
        }
        if (intstat & GINT_STAT_IP_CONFLI)                          //IP conflict
        {
            printf("GINT_STAT_IP_CONFLI\r\n");
        }
        if (intstat & GINT_STAT_PHY_CHANGE)                         //PHY status change
        {
            i = WCHNET_GetPHYStatus(netif_indx);
            if (i & PHY_Linked_Status)
                printf("Net %d PHY Link Success\r\n", netif_indx);
        }
        if (intstat & GINT_STAT_SOCKET) {                           //socket related interrupt
            for (i = 0; i < WCHNET_MAX_SOCKET_NUM; i++) {
                socketint = WCHNET_GetSocketInt(i);
                if (socketint)
                    WCHNET_HandleSockInt(i, socketint);
            }
        }
    }
}

/*********************************************************************
 * @fn      WCHNET_DHCPCallBack
 *
 * @brief   DHCPCallBack
 *
 * @param   status - status returned by DHCP
 *                   0x00 - Success
 *                   0x01 - Failure
 *          arg - Data returned by DHCP
 *
 * @return  DHCP status
 */
u8 WCHNET_DHCPCallBack(u8 status, u8 NetIfIndex, void *arg)
{
    u8 *p;
    u8 tmp[4] = {0, 0, 0, 0};

    if(!status)
    {
        p = arg;
        printf("DHCP Success\r\n");
        /*If the obtained IP is the same as the last IP, exit this function.*/
        if(!memcmp(IPAddr[NetIfIndex], p ,sizeof(IPAddr[NetIfIndex])))
            return READY;
        /*Determine whether it is the first successful IP acquisition*/
        if(memcmp(IPAddr[NetIfIndex], tmp ,sizeof(IPAddr[NetIfIndex]))){
            /*The obtained IP is different from the last value,
             * then disconnect the last connection.*/
            WCHNET_SocketClose(SocketId1[NetIfIndex], TCP_CLOSE_NORMAL);
        }
        memcpy(IPAddr[NetIfIndex], p, 4);
        memcpy(GWIPAddr[NetIfIndex], &p[4], 4);
        memcpy(IPMask[NetIfIndex], &p[8], 4);
        printf("IPAddr: %d.%d.%d.%d \r\n", (u16)IPAddr[NetIfIndex][0], (u16)IPAddr[NetIfIndex][1],
               (u16)IPAddr[NetIfIndex][2], (u16)IPAddr[NetIfIndex][3]);
        printf("GWIPAddr: %d.%d.%d.%d \r\n", (u16)GWIPAddr[NetIfIndex][0], (u16)GWIPAddr[NetIfIndex][1],
               (u16)GWIPAddr[NetIfIndex][2], (u16)GWIPAddr[NetIfIndex][3]);
        printf("IPMask: %d.%d.%d.%d \r\n", (u16)IPMask[NetIfIndex][0], (u16)IPMask[NetIfIndex][1],
               (u16)IPMask[NetIfIndex][2], (u16)IPMask[NetIfIndex][3]);
        printf("DNS1: %d.%d.%d.%d \r\n", p[12], p[13], p[14], p[15]);
        printf("DNS2: %d.%d.%d.%d \r\n", p[16], p[17], p[18], p[19]);
        WCHNET_CreateTcpSocket_IPv4(NetIfIndex, DESIP[NetIfIndex]);                  //Create a TCP connection
        return READY;
    }
    else
    {
        printf("DHCP Fail %02x \r\n", status);
        /*Determine whether it is the first successful IP acquisition*/
        if(memcmp(IPAddr[NetIfIndex], tmp ,sizeof(IPAddr[NetIfIndex]))){
            /*The obtained IP is different from the last value*/
            WCHNET_SocketClose(SocketId1[NetIfIndex], TCP_CLOSE_NORMAL);
        }
        return NoREADY;
    }
}

/*********************************************************************
 * @fn      WCHNET_DHCPv6CallBack
 *
 * @brief   DHCPCallBack
 *
 * @param   status - status returned by DHCP
 *                   0x00 - Success
 *                   0x01 - Failure
 *          arg - Data returned by DHCP
 *
 * @return  DHCP status
 */
u8 WCHNET_DHCPv6CallBack(u8 status, u8 NetIfIndex, void *arg)
{
    u32 *p;

    if(!status)
    {
        p = arg;
        printf("DHCPv6 Success\r\n");
        IPv6Addr.IPv6Addr[0] = HTONL(p[0]);
        IPv6Addr.IPv6Addr[1] = HTONL(p[1]);
        IPv6Addr.IPv6Addr[2] = HTONL(p[2]);
        IPv6Addr.IPv6Addr[3] = HTONL(p[3]);
        IPv6Addr.index = 1;

        printf("IPv6Addr: %x.%x.%x.%x \r\n", IPv6Addr.IPv6Addr[0], IPv6Addr.IPv6Addr[1],\
                                             IPv6Addr.IPv6Addr[2], IPv6Addr.IPv6Addr[3]);
        printf("DNS1: %x.%x.%x.%x \r\n", HTONL(p[4]), HTONL(p[5]), HTONL(p[6]), HTONL(p[7]));
        printf("DNS2: %x.%x.%x.%x \r\n", HTONL(p[8]), HTONL(p[9]), HTONL(p[10]), HTONL(p[11]));
        WCHNET_CreateTcpSocket_IPv6(NetIfIndex, IPv6Addr.index, DesIPv6Addr.IPv6Addr);                            //Create a TCP connection
        return READY;
    }
    else
    {
        printf("DHCP Fail %02x \r\n", status);
        return NoREADY;
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program
 *
 * @return  none
 */
int main(void)
{
    u8 i;

    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);                                            //USART initialize
    printf("DHCP Test\r\n");  	
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("net version:%x\n", WCHNET_GetVer());
    if (WCHNET_LIB_VER != WCHNET_GetVer()) {
        printf("version error.\n");
    }
    WCHNET_GetMacAddr(MACAddr[0]);                                 //get the chip MAC address
    printf("mac0 addr:");
    for(i = 0; i < 6; i++)
        printf("%x ", MACAddr[0][i]);
    printf("\n");

    CH390TaskInit(MACAddr[1]);
    printf("mac1 addr:");
    for(i = 0; i < 6; i++)
        printf("%x ", MACAddr[1][i]);
    printf("\n");

    TIM2_Init();
    i = ETH_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);                      //Ethernet library initialize
    mStopIfError(i);
    if(i == WCHNET_ERR_SUCCESS)
        printf("WCHNET_LibInit Success\r\n");
	else {
        printf("WCHNET_LibInit failed, halted\r\n");
        while(1);
    }
    i = WCHNET_DHCPStart(WCHNET_DHCPCallBack, NETIF_INDEX);
    mStopIfError(i); 
    i = WCHNET_DHCPv6Start(WCHNET_DHCPv6CallBack, NETIF_INDEX);
    mStopIfError(i);                           
    i = WCHNET_DHCPStart(WCHNET_DHCPCallBack, NETIF1_INDEX);
    mStopIfError(i);                                  
    i = WCHNET_DHCPv6Start(WCHNET_DHCPv6CallBack, NETIF1_INDEX);
    mStopIfError(i);                            
    while(1)
    {
        /*Ethernet library main task function,
         * which needs to be called cyclically*/
        WCHNET_MainTask();
        /*Query the Ethernet global interrupt,
         * if there is an interrupt, call the global interrupt handler*/
        if(WCHNET_QueryGlobalInt(NETIF_INDEX) | WCHNET_QueryGlobalInt(NETIF1_INDEX) )
        {
            WCHNET_HandleGlobalInt();
        }
        CH390Task();
    }
}