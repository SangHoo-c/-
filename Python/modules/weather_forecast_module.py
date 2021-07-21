"""
https://openweathermap.org/api/one-call-api

1. 고유의 key 사용 
2. API 로 json 데이터를 받아온 후, 원하는 정보 추출. 

"""

from datetime import datetime

import requests

api_key = '78068677d75f4cb5f080b78e97c2a5ef'
MY_LON = 126.977
MY_LAT = 37.5683
WEATHER_Endpoint = 'https://api.openweathermap.org/data/2.5/onecall'

params = {'lat': MY_LAT, 'lon': MY_LON, 'appid': api_key}
response = requests.get(url=WEATHER_Endpoint, params=params)
response.raise_for_status()

for hour in response.json()['hourly']:
    # by using datetime module.
    # converting unix timestamp string to readable data.
    print(datetime.utcfromtimestamp(hour['dt']).strftime('%Y-%m-%d %H:%M:%S'))
    print(hour['weather'])
