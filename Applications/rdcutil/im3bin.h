#ifndef _IMANAGER3_BIN_H_
#define _IMANAGER3_BIN_H_

typedef struct _bin {
	const char *filename;
	FILE *hFile;
	uint32_t fsize;
} bin_t;

/* Functions */
void Bin_Free(bin_t *obj);
bin_t *Bin_Open(char *filename);	// Open bin from disk
bin_t *Bin_Load(char *filename);	// Load from EC ROM
bin_t *Sign_Bin(char *filename);	// Sign bin

int Bin_CheckIntegrity(bin_t *obj);
int Bin_CheckMatchPlatform(bin_t *obj);
void Bin_ShowInfo(bin_t *obj);

int Bin_EraseOptionBlock(bin_t *obj);
int Bin_EraseBootBlock(bin_t *obj);
int Bin_WriteBootBlock(bin_t *obj);
int Bin_VerifyBootBlock(bin_t *obj);
int Bin_EraseAppBlock(bin_t *obj);
int Bin_WriteAppBlock(bin_t *obj);
int Bin_VerifyAppBlock(bin_t *obj);

#endif /* _IMANAGER3_BIN_H_ */
