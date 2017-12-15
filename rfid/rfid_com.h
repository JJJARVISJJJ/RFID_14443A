/**
 *	\file	RFID_com.h
 *
 *	\brief	����RFIDϵͳ�У���/������������շ�.
 *
 */
#ifndef _RFID_COM_H_
#define _RFID_COM_H_

#include "rfid_def.h"

void RFID_COM_Init( int pcd_or_picc );

/**
 *	�������ݣ���������
 */
int RFID_Card_Tx( UINT16 u16BitLen, UCHAR *pBuf );

/**
 *	����������.
 *
 *	����ֵ����Ϊ��
 *		>0��ʵ���յ����ݵı��س���;
 *		=0, û���յ�����;
 */
int RFID_Card_Rx( UINT16 u16ByteLen, UCHAR *pBuf );

/**
 *	�������ݣ���������
 */
int RFID_Reader_Tx( UINT16 u16BitLen, UCHAR *pBuf );

/**
 *	����������.
 *
 *	����ֵ����Ϊ��
 *		>0��ʵ���յ����ݵı��س���;
 *		=0, û���յ�����;
 */
int RFID_Reader_Rx( UINT16 u16ByteLen, UCHAR *pBuf );

#endif

 