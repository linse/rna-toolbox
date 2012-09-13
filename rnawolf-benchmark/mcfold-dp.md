Run mcfold-DP in a loop on all sequences from the reference files.
```bash
for f in `ls ../rnawolf-benchmark/data-long-mcfold/*Ref`; 
do 
name=`head -n 1 $f | tr -d ['>']`; 
sequence=`head -n 2 $f|tail -n1`; 
mcfold-2.0.3 -n $name -t 1 -s $sequence > $f.mcfold-dp-res.fsa; 
done
```
Actually, run it and let it compute 10 suboptimals.
```bash
for f in `ls ../rnawolf-benchmark/data-long-mcfold/*Ref`; do name=`head -n 1 $f | tr -d ['>']`; sequence=`head -n 2 $f|tail -n1`; mcfold-2.0.3 -n $name -t 10 -s $sequence > $f.mcfold-dp-res-10.fsa; done
```

Now parse and transform for stats tool:
```bash
shortname="4ERJ";name=`head -n1 data-long-mcfold/$shortname-Ref | tr -d ['>']`; struct=`head -n3 data-long-mcfold/$shortname-Ref | tail -n1`; python parse.py -i "results/dp-mcfold/$shortname-Ref.mcfold-dp-res-10.fsa" -n "$name" -s "$struct" -o "results/dp-mcfold/$shortname-Ref.mcfold-dp-res-10-for-stats.fsa"
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
for f in `ls results/dp-mcfold/*stats.fsa`; do `./makeStats $f > $f.stats`;done
```
