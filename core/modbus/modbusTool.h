
#include <iostream>
#include <string>
#include <vector>

#include <chrono> // 操作时间
#include <thread>

#include <modbus.h>
#include <errno.h>



/* 高阶字节CRC值表 */
static const uint8_t table_crc_hi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};



/* 低位字节CRC值表 */
static const uint8_t table_crc_lo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

/// <summary>
/// crc校验
/// </summary>
/// <param name="buffer"></param>
/// <param name="buffer_length"></param>
/// <returns></returns>
static uint16_t crc16(uint8_t* buffer, uint16_t buffer_length);



/// <summary>
/// modbus 主机模式
/// </summary>
class ModbusPoll
{
private:
	modbus_t* ctx;

public:

	/// <summary>
	/// 构造函数初始化
	/// </summary>
	/// <param name="device">串口设备路径(例如COM6, /dev/ttyS5)</param>
	/// <param name="baud">波特率</param>
	/// <param name="parity">奇偶校验位（'N'表示无校验）</param>
	/// <param name="data_bits">数据位（8位）</param>
	/// <param name="stop_bits">停止位（1位）</param>
	ModbusPoll(const char* device, int baud, char parity, int data_bits, int stop_bits)
	{
		// 创建Modbus RTU上下文
		ctx = modbus_new_rtu(device, baud, parity, data_bits, stop_bits);
		if (ctx == NULL)
		{
			std::cout << "Unable to create the libmodbus context" << std::endl;
			throw std::runtime_error("Unable to create the libmodbus context");
		}
	}

	~ModbusPoll()
	{
		// 关闭Modbus连接并释放资源
		modbus_close(ctx);
		modbus_free(ctx);
	}

	/// <summary>
	/// 主模式 写入数据到从机寄存器
	/// 查看使用 modbus slave: 
	///     Slave Definition:(功能码)03:Holding Register(04)
	/// </summary>
	/// <param name="slave_id">Modbus从机地址</param>
	/// <param name="start_addr">起始寄存器地址</param>
	/// <param name="num_regs">要读取或写入的寄存器数量</param>
	void pollWriteRegisters(int slave_id, int start_addr, int num_regs);

	/// <summary>
	/// 主模式 读取从机寄存器数据
	/// 查看使用 modbus slave: 
	///     Slave Definition:(功能码)03:Holding Register(04)
	/// </summary>
	/// <param name="slave_id">Modbus从机地址</param>
	/// <param name="start_addr">起始寄存器地址</param>
	/// <param name="num_regs">要读取或写入的寄存器数量</param>
	void pollReadRegisters(int slave_id, int start_addr, int num_regs);

	/// <summary>
	/// 主模式 写入线圈数据到从机线圈
	/// 查看使用 modbus slave: 01 Clil Status (0x)
	/// </summary>
	/// <param name="slave_id"></param>
	/// <param name="coil_addr"></param>
	/// <param name="num_coils"></param>
	void pollWriteCoils(int slave_id, int coil_addr, int num_coils);


	/// <summary>
	/// 主模式 读取从机线圈数据
	/// 查看使用 modbus slave: 查看使用 modbus slave: 01 Clil Status (0x)
	/// </summary>
	/// <param name="slave_id">Modbus从机地址</param>
	/// <param name="start_addr">起始线圈地址</param>
	/// <param name="num_coils">要读取或写入的线圈数量</param>
	void pollReadCoils(int slave_id, int coil_addr, int num_coils);



};



#define SERVER_ID 1
#define MD_COILS_MAX 10
#define MD_INPUT_COILS_MAX 10
#define MD_REGISTERS_MAX 10
#define MD_INPUT_REGISTERS_MAX 10

/// <summary>
/// modbus 从机模式
/// </summary>
class ModbusSlave
{
private:
	modbus_t* ctx_;
	uint8_t* query_;
	modbus_mapping_t* mapping_;
	bool running_;
	int ret;
	const uint16_t registersNbMax;
	const uint8_t serverId;


public:
	ModbusSlave(uint8_t serverId, uint16_t registersNbMax) : serverId(serverId), registersNbMax(registersNbMax), ctx_(nullptr), query_(nullptr), mapping_(nullptr), running_(false) {}

	~ModbusSlave()
	{
		stop();
		cleanup();
	}

	void start()
	{
		running_ = true;
		std::thread(&ModbusSlave::run, this).detach();
	}

	void stop()
	{
		running_ = false;
	}

	void cleanup()
	{
		if (mapping_)
			modbus_mapping_free(mapping_);
		if (query_)
			free(query_);
		if (ctx_)
		{
			modbus_close(ctx_);
			modbus_free(ctx_);
		}
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="port"></param>
	/// <param name="baudrate"></param>
	/// <param name="parity"></param>
	/// <param name="databits"></param>
	/// <param name="stopbits"></param>
	void initialize(const char* port, int baudrate, char parity, int databits, int stopbits)
	{
		// ctx = modbus_new_rtu("COM6", 9600, 'N', 8, 1);
		ctx_ = modbus_new_rtu(port, baudrate, parity, databits, stopbits);
		if (!ctx_)
		{
			std::cerr << "Failed to create Modbus context: " << modbus_strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}

		modbus_set_slave(ctx_, SERVER_ID);
		query_ = reinterpret_cast<uint8_t*>(malloc(MODBUS_RTU_MAX_ADU_LENGTH));
		if (!query_)
		{
			std::cerr << "Failed to allocate memory for query buffer" << std::endl;
			exit(EXIT_FAILURE);
		}

		mapping_ = modbus_mapping_new(MD_COILS_MAX, MD_INPUT_COILS_MAX, MD_REGISTERS_MAX, MD_INPUT_REGISTERS_MAX);
		if (!mapping_)
		{
			std::cerr << "Failed to allocate Modbus mapping: " << modbus_strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}

		if (modbus_connect(ctx_) == -1)
		{
			std::cerr << "Connection failed: " << modbus_strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
	}

private:

	void run()
	{
		while (running_)
		{
			ret = modbus_receive(ctx_, query_);
			if (ret == -1)
			{
				std::cerr << "Receive error: " << modbus_strerror(errno) << std::endl;
				break;
			}
			else
			{
				processRequest(ret);
			}
		}
	}

	void processRequest(int ret)
	{
		if (ret >= 8)
		{
			uint8_t slaveId = query_[0];
			uint8_t function_code = query_[1];
			uint16_t startAddress = (query_[2] << 8) | query_[3];
			uint16_t endAddress = (query_[4] << 8) | query_[5];

			switch (function_code)
			{
			case MODBUS_FC_READ_COILS:
				handleReadCoils(startAddress);
				break;
			case MODBUS_FC_READ_HOLDING_REGISTERS:
				handleReadHoldingRegisters(startAddress);
				break;
			case MODBUS_FC_WRITE_MULTIPLE_COILS:
				handleWriteMultipleCoils(startAddress, endAddress);
				break;
			case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
				handleWriteMultipleRegisters(startAddress, endAddress);
				break;
			default:
				modbus_reply_exception(ctx_, query_, MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
				break;
			}
		}
	}

	void handleReadCoils(uint16_t startAddress)
	{
		if (startAddress >= 0 && startAddress < MD_COILS_MAX)
		{
			for (int i = 0; i < 10; ++i)
			{
				mapping_->tab_bits[startAddress + i] = std::rand() % 2; // 随机生成
			}
			modbus_reply(ctx_, query_, ret, mapping_);
		}
		else
		{
			modbus_reply_exception(ctx_, query_, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
		}
	}

	void handleReadHoldingRegisters(uint16_t startAddress)
	{
		if (startAddress >= 0 && startAddress < MD_REGISTERS_MAX)
		{
			for (int i = 1; i <= MD_REGISTERS_MAX; i++)
			{
				mapping_->tab_registers[i - 1] = std::rand() % 30; // 随机生成
			}
			modbus_reply(ctx_, query_, ret, mapping_);
		}
		else
		{
			modbus_reply_exception(ctx_, query_, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
		}
	}

	void handleWriteMultipleCoils(uint16_t startAddress, uint16_t endAddress)
	{
		if (startAddress >= 0 && startAddress < MD_COILS_MAX)
		{

			modbus_reply(ctx_, query_, ret, mapping_);
			for (int i = 0; i < endAddress; i++)
			{
				int bitValue = mapping_->tab_bits[startAddress + i];
				std::cout << "Coil " << startAddress + i << ": " << bitValue << std::endl;
			}
			std::cout << "=================================\n";
			//std::this_thread::sleep_for(std::chrono::seconds(3));
		}
		else
		{
			modbus_reply_exception(ctx_, query_, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
		}
	}


	void handleWriteMultipleRegisters(uint16_t startAddress, uint16_t endAddress)
	{
		if (startAddress >= 0 && startAddress < MD_REGISTERS_MAX)
		{
			modbus_reply(ctx_, query_, ret, mapping_);
			for (int i = 0; i < endAddress; i++)
			{
				int registerValue = mapping_->tab_registers[startAddress + i];
				std::cout << "Register " << startAddress + i << ": " << registerValue << std::endl;
			}
			std::cout << "=================================\n";
			//std::this_thread::sleep_for(std::chrono::seconds(3));
		}
		else
		{
			modbus_reply_exception(ctx_, query_, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
		}
	}
};




static uint16_t crc16(uint8_t* buffer, uint16_t buffer_length)
{
	uint8_t crc_hi = 0xFF;	/*已初始化高CRC字节*/
	uint8_t crc_lo = 0xFF;	/*低CRC字节已初始化*/
	unsigned int i;			/*将索引到CRC查找中*/

	/* pass through message buffer */
	while (buffer_length--)
	{
		i = crc_hi ^ *buffer++; /* calculate the CRC  */
		crc_hi = crc_lo ^ table_crc_hi[i];
		crc_lo = table_crc_lo[i];
	}

	uint16_t res_crc = (crc_hi << 8 | crc_lo);
	return res_crc;
}














//class ModbusSlave
//{
//private:
//	modbus_t* context;
//	uint8_t* queryBuffer;
//	modbus_mapping_t* mapping;
//	const uint16_t registersNbMax;
//	const uint8_t serverId;
//
//	/// <summary>
//	/// 写入数据到寄存器,
//	/// 对应主机功能码为度寄存器(03),主机是读,这里就是写
//	/// </summary>
//	/// <param name="startAddress"></param>
//	/// <param name="mapping"></param>
//	void writeHoldingRegisters(uint16_t startAddress, modbus_mapping_t* mapping)
//	{
//		if (startAddress >= 0 && startAddress < registersNbMax)
//		{
//			modbus_reply(context, queryBuffer, 8, mapping);
//		}
//		else
//		{
//			modbus_reply_exception(context, queryBuffer, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
//		}
//	}
//
//	// 预留的函数框架
//	void writeCoil(uint16_t startAddress, bool value)
//	{
//
//	}
//
//	uint16_t readHoldingRegister(uint16_t registerAddress)
//	{
//		// 实现读寄存器功能
//		return 0;
//	}
//
//	bool readDiscreteInput(uint16_t inputAddress)
//	{
//		// 实现读线圈功能
//		return false;
//	}
//
//public:
//
//	ModbusSlave(uint8_t serverId, uint16_t registersNbMax)
//		: serverId(serverId), registersNbMax(registersNbMax), context(nullptr), queryBuffer(nullptr), mapping(nullptr) {}
//
//	~ModbusSlave()
//	{
//		this->cleanup();
//	}
//
//	void initialize(const char* port, int baudrate, char parity, int databits, int stopbits)
//	{
//		if (!context)
//		{
//			context = modbus_new_rtu(port, baudrate, parity, databits, stopbits);
//			if (!context)
//			{
//				std::cout << "Failed to create Modbus context: " << modbus_strerror(errno) << std::endl;
//				return;
//			}
//			modbus_set_slave(context, serverId);
//			queryBuffer = (uint8_t*)malloc(MODBUS_RTU_MAX_ADU_LENGTH);
//			if (!queryBuffer)
//			{
//				std::cout << "Failed to allocate memory for query buffer" << std::endl;
//				return;
//			}
//			mapping = modbus_mapping_new(0, 0, registersNbMax, 0);
//			if (!mapping)
//			{
//				std::cout << "Failed to allocate Modbus mapping: " << modbus_strerror(errno) << std::endl;
//				return;
//			}
//			if (modbus_connect(context) == -1)
//			{
//				std::cout << "Connection failed: " << modbus_strerror(errno) << std::endl;
//				return;
//			}
//		}
//	}
//
//	void handleRequests()
//	{
//		while (true)
//		{
//			int ret = modbus_receive(context, queryBuffer);
//			if (ret == -1)
//			{
//				std::cout << "Receive error: " << modbus_strerror(errno) << std::endl;
//				break;
//			}
//			else
//			{
//				handleRequest(queryBuffer, ret);
//			}
//		}
//	}
//
//	void handleRequest(uint8_t* request, int length)
//	{
//		if (length >= 8)
//		{
//
//			//uint8_t functionCode = request[1];
//			//uint16_t address = (request[2] << 8) | request[3];
//
//			uint8_t slaveId = request[0];
//			uint8_t function_code = request[1]; // 获取Modbus功能码
//			uint16_t startAddress = (request[2] << 8) | request[3]; // 获取寄存器地址
//			uint16_t endAddress = (request[4] << 8) | request[5]; // 获取写入的值
//			uint16_t crc = (request[6] << 8) | request[7]; // 获取写入的值
//
//			for (int i = 1; i <= registersNbMax; i++)
//			{
//				mapping->tab_registers[i - 1] = i;
//			}
//
//			switch (function_code)
//			{
//			case MODBUS_FC_READ_COILS:
//				// 调用writeCoil函数
//
//				break;
//			case MODBUS_FC_READ_HOLDING_REGISTERS:   // 主机是03读取寄存器,这里就应该是写寄存器 
//				writeHoldingRegisters(startAddress, mapping);
//				break;
//			case MODBUS_FC_WRITE_SINGLE_REGISTER:
//				// 调用readHoldingRegister函数
//				break;
//			case MODBUS_FC_READ_DISCRETE_INPUTS:
//				// 调用readDiscreteInput函数
//				break;
//			default:
//				modbus_reply_exception(context, request, MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
//				break;
//			}
//		}
//	}
//
//	void cleanup()
//	{
//		if (mapping)
//		{
//			modbus_mapping_free(mapping);
//		}
//		if (queryBuffer)
//		{
//			free(queryBuffer);
//		}
//		if (context)
//		{
//			modbus_close(context);
//			modbus_free(context);
//		}
//	}
//
//};






