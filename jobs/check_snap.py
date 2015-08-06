import datetime
import os
import subprocess
import sys

file_prefix = "/home/pemantle/snap/Data/"
file_suffix = "-Combined.txt"
email_subject = "Warning: SNAP files missing"
to = ["phamp@math.upenn.edu", "pemantle@math.upenn.edu", "mutz@sas.upenn.edu"]
# to = ["phamp@math.upenn.edu"] 
from_date = datetime.date.today() - datetime.timedelta(60)
current_date = datetime.date.today() - datetime.timedelta(60)
to_date = datetime.date.today()
files_to_check = []
while current_date < to_date:
    files_to_check.append(str(current_date) + file_suffix)
    current_date += datetime.timedelta(1)
missing_files = []
for f in files_to_check:
    if not os.path.isfile(file_prefix + f):
        missing_files.append(f)

if len(missing_files) == 0:
    sys.exit(0)
else:
    subject = "WARNING: Missing SNAP files"
    msg = "The following files are missing:\n" + "\n".join(missing_files)
    command = "echo" + " \""+msg+"\" | mail -s \"" + subject + "\" " + ",".join(to)
    subprocess.call(command, shell=True)
    sys.exit(-1)
