## @file Slaves.py
## @brief This script is used as component (Slave devices with formula) for testing a BlueNRG-1, BlueNRG-2 multiple connection & simultaneously Master & Slave scenario. One BLE device (Master_Slave) is configured as Central & Peripheral, with a service and one characteristic and it performs simultaneously advertising and scanning in order to connect respectively, to up to Num_Slaves BLE Peripheral/Slave devices Slave1, Slave2, ..... (which have defined the same service and characteristic) and then to  up to Num_Masters Central/Master devices. The number Num_Slaves of Slave device depends on max number of supported multiple connections (8) and the number Num_Masters [0-2] of selected Master devices: Num_Slaves = 8 - Num_Masters.
## Once the connections with slaves and devices are performed, BLE Master_Slave device receives characteristics notifications from Num_Slaves devices and it also notifies  characteristics (as Peripheral)  to the Num_Masters BLE Master devices (if any),  which displays the related values. Num_Slaves devices notified characteristic value is as follow: <slave_index><counter_value>, where slave_index is one byte in the range [1 - Num_Slaves], and counter_value is a 2 bytes integer counter starting from 0. Master_Slave device notifies only the slave_index component to the Master device. 
## @li NOTE: if both Master_Slave and Slave(i) device supports data length extension, the ATT_MTU size is increased to the common max supported value (Att_Mtu),  using ACI_GATT_EXCHANGE_CONFIG() API, and the slave(i) notifies a characteristic of Att_Mtu - 3 bytes (the notification length is also tailored to the actual notification length received from each Slave, and to the max number of octets for data length extension handled by connected Master devices).
## @li The script is implemented using the new BlueNRG GUI language scripts based on the BLE ACI APIs. 
## @li The script can be run only with the script launcher standalone utility available within BlueNRG GUI SW package.
## @li A PC running Windows Vista/7 operating systems (64 and 32 bit version) or Windows XP operating systems (32 bit edition), with available USB ports is required for installing the BlueNRG GUI SW package.
##
## <!-- Copyright 2018 by STMicroelectronics.  All rights reserved.       *80*-->
##
##  @section S320 Slaves.py execution steps 
##
##  @li <b>BlueNRG/BlueNRG-MS platform configuration steps</b>: <ol>
##  <li> Connect the selected platform to a PC USB port
##  <li> Put selected platform in DFU mode and download the "C:\Program Files (x86)\STMicroelectronics\BlueNRG GUI x.x.x\Firmware\STM32L1_prebuilt_images\BlueNRG_VCOM_x_x.hex" using the BlueNRG GUI, Tools, Flash Motherboard FW... utility </ol>
##  @li <b>BlueNRG-1,2 platforms configuration steps</b>: <ol>
##  <li> Connect the selected Num_Slaves platforms to a Num_Slaves PC USB ports
##  <li> BlueNRG-1 platform: download the "C:\Program Files (x86)\STMicroelectronics\BlueNRG GUI x.x.x\Firmware\DTM\BlueNRG1\DTM_UART.hex or DTM_SPI.hex" file, using the Drag-and-Drop upgrade capability or the BlueNRG-1,2 Flasher utility available on STSW-BNRGFLASHER SW package.
##  <li> BlueNRG-2 platform: download the "C:\Program Files (x86)\STMicroelectronics\BlueNRG GUI x.x.x\Firmware\DTM\BlueNRG2\DTM_UART.hex or DTM_SPI.hex" file, using the Drag-and-Drop upgrade capability or the BlueNRG-1,2 Flasher utility available on STSW-BNRGFLASHER SW package. 
##  <li> STEVAL-IDB007V1M  platform: download the "C:\Program Files (x86)\STMicroelectronics\BlueNRG GUI x.x.x\Firmware\DTM\STEVAL-IDB007V1M\DTM_UART.hex or DTM_SPI.hex" file, using the Drag-and-Drop upgrade capability or the BlueNRG-1,2 Flasher utility available on STSW-BNRGFLASHER SW package.
##  <li> <b> NOTES</b>: SPI Network coprocessor configuration file (DTM_SPI.hex) requires some HW changes to be applied to the BlueNRG-1 development platform. Refer to STSW-BNRGUI user manual UM2058. </ol>
##  @li Open a DOS shell and go to the BlueNRG GUI installation folder : C:\Program Files (x86)\STMicroelectronics\BlueNRG GUI x.x.x\Application
##  @li On DOS shell launch the script launcher utility:  BlueNRG_Script_Launcher.exe -s <script_user_folder>\Slaves_Num_Slaves.py
##  @li Script requests user to add the number of expected Master devices (masters of Master_Slave device) and the number of slaves devices Num_Slaves and to confirm that Num_Slaves devices are actually connected to PC USB ports
##  @li Slaves.py puts each of Num_Slaves device in discovey mode with advertising name = slavem, waiting for Master_Slave device to connect it and to enable characteristic notification
##  @li At this stage, Slaves.py script start Num_Slave characteristic notifications with length equal to the MIN between the agreed Att_Mtu value , Slave(i) characteristic length and max number of octets for data length extension handled by connected Master devices.
##
##  @section S321 Master_Slave.py script execution steps through BlueNRG GUI 
##
##  @li Refer to Master_Slave.py script documentation
##
##  @section S322 Master.py execution steps through BlueNRG GUI
##
##  @li Refer to Master.py script documentation
##
##  @section S323 Supported platforms
##
##  @li BlueNRG-2 development platforms (order codes: STEVAL-IDB008V1, STEVAL-IDB008V2, STEVAL-IDB009V1)
##  @li BlueNRG-1 development platforms (order codes: STEVAL-IDB007V1 (NRND), STEVAL-IDB007V2, STEVAL-IDB007V1M) 
##  @li BlueNRG-MS development platform (order code: STEVAL-IDB005V1) 
##  @li BlueNRG-MS Daughter Board (order code: STEVAL-IDB005V1D)
##  @li BlueNRG-MS USB dongle (order code: STEVAL-IDB006V1)
## 
##  @section S324 Supported BLE devices and stack versions
##
##  @li BlueNRG-2 stack v2.x
##  @li BlueNRG-1 stack v1.x, v2.x
##  @li BlueNRG-MS stack v7.2c
##
## \addtogroup BlueNRG_1_2_V2x_Formula_scripts
## @{
## \see Slaves.py for documentation.
## 
## @} 

## @cond DOXYGEN_SHOULD_SKIP_THIS

## Slaves device name and length
Device_Name = [0x73, 0x6c, 0x61,0x76,0x65,0x6d] ## slavem
Device_Name_Len = 6

MAX_SLAVE_DEVICES = 8
MAX_MASTER_DEVICES = 2

DEFAULT_ATT_MTU = 23
MAX_ATT_MTU = 220 ## MAX allowed for DTM application used with BlueNRG GUI

Num_Masters = INSERT_INT_NUMBER('Please enter number of expected Master devices (Num_Masters): [0 - ' + str(MAX_MASTER_DEVICES) + ']')

if (Num_Masters < 0) or (Num_Masters > MAX_MASTER_DEVICES) :
     ERROR('Wrong number for Num_Masters: ' + str(Num_Masters))

Num_Slaves =  INSERT_INT_NUMBER('Please enter number of expected Slave devices (Num_Slaves): [0 - ' + str(MAX_SLAVE_DEVICES - Num_Masters) + ']')

if (Num_Slaves < 0) or (Num_Slaves > (MAX_SLAVE_DEVICES - Num_Masters)) :
     ERROR('Wrong number for Num_Slaves: ' + str(Num_Slaves))

#User_Char_Len = INSERT_INT_NUMBER('Please enter Char Lenght value to be used with BlueNRG-2, BLE stack v2.1 or later Slave devices:' + ' [' + str(DEFAULT_ATT_MTU - 3) + ' - ' + str(MAX_ATT_MTU - 3) + ']' + ' extended data length')
#if (User_Char_Len < (DEFAULT_ATT_MTU - 3)) or (User_Char_Len > (MAX_ATT_MTU - 3)) :
#     ERROR('Wrong Char Length value:' + str(User_Char_Len))

INFO('Please connect ' + str(Num_Slaves) + ' Slave devices and press Enter key')

slave=[]
port= GET_ALL_ST_DK_COM_PORT()

print port

PRINT ('NUM PORT ' + str(len(port)))
Number_Port = len(port)


if (Number_Port < Num_Slaves):
    INFO('No sufficient Slaves Devices Connected (expected: ' + str(Num_Slaves) + ')')
    EXIT(-1)

#Initialize other devices Devices as slave
for i in range (0,min(Num_Slaves ,len(port))) :
    slave.append(BLUENRGNODE(port[i]))

##NUM_SLAVES = len(port)



## --------- Slaves Init ---------------------------------------------------------------------------------------------

PRINT('------------------------- INITIALIZE Slaves')
Service_Handle=[]
Char_Handle=[]

#PRINT(slave)
slave_fw_version=[]
char_len = []

for i in range (0,len(slave)):
    PRINT('INITIALIZE SLAVE ' +str(i+1))
    slave[i].HW_RESET()
   
    slave[i].SET_PUBLIC_ADDRESS()
    slave[i].ACI_HAL_SET_TX_POWER_LEVEL(En_High_Power=1,PA_Level = 4)

    # Get Device HW & BLE stack FW version 
    (hw_version, fw_version) =  slave[i].GET_STACK_VERSION()

    print fw_version
    
    slave_fw_version.insert(i,fw_version)

    slave[i].ACI_GATT_INIT()
    
    status,service_handle, dev_name_char_handle,appearance_char_handle=  slave[i].ACI_GAP_INIT(Role=slave[i].PERIPHERAL)
    if status !=0x00:
        ERROR('ACI_GAP_INIT ON SLAVE ' + str(i+1) + ' FAILED')

    ## Set Device Name Char to slavem
    #print slave_fw_version[i][0]
    #print slave_fw_version[i][1]

    if ((IS_BlueNRG_1(slave[i]) or IS_BlueNRG_2(slave[i])) and ((slave_fw_version[i][0] >= 2) and (slave_fw_version[i][1] >= 1))):
         status = slave[i].ACI_GATT_UPDATE_CHAR_VALUE_EXT(Conn_Handle_To_Notify = 0, Service_Handle=service_handle,Char_Handle=dev_name_char_handle, Update_Type = 0,  Char_Length = Device_Name_Len, Value_Offset=0x00,Value_Length = Device_Name_Len,Value=Device_Name)
    else:
        status =slave[i].ACI_GATT_UPDATE_CHAR_VALUE(Service_Handle=service_handle,Char_Handle=dev_name_char_handle,Val_Offset=0x00,Char_Value_Length = Device_Name_Len,Char_Value=Device_Name)
    if status!=0x00:
        ERROR('ACI_GATT_UPDATE_CHAR_VALUE ON SLAVE ' + str(i+1) + ' FAILED')   
    
    #Add a Services for each Slave
    Status,ServHandle=slave[i].ACI_GATT_ADD_SERVICE(Service_UUID_Type=0x01, Service_UUID_16=0xA001,Service_Type=1,Max_Attribute_Records=5)      
    if Status !=0x00:
        ERROR('ACI_GATT_ADD_SERVICE ON SLAVE ' + str(i+1) + 'CALL FAILED')    

    Service_Handle.insert(i, ServHandle)
   
    if IS_BlueNRG_2(slave[i]) and ((slave_fw_version[i][0] >= 2) and (slave_fw_version[i][1] >= 1)):
         
         ## Data length extension is supported: set characteristic lenght to MAX_ATT_MTU - 3
         char_len.insert(i, MAX_ATT_MTU - 3) ##User_Char_Len - 3
         #Add a Characteristic (Notify properties) on Slave
         Status,CharHandle=slave[i].ACI_GATT_ADD_CHAR(Service_Handle =ServHandle, Char_UUID_Type=0x01,Char_UUID_16= 0xA002,Char_Value_Length=char_len[i], Char_Properties=0x10, #Only Notification
                         Security_Permissions=0x00,   
                         GATT_Evt_Mask=0x01, Enc_Key_Size=0x07,Is_Variable=0x01)      
    else: 
         ## Set characteristic lenght to DEFAULT_ATT_MTU - 3
         char_len.insert(i, DEFAULT_ATT_MTU -3)
         #Add a Characteristic (Notify properties) on Slave
         Status,CharHandle=slave[i].ACI_GATT_ADD_CHAR(Service_Handle =ServHandle, Char_UUID_Type=0x01,Char_UUID_16= 0xA002,Char_Value_Length=char_len[i], Char_Properties=0x10, #Only Notification
                         Security_Permissions=0x00,   
                         GATT_Evt_Mask=0x01, Enc_Key_Size=0x07,Is_Variable=0x01)         
    if Status !=0x00:
        ERROR('ACI_GATT_ADD_CHAR ON SLAVE ' + str(i+1) + ' FAILED')
    
    Char_Handle.insert(i, CharHandle)


    ## Set Data length extension to max supported value x allowing to handle increased ATT_MTU size on a single LL PDU packet
    if True:

         if IS_BlueNRG_2(slave[i]) and ((slave_fw_version[i][0] >= 2) and (slave_fw_version[i][1] >= 1)): 
             TX_OCTECTS = MAX_ATT_MTU + 4 ## +4  for L2CAP header 
             TX_TIME = (TX_OCTECTS +14)*8 ## 0 Don't modify it `
             Status = slave[i].HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH(SuggestedMaxTxOctets = TX_OCTECTS, SuggestedMaxTxTime  = TX_TIME)
             if Status !=0x00:
                 ERROR('HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH ON SLAVE ' + str(i+1) + ' FAILED')


    Status = slave[i].HCI_LE_SET_SCAN_RESPONSE_DATA(Scan_Response_Data_Length = 0,Scan_Response_Data = 0) ## Set Scan Response to 0  
    if Status!=0x00:
        ERROR('HCI_LE_SET_SCAN_RESPONSE_DATA ON SLAVE ' +  str(i+1)+ ' FAILED')
    

## --------- Slaves Discovery Mode --------------------------------------------------------------------------------------------------

    PRINT('------------------------- SLAVE ' +  str(i+1)+ ' DISCOVERY MODE')
    #Slave: Put in Discoverable Mode with Name = 'slavem' = [0x73, 0x6c, 0x61,0x76,0x65,0x6e](Add prefix 0x09 to indicate the AD type Name)
    Status =slave[i].ACI_GAP_SET_DISCOVERABLE(Advertising_Type= 0x00,
                                              Advertising_Interval_Min=0x100, #0x20,
                                              Advertising_Interval_Max=0x100,
                                              Local_Name_Length=Device_Name_Len +1,
                                              Local_Name=[0x09,0x73, 0x6c, 0x61,0x76,0x65,0x6d], ##+ [i] ##0x09 + slavem
                                              Slave_Conn_Interval_Min = 0x0006,
                                              Slave_Conn_Interval_Max = 0x0008) 
    if Status !=0x00:
        ERROR('ACI_GAP_SET_DISCOVERABLE ON SLAVE ' +  str(i+1)+ ' FAILED')
    else:
        PRINT('ACI_GAP_SET_DISCOVERABLE ON SLAVE ' +  str(i+1)+ ' OK')


conn_handle = []

for i in range (0,len(slave)):

    event = slave[i].WAIT_EVENT(HCI_LE_META_EVENT,Subevent_Code=HCI_LE_CONNECTION_COMPLETE_EVENT )

    if event.get_param('Status').val==0x00:
        conn_handle.insert(i, event.get_param('Connection_Handle').val)

        PRINT('Master_Slave connection handle with Slave[' + str(i+1)+ ']: ' + str(hex(conn_handle[i])))
        ##event = slave[i].WAIT_EVENT(HCI_VENDOR_EVENT,Ecode=ACI_GATT_ATTRIBUTE_MODIFIED_EVENT)  
        SLEEP(1)   


master_slave_DLE_support = 0 ## Flag to store if Master_Slave device and at least one Slave device support data length extension

search_for_DLE = True

## Check if connected peer Master_Slave device supports data length extension and if this can be actually used on 
## communication with specific slave also supporting data length extension. 
## If connected peer Master_Slave device supports data length extension, after connection, if at least on slave supports data length extension, the
##  HCI_LE_DATA_LENGTH_CHANGE_EVENT should be raised as consequence of the related HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH
while (search_for_DLE):
     for i in range (0,len(slave)):
         if IS_BlueNRG_2(slave[i]) and ((slave_fw_version[i][0] >= 2) and (slave_fw_version[i][1] >= 1)):
              event = slave[i].WAIT_EVENT(HCI_LE_META_EVENT, Subevent_Code = HCI_LE_DATA_LENGTH_CHANGE_EVENT, timeout=2, continueOnEvtMiss=True)
              if event is None:
                    ##NO HCI_LE_DATA_LENGTH_CHANGE_EVENT received: data length extension  not supported on master__slave side
                    PRINT('Master_Slave device doesn\'t support data length extension: use default ATT_MTU (23 bytes)') 
                    search_for_DLE = False
                    break
              else:
                   ## HCI_LE_DATA_LENGTH_CHANGE_EVENT has been raised : Master_Slave device supports data length extension  --> set flag
                   PRINT('Master_Slave device and at least one Slave(i) device support data length extension: increase ATT_MTU to max supported from both devices')
                   master_slave_DLE_support = 1
                   search_for_DLE = False
                   break
     if (i ==  len(slave) -1): ## No slave device supports data length extension
          PRINT('No slave device supports data length extension: use default ATT_MTU (23 bytes)')
          search_for_DLE = False
     

Rx_Mtu = []

TX_OCTECTS = MAX_ATT_MTU + 4 ## +4  for L2CAP header 
TX_TIME = (TX_OCTECTS +14)*8 ## 0 Don't modify it `

for i in range (0,len(slave)):
     if (IS_BlueNRG_2(slave[i]) and ((slave_fw_version[i][0] >= 2) and (slave_fw_version[i][1] >= 1))):

       ## BlueNRG-2, BLE stack v2.1 slave(i)  support DLE: if Master_Slave supports data length extension, slave(i) performs
       ## an ATT_MTU exchange for try to increase ATT_MTU to max value, and get benefit from data length extension on both devices.
       if (master_slave_DLE_support == 1): 

            Status =slave[i].ACI_GATT_EXCHANGE_CONFIG(Connection_Handle= conn_handle[i])
            if (status !=0):
               ret = slave[i].ACI_GAP_TERMINATE(conn_handle[i], Reason=0x13) 
               ERROR('ACI_GATT_EXCHANGE_CONFIG FAILURE: ') + str(status)
            else:
               event = slave[i].WAIT_EVENT(HCI_VENDOR_EVENT, Ecode = ACI_ATT_EXCHANGE_MTU_RESP_EVENT)
               PRINT('Slave(' + str(i) + '): ACI_ATT_EXCHANGE_MTU_RESP_EVENT , RX MTU: ' + str(event.get_param('Server_RX_MTU').val))
               ## Set the MTU value to be used for the communication between slave(i) and master_slave device
               Rx_Mtu.insert(i,event.get_param('Server_RX_MTU').val)
               event = slave[i].WAIT_EVENT(HCI_VENDOR_EVENT, Ecode = ACI_GATT_PROC_COMPLETE_EVENT)
               if event.get_param('Error_Code').val ==0:
                  PRINT('ACI_GATT_PROC_COMPLETE_EVENT OK')
       else:
            ## Master_Slave device doesn't support extended data length: MTU is set to default value
            Rx_Mtu.insert(i,DEFAULT_ATT_MTU)
     else:
          ## Slave(i) device doesn't support extended data length: MTU is set to default value
          Rx_Mtu.insert(i,DEFAULT_ATT_MTU)
          

                       
PRINT('------------------------- Notification Loop')
counter = 0 
##PRINT(len(slave))

#Send Notifications loop from Slave9i) to Master device

while(True):
    
        for i in range (0,len(slave)):
            
            SLEEP(0.5)
            ## Slave[i] sends notification to master_slave: slave index is used as notification value in order to trace notification from each slave on all masters
            value = ((i+1)<<16) + counter

            ## Set notification length based on the defined Rx_Mtu[i] value: if both devices supports data length extension, the RX_Mtu is increaased 
            ## for taking benefit of single ATT packet sent on a single LL PDU packet
            update_len = Rx_Mtu[i] - 3  ##DEFAULT_ATT_MTU -3

            PRINT (str(counter) + ': Send Notification Value (lenght: ' + str(update_len) + '): ' + str(hex(value)))

            Status = 0x64 ##BLE_STATUS_INSUFFICIENT_RESOURCES
            while (Status == 0x64):
                 if (IS_BlueNRG_1(slave[i]) or IS_BlueNRG_2(slave[i]) and ((slave_fw_version[i][0] >= 2) and (slave_fw_version[i][1] >= 1))): 
            
                      Status = slave[i].ACI_GATT_UPDATE_CHAR_VALUE_EXT(Conn_Handle_To_Notify =conn_handle[i], Service_Handle=Service_Handle[i], Char_Handle=Char_Handle[i], Update_Type = 1,  Char_Length = update_len,Value_Offset=0x00, Value_Length = update_len, Value=value)
                 else:
                      Status = slave[i].ACI_GATT_UPDATE_CHAR_VALUE(Service_Handle=Service_Handle[i], Char_Handle=Char_Handle[i], Val_Offset=0x00, Char_Value_Length = update_len, Char_Value=value)

                      if Status == 0x64:
                           ## Wait tx pool available event
                           WAIT_EVENT(HCI_VENDOR_EVENT, Ecode = ACI_GATT_TX_POOL_AVAILABLE_EVENT)
                      elif Status !=0:  
                           PRINT('--- Slave(' + str(i+1) + ') ACI_GATT_UPDATE_CHAR_VALUE FAILED: ' + str(hex(Status)) + '  ' + str(hex(conn_handle[i])))         
                      
            if (i == len(slave)-1):
                 ##Increase counter
                 counter +=1
## @endcond

            
