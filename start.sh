FILE=$(basename $1 | rev | cut -c5- | rev)
gcc -x c++ -I./include $1 -lstdc++ -lcurl -o $FILE
mkdir -p out
mv $FILE out/$FILE
if [ $# -eq 2 ]
then
    if [ $2 = "--run" ]
    then
        screen /dev/ttyUSB0 115200
        sudo ./out/$FILE
    fi
fi
