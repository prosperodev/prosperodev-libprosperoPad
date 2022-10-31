/*
#  ____   ____   ____      ___  ____   ____  ____   ____  ____   ____ _     _
#  ____|  ____> |    |    |     ____| <____  ____> |    |     | <____  \   /
# |      |    \ |____| ___|    |      <____ |    \ |____| ____| <____   \_/  
# 
# PROSPERODEV Open Source Project.
#------------------------------------------------------------------------------------
# Copyright 2010-2022, prosperodev - http://github.com/prosperodev
# Licenced under MIT License
# Review README & LICENSE files for further details.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>

//#include <types/kernel.h>
//#include <types/userservice.h>
//#include <types/pad.h>
#include <prosperodev.h>
#ifdef __cplusplus
extern "C" {
#endif

#define	PROSPEROPAD_L3		0x00000002
#define	PROSPEROPAD_R3		0x00000004
#define	PROSPEROPAD_OPTIONS	0x00000008
#define	PROSPEROPAD_UP		0x00000010
#define	PROSPEROPAD_RIGHT		0x00000020
#define	PROSPEROPAD_DOWN		0x00000040
#define	PROSPEROPAD_LEFT		0x00000080
#define	PROSPEROPAD_L2		0x00000100
#define	PROSPEROPAD_R2		0x00000200
#define	PROSPEROPAD_L1		0x00000400
#define	PROSPEROPAD_R1		0x00000800
#define	PROSPEROPAD_TRIANGLE	0x00001000
#define	PROSPEROPAD_CIRCLE		0x00002000
#define	PROSPEROPAD_CROSS		0x00004000
#define	PROSPEROPAD_SQUARE		0x00008000
#define	PROSPEROPAD_TOUCH_PAD	0x00100000
#define	PROSPEROPAD_INTERCEPTED	0x80000000


typedef struct ProsperoPadConfig
{
	SceUserServiceUserId userId;
	ScePadData *padDataCurrent;
	ScePadData *padDataLast;
	unsigned int buttonsPressed;
	unsigned int buttonsReleased;
	unsigned int buttonsHold;
	int padHandle;
	int prosperopad_initialized;
	
}ProsperoPadConfig;

int prosperoPadInit();
void prosperoPadFinish();
ProsperoPadConfig *prosperoPadGetConf();
int prosperoPadCreateConf();
int prosperoPadSetConf(ProsperoPadConfig *conf);
int prosperoPadInitWithConf(ProsperoPadConfig *conf);
bool prosperoPadGetButtonHold(unsigned int filter);
bool prosperoPadGetButtonPressed(unsigned int filter);
bool prosperoPadGetButtonReleased(unsigned int filter);
unsigned int prosperoPadGetCurrentButtonsPressed();
unsigned int prosperoPadGetCurrentButtonsReleased();
void prosperoPadSetCurrentButtonsPressed(unsigned int buttons);
void prosperoPadSetCurrentButtonsReleased(unsigned int buttons);
int prosperoPadUpdate();

#ifdef __cplusplus
}
#endif
