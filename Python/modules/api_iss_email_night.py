"""
2021.7.20
python API 학습
1. ISS api 위치 데이터 확인 
2. 현재 위도/경도에 맞는 일몰 / 일출 시간 체크 
3. email SMTP 보내기 
"""


import time

import requests
from datetime import datetime
import smtplib


MY_EMAIL = "****"
MY_PASSWORD = "****"
MY_LAT = 37.0  # Your latitude
MY_LONG = -4.0  # Your longitude

response = requests.get(url="http://api.open-notify.org/iss-now.json")
response.raise_for_status()
data = response.json()

iss_latitude = float(data["iss_position"]["latitude"])
iss_longitude = float(data["iss_position"]["longitude"])


# Your position is within +5 or -5 degrees of the ISS position.
def check_iss_position():
    if iss_longitude - 5 <= MY_LONG <= iss_longitude + 5 and iss_latitude - 5 <= MY_LAT <= iss_latitude + 5:
        return True
    return False


# If the ISS is close to my current position
# and it is currently dark
def nigth_check():
    parameters = {
        "lat": MY_LAT,
        "lng": MY_LONG,
        "formatted": 0,
    }

    response = requests.get("https://api.sunrise-sunset.org/json", params=parameters)
    response.raise_for_status()
    data = response.json()
    sunrise = int(data["results"]["sunrise"].split("T")[1].split(":")[0])
    sunset = int(data["results"]["sunset"].split("T")[1].split(":")[0])

    time_now = datetime.now().hour

    if time_now < sunrise or sunset < time_now:
        return True
    return False


# iss 의 위치 확인 & 밤이라면, 이메일을 보낸다.
def send_email():
    if check_iss_position() and nigth_check():
        connection = smtplib.SMTP("smtp.gmail.com")
        connection.starttls()
        connection.login(MY_EMAIL, MY_PASSWORD)
        connection.sendmail(
            from_addr=MY_EMAIL,
            to_addrs=MY_EMAIL,
            msg="LOOK UP \n\n THE ISS ABOVE YOU IN THE SKY."
        )

# 60 초마다 send_email 호출
if __name__ == '__main__':
    while True:
        time.sleep(60)
        send_email()




