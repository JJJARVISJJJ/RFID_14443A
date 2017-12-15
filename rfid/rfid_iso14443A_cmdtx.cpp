/**
 *	\file	rfid_iso14443A_cmdtx.c
 *
 *	\brief	ISO14443A������.
 *
 */

/*-------------------------------------------------------------*\
 *	ISO14443A 
 *	1. ��֡��REQA(PCD) & WUPA(PCD)
 *	2. ��׼֡��SELECT(PCD), HALT(PCD), ATQA(PICC), SAK(PICC)
 *	3. ����ײ֡��ANTICOLLISION_req(PCD)/rsp(PICC)
 *-------------------------------------------------------------*/

#include "rfid_iso14443A_cmdtx.h"
#include "memory.h"

/*--------------------------------------------------------------*\
 * ��֡
\*---------------------------------------------------------------*/
/**
 * 	\fun	ISO14443A_REQA_req()
 *	\brief	PCD->PICC
 *
 *	\input	.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_REQA()
{
	/**
	 *	REQA��� 
	 *	7bit����֡��ֵ��0x26.
	 */
	return ISO14443A_ShortFrame_req( ISO_14443_CMD_REQA );
}

/**
 * 	\fun	ISO14443A_WUPA_req()
 *	\brief	PCD->PICC
 *
 *	\input	.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_WUPA()
{
	/**
	 *	WUPA��� 
	 *	7bit����֡��ֵ��0x52.
	 */
	return ISO14443A_ShortFrame_req( ISO_14443_CMD_WUPA );
}
 
// ��֡
// �����롢��֡�����͡�
// ��PCD/PICC�˶���ͬ
int ISO14443A_ShortFrame_req( BYTE byCmd )
{
	BYTE abyTxBuf[100], abyFrmBuf[100];
	UINT16 u16CodedLen,u16FrmLen;
 
	/**
	 *	1. ��֡
	 */	
	if( ISO14443A_ShortFraming( byCmd, abyFrmBuf, &u16FrmLen ) < 0 )
	{
		return -1;
	}
	
	/**
	 *	2. ����
	 *	��֡һ����SOF��EOF
	 */
	if( ISO14443A_PCD_Code( abyFrmBuf, u16FrmLen, abyTxBuf, &u16CodedLen ) < 0 )
	{
		return -1;
	}
	
	return RFID_Reader_Tx( u16CodedLen, abyTxBuf );
}

/*--------------------------------------------------------------*\
 * ��׼֡
 *---------------------------------------------------------------*/

/**
 * 	\fun	ISO14443A_SELECT()
 *	\brief	PCD->PICC
 *
 *	\input	.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_SELECT( BYTE sel, BYTE abyUID[] )
{
	/**
	 *	SELECT��CRC16.
	 */	
	BYTE abyCmd[7];
	int i;
	
	abyCmd[0] = sel;	// SELECT: 0x93,0x95,0x97
	abyCmd[1] = 0x70;	// SELECT
	abyCmd[6] = 0x00;
	for( i = 0; i < 4; i++ )
	{
		abyCmd[i+2] = abyUID[i];
		abyCmd[6] = abyCmd[6] ^ abyUID[i];
	}
	
	return ISO14443A_stdFrame_req( abyCmd, 56 );
}


/**
 * 	\fun	ISO14443A_HALT()
 *	\brief	PCD->PICC
 *
 *	\input	.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_HALT()
{
	BYTE abyCmd[]={0x50, 0x00};
	return ISO14443A_stdFrame_req( abyCmd, 16 );	
}

/**
 * 	\fun	ISO14443A_ATQA()
 *	\brief	PICC->PCD
 *			��REQA/WUKA��Ӧ��
 *			��׼֡.
 *	\input	
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_ATQA( BYTE byCLn )
{
	BYTE abyCmd[2];
	
	byCLn = byCLn & 0x03;		// ���3λ��Ч
	abyCmd[1] = (byCLn << 6) + 0x01;
	abyCmd[0] = 0x00;

	
	return ISO14443A_stdFrame_rsp( abyCmd, 16 );	
}

/**
 * 	\fun	ISO14443A_SAK()
 *	\brief	PICC->PCD
 *			��SELECT/ANTICOLLISION��Ӧ��
 *
 *	\input	cascade: 0/1.
 *	\return	0�ɹ�������ʧ��;
 */
int ISO14443A_SAK( BYTE cascade )
{
	return ISO14443A_stdFrame_rsp( &cascade, 8 );
}


// ��׼֡���������Ƿ���CRC��������У��
// PCDʹ��
int ISO14443A_stdFrame_req( BYTE abyBuf[], UINT16 u16BitLen )
{
	BYTE abyCodeBuf[1024], abyFrmBuf[1024];
	UINT16 u16CodedLen,u16FrmLen;
	
	/**
	 *	1. ��֡
	 */	
	if( ISO14443A_stdFraming( abyBuf, u16BitLen, abyFrmBuf, &u16FrmLen ) < 0 )
	{
		return -1;
	}

	/**
	 *	2. ����
	 *	��׼֡һ����SOF��EOF
	 */		
	if( ISO14443A_PCD_Code( abyFrmBuf, u16FrmLen, abyCodeBuf, &u16CodedLen ) < 0 )
	{
		return -1;
	}
	
	/**
	 *	3. ����
	 */
	return RFID_Reader_Tx( u16CodedLen, abyCodeBuf );
}

// ��׼֡���������Ƿ���CRC��������У��
// PICC��ʹ��
int ISO14443A_stdFrame_rsp( BYTE abyBuf[], UINT16 u16BitLen )
{
	BYTE abyCodeBuf[1024], abyFrmBuf[1024];
	UINT16 u16CodedLen,u16FrmLen;
	
	/**
	 *	1. ��֡
	 */	
	if( ISO14443A_stdFraming( abyBuf, u16BitLen, abyFrmBuf, &u16FrmLen ) < 0 )
	{
		return -1;
	}

	/**
	 *	2. ����
	 *	��׼֡һ����SOF��EOF
	 */		
	if( ISO14443A_PICC_Code( abyFrmBuf, u16FrmLen, abyCodeBuf, &u16CodedLen ) < 0 )
	{
		return -1;
	}
	
	/**
	 *	3. ����
	 */
	return RFID_Card_Tx( u16CodedLen, abyCodeBuf );
}
/*--------------------------------------------------------------*\
 * ����ײ֡
 *---------------------------------------------------------------*/
 
/**
 *	����ײ֡����PCD��PICC�˲�ͬ
 */
int ISO14443A_AnticollisionFrame_req( 
	BYTE bySEL, BYTE abyUID[], UINT16 u16BitLen )
{
	BYTE abyCmd[7];
	BYTE abyCodeBuf[1024], abyFrmBuf[1024];
	UINT16 u16CodedLen,u16FrmLen, u16CmdBitLen;
	int m,n,k,i;

	abyCmd[0] = bySEL;
	
	n = u16BitLen % 8;
	k = u16BitLen/8 + ((n != 0 )? 1:0);
	m = 2 + k;
	abyCmd[1] = ((m & 0x0F) << 4 )+(n & 0x0F);
	
	for( i = 0; i < k; i++ )
	{
		abyCmd[i+2] = abyUID[i];
	}
	u16CmdBitLen = 16 + u16BitLen;
	
	/**
	 *	1. ��֡
	 */	
	if( ISO14443A_AnticollisionFraming( 0, 0, abyCmd, u16CmdBitLen, abyFrmBuf, &u16FrmLen ) < 0 )
	{
		return -1;
	}

	/**
	 *	2. ����
	 *	����ײ֡һ����SOF����û��EOF
	 */		
	if( ISO14443A_PCD_Code( abyFrmBuf, u16FrmLen, abyCodeBuf, &u16CodedLen ) < 0 )
	{
		return -1;
	}
	
	/**
	 *	3. ����
	 */
	return RFID_Reader_Tx( u16CodedLen, abyCodeBuf );
	
	return 0;	 
}

// PICC������һ����RSP.
// BCCҪ���ϲ���������
int ISO14443A_AnticollisionFrame_rsp( 
				BYTE		byBitLenofFirstByte,		// ��һ���ֽ��е�bit Len���ڷ���ײ֡����Ҫ
				BYTE		byFirstByte,
				BYTE 		abyUID[], UINT16 u16UIDBitLen, 
				BYTE		byBCC						
		)
{
	BYTE abyCmd[7];
	BYTE abyCodeBuf[1024], abyFrmBuf[1024];
	UINT16 u16CodedLen,u16FrmLen;
	int k;
	
	/**
	 *	1. ��֡
	 */	
	k = u16UIDBitLen / 8;
	memcpy( abyCmd, abyUID, k );
	abyCmd[k] = byBCC;
	
	if( ISO14443A_AnticollisionFraming( byBitLenofFirstByte, byFirstByte, 
					abyCmd, (k+1)*8, abyFrmBuf, &u16FrmLen ) < 0 )
	{
		return -1;
	}

	/**
	 *	2. ����
	 *	��׼֡һ����SOF��EOF
	 */		
	if( ISO14443A_PICC_Code( abyFrmBuf, u16FrmLen, abyCodeBuf, &u16CodedLen ) < 0 )
	{
		return -1;
	}
	
	/**
	 *	3. ����
	 */
	return RFID_Card_Tx( u16CodedLen, abyCodeBuf );
}
 

