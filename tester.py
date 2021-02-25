import requests
import json
import sys
 
# some JSON:
x =  '{ "name":"John", "age":30, "city":"New York"}'

url = 'http://127.0.0.1:5000/updateState/999'

print(sys.argv)
if int(sys.argv[1]) == 0:
    print("Updating device state using method 0")
    deviceState = {"appliance_name": "Insert Cool Device Name Here", "device_state": 1, "device_battery":3.6}
    deviceState = json.dumps(deviceState)
    print(deviceState)
    print(requests.patch(url, json=deviceState ))

elif int(sys.argv[1]) == 1:
    print("Updating device state using method 1")
    deviceState = {"appliance_name": "Other Device test", "device_state": 0, "device_battery":4.2}
    deviceState = json.dumps(deviceState)
    print(deviceState)
    print(requests.patch(url, json=deviceState ))

else:
    print("Updating device state using method 2")
    deviceState = {"appliance_name": "Wierd Device State", "device_state": 99, "device_battery":3.0}
    deviceState = json.dumps(deviceState)
    print(deviceState)
    print(requests.patch(url, json=deviceState ))


