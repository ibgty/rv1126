import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import re

# Your data parsing code remains the same
file_path = '../log/individual_qnt_error_analysis.txt'
data = []

with open(file_path, 'r') as f:
    for line in f:
        line = line.strip()
        if not line:  # Skip empty lines
            continue
        
        # Use regex to extract layer names and norm values
        layer_match = re.search(r'(\d+\s+)?([^\t]+)', line)
        euclidean_match = re.search(r'eculidean_norm=([0-9.]+)', line)
        cosine_match = re.search(r'cosine_norm=([0-9.]+)', line)
        
        if layer_match and euclidean_match and cosine_match:
            layer_name = layer_match.group(2).strip()
            euclidean_val = float(euclidean_match.group(1))
            cosine_val = float(cosine_match.group(1))
            
            data.append({
                'layer': layer_name,
                'euclidean_norm': euclidean_val,
                'cosine_norm': cosine_val
            })

# Convert to DataFrame
df = pd.DataFrame(data)

# Create dual Y-axis chart
fig, ax1 = plt.subplots(figsize=(16, 10))

# Plot Euclidean norm (left Y-axis)
color_euclidean = 'blue'
line1 = ax1.plot(range(len(df)), df['euclidean_norm'], 
                 color=color_euclidean, linewidth=1.5, marker='o', 
                 markersize=4, label='Euclidean Norm')
ax1.set_xlabel('Layers', fontsize=12)
ax1.set_ylabel('Euclidean Norm', color=color_euclidean, fontsize=12)
ax1.tick_params(axis='y', labelcolor=color_euclidean)
ax1.set_ylim(0, max(df['euclidean_norm']) * 1.1)  # Set Y-axis range

# Create second Y-axis (right) for cosine similarity
ax2 = ax1.twinx()
color_cosine = 'red'
line2 = ax2.plot(range(len(df)), df['cosine_norm'], 
                 color=color_cosine, linewidth=1.5, marker='s', 
                 markersize=4, label='Cosine Similarity')
ax2.set_ylabel('Cosine Similarity', color=color_cosine, fontsize=12)
ax2.tick_params(axis='y', labelcolor=color_cosine)
ax2.set_ylim(min(df['cosine_norm']) * 0.999, 1.0005)  # Set Y-axis range

# Set X-axis tick labels as layer names (shortened for display)
short_names = []
for name in df['layer']:
    # Extract meaningful short names
    parts = name.split('_')
    if len(parts) > 3:
        short_name = f"{parts[0]}_{parts[2]}_{parts[-1]}"
    else:
        short_name = name
    short_names.append(short_name)

plt.xticks(range(len(df)), short_names, rotation=45, ha='right')

# Add combined legend
lines = line1 + line2
labels = [l.get_label() for l in lines]
ax1.legend(lines, labels, loc='upper left', frameon=True, 
           fancybox=True, shadow=True, fontsize=10)

# Add title and grid
plt.title('RKNN Model Quantization Error - Complete Execution', fontsize=14, pad=20)
ax1.grid(True, linestyle=':', alpha=0.7)

# Adjust layout and display
plt.tight_layout()
plt.show()

# Save chart (optional)
plt.savefig('../log/individual_qnt_error_analysis.png', dpi=300, bbox_inches='tight')