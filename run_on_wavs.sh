#!/bin/bash

for NAME in *.wav
do

    echo "round for $NAME"
    
    echo "processing $NAME" >> log.txt

echo """
Read from file: \"$NAME\"
To Pitch (filtered autocorrelation): 0, 50, 800, 15, \"no\", 0.03, 0.09, 0.5, 0.055, 0.35, 0.14
selectObject: \"Pitch ${NAME%.*}\"
#Draw: 0, 0, 0, 500, \"yes\"
Get mean: 0, 0, \"Hertz\"
""" > temp.praat

    
    echo -n "F0(Hz) " >> log.txt
    praat --run temp.praat >> log.txt
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
    
    
    
    echo -n "F1(Hz) " >> log.txt
    cat temp.txt | tail --lines=+2 | less | awk -v N=2 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> log.txt
    
    echo -n "B1(Hz) " >> log.txt
    cat temp.txt | tail --lines=+2 | less | awk -v N=3 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> log.txt
    
    echo -n "F2(Hz) " >> log.txt
    cat temp.txt | tail --lines=+2 | less | awk -v N=4 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> log.txt
    
    echo -n "B2(Hz) " >> log.txt
    cat temp.txt | tail --lines=+2 | less | awk -v N=5 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> log.txt
    
    echo -n "F3(Hz) " >> log.txt
    cat temp.txt | tail --lines=+2 | less | awk -v N=6 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> log.txt
    
    echo -n "B3(Hz) " >> log.txt
    cat temp.txt | tail --lines=+2 | less | awk -v N=7 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> log.txt
    
    
    echo -n "F4(Hz) " >> log.txt
    cat temp.txt | tail --lines=+2 | less | awk -v N=8 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> log.txt
    
    echo -n "B4(Hz) " >> log.txt
    cat temp.txt | tail --lines=+2 | less | awk -v N=9 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> log.txt
    
    
    echo -n "F5(Hz) " >> log.txt
    cat temp.txt | tail --lines=+2 | less | awk -v N=10 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> log.txt
    
    echo -n "B5(Hz) " >> log.txt
    cat temp.txt | tail --lines=+2 | less | awk -v N=11 '{ sum += $N } END { if (NR > 0) print sum / NR }' >> log.txt
    
    
    echo "" >> log.txt
    echo "" >> log.txt
    echo "" >> log.txt
    
    
    rm temp.txt
    
    
done
    
