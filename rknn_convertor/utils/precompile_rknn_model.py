import sys
from rknn.api import RKNN

def main(input_model, output_model):
    # 确定目标设备target（模型预编译，目前适用于rv1126，不适用于rk3568和rk3588）
    target = 'rv1126'

    # 创建RKNN对象
    rknn = RKNN()
    
    # 加载RKNN模型
    print(f'--> 加载RKNN模型: {input_model}')
    ret = rknn.load_rknn(input_model)
    if ret != 0:
        print('加载RKNN模型失败!')
        rknn.release()
        exit(ret)
    print('完成')

    # 初始化运行环境
    print('--> 初始化运行环境')
    # 注意: 
    # 1、模型预编译，必须设置参数rknn2precompile=True；
    # 2、RK3399Pro、RK3568和RK3588以及安卓平台不支持模型预编译功能；
    ret = rknn.init_runtime(target=target, rknn2precompile=True)
    if ret != 0:
        print('初始化运行环境失败')
        rknn.release()
        exit(ret)
    print('完成')

    # 导出预编译模型
    print(f'--> 导出预编译模型: {output_model}')
    ret = rknn.export_rknn_precompile_model(output_model)
    if ret != 0:
        print('导出预编译模型失败')
        exit(ret)
    print('完成')

    # 释放RKNN对象
    rknn.release()

if __name__ == '__main__':
    # 检查命令行参数
    if len(sys.argv) != 3:
        print("用法: python script.py <输入模型路径> <输出模型路径>")
        print("示例: python script.py ./rknn_model/best_onnx.rknn ./precompiled_model.rknn")
        exit(1)
    
    # 获取输入和输出模型路径
    input_model = sys.argv[1]
    output_model = sys.argv[2]
    
    main(input_model, output_model)
