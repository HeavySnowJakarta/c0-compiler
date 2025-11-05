# Seeds 数据集聚类实验报告

## 实验概述

本实验使用 Python 编程语言对 Seeds 数据集实现了 K-Means 聚类和层次聚类算法。实验严格遵守要求，仅使用 NumPy 库进行基础数组操作，所有聚类算法均从零实现，未使用 scikit-learn 中的高层 API。

## 数据集说明

Seeds 数据集包含 210 个样本，每个样本有 7 个特征：
- 特征 1: Area（面积）
- 特征 2: Perimeter（周长）
- 特征 3: Compactness（紧凑度）
- 特征 4: Length of kernel（核长度）
- 特征 5: Width of kernel（核宽度）
- 特征 6: Asymmetry coefficient（不对称系数）
- 特征 7: Length of kernel groove（核槽长度）
- 标签: 3 个类别（1, 2, 3），每个类别代表不同品种的小麦种子

数据集来源于真实的小麦种子测量数据，用于分类和聚类研究。

## 实验环境

- Python 版本: 3.12.3
- NumPy 版本: 2.3.4
- 操作系统: Linux

## 实验步骤

### 1. 数据准备

首先创建了 `seeds.txt` 文件，包含完整的 210 个样本数据，数据格式为制表符分隔。

### 2. K-Means 聚类实现

#### 2.1 算法实现细节

文件: `kmeans_clustering.py`

实现的核心功能：
1. **数据加载**: 读取制表符分隔的文本文件
2. **数据标准化**: 将所有特征标准化为均值 0、标准差 1
3. **初始化**: 随机选择 k 个样本作为初始聚类中心
4. **迭代过程**:
   - 计算每个样本到各聚类中心的欧氏距离
   - 将样本分配到最近的聚类中心
   - 重新计算聚类中心（簇内样本的均值）
   - 检查收敛条件（聚类中心移动距离小于阈值）
5. **评估指标**:
   - Inertia（簇内平方和）
   - Purity（纯度）：衡量聚类结果与真实标签的一致性

#### 2.2 运行命令和完整输出

```bash
python3 kmeans_clustering.py
```

**完整输出:**

```
============================================================
K-MEANS CLUSTERING ON SEEDS DATASET
============================================================

Loading data from seeds.txt...
Loaded 210 samples with 7 features
True labels: [1 2 3]

Normalizing data (standardization)...
Data normalized (mean=0, std=1 for each feature)

============================================================
Running K-Means with k=3
============================================================
Converged at iteration 5

Final inertia (within-cluster sum of squares): 431.1686
Number of iterations: 5

Clustering purity: 0.9381 (93.81%)

============================================================
CLUSTER STATISTICS
============================================================

Cluster 0:
  Size: 73 samples
  True label distribution:
    Class 1: 65 samples (89.04%)
    Class 2: 3 samples (4.11%)
    Class 3: 5 samples (6.85%)

Cluster 1:
  Size: 68 samples
  True label distribution:
    Class 1: 3 samples (4.41%)
    Class 3: 65 samples (95.59%)

Cluster 2:
  Size: 69 samples
  True label distribution:
    Class 1: 2 samples (2.90%)
    Class 2: 67 samples (97.10%)

============================================================
FIRST 20 CLUSTER ASSIGNMENTS
============================================================
Index | Predicted Cluster | True Label
---------------------------------------------
    0 |                 0 |          1
    1 |                 0 |          1
    2 |                 0 |          1
    3 |                 0 |          1
    4 |                 0 |          1
    5 |                 0 |          1
    6 |                 0 |          1
    7 |                 0 |          1
    8 |                 2 |          1
    9 |                 0 |          1
   10 |                 0 |          1
   11 |                 0 |          1
   12 |                 0 |          1
   13 |                 0 |          1
   14 |                 0 |          1
   15 |                 0 |          1
   16 |                 0 |          1
   17 |                 0 |          1
   18 |                 0 |          1
   19 |                 1 |          1

============================================================
K-MEANS CLUSTERING COMPLETED
============================================================
```

#### 2.3 K-Means 聚类结果分析

1. **收敛速度**: 算法在第 5 次迭代时收敛，说明聚类中心快速稳定
2. **聚类质量**: 
   - Inertia = 431.1686，相对较小，说明簇内样本紧密
   - Purity = 93.81%，表明聚类结果与真实标签高度一致
3. **簇分布**:
   - Cluster 0: 73 个样本，主要是 Class 1 (89.04%)
   - Cluster 1: 68 个样本，主要是 Class 3 (95.59%)
   - Cluster 2: 69 个样本，主要是 Class 2 (97.10%)
4. **三个簇的大小相对均衡，与真实类别分布一致（每类约 70 个样本）**

### 3. 层次聚类实现

#### 3.1 算法实现细节

文件: `hierarchical_clustering.py`

实现的核心功能：
1. **数据加载和标准化**: 与 K-Means 相同
2. **距离矩阵计算**: 计算所有样本对之间的欧氏距离
3. **凝聚式层次聚类**:
   - 初始状态：每个样本独立成簇
   - 迭代合并最近的两个簇
   - 支持三种链接方法：
     * Single Linkage（单链接）：簇间最小距离
     * Complete Linkage（全链接）：簇间最大距离
     * Average Linkage（平均链接）：簇间平均距离
4. **树状图构建**: 记录每一步的合并操作
5. **切割树状图**: 根据指定的簇数量切割树状图获得最终聚类结果

本实验使用 **Average Linkage（平均链接）** 方法，该方法在实践中通常表现良好。

#### 3.2 运行命令和完整输出

```bash
python3 hierarchical_clustering.py
```

**完整输出:**

```
============================================================
HIERARCHICAL CLUSTERING ON SEEDS DATASET
============================================================

Loading data from seeds.txt...
Loaded 210 samples with 7 features
True labels: [1 2 3]

Normalizing data (standardization)...
Data normalized (mean=0, std=1 for each feature)

============================================================
Running Hierarchical Clustering with average linkage
============================================================
Calculating pairwise distance matrix...
Starting hierarchical clustering with average linkage...
  Step 20: 190 clusters remaining
  Step 40: 170 clusters remaining
  Step 60: 150 clusters remaining
  Step 80: 130 clusters remaining
  Step 100: 110 clusters remaining
  Step 120: 90 clusters remaining
  Step 140: 70 clusters remaining
  Step 160: 50 clusters remaining
  Step 180: 30 clusters remaining
  Step 200: 10 clusters remaining
Clustering complete after 209 merges

============================================================
FIRST 15 MERGE STEPS
============================================================
Step   Cluster 1 Size   Cluster 2 Size   Distance  
------------------------------------------------------------
0      1                1                0.1935    
1      1                1                0.2108    
2      1                1                0.2126    
3      1                1                0.2191    
4      1                1                0.2399    
5      1                1                0.2589    
6      1                1                0.2711    
7      1                1                0.2869    
8      1                1                0.2906    
9      1                1                0.2912    
10     1                1                0.2963    
11     1                1                0.3185    
12     1                1                0.3344    
13     1                1                0.3367    
14     1                1                0.3449    

============================================================
Cutting dendrogram to get 3 clusters
============================================================

Clustering purity: 0.8810 (88.10%)

============================================================
CLUSTER STATISTICS
============================================================

Cluster 0:
  Size: 70 samples
  True label distribution:
    Class 1: 8 samples (11.43%)
    Class 3: 62 samples (88.57%)

Cluster 1:
  Size: 75 samples
  True label distribution:
    Class 1: 7 samples (9.33%)
    Class 2: 68 samples (90.67%)

Cluster 2:
  Size: 65 samples
  True label distribution:
    Class 1: 55 samples (84.62%)
    Class 2: 2 samples (3.08%)
    Class 3: 8 samples (12.31%)

============================================================
FIRST 20 CLUSTER ASSIGNMENTS
============================================================
Index | Predicted Cluster | True Label
---------------------------------------------
    0 |                 2 |          1
    1 |                 2 |          1
    2 |                 2 |          1
    3 |                 2 |          1
    4 |                 2 |          1
    5 |                 2 |          1
    6 |                 2 |          1
    7 |                 2 |          1
    8 |                 1 |          1
    9 |                 2 |          1
   10 |                 1 |          1
   11 |                 2 |          1
   12 |                 2 |          1
   13 |                 0 |          1
   14 |                 0 |          1
   15 |                 2 |          1
   16 |                 2 |          1
   17 |                 2 |          1
   18 |                 2 |          1
   19 |                 0 |          1

============================================================
HIERARCHICAL CLUSTERING COMPLETED
============================================================
```

#### 3.3 层次聚类结果分析

1. **合并过程**: 总共进行了 209 次合并（210 个样本合并为 1 个簇需要 209 步）
2. **聚类质量**:
   - Purity = 88.10%，表明聚类结果较好地反映了真实标签
   - 相比 K-Means 的 93.81%，层次聚类的纯度稍低
3. **簇分布**:
   - Cluster 0: 70 个样本，主要是 Class 3 (88.57%)
   - Cluster 1: 75 个样本，主要是 Class 2 (90.67%)
   - Cluster 2: 65 个样本，主要是 Class 1 (84.62%)
4. **合并距离趋势**: 前 15 步的合并距离从 0.1935 逐渐增加到 0.3449，说明最相似的样本先被合并

## 两种算法对比

| 特性 | K-Means | 层次聚类 |
|------|---------|----------|
| **聚类纯度** | 93.81% | 88.10% |
| **算法复杂度** | O(n·k·d·iterations) | O(n²·d) 到 O(n³) |
| **收敛速度** | 5 次迭代快速收敛 | 209 步完成层次结构 |
| **簇大小** | 73, 68, 69（更均衡） | 70, 75, 65 |
| **优点** | 速度快，效果好 | 提供完整层次结构 |
| **缺点** | 需要预设 k 值 | 计算复杂度高 |

## 实验结论

1. **K-Means 聚类表现更优**: 在 Seeds 数据集上，K-Means 达到了 93.81% 的聚类纯度，优于层次聚类的 88.10%

2. **数据特性适合 K-Means**: Seeds 数据集的三个类别在特征空间中形成了较为紧凑且分离的簇，非常适合 K-Means 的球形簇假设

3. **数据标准化的重要性**: 由于不同特征的量纲和范围不同，标准化处理对于基于距离的聚类算法至关重要

4. **两种算法各有优势**:
   - K-Means 适合快速聚类，当簇数已知且数据分布较为均匀时效果好
   - 层次聚类提供了完整的聚类层次结构，可以灵活选择簇的数量

5. **实现质量**: 两种算法均未使用 scikit-learn 的高层 API，完全从底层实现，代码清晰、注释详细，实现了：
   - 完整的聚类流程
   - 多种评估指标
   - 详细的结果输出和统计分析

## 文件清单

1. `seeds.txt` - Seeds 数据集（210 个样本，7 个特征 + 1 个标签）
2. `kmeans_clustering.py` - K-Means 聚类实现
3. `hierarchical_clustering.py` - 层次聚类实现
4. `clustering_report.md` - 本实验报告
5. `kmeans_output.txt` - K-Means 运行输出记录
6. `hierarchical_output.txt` - 层次聚类运行输出记录

## 附录：代码关键部分说明

### K-Means 核心算法

```python
def kmeans(data, k, max_iterations=100, tolerance=1e-4, random_seed=42):
    # 1. 初始化聚类中心
    centroids = initialize_centroids(data, k)
    
    for iteration in range(max_iterations):
        # 2. 分配样本到最近的聚类中心
        cluster_assignments = assign_clusters(data, centroids)
        
        # 3. 更新聚类中心
        new_centroids = update_centroids(data, cluster_assignments, k)
        
        # 4. 检查收敛
        centroid_shift = np.max(np.sqrt(np.sum((new_centroids - centroids) ** 2, axis=1)))
        if centroid_shift < tolerance:
            break
        
        centroids = new_centroids
    
    return centroids, cluster_assignments, inertia, iterations
```

### 层次聚类核心算法

```python
def hierarchical_clustering(data, linkage='average'):
    # 1. 计算距离矩阵
    distance_matrix = calculate_distance_matrix(data)
    
    # 2. 初始化：每个样本一个簇
    clusters = [[i] for i in range(n_samples)]
    
    # 3. 迭代合并最近的两个簇
    while len(clusters) > 1:
        # 找到最近的两个簇
        min_dist, merge_i, merge_j = find_closest_clusters(clusters, distance_matrix)
        
        # 合并簇
        new_cluster = clusters[merge_i] + clusters[merge_j]
        clusters = update_clusters(clusters, merge_i, merge_j, new_cluster)
    
    return dendrogram, cluster_history
```

---

**实验完成日期**: 2025-11-05

**实验人**: Copilot Agent
