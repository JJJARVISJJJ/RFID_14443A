/**
 *	\file	rfid_iso14443A_pcd.h	
 *
 *
 *
 */

#ifndef _RFID_ISO14443A_PCD_H_
#define _RFID_ISO14443A_PCD_H_

#include "rfid_def.h"
#include "rfid_iso14443A_cmdtx.h"
#include "rfid_iso14443A_cmdrx.h"

typedef struct ISO_14443A_PCD_tag
{	
	UCHAR	ucStatus;
	UCHAR	ucCmpBits;					// ��ǰ�Ѿ��ͳ�ȥ���ٸ�bit�ˡ�
	UCHAR	ucCLn;						// 0/1/2�ֱ��ʾ1/2/3��UID����.
	UCHAR	ucSEL;						// ��ǰSEL��ֵ.
	UCHAR	ucSAK;						// SAK��ֵ
	UCHAR	ucAntiBit;					// ����ײ�����У������ӵ�bit��Ĭ��2����ʾ������/û�д��ڷ���ײ�����С�
	UINT16	u16WaitTime;					// �����ȥ�󣬵ȴ������� 
	UCHAR	aucUID[12];					// ���10���ֽ�
}ISO_14443A_PCD;

int PCD_ISO14443A_run( ISO_14443A_PCD *pstPCD );
int PCD_ISO14443A_process( ISO_14443A_PCD *pstPCD, UCHAR aucRxBuf[], UINT16 u16RxBufBitLen );
// ��ͻģ�⣺
// �����յ��ı�����������ʵ���յ��ı�����
int PCD_ISO14443A_CollisionEmu( ISO_14443A_PCD *pstPCD,
			BYTE byBitLenofFirstByte, BYTE firstByte, 
			UCHAR aucCmdBuf[], UINT16 u16CmdBitLen );
// ����SELECT֡
int PCD_ISO14443A_SendSELECT( ISO_14443A_PCD *pstPCD );
// ���ͷ���ײ֡��
// ����������1���ء�
int PCD_ISO14443A_SendANTICOLLISION( ISO_14443A_PCD *pstPCD );
#endif




