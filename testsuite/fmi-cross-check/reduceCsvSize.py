import os
import sys
import csv
import pandas as pd

#
# Start point for script
#
if __name__ == "__main__":

  if not len(sys.argv) == 2 :
    print("Usage: python3 reduceCsvSize.py inputfile.csv\n")
    quit()

  inputFile = os.path.abspath(str(sys.argv[1]))

  print("inputFile: ", inputFile)

  while os.path.getsize(inputFile)/(1024*1024) >= 1:
    my_file = []

    with open(inputFile,'r') as csvfile:
      row_count = sum(1 for row in csvfile)
      print("NumRows: ", row_count)

    with open(inputFile,'r') as csvfile:
      reader = csv.reader(csvfile, delimiter=',')
      for i, row in enumerate(reader):
        if i<=1:
          my_file.append(row)
        elif i==row_count-1:
          print("End")
          my_file.append(row)
        elif i % 2 == 0:
          my_file.append(row)

    #if you want to save a new csv file
    with open(inputFile, 'w') as new_csvfile:
      new_fi_writer = csv.writer(new_csvfile, delimiter=',')
      for line in my_file:
        new_fi_writer.writerow(line)

  print("New file size: ", str( round( os.path.getsize(inputFile)/(1024*1024), 2) ) )
