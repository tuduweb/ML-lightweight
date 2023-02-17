### 实验 - 交叉编译到AX6000路由器

#### 关键流程

```bash
export PATH=/home/tuduweb/Downloads/aarch64-linux-musl-cross/bin/:$PATH
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/aarch64-linux-musl.toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DNCNN_VULKAN=OFF -DNCNN_BUILD_EXAMPLES=ON -DNCNN_SIMPLEOMP=ON ..
```

```bash
export PATH=/home/tuduweb/Downloads/aarch64-linux-musl-cross/bin/:$PATH
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/aarch64-linux-musl.toolchain.cmake ..
```

#### 过程

```
root@XiaoQiang:~# uname -a
Linux XiaoQiang 5.4.150 #0 SMP Tue Mar 29 16:56:29 2022 aarch64 GNU/Linux
```

```

root@XiaoQiang:~# cat /proc/cpuinfo
processor       : 0
model name      : ARMv8 Processor rev 4 (v8l)
BogoMIPS        : 26.00
Features        : fp asimd evtstrm aes pmull sha1 sha2 crc32 cpuid
CPU implementer : 0x41
CPU architecture: 8
CPU variant     : 0x0
CPU part        : 0xd03
CPU revision    : 4

processor       : 1
model name      : ARMv8 Processor rev 4 (v8l)
BogoMIPS        : 26.00
Features        : fp asimd evtstrm aes pmull sha1 sha2 crc32 cpuid
CPU implementer : 0x41
CPU architecture: 8
CPU variant     : 0x0
CPU part        : 0xd03
CPU revision    : 4

processor       : 2
model name      : ARMv8 Processor rev 4 (v8l)
BogoMIPS        : 26.00
Features        : fp asimd evtstrm aes pmull sha1 sha2 crc32 cpuid
CPU implementer : 0x41
CPU architecture: 8
CPU variant     : 0x0
CPU part        : 0xd03
CPU revision    : 4

processor       : 3
model name      : ARMv8 Processor rev 4 (v8l)
BogoMIPS        : 26.00
Features        : fp asimd evtstrm aes pmull sha1 sha2 crc32 cpuid
CPU implementer : 0x41
CPU architecture: 8
CPU variant     : 0x0
CPU part        : 0xd03
CPU revision    : 4
```

通过`aarch64 GNU/Linux`信息，使用`aarch64-linux-gnu`编译，发现编译好的程序，在路由器中有*(暂时)无法理解的特性*:
```
root@XiaoQiang:/data/tuduweb# ./main
-ash: ./main: not found
```

通过在群里询问，和以前的一些经验，使用`ldd`工具查看`shared`信息：

```
root@XiaoQiang:/data/tuduweb# ldd main
        /lib/ld-linux-aarch64.so.1 (0x7fa233f000)
        libc.so.6 => /lib/ld-linux-aarch64.so.1 (0x7fa233f000)
```

但是在系统中无法找到这个库：

```bash
ls /lib
find / -name "ld-linux-aarch64.so.1"
```

通过`ls /lib`，可以看到在`lib`下有个类似的文件`ld-musl-aarch64.so.1`：

```
root@XiaoQiang:/data/tuduweb# ls /lib
config                  functions               libfstools.so           librpc.so               mediatek.sh             uci
config_post_ota         functions.sh            libgcc_s.so.1           libsetlbf.so            mimesh                  upgrade
dnsmasq                 ld-musl-aarch64.so.1    libjson_script.so       libubox.so              modules                 wifi
firewall                lib.scripthelper.sh     libkvcutil.so           libubus.so              netifd                  xq-misc
firewall.sysapi.loader  libblobmsg_json.so      libkvcutil.so.1         libuci.so               network                 xqled
firmware                libc.so                 libkvcutil.so.1.0.0.0   libvalidate.so          preinit
```

在网上检索`musl`可知，这个是一个精简的libc库。

> musl 是一个全新为 Linux 基本系统实现的标准库。特点是轻量级、快速、简单、免费、标准兼容和安全。

在网上，找到相关的编译工具：

https://github.com/lovell/aarch64-linux-musl-crosstools

后续通过新的编译工具，交叉编译，成功编译该软件在路由器上运行helloworld程序。

`aarch64-linux-musl-g++ demo.c -o demo`

```c
#include <stdio.h>

int main() {
    printf("hello world\n");
    return 0;
}
```

### 实验 - Ubuntu-Linux-demo

#### CPP - hello world

```
tuduweb@tuduweb-X1E-ubuntu:~/development/lightweight/ML-lightweight/experiment/pi4-cross-compiling/build-aarch64$ ldd cppmain 
        linux-vdso.so.1 (0x00007ffea99b7000)
        libstdc++.so.6 => /lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f2721c67000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f2721a75000)
        libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f2721926000)
        /lib64/ld-linux-x86-64.so.2 (0x00007f2721e70000)
        libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f272190b000)
```

```
root@XiaoQiang:/data/tuduweb# ./cppmain
hello world
root@XiaoQiang:/data/tuduweb# ldd cppmain
        /lib/ld-musl-aarch64.so.1 (0x7fb0e33000)
        libstdc++.so.6 => /usr/lib/libstdc++.so.6 (0x7fb0c80000)
        libgcc_s.so.1 => /lib/libgcc_s.so.1 (0x7fb0c5d000)
        libc.so => /lib/ld-musl-aarch64.so.1 (0x7fb0e33000)
```

#### CPP - ubuntu demo

查看链接信息
```
tuduweb@tuduweb-X1E-ubuntu:~/development/lightweight/ML-lightweight/experiment/ubuntu-intro/build$ ldd ubuntu-intro
        linux-vdso.so.1 (0x00007ffe20938000)
        libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007fda01848000)
        libstdc++.so.6 => /lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007fda01666000)
        libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007fda01517000)
        libmvec.so.1 => /lib/x86_64-linux-gnu/libmvec.so.1 (0x00007fda014eb000)
        libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007fda014d0000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fda012de000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fda02087000)
```

```
root@XiaoQiang:/data/tuduweb# ldd ubuntu-intro
        /lib/ld-linux-aarch64.so.1 (0x7fb0552000)
        libpthread.so.0 => /lib/ld-linux-aarch64.so.1 (0x7fb0552000)
        libstdc++.so.6 => /usr/lib/libstdc++.so.6 (0x7faffb5000)
        libm.so.6 => /lib/ld-linux-aarch64.so.1 (0x7fb0552000)
        libgcc_s.so.1 => /lib/libgcc_s.so.1 (0x7faff92000)
        libc.so.6 => /lib/ld-linux-aarch64.so.1 (0x7fb0552000)
Error loading shared library ld-linux-aarch64.so.1: No such file or directory (needed by ubuntu-intro)
Error relocating ubuntu-intro: __sprintf_chk: symbol not found
Error relocating ubuntu-intro: __fprintf_chk: symbol not found
```

最终，编译运行成功
```
root@XiaoQiang:/data/tuduweb# ./intro-musl
0.496094 0.768066 0.088440 0.131958 0.307373 0.633789 0.489990 0.895996 0.455566 0.631836 0.348877 0.401611 0.022293 0.168823 0.293701 0.518066 0.697266 0.799805 0.160889 0.282227 0.681152 0.915039 0.396973 0.874023 0.419189 0.552734 0.952637 0.036194 0.185181 0.373291 0.304932 0.931641 0.175781 0.269775 0.150635 0.031677 0.208008 0.929688 0.722656 0.742188 0.525879 0.243652 0.584473 0.033173 0.138672 0.242188 0.815430 0.792969 0.278076 0.481934 0.819336 0.997070 0.698242 0.567383 0.834961 0.205566 0.592773 0.112244 0.153442 0.241699 0.726074 0.700684 0.203735
------------------------
0.066045 0.072331 0.069038 0.074826 0.072655 0.073105 0.075867 0.072308 0.066691 0.073362 0.076217 0.069008 0.070446 0.068099
```