![](docs/)
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

git add .
git commit -m ""
git tag v1.0.0
git push origin v1.0.0

After testing: 
create directory and makefile under the directory, 
package/feeds/telematics/telematics-gateway
- Makefile

make package/telematics-gateway/prepare V=s
make package/telematics-gateway/compile V=s



	$(MKDIR) $(PKG_BUILD_DIR)
	$(CP) ./gateway/* $(PKG_BUILD_DIR)/
