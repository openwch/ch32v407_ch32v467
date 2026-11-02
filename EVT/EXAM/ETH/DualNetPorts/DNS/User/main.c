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
DNS example ,demonstrate that DHCP automatically obtains
an IP address and then requests domain name resolution.
 */
#include "string.h"
#include "eth_driver.h"
#include "CH390.h"
#define HTONL(x) ((((x) & (uint32_t)0x000000ffUL) << 24) | \
                     (((x) & (uint32_t)0x0000ff00UL) <<  8) | \
                     (((x) & (uint32_t)0x00ff0000UL) >>  8) | \
                     (((x) & (uint32_t)0xff000000UL) >> 24))

u8 MACAddr[2][6];                                                   //MAC address
u8 IPAddr[2][4]   = {{0, 0, 0, 0}, {0, 0, 0, 0}};                   //IP address
u8 GWIPAddr[2][4] = {{0, 0, 0, 0}, {0, 0, 0, 0}};                   //Gateway IP address
u8 IPMask[2][4]   = {{0, 0, 0, 0}, {0, 0, 0, 0}};                   //subnet mask
u8 DESIP[2][4]    = {{255, 255, 255, 255}, {255, 255, 255, 255}};   //destination IP address

u16 DnsPort = 53;
__attribute__((__aligned__(4))) u8  RemoteIPv4[NETIF_NUM][4];
__attribute__((__aligned__(4))) u8  RemoteIPv6[NETIF_NUM][16];

uint8_t DomainIPv4Addr[NETIF_NUM] = {4, 5};
uint8_t DomainIPv6Addr[NETIF_NUM] = {6, 7};

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
    if (intstat & SINT_STAT_RECV)                              //receive data
    {
    }
    if (intstat & SINT_STAT_CONNECT)                           //connect successfully
    {
        printf("TCP Connect Success\r\n");
    }
    if (intstat & SINT_STAT_DISCONNECT)                        //disconnect
    {
        printf("TCP Disconnect\r\n");
    }
    if (intstat & SINT_STAT_TIM_OUT)                           //timeout disconnect
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
 * @fn      WCHNET_DNSCallBack
 *
 * @brief   DNSCallBack
 *
 * @return  none
 */
void WCHNET_DNSCallBack(const char *name, u8 *ipaddr, void *callback_arg)
{
    if(ipaddr == NULL)
    {
        printf("DNS Fail:%x\r\n", (*(u8 *)callback_arg));
        return;
    }
    printf("Host Name: %s\r\n", name);

    if((callback_arg == &DomainIPv4Addr[0]) || (callback_arg == &DomainIPv4Addr[1]))         //IPv4
        printf("IP: %d.%d.%d.%d\r\n", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);
    else                                    //IPv6
    {
        uint32_t *p = (uint32_t *)ipaddr;
        printf("IPv6Addr: %x.%x.%x.%x \r\n", HTONL(p[0]), HTONL(p[1]), HTONL(p[2]), HTONL(p[3]));
    }
    if(callback_arg != NULL)
    {
        printf("callback_arg: %02x\r\n", (*(u8 *)callback_arg));
    }
    // WCHNET_DNSStop();                                                          //stop DNS,and release socket
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

    if(!status)
    {
        p = arg;
        printf("DHCP Success\r\n");
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
        WCHNET_InitDNS(&p[12], IP_V4, DnsPort);                                     //Set DNS server IP address, and DNS server port is 53
        if(NetIfIndex == 0)
        {
            WCHNET_HostNameGetIp("www.wch.cn", RemoteIPv4[NetIfIndex], WCHNET_DNSCallBack, &DomainIPv4Addr[NetIfIndex]);     //Start DNS
        }
        else 
        {
            WCHNET_HostNameGetIp("www.a.shifen.com", RemoteIPv4[NetIfIndex], WCHNET_DNSCallBack, &DomainIPv4Addr[NetIfIndex]);     //Start DNS
        }

        return READY;
    }
    else
    {
        printf("DHCP Fail %02x \r\n", status);
        return NoREADY;
    }
}

u8 WCHNET_DHCPv6CallBack(u8 status, u8 NetIfIndex, void *arg)
{
    u32 *p;

    if(!status)
    {
        p = arg;
        printf("DHCPv6 Success\r\n");
        printf("IPv6Addr: %x.%x.%x.%x \r\n", HTONL(p[0]), HTONL(p[1]), HTONL(p[2]), HTONL(p[3]));
        printf("DNS1: %x.%x.%x.%x \r\n", HTONL(p[4]), HTONL(p[5]), HTONL(p[6]), HTONL(p[7]));
        printf("DNS2: %x.%x.%x.%x \r\n", HTONL(p[8]), HTONL(p[9]), HTONL(p[10]), HTONL(p[11]));
        WCHNET_InitDNS((uint8_t *)&p[4], IP_V6, DnsPort);                                       //Set DNS server IP address, and DNS server port is 53                                       
        if(NetIfIndex == 0)
        {
            WCHNET_HostNameGetIp("www.wch.cn", RemoteIPv6[NetIfIndex], WCHNET_DNSCallBack, &DomainIPv6Addr[NetIfIndex]);     //Start DNS
        }
        else 
        {
            WCHNET_HostNameGetIp("www.a.shifen.com", RemoteIPv6[NetIfIndex], WCHNET_DNSCallBack, &DomainIPv6Addr[NetIfIndex]);     //Start DNS
        }
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
    USART_Printf_Init(115200);                                          //USART initialize
    printf("DNS Test\r\n");
    printf("SystemClk:%lu\r\n", (unsigned long)SystemCoreClock);
    printf("ChipID:%08lx\r\n", (unsigned long)DBGMCU_GetCHIPID());
    printf("net version:%x\r\n", (unsigned)WCHNET_GetVer());
    if( WCHNET_LIB_VER != WCHNET_GetVer()){
        printf("version error.\r\n");
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
    if(i == WCHNET_ERR_SUCCESS) {
        printf("WCHNET_LibInit Success\r\n");
    }
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