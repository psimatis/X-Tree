import sys

oldVal = sys.argv[1]
newVal = sys.argv[2]

print(oldVal, newVal)

fname = 'def.h'
file = open(fname, "r")
replacement = ""
for line in file:
    line = line.strip()
    changes = line.replace(oldVal, newVal)
    replacement = replacement + changes + "\n"

file.close()
# opening the file in write mode
fout = open(fname, "w")
fout.write(replacement)
fout.close()