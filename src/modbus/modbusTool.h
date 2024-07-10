
#include <iostream>
#include <string>
#include <vector>
#include <chrono> // 操作时间
#include <thread>
#include <modbus.h>


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
	void pollWriteRegisters(int slave_id, int start_addr, int num_regs)
	{
		uint16_t tab_reg[32];     // 用于存储读取到的寄存器数据
		// 打开串口设备
		if (modbus_connect(ctx) == -1)
		{
			std::cout << "Connection failed: " << modbus_strerror(errno) << std::endl;
			throw std::runtime_error("Connection failed");
		}

		// 设置从机地址
		modbus_set_slave(ctx, slave_id);

		// 循环写入寄存器数据
		while (true)
		{
			// 写入数据到寄存器
			for (int i = 0; i < num_regs; ++i)
			{
				tab_reg[i] = i + 1; // 模拟写入数据，这里写入了1到10
			}

			int rc = modbus_write_registers(ctx, start_addr, num_regs, tab_reg);
			if (rc == -1)
			{
				std::cout << "Write registers failed: " << modbus_strerror(errno) << std::endl;
				continue; // 跳出循环或者处理错误
			}

			std::cout << "Registers written successfully." << std::endl;

			// 可选：等待一段时间后再次写入
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}
	}

	/// <summary>
	/// 主模式 读取从机寄存器数据
	/// 查看使用 modbus slave: 
	/// </summary>
	/// <param name="slave_id">Modbus从机地址</param>
	/// <param name="start_addr">起始寄存器地址</param>
	/// <param name="num_regs">要读取或写入的寄存器数量</param>
	void pollReadRegisters(int slave_id, int start_addr, int num_regs)
	{
		uint16_t tab_reg[32];     // 用于存储读取到的寄存器数据
		// 打开串口设备
		if (modbus_connect(ctx) == -1)
		{
			std::cout << "Connection failed: " << modbus_strerror(errno) << std::endl;
			throw std::runtime_error("Connection failed");
		}

		// 设置从机地址
		modbus_set_slave(ctx, slave_id);

		// 循环读取寄存器数据
		while (true)
		{
			int rc = modbus_read_registers(ctx, start_addr, num_regs, tab_reg);
			if (rc == -1)
			{
				std::cout << "Read registers failed: " << modbus_strerror(errno) << std::endl;
				continue; // 跳出循环或者处理错误
			}

			// 打印读取到的寄存器数据
			std::cout << "Registers read:" << std::endl;
			for (int i = 0; i < num_regs; ++i)
			{
				std::cout << "Register " << start_addr + i << ": " << tab_reg[i] << std::endl;
			}

			// 可选：等待一段时间后再次读取
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}
	}

	/// <summary>
	/// 主模式 写入线圈数据到从机线圈
	/// 查看使用 modbus slave: 
	/// </summary>
	/// <param name="slave_id"></param>
	/// <param name="coil_addr"></param>
	/// <param name="num_coils"></param>
	void pollWriteCoils(int slave_id, int coil_addr, int num_coils)
	{
		uint8_t tab_coils[5] = { 1, 0, 1, 1, 0 }; // 示例数据
		// 打开串口设备
		if (modbus_connect(ctx) == -1)
		{
			std::cout << "Connection failed: " << modbus_strerror(errno) << std::endl;
			modbus_free(ctx);
		}

		// 设置从机地址
		modbus_set_slave(ctx, slave_id);
		while (true)
		{
			// 写入线圈数据
			int rc = modbus_write_bits(ctx, coil_addr, num_coils, tab_coils);
			if (rc == -1)
			{
				std::cout << "Write coil failed: " << modbus_strerror(errno) << std::endl;
			}
			else
			{
				std::cout << "Registers written successfully." << std::endl;
			}

			// 等待一段时间后再关闭连接
			std::this_thread::sleep_for(std::chrono::seconds(1));

		}

		//// 关闭Modbus连接
		//modbus_close(ctx);
		//modbus_free(ctx);
	}


	/// <summary>
	/// 主模式 读取从机线圈数据
	/// 查看使用 modbus slave: 
	/// </summary>
	/// <param name="slave_id">Modbus从机地址</param>
	/// <param name="start_addr">起始线圈地址</param>
	/// <param name="num_coils">要读取或写入的线圈数量</param>
	void pollReadCoils(int slave_id, int coil_addr, int num_coils)
	{
		uint8_t tab_coils[20];
		// 打开串口设备
		if (modbus_connect(ctx) == -1) {
			std::cout << "Connection failed: " << modbus_strerror(errno) << std::endl;
			modbus_free(ctx);
		}

		// 设置从机地址
		modbus_set_slave(ctx, slave_id);

		// 循环读取线圈数据
		while (true) {
			int rc = modbus_read_bits(ctx, coil_addr, num_coils, tab_coils);
			if (rc == -1)
			{
				std::cout << "Read bits failed: " << modbus_strerror(errno) << std::endl;
				continue; // 跳出循环或者处理错误
			}

			// 打印读取到的线圈数据
			std::cout << "Coils read:" << std::endl;
			for (int i = 0; i < num_coils; ++i) {
				std::cout << "Coil " << coil_addr + i << ": " << static_cast<int>(tab_coils[i]) << std::endl;
			}

			// 可选：等待一段时间后再次读取
			std::this_thread::sleep_for(std::chrono::seconds(3));
		}

		//// 关闭Modbus连接
		//modbus_close(ctx);
		//modbus_free(ctx);
	}



};



class ModbusSlave 
{
private:
	modbus_t* context;
	uint8_t* queryBuffer;
	modbus_mapping_t* mapping;
	const uint16_t registersNbMax;
	const uint8_t slaveId;

	/// <summary>
	/// 写入数据到寄存器
	/// 对应主机功能码为度寄存器(03),主机是读,这里就是写
	/// </summary>
	/// <param name="startAddress"></param>
	/// <param name="mapping"></param>
	void writeHoldingRegisters(uint16_t startAddress, modbus_mapping_t* mapping)
	{
		if (startAddress >= 0 && startAddress < registersNbMax)
		{
			modbus_reply(context, queryBuffer, 8, mapping);
		}
		else 
		{
			modbus_reply_exception(context, queryBuffer, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
		}
	}

	// 预留的函数框架
	void writeCoil(uint16_t startAddress, bool value)
	{

	}

	uint16_t readHoldingRegister(uint16_t registerAddress) 
	{
		// 实现读寄存器功能
		return 0;
	}

	bool readDiscreteInput(uint16_t inputAddress) 
	{
		// 实现读线圈功能
		return false;
	}

public:

	ModbusSlave(uint8_t serverId, uint16_t registersNbMax)
		: serverId(serverId), registersNbMax(registersNbMax), context(nullptr), queryBuffer(nullptr), mapping(nullptr) {}

	~ModbusSlave() 
	{
		this->cleanup();
	}

	void initialize(const char* port, int baudrate, char parity, int databits, int stopbits)
	{
		if (!context)
		{
			context = modbus_new_rtu(port, baudrate, parity, databits, stopbits);
			if (!context)
			{
				std::cerr << "Failed to create Modbus context: " << modbus_strerror(errno) << std::endl;
				return;
			}
			modbus_set_slave(context, serverId);
			queryBuffer = (uint8_t*)malloc(MODBUS_RTU_MAX_ADU_LENGTH);
			if (!queryBuffer)
			{
				std::cerr << "Failed to allocate memory for query buffer" << std::endl;
				return;
			}
			mapping = modbus_mapping_new(0, 0, registersNbMax, 0);
			if (!mapping)
			{
				std::cerr << "Failed to allocate Modbus mapping: " << modbus_strerror(errno) << std::endl;
				return;
			}
			if (modbus_connect(context) == -1)
			{
				std::cerr << "Connection failed: " << modbus_strerror(errno) << std::endl;
				return;
			}
		}
	}

	void handleRequests()
	{
		while (true)
		{
			int ret = modbus_receive(context, queryBuffer);
			if (ret == -1)
			{
				std::cerr << "Receive error: " << modbus_strerror(errno) << std::endl;
				break;
			}
			else
			{
				handleRequest(queryBuffer, ret);
			}
		}
	}

	void handleRequest(uint8_t* request, int length) 
	{
		if (length >= 8) 
		{
			
			//uint8_t functionCode = request[1];
			//uint16_t address = (request[2] << 8) | request[3];

			slaveId = request[0];
			uint8_t function_code = request[1]; // 获取Modbus功能码
			uint16_t startAddress = (request[2] << 8) | request[3]; // 获取寄存器地址
			uint16_t endAddress = (request[4] << 8) | request[5]; // 获取写入的值
			uint16_t crc = (request[6] << 8) | request[7]; // 获取写入的值

			for (int i = 1; i <= registersNbMax; i++)
			{
				mapping->tab_registers[i - 1] = i;
			}

			switch (function_code) {
			case MODBUS_FC_READ_COILS:
				// 调用writeCoil函数

				break;
			case MODBUS_FC_READ_HOLDING_REGISTERS:   // 主机是03读取寄存器,这里就应该是写寄存器 
				writeHoldingRegisters(startAddress, mapping);
				break;
			case MODBUS_FC_WRITE_SINGLE_REGISTER:
				// 调用readHoldingRegister函数
				break;
			case MODBUS_FC_READ_DISCRETE_INPUTS:
				// 调用readDiscreteInput函数
				break;
			default:
				modbus_reply_exception(context, request, MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
				break;
			}
		}
	}

	void cleanup() 
	{
		if (mapping)
		{
			modbus_mapping_free(mapping);
		}
		if (queryBuffer)
		{
			free(queryBuffer);
		}
		if (context)
		{
			modbus_close(context);
			modbus_free(context);
		}
	}

};






