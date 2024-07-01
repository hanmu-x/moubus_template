
#include "modbus/modbusTool.h"
#include "modbus/config.hpp"

#include <filesystem>

#include <iostream>
#include <modbus.h>


int main()
{
    try
    {
        const int slave_id = 1;   // Modbus从机地址
        const int start_addr = 0; // 起始寄存器地址
        const int num_regs = 10;  // 要读取或写入的寄存器数量
        ModbusTool tool("COM6", 9600, 'N', 8, 1);

        // 写线圈
        tool.pollWriteCoils(slave_id, 0, 5);

        // 读取线圈示例
        uint8_t read_tab_coils[5]; // 读取结果存储数组
        //tool.pollReadCoils(slave_id, 0, 5); // 读取从地址 0 开始的 5 个线圈

        //// 主模式 写入数据到从机寄存器
        //tool.pollWriteRegisters(slave_id, start_addr, num_regs);

        //// 主模式 读取从机寄存器数据
        //tool.pollReadRegisters(slave_id, start_addr, num_regs);
        ////Registers read :
        ////Register 0 : 1
        ////Register 1 : 2
        ////Register 2 : 3
        ////Register 3 : 4
        ////Register 4 : 5
        ////Register 5 : 6
        ////Register 6 : 7
        ////Register 7 : 8
        ////Register 8 : 9
        ////Register 9 : 10

    }
    catch (const std::exception& e)
    {
        std::cout << "Exception caught: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}







////// 主模式(服务端写入数据到从机寄存器)
//int main() 
//{
//    modbus_t* ctx;
//    int rc;
//    const int slave_id = 1; // Modbus从机地址
//    const int start_addr = 0; // 起始寄存器地址
//    const int num_regs = 10; // 要读取的寄存器数量
//    uint16_t tab_reg[32]; // 用于存储读取到的寄存器数据
//
//    // 创建Modbus RTU上下文
//    ctx = modbus_new_rtu("COM6", 9600, 'N', 8, 1);
//    if (ctx == NULL) {
//        std::cout << "Unable to create the libmodbus context" << std::endl;
//        return 1;
//    }
//
//    // 打开串口设备
//    if (modbus_connect(ctx) == -1) {
//        std::cout << "Connection failed: " << modbus_strerror(errno) << std::endl;
//        modbus_free(ctx);
//        return 1;
//    }
//
//    // 设置从机地址
//    modbus_set_slave(ctx, slave_id);
//
//    // 循环读取寄存器数据
//    while (true)
//    {
//        // 写入数据到寄存器
//        for (int i = 0; i < num_regs; ++i) {
//            tab_reg[i] = i + 1; // 模拟写入数据，这里写入了1到10
//        }
//
//        rc = modbus_write_registers(ctx, start_addr, num_regs, tab_reg);
//        if (rc == -1)
//        {
//            std::cout << "Write registers failed: " << modbus_strerror(errno) << std::endl;
//            continue; // 跳出循环或者处理错误
//        }
//
//        std::cout << "Registers written successfully." << std::endl;
//
//        // 可选：等待一段时间后再次写入
//        std::this_thread::sleep_for(std::chrono::seconds(3));
//    }
//
//    // 关闭Modbus连接
//    modbus_close(ctx);
//    modbus_free(ctx);
//
//    return 0;
//}


//// 主模式(服务端读取客户端线圈地址)
//int main() {
//    modbus_t* ctx;
//    int rc;
//    uint16_t tab_reg[32]; // 用于存储读取到的寄存器数据
//    const int slave_id = 1; // Modbus从机地址
//    const int start_addr = 0; // 起始寄存器地址
//    const int num_regs = 10; // 要读取的寄存器数量
//
//    // 创建Modbus RTU上下文
//    ctx = modbus_new_rtu("COM6", 9600, 'N', 8, 1);
//    if (ctx == NULL) {
//        std::cout << "Unable to create the libmodbus context" << std::endl;
//        return 1;
//    }
//
//    // 打开串口设备
//    if (modbus_connect(ctx) == -1) {
//        std::cout << "Connection failed: " << modbus_strerror(errno) << std::endl;
//        modbus_free(ctx);
//        return 1;
//    }
//
//    // 设置从机地址
//    modbus_set_slave(ctx, slave_id);
//
//    // 循环读取寄存器数据
//    while (true) 
//    {
//        rc = modbus_read_registers(ctx, start_addr, num_regs, tab_reg);
//        if (rc == -1) 
//        {
//            std::cout << "Read registers failed: " << modbus_strerror(errno) << std::endl;
//            continue; // 跳出循环或者处理错误
//        }
//
//        // 打印读取到的寄存器数据
//        std::cout << "Registers read:" << std::endl;
//        for (int i = 0; i < num_regs; ++i) {
//            std::cout << "Register " << start_addr + i << ": " << tab_reg[i] << std::endl;
//        }
//
//        // 可选：等待一段时间后再次读取
//        std::this_thread::sleep_for(std::chrono::seconds(3));
//    }
//
//    // 关闭Modbus连接
//    modbus_close(ctx);
//    modbus_free(ctx);
//
//    return 0;
//}









//int main()
//{
//
//	Config config;
//#ifndef NDEBUG
//	std::string configPath = "../../../../Config/my_config.json";
//#else
//	std::string configPath = "./my_config.json";
//#endif
//    if (config.read_config(configPath))
//    {
//        std::cout << "Read config file succession " << std::endl;
//    }
//    else
//    {
//        std::cout << "ERROR : Failed to read config file " << std::endl;
//        return 1;
//    }
//
//    std::filesystem::path data_1(DEFAULT_DATA_DIR);
//    data_1 += "/geo_db/example6.db";
//
//	modbus_tool tc;
//
//
//	return 0;
//}