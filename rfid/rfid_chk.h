/**
 *	\file	rfid_chk.h
 *
 *	\brief	ʵ��rfid�е�У�顣
 */


#ifndef _RFID_CHECK_H_
#define _RFID_CHECK_H_

#include "rfid_def.h"
/**
 * 	\function ��У��
 *
 *	\input	���룺BYTE bDat: 0/1
 *	\ouput
 *	\return	����У���ֵ��0����1.
 */
int OldParity(BYTE Dat );

/**
 * 	CRCУ��
 *
 *	\input	BYTE 	aBuf[],������
 *			UINT16	u16Len ����������
 *	\ouput	BYTE 	*pCRCMSB	CRC��8bit
 *			BYTE	*pCRCLSB	CRC��8bit
 *	\return	1�ɹ���<0ʧ��.
 */

int ISO14443A_CRC( BYTE aBuf[], UINT16 u16Len,BYTE *pCRCMSB, BYTE *pCRCLSB );
int ISO14443B_CRC( BYTE aBuf[], UINT16 u16Len,BYTE *pCRCMSB, BYTE *pCRCLSB );
int ISO15693_CRC( BYTE aBuf[], UINT16 u16Len, BYTE *pCRCMSB, BYTE *pCRCLSB );

/**
 *	ISO14443B
 *
 *
 *	ÿ���ֽڣ�ǰ��1bit start, �����1bit�� stop��1���ص�egt.
 *	���ǵ����䣺�ȴ���LSB������Ҫ�Ե�������:
 */
// ͬʱҪ�Ե�������
int ISO14443B_ByteExp( BYTE byDat );
// ����ԭʼ�ֽ�
BYTE IOS14443B_UnByteExp( UINT16 u16Dat );

#endif
