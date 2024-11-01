
#include "modbusTool.h"



void ModbusPoll::pollWriteRegisters(int slave_id, int start_addr, int num_regs)
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
			tab_reg[i] = std::rand() % 100; // 随机生成; // 模拟写入数据
		}

		int rc = modbus_write_registers(ctx, start_addr, num_regs, tab_reg);
		if (rc == -1)
		{
			std::cout << "Write registers failed: " << modbus_strerror(errno) << std::endl;
			continue; // 跳出循环或者处理错误
		}

		std::cout << "Registers written successfully." << std::endl;

		// 可选：等待一段时间后再次写入
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}


void ModbusPoll::pollReadRegisters(int slave_id, int start_addr, int num_regs)
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
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}


void ModbusPoll::pollWriteCoils(int slave_id, int coil_addr, int num_coils)
{
	uint8_t tab_coils[20]; // 示例数据
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
		for (int i = 0; i < 10; ++i)
		{
			tab_coils[i] = std::rand() % 2; // 随机生成
		}
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


void ModbusPoll::pollReadCoils(int slave_id, int coil_addr, int num_coils)
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
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	//// 关闭Modbus连接
	//modbus_close(ctx);
	//modbus_free(ctx);
}





















