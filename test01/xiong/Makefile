#获取当前目录下所有的.cpp文件
SRC=$(wildcard ./*.cpp)

#将SRC中所有出现的.cpp文件替换成.o
OBJS=$(patsubst %.cpp, %.o, $(SRC))

TARGET = test

$(TARGET):$(OBJS)
	$(CXX) $^ -o $@

%.o:%.cpp
#$(CXX) -c $< -o $@
#  @不显示命令本身，只显示结果
	@$(CXX) -c $< -o $@

# add.o:add.cpp
# 	$(CXX) -c $< -o $@
# sub.o:sub.cpp
# 	$(CXX) -c $< -o $@
# mul.o:mul.cpp
# 	$(CXX) -c $< -o $@
# dev.o:dev.cpp
# 	$(CXX) -c $< -o $@
# main.o:main.cpp
# 	$(CXX) -c $< -o $@

.PHONY:clean
clean:
	-rm -rf $(OBJS) test	