import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA

# Dataset URL
url = "https://raw.githubusercontent.com/vincentarelbundock/Rdatasets/master/csv/datasets/iris.csv"

# Load data
iris = pd.read_csv(url)


print("First 5 rows of dataset:\n", iris.head())
print("\nDataset description:\n", iris.describe())
print("\nColumns:", iris.columns)

# Features and target
X = iris.iloc[:, :-1]  # numeric features
y = iris.iloc[:, -1]   # species

# --- Step 1: Standardization ---
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)
print("\nData has been standardized (mean ~0, variance ~1).")

# --- Step 2: Correlation matrix ---
corr_matrix = pd.DataFrame(X).corr()
print("\nCorrelation matrix:\n", corr_matrix)

plt.figure(figsize=(6, 5))
sns.heatmap(corr_matrix, annot=True, cmap="coolwarm")
plt.title("Correlation Matrix of Iris Features")
plt.show()

# --- Step 3: PCA computation ---
pca = PCA()
X_pca = pca.fit_transform(X_scaled)

explained_var = pca.explained_variance_ratio_
cumulative_var = np.cumsum(explained_var)
print("\nExplained variance ratio (per PC):", explained_var)
print("Cumulative explained variance:", cumulative_var)

# Determine K (keep components explaining ~95%)
K = np.argmax(cumulative_var >= 0.95) + 1
print(f"\nOptimal number of components (K): {K}")

# --- Step 4: Principal component directions (loadings) ---
loadings = pd.DataFrame(
    pca.components_.T,
    columns=[f"PC{i+1}" for i in range(len(X.columns))],
    index=X.columns
)
print("\nPrincipal Component Directions (Loadings):\n", loadings)

# Correlation of PCs with original variables
pc_corr = np.corrcoef(X_scaled.T, X_pca, rowvar=True)[:X.shape[1], X.shape[1]:]
pc_corr_df = pd.DataFrame(
    pc_corr,
    index=X.columns,
    columns=[f"PC{i+1}" for i in range(len(X.columns))]
)
print("\nCorrelations of PCs with original variables:\n", pc_corr_df)

# --- Step 5: Projection Plot (first 2 PCs) ---
plt.figure(figsize=(8, 6))
sns.scatterplot(x=X_pca[:, 0], y=X_pca[:, 1], hue=y, palette="Set1", s=70)
plt.xlabel("PC1")
plt.ylabel("PC2")
plt.title("Iris Data projected onto First 2 Principal Components")
plt.legend(title="Species")
plt.show()
"""
Output:
First 5 rows of dataset:
    rownames  Sepal.Length  ...  Petal.Width  Species
0         1           5.1  ...          0.2   setosa
1         2           4.9  ...          0.2   setosa
2         3           4.7  ...          0.2   setosa
3         4           4.6  ...          0.2   setosa
4         5           5.0  ...          0.2   setosa

[5 rows x 6 columns]

Dataset description:
          rownames  Sepal.Length  ...  Petal.Length  Petal.Width
count  150.000000    150.000000  ...    150.000000   150.000000
mean    75.500000      5.843333  ...      3.758000     1.199333
std     43.445368      0.828066  ...      1.765298     0.762238
min      1.000000      4.300000  ...      1.000000     0.100000
25%     38.250000      5.100000  ...      1.600000     0.300000
50%     75.500000      5.800000  ...      4.350000     1.300000
75%    112.750000      6.400000  ...      5.100000     1.800000
max    150.000000      7.900000  ...      6.900000     2.500000

[8 rows x 5 columns]

Columns: Index(['rownames', 'Sepal.Length', 'Sepal.Width', 'Petal.Length',
       'Petal.Width', 'Species'],
      dtype='object')

Data has been standardized (mean ~0, variance ~1).

Correlation matrix:
               rownames  ...  Petal.Width
rownames      1.000000  ...     0.900027
Sepal.Length  0.716676  ...     0.817941
Sepal.Width  -0.402301  ...    -0.366126
Petal.Length  0.882637  ...     0.962865
Petal.Width   0.900027  ...     1.000000

[5 rows x 5 columns]
"""