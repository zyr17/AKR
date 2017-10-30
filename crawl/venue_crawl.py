import requests
import json
import warnings
import time
warnings.filterwarnings("ignore")


def processOneArea(sw_lat, sw_lon, ne_lat, ne_lon, fout):
    #r = requests.get('https://api.foursquare.com/v2/venues/search?intent=browse&sw=' + str(sw_lat) +',' + str(sw_lon) + '&ne=' + str(ne_lat) + ',' + str(ne_lon) + '&limit=50&oauth_token=KQXHSRS4GJK2GTPELVRERURBBEJEBMDJAUPNBGM2TDIB5X00&v=20150522')
    r = requests.get('https://api.foursquare.com/v2/venues/search?intent=browse&sw=' + str(sw_lat) +',' + str(sw_lon) + '&ne=' + str(ne_lat) + ',' + str(ne_lon) + '&limit=50&client_id=URNDEZT0JILCO03CY24F55J455NVPAD3S023OD4IXGVO141D&client_secret=BTP1EEZSV1V51NBC5XYMRG1D2GQKNIC42SIMCX245ETPLKIZ&v=20150522')
        
    jsObj = json.loads(r.text)
    if (jsObj["meta"]["code"] != 200): #meet some errors
        print 'error occurs, code = ' + str(jsObj["meta"]["code"]) + ', errorType = ' + jsObj["meta"]["errorType"] + ', errorDetail = ' + jsObj["meta"]["errorDetail"] + "at  lat = " + str(sw_lat) + ", lon = " + str(sw_lon) 
        return False
    venueNum = len(jsObj["response"]["venues"])
    print "venue count = " + str(venueNum) + ", lat = " + str(sw_lat) + ", lon = " + str(sw_lon)
    for venue in jsObj["response"]["venues"]:
        #json.dump(venue, fout, indent = 4, ensure_ascii = False)
        venueStr = json.dumps(venue, indent = 4, ensure_ascii = False)
        fout.write(venueStr.strip().encode('utf-8'))
        fout.write(',\n')        
    return True

def testJSON():
    with open('c:\\test.json','r') as fin:
        jsObj = json.load(fin)
    print len(jsObj["venues"])
    

fout = open("G:\\workspace\\venues.json", 'w')
fout.write('{"venues":[')
lat_step = 0.005 #about 100m
lon_step = 0.005
lat = minlat = 1.0
maxlat = 1.62565
lon = minlon = 103.548
maxlon = 104.1155

while (lat <= maxlat):
    lon = minlon
    while (lon <= maxlon):
        result = processOneArea(lat, lon, lat+lat_step, lon+lon_step, fout)
        while (result == False):
            result = processOneArea(lat, lon, lat+lat_step, lon+lon_step, fout)
        time.sleep(0.4)
        lon += lon_step
    lat += lat_step


fout.write(']}')
fout.close()

print "done"

#testJSON()