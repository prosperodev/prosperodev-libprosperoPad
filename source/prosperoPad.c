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

#include <stdio.h>
//#include <user_mem.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <userservice.h>
//#include <pad.h>
#include <prosperodev.h>

#include <debugnet.h>
#include "prosperoPad.h"

ProsperoPadConfig *prosperoPadConf=NULL;

int prosperopad_external_conf=-1;

void prosperoPadFinish()
{
	int ret;
	if(prosperopad_external_conf!=1)
	{
		if(prosperoPadConf->prosperopad_initialized==1)
		{		
			ret=scePadClose(prosperoPadConf->padHandle);
		
			debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s scePadClose return 0x%08X\n",__FUNCTION__,ret);
		}
		prosperoPadConf->prosperopad_initialized=-1;
	
		debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s finished\n",__FUNCTION__);
	}
}

ProsperoPadConfig *prosperoPadGetConf()
{
	if(prosperoPadConf)
	{
		return prosperoPadConf;
	}
	
	return NULL; 
}
int prosperoPadCreateConf()
{	
	if(!prosperoPadConf)
	{
		prosperoPadConf=(ProsperoPadConfig *)malloc(sizeof(ProsperoPadConfig));
		
		prosperoPadConf->userId=0;
		prosperoPadConf->padHandle=-1;
		
		prosperoPadConf->padDataCurrent=(ScePadData *)malloc(sizeof(ScePadData));
		prosperoPadConf->padDataLast=(ScePadData *)malloc(sizeof(ScePadData));
		
		prosperoPadConf->buttonsPressed=0;
		prosperoPadConf->buttonsReleased=0;
		prosperoPadConf->buttonsHold=0;
		
		prosperoPadConf->prosperopad_initialized=-1;
		
		return 0;
	}
	
	if(prosperoPadConf->prosperopad_initialized==1)
	{
		return prosperoPadConf->prosperopad_initialized;
	}
	//something weird happened
	return -1;			
}
int prosperoPadSetConf(ProsperoPadConfig *conf)
{
	if(conf)
	{
		prosperoPadConf=conf;
		prosperopad_external_conf=1;
		return prosperoPadConf->prosperopad_initialized;
	}
	
	return 0; 
}
int prosperoPadInitWithConf(ProsperoPadConfig *conf)
{
	int ret;
	ret=prosperoPadSetConf(conf);
	if(ret)
	{
		debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s already initialized using configuration external\n",__FUNCTION__);
		debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s prosperopad_initialized=%d\n",__FUNCTION__,prosperoPadConf->prosperopad_initialized);
		debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s ready to have a lot of fun...\n",__FUNCTION__);
		return prosperoPadConf->prosperopad_initialized;
	}
	else
	{
		return 0;
	}
}
unsigned int prosperoPadGetCurrentButtonsPressed()
{
	return prosperoPadConf->buttonsPressed;
}
void prosperoPadSetCurrentButtonsPressed(unsigned int buttons)
{
	prosperoPadConf->buttonsPressed=buttons;
}
unsigned int prosperoPadGetCurrentButtonsReleased()
{
	return prosperoPadConf->buttonsReleased;
}
void prosperoPadSetCurrentButtonsReleased(unsigned int buttons)
{
	prosperoPadConf->buttonsReleased=buttons;
}

bool prosperoPadGetButtonHold(unsigned int filter)
{
	if((prosperoPadConf->buttonsHold&filter)==filter)
	{
		return 1;
	}
	return 0;
}
bool prosperoPadGetButtonPressed(unsigned int filter)
{
	if((prosperoPadConf->buttonsPressed&filter)==filter)
	{
		return 1;
	}
	return 0;
}
bool prosperoPadGetButtonReleased(unsigned int filter)
{
 	if((prosperoPadConf->buttonsReleased&filter)==filter)
	{
		if(~(prosperoPadConf->padDataLast->buttons)&filter)
		{
			return 0;
		}
		return 1;
	}
	return 0;
}
int prosperoPadUpdate()
{
	int ret;
	unsigned int actualButtons=0;
	unsigned int lastButtons=0;
	memcpy(prosperoPadConf->padDataLast,prosperoPadConf->padDataCurrent,sizeof(ScePadData));
	
	ret=scePadReadState(prosperoPadConf->padHandle,prosperoPadConf->padDataCurrent);
	debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s scePadReadState return 0x%08X buttons:0x%08X lx:0x%08X ly:0x%08X \n",__FUNCTION__,ret,prosperoPadConf->padDataCurrent->buttons,prosperoPadConf->padDataCurrent->lx,prosperoPadConf->padDataCurrent->ly);
	if(ret==0 && prosperoPadConf->padDataCurrent->connected==1)
	{
		actualButtons=prosperoPadConf->padDataCurrent->buttons;
		lastButtons=prosperoPadConf->padDataLast->buttons;
		prosperoPadConf->buttonsPressed=(actualButtons)&(~lastButtons);
		if(actualButtons!=lastButtons)
		{
			prosperoPadConf->buttonsReleased=(~actualButtons)&(lastButtons);
		}
		else
		{
			prosperoPadConf->buttonsReleased=0;
		}
		prosperoPadConf->buttonsHold=actualButtons&lastButtons;
		return 0;
		
	}
	else
	{
		return -1;
	}
}
int prosperoPadGetUserHome()
{
	char *dentbuf;
	struct dirent *dent;
	int dfd;
	int userId;
	struct stat st;
	dfd = open("/user/home", O_RDONLY, 0);
	if(dfd<0)
	{
		debugNetPrintf(DEBUGNET_ERROR,"[PROSPEROPAD] %s opening /home/user return error 0x%08X!\n",__FUNCTION__,dfd);
		return -1;
	}
	int err=fstat(dfd,&st);
	if(err<0)
	{
		debugNetPrintf(DEBUGNET_ERROR,"[PROSPEROPAD] %s fstat from directory /home/user return error 0x%08X!\n",__FUNCTION__,err);
		return -1;
	}
	dentbuf=malloc(st.st_blksize+sizeof(struct dirent));
	if(dentbuf==NULL)
	{
		debugNetPrintf(DEBUGNET_ERROR,"[PROSPEROPAD] %s malloc error!\n",__FUNCTION__);
		return -1;
	}
	// Make sure we will have a null terminated entry at the end.Thanks people leaving CryEngine code for orbis on github  :)
	for(int i=0;i<st.st_blksize+sizeof(struct dirent);i++)
	{
		dentbuf[i]=0;
	}
	err=getdents(dfd, dentbuf, st.st_blksize);
	int nOffset=err;
	while(err>0 && err< st.st_blksize)
	{
		err=getdents(dfd,dentbuf+nOffset,st.st_blksize-nOffset);
		nOffset += err;
	}
	if(err>0)
		err=0;
	dent=(struct dirent *)dentbuf;
	while(dent->d_fileno)
	{
		if(strcmp(dent->d_name,".")!=0 && strcmp(dent->d_name,"..")!=0)
		{
			debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s entry %s\n",__FUNCTION__,dent->d_name);
			sscanf(dent->d_name,"%x",&userId);
			debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s userid  0x%08X\n",__FUNCTION__,userId);

		}
		dent = (struct dirent *)((void *)dent + dent->d_reclen);
	}
	return userId;
}
int prosperoPadInit()
{
	int ret;
   
	int param=700;
	
	if(prosperoPadCreateConf()==1)
	{
		return prosperoPadConf->prosperopad_initialized;
	}
	if (prosperoPadConf->prosperopad_initialized==1) 
	{
		debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s is already initialized!\n",__FUNCTION__);
		return prosperoPadConf->prosperopad_initialized;
	}
	ret=sceUserServiceInitialize(&param);
	debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s sceUserServiceInitialize return 0x%08X!\n",__FUNCTION__,ret);
	
       
	if(ret==0 || ret==0x80960003)
	{
		ret=scePadInit();
		if(ret<0)
		{
			debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s scePadInit return error 0x%08X\n",__FUNCTION__,ret);
			return -1;
		}
		debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s scePadInit return %d\n",__FUNCTION__,ret);
		if(ret==0)
		{

			ret=sceUserServiceGetInitialUser(&prosperoPadConf->userId);
			if(ret<0 || prosperoPadConf->userId<0)
			{
				debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s sceUserServiceGetInitialUser return  0x%08X and userId 0x%08X\n",__FUNCTION__,ret,prosperoPadConf->userId);
				prosperoPadConf->userId=prosperoPadGetUserHome();
			}
			debugNetPrintf(DEBUGNET_INFO,"[PROSPEROPAD] %s switching to userId 0x%08X\n",__FUNCTION__,prosperoPadConf->userId);

	
			prosperoPadConf->padHandle=scePadOpen(prosperoPadConf->userId, 0, 0, NULL);
			if(prosperoPadConf->padHandle<0)
			{
				debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s scePadOpen return error 0x%08X\n",__FUNCTION__,prosperoPadConf->padHandle);
				//if(prosperoPadConf->padHandle==0x80920004)
				//{
					prosperoPadConf->padHandle=scePadGetHandle(prosperoPadConf->userId,0,0);
					debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s pad already open tryng with scePadGetHandle return 0x%08X\n",__FUNCTION__,prosperoPadConf->padHandle);

				//}
				if(prosperoPadConf->padHandle<0) 
				return -1;
			}
			debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s handle 0x%08X\n",__FUNCTION__,prosperoPadConf->padHandle);
			if(prosperoPadConf->padHandle>0)
			{
				prosperoPadConf->prosperopad_initialized=1;
            }
		}
	}
	debugNetPrintf(DEBUGNET_DEBUG,"[PROSPEROPAD] %s libprosperopad initialized\n",__FUNCTION__);

	return prosperoPadConf->prosperopad_initialized;
}

