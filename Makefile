# handin info
TEAM=`whoami`
VERSION=`date +%Y%m%d%H%M%S`
PROJ=ext2cat

DELIVERY=ext2cat.c lib/ext2_access.c
ARCHIVE=${TEAM}-${VERSION}-${PROJ}.tar.gz
OS=$(shell uname -s)
ARCH=$(shell uname -m)
REFBLOB=reference-${OS}-${ARCH}.o

CC=gcc
CCOPTS=-g -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-variable -I include
LIBDIR=lib
OBJDIR=obj

VM_NAME = "Ubuntu_1404"
VM_PORT = "3022"

SHELL_ARCH = “64”



all: ext2cat

handin: clean
	tar -cvzf ${ARCHIVE} ${DELIVERY}

test-reg: handin
	HANDIN=`pwd`/${ARCHIVE}; cd testsuite; ./run_testcase.sh $${HANDIN}; rm -f $${HANDIN}

# Build the final binary against your code with the reference blob.
ext2cat: ext2_access mmapfs ext2cat.c
	${CC} ${CCOPTS} -o ext2cat ext2cat.c ${OBJDIR}/mmapfs.o ${OBJDIR}/ext2_access.o ${REFBLOB}

# Build the final binary against your code without the reference blob.
ext2cat_sans_ref: ext2_access mmapfs ext2cat.c
	${CC} ${CCOPTS} -o ext2cat ext2cat.c ${OBJDIR}/mmapfs.o ${OBJDIR}/ext2_access.o

# Build the final binary against the reference blob.
ext2cat_ref: ext2_access_ref mmapfs ext2cat.c
	${CC} ${CCOPTS} -o ext2cat_ref ext2cat.c ${OBJDIR}/mmapfs.o ${OBJDIR}/ext2_access_ref.o ${REFBLOB}



# Intermediate targets.
objdir:
	@mkdir ${OBJDIR} 2>/dev/null || true

# Build reusable .o files.
mmapfs: objdir ${LIBDIR}/mmapfs.c
	${CC} ${CCOPTS} -c -o ${OBJDIR}/mmapfs.o ${LIBDIR}/mmapfs.c

ext2_access: objdir ${LIBDIR}/ext2_access.c
	${CC} ${CCOPTS} -c -o ${OBJDIR}/ext2_access.o ${LIBDIR}/ext2_access.c

ext2_access_ref: objdir ${LIBDIR}/ext2_access_ref.c
	${CC} ${CCOPTS} -c -o ${OBJDIR}/ext2_access_ref.o ${LIBDIR}/ext2_access_ref.c

refblob: reflib/reference_implementation.c
	${CC} ${CCOPTS} -c -o ${REFBLOB} reflib/reference_implementation.c

clean:
	@rm -f ext2cat ext2cat_ref ${OBJDIR}/* *.gch */*.gch


start-vm:
	VBoxManage startvm ${VM_NAME} --type headless

kill-vm:
	VBoxManage controlvm ${VM_NAME} poweroff

test-vm:
	scp -r -i id_aqualab -P 3022 * aqualab@localhost:~/.aqualab/project4/.
	ssh -i id_aqualab -p 3022 aqualab@localhost 'bash -s' < vm_test.sh
