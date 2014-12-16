import sys
import os.path

if len(sys.argv) < 3:
    print("Input file names, input output")
    exit(-1)

input_file_name = sys.argv[1]
output_file_name = sys.argv[2]
    
if os.path.isfile(input_file_name) is False:
    print(input_file_name + " does not exist")
    exit(-1)

output_file = open(output_file_name, mode="w")
output_file.write('\t'.join(['Program','Date','Excerpt']) + '\n')

with open(input_file_name) as f:
    idx = 0
    program = ""
    dt = ""
    txt = ""
    program_found = False
    for l in f:
        if program_found:
            if idx == 2:
                dt = l[6:].rstrip('\n')
            elif idx == 4:
                txt = l.rstrip('\n')
                program_found = False
                output_file.write('\t'.join([program,dt,txt]))
                output_file.write('\n')
            idx += 1
            idx %= 5
        elif l.startswith("Program title: "):
            program_found = True
            idx += 1
            program = l[15:].rstrip('\n')
output_file.close()
    


