#include "8285_download_protocol.h"

uint8_t  origin_buf[16384] = {0};
uint8_t sync_flag=0;
uint8_t wait_baud_flag = 0;
uint8_t nodata_flag = 0;

void Data_formatt_write(uint8_t *packet,int packet_len,uint8_t packet_type)
{
	uint8_t s=0;
	int i = 0;
	int j = 1;

	if(packet_type == HA_HEAD_HA_TAIL)
	{
		s=0xc0;
		usart1_write(&s,1);
		for(i=0;i<packet_len;i++)
		{
			if(packet[i] == 0xc0)
			{
				s=0xdb;
				usart1_write(&s,1);
				s=0xdc;
				usart1_write(&s,1);
			}
			else if(packet[i] == 0xdb)
			{
				s=0xdb;
				usart1_write(&s,1);
				s=0xdd;
				usart1_write(&s,1);
			}
			else
			{
				s=packet[i];
				usart1_write(&s,1);
			}
		}
		s=0xc0;
		usart1_write(&s,1);
	}
	else if(packet_type == NO_HEAD_HA_TAIL)
	{
		j = 0;
		for(i=0;i<packet_len;i++)
		{
			if(packet[i] == 0xc0)
			{
				s=0xdb;
				usart1_write(&s,1);
				s=0xdc;
				usart1_write(&s,1);
			}
			else if(packet[i] == 0xdb)
			{
				s=0xdb;
				usart1_write(&s,1);
				s=0xdd;
				usart1_write(&s,1);
			}
			else
			{
				s=packet[i];
				usart1_write(&s,1);
			}
		}
		s=0xc0;
		usart1_write(&s,1);
	}
	else if(packet_type == HA_HEAD_NO_TAIL)
	{
		s=0xc0;
		usart1_write(&s,1);
		for(i=0;i<packet_len;i++)
		{
			if(packet[i] == 0xc0)
			{
				s=0xdb;
				usart1_write(&s,1);
				s=0xdc;
				usart1_write(&s,1);
			}
			else if(packet[i] == 0xdb)
			{
				s=0xdb;
				usart1_write(&s,1);
				s=0xdd;
				usart1_write(&s,1);
			}
			else
			{
				s=packet[i];
				usart1_write(&s,1);
			}
		}
		j--;
	}
	else if(packet_type == NO_HEAD_NO_TAIL)
	{
		j = 0;
		for(i=0;i<packet_len;i++)
		{
			if(packet[i] == 0xc0)
			{
				s=0xdb;
				usart1_write(&s,1);
				s=0xdc;
				usart1_write(&s,1);
			}
			else if(packet[i] == 0xdb)
			{
				s=0xdb;
				usart1_write(&s,1);
				s=0xdd;
				usart1_write(&s,1);
			}
			else
			{
				s=packet[i];
				usart1_write(&s,1);
			}
		}
		j--;
	}
	i = j;
#if 1
	if(packet_len<512)
	{
		Debug_usart_write(packet,i+1,DATA_SEND_RECV_DABUG);
	}
	else
	{
		Debug_usart_write(packet,24,DATA_SEND_RECV_DABUG);
	}
#endif

}

int Data_formatt_read(uint8_t *packet,int len)
{
	uint8_t buf[512] = {0};
	uint8_t recv_len = 0;
	int i = 0;
	int j = 0;

	recv_len = read_more_data_to_datapool(&stm32rx,buf,512);

	if(recv_len > len)
	{
		recv_len = len;
	}

	for(i=0;i<recv_len;i++)
	{
		if(buf[i] == 0xdb)
		{
			if(buf[i+1] == 0xdc)
			{
				packet[j++] = 0xc0;
				i++;
			}
			else if(buf[i+1] == 0xdd)
			{
				packet[j++] = 0xdb;
				i++;
			}
		}
		else if(buf[i] != 0xc0)
		{
			packet[j++] = buf[i];
		}
	}

	Debug_usart_write(packet,10,DATA_SEND_RECV_DABUG);
	return recv_len;
}

void wait_rxdata_available(int timeout)
{
	recv_time_out = timeout;
	//stm32rx.avail = 0;
	Debug_usart_write("wait recv data...\r\n",19,INFO_DEBUG);
#if 1
	while(1)
	{
		if(stm32rx.avail == 1)
		{
			stm32rx.avail = 0;
			break;
		}
	}
#endif
}

void wait_baud_change(int timeout)
{
	wait_baud_flag = 1;

	wait_rxdata_available(10);

	wait_baud_flag = 0;
}

int device_sync(void)
{
	uint8_t sync_send_packet[44] = {
		0x00,0x08,0x24,0x00,0x00,0x00,0x00,0x00,0x07,0x07,0x12,0x20,0x55,0x55,0x55,
		0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
		0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};
	uint8_t sync_recv_packet[50] = {0};
	uint8_t sync_recv_true[10] = {0x01,0x08,0x02,0x00,0x07,0x07,0x12,0x20,0x00,0x00};
	uint8_t i;

	Data_formatt_write(sync_send_packet,44,HA_HEAD_HA_TAIL);

	sync_flag = 1;
	wait_rxdata_available(5);
	Data_formatt_read(sync_recv_packet,20);

	for(i=0;i<10;i++)
	{
		if(sync_recv_packet[i] != sync_recv_true[i])
		{
			Debug_usart_write("sync nok\r\n",10,INFO_DEBUG);
			return 0;
		}
	}

	return 1;
}

int Change_baud_command(int baud)
{
	//uint8_t change_command[16] = {
	//	0x00,0x0f,0x08,0x00,0x00,0x00,0x00,0x00,
	//	0xc0,0xc6,0x2d,0x00,0x00,0x00,0x00,0x00};
	uint8_t change_command[16] = {
			0x00,0x0f,0x08,0x00,0x00,0x00,0x00,0x00,
			0x60,0xe3,0x16,0x00,0x00,0x00,0x00,0x00};

	uint8_t change_recv[20] = {0};
	uint8_t change_true[10] = {0x01,0x0f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	uint8_t i = 0;

	Data_formatt_write(change_command,16,HA_HEAD_HA_TAIL);
	wait_rxdata_available(10);
	Data_formatt_read(change_recv,20);
	initDataPool(&stm32rx);
	usart1_change_baud(baud);

	for(i=0;i<10;i++)
	{
		if(change_recv[i] != change_true[i])
		{
			Debug_usart_write("change baud nok\r\n",10,INFO_DEBUG);
			return 0;
		}
	}

	return 1;
}

int Erasing_data_command(int file_type)
{
	uint8_t eras_command[24] = {
		0x00,0x02,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	uint8_t eras_recv[20] = {0};
	uint8_t eras_true[10] = {0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	uint8_t i = 0;

	switch(file_type)
	{
		case FIRMWARE_BIN:
			eras_command[10] = 0x0f;
			eras_command[13] = 0x04;
			eras_command[17] = 0x04;
			break;
		case DATA_INFO:
			eras_command[9] =  0x20;
			eras_command[12] = 0x10;
			eras_command[17] = 0x04;
			eras_command[21] = 0x80;
			eras_command[22] = 0x07;
			break;
		case SIGN_INFO:
			eras_command[9] =  0x20;
			eras_command[12] = 0x10;
			eras_command[17] = 0x04;
			eras_command[21] = 0xa0;
			eras_command[22] = 0x07;
			break;
		default:break;
	}

	initDataPool(&stm32rx);
	Data_formatt_write(eras_command,24,HA_HEAD_HA_TAIL);
	wait_rxdata_available(10);
	Data_formatt_read(eras_recv,20);
	initDataPool(&stm32rx);

	for(i=0;i<10;i++)
	{
		if(eras_recv[i] != eras_true[i])
		{
			return 0;
		}
	}

	return 1;
}

int stub_mem_finish(void)
{
	uint8_t mem_finish_command[16] = {
		0x00,0x06,0x08,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x84,0x06,0x10,0x40
	};

	uint8_t men_finish_recv[20] = {0};
	uint8_t men_finish_true[10] = {0x01,0x06,0x02,0x00,0x07,0x07,0x12,0x20,0x00,0x00};
	uint8_t i = 0;

	Data_formatt_write(mem_finish_command,16,HA_HEAD_HA_TAIL);
	wait_rxdata_available(5);
	Data_formatt_read(men_finish_recv,20);
	initDataPool(&stm32rx);

	for(i=0;i<10;i++)
	{
		if(men_finish_recv[i] != men_finish_true[i])
		{
			return 0;
		}
	}

	return 1;
}

int stub_mem_begin(uint8_t type)
{
	uint8_t mem_begin_command[24] = {
		0x00,0x05,0x10,0x00,0x00,0x00,0x00,0x00,0xe0,0x1d,0x00,0x00,
		0x02,0x00,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x00,0x10,0x40
	};

	uint8_t mem_begin_recv[20] = {0};
	uint8_t mem_begin_true[10] = {0x01,0x05,0x02,0x00,0x07,0x07,0x12,0x20,0x00,0x00};
	uint8_t i = 0;

	switch(type)
	{
		case MEM_BEGIN_TEXT:
			break;
		case MEM_BEGIN_DATA:
			mem_begin_command[8] =  0x00;
			mem_begin_command[9] =  0x03;
			mem_begin_command[12] = 0x01;
			mem_begin_command[20] = 0x9c;
			mem_begin_command[21] = 0xab;
			mem_begin_command[22] = 0xff;
			mem_begin_command[23] = 0x3f;
			break;
		default:break;
	}

	Data_formatt_write(mem_begin_command,24,HA_HEAD_HA_TAIL);
	wait_rxdata_available(5);
	Data_formatt_read(mem_begin_recv,20);

	initDataPool(&stm32rx);
	for(i=0;i<10;i++)
	{
		if(mem_begin_recv[i] != mem_begin_true[i])
		{
			return 0;
		}
	}

	return 1;
}

int stub_mem_block(uint8_t type)
{
	uint8_t mem_block_recv[20] = {0};
	uint8_t mem_block_true[10] = {0x01,0x07,0x02,0x00,0x07,0x07,0x12,0x20,0x00,0x00};
	uint8_t i = 0;

	switch(type)
	{
		case STUB_TEXT_UP:
			Data_formatt_write(stub_text_up,STUB_TEXT_UP_LEN,HA_HEAD_HA_TAIL);
			break;
		case STUB_TEXT_DOWN:
			Data_formatt_write(stub_text_down,STUB_TEXT_DOWN_LEN,HA_HEAD_HA_TAIL);
			break;
		case STUB_DATA:
			Data_formatt_write(stub_data,STUB_DATA_LEN,HA_HEAD_HA_TAIL);
			break;
		default:break;
	}

	wait_rxdata_available(5);
	Data_formatt_read(mem_block_recv,20);
	initDataPool(&stm32rx);

	for(i=0;i<10;i++)
	{
		if(mem_block_recv[i] != mem_block_true[i])
		{
			return 0;
		}
	}

	return 1;
}

int run_stub(void)
{
	uint8_t ret;

	ret = stub_mem_begin(MEM_BEGIN_TEXT);

	ret = stub_mem_block(STUB_TEXT_UP);

	ret = stub_mem_block(STUB_TEXT_DOWN);

	ret = stub_mem_begin(MEM_BEGIN_DATA);

	ret = stub_mem_block(STUB_DATA);

	ret = stub_mem_finish();

	return ret;
}

uint8_t checksum(int type,int data_len)
{
	int i = 0;
	uint8_t state = 0xef;

	for(i=0;i<data_len;i++)
	{
		if(type == FIRMWARE_BIN)
		{
			state ^= origin_buf[i];
		}
		else if(type == SIGN_INFO)
		{
			state ^= sign_sector_data[i];
		}
		else
		{
			state ^= data_sector_data[i];
		}
	}

	return state;
}



int send_data_command(int type,int data_len,uint8_t seq)
{
	int i = 0;
	uint8_t send_data[24] = {
		0x00,0x03,0x10,0x40,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	};
	uint8_t flash_block_recv[20] = {0};
	uint8_t flash_block_true[10] = {0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	if((data_len < 0x4000) && (data_len > 0x1000))
	{
		send_data[3] = 0x30;
		send_data[9] = 0x30;
		send_data[12] = 0x3f;
	}
	else if(data_len < 0x1000)
	{
		send_data[2] = 0x0f;
		send_data[3] = 0x10;
		send_data[8] = 0xff;
		send_data[9] = 0x0f;
	}

	send_data[12] = seq;
	send_data[4] = checksum(type,data_len);

	if(type == FIRMWARE_BIN)
	{
		Data_formatt_write(send_data,24,HA_HEAD_NO_TAIL);
		Data_formatt_write(origin_buf,8192,NO_HEAD_NO_TAIL);
		Data_formatt_write(&origin_buf[8192],data_len-8192,NO_HEAD_HA_TAIL);
	}
	else if(type == DATA_INFO)
	{
		Data_formatt_write(send_data,24,HA_HEAD_NO_TAIL);
		Data_formatt_write(data_sector_data,4095,NO_HEAD_HA_TAIL);
	}
	else if(type == SIGN_INFO)
	{
		Data_formatt_write(send_data,24,HA_HEAD_NO_TAIL);
		Data_formatt_write(sign_sector_data,4095,NO_HEAD_HA_TAIL);
	}


	wait_rxdata_available(10);
	Data_formatt_read(flash_block_recv,20);

	for(i=0;i<10;i++)
	{
		if(flash_block_recv[i] != flash_block_true[i])
		{
			return 0;
		}
	}

	return 1;
}

int download_sign_operate(int type)
{
	int ret;

	ret = Erasing_data_command(type);
	if(ret)
	{
		Debug_usart_write("eras_sign_ok\r\n",14,INFO_DEBUG);
		change_sign_info();
		ret = send_data_command(SIGN_INFO,4095,0);
		iwdg_reload();
		if(ret)
		{
			Debug_usart_write("send_sign_ok\r\n",14,INFO_DEBUG);
			return 1;
		}
		else
		{
			Debug_usart_write("send_sign_nok\r\n",15,INFO_DEBUG);
			return 0;
		}
	}
	else
	{
		Debug_usart_write("eras_sign_nok\r\n",15,INFO_DEBUG);
		return 0;
	}
}

int download_data_operate(int type)
{
		int ret = 1;
		FIL fnew;
		int num;
		int seq=0;
		uint8_t data_buf[98] = {0};
		uint8_t buf[2]= {0};
		int file_point = 0;

		ret = Erasing_data_command(type);
		if(ret)
		{
			ret = f_open(&fnew,(uint8_t*)"0:data.csv",FA_READ | FA_WRITE);
	#if 1
			if(ret == FR_OK)
			{
				while(1)
				{
					Debug_usart_write("csv read begin\r\n",16,INFO_DEBUG);
					ret = f_read(&fnew,data_buf,98,&num);
					seq++;
					if(ret == FR_OK)
					{
						if(num < 98)
						{
							f_close(&fnew);
							Debug_usart_write("no csv data\r\n",13,INFO_DEBUG);
							nodata_flag = 1;
							return 0;
						}
						file_point += 98;
						if(data_buf[0] == 'Y')
						{
							Debug_usart_write("csv read cnt:",13,INFO_DEBUG);
							hex_to_str(buf,seq);
							Debug_usart_write(buf,2,INFO_DEBUG);
							Debug_usart_write("\r\n",2,INFO_DEBUG);
							change_datacsv_info(data_buf);
							ret = send_data_command(DATA_INFO,4095,seq);
							iwdg_reload();
							seq++;
							if(!ret)
							{
								f_close(&fnew);
								Debug_usart_write("send_csv error\r\n",16,INFO_DEBUG);
								return ret;
							}
							else
							{
								data_buf[0] = 'N';
								ret = f_lseek(&fnew,file_point-98);
								if(ret == FR_OK)
								{
									Debug_usart_write("change file point ok\r\n",22,INFO_DEBUG);
									ret = f_write(&fnew,data_buf,98,&num);
									if ( ret == FR_OK )
									{
										f_close(&fnew);
										Debug_usart_write("change file ok\r\n",16,INFO_DEBUG);
										Debug_usart_write("send_csv ok\r\n",16,INFO_DEBUG);
										return 1;
									}
									else
									{
										f_close(&fnew);
										Debug_usart_write("change file nok\r\n",17,INFO_DEBUG);
										Debug_usart_write("send_csv error\r\n",16,INFO_DEBUG);
										return 0;
									}
								}
								else
								{
									f_close(&fnew);
									Debug_usart_write("change file point nok\r\n",22,INFO_DEBUG);
									return 0;
								}
							}
						}

					}
					else
					{
						f_close(&fnew);
						Debug_usart_write("csv_read nok\r\n",14,INFO_DEBUG);
						return 0;
					}
				}

			}
			else
			{
				f_close(&fnew);
				Debug_usart_write("csv_noopen\r\n",12,INFO_DEBUG);
				return 0;
			}
	#endif
		}
		else
		{
			Debug_usart_write("eras_csv_nok\r\n",14,INFO_DEBUG);
			return 0;
		}
}


int download_bin_operate(int type)
{
	int ret;
	FIL fnew;
	int num;
	int seq=0;
	uint8_t buf[2] = {0};

	ret = Erasing_data_command(type);
	if(ret)
	{
		Debug_usart_write("bin eras ok\r\n",13,INFO_DEBUG);
		ret = f_open(&fnew,(uint8_t*)"0:FWSW-0185-1.6.1-noflashcipher.bin",FA_READ);
#if 1
		if(ret == FR_OK)
		{
			while(1)
			{
				Debug_usart_write("bin read begin\r\n",17,INFO_DEBUG);
				ret = f_read(&fnew,origin_buf,16384,&num);
				Debug_usart_write("bin read cnt:",13,INFO_DEBUG);
				hex_to_str(buf,seq);
				Debug_usart_write(buf,2,INFO_DEBUG);
				Debug_usart_write("\r\n",2,INFO_DEBUG);
				if(ret == FR_OK)
				{
					if(num==0)
					{
						Debug_usart_write("send_bin_ok\r\n",13,INFO_DEBUG);
						f_close(&fnew);
						return 1;
					}
					iwdg_reload();
					ret = send_data_command(FIRMWARE_BIN,num,seq);
					iwdg_reload();
					seq++;
					if(!ret)
					{
						f_close(&fnew);
						Debug_usart_write("send_bin error\r\n",16,INFO_DEBUG);
						return 0;
					}
				}
				else
				{
					f_close(&fnew);
					Debug_usart_write("bin_read nok\r\n",14,INFO_DEBUG);
					return 0;
				}
			}

		}
		else
		{
			Debug_usart_write("bin_noopen\r\n",12,INFO_DEBUG);
			return 0;
		}
#endif
	}
	else
	{
		Debug_usart_write("eras_bin_nok\r\n",14,INFO_DEBUG);
		return 0;
	}
}


void change_datacsv_info(uint8_t *buf)
{
	int i;

	for(i=0;i<10;i++)
	{
		data_sector_data[103+i] = buf[86+i];//ita
		data_sector_data[124+i] = buf[2+i];//id
		data_sector_data[202+i] = buf[2+i];//id
		if(i<6)
		{
			data_sector_data[172+i] = str_to_hex(buf[50+(3*i)],buf[51+(3*i)]);
			data_sector_data[178+i] = str_to_hex(buf[68+(3*i)],buf[69+(3*i)]);
		}
	}

	for(i=0;i<36;i++)
	{
		data_sector_data[135+i] = buf[13+i];
	}
	for(i=372;i<4095;i++)
	{
		data_sector_data[i] = 0xFF;
	}
	sign_sector_data[0] = 0x01;
}


void change_sign_info(void)
{
	int i;

	sign_sector_data[0] = 0x00;
	for(i=1;i<4095;i++)
	{
		sign_sector_data[i] = 0xFF;
	}
}

int download_data_command(int type)
{
	int ret;

	switch(type)
	{
		case SIGN_INFO:
			ret = download_sign_operate(SIGN_INFO);
			break;
		case DATA_INFO:
			ret = download_data_operate(DATA_INFO);
			break;
		case FIRMWARE_BIN:
			ret = download_bin_operate(FIRMWARE_BIN);
			break;
		default:break;
	}

	return ret;
}

void update_light_status(uint8_t status)
{
	switch(status)
	{
		case DOWNLOAD_OK_STATUS:
			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
			GPIO_SetBits(GPIOB,GPIO_Pin_14);
			break;
		case DOWNLOAD_NOK_STATUS:
			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
			GPIO_SetBits(GPIOB,GPIO_Pin_13);
			break;
		case DOWNLOAD_NOW_STATUS:
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			break;
		default:break;
	}
}

int download_start(void)
{
	uint8_t cnt = 0;
	uint8_t ret = 1;
	//int i = 1000000;

	//ret = (uint8_t)(i>>2);

	//update_light_status(DOWNLOAD_NOW_STATUS);
	//change_datacsv_info("Y 10000d85e1 b58f5ee9-09d6-42c7-99be-4afb9c2b30b8 d0:27:00:1b:09:12 d0:27:00:1b:09:13 PSF-A01-GL");
	//Debug_usart_write(&ret,1,INFO_DEBUG);
	//return 1;
	if(nodata_flag)
	{
		return 0;
	}

	usart1_change_baud(115200);
	iwdg_reload();

	ret = device_sync();
	iwdg_reload();

	if(ret)
	{
		sync_flag = 0;
		update_light_status(DOWNLOAD_NOW_STATUS);
		Debug_usart_write("sync_ok\r\n",9,INFO_DEBUG);
		ret = run_stub();
		iwdg_reload();
	}
	else
	{
		return 0;
	}

	if(ret)
	{
		Debug_usart_write("stub_ok\r\n",9,INFO_DEBUG);
		//ret = Change_baud_command(923076);
		ret = Change_baud_command(1500000);
		iwdg_reload();
	}

	if(ret)
	{
		wait_baud_change(10);
		Debug_usart_write("change_baud_ok\r\n",16,INFO_DEBUG);
		ret = download_data_command(FIRMWARE_BIN);
		if(ret)
		{
			iwdg_reload();
			ret = download_data_command(SIGN_INFO);
			if(ret)
			{
				iwdg_reload();
				ret = download_data_command(DATA_INFO);
				if(ret)
				{
					iwdg_reload();
					update_light_status(DOWNLOAD_OK_STATUS);
					return ret;
				}
			}
		}
	}

	update_light_status(DOWNLOAD_NOK_STATUS);
	return ret;
}


