/**
 *	\file	rfid_frm.c
 *
 *	\brief	ʵ��rfid�е���/��֡��
 *
 *			CRC��֮ǰ���㣬��Ϊ�ڷ���ײ֡�У�Ҫȫ���ֽڲ�����㣬��ֻ�в����ֽڳ�֡��
 */

//#include "StdAfx.h"

#include "rfid_chk.h"
#include "rfid_iso14443A_frm.h"
#include "memory.h"

/** 
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
			 )
{
	int i, j, k, curbit;
	int byoffset, bitoffset;
	int bitLen = 0;

	// ��0����ʾ����ײ֡
	byoffset = 0;
	aoutBuf[byoffset] = 0x00;
	bitoffset = 0;
	if( byBitLenofFirstByte )
	{
		for( i = 0; i < byBitLenofFirstByte; i++ )
		{
			curbit = ( byFirstByte >> i ) & 0x01; 

			byoffset = bitLen / 8;
			bitoffset = bitLen % 8;
			aoutBuf[byoffset] |= curbit << ( 7-bitoffset);
			bitLen++;
		}
		// OldParity.
		byoffset = bitLen / 8;
		bitoffset = bitLen % 8;
		aoutBuf[byoffset] |= 0 << ( 7-bitoffset);
		bitLen++;
	}
	
	// ��ʼ�������е�����
	i = 0;
	while( u16inBitLen  )
	{
		// ���� bitLen�������ֽ�.
		if( u16inBitLen > 8 )
		{
			k = 8;
		}
		else
		{
			k = u16inBitLen;
		}
		
		for( j = 0; j < k; j++ )
		{
			// ���� bitLen�������ֽ�.
			curbit = ( ainBuf[i] >> j ) & 0x01;
			
			// ���㵱ǰ�ֽ�/����.
			byoffset = bitLen / 8;
			bitoffset = bitLen % 8;
			if( bitoffset == 0 )
			{
				aoutBuf[byoffset] = 0x00;
			}
			aoutBuf[byoffset] |= curbit << ( 7-bitoffset);
			bitLen++;
		}
		
		if( k == 8 )
		{
			// �������ֽں󣬲��м�У��
			curbit = OldParity( ainBuf[i] );
			byoffset = bitLen / 8;
			bitoffset = bitLen % 8;
			if( bitoffset == 0 )
			{
				aoutBuf[byoffset] = 0x00;
			}			
			aoutBuf[byoffset] |= curbit << ( 7-bitoffset);
			bitLen++;
		}
		i++;
		u16inBitLen -= k;
	}
	*pu16OutBitLen = bitLen;
	
	return bitLen;
}

// �ӿ�֡
// Ҫ����������
// ����CRC��CRC�ڵ��øó���ǰ���Ѿ�ȥ����.
// ���أ�-1��ʾ����				
int ISO14443A_unFraming(
				BYTE		byBitLenofFirstByte,						// ��һ���ֽ��е�bit Len���ڷ���ײ֡����Ҫ
				BYTE		*byFirstByte,								// ��һ�ֽڡ���u16BitLenofFirstByte=0�����Բ����� 
				BYTE 		ainBuf[], UINT16 u16inBitLen, 				// �����ֽ�
				BYTE 		aoutBuf[], UINT16 *pu16OutBitLen			// ������ز�����*byBitLenofFirstByte
			)
{
	//����
	int i;
	int obitLen, obyoffset, obtoffset; 	// in
	int ibitLen, ibyoffset, ibtoffset;	// out
	int curbit;
	
	
	// ����ʱ���û�֪���Լ��Ƿ��е�һ���ֽ�
	// �ڷ���ײʱ��PCD���ݷ��ͣ�֪���Լ�Ҫ����

	ibitLen = 0;
	obitLen = 0;

	// ��ʵ��u16inBitLen % 9 == byBitLenofFirstByte
	if( byBitLenofFirstByte )
	{
		*byFirstByte = 0;
		for( i = 0; i < byBitLenofFirstByte; i++ )
		{
			curbit = ( ainBuf[0] >> (7-i)) & 0x01;  
			*byFirstByte |= curbit << i;
			ibitLen++;
		}
		// ����У��λ.
		// �����˶�У��λ�Ƿ���ȷ
		curbit = ainBuf[0] >> (7-byBitLenofFirstByte);
		ibitLen++;
	}
	
	// ��ʼ���б���
	obitLen = 0;
	obyoffset = 0;
	obtoffset = 0;

	i = 0;
	while( ibitLen < u16inBitLen )
	{
		// ȡ��һbit
		ibyoffset = ibitLen / 8;
		ibtoffset = ibitLen % 8;
		curbit = ( ainBuf[ibyoffset] >> (7-ibtoffset)) & 0x01;
		
		if( (i % 9) == 8 )
		{
			// �����У�飬����������bit�Ƿ����У��λ��
			// ����
			if( curbit != OldParity( aoutBuf[obyoffset]))
			{
				return -1;
			}
		}
		else
		{			
			// ���.
			obyoffset = obitLen / 8;
			obtoffset = obitLen % 8;	// 
			if( obtoffset == 0 )
			{
				aoutBuf[obyoffset] = 0x00;
			}
			aoutBuf[obyoffset] |= curbit << obtoffset;
			obitLen++;
		}
		i++;
		ibitLen++;
	} 
	
	*pu16OutBitLen = obitLen;
	
	return obitLen;
}

// ǰ�����ӣ�0111,�������ӣ�1111

/**	
 *	ISO14443A_PCD_stFrame()
 *	PCD->PICC����
 *
 *	���ر������س��ȡ�
 */
int ISO14443A_stdFraming( BYTE aInBuf[], UINT16 u16InBitLen, BYTE aoutBuf[], UINT16 *pu16OutBitLen )
{
	// ��׼֡һ����CRCУ�飡
	BYTE abyBuf[100];
	int k;

	k = u16InBitLen/8;
	
	memcpy( abyBuf, aInBuf, k );
	// ����CRCУ��
	ISO14443A_CRC( aInBuf, k, &abyBuf[k+1], &abyBuf[k]);
	
	k += 2;
	
	return ISO14443A_Framing( 0, 0, 	// ��׼֡�������ֽڿ�ʼ
						abyBuf, k*8, 		
						aoutBuf, pu16OutBitLen 
					);									
}

// ��֡�̶������γ��ȣ�7��û����żУ���CRCУ��
int ISO14443A_ShortFraming( BYTE byCmd, BYTE aoutBuf[], UINT16 *pu16OutBitLen )
{
	int i;
	int curbit;
	byCmd = byCmd & 0x7F;
	
	aoutBuf[0] = 0x00;
	for( i = 0; i < 7; i++ )
	{
		curbit = ( byCmd >> i ) & 0x01;
		aoutBuf[0] |= curbit << (7-i);
	}
	*pu16OutBitLen = 7;
	
	return 7;
}

int ISO14443A_AnticollisionFraming( 				
				BYTE		byBitLenofFirstByte,						// ��һ���ֽ��е�bit Len���ڷ���ײ֡����Ҫ
				BYTE		byFirstByte,
				BYTE 		ainBuf[], UINT16 u16InBitLen, 
				BYTE 		aoutBuf[], UINT16 *pu16OutBitLen 
		)
{
	// ����ײ֡û��CRC.
	return ISO14443A_Framing( 
					byBitLenofFirstByte, byFirstByte, 	// ��׼֡���������ֽڿ�ʼ
					ainBuf, u16InBitLen, 		
					aoutBuf, pu16OutBitLen
			);	
}

// ��׼֡
// ���룺���س��ȣ�������ֽڳ��ȣ�
int ISO14443A_Un_stdFraming(BYTE ainBuf[], UINT16 u16inBitLen, BYTE aoutBuf[], UINT16 *pu16OutBufLen )
{
	BYTE abyCRC[2];
	BYTE abyBuf[1000];
	int k;
	
	if( ISO14443A_unFraming( 0,0,ainBuf, u16inBitLen, abyBuf, pu16OutBufLen ) < 0 )
	{
		return -1;
	}
	
	k = (*pu16OutBufLen)/8;
	k -= 2;
	*pu16OutBufLen -= 16;

	// ����CRC.
	ISO14443A_CRC( abyBuf, k, &abyCRC[1], &abyCRC[0] );
	if( abyBuf[k] != abyCRC[0] || abyBuf[k+1] != abyCRC[1] )
	{
		return -1;
	}
	memcpy( aoutBuf, abyBuf, k );

	return k;		// �����ֽڳ���
}

// ֻ��7����.
int ISO14443A_Un_ShortFraming( BYTE aInBuf[], UINT16 u16InBitLen, BYTE *pbyCmd )
{
	int i;
	int curbit;
	
	*pbyCmd = 0x00;
	for( i = 0; i < 7; i++ )
	{
		curbit = ( aInBuf[0] >> i ) & 0x01;
		*pbyCmd |= curbit << (7-i);
	}
	
	return 7;
}

int ISO14443A_Un_AnticollisionFraming(
				BYTE		byBitLenofFirstByte,						// ��һ���ֽ��е�bit Len���ڷ���ײ֡����Ҫ
				BYTE		*pbyFirstByte,
				BYTE 		aInBuf[], UINT16 u16inBitLen, 
				BYTE 		aoutBuf[], UINT16 *pu16OutBitLen 
		)		
{
	return ISO14443A_unFraming( byBitLenofFirstByte, pbyFirstByte, 
		aInBuf, u16inBitLen, aoutBuf, pu16OutBitLen );
}



