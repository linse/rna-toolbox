We run RNAwolf just with a bash loop
```bash
for f in `ls data-long-mcfold/*Ref`; 
do 
filename=$(basename "$f"); 
shortname=${filename%-*}; 
head -n2 data-long-mcfold/$shortname-Ref | tail -n1 |  RNAwolf -i "/u/schirmer/Documents/cloned/RNAwolfBinaries/fr3d-400.db"  -s 0.05 -c 10 > results/wolf/$shortname-result.fsa; 
done
```
We transform the results into the right format for the stats tool
```bash
for f in `ls data-long-mcfold/*Ref`; 
do filename=$(basename "$f");
shortname=${filename%-*}; 
name=`head -n1 data-long-mcfold/$shortname-Ref | tr -d ['>']`;
struct=`head -n3 data-long-mcfold/$shortname-Ref | tail -n1`; 
python parse.py -i "results/wolf/$shortname-result.fsa" -n "$name" -s "$struct" -o "results/wolf/$shortname-wolf-res-10-for-stats.fsa"; 
done
```

Run the stats tool
```bash
for f in `ls results/wolf/*stats.fsa`;
do 
./makeStats $f > $f.stats; 
done
```

View the stats results with
```bash
tail -n 12 results/wolf/*.stats | less
```
