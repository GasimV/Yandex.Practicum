# Megaline Tariff Analysis

Welcome to the Megaline Tariff Analysis project repository. In this project, we analyze the "Smart" and "Ultra" tariff plans offered by Megaline, a federal cellular operator. The goal is to determine which tariff plan is more profitable by examining customer behavior and monthly revenue.

## Project Description

Megaline provides two tariff plans: "Smart" and "Ultra." To adjust the advertising budget, the commercial department wants to understand which tariff brings in more money. To achieve this, we conduct a preliminary analysis of the tariffs using a sample of 500 Megaline users' data, including information on calls, messages, and internet usage in 2018.

### Description of Tariffs

**Tariff "Smart"**
- Monthly fee: 550 rubles
- Includes 500 minutes of talk time, 50 messages, and 15 GB of Internet traffic
- Cost of services above the tariff package:
  - A minute of conversation costs 3 rubles (rounded up)
  - Message - 3 rubles
  - 1 GB of Internet traffic - 200 rubles

**Tariff "Ultra"**
- Monthly fee: 1950 rubles
- Includes 3000 minutes of talk time, 1000 messages, and 30 GB of Internet traffic
- Cost of services above the tariff package:
  - One minute of conversation - 1 ruble
  - Message - 1 ruble
  - 1 GB of Internet traffic - 150 rubles

## Instructions for Completing the Project

1. Examine general information about the data in the files.
2. Perform data analysis, calculate monthly revenue, and draw conclusions.
3. Test two hypotheses about user revenue:
   - Hypothesis 1: Test if the average revenue from users of "Ultra" and "Smart" tariffs differs.
   - Hypothesis 2: Test if the average revenue from users in Moscow differs from revenue from users in other regions.

Please refer to the Jupyter Notebook and code provided in this repository for a detailed step-by-step analysis.

## Data Description

### Table users - Information about users:

- user_id: Unique user identifier
- first_name: User name
- last_name: User's last name
- age: User age (years)
- reg_date: Tariff connection date (day, month, year)
- churn_date: Date of termination of using the tariff (if omitted, the tariff was still in effect)
- city: User’s city of residence
- tariff: Name of the tariff plan

### Calls table - Information about calls:

- id: Unique call number
- call_date: Call date
- duration: Duration of the call in minutes
- user_id: Identifier of the user who made the call

### Messages table - Information about messages:

- id: Unique message number
- message_date: Message date
- user_id: Identifier of the user who sent the message

### Internet table - Information about Internet sessions:

- id: Unique session number
- mb_used: Amount of Internet traffic spent per session (in megabytes)
- session_date: Internet session date
- user_id: User identifier

### Tariffs table - Information about tariffs:

- tariff_name: Tariff name
- rub_monthly_fee: Monthly subscription fee in rubles
- minutes_included: Number of call minutes per month included in the subscription fee
- messages_included: Number of messages per month included in the subscription fee
- mb_per_month_included: Amount of Internet traffic included in the subscription fee (in megabytes)
- rub_per_minute: Cost of a minute of conversation above the tariff package
- rub_per_message: Cost of sending a message above the tariff package
- rub_per_gb: Cost of an additional gigabyte of Internet traffic above the tariff package (1 gigabyte = 1024 megabytes)

Feel free to explore the project's code and data for a detailed analysis of Megaline's tariff plans.