import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy.stats import skew, kurtosis

# Example: Class test results (out of 100 marks)
scores = [68, 74, 81, 77, 63, 89, 72, 66, 84, 91,
          59, 78, 70, 87, 65, 73, 90, 56, 82, 69]


# Calculate statistics
mean = np.mean(scores)
std_dev = np.std(scores)
skewness = skew(scores)
kurt = kurtosis(scores)

# Print summary statistics
print(f"Mean: {mean:.2f}")
print(f"Standard Deviation: {std_dev:.2f}")
print(f"Skewness: {skewness:.2f}")
print(f"Kurtosis: {kurt:.2f}")

# Plot distribution
plt.figure(figsize=(8, 5))
sns.histplot(scores, kde=True, bins=10, color='lightblue',
             stat="density", edgecolor="black")

# Normal curve for comparison
xmin, xmax = plt.xlim()
x = np.linspace(xmin, xmax, 100)
p = 1 / (std_dev * np.sqrt(2 * np.pi)) * np.exp(-(x - mean)**2 / (2 * std_dev**2))
plt.plot(x, p, 'r', linewidth=2, label="Normal Curve")

# Mean & Std deviation lines
plt.axvline(mean, color='blue', linestyle='dashed', linewidth=2, label=f"Mean = {mean:.2f}")
plt.axvline(mean + std_dev, color='orange', linestyle='dashed', linewidth=1, label=f"+1 SD = {mean + std_dev:.2f}")
plt.axvline(mean - std_dev, color='orange', linestyle='dashed', linewidth=1, label=f"-1 SD = {mean - std_dev:.2f}")

# Title and labels
plt.title("Normal Distribution of Class Test Results (Out of 100)")
plt.xlabel("Marks")
plt.ylabel("Density")
plt.legend()
plt.show()

"""
Output:
Mean: 74.70
Standard Deviation: 10.16
Skewness: -0.00
Kurtosis: -1.02
"""