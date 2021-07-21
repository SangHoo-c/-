"""
https://openweathermap.org/api/one-call-api

1. 고유의 key 사용 
2. API 로 json 데이터를 받아온 후, 원하는 정보 추출. 

"""import requests
import os
from twilio.rest import Client
from dotenv import dotenv_values

config = dotenv_values(".env")

if __name__ == '__main__':
    # API 와 연동하기 위해서 사용되는 접근키(access key) & 보안키(secret key) 와 같은 인증 정보는 코드상에 저장하면 안된다.
    # env 파일을 통해 환경변수 설정 => gitignore 파일에 어떤 환경변수가 필요한지, 정리해놓기.
    # 방법 1. os 모듈 사용.
    # os.environ[] 개체를 사용하여 환경변수 설정.
    # os.environ.get() 을 통해 key-value 로 환경변수를 받아온다.

    # 방법 2. python-dotenv 사용.
    # .env 파일을 따로 정의한다. 

    api_key = config['api_key']
    MY_LON = config['MY_LON']
    MY_LAT = config['MY_LAT']
    WEATHER_Endpoint = config['WEATHER_Endpoint']
    TWILIO_ACCOUNT_SID = config['TWILIO_ACCOUNT_SID']
    TWILIO_AUTH_TOKEN = config['TWILIO_AUTH_TOKEN']

    params = {"lat": MY_LAT,
              "lon": MY_LON,
              "appid": api_key,
              "exclude": "current,minutely,daily"
              }
    response = requests.get(url=WEATHER_Endpoint, params=params)
    response.raise_for_status()

    is_rain = False
    weather_data = response.json()
    if is_rain:
        account_sid = os.environ['TWILIO_ACCOUNT_SID']
        auth_token = os.environ['TWILIO_AUTH_TOKEN']
        client = Client(account_sid, auth_token)

        message = client.messages \
            .create(
            body='McAvoy or Stewart? These timelines can get so confusing.',
            from_='+15017122661',
            status_callback='http://postb.in/1234abcd',
            to='+15558675310'
        )

        print(message.sid)


def rain_check_1():
    global is_rain
    weather_slice = weather_data["hourly"][:12]
    for hour_data in weather_slice:
        condition_code = hour_data["weather"][0]["id"]
        if condition_code <= 700:
            print("bring your umbrella!")
            is_rain = True


# 2.
def rain_check_2():
    global is_rain
    weather_code_list = []
    for idx in range(12):
        hour = weather_data["hourly"][idx]
        # by using datetime module.
        # converting unix timestamp string to readable data.
        # print(datetime.utcfromtimestamp(hour['dt']).strftime('%Y-%m-%d %H:%M:%S'))
        weather_code_list.append(hour["weather"][0]["id"])
        if hour["weather"][0]["id"] <= 700:
            print("bring your umbrella!")
            is_rain = True



