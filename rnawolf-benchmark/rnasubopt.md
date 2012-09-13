Make the folding computations with RNAsubopt
```bash
for f in `ls data-long-mcfold/*Ref`; 
do 
filename=$(basename "$f"); 
shortname=${filename%-*}; 
head -n2 data-long-mcfold/$shortname-Ref | tail -n1 | RNAsubopt > results/subopt/$shortname-result.fsa; 
done
```

We have to throw away some lines before parsing, to get only 10 suboptimals
```bash
for f in `ls results/subopt/*-result.fsa`; 
do `tail -n+3 $f | cut -d " " -f 1-2 | head -n11 > $f.cut`; 
done
```

We transform the results into the right format for the stats tool:
```bash
for f in `ls data-long-mcfold/*Ref`; 
do filename=$(basename "$f");
shortname=${filename%-*}; 
name=`head -n1 data-long-mcfold/$shortname-Ref | tr -d ['>']`; 
struct=`head -n3 data-long-mcfold/$shortname-Ref | tail -n1`; 
python parse.py -i "results/subopt/$shortname-Ref.subopt-res.fsa.cut" -n "$name" -s "$struct" -o "results/subopt/$shortname-Ref.subopt-res-for-stats.fsa"; 
done
```

Run stats tool in ye olde loop
```bash
for f in `ls results/subopt/*stats.fsa`; 
do 
./makeStats $f > $f.stats; 
done
```

View the stats results with
```bash
tail -n 12 results/subopt/*.stats | less
```
