"""
HTTP Request 

GET / POST / PUT / DELETE

"""

import requests
from datetime import datetime

USERNAME = "sanghoo"
TOKEN = "asdf897adsfcxz6v87casdf"  # secret token
GRAPH_ID = "sju16"
EDIT_DATE = "20210722"

pixela_endpoint = "https://pixe.la/v1/users"
graph_endpoint = f"{pixela_endpoint}/{USERNAME}/graphs"
pixel_creation_endpoint = f"{pixela_endpoint}/{USERNAME}/graphs/{GRAPH_ID}"
pixel_edit_endpoint = f"{pixela_endpoint}/{USERNAME}/graphs/{GRAPH_ID}/{EDIT_DATE}"


user_params = {
    "token": TOKEN,
    "username": USERNAME,
    "agreeTermsOfService": "yes",
    "notMinor": "yes",
}

# 1. create user account
# response = requests.post(url=pixela_endpoint, json=user_params)
# print(response.text)

# 2. create a graph definition
#
# request header,

graph_config = {
    "id": GRAPH_ID,
    "name": "Commit Graph",
    "unit": "commit",
    "type": "int",
    "color": "sora",
}

# kwargs, keyword arguments
# authenticate by using header.
headers = {
    "X-USER-TOKEN": TOKEN
}

# response = requests.post(url=graph_endpoint, json=graph_config, headers=headers)
# print(response.text)

today = datetime.now()

# create pixel data
pixel_data = {
    "date": today.strftime("%Y%m%d"),
    "quantity": "12",
}

re_pixel_data = {
    "quantity": "12",
}

# response = requests.post(url=pixel_creation_endpoint, json=pixel_data, headers=headers)
# print(response.text)

EDIT_DATE = "20210723"

edit_response = requests.put(url=pixel_edit_endpoint, json=re_pixel_data, headers=headers)
print(edit_response.text)
