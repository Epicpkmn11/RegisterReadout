/*---------------------------------------------------------------------------------

	Simple eeprom example
	-- davr

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "nds/arm9/console.h"

unsigned int * SCFG_ROM=(unsigned int*)0x4004000;
unsigned int * SCFG_CLK=(unsigned int*)0x4004004; 
unsigned int * SCFG_EXT=(unsigned int*)0x4004008;
unsigned int * SCFG_MC=(unsigned int*)0x4004010;
unsigned int * SCFG_CPUID=(unsigned int*)0x4004D04;
unsigned int * SCFG_CPUID2=(unsigned int*)0x4004D00;

struct AutoloadInfo {
	char autoloadId[4]; // "TLNC"
	u8 unknown; // 0x01
	u8 length; // 0x18
	u16 crc;
	u64 oldTid;
	u64 newTid;
	u32 flags; // 0x17
	u32 unused;
	char unused2[0xE0];
};

//---------------------------------------------------------------------------------
void dopause() {
//---------------------------------------------------------------------------------
	iprintf("Press Start to display Arm7...\n");
	while(1) {
		scanKeys();
		if(keysUp() & KEY_START) {
			break;
		}
		swiWaitForVBlank();
	}
	scanKeys();
}

//---------------------------------------------------------------------------------
void reboot(u64 tid) {
//---------------------------------------------------------------------------------
	AutoloadInfo *autoloadInfo = (AutoloadInfo *)0x02000300;
	memset(autoloadInfo, 0, sizeof(AutoloadInfo));
	memcpy(autoloadInfo->autoloadId, "TLNC", 4);
	autoloadInfo->unknown = 0x01;
	autoloadInfo->length = 0x18;
	autoloadInfo->oldTid = tid;
	autoloadInfo->newTid = tid;
	autoloadInfo->flags = 0x17;
	autoloadInfo->crc = swiCRC16(0xFFFF, &autoloadInfo->oldTid, 0x18);

	fifoSendValue32(FIFO_USER_02, 1);
	while(1)
		swiWaitForVBlank();
}

//---------------------------------------------------------------------------------
void dopauseExit() {
//---------------------------------------------------------------------------------
	for (int i = 0; i < 30; i++) { swiWaitForVBlank(); }
	iprintf("Press Start to exit...\n");
	while(1) {
		scanKeys();
		if(keysDown() & KEY_START) {
			powerOn(PM_BACKLIGHT_TOP);
			break;
		}
		swiWaitForVBlank();
	}
	scanKeys();
}

void getSFCG_ARM7() {
	
	iprintf( "SCFG_ROM:" );
	fifoSendValue32(FIFO_USER_01,(unsigned int)SCFG_ROM);	
	iprintf( " \n" );
	iprintf( "SCFG_CLK:" );
	fifoSendValue32(FIFO_USER_01,(unsigned int)SCFG_CLK);
	iprintf( " \n" );
	iprintf( "SCFG_EXT:" );
	fifoSendValue32(FIFO_USER_01,(unsigned int)SCFG_EXT);
	iprintf( " \n" );
	iprintf( "ConsoleID:" );
	fifoSendValue32(FIFO_USER_01,(unsigned int)SCFG_CPUID);
	iprintf( " \n" );
	iprintf( "          " );
	fifoSendValue32(FIFO_USER_01,(unsigned int)SCFG_CPUID2);
}


static void myFIFOValue32Handler(u32 value,void* data) { iprintf( " %08x\n", value ); }

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
	// Top screen not used for anything right now.
	powerOff(PM_BACKLIGHT_TOP);

	consoleDemoInit();
	
	defaultExceptionHandler();

	iprintf( "ARM9 SCFG:\n" );
	iprintf( " \n" );
	iprintf( "SCFG_ROM: %08x\n", *SCFG_ROM );
	iprintf( "SCFG_CLK: %08x\n", *SCFG_CLK );
	iprintf( "SCFG_EXT: %08x\n", *SCFG_EXT );
	iprintf( " \n" );
	iprintf( "\n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( "Slot-1 Status: \n" );
	iprintf( " \n" );
	iprintf( "SCFG_MC: %08x\n", *SCFG_MC ); 
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	dopause();
	
	fifoSetValue32Handler(FIFO_USER_02,myFIFOValue32Handler,0);

	iprintf( "ARM7 SCFG:\n" );
	iprintf( " \n" );
	getSFCG_ARM7();
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	iprintf( " \n" );
	dopauseExit();

	reboot(0x00030005484E494A); // DSi Camera (J) -- 00030005-HNIJ

	return 0;
}

