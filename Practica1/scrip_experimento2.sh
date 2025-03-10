#!/bin/bash
#SBATCH -n 1 -c 64 -t 00:10:00 --mem=200G
#SBATCH --job-name p1acg04

gcc acp2.c -o acp2 -msse3 -O0
#rm salid2a.csv #eliminar el archivo de salida para no escribir en los datos viejos
NL1=768
NL2=20480

L1=$((NL1 / 2))
L2=$((NL1 * 3 / 2))
L3=$((NL2 / 2))
L4=$((NL2 * 3 / 4))
L5=$((NL2 * 2))
L6=$((NL2 * 4))
L7=$((NL2 * 8))



for i in {1..10}
	do
        for D in 2 128 512 1024 2048
	        do
		for L in $L1 $L2 $L3 $L4 $L5 $L6 $L7
			do
			if [ "$D" -gt 16 ]; then
				R=$L
			else
				R=$(((16*L)/D))
			fi
			echo "D: $D"
			echo "L: $L"
			echo "R: $R"
			./acp2 "$D" "$R" "$L" >> salida2.csv
		 done
	done
done


