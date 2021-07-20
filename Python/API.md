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
