Make the folding computations
```bash
schirmer ~/Documents/cloned/rna-toolbox/rnawolf-benchmark $ mkdir results
schirmer ~/Documents/cloned/rna-toolbox/rnawolf-benchmark $ mkdir results/wolf
schirmer ~/Documents/cloned/rna-toolbox/rnawolf-benchmark $ mkdir results/subopt
schirmer ~/Documents/cloned/rna-toolbox/rnawolf-benchmark $ mkdir results/dp-mcfold
schirmer ~/Documents/cloned/rna-toolbox/rnawolf-benchmark $ mkdir results/mcfold
schirmer ~/Documents/cloned/rna-toolbox/rnawolf-benchmark $ for f in `ls ../rnawolf-benchmark/data-long-mcfold/*Ref`; do `head -n 3 $f | RNAsubopt > $f.subopt-res.fsa`; done
schirmer ~/Documents/cloned/rna-toolbox/rnawolf-benchmark $ mv ../rnawolf-benchmark/data-long-mcfold/*.subopt-res.* results/subopt/
schirmer ~/Documents/cloned/rna-toolbox/rnawolf-benchmark $ ls results/subopt/
1GRZ-Ref.subopt-res.fsa  2LKR-Ref.subopt-res.fsa  3DJ0-Ref.subopt-res.fsa  3IYQ-Ref.subopt-res.fsa  3SD1-Ref.subopt-res.fsa  4ERJ-Ref.subopt-res.fsa
2KZL-Ref.subopt-res.fsa  2YDH-Ref.subopt-res.fsa  3IIN-Ref.subopt-res.fsa  3IZD-Ref.subopt-res.fsa  4DS6-Ref.subopt-res.fsa
```

Convert to right format for the stats tool
```bash
schirmer ~/Documents/cloned/rna-toolbox/rnawolf-benchmark $ python parse.py -i "folding-result.fsa" -n "name of sequence" -o "folding-result-formatted-for-stats.fsa" -s "reference-structure-in-dotbracket"
```

Get the name of sequence:
```bash
head -n1 data-long-mcfold/1GRZ-Ref | tr -d ['>']
```
Get the reference structure:
```bash
head -n3 data-long-mcfold/1GRZ-Ref | tail -n1
```

Run stats tool in ye olde loop
