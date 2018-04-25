需要在su超级用户权限下进行如下操作

1、    通过shell 脚本操作GPIO
# 进入GPIO目录
cd /sys/class/gpio
复制代码


# 运行ls命令查看gpio目录中的内容，可以查看到export  gpiochip0  unexport三个文件
sudo ls
复制代码


# GPIO操作接口从内核空间暴露到用户空间
# 执行该操作之后，该目录下会增加一个gpio26文件
echo 26 > export
复制代码


# 进入GPIO26目录，该目录由上一步操作产生
cd gpio26
复制代码


#   运行ls查看gpio26目录中的内容，可查看到如下内容
#   active_low  direction  edge  power  subsystem  uevent  value
sudo ls
复制代码


# 设置GPIO26为输出方向
echo out > direction
复制代码


# BCM_GPIO26输出逻辑高电平，LED点亮
echo 1 > value
复制代码


#  BCM_GPIO26输出逻辑低电平，LED熄灭
echo 0 > value
复制代码


# 返回上一级目录
cd ..
复制代码


# 注销GPIO20接口
echo 20> unexport
复制代码


注：echo 命令为打印输出，相当于C语言的printf函数的功能，>符号为IO重定向符号，IO重定向是指改变linux标准输入和输出的默认设备，指向一个用户定义的设备。例如echo 20 > export便是把20写入到export文件中

我们可以编写成shell 脚本的形式运行
vi led.sh
复制代码



用vi新建led.sh文件，添加如下程序并保存退出。
#! /bin/bash
echo Exporting pin $1
echo $1 > /sys/class/gpio/export
echo Setting direction to out.
echo out > /sys/class/gpio/gpio$1/direction
echo Setting pin $2
echo $2 > /sys/class/gpio/gpio$1/value
复制代码


修改文件属性，使文件可执行。
chmod +x led.sh
复制代码


程序第一句注销表明这个是一个bash shell 文件，通过/bin/bash程序执行。
$1代表第一个参数，$2代表第二个参数，执行如下两个命令可点亮和熄灭LED(Pioneer600扩展板LED1接到树莓派BCM编码的26号管脚）。
sudo ./led.sh 26 1
sudo ./led.sh 26 0
复制代码

