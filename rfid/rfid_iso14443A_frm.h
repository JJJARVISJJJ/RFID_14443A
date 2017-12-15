/**
 *	rfid_iso14443A_frm.h
 *
 *
 *	���ڱ�׼֡������ײ֡��������У�飻
 *	���ڱ�׼֡������CRC;
 *	���ڷ���ײ֡����PICC->PCD���򣬿����ֽڲ�ȫ������ĳ�����
 *	�����������У������ֽڣ�������ͱ��أ�������߱��أ�
 */

#ifndef _RFID_ISO14443A_FRM_H_
#define _RFID_ISO14443A_FRM_H_

#include "rfid_def.h"
#include "rfid_chk.h"


int ISO14443A_stdFraming( BYTE aInBuf[], UINT16 u16InBitLen, BYTE aoutBuf[], UINT16 *pu16OutBitLen );
int ISO14443A_ShortFraming( BYTE byCmd, BYTE aoutBuf[], UINT16 *pu16OutBitLen );
int ISO14443A_AnticollisionFraming( 				
				BYTE		byBitLenofFirstByte,						// ��һ���ֽ��е�bit Len���ڷ���ײ֡����Ҫ
				BYTE		byFirstByte,
				BYTE 		ainBuf[], UINT16 u16inBitLen, 
				BYTE 		aoutBuf[], UINT16 *pu16OutBitLen 
		);
		
int ISO14443A_Un_stdFraming(BYTE ainBuf[], UINT16 u16inBitLen, BYTE aoutBuf[], UINT16 *pu16OutbyLen );
int ISO14443A_Un_ShortFraming( BYTE ainBuf[], UINT16 u16InBitLen, BYTE *pbyCmd );
int ISO14443A_Un_AnticollisionFraming(
				BYTE		byBitLenofFirstByte,						// ��һ���ֽ��е�bit Len���ڷ���ײ֡����Ҫ
				BYTE		*pbyFirstByte,
				BYTE 		ainBuf[], UINT16 u16inBitLen, 
				BYTE 		aoutBuf[], UINT16 *pu16OutBitLen 
		);	
		
/**
 *	ISO14443A Frame
 *	��׼
 *	������
 *				����һ���ֽ��ڵı���˳��
 *				У��P��У��.
 *	������SOF/EOF
 */
 
int ISO14443A_Framing(
				BYTE		byBitLenofFirstByte,						// ��һ���ֽ��е�bit Len���ڷ���ײ֡����Ҫ
				BYTE		byFirstByte,								// ��һ�ֽڡ���u16BitLenofFirstByte=0�����Բ����� 
				BYTE 		ainBuf[], UINT16 u16inBitLen, 				// �����ֽ�
				BYTE 		aoutBuf[], UINT16 *pu16OutBitLen
			 );

int ISO14443A_unFraming(
				BYTE		byBitLenofFirstByte,						// ��һ���ֽ��е�bit Len���ڷ���ײ֡����Ҫ
				BYTE		*byFirstByte,								// ��һ�ֽڡ���u16BitLenofFirstByte=0�����Բ����� 
				BYTE 		ainBuf[], UINT16 u16inBitLen, 				// �����ֽ�
				BYTE 		aoutBuf[], UINT16 *pu16OutBitLen			// ������ز�����*byBitLenofFirstByte
			);
#endif


 