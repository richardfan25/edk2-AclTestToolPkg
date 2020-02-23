#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#include "env.h"
#include "ecutil.h"
#include "ec_access.h"
#include "isp.h"
#include "util.h"

//#define DEBUG
typedef union _HANDLE_OPT{
	UINT8		all;
	struct{
		UINT8 admin		:1;
		UINT8 no_ack	:1;
		UINT8 update	:1;
		UINT8 pjt_info	:1;
		UINT8 bin_info	:1;
		UINT8 reserved	:3;
	}flags;
} HANDLE_OPT;

// HANDLE strcut
typedef struct _HANDLE{
	HANDLE_OPT 	opt;
	HANDLE_BIN	bin;
	INFO_PJT	pjt;
} HANDLE;

/*==============================================================*/
// VARIABLE
/*==============================================================*/
UINT8 		rom_id[dROM_ID_SIZE];
HANDLE 		user;
const char	*env			= ENV_STRING;
const char	*ecutil_data	= ECUTIL_DATE;
const char	*ecutil_ver		= ECUTIL_VER;

/*==============================================================*/
// init handle
// Input:	Argc, Argv
// Output:	HANDLE struct
/*==============================================================*/
void init_handle(void)
{
	user.opt.all = 0;					// clear all option
	user.pjt.name[0] = 0x00;
	user.bin.pjtinfo.name[0] = 0x00;
	user.bin.name = NULL;
	user.bin.ctrl = NULL;
}
/*==============================================================*/
// Decode argument
// Input:	Argc, Argv
// Output:	HANDLE struct
/*==============================================================*/
void decode_arg(int Argc, char **Argv)
{
	int 	Argv_idx = 1;

	if(strcmp(Argv[1], "-su") == 0){
		user.opt.flags.admin = 1;
		Argv_idx++;
	}
	else{
		if(*(Argv[1]) != '-'){									// first argument is bin file; no option
			user.bin.name = Argv[1];
			user.opt.flags.update = 1;
			return;
		}
	}
	
	for(;Argv_idx < Argc; Argv_idx++){
		if (strcmp(Argv[Argv_idx], "-u") == 0){					// [-u] update firmware
				user.opt.flags.update = 1;
		}
		else if (strcmp(Argv[Argv_idx], "-i") == 0){			// [-i] display EC info
			user.opt.flags.pjt_info = 1;
		}
		else if(strcmp(Argv[Argv_idx], "-b") == 0){				// [-b] display bin file info
			user.opt.flags.bin_info = 1;
		}
		else if(strcmp(Argv[Argv_idx], "-f") == 0){				// [-f] no prompt
			user.opt.flags.no_ack = 1;
		}
		
		if((user.opt.flags.bin_info || user.opt.flags.update) && (user.bin.name == NULL)){
			if(++Argv_idx < Argc){								// get input filename
				user.bin.name = Argv[Argv_idx];
			}
		}
	}
}
/*==============================================================*/
// Get Project info(beta: will change to use mbox API)
// Input:	none
// Output:	none
/*==============================================================*/
int check_ec_info(void)
{
	UINT8 idx;
	UINT16 retry;
	char *lock_pjt_name = PJT_NAME;
	
	for(retry = 0; retry < 10000; retry++){
		ec_wr_cmd(0xA0);							// set read mailbox offset 0x0(cmd)
		ec_rd_data(&idx);							// read cmd reg, check mailbox busy?
		if(idx == 0){
			ec_wr_cmd(0x50);						// set write mailbox offset 0x0(cmd)
			ec_wr_data(0xF0);						// write cmd 0xf0
			retry = 0;
			while(1){
				ec_wr_cmd(0xA0);					// set read mailbox offset 0x0(cmd),
				ec_rd_data(&idx);					// check mailbox busy? (ec info ready)
				if(idx == 0x00)
					break;
				if(++retry > 20000)
					goto eECinfo;
			}
			//project name
			for(idx = 0; idx < PJT_NAME_SIZE; idx++){
				ec_wr_cmd(0xA3 + idx);
				ec_rd_data(user.pjt.name + idx);
				if(*(user.pjt.name + idx) != *(lock_pjt_name + idx))
					goto eECinfo_lock;
			}
			user.pjt.name[PJT_NAME_SIZE] = '\0';

			//Version table
			ec_wr_cmd(0xA3 + idx);
			ec_rd_data(user.pjt.ver);

			//Kernel Ver
			idx++;
			ec_wr_cmd(0xA3 + idx);
			ec_rd_data(user.pjt.kver);
			idx++;
			ec_wr_cmd(0xA3 + idx);
			ec_rd_data(user.pjt.kver + 1);

			//Chip cdoe
			idx++;
			ec_wr_cmd(0xA3 + idx);
			ec_rd_data(user.pjt.chip);
			idx++;
			ec_wr_cmd(0xA3 + idx);
			ec_rd_data(user.pjt.chip + 1);

			////Project ID
			idx++;
			ec_wr_cmd(0xA3 + idx);
			ec_rd_data(&user.pjt.id);
			
			//Firmware Ver
			idx++;
			ec_wr_cmd(0xA3 + idx);
			ec_rd_data(user.pjt.ver + 3);
			idx++;
			ec_wr_cmd(0xA3 + idx);
			ec_rd_data(user.pjt.ver + 1);
			idx++;
			ec_wr_cmd(0xA3 + idx);
			ec_rd_data(user.pjt.ver + 2);

			return 0;
		}
		usleep(100);
	}
eECinfo:
	printf("ERROR: Fail to contact with EC.\n");
	return -1;
eECinfo_lock:
	printf("ERROR: Fail to found the valid EC\n");
	if(user.opt.flags.admin){
		user.pjt.name[idx + 1] = '\0';
		printf("Read Project Name:\t%s\n",user.pjt.name);
		printf("Expected Name:\t%s\n",lock_pjt_name);
	}

	return -1;
}

/*==============================================================*/
// Display Project info(beta: will change to use mbox API)
// Input:	none
// Output:	none
/*==============================================================*/
void display_pjt_info(INFO_PJT *pjt)
{
	if(pjt->name[0] == 0x00){
		return;
	}
	//project name
	printf("Project:\t%s\n",pjt->name);

	//Chip cdoe
	if(pjt->chip[0] != 0x00){
		printf("EC Chip:\t");
		if(pjt->chip[0] == 'I')
			printf("ITE-85");
		else if(pjt->chip[0] == 'E')
			printf("ENE-");
		else
			printf("Unkown -%x.",pjt->chip[0]);
		printf("%x\n",pjt->chip[1]);
	}
		
	if(user.opt.flags.admin){
		//Version table
		printf("Version table:\t%c\n",pjt->ver[0]);
		
		//Kernel Ver
		printf("Kernel Ver:\t%x.%x\n",pjt->kver[0],pjt->kver[1]);
		
		//Project ID
		printf("Prject ID:\t%X\n",pjt->id);
	}
	
	//Firmware Ver
	printf("Firmware Ver:\t%c%x.%x\n",pjt->ver[3],pjt->ver[1],pjt->ver[2]);
}
/*==============================================================*/
// check_bin_info
// Input:	none
// Output:	none
/*==============================================================*/
int check_bin_info()
{
	int lsize;
	
	if ((user.bin.ctrl = fopen(user.bin.name, "rb")) == NULL)		// open file
	{
		printf("ERROR: File \"%s\" does not exist.\n",user.bin.name);
		return -1;
	}

	fseek(user.bin.ctrl, 0x4000, SEEK_SET);					// get project info of bin file
	if(fread((UINT8 *)&user.bin.pjtinfo,1,15,user.bin.ctrl) == 0){		// 15 = 2(kver) + 1(id) + 4(ver) + 8(name)
		printf("ERROR: Read binary file fail.\n");
		return -1;
	}
	user.bin.pjtinfo.name[PJT_NAME_SIZE] = '\0';				// let name to string
	user.bin.pjtinfo.chip[0] = 0x00;					// there is no chip info in bin file
	
	fseek(user.bin.ctrl, 0, SEEK_END);					// get file size
	lsize = ftell(user.bin.ctrl);
	if (lsize == -1L) {
		printf("ERROR: Read binary file size fail.\n");
		return -1;
	}
	else if(lsize > 0x200000){
		printf("WARNING: Binary file over 2048kB.\n");
	}
	rewind(user.bin.ctrl);							// init file cursor postion
	user.bin.blocknum = (UINT16)(lsize / dROM_BLOCKSIZE);
	user.bin.over_num = (UINT16)(lsize % dROM_BLOCKSIZE);
	
	return 0;
}

/*==============================================================*/
// Display usage page
// Input:	none
// Output:	none
/*==============================================================*/
void display_usage(void)
{
	if(user.opt.flags.admin)
		printf("Project Name: %s\n",PJT_NAME);
	printf("\nUsage: ecutil - [OPTION] - [OPTION2 ...] - [BIN FILE]\n");
	printf("Example: ecutil [-f] -u EC_FW_V10.bin\n\n");
	printf("Option:\n");
	printf("\t-i\tDisplay EC Firmware info.\n");
	printf("\t-b\tDisplay Binary file info.\n");
	printf("\t-f\tNo prompt.\n");
	printf("\t-u\tUpdate EC firmware.\n");
}

/*==============================================================*/
// MAIN
/*==============================================================*/
int 
main(
  IN int Argc,
  IN char **Argv
  )
{
	int rtn = 0;
	int idx;
	char key;
	char *filename = NULL;
#ifdef _LINUX_	
	if(system("tput clear"));
#endif
	
	printf("\nCopyright(c) , Advantech Corporation 2015-2016\n");
	printf("Program:ECUTIL  Advantech EC utility for %s.\n",env);
	printf("Date:%s\n",ecutil_data);
	printf("Version:%s\n\n",ecutil_ver);

	//---------------------
	// Decode argument
	//---------------------
	if(Argc <= 1){
		goto eErrArg;											// no argument
	}
	else{
		decode_arg(Argc, Argv);
		if((user.opt.all & 0xFC) == 0){							// skip -su/-f
			if(user.opt.flags.no_ack == 1)
				printf("ERROR: Too few argument.\n");
			else
				printf("ERROR: No invalid argument.\n");
			goto eErrArg;										// no invalid argument
		}
		else if(user.bin.name != NULL){
			if(user.bin.name[0] == '-'){
				printf("ERROR: No input file.\n");				// it's no a name after the -b or -u
				goto eErrArg;
			}
		}
	}

	//---------------------
	// Print Firmware info
	//---------------------
	if(user.opt.flags.bin_info || user.opt.flags.update){
		if(check_bin_info() != 0){
			goto eErrEnd;											// bin file error
		}
		filename = &user.bin.name[0];
		for(idx = 0; user.bin.name[idx] != '\0';idx++){				// get file name; remove path
			if(user.bin.name[idx] == CHAR_SLASH){
				filename = &user.bin.name[idx + 1];
			}
		}
		printf("\nBIN File Infomation.\n");
		printf("File name:\t%s\n",filename);
		display_pjt_info(&user.bin.pjtinfo);
		printf("Bin Size:\t%d kB\n",user.bin.blocknum * dROM_BLOCKSIZE / 1024);
		printf("Blcok:\t\t%d\n",user.bin.blocknum);
		user.opt.flags.bin_info = 0;
		if(user.opt.all == 0)
			goto eEnd;
	}

	//---------------------
	// Check/Open IO port
	//---------------------
	if(ec_set_port(IOPORT_ECCMD, IOPORT_ECDAT) != 0){
		goto eErrEnd;
	}
	
	//---------------------
	// check Project info
	//---------------------
	if(check_ec_info() != 0)
		goto eErrEnd;

	//---------------------
	// Print Project info
	//---------------------
	if(user.opt.flags.pjt_info){
		printf("\nRead EC Firmware Infomation.\n");
		display_pjt_info(&user.pjt);
	}

	//---------------------
	// Update EC firmware
	//---------------------
	if(user.opt.flags.update){
		if(strcmp((const char *)user.bin.pjtinfo.name, (const char *)user.pjt.name) != 0){	// check bin_file/ec match
			printf("ERROR: Unkown/Invalid input binary file.\n");
			goto eErrEnd;
		}
		
		if((user.pjt.chip[0] != 'I') && (user.pjt.chip[0] != 'E')){							// check supported chip
			printf("ERROR: Unkown chip vendor.\n");
			printf("\tEC update tool only supports the ITE/ENE chip.\n");
			goto eErrEnd;
		}

		printf("\nAfter the EC update process, must to shutdown PC.\n");
		if(user.opt.flags.no_ack == 0){
			printf("Do you want to continue(y/n)? ");
		
			if(scanf("%c",&key))															// fimware update prompt
				;
			if((key == 'y') || key == 'Y'){
				printf("\nNOTE:\tPlease do not shutdown PC during update process.\n");
				printf("Update EC firmware...\n");
			}
			else{
				printf("Cancel update process.\n");
				goto eEnd;
			}
		}
		if(isp_flash_ec(&user.bin) != 0)													// isp process start
			printf("\nUpdate EC firmware fail.");
		else{
			printf("\n\nEC firmware update successful.");									// isp done
			printf("\n***\tPlease Remove All Power Source,\t\t***");
			printf("\n***\tTo Complete the Update Process.\t\t***");
			rtn = 1;		// is mean update done
		}
		//isp_wdt_reset(); // *** will reset PC ***
	}
	
eEnd:
	if(user.bin.ctrl != NULL)
		fclose(user.bin.ctrl);
	ec_close_port();
	if(rtn == 1)
		system_shutdown();																	// auto shutdown system
	printf("\n");
	return rtn;
	
eErrArg:
	display_usage();
eErrEnd:
	rtn = ERR_EXIT;
	goto eEnd;
}
