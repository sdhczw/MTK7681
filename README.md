#MTK
##使用说明：
###1.MTK工程要删除\cust\ZC\src\aes和\cust\ZC\src\crc目录 
###2.使用WIFI模块自带MCU开发在工程\mak\MT7681\flags_sta.mk添加ZC_MODULE_DEV宏定义，支持简化协议需要增加ZC_EASY_UART，测试环境需呀增加TEST_ADDR
##版本升级历史
###1.AC_MTK7681_v1.0.0_20150811
1.1 AbleCloud MTK7681代码版本入库
##2.AC_MTK7681_v1.0.01_20150827
使用wifi sdk开发和使用外部mcu开发工程合并
##3.AC_MTK7681_v1.0.02_20151116
1.移除makefile -g选项

2.移除makefile MT7681_POWER_SAVING

3.优化代码空间
##3.AC_MTK7681_v1.0.03_20151225
1.同步最新代码支持crc与挺广播

2.支持简化协议2.2版本

3.修复局域网无法发现与响应无法收到的问题

4.修复flash内存过小，导致读取存储数据校验不过的问题

5.ZOTA End成功无法回响应问题暂时无法解决