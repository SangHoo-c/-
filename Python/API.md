### API

Application Programming Interface 

- set of commands, functions, protocols, objects
- programmers can use to create software or interact with an external system.
- **Interface **, request, response, Data 

#### API Endpoints 

URL, location where 000 data could found.

- make request over a internet. 

```python
import requests

response = requests.get(url='http://api.open-notify.org/iss-now.json')
response.raise_for_status()     # 유효한 주소인지 체크 

data = response.json()
longitude = data["iss_position"]["longitude"]
latitude = data["iss_position"]["latitude"]

iss_position = (longitude, latitude)
print(iss_position)
```


### Authentication

API key : personal account number & password, API provider 가 사용자가 얼마나 사용중인지, 관리, 가능하게 한다. 

```
api.openweathermap.org/data/2.5/weather?q=Seoul&appid=78068677d75f4cb5f080b78e97c2a5ef
https://api.openweathermap.org/data/2.5/onecall?lat=37.5683&lon=126.9778&appid=78068677d75f4cb5f080b78e97c2a5ef
```



### Environment Variables

1. Convenicence : you can change your environment variables without touching your main code. 
2. Security : away from where our code.  

```python
$ export OWN_API_KEY=6824397asdc143924
$ env  
=> 한줄 커맨드로 가능 
export OWN_API_KEY=6824397asdc143924; export AUTH_TOKEN=14dasd234sdf; python3 main.py
--------
import os 
api_key = os.environ.get("OWN_API_KEY")
```



