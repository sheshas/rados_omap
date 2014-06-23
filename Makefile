CC=/usr/bin/gcc

# compilation flags
CFLAGS=-g -Wall -D_GNU_SOURCE

# Compile time library paths
C_LIBPATHS=

# Run time library paths
R_LIBPATHS=

# Headers
INC=

# object files
obj=rados_table.o

# binary name
bin=rados_table

# Libraries to load
LIBS=-lrados

DBGFLAGS=-DDEBUG

LDFLAGS=${C_LIBPATHS} ${R_LIBPATHS}

all: rados_table

${obj}:%.o:%.c
	${CC} ${CFLAGS} ${INC} ${DBGFLAGS} -c $^

rados_table:${obj}
	${CC} ${CFLAGS} ${INC} ${LDFLAGS} -o $@ ${obj} ${LIBS}

clean:
	rm -rf *.o ${bin}
