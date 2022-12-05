ENV := gdb-debug
ENV := valgrind-debug
ENV := no-debug

STATIC_FLAGS := -static # enable static
STATIC_FLAGS := # disable static

VALGRIND_TOOL=helgrind
VALGRIND_TOOL=memcheck --leak-check=full --show-leak-kinds=all
VALGRIND_TOOL=memcheck --track-origins=yes --leak-check=full --show-leak-kinds=all --track-fds=yes -s
VALGRIND_TOOL=memcheck --track-origins=yes --leak-check=full --track-fds=yes -s

proyect_lib_path=./lib

GCC := gcc
LINKS := -pthread

ifeq ($(ENV),gdb-debug)
GCC += -Wno-unused-function -g -Wall -O0 $(STATIC_FLAGS) 
else
ifeq ($(ENV),valgrind-debug)
GCC += -Wno-unused-function -g -Wall -O0 $(STATIC_FLAGS) 
else
GCC += -Wno-unused-function -g -Wall $(STATIC_FLAGS) 
endif
endif

HEADERS := \
	-I $(proyect_lib_path)/ \
	-I $(proyect_lib_path)/include \

m_clean:
	rm -f ./build/*.o

m_proxy:
	$(GCC) $(HEADERS) \
	$(proyect_lib_path)/main.c $(LINKS) -o ./build/c_proxy.o
	chmod +x ./build/c_proxy.o

c_proxy:
	make m_clean
	make m_proxy
	echo "building [c_proxy]..."
ifeq ($(ENV),gdb-debug)
	gdb ./build/c_proxy.o
else
ifeq ($(ENV),valgrind-debug)
	valgrind --tool=$(VALGRIND_TOOL) ./build/c_proxy.o
else
	./build/c_proxy.o
endif
endif