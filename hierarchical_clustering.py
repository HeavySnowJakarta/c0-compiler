#!/usr/bin/env python3
"""
Hierarchical Clustering Implementation
This implementation uses only numpy for basic array operations.
No scikit-learn high-level APIs are used.
"""

import numpy as np
import sys


def load_data(filename):
    """Load the Seeds dataset from a tab-separated file."""
    data = []
    labels = []
    
    with open(filename, 'r') as f:
        for line in f:
            parts = line.strip().split('\t')
            # Remove empty strings from split
            parts = [p for p in parts if p]
            if len(parts) >= 8:
                # First 7 columns are features
                features = [float(x) for x in parts[:7]]
                data.append(features)
                # Last column is the true label (for evaluation)
                labels.append(int(parts[7]))
    
    return np.array(data), np.array(labels)


def euclidean_distance(a, b):
    """Calculate Euclidean distance between two points or clusters."""
    return np.sqrt(np.sum((a - b) ** 2))


def calculate_distance_matrix(data):
    """Calculate pairwise distance matrix for all data points."""
    n = len(data)
    distance_matrix = np.zeros((n, n))
    
    for i in range(n):
        for j in range(i + 1, n):
            dist = euclidean_distance(data[i], data[j])
            distance_matrix[i, j] = dist
            distance_matrix[j, i] = dist
    
    return distance_matrix


def single_linkage_distance(cluster1_indices, cluster2_indices, distance_matrix):
    """
    Calculate single linkage distance between two clusters.
    Single linkage: minimum distance between any two points in the clusters.
    """
    min_dist = np.inf
    
    for i in cluster1_indices:
        for j in cluster2_indices:
            dist = distance_matrix[i, j]
            if dist < min_dist:
                min_dist = dist
    
    return min_dist


def complete_linkage_distance(cluster1_indices, cluster2_indices, distance_matrix):
    """
    Calculate complete linkage distance between two clusters.
    Complete linkage: maximum distance between any two points in the clusters.
    """
    max_dist = 0
    
    for i in cluster1_indices:
        for j in cluster2_indices:
            dist = distance_matrix[i, j]
            if dist > max_dist:
                max_dist = dist
    
    return max_dist


def average_linkage_distance(cluster1_indices, cluster2_indices, distance_matrix):
    """
    Calculate average linkage distance between two clusters.
    Average linkage: average distance between all pairs of points in the clusters.
    """
    total_dist = 0
    count = 0
    
    for i in cluster1_indices:
        for j in cluster2_indices:
            total_dist += distance_matrix[i, j]
            count += 1
    
    return total_dist / count if count > 0 else 0


def hierarchical_clustering(data, linkage='average', verbose=True):
    """
    Agglomerative Hierarchical Clustering.
    
    Parameters:
    - data: numpy array of shape (n_samples, n_features)
    - linkage: linkage criterion ('single', 'complete', 'average')
    - verbose: whether to print progress
    
    Returns:
    - dendrogram: list of merge operations (for tree structure)
    - cluster_history: list of cluster states at each step
    """
    n_samples = data.shape[0]
    
    # Calculate pairwise distance matrix
    if verbose:
        print("Calculating pairwise distance matrix...")
    distance_matrix = calculate_distance_matrix(data)
    
    # Initialize: each point is its own cluster
    clusters = [[i] for i in range(n_samples)]
    dendrogram = []
    cluster_history = [clusters.copy()]
    
    # Choose linkage function
    if linkage == 'single':
        linkage_func = single_linkage_distance
    elif linkage == 'complete':
        linkage_func = complete_linkage_distance
    elif linkage == 'average':
        linkage_func = average_linkage_distance
    else:
        raise ValueError(f"Unknown linkage: {linkage}")
    
    if verbose:
        print(f"Starting hierarchical clustering with {linkage} linkage...")
    
    # Merge clusters iteratively
    step = 0
    while len(clusters) > 1:
        # Find the two closest clusters
        min_dist = np.inf
        merge_i, merge_j = -1, -1
        
        for i in range(len(clusters)):
            for j in range(i + 1, len(clusters)):
                dist = linkage_func(clusters[i], clusters[j], distance_matrix)
                if dist < min_dist:
                    min_dist = dist
                    merge_i, merge_j = i, j
        
        # Merge the two closest clusters
        new_cluster = clusters[merge_i] + clusters[merge_j]
        
        # Record the merge
        dendrogram.append({
            'step': step,
            'cluster1': clusters[merge_i].copy(),
            'cluster2': clusters[merge_j].copy(),
            'distance': min_dist,
            'new_cluster': new_cluster.copy()
        })
        
        # Remove old clusters and add new cluster
        # Remove larger index first to avoid index shift
        if merge_i > merge_j:
            clusters.pop(merge_i)
            clusters.pop(merge_j)
        else:
            clusters.pop(merge_j)
            clusters.pop(merge_i)
        
        clusters.append(new_cluster)
        cluster_history.append([c.copy() for c in clusters])
        
        step += 1
        
        if verbose and step % 20 == 0:
            print(f"  Step {step}: {len(clusters)} clusters remaining")
    
    if verbose:
        print(f"Clustering complete after {step} merges")
    
    return dendrogram, cluster_history


def cut_dendrogram(cluster_history, n_clusters):
    """
    Cut the dendrogram to get a specific number of clusters.
    Returns cluster assignments for each data point.
    """
    # Find the clustering state with desired number of clusters
    if n_clusters > len(cluster_history):
        raise ValueError(f"Cannot create {n_clusters} clusters (max is {len(cluster_history)})")
    
    # cluster_history[0] has n samples (each in its own cluster)
    # cluster_history[-1] has 1 cluster (all samples together)
    # We want cluster_history[-(n_clusters)]
    target_clusters = cluster_history[-(n_clusters)]
    
    # Create assignment array
    n_samples = sum(len(c) for c in target_clusters)
    assignments = np.zeros(n_samples, dtype=int)
    
    for cluster_id, cluster_indices in enumerate(target_clusters):
        for idx in cluster_indices:
            assignments[idx] = cluster_id
    
    return assignments


def calculate_purity(cluster_assignments, true_labels):
    """
    Calculate clustering purity.
    Purity = (1/N) * sum_k(max_j(|cluster_k ∩ class_j|))
    """
    n_samples = len(cluster_assignments)
    unique_clusters = np.unique(cluster_assignments)
    
    total_correct = 0
    for cluster_id in unique_clusters:
        # Get all true labels for points in this cluster
        cluster_mask = cluster_assignments == cluster_id
        cluster_labels = true_labels[cluster_mask]
        
        # Find the most common true label in this cluster
        if len(cluster_labels) > 0:
            unique, counts = np.unique(cluster_labels, return_counts=True)
            max_count = np.max(counts)
            total_correct += max_count
    
    purity = total_correct / n_samples
    return purity


def print_cluster_statistics(cluster_assignments, true_labels, n_clusters):
    """Print statistics about the clustering results."""
    print("\n" + "="*60)
    print("CLUSTER STATISTICS")
    print("="*60)
    
    for cluster_id in range(n_clusters):
        cluster_mask = cluster_assignments == cluster_id
        cluster_size = np.sum(cluster_mask)
        cluster_labels = true_labels[cluster_mask]
        
        print(f"\nCluster {cluster_id}:")
        print(f"  Size: {cluster_size} samples")
        
        if cluster_size > 0:
            unique, counts = np.unique(cluster_labels, return_counts=True)
            print(f"  True label distribution:")
            for label, count in zip(unique, counts):
                percentage = (count / cluster_size) * 100
                print(f"    Class {label}: {count} samples ({percentage:.2f}%)")


def print_dendrogram_info(dendrogram, num_steps=10):
    """Print information about the first few merge steps."""
    print("\n" + "="*60)
    print(f"FIRST {num_steps} MERGE STEPS")
    print("="*60)
    print(f"{'Step':<6} {'Cluster 1 Size':<16} {'Cluster 2 Size':<16} {'Distance':<10}")
    print("-" * 60)
    
    for i in range(min(num_steps, len(dendrogram))):
        merge = dendrogram[i]
        print(f"{merge['step']:<6} {len(merge['cluster1']):<16} "
              f"{len(merge['cluster2']):<16} {merge['distance']:<10.4f}")


def main():
    print("="*60)
    print("HIERARCHICAL CLUSTERING ON SEEDS DATASET")
    print("="*60)
    
    # Load data
    print("\nLoading data from seeds.txt...")
    data, true_labels = load_data('seeds.txt')
    print(f"Loaded {data.shape[0]} samples with {data.shape[1]} features")
    print(f"True labels: {np.unique(true_labels)}")
    
    # Normalize data
    print("\nNormalizing data (standardization)...")
    mean = np.mean(data, axis=0)
    std = np.std(data, axis=0)
    data_normalized = (data - mean) / std
    print("Data normalized (mean=0, std=1 for each feature)")
    
    # Run Hierarchical Clustering with average linkage
    linkage_method = 'average'
    print(f"\n{'='*60}")
    print(f"Running Hierarchical Clustering with {linkage_method} linkage")
    print(f"{'='*60}")
    
    dendrogram, cluster_history = hierarchical_clustering(
        data_normalized, linkage=linkage_method, verbose=True
    )
    
    # Print dendrogram information
    print_dendrogram_info(dendrogram, num_steps=15)
    
    # Cut dendrogram to get 3 clusters (matching true number of classes)
    n_clusters = 3
    print(f"\n{'='*60}")
    print(f"Cutting dendrogram to get {n_clusters} clusters")
    print(f"{'='*60}")
    
    cluster_assignments = cut_dendrogram(cluster_history, n_clusters)
    
    # Calculate purity
    purity = calculate_purity(cluster_assignments, true_labels)
    print(f"\nClustering purity: {purity:.4f} ({purity*100:.2f}%)")
    
    # Print cluster statistics
    print_cluster_statistics(cluster_assignments, true_labels, n_clusters)
    
    # Print first few cluster assignments
    print("\n" + "="*60)
    print("FIRST 20 CLUSTER ASSIGNMENTS")
    print("="*60)
    print("Index | Predicted Cluster | True Label")
    print("-" * 45)
    for i in range(min(20, len(cluster_assignments))):
        print(f"{i:5d} | {cluster_assignments[i]:17d} | {true_labels[i]:10d}")
    
    print("\n" + "="*60)
    print("HIERARCHICAL CLUSTERING COMPLETED")
    print("="*60)


if __name__ == "__main__":
    main()
