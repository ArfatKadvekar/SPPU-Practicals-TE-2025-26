import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from sklearn.mixture import GaussianMixture
from sklearn.metrics import confusion_matrix, classification_report

# --- Load Iris dataset ---
url = "https://raw.githubusercontent.com/vincentarelbundock/Rdatasets/master/csv/datasets/iris.csv"
iris = pd.read_csv(url)

# Drop the first column if it’s just an index
# iris = iris.drop(columns=["Unnamed: 0"])

# Split into features and labels
X = iris.iloc[:, :-1]  # features
y = iris.iloc[:, -1]   # actual species

# --- Step 1: Standardize features ---
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

# --- Step 2: Apply Gaussian Mixture Model (GMM) ---
gmm = GaussianMixture(n_components=3, random_state=42)
gmm.fit(X_scaled)
clusters = gmm.predict(X_scaled)

# --- Step 3: PCA for 2D visualization ---
pca = PCA(n_components=2)
X_pca = pca.fit_transform(X_scaled)

# --- Step 4: Plotting ---
plt.figure(figsize=(12, 5))

# (i) True species
plt.subplot(1, 2, 1)
sns.scatterplot(x=X_pca[:, 0], y=X_pca[:, 1], hue=y, palette="Set1", s=70)
plt.title("True Iris Species (PCA Projection)")
plt.xlabel("PC1")
plt.ylabel("PC2")

# (ii) GMM clusters
plt.subplot(1, 2, 2)
sns.scatterplot(x=X_pca[:, 0], y=X_pca[:, 1], hue=clusters, palette="Set2", s=70)
plt.title("GMM Clusters (PCA Projection)")
plt.xlabel("PC1")
plt.ylabel("PC2")

plt.tight_layout()
plt.show()

# --- Step 5: Compare clustering with true labels ---
print("\nConfusion Matrix:\n", confusion_matrix(y, clusters))
print("\nClassification Report (clusters vs species):\n")
print(classification_report(y, clusters))
