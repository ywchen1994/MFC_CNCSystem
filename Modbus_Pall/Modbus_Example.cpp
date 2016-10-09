// Modbus_Example.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "modbus.h"

int _tmain(int argc, _TCHAR* argv[])
{
	modbus_t *mb;
	uint16_t tab_reg[32];

	//MS連結方式是透過modbus TCP, MS IP: 192.168.1.1 ,Port: 502
	mb = modbus_new_tcp("192.168.1.205", 502);
	modbus_connect(mb);
	modbus_set_slave(mb,2); // Slave 設定 2 表示是在PLC定址區操作
	
	/* Read 5 registers from the address 0 */
	int addr = 0x1080;	//位址設定
	int value = 77;	//寫入數值
	int rec = modbus_write_register(mb, addr, value);		//寫入一筆資料是1個word的大小

	modbus_close(mb);
	modbus_free(mb);
}

