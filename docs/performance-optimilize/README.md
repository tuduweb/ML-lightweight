## Performance-optimize

### related tools

#### perf

使用中断来采样程序占用。

```
sudo perf report --sort comm,dso
```

#### eBPF

eBPF(extended Berkeley Packet Filter) 是一种可以在 Linux 内核中运行用户编写的程序，而不需要修改内核代码或加载内核模块的技术。

简单说，eBPF 让 Linux 内核变得可编程化了。

- Socket
- TCP/IP
- Network Device