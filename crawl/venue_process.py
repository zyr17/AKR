import sys
reload(sys)
sys.setdefaultencoding( "utf-8" )

import json

fin = open("G:\\workspace\\Foursquare\\venue\\Singapore\\step = 0.005\\venues.json",'r')
fout = open("G:\\workspace\\venues.txt", 'w')
fout_s = open("G:\\workspace\\venues_simple.txt", 'w')
jsObj = json.load(fin)

for venue in jsObj["venues"]:
    fout.write(venue["id"] + ',' + str(venue["location"]["lat"]) + ',' + str(venue["location"]["lng"]) + ',' + venue["categories"][0]["shortName"] + ',' + venue["categories"][0]["id"] + '\n')
    fout_s.write(str(venue["location"]["lat"]) + ' ' + str(venue["location"]["lng"]) + '\n')

fout_s.close()
fout.close()


