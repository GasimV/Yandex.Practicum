# Customer Churn Prediction Model

Welcome to the Customer Churn Prediction Model project repository. In this project, we build a predictive model to determine whether a client will leave Beta Bank in the near future based on historical data. The project aims to achieve a high F1-measure value (≥ 0.59) and compares it with the AUC-ROC metric.

## Project Description

Beta Bank has been experiencing customer churn, and the bank's marketers have realized that it's more cost-effective to retain current customers than to attract new ones. The task at hand is to predict whether a client is likely to leave the bank, leveraging historical data on customer behavior and contract terminations.

### File Path

The data for this project can be found in the file: `/datasets/Churn.csv`.

## Instructions for Completing the Project

1. Download and prepare the data, explaining the procedure.
2. Explore class balance and initially train the model without considering class imbalance. Provide a brief description of your findings.
3. Improve the model's quality while taking class imbalance into account. Train different models and identify the best one. Summarize your findings.
4. Conduct final testing of the model.

## Data Description

The dataset includes the following features:

- RowNumber: Index of the row in the data
- CustomerId: Unique client identifier
- Surname: Surname of the client
- CreditScore: Credit rating
- Geography: Country of residence
- Gender: Gender
- Age: Age
- Tenure: Number of years the person has been a bank client
- Balance: Account balance
- NumOfProducts: Number of bank products used by the client
- HasCrCard: Availability of a credit card
- IsActiveMember: Client activity
- EstimatedSalary: Estimated salary

Target Feature:
- Exited: Indicates whether the client has left the bank (1 for leaving, 0 for staying)

Feel free to explore the Jupyter Notebook and code provided in this repository for detailed data analysis, model development, and evaluation of customer churn prediction.