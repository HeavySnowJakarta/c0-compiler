#!/usr/bin/env python3
"""
K-Means Clustering Implementation
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
    """Calculate Euclidean distance between two points."""
    return np.sqrt(np.sum((a - b) ** 2))


def initialize_centroids(data, k, method='random'):
    """
    Initialize k centroids.
    method: 'random' - randomly select k data points
    """
    n_samples = data.shape[0]
    if method == 'random':
        # Randomly select k data points as initial centroids
        indices = np.random.choice(n_samples, k, replace=False)
        return data[indices].copy()
    else:
        raise ValueError(f"Unknown initialization method: {method}")


def assign_clusters(data, centroids):
    """Assign each data point to the nearest centroid."""
    n_samples = data.shape[0]
    k = centroids.shape[0]
    
    # Calculate distances from each point to each centroid
    distances = np.zeros((n_samples, k))
    for i in range(k):
        distances[:, i] = np.sqrt(np.sum((data - centroids[i]) ** 2, axis=1))
    
    # Assign to nearest centroid
    cluster_assignments = np.argmin(distances, axis=1)
    return cluster_assignments


def update_centroids(data, cluster_assignments, k):
    """Update centroids as the mean of points in each cluster."""
    n_features = data.shape[1]
    new_centroids = np.zeros((k, n_features))
    
    for i in range(k):
        # Get all points assigned to cluster i
        cluster_points = data[cluster_assignments == i]
        if len(cluster_points) > 0:
            # Calculate mean of cluster points
            new_centroids[i] = np.mean(cluster_points, axis=0)
        else:
            # If no points assigned, keep the old centroid
            # This shouldn't happen with proper initialization
            print(f"Warning: Cluster {i} has no points assigned")
    
    return new_centroids


def kmeans(data, k, max_iterations=100, tolerance=1e-4, random_seed=42):
    """
    K-Means clustering algorithm.
    
    Parameters:
    - data: numpy array of shape (n_samples, n_features)
    - k: number of clusters
    - max_iterations: maximum number of iterations
    - tolerance: convergence threshold
    - random_seed: seed for reproducibility
    
    Returns:
    - centroids: final centroids
    - cluster_assignments: cluster assignment for each point
    - inertia: within-cluster sum of squares
    - iterations: number of iterations performed
    """
    np.random.seed(random_seed)
    
    # Initialize centroids
    centroids = initialize_centroids(data, k)
    
    for iteration in range(max_iterations):
        # Assign points to nearest centroid
        cluster_assignments = assign_clusters(data, centroids)
        
        # Update centroids
        new_centroids = update_centroids(data, cluster_assignments, k)
        
        # Check for convergence
        centroid_shift = np.max(np.sqrt(np.sum((new_centroids - centroids) ** 2, axis=1)))
        
        centroids = new_centroids
        
        if centroid_shift < tolerance:
            print(f"Converged at iteration {iteration + 1}")
            break
    else:
        print(f"Reached maximum iterations ({max_iterations})")
    
    # Calculate inertia (within-cluster sum of squares)
    inertia = 0
    for i in range(k):
        cluster_points = data[cluster_assignments == i]
        if len(cluster_points) > 0:
            inertia += np.sum((cluster_points - centroids[i]) ** 2)
    
    return centroids, cluster_assignments, inertia, iteration + 1


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


def print_cluster_statistics(data, cluster_assignments, true_labels, k):
    """Print statistics about the clustering results."""
    print("\n" + "="*60)
    print("CLUSTER STATISTICS")
    print("="*60)
    
    for cluster_id in range(k):
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


def main():
    print("="*60)
    print("K-MEANS CLUSTERING ON SEEDS DATASET")
    print("="*60)
    
    # Load data
    print("\nLoading data from seeds.txt...")
    data, true_labels = load_data('seeds.txt')
    print(f"Loaded {data.shape[0]} samples with {data.shape[1]} features")
    print(f"True labels: {np.unique(true_labels)}")
    
    # Normalize data (important for distance-based algorithms)
    print("\nNormalizing data (standardization)...")
    mean = np.mean(data, axis=0)
    std = np.std(data, axis=0)
    data_normalized = (data - mean) / std
    print("Data normalized (mean=0, std=1 for each feature)")
    
    # Run K-Means with k=3 (since we have 3 true classes)
    k = 3
    print(f"\n{'='*60}")
    print(f"Running K-Means with k={k}")
    print(f"{'='*60}")
    
    centroids, cluster_assignments, inertia, iterations = kmeans(
        data_normalized, k, max_iterations=100, random_seed=42
    )
    
    print(f"\nFinal inertia (within-cluster sum of squares): {inertia:.4f}")
    print(f"Number of iterations: {iterations}")
    
    # Calculate purity
    purity = calculate_purity(cluster_assignments, true_labels)
    print(f"\nClustering purity: {purity:.4f} ({purity*100:.2f}%)")
    
    # Print cluster statistics
    print_cluster_statistics(data_normalized, cluster_assignments, true_labels, k)
    
    # Print first few cluster assignments
    print("\n" + "="*60)
    print("FIRST 20 CLUSTER ASSIGNMENTS")
    print("="*60)
    print("Index | Predicted Cluster | True Label")
    print("-" * 45)
    for i in range(min(20, len(cluster_assignments))):
        print(f"{i:5d} | {cluster_assignments[i]:17d} | {true_labels[i]:10d}")
    
    print("\n" + "="*60)
    print("K-MEANS CLUSTERING COMPLETED")
    print("="*60)


if __name__ == "__main__":
    main()
