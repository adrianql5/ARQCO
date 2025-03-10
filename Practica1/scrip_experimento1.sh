#!/bin/bash
#SBATCH -n 1 -c 64 -t 00:10:00 --mem=200G
#SBATCH --job-name p1acg04

gcc acp1.c -o acp1 -msse3 -O0
#rm salida.csv #eliminar el archivo de salida para no escribir en los datos viejos

NL1=768
NL2=20480

L1=$((NL1 / 2))
L2=$((NL1 * 3 / 2))
L3=$((NL2 / 2))
L4=$((NL2 * 3 / 4))
L5=$((NL2 * 2))
L6=$((NL2 * 4))
L7=$((NL2 * 8))



for i in {1..10} #Executar 10 veces o experimiento
do
	for D in 2 8 128 512 1024 
	do
        	for L in $L1 $L2 $L3 $L4 $L5 $L6 $L7
        	do
            	if [ "$D" -gt 8 ]; then
				R=$L
			else 
				R=$(((8*L)/D))
			fi
			echo "     "
			echo "D: $D"
			echo "L: $L"
			echo "R: $R"
	    		./acp1 "$D" "$R" "$L" >> salida1.csv
		done
	done
done

