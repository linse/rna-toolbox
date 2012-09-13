We evaluate several RNA folding programs with suboptimals.

All reference structure files should be in one directory, here ``data-long-mcfold``, and should end in ``Ref``.

For the output of the four tools, we create directories
```bash
mkdir -p results/dp-mcfold results/mcfold results/subopt results/wolf
```

The evaluation procedures for the programs are in separate markdown files:

* [mcfold](http://github.com/linse/rna-toolbox/blob/master/rnawolf-benchmark/mcfold.md)
* [dp-mcfold](https://github.com/linse/rna-toolbox/blob/master/rnawolf-benchmark/mcfold-dp.md)
* [rnasubopt](https://github.com/linse/rna-toolbox/blob/master/rnawolf-benchmark/rnasubopt.md)
* [rnawolf](https://github.com/linse/rna-toolbox/blob/master/rnawolf-benchmark/rnawolf.md)

---

Originally, RNAwolf was evaluated with a python script.

> All reference structure input files should be in one directory and should be ending in "Ref". The path to this dir is specified in rnawolf_launcher.py
> 
> Run 
> ```bash
> python rnawolf_launcher.py 
> ```
> to run RNAwolf and parse the output into the format for the stats tool. This run creates the RNAwolf output as well as a fasta file in the suitable format for the stats tool. The directories where to put them are also in rnawolf_launcher.py
> 
> Run the stats tool on the resulting fasta files.
> This can be done with the awesome bash loop:
> 
> ```bash
> for f in `ls *.fsa`; 
> do `./makeStats $f > $f.stats` ; done 
> ```
> 
> For any of the evaluations, it is nice to view the stats results with 
> ```bash
> tail -n 12 results/wolf/*.stats
> ```
> where the result directory has to be changed accordingly.
