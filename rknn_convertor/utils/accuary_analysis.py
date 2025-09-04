import os
# import urllib
import traceback
import time
import sys
import numpy as np
import random
import cv2
from rknn.api import RKNN
# import urllib.request



# def show_outputs(outputs):
#     output = outputs
#     index = sorted(range(len(output)), key=lambda k : output[k], reverse=True)
#     fp = open('../labels/wider_face_list.txt', 'r')
#     labels = fp.readlines()
#     top5_str = 'yolovts\n-----TOP 5-----\n'
#     for i in range(5):
#         value = output[index[i]]
#         if value > 0:
#             topi = '[{:>3d}] score:{:.6f} class:"{}"\n'.format(index[i], value, labels[index[i]].strip().split(':')[-1])
#         else:
#             topi = '[ -1]: 0.0\n'
#         top5_str += topi
#     print(top5_str.strip())

# def softmax(output):
#     return np.exp(output)/np.sum(np.exp(output))

def readable_speed(speed):
    speed_bytes = float(speed)
    speed_kbytes = speed_bytes / 1024
    if speed_kbytes > 1024:
        speed_mbytes = speed_kbytes / 1024
        if speed_mbytes > 1024:
            speed_gbytes = speed_mbytes / 1024
            return "{:.2f} GB/s".format(speed_gbytes)
        else:
            return "{:.2f} MB/s".format(speed_mbytes)
    else:
        return "{:.2f} KB/s".format(speed_kbytes)


def show_progress(blocknum, blocksize, totalsize):
    speed = (blocknum * blocksize) / (time.time() - start_time)
    speed_str = " Speed: {}".format(readable_speed(speed))
    recv_size = blocknum * blocksize

    f = sys.stdout
    progress = (recv_size / totalsize)
    progress_str = "{:.2f}%".format(progress * 100)
    n = round(progress * 50)
    s = ('#' * n).ljust(50, '-')
    f.write(progress_str.ljust(8, ' ') + '[' + s + ']' + speed_str)
    f.flush()
    f.write('\r\n')

def get_random_jpg_path(img_dir="../img"):
    """
    从指定目录（含子目录）中随机获取一张 .jpg 图片的完整路径
    :param img_dir: 图片目录路径（默认为当前目录下的 'img' 文件夹）
    :return: 随机图片的完整路径（若找不到则返回 None）
    """
    # 1. 收集所有符合条件的图片路径
    image_paths = []
    for root, _, files in os.walk(img_dir):  # 递归遍历目录及子目录
        for file in files:
            # 检查扩展名（兼容大小写）
            if file.lower().endswith('.jpg'):
                full_path = os.path.join(root, file)
                image_paths.append(full_path)
    
    # 2. 随机选择并返回结果
    if image_paths:
        return random.choice(image_paths)
    else:
        return None  # 无图片时返回 Non


if __name__ == '__main__':

    # Create RKNN object
    rknn = RKNN(verbose=False)

    # Pre-process config
    if len(sys.argv) != 3:
        print("Usage: python accuary_analysis.py <input_onnx_path> <output_path>")
        print("Example: python convert_onnx_to_rknn.py ./onnx_model/person5_s_relu.onnx ../log")
        exit(1)
    input_onnx_path = sys.argv[1]
    output_path = sys.argv[2]
    print('--> Config model')
    rknn.config(mean_values=[[0, 0, 0]], std_values=[[255, 255, 255]], target_platform='rv1126')
    print('done')

    # Load model
    print('--> Loading model')
    ret = rknn.load_onnx(model=input_onnx_path)
    if ret != 0:
        print('Load model failed!')
        exit(ret)
    print('done')
    # Build model
    print('--> Building model')
    ret = rknn.build(do_quantization=True, dataset='../img/dataset.txt')
    if ret != 0:
        print('Build model failed!')
        exit(ret)
    print('done')

    test_jpg=get_random_jpg_path()
    print("test_jpg:%s\n"%(test_jpg))

    ret = rknn.accuracy_analysis(inputs='../labels/data_oneline.txt', output_dir=output_path)
    if ret != 0:
        print('Accuracy analysis failed!')
        exit(ret)
    print('done')

    # print('float32:')
    # output = np.genfromtxt(f'./{output_path}/fp32/yolo_face_fwd.txt')
    # show_outputs(softmax(output))

    # print('quantized:')
    # output = np.genfromtxt(f'./{output_path}/individual_qnt/yolo_face_fwd.txt')
    # show_outputs(softmax(output))

    rknn.release()
