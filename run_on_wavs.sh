#!/bin/sh

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


    
    echo -n "F0(Hz) " >> $FILENAME
    praat --run temp.praat >> $FILENAME
    rm temp.praat


    echo """
Erase all
Select outer viewport: 0, 6.5, 0, 4
Read from file: \"$NAME\"
To Formant (burg): 0, 5, 5500, 0.025, 50
#Draw tracks: 0, 0, 5500, "yes"
List: \"yes\", \"no\", 3, \"no\", 2, \"no\", 2, \"yes\"
""" > temp.praat

    praat --run temp.praat > temp.txt
    rm temp.praat
    
    
    echo -n "F1(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=2 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    echo -n "B1(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=3 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    echo -n "F2(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=4 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    echo -n "B2(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=5 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    echo -n "F3(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=6 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    echo -n "B3(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=7 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    
    echo -n "F4(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=8 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    echo -n "B4(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=9 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    
    echo -n "F5(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=10 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    echo -n "B5(Hz) " >> $FILENAME
    cat temp.txt | tail --lines=+2 | less | awk -v N=11 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> $FILENAME
    
    
    
    
    rm temp.txt
    
fi
    
done
    
