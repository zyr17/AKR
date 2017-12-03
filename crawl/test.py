import json, requests, os, time
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
		except:
			print('request timeout')
		time.sleep(1)
		if data['meta']['code'] != 200:
			print ('error code', data['meta']['code'])
		else:
			break
	
	return data['response']['venues']

def num2str(a, b, c, d):
	return str(a) + ' ' + str(b) + ' ' + str(c) + ' ' + str(d)

def fullarea():
	f = open('queue.txt', 'r')
	lines = [x.strip() for x in f.readlines()]
	f.close()
	#print(lines)
	if len(lines) == 0:
		return False;
	[sw1, sw2, ne1, ne2] = [float(x) for x in lines[0].split(' ')]
	data = oneblock(sw1, sw2, ne1, ne2)
	f = open('result/' + lines[0] + '.json', 'w')
	f.write(json.dumps(data, ensure_ascii=False))
	f.close()
	'''
	print (len(data))
	raw_input()
	'''
	if len(data) == 50:
		if ne1 - sw1 > ne2 - sw2:
			p = (ne1 + sw1) / 2
			delta = (ne1 - sw1) / 10
			lines.append(num2str(sw1, sw2, p + delta, ne2))
			lines.append(num2str(p - delta, sw2, ne1, ne2))
		else:
			p = (ne2 + sw2) / 2
			delta = (ne2 - sw2) / 10
			lines.append(num2str(sw1, sw2, ne1, p + delta))
			lines.append(num2str(sw1, p - delta, ne1, ne2))
	del(lines[0])
	#print(lines)
	f = open('queue.txt', 'w')
	f.write('\n'.join(lines))
	return True



'''
full shanghai
sw1 = 30.9682
sw2 = 121.1092
ne1 = 31.4064
ne2 = 121.8301
'''
'''
wujiaochang
sw1 = 31.2975
sw2 = 121.5066
ne1 = 31.3112
ne2 = 121.5164
'''

sw1 = 31.30164
sw2 = 121.50761
ne1 = 31.30521
ne2 = 121.51053
if not os.path.exists('queue.txt'):
	f = open('queue.txt', 'w')
	f.write(str(sw1) + ' ' + str(sw2) + ' ' + str(ne1) + ' ' + str(ne2) + '\n')
	f.close()
while fullarea():
	pass
#f = open('out.txt', 'w')
#f.write(json.dumps(data, ensure_ascii=False))