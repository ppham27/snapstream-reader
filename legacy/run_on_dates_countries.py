#!/usr/bin/env python
import sys
import os.path
import datetime
import subprocess
import time
import random

"""
Run snapstream reader script for several files and out a table of match counts.
Example:
$ python run_on_dates_countries.py list_of_countries.txt 2014-01-01 2014-07-01
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
    country_file = sys.argv[1]
    begin_date = sys.argv[2]
    end_date = sys.argv[3]
    if os.path.isfile(country_file) is False:
        print(country_file + " does not exist or is an invalid file")
        exit(-1)        
    validate(begin_date)
    validate(end_date)
    print("Running %s from %s to %s..." % (country_file, begin_date, end_date))
    data_files = ["Data/" + f for f in os.listdir("Data") if f >= begin_date and f < end_date]
    data_files.sort()
    
    excerpts = list()
    country_dict = dict()
    with open(country_file) as f:
        for country_file_line in f:
            country_dict[country_file_line.strip().lower()] = {"total_matches_cnt": 0, "matching_programs_cnt": 0,
                                                               "country_dict": dict()}
    for k, v in country_dict.items():
        for c in country_dict:
            v["country_dict"][c] = {"total_matches_cnt": 0, "matching_programs_cnt": 0}
    full_output = open("search_output.txt","w")
    print("\t".join(["search_string","dt","total_matches_cnt","matching_programs_cnt","total_programs_cnt","selected_programs_cnt"]))
    os.system("gcc eg03search.c")
    for country in country_dict:        
        search_string = country
        for file_name in data_files:
            date_string = file_name[5:15]
            header = "searching for " + search_string + " in " + file_name + "\n" + "====================\n\n"
            proc = subprocess.Popen(["./a.out",search_string, file_name], stdout=subprocess.PIPE)
            proc_out = proc.communicate()[0].decode('utf-8')
            excerpts.append(header + proc_out)
            full_output.write(header + proc_out)            
            proc_out = proc_out.split('\n')
            data_line = proc_out[-2]
            print("\t".join([search_string, date_string]) + '\t' + data_line)
            int(data_line.split('\t')[0])            
            country_dict[search_string]["total_matches_cnt"] +=  int(data_line.split('\t')[0])
            country_dict[search_string]["matching_programs_cnt"] +=  int(data_line.split('\t')[1])
    full_output.close()
    os.system("gcc eg04nearsearch.c")
    print("\t".join(["c1","c2","dt","total_matches_cnt","matching_programs_cnt","total_programs_cnt","selected_programs_cnt"]))
    full_output = open("near_output.txt","w")
    country_list = list(country_dict.keys())
    for i in range(len(country_list)):
        for j in range(i+1, len(country_list)):
            c1 = country_list[i]
            c2 = country_list[j]
            for file_name in data_files:
                date_string = file_name[5:15]
                header = "searching for " + c1 + " near " + c2 + " in " + file_name + "\n" + "====================\n\n"
                proc = subprocess.Popen(["./a.out", c1, c2, file_name], stdout=subprocess.PIPE)
                proc_out = proc.communicate()[0].decode('utf-8')
                excerpts.append(header + proc_out)                
                full_output.write(header + proc_out)
                proc_out = proc_out.split('\n')
                data_line = proc_out[-2]
                print("\t".join([c1, c2, date_string]) + '\t' + data_line)
                country_dict[c1]["country_dict"][c2]["total_matches_cnt"] +=  int(data_line.split('\t')[0])
                country_dict[c2]["country_dict"][c1]["total_matches_cnt"] +=  int(data_line.split('\t')[0])
                country_dict[c1]["country_dict"][c2]["matching_programs_cnt"] +=  int(data_line.split('\t')[1])
                country_dict[c2]["country_dict"][c1]["matching_programs_cnt"] +=  int(data_line.split('\t')[1])
    full_output.close()
    print("\t".join(["search_string","total_matches_cnt","matching_programs_cnt"]))
    for k, v in country_dict.items():
        print(k + '\t' + str(v["total_matches_cnt"]) + '\t' + str(v["matching_programs_cnt"]))
    print("\t".join(["c1","c2","total_matches_cnt","matching_programs_cnt"]))
    for k, v in country_dict.items():
        for kk, vv in v["country_dict"].items():
            if kk !=k:
                print(k + '\t' + kk  + '\t' + str(vv["total_matches_cnt"]) + '\t' + str(vv["matching_programs_cnt"]))
            else:
                print(k + '\t' + kk  + '\t' + str(v["total_matches_cnt"]) + '\t' + str(v["matching_programs_cnt"]))
    random.shuffle(excerpts)
    for e in excerpts[:100]:
        print(e)
    print(str(time.time() - start_time) + " seconds taken")

    
