import requests
import json

def processOneArea(sw_lat, sw_lon, ne_lat, ne_lon, fout):
    #r = requests.get('https://api.foursquare.com/v2/venues/search?intent=browse&sw=1,103.548&ne=1.62565,104.1155&limit=500&oauth_token=KQXHSRS4GJK2GTPELVRERURBBEJEBMDJAUPNBGM2TDIB5X00&v=20150521')
    r = requests.get('https://api.foursquare.com/v2/venues/search?intent=browse&sw=' + str(sw_lat) +',' + str(sw_lon) + '&ne=' + str(ne_lat) + ',' + str(ne_lon) + '&limit=500&oauth_token=KQXHSRS4GJK2GTPELVRERURBBEJEBMDJAUPNBGM2TDIB5X00&v=20150521')
    jsObj = json.loads(r.text)
    if (jsObj["meta"]["code"] != 200):
        return False
    venueNum = len(s["response"]["venues"])
    for venue in s["response"]["venues"]
        json.dump(venue, fout, indent = 4, ensure_ascii = False)
    return True
    

fout = open("C:\\test.json", 'w')
r = requests.get('https://api.foursquare.com/v2/venues/search?intent=browse&sw=1,103.548&ne=1.62565,104.1155&limit=500&oauth_token=KQXHSRS4GJK2GTPELVRERURBBEJEBMDJAUPNBGM2TDIB5X00&v=20150521')
#print(r.text)
#obj = json.JSONDecoder(r.text)
#obj[1]
s = json.loads(r.text)
#print s.keys()
#print s["response"]["venues"][0]

j = json.dumps(s["response"]["venues"][0], indent = 4, ensure_ascii=False)
j2 = json.dumps(s["response"]["venues"][1], indent = 4, ensure_ascii=False)
fout.write('{"venues":[')
fout.write('\n')
fout.write(j.strip().encode('utf-8'))
fout.write(',\n')
fout.write(j2.strip().encode('utf-8')+'\n')
fout.write(']}')
fout.close()

with open('c:\\test.json','r') as fin:
    js = json.load(fin)
print js['venues'][0]
#print j

processOneArea(1, 103.548, 1.62565, 104.1155)
