Make the folding computations
```bash
mkdir results
mkdir results/subopt
for f in `ls ../rnawolf-benchmark/data-long-mcfold/*Ref`; do `head -n 3 $f | RNAsubopt > $f.subopt-res.fsa`; done
mv ../rnawolf-benchmark/data-long-mcfold/*.subopt-res.* results/subopt/
ls results/subopt/
1GRZ-Ref.subopt-res.fsa  2LKR-Ref.subopt-res.fsa  3DJ0-Ref.subopt-res.fsa  3IYQ-Ref.subopt-res.fsa  3SD1-Ref.subopt-res.fsa  4ERJ-Ref.subopt-res.fsa
2KZL-Ref.subopt-res.fsa  2YDH-Ref.subopt-res.fsa  3IIN-Ref.subopt-res.fsa  3IZD-Ref.subopt-res.fsa  4DS6-Ref.subopt-res.fsa
```

Convert to right format for the stats tool
```bash
schirmer ~/Documents/cloned/rna-toolbox/rnawolf-benchmark $ python parse.py -i "folding-result.fsa" -n "name of sequence" -o "folding-result-formatted-for-stats.fsa" -s "reference-structure-in-dotbracket"
```

We have to throw away some lines before parsing
```bash
tail -n+3 results/subopt/1GRZ-Ref.subopt-res.fsa | cut -d " " -f 1-2 | head -n11
for f in `ls results/subopt/*-Ref.subopt-res.fsa`; 
do `tail -n+3 $f | cut -d " " -f 1-2 | head -n11 > $f.cut`; 
done
ls results/subopt/*
```
Get the name of sequence:
```bash
head -n1 data-long-mcfold/1GRZ-Ref | tr -d ['>']
```
Get the reference structure:
```bash
head -n3 data-long-mcfold/1GRZ-Ref | tail -n1
```
Putting it all together:
```bash
shortname="1GRZ"; 
name=`head -n1 data-long-mcfold/$shortname-Ref | tr -d ['>']`; 
struct=`head -n3 data-long-mcfold/$shortname-Ref | tail -n1`; 
python parse.py -i "results/subopt/$shortname-Ref.subopt-res.fsa.cut" -n "$name" -s "$struct" -o "results/subopt/$shortname-Ref.subopt-res-for-stats.fsa" 
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


Run stats tool in ye olde loop
```bash
for f in `ls results/subopt/*stats.fsa`; 
do `./makeStats $f > $f.stats`; 
done

