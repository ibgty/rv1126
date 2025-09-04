# -*- coding: utf-8 -*-
import sys
from rknn.api import RKNN

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python convert_pytorch_to_rknn.py <input_pytorch_path> <output_rknn_path>")
        print("Example: python convert_pytorch_to_rknn.py ./pytorch_model/person5_s_relu.pt ./rknn_model/person_5_s_relu_onnx.rknn")
        exit(1)

    # 获取命令行参数
    input_pytorch_path = sys.argv[1]
    output_rknn_path = sys.argv[2]
    target = 'rv1126'

    rknn = RKNN()

    # 配置模型参数（与原始配置完全一致）
    print('--> config model')
    rknn.config(
        reorder_channel='0 1 2',
        mean_values=[[0, 0, 0]],
        std_values=[[255, 255, 255]],
        target_platform=target,
        force_builtin_perm=True,
        output_optimize=1
    )
    print('done')

    # 加载ONNX模型（使用动态输入路径）
    print('--> loading model')
    ret = rknn.load_pytorch(
        model=input_pytorch_path,  # 使用命令行输入的路径
        input_size_list=[[3, 640, 640]]
    )
    if ret != 0:
        print('load model failed!')
        rknn.release()
        exit(ret)
    print('done')

    # 构建RKNN模型
    print('--> building model')
    ret = rknn.build(dataset='../img/dataset.txt')  # 量化数据集路径保持不变
    if ret != 0:
        print('build model failed!')
        rknn.release()
        exit(ret)
    print('done')

    # 导出RKNN模型（使用动态输出路径）
    print('--> export RKNN model')
    ret = rknn.export_rknn(output_rknn_path)  # 使用命令行输出的路径
    if ret != 0:
        print('export RKNN model failed!')
        rknn.release()
        exit(ret)
    print('done')

    rknn.release()
