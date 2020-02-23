CC = gcc
CFLAGS = -static -s -O2 -Wall
INC = -I .
OBJS = ec_access.o ecutil.o isp.o util.o
OUTPUT = ecutil
ENV = -D_LINUX_

ecutil: ${OBJS}
	@${CC} ${OBJS} ${CFLAGS} ${INC} -pthread -o ${OUTPUT}
	@strip ${OUTPUT}
	@echo "SUCCESS..."
%.o : %.c %.h
	@echo "build "${%}
	@${CC} $< ${CFLAGS} ${INC} ${ENV} -pthread -c 
	
.PHONY: clean all
all : clean ecutil

clean: 
	@rm -rf *.o


