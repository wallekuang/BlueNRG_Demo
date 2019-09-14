## @file Master.py
## @brief This script is used as component (Master devices with formula) for testing a BlueNRG-1, BlueNRG-2 multiple connection & simultaneously Master & Slave scenario. One BLE device (Master_Slave) is configured as Central & Peripheral, with a service and one characteristic and it performs simultaneously advertising and scanning in order to connect respectively, to up to Num_Slaves BLE Peripheral/Slave devices Slave1, Slave2, ..... (which have defined the same service and characteristic) and then to  up to Num_Masters Central/Master devices. The number Num_Slaves of Slave device depends on max number of supported multiple connections (8) and the number Num_Masters [0-2] of selected Master devices: Num_Slaves = 8 - Num_Masters.
## Once the connections with slaves and devices are performed, BLE Master_Slave device receives characteristics notifications from Num_Slaves devices and it also notifies  characteristics (as Peripheral)  to the Num_Masters BLE Master devices (if any),  which displays the related values.Num_Slaves devices notified characteristic value is as follow: <slave_index><counter_value>, where slave_index is one byte in the range [1 - Num_Slaves], and counter_value is a 2 bytes integer counter starting from 0. Master_Slave device notifies only the slave_index component to the Master device. 
## @li NOTE: if both Master_Slave and Slave(i) device supports data length extension, the ATT_MTU size is increased to the common max supported value (Att_Mtu),  using ACI_GATT_EXCHANGE_CONFIG() API, and the slave(i) notifies a characteristic of Att_Mtu - 3 bytes. Same path is followed for the Master_Slave to Master(i) communication , if both devices supports the data length extension feature (the notification length is also tailored to the actual notification length received from each Slave, and to the max number of octets for data length extension handled by connected Master devices).
## @li The script is implemented using the new BlueNRG GUI language scripts based on the BLE ACI APIs. The BlueNRG GUI with script engine is delivered within BlueNRG GUI SW package.
## @li The script can also be run  with the script launcher standalone utility available within BlueNRG GUI SW package.
## @li A PC running Windows Vista/7 operating systems (64 and 32 bit version) or Windows XP operating systems (32 bit edition), with available USB ports is required for installing the BlueNRG GUI SW package.
##
## <!-- Copyright 2018 by STMicroelectronics.  All rights reserved.       *80*-->
##
##  @section S330 Master.py script execution steps through BlueNRG GUI
##
##  @li Open BlueNRG GUI 
##  @li <b>BlueNRG-1,2 platforms configuration steps</b>: <ol>
##  <li> Connect the selected platform to a PC USB port
##  <li> BlueNRG-1 platform: download the "C:\Program Files (x86)\STMicroelectronics\BlueNRG GUI x.x.x\Firmware\DTM\BlueNRG1\DTM_UART.hex or DTM_SPI.hex" file, using the Drag-and-Drop upgrade capability or the BlueNRG-1,2 Flasher utility available on STSW-BNRGFLASHER SW package.
##  <li> BlueNRG-2 platform: download the "C:\Program Files (x86)\STMicroelectronics\BlueNRG GUI x.x.x\Firmware\DTM\BlueNRG2\DTM_UART.hex or DTM_SPI.hex" file, using the Drag-and-Drop upgrade capability or the BlueNRG-1,2 Flasher utility available on STSW-BNRGFLASHER SW package. 
##  <li> STEVAL-IDB007V1M  platform: download the "C:\Program Files (x86)\STMicroelectronics\BlueNRG GUI x.x.x\Firmware\DTM\STEVAL-IDB007V1M\DTM_UART.hex or DTM_SPI.hex" file, using the Drag-and-Drop upgrade capability or the BlueNRG-1,2 Flasher utility available on STSW-BNRGFLASHER SW package.
##  <li> <b> NOTES</b>: SPI Network coprocessor configuration file (DTM_SPI.hex) requires some HW changes to be applied to the BlueNRG-1 development platform. Refer to STSW-BNRGUI user manual UM2058. </ol>
##  @li On BlueNRG GUI, open the USB Port associated to the plugged platform
##  @li On BlueNRG GUI, select the Scripts window 
##  @li On BlueNRG GUI, Scripts window, Scripts Engine section, click on ... tab and browse to the Master.py script location and select  it
##  @li On BlueNRG GUI, click on  Run Script tab for running the selected script
##  @li Master device is configured as Central and it performs a connection procedure for connecting to the Master_Slave device. 
##  @li Then Master device  enables the characteristic notification on connected device.
##  @li At this stage, Master receives and displays the received characteristics values (slave_index component).
##  @li NOTE: Master device can be replaced by a Smarthphone device
##
##  @section S331 Master_Slave.py script execution steps through BlueNRG GUI 
##
##  @li Refer to Master_Slave.py script documentation
##
##  @section S332 Slaves.py execution steps through BlueNRG Script Launcher
##
##  @li Refer to Slaves.py script documentation
##
##  @section S333 Supported platforms
##
##  @li BlueNRG-2 development platforms (order codes: STEVAL-IDB008V1, STEVAL-IDB008V2, STEVAL-IDB009V1) 
##  @li BlueNRG-1 development platforms (order codes: STEVAL-IDB007V1 (NRND), STEVAL-IDB007V2, STEVAL-IDB007V1M) 
## 
##  @section S334 Supported BLE devices and stack versions
##
##  @li BlueNRG-2 stack v2.x
##  @li BlueNRG-1 stack v1.x, v2.x
##

## \addtogroup BlueNRG_1_2_V2x_Formula_scripts
## @{
## \see Master.py for documentation.
## 
## @} 

## @cond DOXYGEN_SHOULD_SKIP_THIS


##Master
ENABLE_CONNECTION_UPDATE = True 

Scan_Interval_ms = 300 ##
Scan_Window_ms =   300 ## 
Connection_Interval_ms = 15 ##15 <-----------------------
CE_Length_ms = 7.5 ## Don't change it

## Master device output connection parameters in internal time units (values to be used on BLE APIs)
Connection_Interval = int(Connection_Interval_ms/1.25)
CE_Length = int(CE_Length_ms/0.625)   ##<------ Time = N * 0.625 ---> N = Time/0.625
Scan_Interval = int(Scan_Interval_ms/0.625)
Scan_Window = int(Scan_Window_ms/0.625) 
SUPERVISION_TIMEOUT = 1000

#HW_RESET() 
SET_PUBLIC_ADDRESS() ##0x0280E100CCCC
ACI_HAL_SET_TX_POWER_LEVEL(En_High_Power=1,PA_Level = 4)
ACI_GATT_INIT()
ACI_GAP_INIT(Role=CENTRAL) #Central Role


ACI_GAP_START_GENERAL_DISCOVERY_PROC(LE_Scan_Interval=Scan_Interval, ##00x10
                                     LE_Scan_Window=Scan_Window) ##0x10)

listOfDeviceInADV=[]
while (True):
    event = WAIT_EVENT()
    if event.event_code==HCI_VENDOR_EVENT and event.get_param('Ecode').val==ACI_GAP_PROC_COMPLETE_EVENT:
        break
    else:
        if event.event_code==HCI_LE_META_EVENT and event.get_param('Subevent_Code').val==HCI_LE_ADVERTISING_REPORT_EVENT and event.get_param('Event_Type').val[0] != 0x4:  #SCAN_RSP    :
            #read name device
            name=GET_NAME(event.get_param('Data').val[0])
            ##PRINT(name)
            if name==None:
                continue
        
            if name=='advscan': ##  Master_Slave_simultaneuosly_SLAVE_of_1Master_MASTER_of_2_Slaves.py local name on advertising packets
                type_address =event.get_param('Address_Type').val[0]
                address =event.get_param('Address').val[0]     
                if type_address!=None and address!=None:
                    #If the device is not already inserted
                    if ([type_address,address] in listOfDeviceInADV)==False:
                        listOfDeviceInADV.insert(0,[type_address,address])                      
            
conn_handle=[]
PRINT('Num. Device in ADV= ' + str(len(listOfDeviceInADV)))

#Master looks for a slave device (Master & Slave device in simultaneously advertising/scanning mode) 
for i in range (0,min(8,len(listOfDeviceInADV))) :    
    #-----------------------------------------CONNECTION TO A SLAVE ---------------------------    
    ACI_GAP_CREATE_CONNECTION(LE_Scan_Interval=Scan_Interval,
                              LE_Scan_Window=Scan_Window, 
                              Peer_Address_Type=listOfDeviceInADV[i][0],
                              Peer_Address=listOfDeviceInADV[i][1],
                              Conn_Interval_Min=Connection_Interval,
                              Conn_Interval_Max=Connection_Interval, 
                              Conn_Latency=0x00,
                              Supervision_Timeout= SUPERVISION_TIMEOUT,
                              Minimum_CE_Length=CE_Length, 
                              Maximum_CE_Length=CE_Length) 
    
    WAIT_EVENT(HCI_VENDOR_EVENT,Ecode=ACI_GAP_PROC_COMPLETE_EVENT )

    event = WAIT_EVENT(HCI_LE_META_EVENT,Subevent_Code=HCI_LE_CONNECTION_COMPLETE_EVENT )
    if event.get_param('Status').val==0x00:
        conn_handle.insert(0, event.get_param('Connection_Handle').val)

    if ENABLE_CONNECTION_UPDATE: 
        ## Wait for connection update request from Master_Slave device for aligning to its connection parameters
        event = WAIT_EVENT(HCI_VENDOR_EVENT, Ecode=ACI_L2CAP_CONNECTION_UPDATE_REQ_EVENT)

        Handle = event.get_param('Connection_Handle').val

        Id = event.get_param('Identifier').val
        L2cap = event.get_param('L2CAP_Length').val
        int_min = event.get_param('Interval_Min').val
        int_max = event.get_param('Interval_Max').val
        latency = event.get_param('Slave_Latency').val
        timeout = event.get_param('Timeout_Multiplier').val


        Status= ACI_L2CAP_CONNECTION_PARAMETER_UPDATE_RESP(Connection_Handle=Handle, 
                                                                      Conn_Interval_Min=int_min,
                                                                      Conn_Interval_Max = int_max,
                                                                      Slave_latency = latency,
                                                                      Timeout_Multiplier = timeout,
                                                                      Minimum_CE_Length = CE_Length, 
                                                                      Maximum_CE_Length = CE_Length,
                                                                      Identifier = Id,
                                                                      Accept = 0x01					  
                                                                      )
        if Status!=0x00:
            PRINT('*****  FAILED COMMAND ACI_L2CAP_CONNECTION_PARAMETER_UPDATE_REQ' + str(hex(Status)))
            ##PRINT(hex(Status))

        WAIT_EVENT(HCI_LE_META_EVENT, Subevent_Code= HCI_LE_CONNECTION_UPDATE_COMPLETE_EVENT)


for i in range (0,len(conn_handle)) :
        #Enable Notification for the connection with Slave i
        PRINT ('ENABLE NOTIFICATION FROM SLAVE ' + str(i+1))
        ACI_GATT_WRITE_CHAR_DESC(Connection_Handle= conn_handle[i],
                                     Attr_Handle = 0x0f,
                                     Attribute_Val_Length=0x02,
                                     Attribute_Val=0x0001)

      
#Receive Notification
while(True):
    event = WAIT_EVENT(HCI_VENDOR_EVENT) 
    eCode = event.get_param('Ecode').val
    if (eCode == ACI_GATT_NOTIFICATION_EVENT):
        #PRINT('Notification from Slave ' + chr(e.get_param('Attribute_Value').val[-1]) + ' value: ' + str(GET_VALUE(e.get_param('Attribute_Value').val[0:-1])))
        #PRINT('Master Received Notification from connection: ' + str(hex(e.get_param('Connection_Handle').val)) + ' value= ' + str(e.get_param('Attribute_Value').val))
        if True:
            len =  event.get_param('Attribute_Value_Length').val
            value1 = event.get_param('Attribute_Value').val

            value = GET_VALUE(value1)
            slave_index = (value>>16) ## slave_index: 1 bytes
            slave_value = (value & 0x00FFFF) ## slave_value: 2 bytes 
            PRINT('Notification of ' + str(len) + ' bytes, from connection: ' + str(hex(event.get_param('Connection_Handle').val)) + ', Slave: ' + str(slave_index) + ', Counter: ' + str(slave_value)) 
        else:
            PRINT('Master Received Notification from connection: ' + str(hex(event.get_param('Connection_Handle').val)) + ' slave= ' + str(event.get_param('Attribute_Value').val[2]))
        
         
## @endcond
