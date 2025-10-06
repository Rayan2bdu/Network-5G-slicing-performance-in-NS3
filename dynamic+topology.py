# network_slicing_visualization_updated.py
import matplotlib.pyplot as plt
import networkx as nx
import pandas as pd
import numpy as np
from matplotlib.patches import Patch

def draw_topology():
    # SAMMA topologi som förut - oförändrad
    plt.figure(figsize=(12, 8))
    G = nx.DiGraph()
    
    # Node positions and styling
    nodes = {
        "gNodeB": (0, 2),
        "URLLC UPF": (-2, 1),
        "eMBB UPF": (0, 1),
        "mMTC UPF": (2, 1),
        "Robot (URLLC)": (-2.5, 0),
        "Drone (URLLC)": (-1.5, 0),
        "8K Video (eMBB)": (-0.5, 0),
        "VR (eMBB)": (0.5, 0),
        "Smart Meter (mMTC)": (1.5, 0),
        "Sensor (mMTC)": (2.5, 0)
    }
    
    # Add nodes and edges
    for node in nodes:
        G.add_node(node)
    
    edges = [
        ("gNodeB", "URLLC UPF", "red"),
        ("gNodeB", "eMBB UPF", "blue"),
        ("gNodeB", "mMTC UPF", "green"),
        ("URLLC UPF", "Robot (URLLC)", "red"),
        ("URLLC UPF", "Drone (URLLC)", "red"),
        ("eMBB UPF", "8K Video (eMBB)", "blue"),
        ("eMBB UPF", "VR (eMBB)", "blue"),
        ("mMTC UPF", "Smart Meter (mMTC)", "green"),
        ("mMTC UPF", "Sensor (mMTC)", "green")
    ]
    
    for src, dst, color in edges:
        G.add_edge(src, dst, color=color)
    
    # Draw elements
    nx.draw_networkx_nodes(G, nodes, nodelist=["gNodeB"], node_size=3000, node_shape="s", node_color="gold", edgecolors="black")
    nx.draw_networkx_nodes(G, nodes, nodelist=["URLLC UPF", "eMBB UPF", "mMTC UPF"], node_size=2000, node_color="lightgray", edgecolors="black")
    nx.draw_networkx_nodes(G, nodes, nodelist=["Robot (URLLC)", "Drone (URLLC)"], node_size=1800, node_color="red")
    nx.draw_networkx_nodes(G, nodes, nodelist=["8K Video (eMBB)", "VR (eMBB)"], node_size=1800, node_color="blue")
    nx.draw_networkx_nodes(G, nodes, nodelist=["Smart Meter (mMTC)", "Sensor (mMTC)"], node_size=1800, node_color="green")
    
    edge_colors = [G[u][v]['color'] for u,v in G.edges()]
    nx.draw_networkx_edges(G, nodes, edge_color=edge_colors, arrows=True, arrowstyle="-|>", width=2)
    nx.draw_networkx_labels(G, nodes, font_size=9)
    
    plt.title("5G Network Slicing Topology", pad=20)
    plt.axis("off")
    plt.tight_layout()
    plt.savefig("network_topology.png", dpi=300)
    plt.close()
    print("Topology saved to network_topology.png")

def visualize_performance():
    # LÄSER FRÅN CSV INSTÄLLET FÖR HARAKODAD DATA
    try:
        # Load static data
        urllc_static = pd.read_csv("urllc_performance.csv")
        embb_static = pd.read_csv("embb_performance.csv")
        mmtc_static = pd.read_csv("mmtc_performance.csv")
        
        # Load dynamic data
        urllc_dynamic = pd.read_csv("urllc_dynamic.csv")
        embb_dynamic = pd.read_csv("embb_dynamic.csv")
        mmtc_dynamic = pd.read_csv("mmtc_dynamic.csv")
        
        # Add configuration type
        urllc_static['Configuration'] = 'Static'
        embb_static['Configuration'] = 'Static'
        mmtc_static['Configuration'] = 'Static'
        urllc_dynamic['Configuration'] = 'Dynamic QoS'
        embb_dynamic['Configuration'] = 'Dynamic QoS'
        mmtc_dynamic['Configuration'] = 'Dynamic QoS'
        
        # Add slice type
        urllc_static['SliceType'] = 'URLLC'
        embb_static['SliceType'] = 'eMBB'
        mmtc_static['SliceType'] = 'mMTC'
        urllc_dynamic['SliceType'] = 'URLLC'
        embb_dynamic['SliceType'] = 'eMBB'
        mmtc_dynamic['SliceType'] = 'mMTC'
        
        # Combine all data
        df = pd.concat([urllc_static, embb_static, mmtc_static, 
                       urllc_dynamic, embb_dynamic, mmtc_dynamic], ignore_index=True)
        
    except FileNotFoundError:
        print("CSV files not found. Please run C++ programs first.")
        return
    
    fig, axes = plt.subplots(3, 1, figsize=(10, 14))
    
    # Create custom legend handles
    legend_elements = [
        Patch(facecolor='red', alpha=0.7, label='URLLC Static'),
        Patch(facecolor='red', alpha=1.0, hatch='//', label='URLLC Dynamic QoS'),
        Patch(facecolor='blue', alpha=0.7, label='eMBB Static'),
        Patch(facecolor='blue', alpha=1.0, hatch='//', label='eMBB Dynamic QoS'),
        Patch(facecolor='green', alpha=0.7, label='mMTC Static'),
        Patch(facecolor='green', alpha=1.0, hatch='//', label='mMTC Dynamic QoS')
    ]
    
    # Common plotting function
    def plot_metric(ax, metric, title, ylabel):
        for i, stype in enumerate(["URLLC", "eMBB", "mMTC"]):
            subset = df[df["SliceType"] == stype]
            static_val = subset[subset["Configuration"]=="Static"][metric].mean()
            dynamic_val = subset[subset["Configuration"]=="Dynamic QoS"][metric].mean()
            
            color = "red" if stype == "URLLC" else "blue" if stype == "eMBB" else "green"
            ax.bar(i-0.2, static_val, width=0.4, color=color, alpha=0.7)
            ax.bar(i+0.2, dynamic_val, width=0.4, color=color, alpha=1.0, hatch='//')
        
        ax.set_title(title)
        ax.set_ylabel(ylabel)
        ax.set_xticks(range(3))
        ax.set_xticklabels(["URLLC", "eMBB", "mMTC"])
        ax.legend(handles=legend_elements, bbox_to_anchor=(1.05, 1), loc='upper left')
    
    # Plot all metrics
    plot_metric(axes[0], "Throughput(Mbps)", "Throughput Comparison", "Mbps")
    plot_metric(axes[1], "PacketLoss(%)", "Packet Loss Comparison", "%")
    plot_metric(axes[2], "Energy(J)", "Energy Consumption Comparison", "Joules")
    
    plt.tight_layout()
    plt.savefig("slice_performance.png", dpi=300, bbox_inches='tight')
    plt.close()
    print("Performance visualization saved to slice_performance.png")

if __name__ == "__main__":
    draw_topology()
    visualize_performance()
