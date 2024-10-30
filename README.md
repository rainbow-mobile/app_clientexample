# app_clientexample

web_mobile_server의 API와 연동가능한 Qt프로젝트 예제

## 목차

- [Install](#설치)
  
## Install

필요 : Qt Creator 5.15 , socket.io-client-cpp

1. Git 다운로드
```bash
git clone https://github.com/rainbow-mobile/app_clientexample.git
```

2. socket.io-client-cpp 다운로드
```bash
sudo apt-get install libboost-all-dev
sudo apt-get install libssl-dev
cd ~
git clone https://github.com/socketio/socket.io-client-cpp.git
cd socket.io-client-cpp
mkdir build
cd build
cmake ..
make
```

3. Qt Creator로 app_clientexample/ClientExample.pro 열기 후 빌드
