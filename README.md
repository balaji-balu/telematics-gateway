docker compose is used to bundle mosquitto broker, gateway and nodes
documentation available in docs folder

go to the root directory

run this command
```
docker compose up --build -d
```

if not installed on your host, install mosquito clients
```
sudo apt  install mosquitto-clients
```
run broker
```
mosquitto_sub -h localhost -p 1883 -t "#" -v
```

After testing: 
create directory and makefile under the directory, 
package/feeds/telematics/telematics-gateway
- Makefile