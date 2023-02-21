### 实验 - perf的使用

```bash
sudo perf record -F 10000 -g ./perf-intro
sudo perf report
```

其中，`-g`参数为显示调用关系。通过`e/c`两个快捷键，展开和收缩每一项。