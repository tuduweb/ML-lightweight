### 实验 - 入门测试

1. Cover the `model` from pytorch type to ncnn VIA **pnnx**
2. Modify *unsupport* layer or implement new layer
3. Make input test data

#### 一些中间遇到的问题

1.  `not exist` and read access violation

    ```
    layer F.log_softmax not exists or registered
    ```

    将`.param`中的层类型修改成了`Softmax`，注意首字母大写

2. `float` datatype
   
   注意在NCNN中，只找到了`ncnn::Mat::from_float16`这一种从普通数据转换成内置类型`Mat`的方法。
   但是在实验的环境win10中，float并不是float16类型，而是float32类型。需要将类型转换，在这里，从stackoverflow上找到了一串转换程序去修改实现。 
