#!/bin/bash

SDIR=.
LAB=Lab5
JDIR=~rchaney/Classes/cs201/Labs/${LAB}

F2H=float-2-hex
TEST_FILE=float-examples.txt
SUCCESS=0
CLEANUP=1
#DIFF_OPTIONS="-B -w -i"
DIFF_OPTIONS=""

while getopts "xC" opt
do
    case $opt in
	    x)
	        # If you really, really, REALLY want to watch what is going on.
	        echo "Hang on for a wild ride."
	        set -x
	        ;;
	    C)
	        # Skip removal of data files
	        CLEANUP=0
	        ;;
	    \?)
	        echo "Invalid option" >&2
	        echo ""
	        exit 1
	        ;;
	    :)
	        echo "Option -$OPTARG requires an argument." >&2
	        exit 1
	        ;;
    esac
done

rm -f *.o ${F2H}
make -s ${F2H}

if [ ! -e ${F2H} ]
then
    echo "Cannot find ${F2H}"
    echo "All tests fail. Zero points."
    exit 1
fi

ln -sf ${JDIR}/${TEST_FILE} .

${SDIR}/${F2H} -f < ${TEST_FILE} > SF-${TEST_FILE}
${JDIR}/${F2H} -f < ${TEST_FILE} > JF-${TEST_FILE}

${SDIR}/${F2H} -d < ${TEST_FILE} > SD-${TEST_FILE}
${JDIR}/${F2H} -d < ${TEST_FILE} > JD-${TEST_FILE}

echo -e "\n\nfloat comparison"
diff ${DIFF_OPTIONS} JF-${TEST_FILE} SF-${TEST_FILE} > /dev/null

if [ $? -eq 0 ]
then
    echo "*** all is good ***"
    ((SUCCESS+=1))
else
    echo "--- did not compare the same ---"
    CLEANUP=0

    DIFF_COUNT=$(diff ${DIFF_OPTIONS} JF-${TEST_FILE} SF-${TEST_FILE} | wc -l)
    echo "line diff count: ${DIFF_COUNT}"
    echo "try this: diff ${DIFF_OPTIONS} -y JF-${TEST_FILE} SF-${TEST_FILE}"
    
    LC=$(wc -l < SF-${TEST_FILE})
    if [ ${LC} -eq 1 ]
    then
        echo "Only one line of output. You probably did not read all the input."
    fi
fi

echo -e "\n\ndouble comparison"
diff ${DIFF_OPTIONS} JD-${TEST_FILE} SD-${TEST_FILE} > /dev/null

if [ $? -eq 0 ]
then
    echo "*** all is good ***"
    ((SUCCESS+=1))
else
    echo "--- did not compare the same ---"
    CLEANUP=0

    DIFF_COUNT=$(diff ${DIFF_OPTIONS} JD-${TEST_FILE} SD-${TEST_FILE} | wc -l)
    echo "line diff count: ${DIFF_COUNT}"
    echo "try this: diff ${DIFF_OPTIONS} -y JD-${TEST_FILE} SD-${TEST_FILE}"

    LC=$(wc -l < SF-${TEST_FILE})
    if [ ${LC} -eq 1 ]
    then
        echo "Only one line of output. You probably did not read all the input."
    fi
fi

echo -e "\n\n"

if [ ${SUCCESS} -eq 2 ]
then
    echo "Excellent work 75 points"
else
    if [ ${SUCCESS} -eq 1 ]
    then
        echo "Part of the way there 35 points"
    else
        echo "**OUCH** something is missing"
    fi
fi

if [ ${CLEANUP} -eq 1 ]
then
    rm [JS][DF]-* ${TEST_FILE}
    rm ${F2H} ${F2H}.o
fi
