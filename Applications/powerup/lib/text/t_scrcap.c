//*****************************************************************************
//*****************************************************************************
//*                                                                           *
//*  TEXT - T_SCRCAP (Text Mode Screen Capture)                               *
//*                                                                           *
//*****************************************************************************
//*****************************************************************************
#include <stdio.h>
#include <time.h>

#include "typedef.h"
#include "t_video.h"
#include "t_msgbox.h"
#include "g_fbmp.h"
#include "sound.h"
#include "key.h"


#pragma pack(1)
//This should be compatible with EFI_UGA_PIXEL
typedef struct {
    UINT8 b;
    UINT8 g;
    UINT8 r;
    UINT8 a;
} BMPIXEL;

typedef struct
{
	UINTN       Width;
	UINTN       Height;
	BOOLEAN     HasAlpha;
	BMPIXEL    *PixelData;
}BMIMAGE;

typedef struct
{
	CHAR8         CharB;
	CHAR8         CharM;
	UINT32        bfSize;
	UINT16        Reserved[2];
	UINT32        ImageOffset;
	UINT32        HeaderSize;
	UINT32        PixelWidth;
	UINT32        PixelHeight;
	UINT16        Planes;             // Must be 1
	UINT16        BitPerPixel;        // 1, 4, 8, or 24
	UINT32        CompressionType;
	UINT32        ImageSize;          // Compressed image size in bytes
	UINT32        XPixelsPerMeter;
	UINT32        YPixelsPerMeter;
	UINT32        NumberOfColors;
	UINT32        ImportantColors;
}BMP_IMAGE_HEADER;
#pragma pack()

// Console defines and variables
static EFI_GRAPHICS_OUTPUT_PROTOCOL *gGraphicsOutput = NULL;

EFI_STATUS EFIInitScreen(void)
{
	EFI_STATUS Status;
	
	Status = gBS->LocateProtocol (&gEfiGraphicsOutputProtocolGuid, NULL, (VOID**)&gGraphicsOutput);
	if(EFI_ERROR(Status))
	{
		//Print(L"Graphics Output Protocol not support!");
		return EFI_UNSUPPORTED;
	}
	
	return Status;
}

BMIMAGE * EFICreateImage(IN UINTN Width, IN UINTN Height, IN BOOLEAN HasAlpha)
{
	BMIMAGE *NewImage;

	NewImage = (BMIMAGE *)AllocateZeroPool(sizeof(BMIMAGE));
	if(NewImage == NULL)
		return NULL;
	NewImage->PixelData = (BMPIXEL *)AllocateZeroPool(Width * Height * sizeof(BMPIXEL));
	if(NewImage->PixelData == NULL)
	{
		FreePool(NewImage);
		return NULL;
	}

	NewImage->Width = Width;
	NewImage->Height = Height;
	NewImage->HasAlpha = HasAlpha;
	return NewImage;
}

VOID EFIFreeImage(IN BMIMAGE *Image)
{
	if(Image != NULL)
	{
		if(Image->PixelData != NULL)
			FreePool(Image->PixelData);
		FreePool(Image);
	}
}

VOID EFIEncodeBMP(IN BMIMAGE *Image, OUT UINT8 **FileDataReturn, OUT UINTN *FileDataLengthReturn)
{
	BMP_IMAGE_HEADER	*BmpHeader;
	UINT8		*FileData;
	UINTN		FileDataLength;
	UINT8		*ImagePtr;
	UINT8		*ImagePtrBase;
	UINTN		ImageLineOffset;
	BMPIXEL		*PixelPtr;
	UINTN		x, y;

	ImageLineOffset = Image->Width * 3;
	if((ImageLineOffset % 4) != 0)
		ImageLineOffset = ImageLineOffset + (4 - (ImageLineOffset % 4));

	// allocate buffer for file data
	FileDataLength = sizeof(BMP_IMAGE_HEADER) + Image->Height * ImageLineOffset;
	FileData = AllocateZeroPool(FileDataLength);
	if(FileData == NULL)
	{
		//Print(L"Error allocate %d bytes\n", FileDataLength);
		*FileDataReturn = NULL;
		*FileDataLengthReturn = 0;
		return;
	}

	// fill header
	BmpHeader = (BMP_IMAGE_HEADER *)FileData;
	BmpHeader->CharB = 'B'; //0x42
	BmpHeader->CharM = 'M'; //0x4D
	BmpHeader->bfSize = (UINT32)FileDataLength;
	BmpHeader->ImageOffset = sizeof(BMP_IMAGE_HEADER);
	BmpHeader->HeaderSize = 40;
	BmpHeader->PixelWidth = (UINT32)Image->Width;
	BmpHeader->PixelHeight = (UINT32)Image->Height;
	BmpHeader->Planes = 1;
	BmpHeader->BitPerPixel = 24;
	BmpHeader->CompressionType = 0;
	BmpHeader->XPixelsPerMeter = 0;//0xb13;
	BmpHeader->YPixelsPerMeter = 0;//0xb13;

	// fill pixel buffer
	ImagePtrBase = FileData + BmpHeader->ImageOffset;
	for(y = 0; y < Image->Height; y++)
	{
		ImagePtr = ImagePtrBase;
		ImagePtrBase += ImageLineOffset;
		PixelPtr = Image->PixelData + (Image->Height - 1 - y) * Image->Width;

		for(x = 0; x < Image->Width; x++)
		{
			*ImagePtr++ = PixelPtr->b;
			*ImagePtr++ = PixelPtr->g;
			*ImagePtr++ = PixelPtr->r;
			PixelPtr++;
		}
	}

	*FileDataReturn = FileData;
	*FileDataLengthReturn = FileDataLength;
}

EFI_STATUS EFISaveBMPFile(IN CHAR16 *FileName, IN UINT8 *FileData, IN UINTN FileDataLength)
{
#if 0
	EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem = NULL;

	EFI_FILE_PROTOCOL *FileProtocol, *FileHandle;
	EFI_DEVICE_PATH_PROTOCOL *dpath = NULL;
	EFI_HANDLE *dhandle = NULL;
	EFI_STATUS status;
	
	CONST CHAR16 *path;
	CHAR16 *tmp = NULL, *name = NULL;
	UINTN buffersize, handlesize = 0;
	uint16_t i;
	
	status = gBS->LocateHandle(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &handlesize, dhandle);
	if(status == EFI_BUFFER_TOO_SMALL)
	{
		dhandle = AllocateZeroPool(handlesize);
		status = gBS->LocateHandle(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &handlesize, dhandle);
		if(EFI_ERROR(status))
		{
			return status;
		}
	}
	else
	{
		return status;
	}
	
	for(i=0; i<(handlesize / sizeof(EFI_HANDLE)); i++)
	{
		dpath = DevicePathFromHandle(dhandle[i]);
		
		if(gEfiShellProtocol != NULL)
		{
			name = gEfiShellProtocol->GetFilePathFromDevicePath((CONST EFI_DEVICE_PATH_PROTOCOL *)dpath);
		}
		else if(mEfiShellEnvironment2 != NULL)
		{
			mEfiShellEnvironment2->GetFsName(dpath, FALSE, &name);
		}
		
		if(StrStr(ShellGetCurrentDir(NULL), name))
		{
			status = gBS->HandleProtocol(dhandle[i], &gEfiSimpleFileSystemProtocolGuid, (VOID**)&SimpleFileSystem);
			FreePool(dhandle);
			if(EFI_ERROR(status))
			{
				return status;
			}
			break;
		}
	}
	
	path = ShellGetCurrentDir(NULL);
	if(path && StrStr(path, L":"))
	{
		tmp = StrStr(path, L":");
		tmp++;
		if(*tmp == L'\\')
		{
			if(StrLen(tmp) == 1)
			{
				tmp = StrCat(StrStr(path, L"\\"), FileName);
			}
			else
			{
				tmp = StrCat(StrnCat(StrStr(path, L"\\"), L"\\", 1), FileName);
			}
		}
		else if(*tmp == '\0')
		{
			tmp = FileName;
		}
		else
		{
			tmp = NULL;
		}
	}

	status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &FileProtocol);
	if(EFI_ERROR(status))
		return status;

	status = FileProtocol->Open(FileProtocol, &FileHandle, tmp, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
	if (EFI_ERROR(status))
		return status;

	buffersize = FileDataLength;
	status = FileHandle->Write(FileHandle, &buffersize, FileData);
	FileHandle->Close(FileHandle);
#endif

	EFI_HANDLE dhandle;
	EFI_STATUS status = 0;
	
	status = ShellOpenFileByName(FileName, &dhandle, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
	if(EFI_ERROR(status))
		return status;

	if(dhandle != NULL)
	{
		status = ShellWriteFile(dhandle, &FileDataLength, FileData);
		if(EFI_ERROR(status))
			return status;
		
		status = ShellCloseFile(&dhandle);
		if(EFI_ERROR(status))
			return status;
	}

	return status;
}

int EFIScreenShot(void)
{
	EFI_STATUS Status;
	BMIMAGE *Image;
	UINT8 *FileData;
	UINTN FileDataLength;
	
	time_t now;
	struct tm *utc;
	char	fname[16], msg[32];
	uint16_t fname16[16];
	uint8_t bk;

	Status = EFIInitScreen();

	// allocate a buffer for the whole screen
	Image = EFICreateImage(gGraphicsOutput->Mode->Info->HorizontalResolution, gGraphicsOutput->Mode->Info->VerticalResolution, FALSE);
	if(Image == NULL)
	{
		goto bailout_wait;
	}

	bk = t_get_color();
	now = time(NULL);
	utc = localtime(&now);

	// file name : <ddhhmmss.BMP>
	sprintf(fname, "%02d%02d%02d%02d.BMP", utc->tm_mday, utc->tm_hour, utc->tm_min, utc->tm_sec);
	sprintf(msg, "%s : creating...", fname);
	
	// get full screen image
	if(gGraphicsOutput != NULL)
	{
		gGraphicsOutput->Blt(gGraphicsOutput, 
				(EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)Image->PixelData, 
				EfiBltVideoToBltBuffer,
				0, 
				0, 
				0, 
				0, 
				Image->Width, 
				Image->Height, 
				0);
	}

	msgbox_progress(T_WHITE, T_MAGENTA, "Screen Capture", msg, 4, 1);

	// encode as BMP
	EFIEncodeBMP(Image, &FileData, &FileDataLength);
	EFIFreeImage(Image);
	if(FileData == NULL)
	{
		goto bailout_wait;
	}
	
	// save to file
	AsciiStrToUnicodeStr(fname, fname16);
	Status = EFISaveBMPFile(fname16, FileData, FileDataLength);
	FreePool(FileData);
	if(EFI_ERROR(Status))
	{
		goto bailout_wait;
	}

	sound_ok();
	msgbox_progress_done();
	sprintf(msg, "%s : created ok!", fname);
	
	// message dialog box
	msgbox_tmout(T_BLACK, T_CYAN, "Screen Capture", msg, 4, 1, 1000);
	key_flush_buf();
	t_set_color(bk);
	
	return 0;

bailout_wait:
	msgbox_progress_done();
	sound_error();
	
	// error message dialog box
	msgbox_waitkey(T_WHITE, T_RED, "Screen Capture", msg, 4, 1);
	key_flush_buf();
	
	return -1;
}

//=============================================================================
//  t_scr_capture
//=============================================================================
int t_scr_capture(void)
{
	/*uint8_t 	*vbuf = gTVBASE;//(uint8_t *)T_BASE;
	uint8_t		atr, asc;
	uint8_t		fg, bg, bk;
	int			x, y;
	int			px, py;

	time_t		now;
	struct tm	*utc;
	char		fname[16];
	char		msg[32];
	uint8_t		text_buf[4000];
	g_fbmp_t	*fb;		// depends on g_fbmp


	bk = t_get_color();

	// save text buffer
	for (y=0; y<4000; y++)
	{
		text_buf[y] = *vbuf++;
	}

	now = time(NULL);
	utc = localtime(&now);

	// file name : <ddhhmmss.BMP>
	sprintf(fname, "%02d%02d%02d%02d.BMP",
			utc->tm_mday, utc->tm_hour, utc->tm_min, utc->tm_sec);

	sprintf(msg, "%s : creating...", fname);

	msgbox_progress(T_WHITE, T_MAGENTA, "Screen Capture", msg, 4, 1);

	// text res    :  80 x  25 chars  (resolution)
	// text font   :   8 x  16 pixels
	// graphic res : 640 x 400 pixels (resolution)
	// graphic fmt : 640 x 400 x 16 colors .BMP (format)

	// 640 x 400 x 4bpp with palette (16-colors)
	// <xres> <yres> <bpp> <palette> <font_data>
	fb = g_fbmp_init(640, 400, 4, bmp_pal_text_16, G_FONT_VBIOS_INTEL);

	if (!fb)
	{
		msgbox_progress_done();

		sound_error();
		// error message dialog box
		msgbox_waitkey(T_WHITE, T_RED, "Screen Capture", msg, 4, 1);
		key_flush_buf();
		return -1;
	}

	vbuf = &text_buf[0];

	for (y=0; y<25; y++)
	{
		for (x=0; x<80; x++)
		{
			asc = *vbuf++;	// ascii
			atr = *vbuf++;	// attribute

			fg = atr & 0xF;
			bg = (atr >> 4) & 0xF;
			
			px = x * fb->fnt.width;
			py = y * fb->fnt.height;

			g_fbmp_put_char(fb, px, py, fg, bg, asc);
		}
	}

	g_fbmp_flush(fb, fname);

	sound_ok();
	msgbox_progress_done();

	sprintf(msg, "%s : created ok!", fname);
	// message dialog box
	msgbox_tmout(T_BLACK, T_CYAN, "Screen Capture", msg, 4, 1, 1000);

	key_flush_buf();
	
	t_set_color(bk);*/
	
	return EFIScreenShot();//0;
}
