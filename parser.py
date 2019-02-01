#!/usr/bin/python3

def extractEnergies(line):
  a =  line.split()[1:3]
  enau = float(a[0])
  encm = float(a[1])
  return [enau,encm]

def getJandP(line):
  line = line.replace(',',' ')
  a = line.split()
  j = int(a[4])
  p = -1
  if a[7]=='even':
    p=1
  return [j,p]

def getConfigs(line, min_pc):
  line = line.strip('\n')
  a=line.split('  ')
  #print(a[-2:])
  st = a[-2]
  pc = float(a[-1].strip('%'))
  if pc>min_pc:
    return [st,pc]
  return []

def get_gFactor(line):
  a = line.split()
  g = float(a[2])
  return [g]

def parsefile(fn):
  import os
  oalist = []  #output list (list of lists)
  min_pc = 10. #only print electron configs > this %
  if os.path.exists(fn):
    file = open(fn, "r")
    jp = [] # J and parity
    lst = []
    good_part = False #"good" part of the file
    for line in file:
      if(line.startswith("AlphaSquaredVariation")):
        oalist.append([""]) 
        oalist.append([line])
      elif(line.startswith("Solutions for J = ")):
        #This is the start of a new level
        jp = getJandP(line)
        good_part = True
      elif not(good_part):
        continue
      elif(line.endswith("/cm\n")):
        lst = []
        e = extractEnergies(line)
        lst = jp+e
        tmpcf = []
      elif(line.endswith("%\n")):
        cf = getConfigs(line, min_pc)
        tmpcf += cf
      elif 'g-factor' in line:
        # g-factor is last thing printed for each level
        g = get_gFactor(line)
        lst = lst + g + tmpcf
        oalist.append(lst)
        lst = []
      elif line.startswith("\n") and jp[0]==0:
        # J=0 states don't have a g-factor printed
        g = [0]
        lst = lst + g + tmpcf
        oalist.append(lst)
        lst = []
  return oalist
  # else return 1

def findGroundState(list):
  gs_en = 1.
  for el in list:
    if len(el)>2:
      en = el[3]
      if en < gs_en:
        gs_en = en
  return gs_en

def makeEnergiesRelativeGS(list, gs_en):
# Shifts energy levels elative to lowest calc'd state.
# nb: only shifts the cm^-1 energies, leaves the a.u. as 'absolute' from CI
  for el in list:
    if len(el)>2:
      el[3] -= gs_en


################################################################################
import sys

inf = ""
if len(sys.argv) < 2:
  print("Run with input file from command line.")
  print(" e.g.: ./parser.py file.txt")
  inf = input("Or, enter input file here: ")
else:
  inf = sys.argv[1]

print ("Reading:",inf)

olist = parsefile(inf)

gs_en = findGroundState(olist)

makeEnergiesRelativeGS(olist, gs_en)

oname = 'fixed_csv_'+inf
with open(oname, 'w') as f:
  for line in olist:
    for el in line:
      f.write("%s, " % el)
    f.write("\n")

print("Outputted to:",oname)
