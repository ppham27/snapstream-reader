#!/usr/bin/env python
import sys
import os.path
import datetime
import subprocess
import time

"""
Run snapstream reader script for several files and out a table of match counts.
Example:
$ python run_on_dates.py eg01china.c 2014-01-01 2014-07-01
"""

def validate(date_string):
    try:
        datetime.datetime.strptime(date_string,"%Y-%m-%d")
    except ValueError as e:
        raise ValueError("Incorrect date format, should be YYYY-MM-DD")
        
if __name__ == "__main__":
    start_time = time.time()
    if len(sys.argv) != 4:
        print("3 arguments are needed, file, begin date and end date")
        exit(-1)
    file_name = sys.argv[1]
    begin_date = sys.argv[2]
    end_date = sys.argv[3]
    if os.path.isfile(file_name) is False:
        print(file_name + " does not exist or is an invalid file")
        exit(-1)        
    validate(begin_date)
    validate(end_date)
    print("Running %s from %s to %s..." % (file_name, begin_date, end_date))
    data_files = ["Data/" + f for f in os.listdir("Data") if f >= begin_date and f < end_date]
    data_files.sort()
    full_output = open("full_output.txt","w")
    print("\t".join(["dt","total_matches_cnt","matching_programs_cnt","total_programs_cnt","selected_programs_cnt"]))
    os.system("gcc " + file_name)
    for f in data_files:
        date_string  = f[5:15]
        full_output.write(f + "\n" + "====================\n\n")
        proc = subprocess.Popen(["./a.out",f], stdout=subprocess.PIPE)
        proc_out = proc.communicate()[0].decode('utf-8')
        full_output.write(proc_out)
        proc_out = proc_out.split('\n')
        print("\t".join([date_string]) + '\t' + proc_out[-2])
    full_output.close()
    print(str(time.time() - start_time) + " seconds taken")
    
