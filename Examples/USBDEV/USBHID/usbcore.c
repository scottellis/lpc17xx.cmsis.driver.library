/*----------------------------------------------------------------------------
 *      U S B  -  K e r n e l
 *----------------------------------------------------------------------------
 *      Name:    USBCORE.C
 *      Purpose: USB Core Module
 *      Version: V1.10
 *----------------------------------------------------------------------------
 *      This software is supplied "AS IS" without any warranties, express,
 *      implied or statutory, including but not limited to the implied
 *      warranties of fitness for purpose, satisfactory quality and
 *      noninfringement. Keil extends you a royalty-free right to reproduce
 *      and distribute executable files created using this software for use
 *      on NXP Semiconductors LPC family microcontroller devices only. Nothing
 *      else gives you the right to use this software.
 *
 *      Copyright (c) 2005-2009 Keil Software.
 *---------------------------------------------------------------------------*/


#include "lpc_types.h"

#include "usb.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "usbdesc.h"
#include "usbuser.h"

#if (USB_CLASS)

#if (USB_AUDIO)
#include "audio.h"
#include "adcuser.h"
#endif

#if (USB_HID)
#include "hid.h"
#include "hiduser.h"
#endif

#if (USB_MSC)
#include "msc.h"
#include "mscuser.h"
extern MSC_CSW CSW;
#endif

#if (USB_CDC)
#include "cdc.h"
#include "cdcuser.h"
#endif

#endif

#if (USB_VENDOR)
#include "vendor.h"
#endif

#ifndef __IAR_SYSTEMS_ICC__
#pragma diag_suppress 111,1441
#endif

uint16_t  USB_DeviceStatus;
uint8_t  USB_DeviceAddress;
uint8_t  USB_Configuration;
uint32_t USB_EndPointMask;
uint32_t USB_EndPointHalt;
uint8_t  USB_NumInterfaces;
uint8_t  USB_AltSetting[USB_IF_NUM];

uint8_t  EP0Buf[USB_MAX_PACKET0];


USB_EP_DATA EP0Data;

USB_SETUP_PACKET SetupPacket;


/*
 *  Reset USB Core
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_ResetCore (void) {

  USB_DeviceStatus  = USB_POWER;
  USB_DeviceAddress = 0;
  USB_Configuration = 0;
  USB_EndPointMask  = 0x00010001;
  USB_EndPointHalt  = 0x00000000;
}


/*
 *  USB Request - Setup Stage
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    None
 */

void USB_SetupStage (void) {
  USB_ReadEP(0x00, (uint8_t *)&SetupPacket);
}


/*
 *  USB Request - Data In Stage
 *    Parameters:      None (global EP0Data)
 *    Return Value:    None
 */

void USB_DataInStage (void) {
  uint32_t cnt;

  if (EP0Data.Count > USB_MAX_PACKET0) {
    cnt = USB_MAX_PACKET0;
  } else {
    cnt = EP0Data.Count;
  }
  cnt = USB_WriteEP(0x80, EP0Data.pData, cnt);
  EP0Data.pData += cnt;
  EP0Data.Count -= cnt;
}


/*
 *  USB Request - Data Out Stage
 *    Parameters:      None (global EP0Data)
 *    Return Value:    None
 */

void USB_DataOutStage (void) {
  uint32_t cnt;

  cnt = USB_ReadEP(0x00, EP0Data.pData);
  EP0Data.pData += cnt;
  EP0Data.Count -= cnt;
}


/*
 *  USB Request - Status In Stage
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_StatusInStage (void) {
  USB_WriteEP(0x80, NULL, 0);
}


/*
 *  USB Request - Status Out Stage
 *    Parameters:      None
 *    Return Value:    None
 */

void USB_StatusOutStage (void) {
  USB_ReadEP(0x00, EP0Buf);
}


/*
 *  Get Descriptor USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

#ifdef __IAR_SYSTEMS_ICC__
inline uint32_t USB_GetDescriptor (void) {
#else
__inline uint32_t USB_GetDescriptor (void) {
#endif
	uint8_t  *pD;
	uint32_t len, n;

	switch (SetupPacket.bmRequestType.BM.Recipient)
	{
		case REQUEST_TO_DEVICE:
			switch (SetupPacket.wValue.WB.H)
			{
				case USB_DEVICE_DESCRIPTOR_TYPE:
					EP0Data.pData = (uint8_t *)USB_DeviceDescriptor;
					len = USB_DEVICE_DESC_SIZE;
					break;
				case USB_CONFIGURATION_DESCRIPTOR_TYPE:
					pD = (uint8_t *)USB_ConfigDescriptor;
					for (n = 0; n != SetupPacket.wValue.WB.L; n++)
					{
						if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bLength != 0)
						{
							pD += ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
						}
					}
					if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bLength == 0)
					{
						return (FALSE);
					}
					EP0Data.pData = pD;
					len = ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
					break;
				case USB_STRING_DESCRIPTOR_TYPE:
					EP0Data.pData = (uint8_t *)USB_StringDescriptor + SetupPacket.wValue.WB.L;
					len = ((USB_STRING_DESCRIPTOR *)EP0Data.pData)->bLength;
					break;
				default:
					return (FALSE);
			}
			break;
		case REQUEST_TO_INTERFACE:
			switch (SetupPacket.wValue.WB.H)
			{
#if USB_HID
				case HID_HID_DESCRIPTOR_TYPE:
					if (SetupPacket.wIndex.WB.L != USB_HID_IF_NUM)
					{
						return (FALSE);    /* Only Single HID Interface is supported */
					}
					EP0Data.pData = (uint8_t *)USB_ConfigDescriptor + HID_DESC_OFFSET;
					len = HID_DESC_SIZE;
					break;
				case HID_REPORT_DESCRIPTOR_TYPE:
					if (SetupPacket.wIndex.WB.L != USB_HID_IF_NUM)
					{
						return (FALSE);    /* Only Single HID Interface is supported */
					}
					EP0Data.pData = (uint8_t *)HID_ReportDescriptor;
					len = HID_ReportDescSize;
					break;
				case HID_PHYSICAL_DESCRIPTOR_TYPE:
					return (FALSE);      /* HID Physical Descriptor is not supported */
#endif
				default:
					return (FALSE);
			}
			break;
		default:
			return (FALSE);
	}

	if (EP0Data.Count > len)
	{
		EP0Data.Count = len;
	}
	USB_DataInStage();

	return (TRUE);
}


/*
 *  Set Configuration USB Request
 *    Parameters:      None (global SetupPacket)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

#ifdef __IAR_SYSTEMS_ICC__
inline uint32_t USB_SetConfiguration (void) {
#else
__inline uint32_t USB_SetConfiguration (void) {
#endif
	USB_COMMON_DESCRIPTOR *pD;
	uint32_t  alt, n;
	uint32_t tmp;

	if (SetupPacket.wValue.WB.L)
	{
		pD = (USB_COMMON_DESCRIPTOR *)USB_ConfigDescriptor;
		while (pD->bLength)
		{
			switch (pD->bDescriptorType)
			{
				case USB_CONFIGURATION_DESCRIPTOR_TYPE:
					if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bConfigurationValue == SetupPacket.wValue.WB.L)
					{
						USB_Configuration = SetupPacket.wValue.WB.L;
						USB_Configure(TRUE);
					/*	if (((USB_CONFIGURATION_DESCRIPTOR *)pD)->bmAttributes & USB_CONFIG_SELF_POWERED)
						{
							USB_DeviceStatus |=  USB_GETSTATUS_SELF_POWERED;
						}
						else
						{
							USB_DeviceStatus &= ~USB_GETSTATUS_SELF_POWERED;
						}	 */
					}
					else
					{
//						(uint8_t *)pD += ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
						tmp = (uint32_t)pD;
						tmp += ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
						pD = (USB_COMMON_DESCRIPTOR *)tmp;
						continue;
					}
					break;
				case USB_INTERFACE_DESCRIPTOR_TYPE:
					alt = ((USB_INTERFACE_DESCRIPTOR *)pD)->bAlternateSetting;
					break;
				case USB_ENDPOINT_DESCRIPTOR_TYPE:
					if (alt == 0)
					{
						n = ((USB_ENDPOINT_DESCRIPTOR *)pD)->bEndpointAddress & 0x8F;
						USB_ConfigEP((USB_ENDPOINT_DESCRIPTOR *)pD);
						USB_EnableEP(n);
						USB_ResetEP(n);
					}
					break;
			}
//			(uint8_t *)pD += pD->bLength;
			tmp = (uint32_t)pD;
			tmp += pD->bLength;
			pD = (USB_COMMON_DESCRIPTOR *)tmp;
		}
	}
	else
	{
		USB_Configuration = 0;
		USB_Configure(FALSE);
	}

	if (USB_Configuration == SetupPacket.wValue.WB.L)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}

/*
 *  USB Endpoint 0 Event Callback
 *    Parameter:       event
 */

void USB_EndPoint0 (uint32_t event)
{
	switch (event)
	{
		case USB_EVT_SETUP:
			USB_SetupStage();
			EP0Data.Count = SetupPacket.wLength;
			switch (SetupPacket.bmRequestType.BM.Type)
			{
				case REQUEST_STANDARD:
					switch (SetupPacket.bRequest)
					{
						case USB_REQUEST_SET_ADDRESS:
							switch (SetupPacket.bmRequestType.BM.Recipient)
							{
								case REQUEST_TO_DEVICE:
									USB_DeviceAddress = SetupPacket.wValue.WB.L;//0x80 | SetupPacket.wValue.WB.L;
									USB_StatusInStage();
									USB_SetAddress(USB_DeviceAddress);
									break;
								default:
									break;
              				}
              				break;

						case USB_REQUEST_GET_DESCRIPTOR:
							USB_GetDescriptor();
							break;

					/*	case USB_REQUEST_GET_CONFIGURATION:
							switch (SetupPacket.bmRequestType.BM.Recipient)
							{
								case REQUEST_TO_DEVICE:
									EP0Data.pData = &USB_Configuration;
									USB_DataInStage();
									break;
								default:
									break;
							}
							break;	*/

						case USB_REQUEST_SET_CONFIGURATION:
							switch (SetupPacket.bmRequestType.BM.Recipient)
							{
								case REQUEST_TO_DEVICE:
									if (!USB_SetConfiguration())
									{
										USB_SetStallEP(0x80);
										EP0Data.Count = 0;
										break;
									}
									USB_StatusInStage();
#if USB_CONFIGURE_EVENT
                  					USB_Configure_Event();
#endif
									break;
								default:
									break;
							}
							break;

					default:
						break;

					}
					break;

				case REQUEST_CLASS:
#if USB_CLASS
					switch (SetupPacket.bmRequestType.BM.Recipient)
					{
						case REQUEST_TO_INTERFACE:
#if USB_HID
							if (SetupPacket.wIndex.WB.L == USB_HID_IF_NUM)
							{
								switch (SetupPacket.bRequest)
								{
									case HID_REQUEST_GET_REPORT:
										if (HID_GetReport())
										{
											EP0Data.pData = EP0Buf;
											USB_DataInStage();
										}
										break;
									case HID_REQUEST_SET_REPORT:
										EP0Data.pData = EP0Buf;
										break;
									case HID_REQUEST_GET_IDLE:
										if (HID_GetIdle())
										{
											EP0Data.pData = EP0Buf;
											USB_DataInStage();
										}
										break;
									case HID_REQUEST_SET_IDLE:
										if (HID_SetIdle())
										{
											USB_StatusInStage();
										}
										break;
									case HID_REQUEST_GET_PROTOCOL:
										if (HID_GetProtocol())
										{
											EP0Data.pData = EP0Buf;
											USB_DataInStage();
										}
										break;
									case HID_REQUEST_SET_PROTOCOL:
										if (HID_SetProtocol())
										{
											USB_StatusInStage();
										}
										break;
									default:
										break;
								}
							}
#endif  /* USB_HID */
						default:
							break;
					}
					break;
#else
#endif  /* USB_CLASS */

				case REQUEST_VENDOR:
					break;

				default:
					USB_SetStallEP(0x80);
					EP0Data.Count = 0;
					break;
			}
			break;

		case USB_EVT_OUT:
			if (SetupPacket.bmRequestType.BM.Dir == 0)
			{
				if (EP0Data.Count)
				{
					USB_DataOutStage();
					if (EP0Data.Count == 0)
					{
						switch (SetupPacket.bmRequestType.BM.Type)
						{
							case REQUEST_STANDARD:
								USB_SetStallEP(0x80);
								//EP0Data.Count = 0;
								break;
#if (USB_CLASS)
							case REQUEST_CLASS:
								switch (SetupPacket.bmRequestType.BM.Recipient)
								{
									case REQUEST_TO_INTERFACE:
#if USB_HID
										if (SetupPacket.wIndex.WB.L == USB_HID_IF_NUM)
										{
											if (!HID_SetReport())
											{
												USB_SetStallEP(0x80);
												//EP0Data.Count = 0;
												//break;
											}
											break;
											//HID_SetReport();
										}
#endif
										USB_SetStallEP(0x80);
										//EP0Data.Count = 0;
										break;
									case REQUEST_TO_ENDPOINT:
										USB_SetStallEP(0x80);
										//EP0Data.Count = 0;
										break;
									default:
										USB_SetStallEP(0x80);
										//EP0Data.Count = 0;
										break;
								}
								break;
#endif
							default:
								USB_SetStallEP(0x80);
								//EP0Data.Count = 0;
								break;
						}
						USB_StatusInStage();
					}
				}
			}
			else
			{
				;//USB_StatusOutStage();
			}


			break;

		case USB_EVT_IN:
			if (SetupPacket.bmRequestType.BM.Dir == 1)
			{
				if(EP0Data.Count > 0) // ysq add
					USB_DataInStage();
			}
			//else
			//{
			//	if (USB_DeviceAddress & 0x80)
			//	{
			//		USB_DeviceAddress &= 0x7F;
			//		USB_SetAddress(USB_DeviceAddress);
			//	}
			//}
			break;

		case USB_EVT_IN_STALL:
			USB_ClrStallEP(0x80);
			break;

		case USB_EVT_OUT_STALL:
			USB_ClrStallEP(0x00);
			break;
	}
}
