#!/bin/bash

touch Modelo.txt finalR.txt finalAR.txt final.txt

touch datosR3.txt promedioR3.txt

touch datosAR3.txt promedioAR3.txt

touch celdas.txt promedioCeldas.txt

####################################################################################################################################################

#Anisotropic Disturbed
echo "AnisoDist(L-STIT)", >> Modelo.txt

for((i=0;i<100;i++))
do
./cdt 0 2 70 0.25 3 0 0.334 0.25 0.333 0.5 0.333 0.2

lineaC=$(sed -n '1p' statSTIT.txt)
variableC=`echo "$lineaC" | cut -c1-4`
echo "$variableC," >> celdas.txt

lineaR=$(sed -n '4p' statSTIT.txt)
variableR=`echo "$lineaR" | cut -c1-14`
echo "$variableR," >> datosR3.txt

lineaAR=$(sed -n '7p' statSTIT.txt)
variableAR=`echo "$lineaAR" | cut -c1-14`
echo "$variableAR," >> datosAR3.txt

sleep 1
done

avgCELDAS=$(awk '{ SUM+=$1 } END { print SUM/NR}' celdas.txt)
echo "$avgCELDAS" >> promedioCeldas.txt

avgR3=$(awk '{ SUM+=$1 } END { print SUM/NR}' datosR3.txt) 
echo "$avgR3," >> promedioR3.txt

avgAR3=$(awk '{ SUM+=$1 } END { print SUM/NR}' datosAR3.txt) 
echo "$avgAR3," >> promedioAR3.txt

> celdas.txt
> datosR3.txt
> datosAR3.txt

####################################################################################################################################################

#Anisotropic Elliptic
echo "AnisoEllip(L-STIT)", >> Modelo.txt

for((i=0;i<100;i++))
do
./cdt 0 3 75 0.25 0.2

lineaC=$(sed -n '1p' statSTIT.txt)
variableC=`echo "$lineaC" | cut -c1-4`
echo "$variableC," >> celdas.txt

lineaR=$(sed -n '4p' statSTIT.txt)
variableR=`echo "$lineaR" | cut -c1-14`
echo "$variableR," >> datosR3.txt

lineaAR=$(sed -n '7p' statSTIT.txt)
variableAR=`echo "$lineaAR" | cut -c1-14`
echo "$variableAR," >> datosAR3.txt

sleep 1
done

avgCELDAS=$(awk '{ SUM+=$1 } END { print SUM/NR}' celdas.txt)
echo "$avgCELDAS" >> promedioCeldas.txt

avgR3=$(awk '{ SUM+=$1 } END { print SUM/NR}' datosR3.txt) 
echo "$avgR3," >> promedioR3.txt

avgAR3=$(awk '{ SUM+=$1 } END { print SUM/NR}' datosAR3.txt) 
echo "$avgAR3," >> promedioAR3.txt

> celdas.txt
> datosR3.txt
> datosAR3.txt

####################################################################################################################################################

#Anisotropic Disturbed
echo "AnisoDist(L-AREA)", >> Modelo.txt

for((i=0;i<100;i++))
do
./cdt 1 2 2400 0.25 3 0 0.334 0.25 0.333 0.5 0.333 0.2

lineaC=$(sed -n '1p' statSTIT.txt)
variableC=`echo "$lineaC" | cut -c1-4`
echo "$variableC," >> celdas.txt

lineaR=$(sed -n '4p' statSTIT.txt)
variableR=`echo "$lineaR" | cut -c1-14`
echo "$variableR," >> datosR3.txt

lineaAR=$(sed -n '7p' statSTIT.txt)
variableAR=`echo "$lineaAR" | cut -c1-14`
echo "$variableAR," >> datosAR3.txt

sleep 1
done

avgCELDAS=$(awk '{ SUM+=$1 } END { print SUM/NR}' celdas.txt)
echo "$avgCELDAS" >> promedioCeldas.txt

avgR3=$(awk '{ SUM+=$1 } END { print SUM/NR}' datosR3.txt) 
echo "$avgR3," >> promedioR3.txt

avgAR3=$(awk '{ SUM+=$1 } END { print SUM/NR}' datosAR3.txt) 
echo "$avgAR3," >> promedioAR3.txt

> celdas.txt
> datosR3.txt
> datosAR3.txt

####################################################################################################################################################

#Anisotropic Elliptic
echo "AnisoEllip(L-AREA)", >> Modelo.txt

for((i=0;i<100;i++))
do
./cdt 1 3 2400 0.25 0.2

lineaC=$(sed -n '1p' statSTIT.txt)
variableC=`echo "$lineaC" | cut -c1-4`
echo "$variableC," >> celdas.txt

lineaR=$(sed -n '4p' statSTIT.txt)
variableR=`echo "$lineaR" | cut -c1-14`
echo "$variableR," >> datosR3.txt

lineaAR=$(sed -n '7p' statSTIT.txt)
variableAR=`echo "$lineaAR" | cut -c1-14`
echo "$variableAR," >> datosAR3.txt

sleep 1
done

avgCELDAS=$(awk '{ SUM+=$1 } END { print SUM/NR}' celdas.txt)
echo "$avgCELDAS" >> promedioCeldas.txt

avgR3=$(awk '{ SUM+=$1 } END { print SUM/NR}' datosR3.txt) 
echo "$avgR3," >> promedioR3.txt

avgAR3=$(awk '{ SUM+=$1 } END { print SUM/NR}' datosAR3.txt) 
echo "$avgAR3," >> promedioAR3.txt

> celdas.txt
> datosR3.txt
> datosAR3.txt

paste Modelo.txt promedioCeldas.txt promedioR3.txt promedioAR3.txt >> final.txt
awk '{print $1, $2, $3, $4}' final.txt | column -t >> Datos.csv

rm -r datosR3.txt
rm -r promedioR3.txt
rm -r datosAR3.txt
rm -r promedioAR3.txt
rm -r finalR.txt
rm -r finalAR.txt
rm -r Modelo.txt
rm -r celdas.txt
rm -r promedioCeldas.txt
rm -r final.txt
