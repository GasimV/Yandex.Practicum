# Megaline Tariff Classification Model

Welcome to the Megaline Tariff Classification Model project repository. In this project, we build a classification model to predict whether a customer should be offered the "Smart" or "Ultra" tariff based on their behavior data. The objective is to achieve a classification accuracy of at least 75% using a dataset of customers who have already switched to these tariffs.

## Project Description

Megaline, a mobile operator, wants to offer its customers a new tariff based on their behavior. We have data on the behavior of customers who have already switched to the "Smart" or "Ultra" tariffs. The task is to build a classification model for this task, with the highest possible accuracy.

### File Path

The source data for this project can be found in the file: `/datasets/users_behavior.csv`.

## Instructions for Completing the Project

1. Open the data file and examine it.
2. Divide the source data into training, validation, and test sets.
3. Explore the quality of different models by changing hyperparameters and summarize your findings.
4. Check the quality of the model on a test sample.

Additional Task (Optional):
- Check the models for sanity, although this data may be more complex than what you've worked with before.

## Data Description

Each object in the dataset represents information about the behavior of one user for a month. The following features are known:
- calls: Number of calls
- minutes: Total duration of calls in minutes
- messages: Number of SMS messages
- mb_used: Consumed Internet traffic in MB
- is_ultra: Target variable indicating the tariff used during the month ("Ultra" - 1, "Smart" - 0).

Feel free to explore the Jupyter Notebook and code provided in this repository for a detailed analysis and classification model development.