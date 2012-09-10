import os
import subprocess
import shlex
import sys

# call a system command and connect via pipe
def call_command(command, pipe=None, echo=False):
    if echo:
        print command
    
    process = subprocess.Popen(shlex.split(command.encode("ascii")),
                               stdin=subprocess.PIPE,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE)
    
    output = process.communicate(input=pipe)
    return output


# the code/script is run directly
if __name__ == '__main__':
    rnawolf_exec = "/u/schirmer/bin/RNAwolf"
    rnawolf_db = "/u/schirmer/Documents/cloned/RNAwolfBinaries/fr3d-400.db"
    
    rnawolf_output_dir = "/u/schirmer/Documents/rnawolf-bench/results/wolf"
    fsa_out_dir = "/u/schirmer/Documents/rnawolf-bench/results/fas"
    
    parse_py = "/u/schirmer/Documents/cloned/rna-toolbox/rnawolf-benchmark/parse.py"
    
    # analyse the output (fasta file directory containing all the seq and their struct
    fasta_file_dir = "/u/schirmer/Documents/cloned/rna-toolbox/rnawolf-benchmark/data-long-mcfold/"#"/u/schirmer/Documents/rnawolf-bench/Bench-Long/1GRZ/"

    list_fasta = os.listdir(fasta_file_dir)
    list_fasta.sort()
    i = 0
    list_processed = []
    
    for fasta in list_fasta:
        # take only the reference structures
        if not fasta.endswith('Ref'):
          continue;
        
        # open each fasta file that contains name, seq and struct
        with open(os.path.join(fasta_file_dir, fasta), 'r') as fasta_contents:
						# get out name, seq, struct
            splitted_content = fasta_contents.read().split('\n')
            #print "opening fasta file with "+str(len(splitted_content))+" lines\n"
            if len(splitted_content) <= 1:
              break
            name = splitted_content[0].replace('>', '')
            sequence = splitted_content[1].upper()
            structure_and_energy = splitted_content[2]
            splitted_content2 = structure_and_energy.split(' ')
            structure = splitted_content2[0]
						#energy = splitted_content2[xx] 
            i += 1
            
						# don't process structures already processed
            if not name in list_processed:
                list_processed.append(name)
                print "processing structure #{i} : {name}".format(i=i, name=name)
                
								# call rnawolf on sequence
                out, err = call_command("{rnawolf} -i {rnawolf_db} -s 0.05 -c 5".format(rnawolf=rnawolf_exec,
                                                                                        rnawolf_db=rnawolf_db), pipe=sequence)
                
                #print rnawolf_output
                rnawolf_output = os.path.join(rnawolf_output_dir, name+'.wolf')
                with open(rnawolf_output, 'w') as wolf_file:
                    wolf_file.write(out)
                
                out_path = os.path.join(fsa_out_dir, '{name}.fsa'.format(name=name))
                # call parse.py giving it the needed params
                print 'python {parse_py} -i {wolf} -n {name} -o {output} -s {structure}'.format(parse_py=parse_py,
                                                                                                                    wolf=rnawolf_output,
                                                                                                                    name=name,
                                                                                                                    output=out_path,
                                                                                                                    structure=structure)
                out2, err2 = call_command('python {parse_py} -i {wolf} -n {name} -o {output} -s {structure}'.format(parse_py=parse_py,
                                                                                                                    wolf=rnawolf_output,
                                                                                                                    name=name,
                                                                                                                    output=out_path,
                                                                                                                    structure=structure))
                # if error, print error
                if err or err2:
                   print "error in {name}:\nerror1:{error}\nerror2:{error2}".format(name=fasta, error=err, error2=err2)
