# slicing_performance_clean.py
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

def create_clean_performance_plot(slice_type, color):
    # Load data
    df = pd.read_csv(f"{slice_type.lower()}_performance.csv")
    
    # Create figure
    fig, axes = plt.subplots(3, 1, figsize=(10, 12))
    fig.suptitle(f"{slice_type} SLICE PERFORMANCE METRICS", 
                fontsize=14, fontweight='bold', y=1.02)
    
    # Custom styling
    bar_width = 0.6
    opacity = 0.8
    devices = df['Device'].tolist()
    x = np.arange(len(devices))
    
    # 1. THROUGHPUT PLOT (no numbers on bars)
    axes[0].bar(x, df['Throughput(Mbps)'], 
                width=bar_width, color=color, alpha=opacity,
                edgecolor='black', linewidth=1)
    axes[0].set_title('THROUGHPUT (Mbps)', fontsize=12, pad=10)
    axes[0].set_ylabel('Megabits per second', fontsize=10)
    axes[0].set_xticks(x)
    axes[0].set_xticklabels(devices, fontsize=10)
    axes[0].grid(axis='y', linestyle='--', alpha=0.4)
    
    # 2. PACKET LOSS PLOT (no numbers on bars)
    axes[1].bar(x, df['PacketLoss(%)'], 
                width=bar_width, color=color, alpha=opacity,
                edgecolor='black', linewidth=1)
    axes[1].set_title('PACKET LOSS (%)', fontsize=12, pad=10)
    axes[1].set_ylabel('Percentage lost', fontsize=10)
    axes[1].set_xticks(x)
    axes[1].set_xticklabels(devices, fontsize=10)
    axes[1].grid(axis='y', linestyle='--', alpha=0.4)
    
    # 3. ENERGY PLOT (no numbers on bars)
    axes[2].bar(x, df['Energy(J)'], 
                width=bar_width, color=color, alpha=opacity,
                edgecolor='black', linewidth=1)
    axes[2].set_title('ENERGY CONSUMPTION (Joules)', fontsize=12, pad=10)
    axes[2].set_ylabel('Joules per operation', fontsize=10)
    axes[2].set_xticks(x)
    axes[2].set_xticklabels(devices, fontsize=10)
    axes[2].grid(axis='y', linestyle='--', alpha=0.4)
    
    # Final adjustments
    plt.tight_layout()
    
    # Add subtle slice identifier
    fig.text(0.5, 0.92, f"{slice_type} SLICE", 
             fontsize=24, color=color, alpha=0.1,
             ha='center', va='center', fontweight='bold')
    
    plt.savefig(f"{slice_type.lower()}_performance_clean.png", 
                dpi=300, bbox_inches='tight')
    plt.close()

if __name__ == "__main__":
    # Generate clean performance plots
    create_clean_performance_plot("URLLC", "firebrick")
    create_clean_performance_plot("eMBB", "royalblue")
    create_clean_performance_plot("mMTC", "forestgreen")
    
    print("Generated 3 clean performance images (no numbers on bars)")
