## Test without openwrt 
docker compose is used to bundle mosquitto broker, gateway and nodes

documentation available in docs folder

In the root directory

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

```
git add .
git commit -m ""
git tag v1.0.0
git push origin v1.0.0
```
## Test with openwrt

Get started with OpenWrt [Hello world](https://openwrt.org/docs/guide-developer/helloworld/start). This article series walks you through the basics of developing new software for your router.

<img src="/docs/telematics-gatway-docker-cropped.jpg" alt="drawing" width="500"/>

Create openwrt docker image with helloworld package and run the docker container
```dockerfile
FROM scratch
ADD bin/targets/x86/64/openwrt-*.tar.gz /
COPY bin/packages/x86_64/mypackages/helloworld_1.0-1_x86_64.ipk /tmp
CMD ["/bin/sh"]
```
```sh
docker build -t openwrt-x86 -f Dockerfile.x86.yml
docker run -it openwrt-x86 sh
```
```sh
# / mkdir -p lock
# / opkg update
# / opkg install /tmp/helloworld_1.0-1_x86_64.ipk
# / ./helloworld
Hello, World
```
package source: `/home/balaji/oprnwrt-helloworld`

package manifest: `/home/balaji/mypackages/examples/helloworld`

package feed at openwrt source: `/home/balaji/openwrt-project/openwrt-23.05.0/feeds.conf`

```src-link mypackages /home/balaji/mypackages```

Compile package
balaji@balaji:~/openwrt-project/openwrt-23-05-2$ make package/helloworld/compile V=sc

