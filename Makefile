

SRCS = $(wildcard ./src/*.cpp ./lib/*.c)    #wildcard把 指定目录 ./ 和 ../lib 下的所有后缀是c的文件全部展开。

OBJS = $(SRCS:./src/*.cpp = .o)    #OBJS将$(SRCS)下的.c文件转化为.o文件

CC = g++   #代表所使用的编译器

INCLUDES = -I/usr/include          \
		   -I/usr/local/include    \
		   -I./kdtree              \
           -I./include/   #头文件查找路径

LIBS = -L./lib -L./kdtree -lemg_dataaccess -lemg_datamanager -logr -lkdtree  #链接库查找地址

CCFLAGS = -g -Wall -Wl,-rpath,./lib    #附加参数
CCFLAGS += -std=c++11 -pthread

OUTPUT = ./showPathlineDemo   #输出程序名称

all:$(OUTPUT)

$(OUTPUT) : $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(CCFLAGS) $(LIBS)

%.o : %.cpp
	$(CC) -c $< $(CCFLAGS)

clean:
	rm -rf $(OUTPUT) *.out *.o    #清除中间文件及生成文件

.PHONY:clean

