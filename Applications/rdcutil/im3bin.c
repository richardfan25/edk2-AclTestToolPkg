#include "im3.h"

#define HEADER_SIGNATURE        "$ADV"
#define HEADER_MAGIC            0x55AA5AA5

#define ADDRESS_MASK            (~0xFFF00000)
#define ROM_SIZE                        0x100000                // 1MB
#define OFFSET_BOOTCODE         (0xFFF00)
#define HEADER_SIZE_MAX         FLA_PAGE_SIZE

#pragma pack(push)
#pragma pack(4)
// V1.0.0
typedef struct rom_header {
        char Signature[4];
        uint32_t Length;                        // Base information length
        uint32_t Version;                       // version of rom_header_t

        char Platform[32];

        struct {
                uint16_t yyyy;                  // ex. 2015
                uint8_t mm;                             // ex. 7  (July)
                uint8_t dd;                             // ex. 17 (17th)
        } Date;

        uint32_t BootStartAddr;         // Bootloader start address in ROM
        uint32_t BootMaxSize;           // Bootloader Maximum Size
        uint32_t FWStartAddr;           // Firmware start address in ROM
        uint32_t FWMaxSize;                     // Firmware Maximum Size
        uint32_t OptionStartAddr;       // Firmware start address in ROM
        uint32_t OptionMaxSize;         // Firmware Maximum Size
        uint32_t Size;                          // Code Size of bootloader or application
        uint32_t Revision;                      // SVN revision
        uint32_t MagicNum;
} rom_header_t;
#pragma pack(pop)

#define STR_ERASE_OPTION                "Erase Option Block......... "
#define STR_ERASE_BOOTLOADER    "Erase Bootloader Block..... "
#define STR_WRITE_BOOTLOADER    "Write Bootloader Block..... "
#define STR_VERIFY_BOOTLOADER   "Verify Bootloader Block.... "
#define STR_ERASE_APPLICATION   "Erase Application Block.... "
#define STR_WRITE_APPLICATION   "Write Application Block.... "
#define STR_VERIFY_APPLICATION  "Verify Application Block... "
#define STR_LOAD_BIN                    "Load firmware from EC...... "
#define STR_LOAD_FW                     "Load firmware file......... "

/* APIs */
void Bin_Free(bin_t *obj)
{
        if (obj != NULL)
        {
                if (obj->hFile != NULL)
                        fclose(obj->hFile);

                free(obj);
        }
}

bin_t *Bin_Open(char *filename) // Open from disk
{
        bin_t *obj = (bin_t *)malloc(sizeof(bin_t));

        if (obj == NULL)
                return NULL;

        obj->filename = filename;
    if ((obj->hFile = fopen(filename, "rb")) == NULL)
    {
        printf("[Error] Open file %s failed!\r\n", filename);
        goto exit_err;
    }

        fseek(obj->hFile, 0, SEEK_END);
        obj->fsize = ftell(obj->hFile);
        fseek(obj->hFile, 0, SEEK_SET);
        return obj;

exit_err:
        Bin_Free(obj);
        return NULL;
}

static uint32_t PartialChecksum(uint32_t partialSum, uint32_t size, const uint16_t* buffer, const uint32_t exclude)
{
        // Main summing loop
        while (size > exclude)
        {
                partialSum += *buffer++;
                size -= 2;
        }

        return partialSum;
}

bin_t *Sign_Bin(char *filename) // Open from disk
{
	int ret = 0;
	uint8_t *buf = NULL;
    uint32_t offset;
	uint32_t sum = 0;
	uint16_t checksum = 0;
	uint8_t skipcnt = 0;
	
	bin_t *obj = (bin_t *)malloc(sizeof(bin_t));
    if (obj == NULL)
            return NULL;

    obj->filename = filename;
	
	// open bin file
    if ((obj->hFile = fopen(filename, "r+")) == NULL)
    {
        printf("[Error] Open file %s failed!\r\n", filename);
        goto exit_err;
    }
	// calculate checksum
    fseek(obj->hFile, 0, SEEK_SET);
    for (offset = 0; offset < ROM_SIZE; offset += FLA_PAGE_SIZE)
    {
        if (fread(buf, 1, FLA_PAGE_SIZE, obj->hFile) != FLA_PAGE_SIZE)
        {
                printf(STR_LOAD_FW" Failed                         \r\n");
                ret = -2;
                goto exit_err;
        }
        else
        {
                printf(STR_LOAD_FW" 0x%08lX\r", offset);
				if((offset + FLA_PAGE_SIZE) == ROM_SIZE)
					skipcnt = 2;
                sum = PartialChecksum(sum, FLA_PAGE_SIZE, (uint16_t*)buf, skipcnt);
        }
    }

    // Fold 32-bit sum to 16 bits
    while (sum >> 16)
            sum = (sum & 0xFFFF) + (sum >> 16);

    checksum = (uint16_t)~sum;

    fseek(obj->hFile, ROM_SIZE - 2, SEEK_SET);
    if (fwrite(&checksum, 1, sizeof(uint16_t), obj->hFile) != sizeof(uint16_t))
    {
            printf("[Error] Write EC firmware to file failed!\r\n");
            goto exit_err;
    }
    return obj;

exit_err:
    Bin_Free(obj);
    return NULL;
}

bin_t *Bin_Load(char *filename) // Load from EC ROM
{
        bin_t *obj = (bin_t *)malloc(sizeof(bin_t));
        uint8_t *buf = NULL;
        uint32_t offset;
        uint16_t checksum = 0;
        uint32_t sum = 0;
        uint32_t exclude = 0;

        if (obj == NULL)
        {
                printf("[Error] malloc failed\r\n");
                return NULL;
        }

        buf = (uint8_t *)malloc(FLA_PAGE_SIZE);
        if (buf == NULL)
        {
                printf("[Error] malloc buf failed\r\n");
                goto exit_err;
        }

        obj->filename = filename;
    if ((obj->hFile = fopen(filename, "wb+")) == NULL)
    {
        printf("[Error] Create file %s failed!\r\n", filename);
        goto exit_err;
    }

        obj->fsize = ROM_SIZE;
    for (offset = 0; offset < obj->fsize; offset += FLA_PAGE_SIZE)
    {
            if (Flash_Read(offset, buf, FLA_PAGE_SIZE) != FLA_PAGE_SIZE)
                {
                        printf(STR_LOAD_BIN" 0x%08lX Fail                  \r\n", offset);
                        goto exit_err;
                }
                else
                {
                        if (fwrite(buf, 1, FLA_PAGE_SIZE, obj->hFile) != FLA_PAGE_SIZE)
                        {
                                printf("[Error] Write EC firmware to file failed!\r\n");
                                goto exit_err;
                        }

                        if (offset + FLA_PAGE_SIZE == obj->fsize)       // last page
                                exclude = 2;
                        sum = PartialChecksum(sum, FLA_PAGE_SIZE, (uint16_t*)buf, exclude);

                        printf(STR_LOAD_BIN" 0x%08lX (%d%%)\r", offset, (offset * 100) / ROM_SIZE);
                }
    }

        // Fold 32-bit sum to 16 bits
        while (sum >> 16)
                sum = (sum & 0xFFFF) + (sum >> 16);

        checksum = (uint16_t)~sum;

        fseek(obj->hFile, ROM_SIZE - 2, SEEK_SET);
        if (fwrite(&checksum, 1, sizeof(uint16_t), obj->hFile) != sizeof(uint16_t))
        {
                printf("[Error] Write EC firmware to file failed!\r\n");
                goto exit_err;
        }

    printf(STR_LOAD_BIN" OK                                    \r\n");

    free(buf);
        return obj;
        
exit_err:
        free(buf);
        Bin_Free(obj);
        return NULL;
}

static rom_header_t *GetHeader(bin_t *obj)
{
        uint32_t readcount;
        rom_header_t *h = (rom_header_t *)malloc(HEADER_SIZE_MAX);

        if (h == NULL)
        {
                printf("[Error] malloc failed!\r\n");
                return NULL;
        }

        fseek(obj->hFile, 0, SEEK_SET);
        readcount = (uint32_t)fread(h, 1, HEADER_SIZE_MAX, obj->hFile);
        if (readcount != HEADER_SIZE_MAX)
        {
                printf("[Error] Read bin file failed (%ld)\r\n", readcount);
                free(h);
                return NULL;
        }
        return h;
}

static rom_header_t *GetBootHeader(bin_t *obj)
{
        uint32_t readcount;
        rom_header_t *h = GetHeader(obj);

        if (h == NULL)
                return NULL;

        fseek(obj->hFile, h->BootStartAddr & ADDRESS_MASK, SEEK_SET);
        readcount = (uint32_t)fread(h, 1, HEADER_SIZE_MAX, obj->hFile);
        if (readcount != HEADER_SIZE_MAX)
        {
                printf("[Error] Read bin file failed (%ld)\r\n", readcount);
                free(h);
                return NULL;
        }
        return h;
}

static int CheckHeaderValid(rom_header_t *h)
{
        if (memcmp(h->Signature, HEADER_SIGNATURE, 4) != 0 ||
                h->Length != sizeof(rom_header_t) ||
                h->MagicNum != HEADER_MAGIC)
        {
                return -1;
        }
        return 0;
}

int Bin_CheckIntegrity(bin_t *obj)
{
        int ret = 0;
        uint8_t *buf = NULL;
        uint32_t offset;
        uint32_t sum = 0;
        uint16_t checksum = 0;

        if ((buf = (uint8_t *)malloc(FLA_PAGE_SIZE)) == NULL)
        {
                printf("[Error] malloc buf failed\r\n");
                ret = -1;
                goto exit;
        }

        fseek(obj->hFile, 0, SEEK_SET);
        for (offset = 0; offset < ROM_SIZE; offset += FLA_PAGE_SIZE)
        {
                if (fread(buf, 1, FLA_PAGE_SIZE, obj->hFile) != FLA_PAGE_SIZE)
                {
                        printf(STR_LOAD_FW" Failed                         \r\n");
                        ret = -2;
                        goto exit;
                }
                else
                {
                        printf(STR_LOAD_FW" 0x%08lX\r", offset);
                        sum = PartialChecksum(sum, FLA_PAGE_SIZE, (uint16_t*)buf, 0);
                }
        }

        // Fold 32-bit sum to 16 bits
        while (sum >> 16)
                sum = (sum & 0xFFFF) + (sum >> 16);

        checksum = (uint16_t)~sum;
        if (checksum != 0)
        {
                printf(STR_LOAD_FW" Checksum Failed                \r\n");
                ret = -3;
                goto exit;
        }
        else
        {
                printf(STR_LOAD_FW" Checksum OK                            \r\n");
        }

exit:
        if (buf != NULL)
                free(buf);
        return ret;
}

int Bin_CheckMatchPlatform(bin_t *obj)
{
        int ret = 0;
        rom_header_t *hbin = GetHeader(obj);
        rom_header_t *hec = NULL;

        if (hbin == NULL)
                return -1;

        if (CheckHeaderValid(hbin) != 0)
        {
                printf("[Warning] Invalid header in bin file\r\n");
                ret = -2;
                goto exit;
        }

        hec = (rom_header_t *)malloc(HEADER_SIZE_MAX);
        if (Flash_Read(0x00000000, (uint8_t *)hec, HEADER_SIZE_MAX) != HEADER_SIZE_MAX)
        {
                printf("[Error] Load firmware from EC failed\r\n");
                ret = -3;
                goto exit;
        }

        if (CheckHeaderValid(hec) != 0)
        {
                printf("[Warning] Invalid header in EC ROM\r\n");
                ret = -4;
                goto exit;
        }

        if (strcmp(hbin->Platform, hec->Platform) != 0 ||
                hbin->FWStartAddr != hec->FWStartAddr ||
                hbin->BootStartAddr != hec->BootStartAddr)
        {
                printf("[Error] This bin is not match target platform\r\n");
				printf("Target:\r\n");
				printf("\tPlatform: %s\r\n",hec->Platform);
				printf("\tStart Addr: 0x%08lX\r\n",hec->FWStartAddr);
				printf("\tBStart Addr: 0x%08lX\r\n",hec->BootStartAddr);
				printf("Expect:\r\n");
				printf("\tPlatform: %s\r\n",hbin->Platform);
				printf("\tStart Addr: 0x%08lX\r\n",hbin->FWStartAddr);
				printf("\tBStart Addr: 0x%08lX\r\n",hbin->BootStartAddr);
                ret = -5;
                goto exit;
        }

exit:
        if (hbin != NULL)
                free(hbin);
        if (hec != NULL)
                free(hec);
        return ret;
}

static void ShowHeaderInfo(rom_header_t *h)
{
        if (CheckHeaderValid(h) != 0)
        {
                printf("[Error] Invalid header\r\n");
        }
        else
        {
                printf("   Header Version: V%d.%d.%d\r\n", (uint8_t)(h->Version >> 24), (uint8_t)(h->Version >> 16), (uint16_t)h->Version);
                printf("   Date:           %d/%02d/%02d\r\n", h->Date.yyyy, h->Date.mm, h->Date.dd);
                printf("   Boot Address:   0x%08lX (%ld)\r\n", h->BootStartAddr & ADDRESS_MASK, h->BootMaxSize);
                printf("   App Address:    0x%08lX (%ld)\r\n", h->FWStartAddr & ADDRESS_MASK, h->FWMaxSize);
                printf("   Option Address: 0x%08lX (%ld)\r\n", h->OptionStartAddr & ADDRESS_MASK, h->OptionMaxSize);
                printf("   Used Size:      %ld\r\n", h->Size);
                printf("   Revision:       %ld\r\n", h->Revision);
        }
}

void Bin_ShowInfo(bin_t *obj)
{
        rom_header_t *h = GetHeader(obj);

        if (h == NULL)
                return;
        printf(" * Application *\r\n");
        ShowHeaderInfo(h);
        free(h);

        printf("\r\n");
        printf(" * Bootloader *\r\n");
        h = GetBootHeader(obj);
        if (h == NULL)
                return;
        ShowHeaderInfo(h);
        free(h);
}

int Bin_EraseOptionBlock(bin_t *obj)
{
        int ret = 0;
        uint32_t start;
        uint32_t addr;
        rom_header_t *h = GetHeader(obj);

        if (h == NULL)
                return -1;

        start = h->OptionStartAddr & ADDRESS_MASK;
        for (addr = start; addr < (start + h->OptionMaxSize); addr += FLA_SECTOR_SIZE)
        {
                if (Flash_SectorErase(addr) != 0)
                {
                        printf(STR_ERASE_OPTION" Failed                              \r\n");
                        ret = -2;
                        goto exit;
                }
                else
                {
                        printf(STR_ERASE_OPTION" 0x%08lX (%d%%)\r", addr, ((addr - start) * 100 / h->OptionMaxSize));
                }
        }
        printf(STR_ERASE_OPTION" OK                                     \r\n");
        
exit:
        if (h != NULL)
                free(h);
        return ret;
}

int Bin_EraseBootBlock(bin_t *obj)
{
        int ret = 0;
        uint32_t start;
        uint32_t addr;
        rom_header_t *h = GetHeader(obj);

        if (h == NULL)
                return -1;

        start = h->BootStartAddr & ADDRESS_MASK;
        for (addr = start; addr < (start + h->BootMaxSize); addr += FLA_SECTOR_SIZE)
        {
                if (Flash_SectorErase(addr) != 0)
                {
                        printf(STR_ERASE_BOOTLOADER" Failed                                 \r\n");
                        ret = -2;
                        goto exit;
                }
                else
                {
                        printf(STR_ERASE_BOOTLOADER" 0x%08lX (%d%%)\r", addr, ((addr - start) * 100 / h->BootMaxSize));
                }
        }
        printf(STR_ERASE_BOOTLOADER" OK                                    \r\n");

exit:
        if (h != NULL)
                free(h);
        return ret;
}

int Bin_WriteBootBlock(bin_t *obj)
{
        int ret = 0;
        uint32_t start;
        uint32_t offset;
        rom_header_t *h = GetBootHeader(obj);
        uint8_t *buf = NULL;

        if (h == NULL)
                return -1;

        if ((buf = (uint8_t *)malloc(FLA_PAGE_SIZE)) == NULL)
        {
                printf("[Error] malloc buf failed\r\n");
                ret = -2;
                goto exit;
        }

        start = h->BootStartAddr & ADDRESS_MASK;
        fseek(obj->hFile, start, SEEK_SET);
        for (offset = 0; offset < (h->Size + FLA_PAGE_SIZE); offset += FLA_PAGE_SIZE)
        {
                if (fread(buf, 1, FLA_PAGE_SIZE, obj->hFile) != FLA_PAGE_SIZE ||
                        Flash_PageProgram(start + offset, buf, FLA_PAGE_SIZE) != FLA_PAGE_SIZE)
                {
                        printf(STR_WRITE_BOOTLOADER" Failed                         \r\n");
                        ret = -3;
                        goto exit;
                }
                else
                {
                        printf(STR_WRITE_BOOTLOADER" 0x%08lX (%d%%)\r", start + offset, (offset * 100 / h->Size));
                }
        }

        // Write boot code
        fseek(obj->hFile, OFFSET_BOOTCODE, SEEK_SET);
        if (fread(buf, 1, FLA_PAGE_SIZE, obj->hFile) != FLA_PAGE_SIZE ||
                Flash_PageProgram(OFFSET_BOOTCODE, buf, FLA_PAGE_SIZE) != FLA_PAGE_SIZE)
        {
                printf(STR_WRITE_BOOTLOADER" Failed                         \r\n");
                ret = -4;
                goto exit;
        }

        printf(STR_WRITE_BOOTLOADER" OK                              \r\n");

exit:
        if (h != NULL)
                free(h);
        if (buf != NULL)
                free(buf);
        return ret;
}

int Bin_VerifyBootBlock(bin_t *obj)
{
        int ret = 0;
        uint32_t start;
        uint32_t offset;
        rom_header_t *h = GetBootHeader(obj);
        uint8_t *bufec = NULL;
        uint8_t *bufbin = NULL;

        if (h == NULL)
                return -1;

        if ((bufec = (uint8_t *)malloc(FLA_PAGE_SIZE)) == NULL ||
                (bufbin = (uint8_t *)malloc(FLA_PAGE_SIZE)) == NULL)
        {
                printf("[Error] malloc buf failed\r\n");
                ret = -2;
                goto exit;
        }

        start = h->BootStartAddr & ADDRESS_MASK;
        fseek(obj->hFile, start, SEEK_SET);
        for (offset = 0; offset < (h->Size + FLA_PAGE_SIZE); offset += FLA_PAGE_SIZE)
        {
                if (Flash_Read(start + offset, bufec, FLA_PAGE_SIZE) != FLA_PAGE_SIZE ||
                        fread(bufbin, 1, FLA_PAGE_SIZE, obj->hFile) != FLA_PAGE_SIZE ||
                        memcmp(bufec, bufbin, FLA_PAGE_SIZE) != 0)
                {
                        printf(STR_VERIFY_BOOTLOADER" Failed                        \r\n");
                        ret = -3;
                        goto exit;
                }
                        
                printf(STR_VERIFY_BOOTLOADER" 0x%08lX (%d%%)\r", start + offset, (offset * 100 / h->Size));
        }

        // Verify boot code
        fseek(obj->hFile, OFFSET_BOOTCODE, SEEK_SET);
        if (Flash_Read(OFFSET_BOOTCODE, bufec, FLA_PAGE_SIZE) != FLA_PAGE_SIZE ||
                fread(bufbin, 1, FLA_PAGE_SIZE, obj->hFile) != FLA_PAGE_SIZE ||
                memcmp(bufec, bufbin, FLA_PAGE_SIZE) != 0)
        {
                printf(STR_VERIFY_BOOTLOADER" Failed                        \r\n");
                ret = -3;
                goto exit;
        }

        printf(STR_VERIFY_BOOTLOADER" OK                                \r\n");

exit:
        if (h != NULL)
                free(h);
        if (bufec != NULL)
                free(bufec);
        if (bufbin != NULL)
                free(bufbin);
        return ret;
}

int Bin_EraseAppBlock(bin_t *obj)
{
        int ret = 0;
        uint32_t start;
        uint32_t addr;
        rom_header_t *h = GetHeader(obj);

        if (h == NULL)
                return -1;

        start = h->FWStartAddr & ADDRESS_MASK;
        for (addr = start; addr < (start + h->FWMaxSize); addr += FLA_SECTOR_SIZE)
        {
                if (Flash_SectorErase(addr) != 0)
                {
                        printf(STR_ERASE_APPLICATION" Failed                             \r\n");
                        ret = -2;
                        goto exit;
                }
                else
                {
                        printf(STR_ERASE_APPLICATION" 0x%08lX (%d%%)\r", addr, ((addr - start) * 100 / h->FWMaxSize));
                }
        }
        printf(STR_ERASE_APPLICATION" OK                              \r\n");

exit:
        if (h != NULL)
                free(h);
        return ret;
}

int Bin_WriteAppBlock(bin_t *obj)
{
        int ret = 0;
        uint32_t start;
        uint32_t offset;
        rom_header_t *h = GetHeader(obj);
        uint8_t *buf = NULL;

        if (h == NULL)
                return -1;

        if ((buf = (uint8_t *)malloc(FLA_PAGE_SIZE)) == NULL)
        {
                printf("[Error] malloc buf failed\r\n");
                ret = -2;
                goto exit;
        }

        start = h->FWStartAddr & ADDRESS_MASK;
        fseek(obj->hFile, start + HEADER_SIZE_MAX, SEEK_SET);
        for (offset = HEADER_SIZE_MAX; offset < (h->Size + FLA_PAGE_SIZE); offset += FLA_PAGE_SIZE)
        {
                if (fread(buf, 1, FLA_PAGE_SIZE, obj->hFile) != FLA_PAGE_SIZE ||
                        Flash_PageProgram(start + offset, buf, FLA_PAGE_SIZE) != FLA_PAGE_SIZE)
                {
                        printf(STR_WRITE_APPLICATION" Failed                           \r\n");
                        ret = -3;
                        goto exit;
                }
                else
                {
                        printf(STR_WRITE_APPLICATION" 0x%08lX (%d%%)\r", start + offset, (offset * 100 / h->Size));
                }
        }

        // Write header at last
        fseek(obj->hFile, start, SEEK_SET);
        if (fread(buf, 1, FLA_PAGE_SIZE, obj->hFile) != FLA_PAGE_SIZE ||
                Flash_PageProgram(start, buf, FLA_PAGE_SIZE) != FLA_PAGE_SIZE)
        {
                printf(STR_WRITE_APPLICATION" Failed                           \r\n");
                ret = -4;
                goto exit;
        }

        printf(STR_WRITE_APPLICATION" OK                          \r\n");
        
exit:
        if (h != NULL)
                free(h);
        if (buf != NULL)
                free(buf);
        return ret;
}

int Bin_VerifyAppBlock(bin_t *obj)
{
        int ret = 0;
        uint32_t start;
        uint32_t offset;
        rom_header_t *h = GetHeader(obj);
        uint8_t *bufec = NULL;
        uint8_t *bufbin = NULL;

        if (h == NULL)
                return -1;

        if ((bufec = (uint8_t *)malloc(FLA_PAGE_SIZE)) == NULL ||
                (bufbin = (uint8_t *)malloc(FLA_PAGE_SIZE)) == NULL)
        {
                printf("[Error] malloc buf failed\r\n");
                ret = -2;
                goto exit;
        }

        start = h->FWStartAddr & ADDRESS_MASK;
        fseek(obj->hFile, start, SEEK_SET);
        for (offset = 0; offset < (h->Size + FLA_PAGE_SIZE); offset += FLA_PAGE_SIZE)
        {
                if (Flash_Read(start + offset, bufec, FLA_PAGE_SIZE) != FLA_PAGE_SIZE ||
                        fread(bufbin, 1, FLA_PAGE_SIZE, obj->hFile) != FLA_PAGE_SIZE ||
                        memcmp(bufec, bufbin, FLA_PAGE_SIZE) != 0)
                {
                        printf(STR_VERIFY_APPLICATION" Failed                        \r\n");
                        ret = -3;
                        goto exit;
                }

                printf(STR_VERIFY_APPLICATION" 0x%08lX (%d%%)\r", start + offset, (offset * 100 / h->Size));
        }

        printf(STR_VERIFY_APPLICATION" OK                                \r\n");

exit:
        if (h != NULL)
                free(h);
        if (bufec != NULL)
                free(bufec);
        if (bufbin != NULL)
                free(bufbin);
        return ret;
}
