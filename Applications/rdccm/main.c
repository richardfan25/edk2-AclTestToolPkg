#include <ctype.h>

#include "im3.h"
#include "svnversion.h"

/* Variables */
#define CMD_DISPLAY_IO_INFO				(1 << 0)
#define CMD_STANDARD_PROTOCOL			(1 << 1)
#define CMD_SEND_COMMAND				(1 << 2)
#define CMD_SEND_DATA				    (1 << 3)
#define CMD_DIRECT_ACCESS			    (1 << 4)
#define CMD_STD_PROTOCOL_VER2			(1 << 5)

#define OPTION_READ_DATA                (1 << 0)
#define OPTION_REPEAT                   (1 << 1)
#define OPTION_CHANGE_PORT              (1 << 2)
#define OPTION_IO_DELAY                 (1 << 3)
#define OPTION_CMD_DELAY                (1 << 4)

#define WBUF_MAX_LENS   256
#define RBUF_MAX_LENS   256

#define DFT_CMD_PORT        0x66
#define DFT_DATA_PORT       0x62

#define DFT_COMMAND_DELAY   300000
#define DFT_IOPORT_DELAY    5000

static uint16_t Commands = 0;
static uint16_t Options = 0;

pmc_port_t ioport;

uint32_t CmdDelay = DFT_COMMAND_DELAY;
uint32_t IODelay = DFT_IOPORT_DELAY;
uint32_t Repeat = 0;

uint8_t *wBuf = NULL;
uint8_t wLen = 0;

uint8_t *rBuf = NULL;
uint8_t rLen = 0;
uint16_t FSR = 0;
uint32_t INDF = 0;

int argc = 0;
char **argv = NULL;

/* Dialogs */
static void ShowTitle(void)
{
    printf("+-----------------------------------------------------------------------------+\r\n");
    printf("| Advantech RDC Utility v1.3.%u                               	              |\r\n", SVN_REVISION);
    printf("| Copyright (C)2016 Advantech Corporation All Rights Reserved.                |\r\n");
    printf("+-----------------------------------------------------------------------------+\r\n");
}

static void ShowHelp(void)
{
    printf(" Usage:\r\n");
    printf("    rdccm - command - [ ... command(N)] - [option ... option(N)]\r\n");
    printf("    ** Default I/O port is 62/66\r\n\r\n");
    printf(" Commands:\r\n");
    printf("    -i   - Display I/O port information\r\n");
    printf("    -m   - ADV_RDC standard protocol Ver2.\r\n");
    printf("           Read Protocol: CMD - CTL - DID - LENS\r\n");
    printf("           Write Protocol: CMD - CTL - DID - LENS - [DATA0 - DATAn]\r\n");
    printf("    -s   - ADV_RDC standard protocol.\r\n");
    printf("           Protocol: Cmd - [Data ... Data(n)] - [Read count]\r\n");
    printf("           Ex. rdccm -s 13 1 2 3\r\n");
    printf("              (Send 0x13 to command port and 0x1,0x2,0x3 to data port\r\n");
    printf("              (and then read three bytes data from data port\r\n");
    printf("    -c   - Send a data to command port\r\n");
    printf("    -d   - Send a data(or string) to data port\r\n");
    printf("    -P   - Parse EC feature.\r\n");
    printf("    -e   - Read EC register.\r\n");
    printf(" Options:\r\n");
    printf("    -r   - Read a data(or string) from data port, it must be use with -c or -d\r\n");
    printf("           Ex. rdccm -c 13 -r 4\r\n");
    printf("              (Send 0x13 to command port and then read four data from data port\r\n");
    printf("    -R   - Repeat operation\r\n");
    printf("    -T   - Delay time of repeat operation. (Default is 300 in ms)\r\n");
/* 
    printf("    -t   - Delay time that is between two i/o port operations. (Default is 5 in ms)\r\n");
    printf("    -p   - Select I/O port\r\n");
    printf("           Ex. rdccm 13 1 2 3 -p 2f2 2f6\r\n");
    printf("               (Send 0x13 to command port-0x2f2 and 0x1,0x2,0x3 to data port-0x2f6\r\n");
    printf("               (and then read three bytes from data port\r\n");
*/
}
void pferr(const char *msg)
{
    printf("\r\nERROR: %s\r\n",msg);
}

int CheckNumeric(char *str, uint8_t carry)
{
    uint8_t cy, idx = 0;
    uint8_t ch = (uint8_t)*(str + idx);

    cy = carry - 1;
    
	while (ch != '\0') {
		if(ch >= 'a')
			ch = ch - 'a' + 10;
		else if(ch >= 'A')
			ch = ch - 'A' + 10;
		else if(ch >= '0')
			ch -= '0';
		else{
			return -1;
		}
		if(ch > cy){
			return -1;
		}
		idx++;
		ch = (uint8_t)*(str + idx);
    }

	return 0;
}

int GetSendData(uint8_t *buf, int *StIdx, int MaxLimit)
{
    char * ptr;
    int pIdx = 0;
    long int Data;

    while(1) {
        // get param & check input data is end or not
        (*StIdx)++; // set to next param
        if (*StIdx >= argc) 
            break;

        ptr = argv[*StIdx]; 
        if (CheckNumeric(ptr, 16) != 0) {
            (*StIdx)--;
			if(ptr[0] != '-')
                goto eErrData;
            else
                break;
        }

        // string to int & check data range
        Data = strtol(ptr, NULL, 16);
        if ((Data < 0) || ((unsigned long int)Data > 255)) 
            goto eErrData;

        if (pIdx < MaxLimit) {
            *(buf + pIdx) = (uint8_t)Data; 
            pIdx++;
        }
        else
            goto eErrmore;
    };
    if (pIdx == 0) 
        goto eErrfew;
    else
        return pIdx;

eErrmore:
    pferr("Input data too more.");
    return -1;
eErrfew:
    pferr("Too few parameter.");
    return -1;
eErrData:
    pferr("Input data is invalid.");
    return -1;
}

static int CmdStdProtocolVer2(int *pIdx)
{
    int temp;

    if ((Commands & CMD_SEND_COMMAND) || (Commands & CMD_SEND_DATA) || (Commands & CMD_STANDARD_PROTOCOL))
        goto eErrUse;

    temp = GetSendData(&wBuf[0], pIdx, WBUF_MAX_LENS);

    if(temp <= 0)
        return -1;
	if (wBuf[0] != 0 && wBuf[0] != 0x30)
	{
		if (((wBuf[0] & 0x01) == 0) && ((wBuf[3] + 4) != temp))
		{
			pferr("Write Data string and Data Length byte doesn't match.");
			return -1;
		}
	}
    wLen = (uint8_t)temp;
    Commands |= CMD_STD_PROTOCOL_VER2; 
    return 0;

eErrUse:
    pferr("Can't use -s with -c or -d.");
    return -1;
}

//=============================================================================
//  CmdStdV2_GetErrorInfo
//  Get error code for protocol v2
//  Param:	ch		- Device Channel
//			thrd	- thermal threshold of stop fan (unit: 0.1K)
//  return:	0xFFFFFFFF	- Invalied param or Channel is switch off
//			0			- success
//=============================================================================
static uint32_t CmdStdV2_GetErrorInfo(void)
{
	if (Commands & CMD_STD_PROTOCOL_VER2) {
        // send command 0x00
        if(ioport.cmd == im3.pmc[0].cmd){
            if(pmc_cmd_out(&ioport, 0x00) != 0){
                goto eErrUse;
            }
        }
        else if(ioport.cmd == im3.pmc[1].cmd){
            if(pmc_cmd_out(&ioport, 0x01) != 0){
                goto eErrUse;
            }
        }
        else{
            printf("Invalid I/O port\r\n");
            goto eErrUse;
        }
        // get error code
        if(pmc_data_in(&ioport, rBuf) != 0){
            goto eErrUse;
        }

		switch (rBuf[0]) {
        case 0x00:
			// no error
            return 0;
        case 0x01:
            printf("\r\nERROR: Unknown command code");
            break;
        case 0x02:
            printf("\r\nERROR: Unknown control code");
            break;
        case 0x03:
            printf("\r\nERROR: Unknown device id");
            break;
        case 0x04:
            printf("\r\nERROR: Invalid length");
            break;
        case 0x05:
            printf("\r\nERROR: Invalid data");
            break;
        case 0x06:
            printf("\r\nERROR: Invalid protocol");
            break;
        case 0x07:
            printf("\r\nERROR: Read-Only");
            break;
        case 0x08:
            printf("\r\nERROR: Write-Only");
            break;
        default:
            printf("\r\nERROR: Unknown error\r\n");
            return 0xF0000000 | rBuf[0];
		}
        printf("\r\n");
        return rBuf[0];
	}
eErrUse:
    return 0xFFFFFFFF;
}

static int CmdStandardProtocol(int *pIdx)
{
    int temp;

    if ((Commands & CMD_SEND_COMMAND) || (Commands & CMD_SEND_DATA))
        goto eErrUse;

    temp = GetSendData(&wBuf[0], pIdx, WBUF_MAX_LENS);

    if(temp <= 0)
        return -1;

    wLen = (uint8_t)temp;
    Commands |= CMD_STANDARD_PROTOCOL; 
    return 0;

eErrUse:
    pferr("Can't use -s with -c or -d.");
    return -1;
}

static int CmdSendCommand(int *pIdx)
{
    if (Commands & CMD_STANDARD_PROTOCOL)
        goto eErrUse;

    else if(Commands & CMD_SEND_DATA)
        goto eErrUse2;

    if(GetSendData(&wBuf[0], pIdx, 1) <= 0)
        return -1;
    else{
        Commands |= CMD_SEND_COMMAND;
        return 0;
    }

eErrUse:
    pferr("Can't use -c with -s.");
    return -1;
eErrUse2:
    pferr("Can't use -c after -d.");
    return -1;
}

static int CmdSendData(int *pIdx)
{
    int temp;
    if (Commands & CMD_STANDARD_PROTOCOL)
        goto eErrUse;

    temp = GetSendData(&wBuf[1], pIdx, WBUF_MAX_LENS - 1);
    if(temp <= 0)
        return -1;  

    wLen = (uint8_t)temp;
    Commands |= CMD_SEND_DATA;
    return 0;

eErrUse:
    pferr("Can't use -d with -s.");
    return -1;
}

static int CmdDirectAccess(int *pIdx)
{
    FSR = (uint16_t)strtol(argv[++(*pIdx)], NULL, 16);
    INDF = (uint32_t)strtol(argv[++(*pIdx)], NULL, 16);
    if(((INDF && FSR) == 0) || (INDF > 4))
        return -1;

    Commands |= CMD_DIRECT_ACCESS;
    return 0;
}
static int OptReadData(int *pIdx)
{
    int temp;
    char *ptr;

    if ((Commands & CMD_SEND_COMMAND) || (Commands & CMD_SEND_DATA) ||
        (Commands & CMD_STANDARD_PROTOCOL)) {

        if(*pIdx == argc - 1)
            goto eErrfew;

        ptr = argv[*pIdx + 1]; 
        if(CheckNumeric(ptr, 10) != 0)
            goto eErrData;

        temp = atoi(ptr);
        if (((unsigned int)temp) > RBUF_MAX_LENS) {
            temp = RBUF_MAX_LENS;
        }
        else if (temp != 0) {
            rLen = (uint8_t)temp;
            (*pIdx)++;

            Options |= OPTION_READ_DATA;
            return 0;
        }
        else
            goto eErrCnt;
    };

    pferr("-r must be use with -s, -c or -d");
    return -1;
eErrCnt:
    pferr("The minimum read count minimum is 1.");
    return -1;
eErrfew:
    pferr("Too few parameter.");
    return -1;
eErrData:
    pferr("Input data is invalid.");
    return -1;
}

static int OptChangeDefaultSetting(int *pIdx, uint16_t no)
{
    char *ptr;
    uint8_t carry;
    long int data;

    if(*pIdx == argc - 1)
    	goto eErrfew;

    ptr = argv[*pIdx + 1];
    if (no & 0x8000) 
        carry = 16;
    else
        carry = 10;

    if (CheckNumeric(ptr, carry) != 0) 
    	goto eErrData;

    switch (no & 0x7FFF) {
    case 0:
        CmdDelay = (uint32_t)atoi(ptr); 
        printf("Set command delay time: %dms\r\n",CmdDelay);
        CmdDelay = CmdDelay * 1000;
        Options |= OPTION_CMD_DELAY;
        break;
    case 1:
        IODelay = (uint32_t)atoi(ptr);
        printf("Set IO port timeout: %dms\r\n",IODelay);
        IODelay = IODelay * 1000;
        Options |= OPTION_IO_DELAY;
        break;
    case 2:
        Repeat = (uint32_t)atoi(ptr);
        Options |= OPTION_REPEAT;
        break;
    case 3:
        data = strtol(ptr, NULL, 16);
        if ((data <= 0) || ((unsigned long int)data > 0xffff)) 
            goto eErrData;
        ioport.cmd = (uint16_t)data;
        printf("Set IO CMD port: 0x%X\r\n",data);
        break;
    case 4:
        data = strtol(ptr, NULL, 16);
        if ((data <= 0) || ((unsigned long int)data > 0xffff)) 
            goto eErrData;
        ioport.data = (uint16_t)data;
        printf("Set IO DATA port: 0x%X\r\n",data);
        break;
    default:
        goto eErrData;
    }

    (*pIdx)++;
    return 0;

eErrfew:
    pferr("Too few parameter.");
    return -1;
eErrData:
    pferr("Input data is invalid.");
    return -1;
}

/* Functions */
static int ParametersParser(void)
{	
    char * ptr;
    int pIdx;

    for (pIdx = 1; pIdx < argc; pIdx++) {
        //Commands
        ptr = argv[pIdx];
        if (strcmp(ptr, "-i") == 0){
            Commands |= CMD_DISPLAY_IO_INFO;
        }

        else if (strcmp(ptr, "-m") == 0){
            if(CmdStdProtocolVer2(&pIdx) != 0)
                return -1;
        }
        else if (strcmp(ptr, "-s") == 0){
            if(CmdStandardProtocol(&pIdx) != 0)
                return -1;
        }
        else if (strcmp(ptr, "-c") == 0){
            if(CmdSendCommand(&pIdx) != 0)
                return -1;
        }
        else if (strcmp(ptr, "-d") == 0){

            if(CmdSendData(&pIdx) != 0)
                return -1;  
        }
        else if (strcmp(ptr, "-e") == 0){
            if(CmdDirectAccess(&pIdx) != 0)
                return -1;
        }
        //Options
        else if (strcmp(ptr, "-r") == 0){
            if(OptReadData(&pIdx) != 0)
                return -1;
        }
        else if (strcmp(ptr, "-T") == 0){
            if (OptChangeDefaultSetting(&pIdx, 0) != 0)
                return -1;
        }
        else if (strcmp(ptr, "-R") == 0){
            if (OptChangeDefaultSetting(&pIdx, 2) != 0)
                return -1;
        }
        else if (strcmp(ptr, "-P") == 0){
            if (OptChangeDefaultSetting(&pIdx, 3 | 0x8000) != 0)
                return -1;
            if (OptChangeDefaultSetting(&pIdx, 4 | 0x8000) != 0)
                return -1;
        }
        else{
            pferr("Unknown/Incorrect parameter.");
            return -1;
        }
    }
    if (Commands != 0)
		return 0;

    pferr("No invalid command.");
    return -1;
}

static int CMD_DisplayPorts(void)
{
    printf("* I/O Port Information *\r\n");
    printf("  PMC0:   0x%X/0x%X\r\n", im3.pmc[0].data, im3.pmc[0].cmd);
    printf("  PMC1:   0x%X/0x%X\r\n", im3.pmc[1].data, im3.pmc[1].cmd);
//    printf("  PMC MB: 0x%X/0x%X\r\n", im3.pmcmb.index, im3.pmcmb.data);
//    printf("  PMC IO: 0x%X/0x%X\r\n", im3.pmcio.addr, im3.pmcio.data);
    return 0;
}

int main(int arc, char *arv[])
{
    int i;
	int ret = 1;
    argc = arc;
    argv = arv;
    ioport.cmd = DFT_CMD_PORT;
    ioport.data = DFT_DATA_PORT;

#ifdef _WIN32
    if (init_winio() != 0)
    {
        system("pause");
        goto eErrEnd;
    }
#endif
	
	wBuf = (uint8_t *) malloc(sizeof(uint8_t) * WBUF_MAX_LENS);
	if(wBuf == NULL)
	{
		pferr("Fail to alloc write buffer.");
		goto eErrEnd;
	}
	rBuf = (uint8_t *) malloc(sizeof(uint8_t) * RBUF_MAX_LENS);
	if(rBuf == NULL)
	{
		pferr("Fail to alloc read buffer.");
		goto eErrEnd;
	}
    if (arc <= 1)
    {
        ShowTitle();
        ShowHelp();
    }
    else
    {
        if(ParametersParser() != 0)
            goto eErrEnd;

        // Get all of supported i\o port
        if (im3_Initialize() != 0) 
		{
			pferr("Not found correct EC.");
			goto eErrEnd;
		}

        // show i\o ports
        if (Commands & CMD_DISPLAY_IO_INFO) {
            CMD_DisplayPorts();
        }

        if (Commands & CMD_DIRECT_ACCESS) {

            if (INDF == 4) {
                INDF = ecio_ind(FSR);
            }
            else if (INDF == 2) {
                INDF = (uint32_t)ecio_inw(FSR);
            }
            else{
                INDF = (uint32_t)ecio_inb(FSR);
            }
            printf("Read 0x%04x :  0x%04x\n",FSR, INDF);
            goto End;
        }
        if (Commands & CMD_STANDARD_PROTOCOL) {
			if(wLen >= 1){
				Commands |= CMD_SEND_COMMAND;
				if(wLen >= 2){
					wLen--;
					Commands |= CMD_SEND_DATA;
				}
			}
        }
        else if (Commands & CMD_STD_PROTOCOL_VER2) {
			if (wLen >= 3) {
                Commands |= (CMD_SEND_COMMAND | CMD_SEND_DATA); 
                wLen--; // Dec cmd byte
                // Command byte[bit0] = 1 or Command byte = 0 is read operation
                if (wBuf[0] & 0x01) {
                    // Read protocol
                    Options = Options | OPTION_READ_DATA;
                    rLen = wBuf[wLen];  // Get Lens byte
                }
                else{
                    ;// Write protocol
                }
			}
            else if((wLen == 2) && ((wBuf[0] == 0x01) || (wBuf[0] == 0x00))){
				Commands |= CMD_SEND_COMMAND; 
                Options = Options | OPTION_READ_DATA;
                wLen = 1;
                rLen = 1;
            }
            else{
                pferr("Too few parameter.");
                goto eErrEnd;
            }
        }
lRepeat:
        if (Commands & CMD_SEND_COMMAND) {
            if(pmc_cmd_out(&ioport, wBuf[0]) != 0){
                pferr("Fail to write command port.");
                goto eErrEnd;
            }
            printf("CMD: %02x",wBuf[0]);
        }

        if (Commands & CMD_SEND_DATA) {
            printf("\r\nDO:  ");
            for (i = 0; i < wLen; i++) {
                if(pmc_data_out(&ioport, *(wBuf + 1 + i)) != 0){
                    if (CmdStdV2_GetErrorInfo() == 0xFFFFFFFF) {
                        pferr("Fail to write data port.");
                    }
                    goto eErrEnd;
                }
				printf("%02x ",*(wBuf + 1 + i));
 				if ((i & 0xF) == 0xF)
					printf("\r\n     ");
            }
			if ((Options & OPTION_READ_DATA) == 0)
				CmdStdV2_GetErrorInfo();
        }
        if ((Options & OPTION_READ_DATA) && (Commands & (CMD_SEND_COMMAND | CMD_SEND_DATA))) {
            if (rLen == 0)
				CmdStdV2_GetErrorInfo();
			else
			{
				printf("\r\nDI:  ");
				for (i = 0; i < rLen; i++) {
					if(pmc_data_in(&ioport, rBuf) != 0){
						if((Commands & CMD_STANDARD_PROTOCOL) == 0){
							if (CmdStdV2_GetErrorInfo() == 0xFFFFFFFF) {
								pferr("Fail to read data port."); 
							}
							goto eErrEnd;
						}
						else
							rBuf[0] = 0xff;
					}
					printf("%02x ",rBuf[0]);
					if ((i & 0xF) == 0xF)
						printf("\r\n     ");
				}
			}
        }

		if(Repeat > 1){
			printf("\r\n");
			usleep(CmdDelay);
			Repeat--;
			goto lRepeat;
		}
    }

#ifdef _WIN32
        deinit_winio();
#endif
End:
     printf("\r\n");
	 ret = 0;

eErrEnd:
	 if (wBuf != NULL) {
		 free(wBuf);
	 }
	 if (rBuf != NULL) {
		 free(rBuf);
	 }

     return ret; 
}
