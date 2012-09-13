# -*- coding: UTF-8 -*-
#
"""
Parser for RNAWolf output

Usage: python parse.py [options]

Options:
    
  -h,      --help                                        show this help
  -i ... , --input=[file_path]                           RNAWolf output file
  -o ... , --output=[file_path](arg=parse.out)           (optional) output file
  -n ... , --name=[sequence_name](arg=input_filename)    name of the sequence
  -s ... , --sequence=[structure]                        structure used as reference
"""

import os
import argparse



def write_output(sequence, structure, output_path, name, ref_structure, just_struct):
    # structure of a dot-bracket is in the format: dict(position='(')
    
    struct_dict = dict()
    for idx, val in enumerate(ref_structure):
        struct_dict[idx] = val
        
    nb_paires_canoniques = 0
    nb_paires_totales = 0
    
    list_proc = []
    for i in xrange(len(struct_dict)):
        if ')' in struct_dict[i] and (not i in list_proc):
            for j in reversed(xrange(i-1)):
                try:
                    if '(' in struct_dict[j] and (not j in list_proc):
                        # check if pairs are canonical
                        if (('A' in sequence[i] and 'T' in sequence[j]) 
                         or ('T' in sequence[i] and 'A' in sequence[j]) 
                         or ('C' in sequence[i] and 'G' in sequence[j]) 
                         or ('G' in sequence[i] and 'C' in sequence[j]) 
                         or ('A' in sequence[i] and 'U' in sequence[j]) 
                         or ('U' in sequence[i] and 'A' in sequence[j]) 
                         or ('G' in sequence[i] and 'U' in sequence[j]) 
                         or ('U' in sequence[i] and 'G' in sequence[j])):
                            nb_paires_canoniques += 1
                            
                        nb_paires_totales += 1
                    
                        # delete dictionary entries
                        list_proc.append(i)
                        list_proc.append(j)
                        
                        break
                    
                except Exception as e:
                    pass
                
    with open(output_path, 'a') as output:
        # prepare the output string
        header = ">{name} {length} B1R:{res_first}-B{length}R:{res_last} {structure} {canon}/{total}\n".format(name=name,
                                                                                                               length=len(sequence),
                                                                                                               res_first=sequence[0],
                                                                                                               res_last=sequence[len(sequence)-1],
                                                                                                               structure=ref_structure,
                                                                                                               canon=nb_paires_canoniques,
                                                                                                               total=nb_paires_totales)
        seq_and_str = "{sequence}\n{structure_w_score}\n".format(sequence=sequence, structure_w_score=structure)
        if just_struct:
          temp_str = seq_and_str
        else:
          temp_str = header+seq_and_str
        print temp_str.strip()
        output.write(temp_str)

if __name__ == '__main__':
 
    parser = argparse.ArgumentParser(description='Parser for parse.py')

    parser.add_argument('--input', '-i', action="store", dest="input_file")
    parser.add_argument('--name', '-n', action="store", dest="name", default=None)
    parser.add_argument('--structure', '-s', action="store", dest="ref_structure")
    parser.add_argument('--output', '-o', action="store", dest="output_file", default="parse.out")
    
    ns = parser.parse_args()
    
    input_file = ns.input_file
    output_file = ns.output_file
    ref_structure = ns.ref_structure
    
    input_path= os.path.join(os.getcwd(), input_file)
    output_path= os.path.join(os.getcwd(), output_file)
    
    name = ns.name
    if not name:
        name = os.path.basename(input_path)
    
    with open(input_path, 'r') as input_content:
        
        sequence = None
        structure = None
        
        bPrint = True
        just_struct = False
        for line in input_content:
            stripped_line= line.strip()
            splitted_line = stripped_line.split()
            
            if ('A' in stripped_line 
             or 'C' in stripped_line 
             or 'G' in stripped_line 
             or 'T' in stripped_line 
             or 'U' in stripped_line) and (len(splitted_line) <= 1):
                # take the sequence 
                sequence = stripped_line
                
            elif '('in stripped_line or ')' in stripped_line:
                # take the structure
                structure = stripped_line
                bPrint= True
                
            else:# e.g. lines with just numbers
                structure = None
                
            if sequence and structure and bPrint:
#              print "we are writing out for *{sequence}* *{structure}* *{output_path}* *{name}* *{ref_structure}* *{just_struct}*".format(sequence=sequence, 
#                           structure=structure, 
#                           output_path=output_path, 
#                           name=name, 
#                           ref_structure=ref_structure, 
#                           just_struct=just_struct)

              write_output(sequence=sequence, 
                           structure=structure, 
                           output_path=output_path, 
                           name=name, 
                           ref_structure=ref_structure, 
                           just_struct=just_struct)
              bPrint = False
              just_struct = True
