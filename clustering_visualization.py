#!/usr/bin/env python3
"""
Clustering Visualization and Analysis
This script provides visualization and comprehensive evaluation for clustering results.
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend for server environments

# Import from our implementations
import kmeans_clustering
import hierarchical_clustering


def calculate_metrics(cluster_assignments, true_labels):
    """
    Calculate comprehensive clustering evaluation metrics.
    
    Metrics include:
    - Purity: measures how well clusters match true classes
    - Rand Index: measures similarity between two clusterings
    - Adjusted Rand Index: Rand Index adjusted for chance
    """
    n = len(cluster_assignments)
    
    # Purity
    purity = kmeans_clustering.calculate_purity(cluster_assignments, true_labels)
    
    # Rand Index and Adjusted Rand Index
    # Count pairs that are in the same cluster in both clusterings
    same_cluster_both = 0
    same_cluster_pred = 0
    same_cluster_true = 0
    
    for i in range(n):
        for j in range(i + 1, n):
            # Same cluster in predicted
            if cluster_assignments[i] == cluster_assignments[j]:
                same_cluster_pred += 1
                # Same cluster in both
                if true_labels[i] == true_labels[j]:
                    same_cluster_both += 1
            
            # Same cluster in true labels
            if true_labels[i] == true_labels[j]:
                same_cluster_true += 1
    
    total_pairs = n * (n - 1) / 2
    different_cluster_both = total_pairs - same_cluster_pred - same_cluster_true + same_cluster_both
    
    # Rand Index
    rand_index = (same_cluster_both + different_cluster_both) / total_pairs
    
    # Adjusted Rand Index
    expected_index = (same_cluster_pred * same_cluster_true) / total_pairs
    max_index = (same_cluster_pred + same_cluster_true) / 2
    if max_index - expected_index == 0:
        ari = 0
    else:
        ari = (same_cluster_both - expected_index) / (max_index - expected_index)
    
    return {
        'purity': purity,
        'rand_index': rand_index,
        'adjusted_rand_index': ari
    }


def visualize_clusters_2d(data, cluster_assignments, true_labels, title, filename):
    """
    Visualize clustering results using PCA for dimensionality reduction to 2D.
    """
    # Simple PCA implementation (without sklearn)
    # Center the data
    mean = np.mean(data, axis=0)
    centered_data = data - mean
    
    # Calculate covariance matrix
    cov_matrix = np.cov(centered_data.T)
    
    # Get eigenvalues and eigenvectors
    eigenvalues, eigenvectors = np.linalg.eig(cov_matrix)
    
    # Sort by eigenvalues (descending)
    idx = eigenvalues.argsort()[::-1]
    eigenvalues = eigenvalues[idx]
    eigenvectors = eigenvectors[:, idx]
    
    # Project to 2D using first 2 principal components
    pca_2d = centered_data @ eigenvectors[:, :2]
    
    # Create figure with two subplots
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
    
    # Plot predicted clusters
    unique_clusters = np.unique(cluster_assignments)
    colors = plt.cm.Set1(np.linspace(0, 1, len(unique_clusters)))
    
    for i, cluster_id in enumerate(unique_clusters):
        mask = cluster_assignments == cluster_id
        ax1.scatter(pca_2d[mask, 0], pca_2d[mask, 1], 
                   c=[colors[i]], label=f'Cluster {cluster_id}', 
                   alpha=0.6, edgecolors='black', linewidth=0.5)
    
    ax1.set_xlabel('First Principal Component')
    ax1.set_ylabel('Second Principal Component')
    ax1.set_title(f'{title} - Predicted Clusters')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # Plot true labels
    unique_labels = np.unique(true_labels)
    colors_true = plt.cm.Set2(np.linspace(0, 1, len(unique_labels)))
    
    for i, label in enumerate(unique_labels):
        mask = true_labels == label
        ax2.scatter(pca_2d[mask, 0], pca_2d[mask, 1], 
                   c=[colors_true[i]], label=f'Class {label}', 
                   alpha=0.6, edgecolors='black', linewidth=0.5)
    
    ax2.set_xlabel('First Principal Component')
    ax2.set_ylabel('Second Principal Component')
    ax2.set_title(f'{title} - True Labels')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.close()
    print(f"  Saved visualization to {filename}")


def analyze_k_values(data, true_labels, k_range):
    """
    Analyze the relationship between k (number of clusters) and clustering performance.
    """
    results = {
        'k_values': [],
        'inertia': [],
        'purity': [],
        'rand_index': [],
        'ari': []
    }
    
    print("\nAnalyzing K-Means performance for different k values...")
    print("="*70)
    print(f"{'k':<5} {'Inertia':<12} {'Purity':<10} {'Rand Index':<12} {'ARI':<10}")
    print("-"*70)
    
    for k in k_range:
        # Run k-means
        centroids, assignments, inertia, _ = kmeans_clustering.kmeans(
            data, k, max_iterations=100, random_seed=42
        )
        
        # Calculate metrics
        metrics = calculate_metrics(assignments, true_labels)
        
        # Store results
        results['k_values'].append(k)
        results['inertia'].append(inertia)
        results['purity'].append(metrics['purity'])
        results['rand_index'].append(metrics['rand_index'])
        results['ari'].append(metrics['adjusted_rand_index'])
        
        print(f"{k:<5} {inertia:<12.4f} {metrics['purity']:<10.4f} "
              f"{metrics['rand_index']:<12.4f} {metrics['adjusted_rand_index']:<10.4f}")
    
    print("="*70)
    
    return results


def plot_k_analysis(results, filename):
    """
    Plot the relationship between k and various performance metrics.
    """
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(14, 10))
    
    k_values = results['k_values']
    
    # Plot 1: Inertia (Elbow plot)
    ax1.plot(k_values, results['inertia'], 'bo-', linewidth=2, markersize=8)
    ax1.set_xlabel('Number of Clusters (k)', fontsize=11)
    ax1.set_ylabel('Inertia (Within-cluster sum of squares)', fontsize=11)
    ax1.set_title('Elbow Method - Inertia vs k', fontsize=12, fontweight='bold')
    ax1.grid(True, alpha=0.3)
    ax1.set_xticks(k_values)
    
    # Plot 2: Purity
    ax2.plot(k_values, results['purity'], 'go-', linewidth=2, markersize=8)
    ax2.set_xlabel('Number of Clusters (k)', fontsize=11)
    ax2.set_ylabel('Purity', fontsize=11)
    ax2.set_title('Purity vs k', fontsize=12, fontweight='bold')
    ax2.grid(True, alpha=0.3)
    ax2.set_xticks(k_values)
    ax2.set_ylim([0, 1.05])
    
    # Plot 3: Rand Index
    ax3.plot(k_values, results['rand_index'], 'ro-', linewidth=2, markersize=8)
    ax3.set_xlabel('Number of Clusters (k)', fontsize=11)
    ax3.set_ylabel('Rand Index', fontsize=11)
    ax3.set_title('Rand Index vs k', fontsize=12, fontweight='bold')
    ax3.grid(True, alpha=0.3)
    ax3.set_xticks(k_values)
    ax3.set_ylim([0, 1.05])
    
    # Plot 4: Adjusted Rand Index
    ax4.plot(k_values, results['ari'], 'mo-', linewidth=2, markersize=8)
    ax4.set_xlabel('Number of Clusters (k)', fontsize=11)
    ax4.set_ylabel('Adjusted Rand Index', fontsize=11)
    ax4.set_title('Adjusted Rand Index vs k', fontsize=12, fontweight='bold')
    ax4.grid(True, alpha=0.3)
    ax4.set_xticks(k_values)
    
    plt.tight_layout()
    plt.savefig(filename, dpi=150, bbox_inches='tight')
    plt.close()
    print(f"\nSaved k-analysis plot to {filename}")


def main():
    print("="*70)
    print("CLUSTERING VISUALIZATION AND PERFORMANCE ANALYSIS")
    print("="*70)
    
    # Load data
    print("\nLoading data from seeds.txt...")
    data, true_labels = kmeans_clustering.load_data('seeds.txt')
    print(f"Loaded {data.shape[0]} samples with {data.shape[1]} features")
    
    # Normalize data
    print("Normalizing data...")
    mean = np.mean(data, axis=0)
    std = np.std(data, axis=0)
    data_normalized = (data - mean) / std
    
    # ========================================================================
    # 1. K-Means Clustering Visualization
    # ========================================================================
    print("\n" + "="*70)
    print("1. K-MEANS CLUSTERING (k=3)")
    print("="*70)
    
    centroids, kmeans_assignments, inertia, iterations = kmeans_clustering.kmeans(
        data_normalized, k=3, max_iterations=100, random_seed=42
    )
    
    # Calculate metrics
    kmeans_metrics = calculate_metrics(kmeans_assignments, true_labels)
    
    print(f"\nPerformance Metrics:")
    print(f"  Purity:               {kmeans_metrics['purity']:.4f}")
    print(f"  Rand Index:           {kmeans_metrics['rand_index']:.4f}")
    print(f"  Adjusted Rand Index:  {kmeans_metrics['adjusted_rand_index']:.4f}")
    print(f"  Inertia:              {inertia:.4f}")
    print(f"  Iterations:           {iterations}")
    
    # Visualize
    print("\nGenerating visualizations...")
    visualize_clusters_2d(data_normalized, kmeans_assignments, true_labels,
                          'K-Means Clustering', 'kmeans_visualization.png')
    
    # ========================================================================
    # 2. Hierarchical Clustering Visualization
    # ========================================================================
    print("\n" + "="*70)
    print("2. HIERARCHICAL CLUSTERING (3 clusters)")
    print("="*70)
    
    dendrogram, cluster_history = hierarchical_clustering.hierarchical_clustering(
        data_normalized, linkage='average', verbose=False
    )
    
    hierarchical_assignments = hierarchical_clustering.cut_dendrogram(cluster_history, 3)
    
    # Calculate metrics
    hierarchical_metrics = calculate_metrics(hierarchical_assignments, true_labels)
    
    print(f"\nPerformance Metrics:")
    print(f"  Purity:               {hierarchical_metrics['purity']:.4f}")
    print(f"  Rand Index:           {hierarchical_metrics['rand_index']:.4f}")
    print(f"  Adjusted Rand Index:  {hierarchical_metrics['adjusted_rand_index']:.4f}")
    
    # Visualize
    print("\nGenerating visualizations...")
    visualize_clusters_2d(data_normalized, hierarchical_assignments, true_labels,
                          'Hierarchical Clustering', 'hierarchical_visualization.png')
    
    # ========================================================================
    # 3. Analysis of k values vs performance
    # ========================================================================
    print("\n" + "="*70)
    print("3. K-VALUE ANALYSIS (k=2 to k=10)")
    print("="*70)
    
    k_range = range(2, 11)
    k_results = analyze_k_values(data_normalized, true_labels, k_range)
    
    # Plot results
    print("\nGenerating k-analysis plots...")
    plot_k_analysis(k_results, 'k_analysis.png')
    
    # ========================================================================
    # 4. Comparison Summary
    # ========================================================================
    print("\n" + "="*70)
    print("4. ALGORITHM COMPARISON SUMMARY")
    print("="*70)
    
    print(f"\n{'Metric':<25} {'K-Means':<15} {'Hierarchical':<15}")
    print("-"*55)
    print(f"{'Purity':<25} {kmeans_metrics['purity']:<15.4f} {hierarchical_metrics['purity']:<15.4f}")
    print(f"{'Rand Index':<25} {kmeans_metrics['rand_index']:<15.4f} {hierarchical_metrics['rand_index']:<15.4f}")
    print(f"{'Adjusted Rand Index':<25} {kmeans_metrics['adjusted_rand_index']:<15.4f} {hierarchical_metrics['adjusted_rand_index']:<15.4f}")
    
    print("\n" + "="*70)
    print("ANALYSIS COMPLETE")
    print("="*70)
    print("\nGenerated files:")
    print("  - kmeans_visualization.png")
    print("  - hierarchical_visualization.png")
    print("  - k_analysis.png")
    print("="*70)


if __name__ == "__main__":
    main()
