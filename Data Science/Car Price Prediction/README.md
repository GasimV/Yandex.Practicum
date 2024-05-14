# Car Price Prediction Model

Welcome to the Car Price Prediction Model project repository. In this project, we aim to develop a model for determining the market value of used cars for the "Not Bit, Not Painted" used car sales service. The goal is to provide customers with accurate price estimates based on technical characteristics, equipment, and prices of other cars.

## Project Description

The "Not Bit, Not Painted" service is developing an application to help customers determine the market value of their cars. We are tasked with building a prediction model for this purpose. The project's success criteria include the quality of predictions, model training time, and model prediction time.

### File Path

The dataset used in this project can be found in the file: `/datasets/autos.csv`.

## Instructions for Completing the Project

1. Download the data.
2. Explore the data, filling in missing values, and handling anomalies in columns. Remove uninformative features if present.
3. Prepare samples for training machine learning models.
4. Train different models, including at least one that is not boosted (e.g., simpler models). Experiment with different hyperparameters.
5. Analyze training time, prediction time, and model quality.
6. Choose the best model based on the customer's criteria and evaluate its quality on a test sample.

## Notes

- To assess model quality, use the RMSE metric, aiming for an RMSE value less than 2500.
- Master the LightGBM library for building gradient boosting models.
- Use commands to measure code cell execution time in Jupyter Notebook.
- Experiment with gradient boosting models by changing a small number of parameters.
- In case of memory issues, use the `del` operator to remove unnecessary variables.

## Data Description

The dataset includes the following features:

- DateCrawled: Date the profile was downloaded from the database
- VehicleType: Car body type
- RegistrationYear: Year of vehicle registration
- Gearbox: Gearbox type
- Power: Power (hp)
- Model: Car model
- Kilometer: Mileage (km)
- RegistrationMonth: Month of vehicle registration
- FuelType: Fuel type
- Brand: Car brand
- Repaired: Whether the car was repaired or not
- DateCreated: Date of creation of the questionnaire
- NumberOfPictures: Number of photos of the car
- PostalCode: Postal code of the profile owner (user)
- LastSeen: Date of last user activity

Target Feature:
- Price: Price (euros)

Feel free to explore the Jupyter Notebook and code provided in this repository for detailed data analysis and car price prediction using various machine learning models while considering training and prediction times.

## Author
- Gasym A. Valiyev