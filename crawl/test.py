import json, requests
url = 'https://api.foursquare.com/v2/venues/search'
ID = 'XXEJHJYVVFEPBXB4M5HGNWFIPQQ4TQWREYDWYUQ0XSMKCVVU'
SECRET = 'KVLGAERLAFNEWHEWIM43L4EUAQJ2PE4INNHU2CIBLUWAY5RL'

def oneblock(sw1, sw2, ne1, ne2):
	print ('start query', sw1, sw2, ne1, ne2)
	params = dict(
		client_id=ID,
		client_secret=SECRET,
		v='20170801',
		sw = str(sw1) + ',' + str(sw2),
		ne = str(ne1) + ',' + str(ne2),
		intent='browse',
		limit=50
	)
	while True:
		try:
			resp = requests.get(url=url, params=params, timeout = 10)
			data = json.loads(resp.text)
			break
		except:
			print('request timeout')
	
	if data['meta']['code'] != 200:
		print ('error code', data['meta']['code'])
		raw_input()
	
	return data['response']['venues']

def fullarea(sw1, sw2, ne1, ne2):
	data = oneblock(sw1, sw2, ne1, ne2)
	'''
	print (len(data))
	raw_input()
	'''
	if len(data) < 50:
		return data
	if ne1 - sw1 > ne2 - sw2:
		p = (ne1 + sw1) / 2
		delta = (ne1 - sw1) / 10
		data1 = fullarea(sw1, sw2, p + delta, ne2)
		data2 = fullarea(p - delta, sw2, ne1, ne2)
		for i in data2:
			data1.append(i)
		return data1
	p = (ne2 + sw2) / 2
	delta = (ne2 - sw2) / 10
	data1 = fullarea(sw1, sw2, ne1, p + delta)
	data2 = fullarea(sw1, p - delta, ne1, ne2)
	for i in data2:
		data1.append(i)
	return data1






sw1 = 31.30164
sw2 = 121.50761
ne1 = 31.30521
ne2 = 121.51053
data = fullarea(sw1, sw2, ne1, ne2)
f = open('out.txt', 'w')
f.write(json.dumps(data, ensure_ascii=False))