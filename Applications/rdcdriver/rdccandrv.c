#include "rdcdriver.h"



//
// Platform specific CAN Bus device path
//
CAN_DEVICE_PATH gCANDevMemMapPathTemplate = {
	/*{
		HARDWARE_DEVICE_PATH,
		HW_MEMMAP_DP,
		(UINT8)(sizeof(MEMMAP_DEVICE_PATH)),
		(UINT8)((sizeof(MEMMAP_DEVICE_PATH)) >> 8),
		EfiMemoryMappedIO,
		0,
		0
	},*/
	{
		MESSAGING_DEVICE_PATH,
		MSG_DEVICE_LOGICAL_UNIT_DP,
		(UINT8)(sizeof(DEVICE_LOGICAL_UNIT_DEVICE_PATH)),
		(UINT8)((sizeof(DEVICE_LOGICAL_UNIT_DEVICE_PATH)) >> 8),
		0
	},
	{
		HARDWARE_DEVICE_PATH,
		HW_CONTROLLER_DP,
		(UINT8)(sizeof(CONTROLLER_DEVICE_PATH)),
		(UINT8)((sizeof(CONTROLLER_DEVICE_PATH)) >> 8),
		0
	},
	{
		END_DEVICE_PATH_TYPE,
		END_ENTIRE_DEVICE_PATH_SUBTYPE,
		(sizeof(EFI_DEVICE_PATH_PROTOCOL)),
		0
	}
};

EFI_GUID gEfiCanProtocolGuid = EFI_CAN_PROTOCOL_GUID;

UINT8 gCANLDN[CAN_CH_NUM] = { RDC_LDN_CAN0, RDC_LDN_CAN1};



EFI_STATUS EFIAPI RDCCANInit(UINT32 ch, UINT16 *bio)
{
	UINT8 act;

	// RDC : enter config
	IoWrite8(RDC_CFG_IDX_PORT, 0x87);
	IoWrite8(RDC_CFG_IDX_PORT, 0x87);

	// select LDN
	IoWrite8(RDC_CFG_IDX_PORT, 0x07);
	IoWrite8(RDC_CFG_DAT_PORT, gCANLDN[ch]);

	// active
	IoWrite8(RDC_CFG_IDX_PORT, 0x30);
	act = IoRead8(RDC_CFG_DAT_PORT);
	if ((act & 0x1) == 0)
	{
		return EFI_NOT_FOUND;
	}
	
	// base
	IoWrite8(RDC_CFG_IDX_PORT, 0x60);	// MSB of base
	*bio = IoRead8(RDC_CFG_DAT_PORT);
	*bio <<= 8;

	IoWrite8(RDC_CFG_IDX_PORT, 0x61);	// LSB of base
	*bio |= IoRead8(RDC_CFG_DAT_PORT);
	
	// base : address error
	if (*bio == 0x0000 || *bio == 0xFFFF)
	{
		return EFI_NOT_FOUND;
	}

	//Reset CAN Bus
	IoWrite8(*bio + CAN_GLB_CTL, IoRead8(*bio + CAN_GLB_CTL) | CAN_GLB_CTL_RST_bm);

	IoWrite8(RDC_CFG_IDX_PORT, 0xAA);	// RDC: exit config

	return EFI_SUCCESS;
}

EFI_STATUS EFIAPI SetBusBaudrate(IN CONST EFI_CAN_TRANSCEIVER_PROTOCOL *This, IN UINT8 *baudrate)
{
	CANMASTER_PRIVATE *private;
	UINT16 bio;
	UINT16 retry = 3000;

	private = (CANMASTER_PRIVATE*)This;
	bio = private->CANBusIO;
	
	//Set CAN Bus controller Deactivate
	IoWrite32(bio + CAN_GLB_CTL, IoRead32(bio + CAN_GLB_CTL) & ~CAN_GLB_CTL_CBA_bm);

	//wait CAN Bus controller Deactivate
	while(retry)
	{
		if((IoRead32(bio + CAN_GLB_CTL) & CAN_GLB_CTL_CBA_bm) == 0)
		{
			break;
		}

		retry--;
		if(retry == 0)
		{
			return EFI_TIMEOUT;
		}
	}

	switch(*baudrate)
	{
		case 0:		//1000 Kbps
		{
			IoWrite32(bio + CAN_CLK_PRE, 0x00000000);	//CKDIV = 2
			IoWrite32(bio + CAN_BUS_TIM, 0x00000330);
			private->Baudrate = 1000;
			break;
		}
		case 1:		//833.333 Kbps
		{
			IoWrite32(bio + CAN_CLK_PRE, 0x00000000);	//CKDIV = 2
			IoWrite32(bio + CAN_BUS_TIM, 0x00000440);
			private->Baudrate = 833;
			break;
		}
		case 2:		//500 Kbps
		{
			IoWrite32(bio + CAN_CLK_PRE, 0x00000000);	//CKDIV = 2
			IoWrite32(bio + CAN_BUS_TIM, 0x00000772);
			private->Baudrate = 500;
			break;
		}
		case 3:		//250 Kbps
		{
			IoWrite32(bio + CAN_CLK_PRE, 0x00000001);	//CKDIV = 4
			IoWrite32(bio + CAN_BUS_TIM, 0x00000772);
			private->Baudrate = 250;
			break;
		}
		case 4:		//125 Kbps
		{
			IoWrite32(bio + CAN_CLK_PRE, 0x00000003);	//CKDIV = 8
			IoWrite32(bio + CAN_BUS_TIM, 0x00000772);
			private->Baudrate = 125;
			break;
		}
		case 5:		//100 Kbps
		{
			IoWrite32(bio + CAN_CLK_PRE, 0x00000004);	//CKDIV = 10
			IoWrite32(bio + CAN_BUS_TIM, 0x00000772);
			private->Baudrate = 100;
			break;
		}
		case 6:		//62.5 Kbps
		{
			IoWrite32(bio + CAN_CLK_PRE, 0x00000007);	//CKDIV = 16
			IoWrite32(bio + CAN_BUS_TIM, 0x00000772);
			private->Baudrate = 62;
			break;
		}
		case 7:		//50 Kbps
		{
			IoWrite32(bio + CAN_CLK_PRE, 0x00000009);	//CKDIV = 20
			IoWrite32(bio + CAN_BUS_TIM, 0x00000772);
			private->Baudrate = 50;
			break;
		}
		case 8:		//25 Kbps
		{
			IoWrite32(bio + CAN_CLK_PRE, 0x00000013);	//CKDIV = 40
			IoWrite32(bio + CAN_BUS_TIM, 0x00000772);
			private->Baudrate = 25;
			break;
		}
		case 9:		//10 Kbps
		{
			IoWrite32(bio + CAN_CLK_PRE, 0x00000031);	//CKDIV = 100
			IoWrite32(bio + CAN_BUS_TIM, 0x00000772);
			private->Baudrate = 10;
			break;
		}
	}

	return EFI_SUCCESS;
}

void CANBusSetTxFrameControlID(IN CANMASTER_PRIVATE *private)
{
	UINT16 bio;
	UINT32 tmp;
	
	bio = private->CANBusIO;
	
	tmp = (private->Length << 4) | private->Format;
	IoWrite32(bio + CAN_TXD_CTL0, tmp);
	//Print(L"Set TX Frame Control \n");

	IoWrite32(bio + CAN_TXD_IDF0, private->ID);
	//Print(L"Set TX Frame ID \n");
}

void CANBusSetTxData(IN CANMASTER_PRIVATE *private)
{
	UINT16 bio;
	UINT8 index;
	UINT32 tmp;
	
	bio = private->CANBusIO;	
	index = private->Length;
	
	//Set TX Data
	//High
	for(tmp=0; index>=4; index--)
	{
		tmp <<= 8;
		tmp |= *(private->Data + index);
	}
	IoWrite32(bio + CAN_TXD_DAH0, tmp);

	//Low
	for(tmp=0; index>=0; index--)
	{
		tmp <<= 8;
		tmp |= *(private->Data + index);
		if(index == 0)
		{
			break;
		}
	}
	IoWrite32(bio + CAN_TXD_DAL0, tmp);
}

EFI_STATUS CANBusSetGlobalControlActive(IN CANMASTER_PRIVATE *private)
{
	UINT16 bio;
	UINT16 retry = 3000;
	
	bio = private->CANBusIO;	
	
	//Set CAN Bus controller Active
	IoWrite32(bio + CAN_GLB_CTL, IoRead32(bio + CAN_GLB_CTL) | CAN_GLB_CTL_CBA_bm);

	//wait CAN Bus controller Active
	while(retry)
	{
		if((IoRead32(bio + CAN_GLB_CTL) & CAN_GLB_CTL_CBA_bm) != 0)
		{
			break;
		}

		retry--;
		if(retry == 0)
		{
			return EFI_TIMEOUT;
		}
	}
	
	return EFI_SUCCESS;
}

void CANBusGetRxFrame(IN OUT CANMASTER_PRIVATE *private, OUT EFI_CAN_REQUEST_PACKET *packet)
{
	UINT16 bio;
	UINT8 i;
	UINT32 tmp;
	
	bio = private->CANBusIO;	
	
	//get RX Frame Type
	tmp = IoRead32(bio + CAN_RXD_FRM);
	packet->Format = (UINT8)tmp & 0x01;
	packet->Length = ((UINT8)tmp >> 4) & 0x0F;

	//get RX ID
	packet->ID = IoRead32(bio + CAN_RXD_IDF);

	//get RX Data
	tmp = IoRead32(bio + CAN_RXD_DAL);
	for(i=0; i<private->Length; i++)
	{
		if(i==4)
		{
			tmp = IoRead32(bio + CAN_RXD_DAH);
		}
		packet->Data[i] = tmp & 0xFF;
		tmp >>= 8;
	}

	//set Request to release Receive Status
	IoWrite32(bio + CAN_REQ_CTL, IoRead32(bio + CAN_REQ_CTL) | CAN_REQ_CTL_RRB_bm);
}

UINT8 CANBusWaitTxComplete(IN CANMASTER_PRIVATE *private)
{
	UINT8 ret = 0;
	UINT16 retry = 3000;
	UINT32 status;
	UINT16 bio;
	
	bio = private->CANBusIO;

	while(retry)
	{
		gBS->Stall(1);
		status = IoRead32(bio + CAN_INT_STS);
		if(status & (/*CAN_INT_STS_RBOI_bm | */CAN_INT_STS_RBEI_bm | CAN_INT_STS_ALI_bm | CAN_INT_STS_BOI_bm | CAN_INT_STS_EPI_bm | CAN_INT_STS_ECI_bm))	//check error
		{
			/*if(status & CAN_INT_STS_RBOI_bm)
			{
				gST->ConOut->OutputString(gST->ConOut, L"Receive buffer is overrun. \n");
			}*/
			if(status & CAN_INT_STS_RBEI_bm)
			{
				//gST->ConOut->OutputString(gST->ConOut, L"Bus receive error occurs. \n");
			}
			if(status & CAN_INT_STS_ALI_bm)
			{
				//gST->ConOut->OutputString(gST->ConOut, L"The controller loses arbitration. \n");
			}
			if(status & CAN_INT_STS_BOI_bm)
			{
				//gST->ConOut->OutputString(gST->ConOut, L"The controller enters bus off. \n");
			}
			if(status & CAN_INT_STS_EPI_bm)
			{
				//gST->ConOut->OutputString(gST->ConOut, L"The controller is at the error passive state. \n");
			}
			if(status & CAN_INT_STS_ECI_bm)
			{
				//gST->ConOut->OutputString(gST->ConOut, L"The error counter value is greater than or equal to the warning limit value. \n");
			}

			ret = 1;
			goto fail;
		}
		else if(status & CAN_INT_STS_TX0I_bm)	//check completed
		{
			status = IoRead32(bio + CAN_TXD_STS0);
			if((status & (CAN_TXD_STS0_BEO0_bm | CAN_TXD_STS0_TRC0_bm)) == (CAN_TXD_STS0_BEO0_bm | CAN_TXD_STS0_TRC0_bm))	//Bus error occur 0
			{
				switch((status & CAN_TXD_STS0_BEC0_gm) >> 16)	//Bus Error Type
				{
					case 1:
					{
						//gST->ConOut->OutputString(gST->ConOut, L"Bus Error - Bit Error\n");
						break;
					}
					case 2:
					{
						//gST->ConOut->OutputString(gST->ConOut, L"Bus Error - Stuff Error\n");
						break;
					}
					case 3:
					{
						//gST->ConOut->OutputString(gST->ConOut, L"Bus Error - CRC Error\n");
						break;
					}
					case 4:
					{
						//gST->ConOut->OutputString(gST->ConOut, L"Bus Error - Form Error\n");
						break;
					}
					case 5:
					{
						//gST->ConOut->OutputString(gST->ConOut, L"Bus Error - Acknowledge Error\n");
						break;
					}
				}
				
				//status = IoRead32(bio + CAN_GLB_STS);
				//Print(L"CtrlSts = %x \n", status);

				status = IoRead32(bio + CAN_INT_STS);
				if(status & (1 << 16))
				{
					//gST->ConOut->OutputString(gST->ConOut, L"Bus error occurs at receiving.\n");
				}
				else
				{
					//gST->ConOut->OutputString(gST->ConOut, L"Bus error occurs at transmitting.\n");
				}
				
				ret = 2;
				goto fail;
			}
			else if((status & (CAN_TXD_STS0_ALO0_bm | CAN_TXD_STS0_TRC0_bm)) == (CAN_TXD_STS0_ALO0_bm | CAN_TXD_STS0_TRC0_bm))	//Arbitration Lost
			{
				//gST->ConOut->OutputString(gST->ConOut, L"Arbitration Lost !! \n");

				ret = 3;
				goto fail;
			}
			else if((status & (CAN_TXD_STS0_TC0_bm | CAN_TXD_STS0_TRC0_bm)) == (CAN_TXD_STS0_TC0_bm | CAN_TXD_STS0_TRC0_bm))	//Transmission completed
			{
				ret = 0;
				goto fail;
			}
		}

		retry--;
		if(retry == 0)
		{
			//gST->ConOut->OutputString(gST->ConOut, L"CAN Bus Tx Timeout !! \n");
			//Print(L"IntSts=%x\n", IoRead32(bio + CAN_INT_STS));
			//Print(L"TxSts0=%x\n", IoRead32(bio + CAN_TXD_STS0));
			
			ret = 4;
			goto fail;
		}
	}
	
fail:
	//clean all Interrupt Status
	IoWrite32(bio + CAN_INT_STS, 0x7FF);
	
	return ret;
}

UINT8 CANBusWaitRxComplete(IN OUT CANMASTER_PRIVATE *private)
{
	UINT8 ret = 0;
	UINT16 retry = 3000;
	UINT32 status;
	UINT16 bio;
	
	bio = private->CANBusIO;
	
	while(retry)
	{
		gBS->Stall(1);
		status = IoRead32(bio + CAN_INT_STS);
		if(status & (/*CAN_INT_STS_RBOI_bm | */CAN_INT_STS_RBEI_bm | CAN_INT_STS_ALI_bm | CAN_INT_STS_BOI_bm | CAN_INT_STS_EPI_bm | CAN_INT_STS_ECI_bm))	//check error
		{
			/*if(status & CAN_INT_STS_RBOI_bm)
			{
				gST->ConOut->OutputString(gST->ConOut, L"Receive buffer is overrun. \n");
			}*/
			if(status & CAN_INT_STS_RBEI_bm)
			{
				//gST->ConOut->OutputString(gST->ConOut, L"Bus receive error occurs. \n");
			}
			if(status & CAN_INT_STS_ALI_bm)
			{
				//gST->ConOut->OutputString(gST->ConOut, L"The controller loses arbitration. \n");
			}
			if(status & CAN_INT_STS_BOI_bm)
			{
				//gST->ConOut->OutputString(gST->ConOut, L"The controller enters bus off. \n");
			}
			if(status & CAN_INT_STS_EPI_bm)
			{
				//gST->ConOut->OutputString(gST->ConOut, L"The controller is at the error passive state. \n");
			}
			if(status & CAN_INT_STS_ECI_bm)
			{
				//gST->ConOut->OutputString(gST->ConOut, L"The error counter value is greater than or equal to the warning limit value. \n");
			}

			ret = 1;
			goto fail;
		}
		else if(status & CAN_INT_STS_RXI_bm)	//check receive
		{
			status = IoRead32(bio + CAN_RCV_STS);
			if(status & CAN_RCV_STS_BEOR_bm)	//Bus error occur
			{
				switch((status & CAN_RCV_STS_RBEC_gm) >> 4)	//Bus Error Type
				{
					case 1:
					{
						//gST->ConOut->OutputString(gST->ConOut, L"Bus Error - Bit Error\n");
						break;
					}
					case 2:
					{
						//gST->ConOut->OutputString(gST->ConOut, L"Bus Error - Stuff Error\n");
						break;
					}
					case 3:
					{
						//gST->ConOut->OutputString(gST->ConOut, L"Bus Error - CRC Error\n");
						break;
					}
					case 4:
					{
						//gST->ConOut->OutputString(gST->ConOut, L"Bus Error - Form Error\n");
						break;
					}
					case 5:
					{
						//gST->ConOut->OutputString(gST->ConOut, L"Bus Error - Acknowledge Error\n");
						break;
					}
				}
				
				status = IoRead32(bio + CAN_INT_STS);
				if(status & (1 << 16))
				{
					//gST->ConOut->OutputString(gST->ConOut, L"Bus error occurs at receiving.\n");
				}
				else
				{
					//gST->ConOut->OutputString(gST->ConOut, L"Bus error occurs at transmitting.\n");
				}
				
				ret = 2;
				goto fail;
			}
			else if(status & CAN_RCV_STS_RBO_bm)	//Receive Buffer Overrun
			{
				/*if(p)
				{
					Print(L"Receive Buffer Overrun !! (%x)\n", status);
				}*/
				ret = 0;
				goto fail;
			}
			else if(status & CAN_RCV_STS_RBS_bm)	//there are pending messages in the receive buffer
			{
				ret = 0;
				goto fail;
			}
		}

		retry--;
		if(retry == 0)
		{
			/*if(p)
			{
				gST->ConOut->OutputString(gST->ConOut, L"CAN Bus Rx Timeout !! \n");
				Print(L"IntSts=%x\n", IoRead32(bio + CAN_INT_STS));
				Print(L"RxSts0=%x\n", IoRead32(bio + CAN_RCV_STS));
			}*/
			ret = 4;
			goto fail;
		}
	}

fail:
	//clean all Interrupt Status
	IoWrite32(bio + CAN_INT_STS, 0x7FF);
	
	return ret;
}

EFI_STATUS CANTransceiver(IN OUT CANMASTER_PRIVATE *private, OUT EFI_CAN_REQUEST_PACKET *packet)
{
	UINT16 bio;
	
	bio = private->CANBusIO;
	
	//clean all Interrupt Status
	IoWrite32(bio + CAN_INT_STS, 0x7FF);
	
	//TX
	if(private->Flag & CAN_TX)
	{
		//set CAN bus mode id length
		CANBusSetTxFrameControlID(private);
		
		//set CAN bus active
		CANBusSetGlobalControlActive(private);
		
		//set data
		CANBusSetTxData(private);
		
		//set tx request
		IoWrite32(bio + CAN_REQ_CTL, IoRead32(bio + CAN_REQ_CTL) | CAN_REQ_CTL_TBR0_bm);
		
		//wait tx complete
		if(CANBusWaitTxComplete(private))
		{
			goto fail;
		}
	}
	
	//RX
	if(private->Flag & CAN_RX)
	{
		//set CAN bus active
		CANBusSetGlobalControlActive(private);
		
		//wait rx complete
		if(CANBusWaitRxComplete(private))
		{
			goto fail;
		}
		
		//get rx data
		CANBusGetRxFrame(private, packet);
	}
	
	return EFI_SUCCESS;
	
fail:
	return EFI_TIMEOUT;
}

EFI_STATUS EFIAPI CANExecute(IN CONST EFI_CAN_TRANSCEIVER_PROTOCOL *This, IN OUT EFI_CAN_REQUEST_PACKET *Packet)
{
	CANMASTER_PRIVATE *private;
	
	private = (CANMASTER_PRIVATE*)This;
	
	private->Flag = Packet->Flag;
	private->Format = Packet->Format;
	private->ID = Packet->ID;
	private->Length = Packet->Length;
	private->Data = Packet->Data;
	
	return CANTransceiver(private, Packet);
}

EFI_STATUS EFIAPI RDCCanMasterDxeInstall(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable)
{
	EFI_STATUS status = EFI_SUCCESS;
	EFI_HANDLE handle;
	CAN_MASTER_PRIVATE *canPrivate;
	UINT8 index;
	CAN_DEVICE_PATH *canDevPath;
	
	UINT16 bio = 0xFFFF;
	
	status = gBS->AllocatePool(EfiBootServicesData, sizeof(CAN_MASTER_PRIVATE), &canPrivate);
	if(EFI_ERROR (status))
	{
		return status;
	}
	else
	{
		//clear instances content
		gBS->SetMem(canPrivate, sizeof(CAN_MASTER_PRIVATE), 0);
	}
	
	handle = ImageHandle;
	
	for(index=0; index<CAN_CH_NUM; index++)
	{
		status = RDCCANInit(index, &bio);
		if(EFI_ERROR (status))
		{
			continue;
		}
		
		//canPrivate->CANMaster[index].Handle = Handle;
		canPrivate->CANMaster[index].CANBusIO = bio;
		canPrivate->CANMaster[index].CANBusId = index;
		canPrivate->CANMaster[index].CANController.CANExecute = CANExecute;
		canPrivate->CANMaster[index].CANController.SetBusBaudrate = SetBusBaudrate;
		
		status = gBS->AllocatePool(EfiBootServicesData, sizeof(CAN_DEVICE_PATH), &canDevPath);
		if(EFI_ERROR (status))
		{
			canDevPath = &gCANDevMemMapPathTemplate;
		}
		else
		{
			// Update instances content.
			gBS->CopyMem(canDevPath, &gCANDevMemMapPathTemplate, sizeof(CAN_DEVICE_PATH));

			canDevPath->CanBus.ControllerNumber = canPrivate->CANMaster[index].CANBusId;
			canDevPath->RDCCanLDN.Lun = gCANLDN[index];
			//canDevPath->CanMemMapDevPath.StartingAddress = bio;
		}
		canPrivate->CANMaster[index].DevPath = (EFI_DEVICE_PATH_PROTOCOL *)canDevPath;
		
		// Install Protocol
		status = gBS->InstallMultipleProtocolInterfaces(&canPrivate->CANMaster[index].Handle, &gEfiCanProtocolGuid, &canPrivate->CANMaster[index], &gEfiDevicePathProtocolGuid, canPrivate->CANMaster[index].DevPath, NULL);
	}
	
	return status;
}

