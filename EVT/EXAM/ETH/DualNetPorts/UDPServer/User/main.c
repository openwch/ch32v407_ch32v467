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
UDP Server example, demonstrating that the UDP Server receives data and loops back.
For details on the selection of engineering chips,
please refer to the "CH32V30x Evaluation Board Manual" under the CH32V307EVT\EVT\PUB folder.
 */
#include "string.h"
#include "eth_driver.h"
#include "CH390.h"

#define UDP_RECE_BUF_LEN                                    1472
#define UDP_SERVER_PORT                                     1000
u8 MACAddr[2][6];                                                   //MAC address
u8 IPAddr[2][4]   = {{192, 168, 1, 10}, {192, 168, 1, 20}};         //IP address
u8 GWIPAddr[2][4] = {{192, 168, 1, 1}, {192, 168, 1, 1}};           //Gateway IP address
u8 IPMask[2][4]   = {{255, 255, 255, 0}, {255, 255, 255, 0}};       //subnet mask
u8 DESIP[2][4]    = {{192, 168, 1, 100}, {192, 168, 1, 100}};       //destination IP address

struct _WCH_IPv6_Addr IPv6Addr1 = {
        {0xFE800000,0x00000000,0x3a3b26ff,0xfe3e6de1},
        0
};
struct _WCH_IPv6_Addr IPv6Addr2 = {
        {0xFE800000,0x00000000,0x3a3b26ff,0xfe3e6de2},
        0
};

struct _WCH_IPv6_Addr IPv6Addr3 = {
        {0xFE800000,0x00000000,0x3a3b26ff,0xfe3e6de3},
        0
};
struct _WCH_IPv6_Addr IPv6Addr4 = {
        {0xFE800000,0x00000000,0x3a3b26ff,0xfe3e6de4},
        0
};

struct _WCH_IPv6_Addr DesIPv6Addr = {
        {0xFE800000,0x00000000,0x58b5a5f1,0x062cd266},
        0
};

struct _WCH_IPv6_Addr DesIPv6Addr1 = {
        {0xFE800000,0x00000000,0x58b5a5f1,0x062cd266},
        0
};

u8 SocketId;
u8 SocketBufIndex = 0;                      //index of socket receive buffer
u8 SocketRecvBuf[WCHNET_MAX_SOCKET_NUM][RECE_BUF_LEN];  //socket receive buffer
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
 * @fn      WCHNET_UdpServerRecv
 *
 * @brief   UDP Receive data function
 *
 *@param    socinf - socket information.
 *          ipaddr - The IP address from which the data was sent
 *          port - source port
 *          buf - pointer to the data buffer
 *          len - received data length
 * @return  none
 */
void WCHNET_UdpServerRecv(struct _SOCK_INF *socinf, u32 *ipaddr, uint8_t IPVersion, u16 port, u8 *buf, u32 len)
{
    u8 ipv4_addr[4], i;
    uint32_t ipv6_addr[4];

    if(IPVersion == IP_V4)
    {
        printf("Remote IP = ");
        for (i = 0; i < 4; i++) {
            ipv4_addr[i] = *ipaddr & 0xff;
            printf("%d ", ipv4_addr[i]);
            *ipaddr = *ipaddr >> 8;
        }
        printf("srcport = %d len = %d socketid = %d\r\n", port, len,
                socinf->SockIndex);

        WCHNET_SocketUdpSendTo(socinf->SockIndex, buf, &len, (uint32_t *)ipv4_addr, IPVersion, port, socinf->netindex);
    }
    else if(IPVersion == IP_V6)
    {
        printf("Remote IP = ");
        for (i = 0; i < 4; i++) {
            ipv6_addr[i] = ((ipaddr[i] & 0xff000000)>>24) |\
                            ((ipaddr[i] & 0x00ff0000)>>8) |\
                            ((ipaddr[i] & 0x0000ff00)<<8) |\
                            ((ipaddr[i] & 0x000000ff)<<24);
            printf("%x ", ipv6_addr[i]);
        }
        printf("srcport = %d len = %d socketid = %d\r\n", port, len,
                socinf->SockIndex);

        WCHNET_SocketUdpSendTo(socinf->SockIndex, buf, &len, (uint32_t *)ipv6_addr, IPVersion, port, socinf->netindex);

    }
    else return;

}

/*********************************************************************
 * @fn      WCHNET_CreateUdpSocket_IPv4
 *
 * @brief   Create UDP Socket based on IPv4
 *
 * @return  none
 */
void WCHNET_CreateUdpSocket_IPv4(uint8_t NetIfIndex)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    TmpSocketInf.SourPort = UDP_SERVER_PORT;
    TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
    TmpSocketInf.RecvStartPoint = (u32) SocketRecvBuf[SocketBufIndex++];
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    TmpSocketInf.netindex = NetIfIndex;
    TmpSocketInf.AppCallBack = WCHNET_UdpServerRecv;
    TmpSocketInf.IPVersion = IP_V4;
    i = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
    if (i != WCHNET_ERR_SUCCESS) {
        mStopIfError(i);
        return;
    }
    printf("WCHNET_SocketCreat %d\r\n", SocketId);
}

/*********************************************************************
 * @fn      WCHNET_CreateUdpSocket_IPv6
 *
 * @brief   Create UDP Socket based on IPv6
 *
 * @return  none
 */
void WCHNET_CreateUdpSocket_IPv6(uint8_t NetIfIndex, uint8_t AddrIndex)
{
    u8 i;
    SOCK_INF TmpSocketInf;

    memset((void *) &TmpSocketInf, 0, sizeof(SOCK_INF));
    TmpSocketInf.SourPort = UDP_SERVER_PORT;
    TmpSocketInf.ProtoType = PROTO_TYPE_UDP;
    TmpSocketInf.RecvStartPoint = (u32) SocketRecvBuf[SocketBufIndex++];
    TmpSocketInf.RecvBufLen = RECE_BUF_LEN;
    TmpSocketInf.netindex = NetIfIndex;
    TmpSocketInf.AppCallBack = WCHNET_UdpServerRecv;
    TmpSocketInf.IPVersion = IP_V6;
    TmpSocketInf.IPv6SrcAddrIndex = AddrIndex;
    i = WCHNET_SocketCreat(&SocketId, &TmpSocketInf);
    if (i != WCHNET_ERR_SUCCESS) {
        mStopIfError(i);
        return;
    }
    printf("WCHNET_SocketCreat %d\r\n", SocketId);
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
    if (intstat & SINT_STAT_RECV)                               //receive data
    {
    }
    if (intstat & SINT_STAT_CONNECT)                            //connect successfully
    {
        printf("TCP Connect Success\r\n");
    }
    if (intstat & SINT_STAT_DISCONNECT)                         //disconnect
    {
        printf("TCP Disconnect\r\n");
    }
    if (intstat & SINT_STAT_TIM_OUT)                            //timeout disconnect
    {
        printf("TCP Timeout\r\n");
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
    USART_Printf_Init(115200);                                    //USART initialize
    printf("UDPServer Test\r\n");
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
    i = ETH_LibInit(IPAddr, GWIPAddr, IPMask, MACAddr);         //Ethernet library initialize
    mStopIfError(i);
    if (i == WCHNET_ERR_SUCCESS)
        printf("WCHNET_LibInit Success\r\n");
	else {
        printf("WCHNET_LibInit failed, halted\r\n");
        while(1);
    }
        
    /* support 1 linklocal address and 2 unicast addresses.
     * When this function is called, the linklocal
     * address is automatically created,
     * and the index of the address is 0.*/
    WCHNET_AddIPV6Addr(&IPv6Addr1,NETIF_INDEX);
    printf("netif%d ipv6 index %d\r\n", NETIF_INDEX, IPv6Addr1.index);
    WCHNET_AddIPV6Addr(&IPv6Addr2, NETIF_INDEX);
    printf("netif%d ipv6 index %d\r\n", NETIF_INDEX, IPv6Addr2.index);
    WCHNET_AddIPV6Addr(&IPv6Addr3,NETIF1_INDEX);
    printf("netif%d ipv6 index %d\r\n", NETIF1_INDEX, IPv6Addr3.index);
    WCHNET_AddIPV6Addr(&IPv6Addr4, NETIF1_INDEX);
    printf("netif%d ipv6 index %d\r\n", NETIF1_INDEX, IPv6Addr4.index);

    WCHNET_CreateUdpSocket_IPv4(NETIF_INDEX);                   //Create UDP Socket
    WCHNET_CreateUdpSocket_IPv6(NETIF_INDEX, IPv6Addr1.index);
    WCHNET_CreateUdpSocket_IPv6(NETIF_INDEX, IPv6Addr2.index);
    WCHNET_CreateUdpSocket_IPv4(NETIF1_INDEX);                  //Create UDP Socket
    WCHNET_CreateUdpSocket_IPv6(NETIF1_INDEX, IPv6Addr3.index);
    WCHNET_CreateUdpSocket_IPv6(NETIF1_INDEX, IPv6Addr4.index);
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