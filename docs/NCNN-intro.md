### NCNN环境配置入门

需要在win10下进行相关测试，所以需要在平台上配置运行。

#### 依赖环境

protobuf、opencv

##### protobuf安装

`protobuf-3.4.0`（网上教程推荐的版本，其它版本未测试）

https://github.com/google/protobuf/archive/v3.4.0.zip

通过以下方式，或者进行vs2019x64的编译环境（CMD）

```c++
cd <protobuf-root-dir>
mkdir build
cd build

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall.bat" amd64

cmake -G"NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%cd%/install -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_MSVC_STATIC_RUNTIME=OFF ../cmake

nmake
nmake install
```

注意这里编译的是Release版本，有时候需要Debug版本。

##### 编译opencv

可以直接下载编译好的版本：https://opencv.org/releases/

解压后，记住地址，用于后续流程。

#### NCNN

##### 编译、构建NCNN

在完成依赖环境的编译后，就可以对NCNN进行编译了。

将工程`clone`到本地：

```bash
git clone --recursive https://github.com/Tencent/ncnn.git
```

编译`ncnn`，通过以下方式，或者进行vs2019x64的编译环境（CMD）：

```
cd <ncnn-root-dir>
mkdir -p build
cd build

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvarsall.bat" amd64

cmake -G"NMake Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%cd%/install -DProtobuf_INCLUDE_DIR=<protobuf-root-dir>/build/install/include -DProtobuf_LIBRARIES=<protobuf-root-dir>/build/install/lib/libprotobuf.lib -DProtobuf_PROTOC_EXECUTABLE=<protobuf-root-dir>/build/install/bin/protoc.exe -DNCNN_VULKAN=OFF ..

nmake
nmake install
```

##### 构建vs2019工程

（这种方式需要改变为通过qmake、cmake更高级的方式进行）

主要是添加头文件目录、链接器中添加`lib`目录，并输入链接源到input中。

1. 在调试设置中，`VC++ Directory , IncludePath`里添加：

   ```
   D:\opencv\build\include
   D:\opencv\build\include\opencv2
   D:\protobuf-3.4.0\build-2022\install\include
   D:\github\ncnn\build2022\install\include\ncnn
   D:\github\ncnn\build2022\install\include\
   ```

2. 添加`Library Directories`：

   ```
   D:\opencv\build\x64\vc15\lib
   D:\github\ncnn\build2022\install\lib
   D:\protobuf-3.4.0\build-2022\install\lib
   ```

3. 在`Linker , Input`中添加依赖：

   ```
   opencv_world460d.lib
   ncnnd.lib
   libprotobuf.lib
   ```

   其中以`d`结尾的是`debug`类型。

#### 测试NCNN

在设置好的工程下，添加相关程序，测试NCNN可行性。在这里使用`NanoDet`进行测试：

相关代码移植自`nanodet`官方仓库的`ncnn`实例：

https://github.com/RangiLyu/nanodet/tree/main/demo_ncnn