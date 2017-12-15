/**
 *	\file	rfid_def.h
 *
 *	\brief	ͷ�ļ�����
 */
#ifndef _RFID_DEF_H_
#define _RFID_DEF_H_

//#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <memory.h>

//#define _RFID_DEBUG_

//#define _OS_WINDOWS_

#ifdef _OS_WINDOWS_
	#define _RFID_COM_FILE_
	#define PCD_WAIT_TIME	25
#else
	#define PCD_WAIT_TIME	1
#endif

typedef	unsigned char BYTE;
typedef unsigned char UCHAR;
typedef unsigned short UINT16;
typedef unsigned long ULONG;
typedef unsigned long UINT32;
typedef unsigned int  BOOL;
#define TRUE	1
#define FALSE	0

/**
 *	ϵͳ����10�� PICC.
 */

#define MAX_PICC_NUM				10

/*-----------------------------------------------------------------*\
 *	����ISO14443A�����״̬
 *
 *
 *-----------------------------------------------------------------*/
// ʵ���ϲ�����
#define	ISO_14443A_STATE_POWEROFF	0xA0			
// ϵͳ�������ʹ���IDLE״̬���൱���Ѿ��ӵ�.		
#define	ISO_14443A_STATE_IDLE		0xA1
#define	ISO_14443A_STATE_READY		0xA2
#define	ISO_14443A_STATE_ACTIVE		0xA3
#define	ISO_14443A_STATE_HALT		0xA4

// ����PCD��״̬��
#define ISO_14443A_PCD_STATE_OK			0xA8		// PCDʶ���һ�ſ�
#define ISO_14443A_PCD_STATE_ER			0xA9		// PCDʶ�𿨹����У�����

#define ISO_14443A_PCD_STATE_START		0xAA		// ��ʼ�����û���յ�����������REQA
#define ISO_14443A_PCD_STATE_ANTI		0xAB		// �յ�ATQA��ת��READY.��ʼ����ͻѭ��?
#define ISO_14443A_PCD_STATE_SELECT		0xAC		// �յ�SAK.


#define ISO_14443_CMD_REQA			0x26
#define ISO_14443_CMD_WUPA			0x52
#define ISO_14443_CMD_SEL_CL1		0x93
#define ISO_14443_CMD_SEL_CL2		0x95
#define ISO_14443_CMD_SEL_CL3		0x97
#define ISO_14443_CMD_HALT			0x50

#define ISO_14443_PCD				0xF0
#define ISO_14443_PICC				0xF1

int codec_main();
int framing_main();
int cmd_main();
int picc_main( int argc, char *argv[] );
int pcd_main( int numofpicc );

#endif

