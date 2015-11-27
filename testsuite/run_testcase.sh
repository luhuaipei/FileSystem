#!/bin/bash


ORIG_FILES="Makefile
            reference-Linux-i686.o
            reference-Linux-x86_64.o
            reference-Darwin-x86_64.o
            include
            lib/mmapfs.c
            lib/ext2_access_ref.c
            eecs343.img"
TARGZ=$1
TC_DIR=`pwd`
TMP=`mktemp -d /tmp/cs343.tests.XXXXXX`
chmod go-rwx ${TMP} || exit 1


function cleanUp()
{
	rm -Rf ${TMP}
}


if [[ ! "$#" -eq 1 ]]; then
	echo -e "usage: $0 targz_file"
	exit 1
fi
echo "Testing ${TARGZ}"
echo


# Setup the environment
cd ${TMP} || { cleanUp; exit 1; }

for f in ${ORIG_FILES}; do
    mkdir -p `dirname $f`
	cp -fLr ${TC_DIR}/$f $f
done

# Untar sources
tar xfz ${TARGZ} || { cleanUp; exit 1; }
if [[ `ls */*.c | wc -w` -eq 0 ]]; then
    echo "error: no source files (*.c) found"
	echo "Please follow the submission instructions carefully."
	cleanUp
    exit 1
fi


# Build.
echo "BUILD"
make ext2cat_ref >${TMP}/gcc.out 2>&1 ||
    { echo "Couldn't build ext2cat_ref!";
      cat ${TMP}/gcc.out;
      cleanUp;
      exit 1; }
make ext2cat >${TMP}/gcc.out 2>&1

ERRORS=`grep -c error: ${TMP}/gcc.out`
WARNINGS=`grep -c warning: ${TMP}/gcc.out`

if [ ! "${ERRORS}" == "0" ]; then
    echo "${ERRORS} error(s) during compilation:"
    echo `grep error: ${TMP}/gcc.out`
    echo
fi

if [ ! "${WARNINGS}" == "0" ]; then
    echo "${WARNINGS} warning(s) during compilation:"
    echo `grep warning: ${TMP}/gcc.out`
    echo
fi

if [ ! "${ERRORS}" == "0" ]; then
    cleanUp
    exit 1
fi

echo "    Build succeeded."
echo


# Test.
# First, confirm that ext2cat works correctly
echo "TESTING"
TEST_FILES="/README.txt
            /photos/corn.jpg
            /photos/cows.jpg
            /code/ext2_headers/ext2_types.h
            /code/python/ouroboros.py
            /code/haskell/qsort.hs"
NUM_FILES=`echo "$TEST_FILES" | wc -l`
FILES_RETRIEVED=0

for i in $TEST_FILES; do
    echo -n "    Retrieving" "`printf \"%-40s\" $i...`"
    SUMFILE=`echo $i | sed "s/\//_/g"`
    ${TMP}/ext2cat ${TMP}/eecs343.img $i | md5sum > ${TMP}/${SUMFILE} || { cleanUp; exit 1; }
    ${TMP}/ext2cat_ref ${TMP}/eecs343.img $i | md5sum > ${TMP}/${SUMFILE}_ref
    diff ${TMP}/${SUMFILE} ${TMP}/${SUMFILE}_ref >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "Failed!"
        continue
    fi
    echo "Looks good."
    FILES_RETRIEVED=$(($FILES_RETRIEVED + 1))
done
echo "    Successfully retrieved $FILES_RETRIEVED of $NUM_FILES files."
echo


echo "GRADING"
# Primary grading: check which reference functions have been eliminated from the
# submission code. To do that, we build ext2cat without the reference blob and
# count the linker errors.
make ext2cat_sans_ref >${TMP}/gcc-graded.out 2>&1
LINK_ERRORS=`grep "undefined reference" ${TMP}/gcc-graded.out`
REFS_LEFT=`printf "$LINK_ERRORS" | grep -oP "_ref_[\w]+"`
NUM_REFS_LEFT=`printf "$REFS_LEFT" | sort | uniq | wc -l`
NUM_REF_FUNCS=`grep -oP "_ref_[\w]+" lib/ext2_access_ref.c | sort | uniq | wc -l`
NUM_REFS_ELMINATED=$(($NUM_REF_FUNCS - $NUM_REFS_LEFT))

# Extra credit: attempt to retrieve loons.jpg.
make ext2cat >/dev/null 2>&1
EC_SUM=`${TMP}/ext2cat ${TMP}/eecs343.img /photos/loons.jpg | md5sum | awk '{ print $1; }'`
if [ "$EC_SUM" == "eb5826a89dc453409ca76560979699bb" ]; then
    EC_SUCCESS="yep!"
fi

# Final grade report.
FILES_PENALTY=0
if [ $FILES_RETRIEVED -ne $NUM_FILES ]; then
    FILES_PENALTY=100
fi
REFS_PENALTY=$(($NUM_REFS_LEFT * (100 / $NUM_REF_FUNCS)))
WARNINGS_PENALTY=$((2 * $WARNINGS))
if [[ $WARNINGS_PENALTY > 16 ]]; then
    WARNINGS_PENALTY=16
fi
if [ "$EC_SUCCESS" ]; then
    EC_BONUS=10
else
    EC_BONUS=0
fi
GRADE=$((100 - FILES_PENALTY - REFS_PENALTY - WARNINGS_PENALTY + EC_BONUS))

if [ $FILES_PENALTY -ne 0 ]; then
    echo "    Your code could not extract all tested files. (-$FILES_PENALTY points)"
fi
echo -n "    You eliminated $NUM_REFS_ELMINATED of $NUM_REF_FUNCS reference functions."
if [ $REFS_PENALTY != 0 ]; then
    echo " (-$REFS_PENALTY points)"
else
    echo
fi
echo -n "    Your code produced $WARNINGS warnings during compilation."
if [ $WARNINGS_PENALTY != 0 ]; then
    echo " (-$WARNINGS_PENALTY points)"
else
    echo
fi
if [ $EC_BONUS != 0 ]; then
    echo "    You completed the extra credit. (+$EC_BONUS points)"
else
    echo "    You did not complete the extra credit."
fi
echo
echo -n "FINAL GRADE: $GRADE%"
if [ $GRADE -ge 90 ]; then
    echo "    \o/"
else
    echo
fi

cleanUp
exit 0

