/**
 *	\file	rfid_iso14443A_cmdrx.c
 *
 *	\brief	ISO14443A������.
 *
 *			�ֱ���PCD/PICC���շ����õ��������ĸ�����
 *			
 *			��ͬ״̬�£�Ҫ�ȴ������ͬ�����Կ��ǵȴ��ĸ���������������������
 */
//#include "stdafx.h"
#include "rfid_iso14443A_codec.h"
#include "rfid_iso14443A_frm.h"

#include "rfid_iso14443A_cmdrx.h"
/**
 *
 *	 PICC	�ȴ�REQA
 */
int ISO14443A_wait_ShortFrame( BYTE abyRxBuf[], UINT16 u16BufBitLen, BYTE *pbyCmd )
{
	BYTE 	abyFrmBuf[100];
	UINT16 	u16FrmBitLen;

	// REQA�ı��볤��ȷ��Ϊ: SOF(4)+CMD(28)+EOF(4) = 36
	if( u16BufBitLen != 36 )
	{
		return -1;
	}  
	
	// �����֡
	if( ISO14443A_PICC_Decode( abyRxBuf, u16BufBitLen, abyFrmBuf, &u16FrmBitLen) < 0)
	{
		return -1;
	}	
	
	// ��֡
	if( ISO14443A_Un_ShortFraming( abyFrmBuf, u16FrmBitLen, pbyCmd ) < 0 )
	{
		return -1;
	}
	return 1;
}
	
int ISO14443A_wait_REQA( BYTE abyRxBuf[], UINT16 u16BufBitLen )
{	// �������
	BYTE byCmd;
	
	if( ISO14443A_wait_ShortFrame( abyRxBuf, u16BufBitLen, &byCmd ) < 0 )
	{
		return -1;
	}
	
	return byCmd == ISO_14443_CMD_REQA ? 1:-1;
}


int ISO14443A_wait_WUPA( BYTE abyRxBuf[], UINT16 u16BufBitLen )
{	// �������
	BYTE byCmd;
	
	if( ISO14443A_wait_ShortFrame( abyRxBuf, u16BufBitLen, &byCmd ) < 0 )
	{
		return -1;
	}
	
	return byCmd == ISO_14443_CMD_WUPA ? 1:-1;
}

// PCD/PICC������յ���׼֡
// 
int ISO14443A_wait_stdFrame( BOOL bPCD, UINT16 u16HopeCmdLen, 
		BYTE abyRxBuf[], UINT16 u16BufBitLen, 
		BYTE abyCmdBuf[], UINT16 *pu16CmdBitLen )
{
	BYTE 	abyFrmBuf[100];
	UINT16 	u16FrmBitLen;
	int 	r;
	int 	k;
	
	// ��׼֡�ĳ��ȣ�(( cmdlen + 2(crc)) * 9(parity) + sof(1) + eof(1)) * 4
	k = (( u16HopeCmdLen + 2 ) * 9 + 2 ) * ISO14443A_BIT_CODEC_LEN; 
	
	// REQA�ı��볤��ȷ��Ϊ: SOF(4)+CMD(28)+EOF(4) = 36
	if( u16BufBitLen != k )
	{
		return -1;
	}  
	
	// ����
	if( bPCD )
	{
		r = ISO14443A_PCD_Decode( abyRxBuf, u16BufBitLen, abyFrmBuf, &u16FrmBitLen ); 
	}
	else
	{
		r = ISO14443A_PICC_Decode( abyRxBuf, u16BufBitLen, abyFrmBuf, &u16FrmBitLen ); 
	}
	
	
	if( r < 0)
	{
		return -1;
	}	
	
	// ��֡
	if( ISO14443A_Un_stdFraming( abyFrmBuf, u16FrmBitLen, abyCmdBuf, pu16CmdBitLen ) < 0 )
	{
		return -1;
	}
	
	return *pu16CmdBitLen;
}

// ���ص���Զ��4�ֽڳ��ȵ�UID.
int ISO14443A_wait_SELECT( BYTE abyRxBuf[], UINT16 u16BufBitLen, BYTE *psel, BYTE abyUID[] )
{
	BYTE 	abyCmdBuf[100], byBCC;
	UINT16 	u16CmdBitLen;
	int		i;
	
	if( ISO14443A_wait_stdFrame( false, 7, abyRxBuf, u16BufBitLen, abyCmdBuf, &u16CmdBitLen ) < 0 )
	{
		return -1;
	}

	// ����ɹ�.
	if( abyCmdBuf[0] != ISO_14443_CMD_SEL_CL1 
			&& abyCmdBuf[0] != ISO_14443_CMD_SEL_CL2 
			&& abyCmdBuf[0] != ISO_14443_CMD_SEL_CL3
			)
	{
		return -1;
	}
	
	// NVB
	if( abyCmdBuf[1] != 0x70 )
	{
		return -1;
	}
	
	// ����BCC.
	byBCC = 0x00;
	for( i = 2; i < 6; i++ )
	{
		byBCC = byBCC ^ abyCmdBuf[i];
	}	
	if( byBCC != abyCmdBuf[6] )
	{
		return -1;
	}
	
	*psel = abyCmdBuf[0];
	memcpy( abyUID, &abyCmdBuf[2], 4 );
	
	return 1; 
}

int ISO14443A_wait_HALT( BYTE abyRxBuf[], UINT16 u16BufBitLen )
{
	BYTE 	abyCmdBuf[100];
	UINT16 	u16CmdBitLen;
	
	if( ISO14443A_wait_stdFrame( false, 2, abyRxBuf, u16BufBitLen, abyCmdBuf, &u16CmdBitLen ) < 0 )
	{
		return -1;
	}
	
	if( abyCmdBuf[0] != 0x50 || abyCmdBuf[1] != 0x00 )
	{
		return -1;
	}	
	
	return 1;
}

int ISO14443A_wait_SAK( BYTE abyRxBuf[], UINT16 u16BufBitLen, BYTE *pByCmd )
{
	BYTE 	abyCmdBuf[100];
	UINT16 	u16CmdBitLen;
	
	if( ISO14443A_wait_stdFrame( true, 1, abyRxBuf, u16BufBitLen, abyCmdBuf, &u16CmdBitLen ) < 0 )
	{
		return -1;
	}
	
	*pByCmd = abyCmdBuf[0];
	
	return 1;
}

int ISO14443A_wait_ATQA( BYTE abyRxBuf[], UINT16 u16BufBitLen, BYTE *pByCLn )
{
	BYTE 	abyCmdBuf[100];
	UINT16 	u16CmdBitLen;
	
	if( ISO14443A_wait_stdFrame( true, 2, abyRxBuf, u16BufBitLen, abyCmdBuf, &u16CmdBitLen ) < 0 )
	{
		return -1;
	}
	
	*pByCLn = ( abyCmdBuf[1] >> 6 ) & 0x03;
	
	return 1;
}

// ����ײ֡.
int ISO14443A_wait_anticollisionFrame( 
		BOOL bPCD,
		BYTE byFirstByteLen, BYTE *pFirstByte, 
		BYTE abyRxBuf[], UINT16 u16BufBitLen, 
		BYTE abyCmdBuf[], UINT16 *pu16CmdBitLen )
{
	BYTE 	abyFrmBuf[100];
	UINT16 	u16FrmBitLen;
	int 	r;
	
	// ����
	if( bPCD )
	{
		r = ISO14443A_PCD_Decode( abyRxBuf, u16BufBitLen, abyFrmBuf, &u16FrmBitLen ); 
	}
	else
	{
		r = ISO14443A_PICC_Decode( abyRxBuf, u16BufBitLen, abyFrmBuf, &u16FrmBitLen ); 
	}
	
	
	if( r < 0)
	{
		return -1;
	}	
	
	// ��֡
	if( ISO14443A_Un_AnticollisionFraming( byFirstByteLen, pFirstByte, 
				abyFrmBuf, u16FrmBitLen, abyCmdBuf, pu16CmdBitLen ) < 0 )
	{
		return -1;
	}
	
	return *pu16CmdBitLen;
}

// req:  PICC�˵ȴ�
int ISO14443A_wait_anticollisionFrame_req( 
		BYTE abyRxBuf[], UINT16 u16BufBitLen, 
		BYTE abyCmdBuf[], UINT16 *pu16CmdBitLen )
{
	return ISO14443A_wait_anticollisionFrame( false, 0, NULL, 
			abyRxBuf, u16BufBitLen,
			abyCmdBuf, pu16CmdBitLen );
}

// rsp: PCD�ȴ�
int ISO14443A_wait_anticollisionFrame_rsp( 
		BYTE byFirstByteLen, BYTE *pFirstByte, 
		BYTE abyRxBuf[], UINT16 u16BufBitLen, 
		BYTE abyCmdBuf[], UINT16 *pu16CmdBitLen )
{
	return ISO14443A_wait_anticollisionFrame( true, byFirstByteLen, pFirstByte, 
			abyRxBuf, u16BufBitLen,
			abyCmdBuf, pu16CmdBitLen );
}


