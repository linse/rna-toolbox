Run mcfold-DP in a loop on all sequences from the reference files.
Actually, run it and let it compute 10 suboptimals.
TODO: make sure you have the right version (here: without DP)
```bash
for f in `ls ../rnawolf-benchmark/data-long-mcfold/*Ref`; 
do 
name=`head -n 1 $f | tr -d ['>']`; 
sequence=`head -n 2 $f|tail -n1`; 
mcfold -n $name -t 10 -s $sequence > $f-MCFold-10.fsa; 
done
```
Move computation results to mcfold result directory.

Now parse and transform for stats tool:
```bash
shortname="4ERJ";
name=`head -n1 data-long-mcfold/$shortname-Ref | tr -d ['>']`; 
struct=`head -n3 data-long-mcfold/$shortname-Ref | tail -n1`; 
python parse.py -i "results/mcfold/$shortname-MCFold-10" -n "$name" -s "$struct" -o "results/mcfold/$shortname-mcfold-res-10-for-stats.fsa"
```
Run in loop:
```bash
for f in `ls data-long-mcfold/*Ref`; 
do 
filename=$(basename "$f"); 
shortname=${filename%-*}; 
echo $shortname; 
done
```


Run stats tool:
```bash
for f in `ls results/mcfold/*stats.fsa`; 
do 
`./makeStats $f > $f.stats`;
done
```
