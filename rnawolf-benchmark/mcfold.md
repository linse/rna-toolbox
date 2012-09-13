Run mcfold in a loop on all sequences from the reference files.
For each sequence, let it compute 10 suboptimals.

Note: make sure you have the right version (without DP)
```bash
for f in `ls ../rnawolf-benchmark/data-long-mcfold/*Ref`; 
do 
filename=$(basename "$f"); 
shortname=${filename%-*}; 
name=`head -n 1 $f | tr -d ['>']`; 
sequence=`head -n 2 $f|tail -n1`; 
mcfold -n $name -t 10 -s $sequence > results/mcfold/$shortname-result.fsa; 
done
```

We transform the results into the right format for the stats tool
```bash
for f in `ls data-long-mcfold/*Ref`; 
do 
filename=$(basename "$f"); 
shortname=${filename%-*}; 
name=`head -n1 data-long-mcfold/$shortname-Ref | tr -d ['>']`; 
struct=`head -n3 data-long-mcfold/$shortname-Ref | tail -n1`; 
python parse.py -i "results/mcfold/$shortname-result.fsa" -n "$name" -s "$struct" -o "results/mcfold/$shortname-mcfold-res-10-for-stats.fsa";
done
```

Run the stats tool
```bash
for f in `ls results/mcfold/*stats.fsa`; 
do 
`./makeStats $f > $f.stats`;
done
```

View the stats results with
```bash
tail -n 12 results/mcfold/*.stats | less
```
