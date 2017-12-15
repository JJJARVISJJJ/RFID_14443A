/**
 *	rfid_iso14443A_codec.h
 *
 */

#ifndef _RFID_ISO14443A_CODEC_H_
#define _RFID_ISO14443A_CODEC_H_

#include "rfid_def.h"

#define ISO14443A_PCD_X			0xD		// 1101, '1'
#define ISO14443A_PCD_Y 		0xF		// 1111, '0'	EOF
#define ISO14443A_PCD_Z 		0x7		// 0111, '0', 	SOF

#define ISO14443A_PCD_SOF		ISO14443A_PCD_Z
// EOFΪ0, #define ISO14443A_PCD_EOF
#define ISO14443A_PCD_1			ISO14443A_PCD_X
#define ISO14443A_PCD_0P1		ISO14443A_PCD_Y		// ����0��ǰһ������Ϊ1
#define ISO14443A_PCD_0P0		ISO14443A_PCD_Z		// ����0��ǰһ������Ϊ0
#define ISO14443A_BIT_CODEC_LEN	4					// ÿ�����ر��볤��Ϊ4


#define ISO14443A_PICC_D 		0xC		// 1100��'1', SOF
#define ISO14443A_PICC_E 		0x3		// 0011, '0'
#define ISO14443A_PICC_F		0x0		// 0000,      EOF

#define ISO14443A_PICC_SOF		ISO14443A_PICC_D
#define ISO14443A_PICC_EOF		ISO14443A_PICC_F
#define ISO14443A_PICC_1		ISO14443A_PICC_D
#define ISO14443A_PICC_0		ISO14443A_PICC_E

// ISO/IEC 14443A, PCD->PICCʹ��
#define RFID_CODE_MILLER_1			13	// 1101
#define RFID_CODE_MILLER_0_p1		15	// 1111
#define RFID_CODE_MILLER_0_p0		7	// 0111

// ISO/IEC 14443A, PICC->PCDʹ��
#define RFID_CODE_MANCHESTER_1		12	// 1100
#define RFID_CODE_MANCHESTER_0		3	// 0011


/**
 * 	\function
 *
 *	\input	BYTE 	aInBuf[],		���뻺����
 *			UINT16	u16InBitLen		���������س���
 *	\ouput	BYTE 	aoutBuf			��������������
 *			BYTE	*pu16OutBitLen	��������ݳ���
 *
 *	\return	-1��ʧ�ܣ� �������ؽ������س���
 */

int ISO14443A_PCD_Code( 
			BYTE aInBuf[], UINT16 u16InBitLen, 
			BYTE aOutBuf[], UINT16 *pu16OutBitLen 
		);
			
int ISO14443A_PCD_Decode( 
			BYTE aInBuf[], UINT16 u16InBitLen, 
			BYTE aOutBuf[], UINT16 *pu16OutBitLen 
		);
			
int ISO14443A_PICC_Code( 
			BYTE aInBuf[], UINT16 u16InBitLen, 
			BYTE aoutBuf[], UINT16 *pu16OutBitLen 
		);

int ISO14443A_PICC_Decode(
			BYTE aInBuf[], UINT16 u16InBitLen, 
			BYTE aOutBuf[], UINT16 *pu16OutBitLen 
		);
		
/**
 *	1. bPCD:��		��PCD/PICC������֡������뷽ʽ/SOF��EOF����ͬ��
 *	���������������˸����;
 */
int ISO14443A_Code( 
			BOOL bPCD,							// PCD�����
			BYTE aInBuf[], UINT16 u16InBitLen, 
			BYTE aoutBuf[], UINT16 *pu16OutBitLen 
		);
			
int ISO14443A_Decode(
			BOOL bPCD, 							// PCD�����
			BYTE aInBuf[], UINT16 u16InBitLen, 
			BYTE aOutBuf[], UINT16 *pu16OutBitLen 
		);
			
/*-------------------------------------------------------------*\
 *	Miller
 *	ISO14443A PCD->PICC ��
 *-------------------------------------------------------------*/
/**
 * 	�Ľ���Miller����,����4���ر���
 *
 *	\input	���룺BYTE prebit: 0/1����ǰ���ص�ǰһ������
 *			���룺BYTE bitDat: 0/1����ǰ���������
 *	\ouput
 *	\return	bitDat=1, ����1101
 *			prebit=1,bitDat=0�����أ�1111
 *			prebit=0,bitDat=0�����أ�0111
 */
int RFID_Code_Miller( BYTE prebit, BYTE bitDat );	
/**
 * 	�Ľ���Miller���룺����0����1.����ʧ��
 *
 *	\input	���룺BYTE codedDat: 1101 for 1, 1111/0111 for 0
 *	\ouput
 *	\return	0/1
 *			-1 ,�Ƿ�����
 */
int RFID_Decode_Miller( BYTE codedDat );

/*-------------------------------------------------------------*\
 *	Manchester
 *	ISO14443A PICC->PCD
 *-------------------------------------------------------------*/

/**
 * 	 Manchester���룬����4���ر���
 *
 *	\input	���룺BYTE bitDat: 0/1����ǰ���������
 *	\ouput
 *	\return	bitDat=1, ���أ�1100
 *			bitDat=0�����أ�0011
 */
int RFID_Code_Manchester( BYTE bitDat );	// bitDat=1 or 0
/**
 * 	Manchester���룺����0����1.����ʧ��
 *
 *	\input	���룺BYTE codedDat: 1100 for 1, 0011 for 0
 *	\ouput
 *	\return	0/1
 *			-1 ,�Ƿ�����
 */
int RFID_Decode_Manchester( BYTE codedDat );

#endif
