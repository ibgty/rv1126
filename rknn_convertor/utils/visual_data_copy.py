import pandas as pd
import plotly.graph_objects as go
from plotly.subplots import make_subplots
import re

# 解析文本文件并创建 DataFrame
file_path = '../log/individual_qnt_error_analysis.txt'  # 请确保路径正确
data = []
current_layer = None

with open(file_path, 'r') as f:
    lines = f.readlines()
    
# 逐行处理，将层名称和对应的误差值配对
for i in range(0, len(lines), 2):  # 每两行为一组数据
    if i + 1 >= len(lines):
        break
        
    # 第一行：提取层名称
    layer_line = lines[i].strip()
    # 使用正则表达式提取层名称（去掉前面的数字）
    layer_match = re.search(r'\d+\s+(.+)', layer_line)
    if layer_match:
        layer_name = layer_match.group(1)
    else:
        layer_name = layer_line  # 如果正则匹配失败，使用整行
        
    # 第二行：提取误差值
    error_line = lines[i + 1].strip()
    euclidean_match = re.search(r'eculidean_norm=([0-9.]+)', error_line)
    cosine_match = re.search(r'cosine_norm=([0-9.]+)', error_line)
    
    euclidean_val = float(euclidean_match.group(1)) if euclidean_match else None
    cosine_val = float(cosine_match.group(1)) if cosine_match else None
    
    data.append({
        'layer': layer_name,
        'euclidean_norm': euclidean_val,
        'cosine_norm': cosine_val
    })

# 转换为 DataFrame
df = pd.DataFrame(data)

# 打印数据信息确认解析正确
print("数据解析完成！")
print(f"共解析了 {len(df)} 层的数据")
print("前5层数据:")
print(df.head())

# 创建带双Y轴的交互式图表
fig = make_subplots(
    specs=[[{"secondary_y": True}]],
    subplot_titles=("RKNN量化误差分析",)
)

# 添加欧几里得范数数据 (左轴，红色)
fig.add_trace(
    go.Scatter(
        x=df['layer'], 
        y=df['euclidean_norm'], 
        name="欧几里得范数", 
        mode='lines+markers',
        marker=dict(color='red', size=6),
        line=dict(color='red', width=2),
        hovertemplate='<b>%{x}</b><br>欧几里得范数: %{y:.6f}<extra></extra>'
    ),
    secondary_y=False,
)

# 添加余弦范数数据 (右轴，蓝色)
fig.add_trace(
    go.Scatter(
        x=df['layer'], 
        y=df['cosine_norm'], 
        name="余弦范数", 
        mode='lines+markers',
        marker=dict(color='blue', size=6),
        line=dict(color='blue', width=2),
        hovertemplate='<b>%{x}</b><br>余弦范数: %{y:.6f}<extra></extra>'
    ),
    secondary_y=True,
)

# 设置轴标签和标题
fig.update_xaxes(
    title_text="网络层名称",
    tickangle=-45,
    tickmode='array',
    tickvals=list(range(len(df))),
    ticktext=df['layer'].tolist()
)

fig.update_yaxes(
    title_text="<b>欧几里得范数</b> (数值误差)",
    secondary_y=False,
    title_font=dict(color="red"),
    tickfont=dict(color="red")
)

fig.update_yaxes(
    title_text="<b>余弦范数</b> (相似度)",
    secondary_y=True,
    range=[0.998, 1.0001],  # 聚焦在接近1.0的范围内
    title_font=dict(color="blue"),
    tickfont=dict(color="blue")
)

# 更新布局
fig.update_layout(
    title={
        'text': "RKNN模型量化误差分析 - 逐层对比",
        'x': 0.5,
        'xanchor': 'center',
        'font': dict(size=16)
    },
    height=600,
    width=1200,
    hovermode='x unified',
    legend=dict(
        x=0.02,
        y=0.98,
        bgcolor='rgba(255, 255, 255, 0.8)',
        bordercolor='black',
        borderwidth=1
    )
)

# 添加网格线
fig.update_xaxes(showgrid=True, gridwidth=1, gridcolor='LightGrey')
fig.update_yaxes(showgrid=True, gridwidth=1, gridcolor='LightGrey', secondary_y=False)
fig.update_yaxes(showgrid=True, gridwidth=1, gridcolor='LightGrey', secondary_y=True)

# 显示图表
fig.show()

# 可选：保存为HTML文件以便分享
# fig.write_html("quantization_error_analysis.html")