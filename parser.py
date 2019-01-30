#!/usr/bin/python3

print ('ok')

def extractEnergies(line):
  a =  line.split()[1:3]
  enau = float(a[0])
  encm = float(a[1])
  return [enau,encm]

def getJandP(line):
  line = line.replace(',',' ')
  a = line.split()
  #print(a[4],a[7])
  j = int(a[4])
  p = -1
  if a[7]=='even':
    p=1
  #print(j,p)
  return [j,p]

def getConfigs(line):
  line = line.strip('\n')
  a=line.split('  ')
  #print(a[-2:])
  st = a[-2]
  pc = float(a[-1].strip('%'))
  if pc>20.:
    return [st,pc]
  return []

def get_gFactor(line):
  a = line.split()
  g = float(a[2])
  return [g]

def parsefile(fn):
  import os
  oalist = []
  #fn = "dat.txt"
  if os.path.exists(fn):
    file = open(fn, "r")
    jp = []
    lst = []
    good_part = False
    for line in file:
      if(line.startswith("Solutions for J = ")):
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
        cf = getConfigs(line)
        tmpcf += cf
      elif 'g-factor' in line:
        g = get_gFactor(line)
        lst = lst + g + tmpcf
        oalist.append(lst)
        lst = []
      elif line.startswith("\n") and jp[0]==0:
        g = [0]
        lst = lst + g + tmpcf
        oalist.append(lst)
        lst = []
  return oalist

def findGroundState(list):
  gs_en = 1.
  for el in list:
    en = el[3]
    if en < gs_en:
      gs_en = en
  return gs_en

def makeEnergiesRelativeGS(list, gs_en):
  for el in list:
    el[3] -= gs_en



import sys
inf = sys.argv[1]
print ("Reading:",inf)

olist = parsefile(inf)

gsen = findGroundState(olist)
#print (gsen)

makeEnergiesRelativeGS(olist, gsen)

#for el in olist:
#  print(el)

oname = 'fixed_csv_'+inf
with open(oname, 'w') as f:
  for line in olist:
    for el in line:
      f.write("%s, " % el)
    f.write("\n")















