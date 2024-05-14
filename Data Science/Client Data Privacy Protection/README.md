# Client Data Privacy Protection

Welcome to the Client Data Privacy Protection project repository. In this project, we address the need to protect the data of clients for the insurance company "Though the Flood." Our goal is to develop a data transformation method that makes it difficult to recover personal information while ensuring that the quality of machine learning models remains intact.

## Project Description

The project revolves around protecting clients' personal data while preserving the quality of machine learning models. We aim to achieve this by transforming the data in such a way that it becomes challenging to recover personal information while maintaining the model's performance, specifically in the context of linear regression.

### File Path

The dataset used in this project can be found in the file: `/datasets/insurance.csv`.

## Instructions for Completing the Project

1. Download and explore the data.
2. Answer the question and justify your solution regarding the impact of multiplying features by an invertible matrix on the quality of linear regression:
   - a. Will the quality of linear regression change? Provide examples of matrices.
   - b. Will the quality of linear regression change? Explain the relationship between linear regression parameters in the original and transformed problems.
3. Propose a data transformation algorithm to protect clients' personal data while maintaining linear regression quality. Justify why the quality of linear regression will not change.
4. Program the algorithm using matrix operations and verify that the quality of linear regression from sklearn remains the same before and after the transformation using the R2 metric.

## Data Description

The dataset includes the following features:
- Gender: Gender of the insured
- Age: Age of the insured
- Salary: Salary of the insured
- FamilyMembers: Number of family members

Target Attribute:
- InsurancePayments: Number of insurance payments to the client over the last 5 years

Feel free to explore the Jupyter Notebook and code provided in this repository for a detailed implementation of the data transformation method while ensuring the quality of linear regression models remains unchanged.