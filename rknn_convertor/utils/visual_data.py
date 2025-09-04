import re
import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots

# 解析文本文件并创建 DataFrame `df`
file_path = '../log/entire_qnt_error_analysis.txt'
data = []
# print("1")
with open(file_path, 'r') as f:
    for line in f:
        line = line.strip()
        if not line:  # 跳过空行
            continue
        # print("1")
        
        # 使用正则表达式提取层名称和范数值
        layer_match = re.search(r'(\S+\.npy)', line)
        euclidean_match = re.search(r'eculidean_norm=([0-9.]+)', line)
        cosine_match = re.search(r'cosine_norm=([0-9.]+)', line)
        
        if layer_match:
            layer_name = layer_match.group(1)
            euclidean_val = float(euclidean_match.group(1)) if euclidean_match else None
            cosine_val = float(cosine_match.group(1)) if cosine_match else None
            
            data.append({
                'layer': layer_name,
                'euclidean_norm': euclidean_val,
                'cosine_norm': cosine_val
            })

# 转换为 DataFrame
df = pd.DataFrame(data)

# 打印列名和前几行数据以确保正确解析
print("DataFrame 列名:", df.columns.tolist())
print("DataFrame 前几行数据:")
print(df.head())

# 删除两列都是NaN的行（如果有的话）
df.dropna(subset=['euclidean_norm', 'cosine_norm'], how='all', inplace=True)

# 创建带双Y轴的子图
fig = make_subplots(specs=[[{"secondary_y": True}]])

# 添加欧几里得范数数据 (左轴)
fig.add_trace(
    go.Scatter(x=df['layer'], y=df['euclidean_norm'], name="Euclidean Norm", mode='lines+markers'),
    secondary_y=False,
)

# 添加余弦范数数据 (右轴)
fig.add_trace(
    go.Scatter(x=df['layer'], y=df['cosine_norm'], name="Cosine Norm", mode='lines+markers'),
    secondary_y=True,
)

# 设置轴标签
fig.update_xaxes(title_text="Layer Name")
fig.update_yaxes(title_text="<b>Euclidean</b> Norm", secondary_y=False)
fig.update_yaxes(title_text="<b>Cosine</b> Norm", secondary_y=True)

# 旋转X轴标签
fig.update_layout(
    xaxis_tickangle=-45,
    title="RKNN Quantization Error Analysis by Layer"
)

fig.show()