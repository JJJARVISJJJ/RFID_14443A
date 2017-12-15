/**
 *	\file	rfid_iso14443A_pcd.cpp
 *
 *	
 *			����������
 */

//#include "stdafx.h"
#include <stdlib.h>

#include "rfid_def.h"
#include "memory.h"
#include "rfid_iso14443A_cmdtx.h"
#include "rfid_iso14443A_cmdrx.h"

#include "rfid_iso14443A_pcd.h"

void PCD_ISO14443A_SetSELECT( ISO_14443A_PCD *pstPCD, BYTE cmd );
FILE *g_fpDebug = NULL;

int main( int argc, char* argv[] )
{
	return pcd_main( 10 );
}

int pcd_main( int numofpicc )
{
	ISO_14443A_PCD stPCD;
	int i, k;
	
	RFID_COM_Init( ISO_14443_PCD );
	g_fpDebug = fopen("d:\\rfid\\pcddebug.txt", "w");

	k = 0;
	while( k < numofpicc )
	{
		PCD_ISO14443A_run( &stPCD );
		if( stPCD.ucStatus == ISO_14443A_PCD_STATE_OK )
		{
			FILE *fp = fopen("d:\\rfid\\picc.txt", "a+");
			if( !fp )
			{
				fp = fopen( "d:\\rfid\\picc.txt", "w" );
			}
			if( k == 0 )
			{
				fprintf(fp, "\n--------------new test. " );
			}
			printf("\nUID[%d]: ", ++k );
			fprintf(fp, "\nUID[%d]: ", k );
			for( i = 0; i < stPCD.ucCmpBits/8; i++ )
			{
				if( stPCD.aucUID[i] != 0x88 )
				{
					printf("0x%02X.", stPCD.aucUID[i]);
					fprintf(fp, "0x%02X.", stPCD.aucUID[i]);
				}
			}
			fclose( fp );
		}
	}
	fclose( g_fpDebug ); 
	return 1;
}

// һ��ֻʶ���һ�ſ�
int PCD_ISO14443A_run( ISO_14443A_PCD *pstPCD )
{
	UCHAR 	aucRxBuf[256];
	UINT16 	u16BufLen = 256;
	int 	len, r, k;
	
	pstPCD->ucStatus 	= ISO_14443A_PCD_STATE_START;
	pstPCD->ucCmpBits	= 0;
	pstPCD->ucCLn		= 3;			// û���յ�CLn: 0,1,2.
	pstPCD->ucAntiBit	= 2;			// ����ͻ�����У��ȷ���0
	pstPCD->u16WaitTime  = PCD_WAIT_TIME;			// ���ȴ�ʱ�䡣

	memset( pstPCD->aucUID, 0x00, 12 );
	len = 0;
	while( ( pstPCD->ucStatus != ISO_14443A_PCD_STATE_ER ) 
		&& ( pstPCD->ucStatus != ISO_14443A_PCD_STATE_OK ) )
	{
		// �������ݲ�����
		
		// len����Ϊ0.
		r = PCD_ISO14443A_process( pstPCD, aucRxBuf, len );
		if( r < 0 )
		{
			break;
		}
		
		if( pstPCD->ucStatus == ISO_14443A_PCD_STATE_OK )
		{
			// �����ˣ�
			break;
		}

		// �ȴ�
		k = 10;
		while(k--)
		{
			len = RFID_Reader_Rx( u16BufLen, aucRxBuf );
			if( len > 0 )
			{
				break;
			}
		}
	}	
	
	return r;
}

// cmd: sel = 0, anti = 1;
void PCD_ISO14443A_SetSELECT( ISO_14443A_PCD *pstPCD, BYTE cmd )
{
	if( pstPCD->ucCmpBits < 32 )
	{
		pstPCD->ucSEL = ISO_14443_CMD_SEL_CL1;
	}
	else if( pstPCD->ucCmpBits == 32 )
	{
		pstPCD->ucSEL = ( cmd == 0 ) ? ISO_14443_CMD_SEL_CL1:ISO_14443_CMD_SEL_CL2;
	}
	else if( pstPCD->ucCmpBits < 64 )
	{
		pstPCD->ucSEL = ISO_14443_CMD_SEL_CL2;
	} 
	else if( pstPCD->ucCmpBits == 64 )
	{
		pstPCD->ucSEL = ( cmd == 0 ) ? ISO_14443_CMD_SEL_CL2:ISO_14443_CMD_SEL_CL3;
	}
	else
	{
		pstPCD->ucSEL = ISO_14443_CMD_SEL_CL3;
	}
}

// ��ͻģ�⣺
// �����յ��ı�����������ʵ���յ��ı�����
int PCD_ISO14443A_CollisionEmu( ISO_14443A_PCD *pstPCD,
			BYTE byBitLenofFirstByte, BYTE firstByte, 
			UCHAR aucCmdBuf[], UINT16 u16CmdBitLen )
{
	BYTE	bcc;
	int i, j, k, m, n;

	BYTE abyRxUID[4]={0x00,0x00,0x00,0x00};				// 4�����ˣ�
	int  rxUIDbitLen;

	int ibyoffset, ibtoffset;
	int obyoffset, obtoffset;
	int curbit, bitlen;

	// ��ͻģ��: ����PCDֻ�յ��������bits.
	// ʵ���յ���UID����
	bitlen = byBitLenofFirstByte + u16CmdBitLen - 8;		// 
						
	// ������0����0��ʾȫ����������ײ.
	// ���������.
	if( bitlen > 1 )
	{
		rxUIDbitLen = rand() % bitlen;
	}
	else
	{
		rxUIDbitLen = 1;
	}
	// �ѽ��յ���bitlen�����ص����ݣ�����rxUIDbitLen����abyRxUID[]��
	//

	// abyRxUID���ֽ�ƫ�ƺ�bit����
	obyoffset = 0;
	obtoffset = 0;
	// ����FirstByte.
	n = 0;			// ��ʾ�Ѿ������bit��.
	if( byBitLenofFirstByte > 0 )
	{
		// �в���bit.
		if( byBitLenofFirstByte <= rxUIDbitLen )
		{
			// ȫ��FirstByte.
			n = byBitLenofFirstByte;
		}
		else
		{
			// ����FirstByte����߲���Ҫdrop.
			n = rxUIDbitLen;
		}

		// ����n�����أ�firstByte�����n������.
		i = n - 1;
		while( i >= 0 )
		{
			curbit = (firstByte >> i)&0x01;
			abyRxUID[obyoffset] |= curbit << ( 7 - obtoffset );
			i--;
			obtoffset++;
		}
	}

	// ����: ��������ֽ�.
	// 
	if( rxUIDbitLen > byBitLenofFirstByte )
	{
		// ��aucCmdBuf�����һ���ֽ������ƶ�8-k�����ؾͺ�.
		k = 8-((rxUIDbitLen - byBitLenofFirstByte)%8);	//

		// ���һ���ֽ�.
		m = (rxUIDbitLen - byBitLenofFirstByte)/8;	// ���һ���ֽڣ�BCC.
		aucCmdBuf[m] = aucCmdBuf[m] << k;

		j = n;
		i = 0;
		while( j < rxUIDbitLen )
		{						
			ibyoffset = i / 8;
			ibtoffset = i % 8;
			curbit = ( aucCmdBuf[ibyoffset] >> ( 7-ibtoffset)) & 0x01;

			obyoffset = j / 8;
			obtoffset = j % 8;
			abyRxUID[obyoffset] |= curbit << ( 7 - obtoffset );
			i++;
			j++;
		}
		// �������һ���ֽ�.
	}

	// �����Ѿ�������abyRxUID[]�ڣ���rxUIDbitLen�����ء�
	
	// ��һ��������ƴ����UID�ڡ�
	bitlen = rxUIDbitLen;		// bitlen��ʾ����Ҫ����ı�����.
	i = 0;						// ��ǰ�����
	if( bitlen > 0 )
	{
		// �Ȳ��ϴε�ȱ��
		if( pstPCD->ucCmpBits % 8)
		{
			// ��ȱ��: 
			k = byBitLenofFirstByte;		// ��Ҫk�����أ��ճ�һ���ֽ�
			if( k > rxUIDbitLen )
			{
				k = rxUIDbitLen;			// ����������ಹk��.
			}
						 
			// ��k����
			obyoffset = pstPCD->ucCmpBits/8;
			obtoffset = pstPCD->ucCmpBits%8;
			j = obtoffset + k - 1;
			ibyoffset = 0;
			ibtoffset = 0;

			while( j >= obtoffset )
			{
				curbit = ( abyRxUID[ibyoffset] >> ( 7-ibtoffset)) & 0x01;

				pstPCD->aucUID[obyoffset] |= curbit << j;
				ibtoffset++;
				j--;
			}
			bitlen -= k;
			i += k;
			pstPCD->ucCmpBits += k;
		}
	}

	// ��ʼ������k���ֽ�
	if( bitlen > 0 )
	{
		// ����������bit.
		n = ( bitlen / 8 )*8;

		j = pstPCD->ucCmpBits;
		k = n;	// ����k��.
		while( k-- )
		{
			ibyoffset = i / 8;
			ibtoffset = i % 8;
			curbit = ( abyRxUID[ibyoffset] >> ( 7-ibtoffset)) & 0x01;

			obyoffset = j / 8;
			obtoffset = j % 8;
			pstPCD->aucUID[obyoffset] |= curbit << ( 7 - obtoffset );
			i++;
			j++;
		}
		bitlen -= n;
		pstPCD->ucCmpBits += n;
	}

	// ������м���ʣ��bit����ӵ���ͱ���λ��.
	if( bitlen > 0 )
	{
		// UIDһ��λ�����ֽڽ�����.
		obyoffset = pstPCD->ucCmpBits / 8;
		obtoffset = 0;

		pstPCD->ucCmpBits += bitlen;
		i = rxUIDbitLen-1;	// �����ʼ.
		while( bitlen-- )
		{
			ibyoffset = i / 8;
			ibtoffset = i % 8;
			curbit = ( abyRxUID[ibyoffset] >> ( 7-ibtoffset)) & 0x01;

			pstPCD->aucUID[obyoffset] |= curbit << obtoffset;
			obtoffset++;
			i--;
		}
	}


	// �����ȷ���յ���32���أ���鿴BCC�Ƿ���ȷ
	if( (pstPCD->ucCmpBits != 0) && (pstPCD->ucCmpBits % 32 == 0) )
	{
		bcc = 0x00;
		k = pstPCD->ucCmpBits / 8 - 4; 
		for( i = k; i < 4+k; i++ )
		{
			bcc = bcc ^ pstPCD->aucUID[i];
		}
		if( bcc != aucCmdBuf[u16CmdBitLen/8-1] )
		{
			return -1;
		}
	}
	return 1;
}

int PCD_ISO14443A_SendSELECT( ISO_14443A_PCD *pstPCD )
{
	int r;
	int i, byoffset, btoffset, k;

	PCD_ISO14443A_SetSELECT( pstPCD, 0 );
	// ����SELECT.
	r = (pstPCD->ucCmpBits - 32 )/8;
	r = ISO14443A_SELECT( pstPCD->ucSEL, &(pstPCD->aucUID[r]) );
	pstPCD->ucStatus = ISO_14443A_PCD_STATE_SELECT;
	printf("\n\nTx SELECT, waiting SAK");
	fprintf(g_fpDebug, "\n\nTx SELECT, waiting SAK");

	printf("\nUID[0x%02x]:", pstPCD->ucSEL);
	fprintf(g_fpDebug, "\nUID[0x%02x]:", pstPCD->ucSEL);
	k = (pstPCD->ucSEL-0x93)/2;
	for( i = k*32; i < pstPCD->ucCmpBits; i++ )
	{
		byoffset = i/8;
		btoffset = i%8;
		printf("%d", ( pstPCD->aucUID[byoffset] >> btoffset)&0x01);
		fprintf(g_fpDebug, "%d", (pstPCD->aucUID[byoffset] >> btoffset)&0x01);
		if( i%4 == 3 )
		{
			printf(".");
			fprintf(g_fpDebug,".");
		}
	}
	printf("\nwait[0x%02x]", pstPCD->ucSEL);
	fprintf(g_fpDebug, "\nwait[0x%02x]", pstPCD->ucSEL);
	return r;
}

// ����ú�����һ��û���յ����еı��أ�
// newbit = 0/1. ��һ��ȡ0.
int PCD_ISO14443A_SendANTICOLLISION( ISO_14443A_PCD *pstPCD )
{
	int r;
	int k;
	int i, byoffset, btoffset;

	// ����1 bit. ��Ϊ��һ��ȡ0�����ȡ1ʱ��һ����ȡ0û�з��ء�
	// ������ȡ��ʱ��һ���ǿ��Է��صģ���û�з��أ��ͽ����ˡ�

	if( pstPCD->ucAntiBit == 0 )
	{
		pstPCD->ucCmpBits ++;		// ��һ�μ�1������;
	}

	// ��ucCmpBits����Ϊnewbit.
	byoffset = (pstPCD->ucCmpBits-1) / 8;
	btoffset = (pstPCD->ucCmpBits-1) % 8;

	pstPCD->aucUID[byoffset] &= ~(1 << btoffset);		// ����Ϊ0.
	pstPCD->aucUID[byoffset] |= pstPCD->ucAntiBit << btoffset;

	// ��������UID��.
	if( pstPCD->ucCmpBits == 32 || pstPCD->ucCmpBits == 64 || pstPCD->ucCmpBits == 96 )
	{
		// ����һ�����غ󣬳���32bit����ʱ��Ҫ����SELECT.
		r = PCD_ISO14443A_SendSELECT( pstPCD );
	}
	else
	{
		// ������anticollision.
		PCD_ISO14443A_SetSELECT( pstPCD, 1 );
		k = ( pstPCD->ucCmpBits / 32 ) * 4;

		r = ISO14443A_AnticollisionFrame_req( pstPCD->ucSEL, &(pstPCD->aucUID[k]), pstPCD->ucCmpBits % 32 );
		printf("\n\nTx anticollision(new bit=%d), waiting anticollision.", pstPCD->ucAntiBit);
		fprintf( g_fpDebug, "\n\nTx anticollision(new bit=%d), waiting anticollision.", pstPCD->ucAntiBit );

		printf("\nUID[0x%02x]:", pstPCD->ucSEL);
		fprintf(g_fpDebug, "\nUID[0x%02x]: ", pstPCD->ucSEL);
		k = (pstPCD->ucSEL-0x93)/2;
		for( i = k * 32; i < pstPCD->ucCmpBits; i++ )
		{
			byoffset = i/8;
			btoffset = i%8;
			printf("%d", ( pstPCD->aucUID[byoffset] >> btoffset)&0x01);
			fprintf(g_fpDebug, "%d", (pstPCD->aucUID[byoffset] >> btoffset)&0x01);
			if( i % 4 == 3 )
			{
				printf(".");
				fprintf(g_fpDebug, ".");
			}
		}
		printf("\nwait[0x%02x]", pstPCD->ucSEL );
		fprintf(g_fpDebug, "\nwait[0x%02x]", pstPCD->ucSEL);
	}
	return r;
}

// ״̬��
int PCD_ISO14443A_process( ISO_14443A_PCD *pstPCD, UCHAR aucRxBuf[], UINT16 u16RxBufBitLen )
{
	int r = 1;
	
	switch( pstPCD->ucStatus )
	{
		case ISO_14443A_PCD_STATE_START:
			// ����REQA���յ�ATRQ��תΪ
			if( pstPCD->ucCLn == 3 )
			{
				// ����REQA().
				ISO14443A_REQA();
				printf("\n\nTx REQA. waiting ATQA.");
				fprintf(g_fpDebug, "\n\nTx REQA. waiting ATQA.");
				pstPCD->u16WaitTime = PCD_WAIT_TIME;
				pstPCD->ucCLn = 0;
			}
			else
			{
				// ����
				BYTE CL;
				r = ISO14443A_wait_ATQA( aucRxBuf, u16RxBufBitLen, &CL );
				if( r > 0 )
				{
					pstPCD->ucStatus = ISO_14443A_PCD_STATE_ANTI;
					pstPCD->ucCLn 	 = CL;		// 0.1.2
					pstPCD->ucSEL	= ISO_14443_CMD_SEL_CL1;
					pstPCD->ucCmpBits= 0;
					pstPCD->ucAntiBit = 2;			// �տ�ʼ��
					pstPCD->u16WaitTime = PCD_WAIT_TIME;
					
					r = ISO14443A_AnticollisionFrame_req( ISO_14443_CMD_SEL_CL1, NULL, 0 );

					printf("\nRx ATQA.");
					fprintf(g_fpDebug, "\nRx ATQA.");
					printf("\n\nTx SELECT(anticollision), waiting anticollision.\nwait");
					fprintf( g_fpDebug,"\n\nTx SELECT(anticollision), waiting anticollision.\nwait"); 
				}
			}
			break;
		case ISO_14443A_PCD_STATE_ANTI:
			// ִ�з���ͻѭ��. 
			if( u16RxBufBitLen == 0 )
			{
				if( pstPCD->ucAntiBit == 2 || pstPCD->ucAntiBit == 1 )
				{
					if( pstPCD->u16WaitTime )
					{
						pstPCD->u16WaitTime--;
#ifdef _RFID_DEBUG_
						printf("\nwaitTime = %d", pstPCD->u16WaitTime );
#else
						printf(".");
#endif
						return 0;
					}
					else
					{
#ifdef _RFID_DEBUG_
						printf("\nwaitTime = %d. not wait.", pstPCD->u16WaitTime );
#endif
						printf("\nTimeout. not wait." );
						return -1;		// �����ˡ�
					}
				}
				else
				{
					// ˵�����͹�����Ϊ����.
					// �ȴ���
					if( pstPCD->u16WaitTime == 1 )
					{
						// �����ˣ���newbit = 1;
						pstPCD->ucAntiBit = 1;
						pstPCD->u16WaitTime = PCD_WAIT_TIME;
 						return PCD_ISO14443A_SendANTICOLLISION( pstPCD );
					}
					else
					{
						// �ȴ�bit = 0���Ǹ�
						pstPCD->u16WaitTime--;
#ifdef _RFID_DEBUG_
						printf("\nwaitTime = %d", pstPCD->u16WaitTime );
#else
						printf(".");
#endif
						return 0;
					}
				}
			}			
			else
			{
				BYTE firstByte;
				BYTE byBitLenofFirstByte;
				BYTE aucCmdBuf[100];
				UINT16 u16CmdBitLen;
				
				if( pstPCD->ucCmpBits%8 )
				{
					byBitLenofFirstByte = 8 - pstPCD->ucCmpBits%8;
				}
				else
				{
					byBitLenofFirstByte = 0;
				}
				
				r = ISO14443A_wait_anticollisionFrame_rsp( byBitLenofFirstByte, &firstByte,  
						aucRxBuf, u16RxBufBitLen,
						aucCmdBuf, &u16CmdBitLen
						);
				if( r < 0 )
				{
					pstPCD->ucStatus = ISO_14443A_PCD_STATE_ER;
					return r;
				}
				// ��ӡ�յ���UID

				printf("\nRx UID: ");
				fprintf(g_fpDebug, "\nRx UID:");
				if( byBitLenofFirstByte )
				{
					int i, k;
					for( i = 0; i < byBitLenofFirstByte; i++ )
					{
						k = ( firstByte >> i ) & 0x01 ;
						printf("%d", k);
						fprintf(g_fpDebug, "%d", k);
						if( i == byBitLenofFirstByte - 5 )
						{
							printf(".");
							fprintf(g_fpDebug, ".");
						}
					}
					printf(".");
					fprintf(g_fpDebug, ".");
				}
				
				if( u16CmdBitLen-8 > 0 )
				{
					int i,k;
					int btoffset, byoffset;

					for( i = 0; i < u16CmdBitLen-8; i++ )
					{
						byoffset = i/8;
						btoffset = i%8;
						k = ( aucCmdBuf[byoffset] >> btoffset ) & 0x01;
						printf("%d", k);
						fprintf(g_fpDebug, "%d", k);
						if( i % 4 == 3 )
						{
							printf(".");
							fprintf(g_fpDebug, ".");
						}
					}
				}
				
				// ��ͻģ��.
				PCD_ISO14443A_CollisionEmu( pstPCD, byBitLenofFirstByte, firstByte, aucCmdBuf, u16CmdBitLen);

				// �����ǲ����ǳ�ͻ�����.
				// 
				//int k = pstPCD->ucCmpBits / 8;
				//BYTE mask[] = {0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};

				//if( byBitLenofFirstByte != 0 )
				//{
				//	// ǰ���ء�
				//	pstPCD->aucUID[k] &= mask[byBitLenofFirstByte-1];
				//	firstByte		  &= ~mask[byBitLenofFirstByte-1];
				//	pstPCD->aucUID[k] |= firstByte;
				//	k++;				// ��һ���ֽ�.
				//	pstPCD->ucCmpBits += byBitLenofFirstByte;
				//}

				//j = 0;
				//for( i = k; i < 4+k; i++ )
				//{
				//	pstPCD->aucUID[i] = aucCmdBuf[j++];
				//	pstPCD->ucCmpBits += 8;
				//}

				printf("\nRx anticollision.");
				fprintf( g_fpDebug, "\nRx anticollision.");

				if( pstPCD->ucCmpBits == 32 || pstPCD->ucCmpBits == 64 || pstPCD->ucCmpBits == 96 )
				{
					// �����յ������е�UID.
					pstPCD->ucAntiBit = 2;			// ����.
					r = PCD_ISO14443A_SendSELECT(pstPCD);
				}
				else
				{
					pstPCD->ucAntiBit = 0;			// ����.
					r = PCD_ISO14443A_SendANTICOLLISION(pstPCD );
				}
				pstPCD->u16WaitTime = PCD_WAIT_TIME;
			}
			// ����SELECT��תΪACTIVE
			break;
		case ISO_14443A_PCD_STATE_SELECT:
			/**
			 * �����������
			 *	1. ����ͻѭ���У��������յ���32bits��UID����ʱ����SELECT���ٸ�״̬�ȴ���
			 *  2. ����ͻѭ���У����յ��ı���С��31bits��PCD�����ӱ�������Ϊ 0��Ȼ���ʹ�ʱ��
			 *		2.1����û�л�Ӧ��
			 *			2.1.1 ����ʱ�ȴ� ucAntiBit �κ󣬰������ӱ�������Ϊ1�����ͣ�Ȼ��һ�����Եȵ���
			 *			2.1.2 ������û�л�Ӧ������Ϊ������
			 *		2.2�л�Ӧ������
			 */
			if( u16RxBufBitLen == 0 )
			{
				// û�л�Ӧ��
				if( pstPCD->ucAntiBit == 2 || pstPCD->ucAntiBit == 1 )
				{
					// ������SELECT��frame
					if( pstPCD->u16WaitTime )
					{
						pstPCD->u16WaitTime--;
						return 0;
					}
					else
					{
						// �ȹ���
						return -1;
					}
				}
				else
				{
					// ��һ�γ��ԣ������ӱ���Ϊ0.pstPCD->ucAntiBit == 0
					// ������SELECT��frame
					if( pstPCD->u16WaitTime == 1)
					{
						// �����ˣ���newbit = 1;
						pstPCD->ucAntiBit = 1;
						pstPCD->u16WaitTime = PCD_WAIT_TIME;
 						return PCD_ISO14443A_SendANTICOLLISION( pstPCD );
					}
					else if( pstPCD->u16WaitTime )
					{
						pstPCD->u16WaitTime--;
#ifdef _RFID_DEBUG_
						printf("\nwaitTime = %d.", pstPCD->u16WaitTime );
#endif
						return 0;
					}
				}
			}

			// �յ�SAK.
			// �ȴ�����SAK
			r = ISO14443A_wait_SAK( aucRxBuf, u16RxBufBitLen, &pstPCD->ucSAK );
			if( r < 0 )
			{
				return r;
			}
			printf("\nRx SAK[0x%02x].", pstPCD->ucSAK);
			fprintf( g_fpDebug, "\nRx SAK[0x%02x].", pstPCD->ucSAK);
				
				
			// �Ƿ���UID.
			if( ( pstPCD->ucSAK >> 2 ) & 0x01 )
			{
				// ����UID.
				pstPCD->ucStatus = ISO_14443A_PCD_STATE_ANTI;
				
				// ��������ANTI.
				pstPCD->ucSEL = ( pstPCD->ucCmpBits == 32 )?ISO_14443_CMD_SEL_CL2:ISO_14443_CMD_SEL_CL3;
				
				r = ISO14443A_AnticollisionFrame_req( pstPCD->ucSEL, NULL, 0 );
				if( r < 0 )
				{
					return r;
				}
				printf("\n\nTx SELECT(anticollision), waiting anticollision.\nwait");
				fprintf( g_fpDebug, "\n\nTx SELECT(anticollision), waiting anticollision.\nwait");
			}
			else
			{
				// bit3=0;
				// ����������תΪEND
				// ����HALT.
				ISO14443A_HALT();
				pstPCD->ucStatus = ISO_14443A_PCD_STATE_OK;
				printf("\nEnd.");
				fprintf( g_fpDebug, "\nEnd." );
			}
			break;
	}
	return r;
}







