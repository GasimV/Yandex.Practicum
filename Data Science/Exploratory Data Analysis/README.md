## Project Description

The project utilizes data from the Yandex Real Estate service, comprising an archive of apartment sale advertisements in St. Petersburg and its neighboring areas over several years. The primary goal is to understand how to assess the market value of real estate effectively. This involves conducting exploratory data analysis to identify key parameters influencing property prices. The insights gained will contribute to the development of an automated system capable of detecting anomalies and fraudulent activities in real estate advertisements.

### Data Description

The dataset includes various features, including:

- `children`: Number of children in the family
- `days_employed`: Total length of service in days
- `dob_years`: Client age in years
- `education`: Client's education level
- `education_id`: Education level identifier
- `family_status`: Marital status
- `family_status_id`: Marital status identifier
- `gender`: Client gender
- `income_type`: Employment type
- `debt`: Whether there was a debt to repay loans
- `total_income`: Monthly income
- `purpose`: Purpose of obtaining a loan

And additional features such as distance to the nearest airport, number of balconies, ceiling height, distance to the city center, days an advertisement was posted, floor details, kitchen area, and many more.

## Instructions for Completing the Project

The project is divided into several steps, including:

1. Reviewing the general information of the dataset.
2. Preprocessing the data by handling missing values, data type conversions, and dealing with duplicates.
3. Adding new columns with parameters like price per square meter, day of the week, month, year of publication, apartment floor type, and distance to the city center in kilometers.
4. Conducting exploratory data analysis (EDA) on various parameters, including handling anomalies, determining factors influencing property prices, and analyzing the time it takes for apartments to sell.
5. Analyzing the dependencies between price and factors such as total area, living space, kitchen area, number of rooms, floor type, and posting dates.
6. Calculating the average price per square meter in different localities and analyzing the relationship between property prices and distance from the city center.

## Author

Gasym A. Valiyev