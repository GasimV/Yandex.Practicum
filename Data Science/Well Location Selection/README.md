# Well Location Selection Model

Welcome to the Well Location Selection Model project repository. In this project, we build a predictive model to choose the best location for drilling a new well for the mining company GlavRosGosNeft. The objective is to maximize profit while minimizing risks for each of the three regions under consideration.

## Project Description

The mining company GlavRosGosNeft needs to select a location for drilling a new well. The process involves collecting characteristics for wells, building a model to predict reserve volumes, selecting wells with the highest estimated values, and determining the region with the maximum total profit from the selected wells. We are provided with oil samples from three regions, and the goal is to build a model to determine the most profitable region.

### File Paths

Geological exploration data for three regions can be found in the following files:
- `/datasets/geo_data_0.csv`
- `/datasets/geo_data_1.csv`
- `/datasets/geo_data_2.csv`

## Instructions for Completing the Project

1. Download and prepare the data, explaining the procedure.
2. Train and test a model for each region:
   - 2.1. Split the data into training and validation sets in a 75:25 ratio.
   - 2.2. Train the model and make predictions on the validation set.
   - 2.3. Store predictions and correct answers on the validation set.
   - 2.4. Print the average inventory of predicted raw materials and the RMSE of the model.
   - 2.5. Analyze the results.
3. Prepare to calculate your profit:
   - 3.1. Save all key values for calculations in separate variables.
   - 3.2. Calculate the sufficient volume of raw materials to break even the development of a new well. Compare with the average stock in each region.
   - 3.3. Write conclusions on the stage of preparing the profit calculation.
4. Write a function to calculate profits for selected wells and model predictions:
   - 4.1. Select wells with maximum prediction values.
   - 4.2. Sum up the raw material volume target corresponding to these predictions.
   - 4.3. Calculate the profit for the received volume of raw materials.
5. Calculate the risks and benefits for each region:
   - 5.1. Apply the Bootstrap technique with 1000 samples to find the profit distribution.
   - 5.2. Find the average return, 95% confidence interval, and risk of loss.
   - 5.3. Write your conclusions: suggest a region for well development and justify your choice.

## Data Description

Geological exploration data for three regions include the following features:

- id: Unique identifier of the well
- f0, f1, f2: Three significant features of points
- product: Volume of reserves in the well (thousand barrels)

Conditions of the problem:
- Only linear regression is suitable for training the model.
- During exploration, 500 points are examined, and the best 200 are selected for development.
- The budget for well development in the region is 10 billion rubles.
- One barrel of raw materials brings 450 rubles in income.
- The income from each unit of product is 450 thousand rubles since the volume is in thousands of barrels.
- After assessing the risks, regions with a probability of losses greater than 2.5% are excluded.
- The data is synthetic and does not disclose details of contracts or field characteristics.

Feel free to explore the Jupyter Notebook and code provided in this repository for a detailed analysis and well location selection using linear regression models and risk assessment techniques.