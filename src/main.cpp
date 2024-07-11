
#include "modbus/modbusTool.h"
#include "modbus/config.hpp"

#include <filesystem>

#include <iostream>
#include <modbus.h>
#include <errno.h>



#include <thread>
#include <chrono> // 操作时间

// 从机测试
int main() 
{
    // 创建ModbusSlave实例，服务器ID为1，最多有100个寄存器
    ModbusSlave slave(1, 100);

    // 初始化Modbus上下文，连接到串口"COM6"，波特率9600，数据位8，无奇偶校验，停止位1
    slave.initialize("COM6", 9600, 'N', 8, 1);

    slave.start();
    // Keep the program running until interrupted.
    std::cin.get();
    slave.stop();

    return 0;
}


//#define SERVER_ID 1
//#define MD_COILS_MAX 10
//#define MD_INPUT_COILS_MAX 10
//#define MD_REGISTERS_MAX 10
//#define MD_INPUT_REGISTERS_MAX 10
//
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
//    // 创建Modbus映射，定义了保持寄存器的数量为MD_REGISTERS_MAX
//    mapping = modbus_mapping_new(MD_COILS_MAX, MD_INPUT_COILS_MAX, MD_REGISTERS_MAX, MD_INPUT_REGISTERS_MAX);
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
//            case MODBUS_FC_READ_COILS:  // 主机为01读线圈, 这里从机写线圈
//                if (startAddress >= 0 && startAddress < MD_COILS_MAX)
//                {
//                    if (startAddress == 0 && endAddress >= 10)
//                    {
//                        // 线圈数值: 0101010101
//                        for (int i = 0; i < 10; ++i) 
//                        {
//                            mapping->tab_bits[startAddress + i] = i % 2;
//                        }
//                    }
//                    modbus_reply(ctx, query, ret, mapping);
//                }
//                else {
//                    modbus_reply_exception(ctx, query, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
//                }
//                break;
//            case MODBUS_FC_READ_HOLDING_REGISTERS: // 主机是03读取寄存器,这里从机写寄存器 
//                
//                // 写入保持寄存器
//                // 检查地址是否在有效范围内
//                if (startAddress >= 0 && startAddress < MD_REGISTERS_MAX)
//                {
//                    // 将0~9 写入1~10
//                    for (int i = 1; i <= MD_REGISTERS_MAX; i++)
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
//            case MODBUS_FC_WRITE_MULTIPLE_COILS:
//                if (startAddress >= 0 && startAddress < MD_COILS_MAX)
//                {
//                    (ctx, query, ret, mapping);
//                    // 这里我们不执行实际的写入操作，而是直接读取线圈状态并打印
//                    for (int i = 0; i < endAddress; i++)
//                    {
//                        int bitValue = mapping->tab_bits[startAddress + i];
//                        printf("Coil %d: %d\n", startAddress + i, bitValue);
//                    }
//                    printf("=================================\n");
//                    std::this_thread::sleep_for(std::chrono::seconds(3));
//                }
//                else
//                {
//                    modbus_reply_exception(ctx, query, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS);
//                }
//                break;
//            case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
//                if (startAddress >= 0 && startAddress < MD_COILS_MAX)
//                {
//                    modbus_reply(ctx, query, ret, mapping);
//                    // 这里我们不执行实际的写入操作，而是直接读取线圈状态并打印
//                    for (int i = 0; i < endAddress; i++)
//                    {
//                        int bitValue = mapping->tab_registers[startAddress + i];
//                        printf("Register %d: %d\n", startAddress + i, bitValue);
//                    }
//                    printf("=================================\n");
//                    std::this_thread::sleep_for(std::chrono::seconds(3));
//                }
//                else
//                {
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






// 主机模式测试
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










/**
 *               ii.                                         ;9ABH,
 *              SA391,                                    .r9GG35&G
 *              &#ii13Gh;                               i3X31i;:,rB1
 *              iMs,:,i5895,                         .5G91:,:;:s1:8A
 *               33::::,,;5G5,                     ,58Si,,:::,sHX;iH1
 *                Sr.,:;rs13BBX35hh11511h5Shhh5S3GAXS:.,,::,,1AG3i,GG
 *                .G51S511sr;;iiiishS8G89Shsrrsh59S;.,,,,,..5A85Si,h8
 *               :SB9s:,............................,,,.,,,SASh53h,1G.
 *            .r18S;..,,,,,,,,,,,,,,,,,,,,,,,,,,,,,....,,.1H315199,rX,
 *          ;S89s,..,,,,,,,,,,,,,,,,,,,,,,,....,,.......,,,;r1ShS8,;Xi
 *        i55s:.........,,,,,,,,,,,,,,,,.,,,......,.....,,....r9&5.:X1
 *       59;.....,.     .,,,,,,,,,,,...        .............,..:1;.:&s
 *      s8,..;53S5S3s.   .,,,,,,,.,..      i15S5h1:.........,,,..,,:99
 *      93.:39s:rSGB@A;  ..,,,,.....    .SG3hhh9G&BGi..,,,,,,,,,,,,.,83
 *      G5.G8  9#@@@@@X. .,,,,,,.....  iA9,.S&B###@@Mr...,,,,,,,,..,.;Xh
 *      Gs.X8 S@@@@@@@B:..,,,,,,,,,,. rA1 ,A@@@@@@@@@H:........,,,,,,.iX:
 *     ;9. ,8A#@@@@@@#5,.,,,,,,,,,... 9A. 8@@@@@@@@@@M;    ....,,,,,,,,S8
 *     X3    iS8XAHH8s.,,,,,,,,,,...,..58hH@@@@@@@@@Hs       ...,,,,,,,:Gs
 *    r8,        ,,,...,,,,,,,,,,.....  ,h8XABMMHX3r.          .,,,,,,,.rX:
 *   :9, .    .:,..,:;;;::,.,,,,,..          .,,.               ..,,,,,,.59
 *  .Si      ,:.i8HBMMMMMB&5,....                    .            .,,,,,.sMr
 *  SS       :: h@@@@@@@@@@#; .                     ...  .         ..,,,,iM5
 *  91  .    ;:.,1&@@@@@@MXs.                            .          .,,:,:&S
 *  hS ....  .:;,,,i3MMS1;..,..... .  .     ...                     ..,:,.99
 *  ,8; ..... .,:,..,8Ms:;,,,...                                     .,::.83
 *   s&: ....  .sS553B@@HX3s;,.    .,;13h.                            .:::&1
 *    SXr  .  ...;s3G99XA&X88Shss11155hi.                             ,;:h&,
 *     iH8:  . ..   ,;iiii;,::,,,,,.                                 .;irHA
 *      ,8X5;   .     .......                                       ,;iihS8Gi
 *         1831,                                                 .,;irrrrrs&@
 *           ;5A8r.                                            .:;iiiiirrss1H
 *             :X@H3s.......                                .,:;iii;iiiiirsrh
 *              r#h:;,...,,.. .,,:;;;;;:::,...              .:;;;;;;iiiirrss1
 *             ,M8 ..,....,.....,,::::::,,...         .     .,;;;iiiiiirss11h
 *             8B;.,,,,,,,.,.....          .           ..   .:;;;;iirrsss111h
 *            i@5,:::,,,,,,,,.... .                   . .:::;;;;;irrrss111111
 *            9Bi,:,,,,......                        ..r91;;;;;iirrsss1ss1111
 *
 *								狗头保佑，永无BUG！
 */







