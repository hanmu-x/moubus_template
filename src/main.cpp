
#include "modbus/modbusTool.h"
#include "modbus/config.hpp"

#include <filesystem>

#include <iostream>
#include <modbus.h>

#define SERVER_ID 1
#define UT_REGISTERS_ADDRESS 0
#define UT_REGISTERS_NB_MAX 10



#include <thread>

// 假设ModbusSlave类和其他必要的头文件已经被包含

int main() 
{
    // 创建ModbusSlave实例，服务器ID为1，最多有100个寄存器
    ModbusSlave slave(1, 100);

    // 初始化Modbus上下文，连接到串口"COM6"，波特率9600，数据位8，无奇偶校验，停止位1
    slave.initialize("COM6", 9600, 'N', 8, 1);

    // 启动一个线程来处理Modbus请求
    slave.handleRequests();

    return 0;
}










//int main()
//{
//    modbus_t* ctx = NULL;
//    uint8_t* query = NULL;
//    modbus_mapping_t* mapping = NULL;
//    int ret;
//
//    // 初始化Modbus上下文为RTU模式，连接到串口"COM6"，波特率9600，数据位8，无奇偶校验，停止位1
//    ctx = modbus_new_rtu("COM6", 9600, 'N', 8, 1);
//    if (ctx == NULL) 
//    {
//        fprintf(stderr, "Failed to create Modbus context: %s\n", modbus_strerror(errno));
//        return EXIT_FAILURE;
//    }
//
//    // 设置从机地址
//    modbus_set_slave(ctx, SERVER_ID);
//
//    // 分配查询缓冲区
//    query = (uint8_t*)malloc(MODBUS_RTU_MAX_ADU_LENGTH);
//    if (query == NULL) 
//    {
//        fprintf(stderr, "Failed to allocate memory for query buffer\n");
//        modbus_free(ctx);
//        return EXIT_FAILURE;
//    }
//
//    // 创建Modbus映射，定义了保持寄存器的数量为UT_REGISTERS_NB_MAX
//    mapping = modbus_mapping_new(0, 0, UT_REGISTERS_NB_MAX, 0);
//    if (mapping == NULL) 
//    {
//        fprintf(stderr, "Failed to allocate Modbus mapping: %s\n", modbus_strerror(errno));
//        free(query);
//        modbus_free(ctx);
//        return EXIT_FAILURE;
//    }
//
//    // 连接到串行设备
//    if (modbus_connect(ctx) == -1) 
//    {
//        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
//        modbus_mapping_free(mapping);
//        free(query);
//        modbus_free(ctx);
//        return EXIT_FAILURE;
//    }
//
//    // 主循环处理请求
//    while (1) 
//    {
//        // 接收Modbus请求
//        ret = modbus_receive(ctx, query);
//        if (ret == -1) 
//        {
//            fprintf(stderr, "Receive error: %s\n", modbus_strerror(errno));
//            break;
//        }
//        else 
//        {
//            //printf("Received bytes: %d\n", ret);
//            //for (int i = 0; i < ret; i++)
//            //{
//            //    printf("%x ", query[i]);
//            //}
//            //std::cout << std::endl;
//
//            uint8_t slaveId;
//            uint8_t function_code;
//            uint16_t startAddress; // 获取寄存器地址
//            uint16_t endAddress; // 获取写入的值
//            uint16_t crc; // 获取写入的值
//            if (ret >= 8)
//            {
//                slaveId = query[0];
//                function_code = query[1]; // 获取Modbus功能码
//                startAddress = (query[2] << 8) | query[3]; // 获取寄存器地址
//                endAddress = (query[4] << 8) | query[5]; // 获取写入的值
//                crc = (query[6] << 8) | query[7]; // 获取写入的值
//            }
//
//            switch (function_code) 
//            {
//            case MODBUS_FC_READ_HOLDING_REGISTERS: // 0x03 // // 主机是03读取寄存器,这里就应该是写寄存器 
//                
//                // 写入保持寄存器
//                // 检查地址是否在有效范围内
//                if (startAddress >= 0 && startAddress < UT_REGISTERS_NB_MAX)
//                {
//                    // 将0~9 写入1~10
//                    for (int i = 1; i <= UT_REGISTERS_NB_MAX; i++)
//                    {
//                        mapping->tab_registers[i-1] = i;
//                    }
//                    // 发送响应
//                    modbus_reply(ctx, query, ret, mapping);
//                }
//                else 
//                {
//                    // 发送非法数据地址错误响应
//                    modbus_reply_exception(ctx, query, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
//                }
//                break;
//
//            case MODBUS_FC_WRITE_SINGLE_REGISTER: // 0x06
//                
//                // 获取读取寄存器的起始地址
//                if (startAddress >= 0 && startAddress < UT_REGISTERS_NB_MAX)
//                {
//                    // 读取保持寄存器的值
//                    uint16_t num_registers = (query[4] << 8) | query[5]; // 要读取的寄存器数量
//                    uint8_t response_length = 1 + num_registers * 2; // 响应长度
//
//                    // 确保请求的寄存器数量在有效范围内
//                    if (num_registers > 0 && num_registers <= UT_REGISTERS_NB_MAX - startAddress)
//                    {
//                        // 构造响应报文
//                        query[2] = response_length - 1; // 更新响应长度字段
//                        query[3] = num_registers * 2; // 更新数据长度字段
//
//                        // 依次将寄存器值写入响应报文
//                        for (int i = 0; i < num_registers; ++i) {
//                            uint16_t reg_value = mapping->tab_registers[startAddress + i];
//                            query[3 + i * 2 + 1] = reg_value >> 8; // 高字节
//                            query[3 + i * 2 + 2] = reg_value & 0xFF; // 低字节
//                        }
//
//                        // 发送响应
//                        modbus_reply(ctx, query, ret, mapping);
//                    }
//                    else {
//                        // 发送非法数据地址错误响应
//                        modbus_reply_exception(ctx, query, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
//                    }
//                }
//                else {
//                    // 发送非法数据地址错误响应
//                    modbus_reply_exception(ctx, query, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
//                }
//                break;
//            default:
//                // 发送不支持功能码错误响应
//                modbus_reply_exception(ctx, query, MODBUS_EXCEPTION_ILLEGAL_FUNCTION);
//                break;
//            }
//        }
//    }
//
//
//    // 清理
//    modbus_mapping_free(mapping);
//    free(query);
//    modbus_close(ctx);
//    modbus_free(ctx);
//
//    return EXIT_SUCCESS;
//}







//int main()
//{
//    try
//    {
//        const int slave_id = 1;   // Modbus从机地址
//        const int start_addr = 0; // 起始寄存器地址
//        const int num_regs = 10;  // 要读取或写入的寄存器数量
//        ModbusPoll tool("COM6", 9600, 'N', 8, 1);
//
//        // 写线圈
//        tool.pollWriteCoils(slave_id, 0, 5);
//
//        // 读取线圈示例
//        uint8_t read_tab_coils[5]; // 读取结果存储数组
//        //tool.pollReadCoils(slave_id, 0, 5); // 读取从地址 0 开始的 5 个线圈
//
//        //// 主模式 写入数据到从机寄存器
//        //tool.pollWriteRegisters(slave_id, start_addr, num_regs);
//
//        //// 主模式 读取从机寄存器数据
//        //tool.pollReadRegisters(slave_id, start_addr, num_regs);
//        ////Registers read :
//        ////Register 0 : 1
//        ////Register 1 : 2
//        ////Register 2 : 3
//        ////Register 3 : 4
//        ////Register 4 : 5
//        ////Register 5 : 6
//        ////Register 6 : 7
//        ////Register 7 : 8
//        ////Register 8 : 9
//        ////Register 9 : 10
//
//    }
//    catch (const std::exception& e)
//    {
//        std::cout << "Exception caught: " << e.what() << std::endl;
//        return 1;
//    }
//
//    return 0;
//}


















