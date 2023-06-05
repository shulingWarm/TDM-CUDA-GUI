# 介绍

本项目是[TDM-CUDA](https://github.com/shulingWarm/TDM-CUDA)的用户界面，用于生成数字正射影像。

# 构建

## 环境依赖

#### QT

#### ZeroMq

需要注意zmq需要使用源码编译的方式，这样才能在CMakeLists.txt里面用find_package找到它。

```shell
git clone https://github.com/zeromq/libzmq.git
cd libzmq
mkdir build
cd build
cmake ..
make
sudo make install
```

## 编译和运行

在安装好Qt的情况下，用Qt Creator打开src/CMakeLists.txt即可

# 视频使用说明
[演示视频](https://www.bilibili.com/video/BV1xz4y1q7iG)

