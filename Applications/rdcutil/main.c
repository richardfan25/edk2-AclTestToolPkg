#include <ctype.h>

#include "im3.h"
#include "svnversion.h"

/* Variables */
#define CMD_SAVE_CURRENT_FIRMWARE			(1 << 0)
#define CMD_DISPLAY_FIRMWARE_INFO			(1 << 1)
#define CMD_DISPLAY_IO_INFO					(1 << 2)
#define CMD_FACTORY_BURN_INFO				(1 << 3)
#define CMD_FACTORY_READ_INFO				(1 << 4)
#define CMD_SIGN_BINFILE					(1 << 15)
static uint16_t Commands = 0;

#define OPTION_FLASH_BOOTLOADER                 (1 << 0)
#define OPTION_ERASE_OPTION_BLOCK               (1 << 1)
#define OPTION_ADMIN                                    (1 << 2)
static uint16_t Options = 0;

static char *filename = NULL;

/* Dialogs */
static void ShowTitle(void)
{
        printf("+-----------------------------------------------------------------------------+\r\n");
        printf("| Advantech RDC Firmware Update Utility v1.0.%u                               |\r\n", SVN_REVISION);
        printf("| Copyright (C)2016 Advantech Corporation All Rights Reserved.                |\r\n");
        printf("+-----------------------------------------------------------------------------+\r\n");
}

static void ShowHelp(void)
{
        printf(" Usage:\r\n");
        printf("    ecfu <bin file name> [option 1] [option 2] ...\r\n");
        printf("    ecfu <Input or output file name> [command]\r\n");
        printf("    ecfu [command]\r\n");
        printf(" Commands:\r\n");
        printf("    /o   - Save current firmware into file\r\n");
        printf("    /d   - Display binary file's information\r\n");
        printf("    /p   - Display I/O port information\r\n");
        printf("    /f   - Factory burn information\r\n");
        printf("    /r   - Factory read information\r\n");
        printf(" Options:\r\n");
        printf("    /b   - Program bootloader\r\n");
        printf("    /e   - Erase option space\r\n");
}

/* Functions */
static void ParametersParser(int index, char *param)
{
        if (strcmp(param, "/o") == 0)
        {
                Commands |= CMD_SAVE_CURRENT_FIRMWARE;
        }
        else if (strcmp(param, "/d") == 0)
        {
                Commands |= CMD_DISPLAY_FIRMWARE_INFO;
        }
        else if (strcmp(param, "/p") == 0)
        {
                Commands |= CMD_DISPLAY_IO_INFO;
        }
        else if (strcmp(param, "/f") == 0)
        {
                Commands |= CMD_FACTORY_BURN_INFO;
        }
        else if (strcmp(param, "/r") == 0)
        {
                Commands |= CMD_FACTORY_READ_INFO;
        }
        else if (strcmp(param, "/sg") == 0)
        {
                Commands |= CMD_SIGN_BINFILE;
        }
        else if (strcmp(param, "/b") == 0)
        {
                Options |= OPTION_FLASH_BOOTLOADER;
        }
        else if (strcmp(param, "/e") == 0)
        {
                Options |= OPTION_ERASE_OPTION_BLOCK;
        }
        else if (strcmp(param, "/ko") == 0)
        {
                Options = OPTION_FLASH_BOOTLOADER | OPTION_ERASE_OPTION_BLOCK | OPTION_ADMIN;
        }
        else
        {
            if (*((uint8_t *)param) != '/') 
            {
                    filename = param;
                    DPRINTF("File name: %s\r\n", filename);
            }
            else
            {
                    printf("[Warning] Unknown parameter %s\r\n",param);
            }
        }
}

static int EnterUpgradeMode(void)
{
        if (im3_EnterUpgradeMode() != 0)
        {
                printf("[Error] EC cannot enter upgrade mode!\r\n");
                return -1;
        }

        SPI_Init();
        return 0;
}

static void LeaveUpgradeMode(void)
{
        SPI_UnInit();

        if (Options & OPTION_FLASH_BOOTLOADER)
        {
                printf("<Press any key to reset EC>\r\n");
                getchar();
                im3_ResetEC();
        }
        else
        {
                im3_LeaveUpgradeMode();
        }
}

static int CMD_SaveCurrentFirmware(void)
{
        int ret = 0;
        bin_t *pbin;

        if (EnterUpgradeMode() != 0)
                return -1;

        pbin = Bin_Load(filename);
        if (pbin == NULL)
        {
                printf("[Error] Load bin file failed.\r\n");
                ret = -2;
        }

        Bin_Free(pbin);
        LeaveUpgradeMode();
        return ret;
}

static int CMD_DisplayFirmwareInformation(void)
{
        bin_t *pbin = Bin_Open(filename);
        if (pbin == NULL)
        {
                printf("[Error] Open bin file failed.\r\n");
                return -1;
        }
        Bin_ShowInfo(pbin);
        Bin_Free(pbin);
        return 0;
}

static int CMD_SignFirmware(void)
{
        bin_t *pbin = Sign_Bin(filename);
        if (pbin == NULL)
        {
                printf("[Error] Sign bin file failed.\r\n");
                return -1;
        }
        Bin_ShowInfo(pbin);
        Bin_Free(pbin);
        return 0;
}

static int CMD_DisplayPorts(void)
{
        printf("* I/O Port Information *\r\n");
        printf("  PMC0:   0x%X/0x%X\r\n", im3.pmc[0].data, im3.pmc[0].cmd);
        printf("  PMC1:   0x%X/0x%X\r\n", im3.pmc[1].data, im3.pmc[1].cmd);
        printf("  PMC MB: 0x%X/0x%X\r\n", im3.pmcmb.index, im3.pmcmb.data);
        printf("  PMC IO: 0x%X/0x%X\r\n", im3.pmcio.addr, im3.pmcio.data);
        return 0;
}

static char *upper(char string[])
{
        size_t i;

        for (i = 0; i < strlen(string); i++)
                string[i] = toupper(string[i]);

        return(string);
}

/*
        If the function succeeds, the return value is 0.
        If the function fails, the return value is -1. 
*/
static int GetRoInfoFromFile(const PEIOIS200ROInfo roInfo)
{
        char* endptr;
        unsigned long eapi_spec_version;
        unsigned long board_vendor_pnpid;
        unsigned long board_vendor_compressed_pnpid;
        unsigned long vendor_specific_id;
        unsigned long board_platform_spec_revision;
        unsigned long eapi_spec_version_version;
        unsigned long eapi_spec_version_revision;
        unsigned long board_platform_spec_revision_version;
        unsigned long board_platform_spec_revision_revision;
        
        FILE * pFile;
        char buffer[256];
        int iRet = -1, count = 0;
    board_platform_spec_revision = 0;
    board_vendor_pnpid = 0;
    board_vendor_compressed_pnpid = 0;
    vendor_specific_id = 0;
    eapi_spec_version = 0;
    eapi_spec_version_version = 0;
    eapi_spec_version_revision = 0;
        board_platform_spec_revision_version = 0;
        board_platform_spec_revision_revision = 0; 
        
        pFile = fopen("factory", "r");  // read, binary
        if (pFile == NULL)
        {
                fputs ("File error",stderr);
                goto exit;
        }

        while (fgets(buffer, 256, pFile) != NULL)
        {
                char *name, *value, *value1, *value2;
                name = strtok(buffer, "=");

                if (strcmp(name, "BMN") == 0)
                {
                        value = strtok(NULL, "=");

                        if (value == NULL)
                        {
                                printf("Get BMN error\n");
                                goto exit;
                        }

                        if (strlen(value) > BOARD_MANUFACTURER_NAME_SIZE)
                        {
                                printf("BMN(%s) is too long\n", value);
                                goto exit;
                        }

                        printf("BMN = %s", value);

                        memset(roInfo->BoardManufacturerName, 0, BOARD_MANUFACTURER_NAME_SIZE);
                        strncpy(roInfo->BoardManufacturerName, value, strlen(value));

                        count++;
                }
                else if (strcmp(name, "SN") == 0)
                {
                        value = strtok(NULL, "=");

                        if (value == NULL)
                        {
                                printf("Get SN error\n");
                                goto exit;
                        }

                        if (strlen(value) > BOARD_SERIAL_NUMBER_SIZE)
                        {
                                printf("SN(%s) is too long\n", value);
                                goto exit;
                        }

                        printf("SN = %s", value);

                        memset(roInfo->BoardSerialNumber, 0, BOARD_SERIAL_NUMBER_SIZE);
                        strncpy(roInfo->BoardSerialNumber, value, strlen(value));

                        count++;
                }
                else if (strcmp(name, "PT") == 0)
                {
                        value = strtok(NULL, "=");

                        if (value == NULL)
                        {
                                printf("Get PT error\n");
                                goto exit;
                        }

                        if (strlen(value) > BOARD_PLATFORM_TYPE_SIZE)
                        {
                                printf("PT(%s) is too long\n", value);
                                goto exit;
                        }

                        printf("PT = %s", value);

                        memset(roInfo->BoardPlatformType, 0, BOARD_PLATFORM_TYPE_SIZE);
                        strncpy(roInfo->BoardPlatformType, value, strlen(value));

                        count++;
                }
                else if (strcmp(name, "ESV") == 0)
                {
                        value = strtok(NULL, "=");

                        if (value == NULL)
                        {
                                printf("Get ESV error\n");
                                goto exit;
                        }

                        if (strlen(value) > 6)
                        {
                                printf("ERRPR!! ESV is too long!!\n");
                                goto exit;
                        }

                        printf("ESV = %s", value);

                        value1 = strtok(value, ",");
                        value2 = strtok(NULL, ",");

                        eapi_spec_version_version = (unsigned long)strtoul(value1, &endptr, 10);
                        eapi_spec_version_revision = (unsigned long)strtoul(value2, &endptr, 10);
                        eapi_spec_version = ((eapi_spec_version_version << 24) | (eapi_spec_version_revision << 16));

                        if (eapi_spec_version > 0xFFFFFFFE)
                        {
                                printf("ESV is out of range!!\n");
                                goto exit;
                        }

                        roInfo->EAPISpecVersion = eapi_spec_version;

                        count++;
                }
                else if (strcmp(name, "BVP") == 0)
                {
                        value = strtok(NULL, "=");

                        if (value == NULL)
                        {
                                printf("Get BVP error\n");
                                goto exit;
                        }

                        if (strlen(value) > 10)
                        {
                                printf("ERROR, BVP is too long!!\n");
                                goto exit;                
                        }

                        printf("BVP = %s", value);

                        value1 = strtok(value, ","); 
                        value2 = strtok(NULL, ",");        

                        value1 = upper(value1);   

                        value1[0] = (value1[0] - 0x40) & 0x1F;
                        value1[1] = (value1[1] - 0x40) & 0x1F;
                        value1[2] = (value1[2] - 0x40) & 0x1F;           

                        board_vendor_compressed_pnpid = ((value1[0] << 10) | (value1[1] << 5) | value1[2]) & 0x0000EFFF;
                        board_vendor_compressed_pnpid
                                = ((board_vendor_compressed_pnpid << 8) & 0x0000FF00) | ( (board_vendor_compressed_pnpid >> 8) & 0x000000FF);
                        vendor_specific_id = (unsigned long)strtoul(value2, &endptr, 16);
                        board_vendor_pnpid = 0xF0000000 | ((board_vendor_compressed_pnpid << 12) | (vendor_specific_id & 0x00000FFF));

                        if (board_vendor_pnpid > 0xFFFFFFFE)
                        {
                                printf("BVP is out of range!!\n");
                                goto exit;
                        }      

                        roInfo->BoardPnpidVal = board_vendor_pnpid;

                        count++;
                }
                else if (strcmp(name, "BPSR") == 0)
                {
                        value = strtok(NULL, "=");

                        if (value == NULL)
                        {
                                printf("Get BPSR error\n");
                                goto exit;
                        }

                        if (strlen(value) > 5)
                        {
                                printf("Get BPSR error\n");
                                goto exit;                
                        }

                        printf("BPSR = %s", value);

                        value1 = strtok(value, ","); 
                        value2 = strtok(NULL, ",");

                        board_platform_spec_revision_version = (unsigned long)strtoul(value1, &endptr, 10);
                        board_platform_spec_revision_revision = (unsigned long)strtoul(value2, &endptr, 10);
                        board_platform_spec_revision = ((board_platform_spec_revision_version << 24) | (board_platform_spec_revision_revision << 16));

                        if (board_platform_spec_revision > 0xFFFFFFFE)
                        {
                                printf("BPSR is out of range!!\n");
                                goto exit;
                        }

                        roInfo->BoardPlatformRevVal = board_platform_spec_revision;

                        count++;
                }                            
        }

exit:
        // terminate
        fclose(pFile);

        if (count == 6)
                iRet = 0;

        return iRet;
}

static int CMD_FactoryBurnInfo(void)
{
        EIOIS200ROInfo roInfo;

        memset(&roInfo, 0, sizeof(EIOIS200ROInfo));

#if 0
        strcpy(roInfo.BoardManufacturerName, "ADVANTECH");
        strcpy(roInfo.BoardName, "EIO-IS200");
        strcpy(roInfo.BoardSerialNumber, "EPA0000001");
        strcpy(roInfo.BoardBIOSRevision, "V05");
        strcpy(roInfo.BoardPlatformType, "COMExpress");
        roInfo.EAPISpecVersion = 0x01000000;
        roInfo.BoardPnpidVal = 0xF9604123;
        roInfo.BoardPlatformRevVal = 0x02000000;
        roInfo.BoardDriverVersion = 0x01000000;
        roInfo.BoardLibraryVersion = 0x01000000;
        roInfo.BoardFirmwareVersion = 0x01000000;
#endif

        GetRoInfoFromFile(&roInfo);

        printf("* Factory Burn Information *\r\n");
        printf("  Manufacture Name: %s\r\n", roInfo.BoardManufacturerName);
        printf("  Board Name:       %s\r\n", roInfo.BoardName);
        printf("  Board SN:         %s\r\n", roInfo.BoardSerialNumber);
        printf("  BIOS Revision:    %s\r\n", roInfo.BoardBIOSRevision);
        printf("  Platform Type:    %s\r\n", roInfo.BoardPlatformType);
        printf("  EAPI Spec Ver:    0x%X\r\n", roInfo.EAPISpecVersion);
        printf("  PNP ID:           0x%X\r\n", roInfo.BoardPnpidVal);
        printf("  Platform Rev:     0x%X\r\n", roInfo.BoardPlatformRevVal);
        printf("  Driver Version:   0x%X\r\n", roInfo.BoardDriverVersion);
        printf("  Library Version:  0x%X\r\n", roInfo.BoardLibraryVersion);
        printf("  Firmware Version: 0x%X\r\n", roInfo.BoardFirmwareVersion);

        printf("\r\n<Press any key to start burning...>\r\n");
        getchar();

        if (im3_FactoryBurn(&roInfo) != 0)
                printf("Update Board Information Fail!!\r\n");
        else
                printf("Update Board Information OK!!\r\n");

        return 0;
}

static int CMD_FactoryReadInfo(void)
{
        EIOIS200ROInfo roInfo;

        if (im3_FactoryRead(&roInfo) != 0)
        {
                printf("Read Board Information Fail!!\r\n");
        }
        else
        {
                printf("* Factory Burn Information *\r\n");
                printf("  Manufacture Name: %s\r\n", roInfo.BoardManufacturerName);
                printf("  Board Name:       %s\r\n", roInfo.BoardName);
                printf("  Board SN:         %s\r\n", roInfo.BoardSerialNumber);
                printf("  BIOS Revision:    %s\r\n", roInfo.BoardBIOSRevision);
                printf("  Platform Type:    %s\r\n", roInfo.BoardPlatformType);
                printf("  EAPI Spec Ver:    0x%X\r\n", roInfo.EAPISpecVersion);
                printf("  PNP ID:           0x%X\r\n", roInfo.BoardPnpidVal);
                printf("  Platform Rev:     0x%X\r\n", roInfo.BoardPlatformRevVal);
                printf("  Driver Version:   0x%X\r\n", roInfo.BoardDriverVersion);
                printf("  Library Version:  0x%X\r\n", roInfo.BoardLibraryVersion);
                printf("  Firmware Version: 0x%X\r\n", roInfo.BoardFirmwareVersion);
        }

        return 0;
}

static int StartCommand(void)
{
        if (Commands & CMD_SAVE_CURRENT_FIRMWARE)
                return CMD_SaveCurrentFirmware();
        else if (Commands & CMD_DISPLAY_FIRMWARE_INFO)
                return CMD_DisplayFirmwareInformation();
        else if (Commands & CMD_DISPLAY_IO_INFO)
                return CMD_DisplayPorts();
        else if (Commands & CMD_SIGN_BINFILE)
                return CMD_SignFirmware();
        else if (Commands & CMD_FACTORY_BURN_INFO)
                return CMD_FactoryBurnInfo();
        else if (Commands & CMD_FACTORY_READ_INFO)
                return CMD_FactoryReadInfo();
        else
                return -1;
}

static int StartUpdate(void)
{
        int ret = 0;
        bin_t *pbin;

        // Steps:
        //      - Open binary file
        //      - Enter upgrade mode
        //      - Check binary is match target platform
        //      - Erase option space (option)
        //      - Erase, program and verify bootloader (option)
        //      - Erase, program and verify firmware

        /* Open binary file */
        pbin = Bin_Open(filename);
        if (pbin == NULL)
        {
                printf("[Error] Open bin file failed.\r\n");
                return -1;
        }

        /* Enter upgrade mode */
        if (EnterUpgradeMode() != 0)
        {
                ret = -2;
                goto exit;
        }

        /* Check binary file is not corrupted */
        if (Bin_CheckIntegrity(pbin) != 0)
        {
                ret = -11;
                goto exit;
        }

        /* Check binary is match target platform */
        if (((Options & OPTION_ADMIN) != OPTION_ADMIN) && Bin_CheckMatchPlatform(pbin) != 0)
        {
                ret = -3;
                goto exit;
        }

        /* Erase option space */
        if (Options & OPTION_ERASE_OPTION_BLOCK)
        {
                if (Bin_EraseOptionBlock(pbin) != 0)
                {
                        ret = -4;
                        goto exit;
                }
        }

        /* Erase, program and verify bootloader */
        if (Options & OPTION_FLASH_BOOTLOADER)
        {
                if (Bin_EraseBootBlock(pbin) != 0)
                {
                        ret = -5;
                        goto exit;
                }
                
                if (Bin_WriteBootBlock(pbin) != 0)
                {
                        ret = -6;
                        goto exit;
                }
                
                if (Bin_VerifyBootBlock(pbin) != 0)
                {
                        ret = -7;
                        goto exit;
                }
        }

        /* Erase, program and verify firmware */
        if (Bin_EraseAppBlock(pbin) != 0)
        {
                ret = -8;
                goto exit;
        }

        if (Bin_WriteAppBlock(pbin) != 0)
        {
                ret = -9;
                goto exit;
        }

        if (Bin_VerifyAppBlock(pbin) != 0)
        {
                ret = -10;
                goto exit;
        }

exit:
        Bin_Free(pbin);
        LeaveUpgradeMode();
        return 0;
}

static int Start(void)
{
        if (Commands > 0 && Options > 0)
        {
                printf("[Error] Invalid parameter combination.\r\n");
                return -1;
        }

        if (im3_Initialize() != 0)
        {
                printf("[Error] Not found correct EC.\r\n");
                return -2;
        }

        if (Commands > 0)
                return StartCommand();
        else
                return StartUpdate();
}

int main(int arc, char *arv[])
{
        int i;

#ifdef _WIN32
        if (init_winio() != 0)
        {
                system("pause");
                return 1;
        }
#endif

        ShowTitle();

        if (arc <= 1)
        {
                ShowHelp();
        }
        else
        {
                for (i = 1; i < arc; i++)
                        ParametersParser(i, arv[i]);

                if (Commands > 0 && Options > 0)
				{
						printf("[Error] Invalid parameter combination.\r\n");
						return 1;
				}

				if (im3_Initialize() != 0)
				{
						printf("[Error] Not found correct EC.\r\n");
						return 2;
				}

				if (Commands > 0)
						return StartCommand();
				else
						return StartUpdate();

        }

#ifdef _WIN32
        deinit_winio();
#else
/* 		i = 5;
		printf("Wait for reset");
		while(i > 0){
			printf("...%d",i);
			fflush(stdout);
			sleep(1);
			i--;
		} */
		//gRT->ResetSystem(EfiResetShutdown,EFI_SUCCESS,0,NULL);
#endif

        return 0;
}
