import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt

# Load dataset
df = pd.read_csv('personality_synthetic_dataset.csv')

# Select only numeric columns
numeric_cols = df.select_dtypes(include='number').columns.tolist()

# Print descriptive stats
print("\nDescriptive Statistics:\n")
print("| Column\t\t\t| Mean\t| Median\t| Mode\t| Std Dev\t| Variance\t|")

stats = {}
for col in numeric_cols:
    data = df[col].dropna()
    stats[col] = data.mean()
    op  = f"| {col:25} "
    op += f"| {data.mean():.2f}\t"
    op += f"| {data.median():.2f}\t"
    op += f"| {data.mode().tolist()[0]:.2f}\t"
    op += f"| {data.std():.2f}\t"
    op += f"| {data.var():.2f}\t|"
    print(op)

# Plot line graph of average values per trait
plt.figure(figsize=(18, 6))
trait_names = list(stats.keys())
trait_means = list(stats.values())

sns.lineplot(x=trait_names, y=trait_means, marker='o', color='blue')
plt.xticks(rotation=45, ha='right')
plt.ylabel('Average Score')
plt.title('Average Value per Personality Trait')
plt.grid(True)
plt.tight_layout()
plt.show()

"""
Descriptive Statistics:

| Column                        | Mean  | Median        | Mode       | Std Dev       | Variance      |
| social_energy             | 5.51      | 5.54  | 10.00 | 2.47       | 6.09  |
| alone_time_preference     | 5.45      | 5.46  | 10.00 | 2.49       | 6.22  |
| talkativeness             | 5.53      | 5.57  | 10.00 | 2.49       | 6.19  |
| deep_reflection           | 5.96      | 5.97  | 10.00 | 2.16       | 4.68  |
| group_comfort             | 6.01      | 6.05  | 10.00 | 2.18       | 4.75  |
| party_liking              | 5.04      | 5.05  | 10.00 | 2.81       | 7.90  |
| listening_skill           | 6.96      | 6.99  | 10.00 | 1.65       | 2.72  |
| empathy                   | 6.48      | 6.48  | 10.00 | 1.53       | 2.36  |
| creativity                | 6.00      | 6.00  | 10.00 | 1.50       | 2.24  |
| organization              | 5.97      | 5.99  | 10.00 | 1.70       | 2.88  |
| leadership                | 6.02      | 6.04  | 10.00 | 2.17       | 4.70  |
| risk_taking               | 5.01      | 5.01  | 0.00  | 2.19       | 4.82  |
| public_speaking_comfort   | 5.51      | 5.53  | 10.00 | 2.47       | 6.12  |
| curiosity                 | 6.50      | 6.50  | 10.00 | 1.54       | 2.37  |
| routine_preference        | 5.45      | 5.47  | 10.00 | 1.93       | 3.72  |
| excitement_seeking        | 5.53      | 5.54  | 10.00 | 2.49       | 6.19  |
| friendliness              | 6.98      | 7.00  | 10.00 | 1.66       | 2.76  |
| emotional_stability       | 6.02      | 6.03  | 10.00 | 1.49       | 2.22  |
| planning                  | 5.99      | 5.98  | 10.00 | 1.70       | 2.87  |
| spontaneity               | 5.50      | 5.50  | 10.00 | 1.92       | 3.70  |
| adventurousness           | 5.03      | 5.05  | 10.00 | 2.20       | 4.85  |
| reading_habit             | 5.94      | 5.96  | 10.00 | 2.17       | 4.71  |
| sports_interest           | 5.49      | 5.51  | 10.00 | 1.93       | 3.73  |
| online_social_usage       | 6.50      | 6.53  | 10.00 | 1.89       | 3.57  |
| travel_desire             | 5.51      | 5.50  | 10.00 | 1.93       | 3.73  |
| gadget_usage              | 6.01      | 6.01  | 10.00 | 1.68       | 2.83  |
| work_style_collaborative  | 6.50      | 6.53  | 10.00 | 1.89       | 3.57  |
| decision_speed            | 5.52      | 5.53  | 10.00 | 1.94       | 3.78  |
| stress_handling           | 5.98      | 5.98  | 10.00 | 1.49       | 2.22  |

"""