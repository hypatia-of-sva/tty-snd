#!/bin/sh

#see https://stackoverflow.com/questions/394230/how-to-detect-the-os-from-a-bash-script
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PROGRAMNAME=praat
elif [[ "$OSTYPE" == "darwin"* ]]; then
        # Mac OSX
    PROGRAMNAME=Praat
    export PATH=$PATH:/Applications/Praat.app/Contents/macOS
#elif [[ "$OSTYPE" == "cygwin" ]]; then
        # POSIX compatibility layer and Linux environment emulation for Windows
#elif [[ "$OSTYPE" == "msys" ]]; then
#        # Lightweight shell and GNU utilities compiled for Windows (part of MinGW)
#elif [[ "$OSTYPE" == "win32" ]]; then
        # I'm not sure this can happen.
#elif [[ "$OSTYPE" == "freebsd"* ]]; then
        # ...
else
        # Unknown.
        echo "Only Linux and MacOS currently tested and supported!"
        exit
fi


for NAME in *.wav
do

    echo "round for $NAME"
    
    
    FILENAME=${NAME%.*}".txt"
    
    
    if ! grep -q "F0" "$FILENAME" ; then
    

echo """
Read from file: \"$NAME\"
To Pitch (filtered autocorrelation): 0, 50, 800, 15, \"no\", 0.03, 0.09, 0.5, 0.055, 0.35, 0.14
selectObject: \"Pitch ${NAME%.*}\"
#Draw: 0, 0, 0, 500, \"yes\"
Get mean: 0, 0, \"Hertz\"
""" > temp.praat


    
    printf "F0(Hz) " >> $FILENAME
    $PROGRAMNAME --run temp.praat >> $FILENAME
    rm temp.praat


    echo """
Erase all
Select outer viewport: 0, 6.5, 0, 4
Read from file: \"$NAME\"
To Formant (burg): 0, 5, 5500, 0.025, 50
#Draw tracks: 0, 0, 5500, "yes"
List: \"yes\", \"no\", 3, \"no\", 2, \"no\", 2, \"yes\"
""" > temp.praat

    $PROGRAMNAME --run temp.praat > temp.txt
    rm temp.praat
    
    
    printf "F1(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=2 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    printf "B1(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=3 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    printf "F2(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=4 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    printf "B2(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=5 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    printf "F3(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=6 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    printf "B3(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=7 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    
    printf "F4(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=8 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    printf "B4(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=9 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    
    printf "F5(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=10 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    printf "B5(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=11 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    
    
    
    rm temp.txt
    
fi
    
done
    
