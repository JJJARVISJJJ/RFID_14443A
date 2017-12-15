/**
 *	rfid_iso14443A_codec.c
 *
 *	ISO14443�����
 *	1. ����/ȥ�� SOF/EOF
 *	2. ���Buf: bit7 ���ͳ�.
 *	3. ����˳��bit7,6,...0.
 */

//#include "stdafx.h"
#include "rfid_iso14443A_codec.h"

int ISO14443A_PCD_Code(
			BYTE aInBuf[], UINT16 u16InBitLen, 
			BYTE aoutBuf[], UINT16 *pu16OutBitLen 
		)
{
	return ISO14443A_Code(
			TRUE, 
			aInBuf, u16InBitLen, aoutBuf, pu16OutBitLen 
		);
}
			
int ISO14443A_PCD_Decode( 
			BYTE aInBuf[], UINT16 u16InBitLen, 
			BYTE aOutBuf[], UINT16 *pu16OutBitLen 
		)
{
	return ISO14443A_Decode(
			true,
			aInBuf, u16InBitLen, aOutBuf, pu16OutBitLen 
		);	
}
			
int ISO14443A_PICC_Code( 
			BYTE aInBuf[], UINT16 u16InBitLen, 
			BYTE aoutBuf[], UINT16 *pu16OutBitLen 
		)
{
	return ISO14443A_Code(
			FALSE, 
			aInBuf, u16InBitLen, aoutBuf, pu16OutBitLen 
		);
}

int ISO14443A_PICC_Decode(
			BYTE aInBuf[], UINT16 u16InBitLen, 
			BYTE aOutBuf[], UINT16 *pu16OutBitLen 
		)
{
	return ISO14443A_Decode(
			FALSE, 
			aInBuf, u16InBitLen, aOutBuf, pu16OutBitLen 
		);
}



// ���룺
// ����˳��ÿ���ֽڣ���������أ�����߱���bit7��ʼ
// 
// 		BYTE1 			BYTE2			...BYTEn
// bit: 7,6,5,4,3,2,1,0 7,...			   7,6,5.
//
// �����Ѿ�������У�飬CRC�ȡ� 
// 
// ���ر����ȳ���.
int ISO14443A_Code( 
			BOOL bPCD,							// PCD�����
			BYTE aInBuf[], UINT16 u16InBitLen, 
			BYTE aOutBuf[], UINT16 *pu16OutBitLen 
		)
{
	// ����SOF
	int bitLen, byLen, ibitLen;
	int i, j, k;
	int prebit,curbit;
	BYTE code;
	
	// ����SOF
	bitLen = 0;
	byLen = 0;
	aOutBuf[byLen] = 0x00;

	// SOF	
	{
		if( bPCD )
		{
			aOutBuf[byLen] = ISO14443A_PCD_SOF << 4;
		}
		else
		{
			aOutBuf[byLen] = ISO14443A_PICC_SOF << 4;
		}
		
		bitLen++;
	}
	
	// ����ÿ��bit.
	prebit = 0;			// ��һ������Ϊ0, ֻ��PCD��Ч��
	i = 0;
	ibitLen = 0;
	while( ibitLen < u16InBitLen )
	{
		// ÿ��BYTE�������λ��ʼ����.
		if( (u16InBitLen - ibitLen) >= 8 )
		{
			k = 0;
		}
		else
		{
			k = 8 - u16InBitLen % 8;
		}
		
		// �ȱ�����bit
		for( j = 7; j >=k ; j-- )
		{
			curbit = ( aInBuf[i] >> j ) & 0x01;
			if( bPCD )
			{
				code = RFID_Code_Miller( prebit, curbit ) & 0x0F;
			}
			else
			{
				code = RFID_Code_Manchester( curbit );
			}
			
			if( bitLen % 2 == 0 )
			{
				aOutBuf[byLen] |= code << 4;
			}
			else
			{
				aOutBuf[byLen] |= code;
				byLen++;
				aOutBuf[byLen] = 0x00;
			}
			bitLen++;
			ibitLen++;
			prebit = curbit; 
		}
		i++;
	}
	
	// ����EOF: EOFΪ�߼�0.
	// EOF
	{
		if( bPCD )
		{
			curbit = 0;
			code = RFID_Code_Miller( prebit, curbit ) & 0x0F;
		}
		else
		{
			code = ISO14443A_PICC_EOF;
		}
		
		if( bitLen % 2 == 0 )
		{
			aOutBuf[byLen] |= code << 4;
		}
		else
		{
			aOutBuf[byLen] |= code;
		}
		bitLen++;	
	}
	*pu16OutBitLen = bitLen * ISO14443A_BIT_CODEC_LEN;
	
	return *pu16OutBitLen;
}

// ������PICC�˵���.
// ȥ��SOF/EOF��.
// �ɹ������ؽ�����س��ȣ�-1ʧ�ܡ�
int ISO14443A_Decode( 
			BOOL bPCD,							// PCD�����
			BYTE aInBuf[], UINT16 u16InBitLen, 
			BYTE aOutBuf[], UINT16 *pu16OutBitLen )
{
	int i, k;
	int bitLen, ibitLen;
	BYTE coded;
	int byoffset, btoffset;
	
	if( u16InBitLen % 4 != 0 )
	{
		return -1;
	}
	
	// ��ʼ����
	i = 0;
	
	// SOF;
	ibitLen = 0;
	// SOF
	if( u16InBitLen > 4 )
	{	
		BYTE bySOF;
		bySOF = bPCD ? ISO14443A_PICC_SOF : ISO14443A_PCD_SOF;
		
		coded = ( aInBuf[i] >> 4 ) & 0x0F; 
		if( coded != bySOF )
		{
			return -1;
		}
		ibitLen++;
		u16InBitLen -= ISO14443A_BIT_CODEC_LEN;
	}
	
	bitLen = 0;
	// ��󳤶�-2;
	while( u16InBitLen > ISO14443A_BIT_CODEC_LEN)
	{
		if( ibitLen % 2 == 1 )
		{
			coded = aInBuf[i] & 0x0F;
			i++;
		}
		else
		{
			coded = ( aInBuf[i] >> 4 ) & 0x0F;
		}
		
		// PCD�࣬�������Manchester.
		if( !bPCD )
		{
			k = RFID_Decode_Miller( coded );
		}
		else
		{
			k = RFID_Decode_Manchester( coded );
		}
		
		if( k == -1 )
		{
			return -1;
		}
		byoffset = bitLen / 8;
		btoffset = bitLen % 8;

		if( btoffset == 0 )
		{
			aOutBuf[byoffset] = 0x00;
		}
		aOutBuf[byoffset] |= k << ( 7 - btoffset);
		
		bitLen++;
		ibitLen++;
		u16InBitLen -= ISO14443A_BIT_CODEC_LEN;
	}

	// EOF	
	if( u16InBitLen >= 4 )
	{
		BOOL bEOFResult;
		if( ibitLen % 2 == 1)
		{
			coded = aInBuf[i] & 0x0F;
			i++;
		}
		else
		{
			coded = ( aInBuf[i] >> 4 ) & 0x0F;
		}
		
		if( bPCD )
		{
			// PCD���뷽��
			bEOFResult = ( coded == ISO14443A_PICC_EOF)?TRUE:FALSE;			
		}
		else
		{
			bEOFResult = ( (coded == ISO14443A_PCD_0P0) || ( coded == ISO14443A_PCD_0P1) ) ? TRUE:FALSE;
		}		
		if( !bEOFResult )
		{
			return -1;
		} 	
		u16InBitLen -= ISO14443A_BIT_CODEC_LEN;
	}	
	*pu16OutBitLen = bitLen;
	return bitLen;
}

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
 
int RFID_Code_Miller( BYTE prebit, BYTE bitDat )
{
	bitDat &= 0x01;
	prebit &= 0x01;
	if( bitDat == 0 )
	{
		if( prebit == 0 )
		{
			return RFID_CODE_MILLER_0_p0;			// 0111
		}
		else
		{
			return RFID_CODE_MILLER_0_p1;			// 1111
		}
	}
	else
	{
		return RFID_CODE_MILLER_1;				// 1101
	}
}

/**
 * 	�Ľ���Miller���룺����0����1.����ʧ��
 *
 *	\input	���룺BYTE codedDat: 1101 for 1, 1111/0111 for 0
 *	\ouput
 *	\return	0/1
 *			-1 ,�Ƿ�����
 */
int RFID_Decode_Miller( BYTE codedDat )
{
	codedDat &= 0x0F;
	if( codedDat == RFID_CODE_MILLER_1 )
	{
		return 1;
	}
	else if( codedDat == RFID_CODE_MILLER_0_p1 || codedDat == RFID_CODE_MILLER_0_p0 )
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/*-------------------------------------------------------------*\
 *	Manchester
 *	ISO14443A PICC->PCD & ISO15693 VICC->VCD
 *-------------------------------------------------------------*/

/**
 * 	 Manchester���룬����4���ر���
 *
 *	\input	���룺BYTE bitDat: 0/1����ǰ���������
 *	\ouput
 *	\return	bitDat=1, ���أ�1100
 *			bitDat=0�����أ�0011
 */
int RFID_Code_Manchester( BYTE bitDat )
{
	return bitDat ? RFID_CODE_MANCHESTER_1:RFID_CODE_MANCHESTER_0;
}

/**
 * 	Manchester���룺����0����1.����ʧ��
 *
 *	\input	���룺BYTE codedDat: 1100 for 1, 0011 for 0
 *	\ouput
 *	\return	0/1
 *			-1 ,�Ƿ�����
 */
int RFID_Decode_Manchester( BYTE codedDat )
{
	if( codedDat == RFID_CODE_MANCHESTER_1 )
	{
		return 1;
	}
	else if( codedDat == RFID_CODE_MANCHESTER_0 )
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
