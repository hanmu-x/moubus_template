cmake_minimum_required (VERSION 3.15)

project(modbus_test CXX)


add_executable(${PROJECT_NAME}  "main.cpp" )
target_link_libraries(${PROJECT_NAME} PUBLIC modbus_lib)







