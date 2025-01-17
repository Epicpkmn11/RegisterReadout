/*---------------------------------------------------------------------------------

	default ARM7 core

		Copyright (C) 2005 - 2010
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.

	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.

	3.	This notice may not be removed or altered from any source
		distribution.

---------------------------------------------------------------------------------*/
#include <nds.h>

unsigned int * SCFG_ROM=(unsigned int*)0x4004000;
unsigned int * SCFG_CLK=(unsigned int*)0x4004004; 
unsigned int * SCFG_EXT=(unsigned int*)0x4004008;
unsigned int * SCFG_MC=(unsigned int*)0x4004010;
unsigned int * SCFG_CPUID=(unsigned int*)0x4004D04;
unsigned int * SCFG_CPUID2=(unsigned int*)0x4004D00;

void VblankHandler(void) { }
void VcountHandler() { inputGetAndSend(); }

volatile bool exitflag = false;

void powerButtonCB() { exitflag = true; }

static void rebootHandler(u32 value,void* data) {
	if (isDSiMode()) {
		i2cWriteRegister(0x4A, 0x70, 0x01);		// Bootflag = Warmboot/SkipHealthSafety
		i2cWriteRegister(0x4A, 0x11, 0x01);		// Reset to DSi Menu
	} else {
		u8 readCommand = readPowerManagement(0x10);
		readCommand |= BIT(0);
		writePowerManagement(0x10, readCommand);
	}
}

static void myFIFOValue32Handler(u32 value,void* data) { fifoSendValue32(FIFO_USER_02,*((unsigned int*)value)); }

//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	
	dmaFillWords(0, (void*)0x04000400, 0x100);

	REG_SOUNDCNT |= SOUND_ENABLE;
	writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
	powerOn(POWER_SOUND);

	readUserSettings();
	ledBlink(0);

	irqInit();

	initClockIRQ();
	fifoInit();


	SetYtrigger(80);

	installSoundFIFO();

	installSystemFIFO();

	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

	setPowerButtonCB(powerButtonCB);	
		
	fifoSetValue32Handler(FIFO_USER_01,myFIFOValue32Handler,0);
	fifoSetValue32Handler(FIFO_USER_02,rebootHandler,0);
	
	// Keep the ARM7 mostly idle
	while (1) {
		swiWaitForVBlank();
	}
	return 0;
}

