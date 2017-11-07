import json, requests
url = 'https://api.foursquare.com/v2/venues/search'
ID = 'XXEJHJYVVFEPBXB4M5HGNWFIPQQ4TQWREYDWYUQ0XSMKCVVU'
SECRET = 'KVLGAERLAFNEWHEWIM43L4EUAQJ2PE4INNHU2CIBLUWAY5RL'


params = dict(
  client_id=ID,
  client_secret=SECRET,
  v='20170801',
  sw='31.30164,121.50761',
  ne='31.30521,121.51053',
  intent='browse',
  limit=50
)
resp = requests.get(url=url, params=params)
data = json.loads(resp.text)
f = open('out.txt', 'w')
f.write(str(data))