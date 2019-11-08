# BlueNRG_Demo
supplement BlueNRG demo usage

为了方便客户使用BlueNRG-1/2  我补充一些相关的Demo 和增加一些补丁在此工程中。
备注: 代码仅供参考，非正式官方发布，使用需要自行测试验证，欢迎汇报问题和提意见建议。


1.基于virtual timer 的软件RTC demo:
	工程目录:	BlueNRG_Demo\BlueNRG-1_2 DK 3.1.0\Project\Supply\BLE_RTC\
	BlueNRG-1/2 本身包含硬件RTC， 但是硬件RTC，在睡眠的时候
	是不工作的。为了更好的提供时间得管理，这里我增加一个
	基于软件virtual timer RTC的Demo，补充这点不足。
	基本原理是数 virtual timer的tick数。
	
	
	
2. OTA的优化
	


3. Flash访问和蓝牙事件互斥程序设计参考demo
	工程目录: BlueNRG_Demo\BlueNRG-1_2 DK 3.1.0\Project\Supply\BLE_Chat_Master_Slave_WithFlashAccess\



4. 串口透传程序设计
	SDK本身提供DTM固件可以做协处理器，非常好用，强烈建议使用DTM工程进行
	和外部MCU之间通过ACI指令（类似HCI指令）的通信。
	DTM 工程路径: BlueNRG_Demo\BlueNRG-1_2 DK 3.1.0\Project\BLE_Examples\DTM
	
	依旧很多客户做模块喜欢设计自己的透传程序，可以参考如下Demo:
	BlueNRG-1_2 DK 3.1.0\Project\Supply\BLE_Throughput_MaxGatt
	这个Demo也包含设置使用最大MTU的方法
	

5. 使用芯片序列号映射 免费的Random device address
	


6. 类似OS的软件定时器和消息队列设计
	
	
	





	



