/**
 *	\file	rfid_iso14443A_cmdtx.h
 *
 *	\brief	ISO14443A������.
 *
 */
#ifndef _RFID_ISO14443A_CMD_H_
#define _RFID_ISO14443A_CMD_H_

#include "rfid_iso14443A_codec.h"
#include "rfid_iso14443A_frm.h"
#include "rfid_com.h"

/*-------------------------------------------------------------*\
 *	ISO14443A 
 *	PCD����������
 *-------------------------------------------------------------*/

/**
 * 	\fun	ISO14443A_REQA_req()
 *	\brief	PCD->PICC
 *
 *	\input	.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_REQA();
/**
 * 	\fun	ISO14443A_WUPA_req()
 *	\brief	PCD->PICC
 *
 *	\input	.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_WUPA();
/**
 * 	\fun	ISO14443A_SELECT()
 *	\brief	PCD->PICC
 *
 *	\input	.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_SELECT( BYTE sel, BYTE abyUID[] );

/**
 * 	\fun	ISO14443A_ANTICOLLISION()
 *	\brief	PCD->PICC
 *
 *	\input	.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_AnticollisionFrame_req( BYTE bySEL, BYTE abyUID[], UINT16 u16BitLen );

/**
 * 	\fun	ISO14443A_HALT()
 *	\brief	PCD->PICC
 *
 *	\input	.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_HALT();

/*-------------------------------------------------------------*\
 *	ISO14443A 
 *	PICC����������
 *-------------------------------------------------------------*/

/**
 * 	\fun	ISO14443A_ATQA()
 *	\brief	PCD->PICC
 *			��REQA/WUKA��Ӧ��
 *
 *	\input	.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_ATQA( BYTE uidCLn );


/**
 * 	\fun	ISO14443A_SAK()
 *	\brief	PCD->PICC
 *			��SELECT��Ӧ��
 *
 *	\input	.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_SAK( BYTE cascade );


/**
 * 	\fun	ISO14443A_ANTICOLLISION()
 *	\brief	PCD->PICC
 *			��ANTICOLLISION��Ӧ��
 *
 *	\input	.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_AnticollisionFrame_rsp( 
				BYTE		byBitLenofFirstByte,		// ��һ���ֽ��е�bit Len���ڷ���ײ֡����Ҫ
				BYTE		byFirstByte,
				BYTE 		abyUID[], UINT16 u16UIDBitLen, 
				BYTE		byBCC						
		);


int ISO14443A_ShortFrame_req( BYTE byCmd );
int ISO14443A_stdFrame_req( BYTE abyBuf[], UINT16 u16BitLen );
int ISO14443A_stdFrame_rsp( BYTE abyBuf[], UINT16 u16BitLen );

#endif