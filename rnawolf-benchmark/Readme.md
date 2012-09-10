All reference structure input files should be in one directory and should be ending in "Ref". The path to this dir is specified in rnawolf_launcher.py

Run 
```bash
python rnawolf_launcher.py 
```
to run RNAwolf and parse the output into the format for the stats tool. This run creates the RNAwolf output as well as a fasta file in the suitable format for the stats tool. The directories where to put them are also in rnawolf_launcher.py

Run the stats tool on the resulting fasta files.
This can be done with the awesome bash loop:

```bash
for f in `ls *.fsa`; do `~/Documents/cloned/rna-toolbox/rnawolf-benchmark/makeStats $f > $f.stats` ; done 
```
