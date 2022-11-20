"""
2022-08-08  13:13:02
"""
from __future__ import print_function
from torchsummary import summary
import torch.nn as nn
import torch.nn.functional as F
import math
import torch


class SamePad2d(nn.Module):
    # Mimics tensorflow's 'SAME' padding.

    def __init__(self, kernel_size, stride):
        super(SamePad2d, self).__init__()
        self.kernel_size = torch.nn.modules.utils._pair(kernel_size)
        self.stride = torch.nn.modules.utils._pair(stride)

    def forward(self, input):
        in_width = input.size()[2]
        in_height = input.size()[3]
        out_width = math.ceil(float(in_width) / float(self.stride[0]))
        out_height = math.ceil(float(in_height) / float(self.stride[1]))
        pad_along_width = ((out_width - 1) * self.stride[0] +
                           self.kernel_size[0] - in_width)
        pad_along_height = ((out_height - 1) * self.stride[1] +
                            self.kernel_size[1] - in_height)
        pad_left = math.floor(pad_along_width / 2)
        pad_top = math.floor(pad_along_height / 2)
        pad_right = pad_along_width - pad_left
        pad_bottom = pad_along_height - pad_top
        return F.pad(input, (pad_left, pad_right, pad_top, pad_bottom), 'constant', 0)

    def __repr__(self):
        return self.__class__.__name__

class EvoCNNModel(nn.Module):
    def __init__(self):
        super(EvoCNNModel, self).__init__()
        # all unit
        self.pad0 = SamePad2d(kernel_size=(6, 6), stride=(1, 1))
        self.op0 = nn.Conv2d(in_channels=1, out_channels=44, kernel_size=(6, 6), stride=(1, 1), padding=0)
        nn.init.normal_(self.op0.weight, -0.965782, 0.047517)
        self.norm0 = nn.BatchNorm2d(44)
        self.pad2 = SamePad2d(kernel_size=(3, 3), stride=(1, 1))
        self.op2 = nn.Conv2d(in_channels=44, out_channels=27, kernel_size=(3, 3), stride=(1, 1), padding=0)
        nn.init.normal_(self.op2.weight, -0.812050, 0.639912)
        self.norm2 = nn.BatchNorm2d(27)
        self.pad3 = SamePad2d(kernel_size=(18, 18), stride=(1, 1))
        self.op3 = nn.Conv2d(in_channels=27, out_channels=16, kernel_size=(18, 18), stride=(1, 1), padding=0)
        nn.init.normal_(self.op3.weight, 0.536160, 0.659682)
        self.norm3 = nn.BatchNorm2d(16)
        self.pad4 = SamePad2d(kernel_size=(20, 20), stride=(1, 1))
        self.op4 = nn.Conv2d(in_channels=16, out_channels=50, kernel_size=(20, 20), stride=(1, 1), padding=0)
        nn.init.normal_(self.op4.weight, 1.000000, 0.900935)
        self.norm4 = nn.BatchNorm2d(50)
        self.op6 = nn.Linear(in_features=450, out_features=10)
        nn.init.normal_(self.op6.weight, -0.455591, 0.887854)
        self.norm6 = nn.BatchNorm1d(10)


    def forward(self, x):
        out_0 = self.pad0(x)
        out_0 = self.op0(out_0)
        out_0 = self.norm0(out_0)
        out_1 = F.max_pool2d(out_0, 3)
        out_2 = self.pad2(out_1)
        out_2 = self.op2(out_2)
        out_2 = self.norm2(out_2)
        out_3 = self.pad3(out_2)
        out_3 = self.op3(out_3)
        out_3 = self.norm3(out_3)
        out_4 = self.pad4(out_3)
        out_4 = self.op4(out_4)
        out_4 = self.norm4(out_4)
        out_5 = F.max_pool2d(out_4, 3)
        out_5 = out_5.view(out_5.size(0), -1)
        out_6 = self.op6(out_5)
        out_6 = self.norm6(out_6)
        return out_6


if __name__ == '__main__':
    model = EvoCNNModel()
    #summary(model.cuda(), (1, 32, 32), batch_size=16)

    model.load_state_dict(torch.load('./model/indi00031_00028.pt'), False)  # 加载训练好的pth模型
    batch_size = 1  # 批处理大小
    input_shape = (1, 32, 32)  # 输入数据,我这里是灰度训练所以1代表是单通道，RGB训练是3，128是图像输入网络的尺寸

    model.eval().cpu()  # cpu推理

    x = torch.randn(batch_size, *input_shape).cpu()  # 生成张量
    export_onnx_file = "./indi00031_00028.onnx"  # 要生成的ONNX文件名
    torch.onnx.export(model,
                    x,
                    export_onnx_file,
                    do_constant_folding=True,  # 是否执行常量折叠优化
                    input_names=["input"],  # 输入名
                    output_names=["output"],  # 输出名
                    dynamic_axes={"input": {0: "batch_size"},  # 批处理变量
                                    "output": {0: "batch_size"}})