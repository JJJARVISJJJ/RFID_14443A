/**
 *	\file	rfid_iso14443A_picc.h	
 *
 *
 *
 */

#ifndef _RFID_ISO14443A_PICC_H_
#define _RFID_ISO14443A_PICC_H_

#include "rfid_def.h"
#include "rfid_iso14443A_cmdtx.h"
#include "rfid_iso14443A_cmdrx.h"


typedef struct ISO_14443A_PICC_tag
{	
	UCHAR	ucStatus;
	UCHAR	ucCmpBits;					// ��ǰ�Ѿ��ͳ�ȥ���ٸ�bit�ˡ�
	UCHAR	ucCLn;						// 0/1/2�ֱ��ʾ1/2/3��UID����.
	UCHAR	ucSAK;						// ����͵�sak
	UCHAR	aucUID[12];					// ���10���ֽ�
}ISO_14443A_PICC;

int PICC_ISO14443A_run(ISO_14443A_PICC	*pstPICC);
int PICC_ISO14443A_process( ISO_14443A_PICC	*pstPICC, BYTE abyRxBuf[], UINT16 u16BitLen );

int PICC_ISO14443A_PICC_anticollision( ISO_14443A_PICC *pstPICC, BYTE abyRxCmd[], UINT16 u16CmdBitLen );
int PICC_ISO14443A_PICC_select( ISO_14443A_PICC *pstPICC, BYTE abyRxCmd[], UINT16 u16CmdBitLen );
int GetUIDFromString( ISO_14443A_PICC *pstPICC, char *szUID );

#endif