import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from scipy.stats import pearsonr
import statsmodels.api as sm

# Dataset URL
url = "https://raw.githubusercontent.com/vincentarelbundock/Rdatasets/master/csv/MASS/birthwt.csv"

# Load data
data = pd.read_csv(url)

print("Columns in dataset:", data.columns)
print(data.head())

# Variables of interest
age = data['age']
lwt = data['lwt']
bwt = data['bwt']

# 1. Age vs Birth weight
corr_age, pval_age = pearsonr(age, bwt)
print("\n--- Age vs Birth Weight ---")
print(f"Correlation: {corr_age:.3f}, p-value: {pval_age:.3f}")

X_age = sm.add_constant(age)
model_age = sm.OLS(bwt, X_age).fit()
print(model_age.summary())

# 2. Mother's weight vs Birth weight
corr_lwt, pval_lwt = pearsonr(lwt, bwt)
print("\n--- Mother's Weight vs Birth Weight ---")
print(f"Correlation: {corr_lwt:.3f}, p-value: {pval_lwt:.3f}")

X_lwt = sm.add_constant(lwt)
model_lwt = sm.OLS(bwt, X_lwt).fit()
print(model_lwt.summary())

# 3. Scatter plots
plt.figure(figsize=(12, 5))

# Age vs Birth Weight
plt.subplot(1, 2, 1)
sns.regplot(x=age, y=bwt, scatter_kws={'alpha': 0.6}, line_kws={'color': 'red'})
plt.title("Mother's Age vs Birth Weight")
plt.xlabel("Mother's Age (years)")
plt.ylabel("Birth Weight (grams)")

# Mother's Weight vs Birth Weight
plt.subplot(1, 2, 2)
sns.regplot(x=lwt, y=bwt, scatter_kws={'alpha': 0.6}, line_kws={'color': 'blue'})
plt.title("Mother's Weight vs Birth Weight")
plt.xlabel("Mother's Weight (lbs)")
plt.ylabel("Birth Weight (grams)")

plt.tight_layout()
plt.show()

"""
┌──(venv)─(arfat㉿kali)-[~/Desktop/CS-AIML]
└─$ pip python3 Practical_3.py   
Columns in dataset: Index(['rownames', 'low', 'age', 'lwt', 'race', 'smoke', 'ptl', 'ht', 'ui',
       'ftv', 'bwt'],
      dtype='object')
   rownames  low  age  lwt  race  ...  ptl  ht  ui  ftv   bwt
0        85    0   19  182     2  ...    0   0   1    0  2523
1        86    0   33  155     3  ...    0   0   0    3  2551
2        87    0   20  105     1  ...    0   0   0    1  2557
3        88    0   21  108     1  ...    0   0   1    2  2594
4        89    0   18  107     1  ...    0   0   1    0  2600

[5 rows x 11 columns]

--- Age vs Birth Weight ---
Correlation: 0.090, p-value: 0.216
                            OLS Regression Results                            
==============================================================================
Dep. Variable:                    bwt   R-squared:                       0.008
Model:                            OLS   Adj. R-squared:                  0.003
Method:                 Least Squares   F-statistic:                     1.538
Date:                Wed, 08 Oct 2025   Prob (F-statistic):              0.216
Time:                        23:31:25   Log-Likelihood:                -1512.8
No. Observations:                 189   AIC:                             3030.
Df Residuals:                     187   BIC:                             3036.
Df Model:                           1                                         
Covariance Type:            nonrobust                                         
==============================================================================
                 coef    std err          t      P>|t|      [0.025      0.975]
------------------------------------------------------------------------------
const       2655.7445    238.857     11.119      0.000    2184.544    3126.945
age           12.4297     10.023      1.240      0.216      -7.343      32.202
==============================================================================
Omnibus:                        2.779   Durbin-Watson:                   0.186
Prob(Omnibus):                  0.249   Jarque-Bera (JB):                2.692
Skew:                          -0.291   Prob(JB):                        0.260
Kurtosis:                       2.951   Cond. No.                         108.
==============================================================================

Notes:
[1] Standard Errors assume that the covariance matrix of the errors is correctly specified.

--- Mother's Weight vs Birth Weight ---
Correlation: 0.186, p-value: 0.011
                            OLS Regression Results                            
==============================================================================
Dep. Variable:                    bwt   R-squared:                       0.034
Model:                            OLS   Adj. R-squared:                  0.029
Method:                 Least Squares   F-statistic:                     6.681
Date:                Wed, 08 Oct 2025   Prob (F-statistic):             0.0105
Time:                        23:31:25   Log-Likelihood:                -1510.2
No. Observations:                 189   AIC:                             3024.
Df Residuals:                     187   BIC:                             3031.
Df Model:                           1                                         
Covariance Type:            nonrobust                                         
==============================================================================
                 coef    std err          t      P>|t|      [0.025      0.975]
------------------------------------------------------------------------------
const       2369.6235    228.493     10.371      0.000    1918.868    2820.379
lwt            4.4291      1.713      2.585      0.011       1.049       7.809
==============================================================================
Omnibus:                        2.099   Durbin-Watson:                   0.265
Prob(Omnibus):                  0.350   Jarque-Bera (JB):                1.757
Skew:                          -0.223   Prob(JB):                        0.415
Kurtosis:                       3.153   Cond. No.                         583.
==============================================================================

Notes:
[1] Standard Errors assume that the covariance matrix of the errors is correctly specified.

"""