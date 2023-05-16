### 实验

```
root@XiaoQiang:/data/tuduweb# ./intro-musl2
-ash: ./intro-musl2: Permission denied
```

#### 编译器版本

https://github.com/lovell/aarch64-linux-musl-crosstools

Snapshot of http://more.musl.cc/10.2.1/x86_64-linux-musl/aarch64-linux-musl-cross.tgz

#### 步骤

##### 编译NCNN库

```shell
export PATH=/home/n504/onebinary/aarch64-linux-musl-cross/bin/:$PATH
mkdir build-aarch64-linux-musl
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/aarch64-linux-musl.toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DNCNN_VULKAN=OFF -DNCNN_BUILD_EXAMPLES=ON -DNCNN_SIMPLEOMP=ON ..
make -j8
make install
```

##### 设置软件环境

需要设置`CMakeLists.txt`中的`ncnn_DIR`等。

```shell
export PATH=/home/n504/onebinary/aarch64-linux-musl-cross/bin/:$PATH
mkdir build
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/aarch64-linux-musl.toolchain.cmake ..
make -j8
```

##### 将软件传输到目标设备上

```
scp 
```

#### QA

##### `Error relocating`

```
Error relocating runner-musl: _ZSt28__throw_bad_array_new_lengthv: symbol not found
```

编译器环境问题，将编译器版本设置为`10.2.1`而不是最新版本，问题解决