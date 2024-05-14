# Text-Image Matching for Photo Search PoC

Welcome to the GitHub repository for the "With Sense Photo Search PoC" project. In this project, we aim to develop a Proof of Concept (PoC) for an on-demand image search system that allows photographers to search for photos based on text descriptions. This PoC will serve as a demonstration of the feasibility of such a project.

## Project Description

At "With Sense," a photo hosting site for professional photographers, users upload photos along with detailed descriptions that include information such as the location of the shooting, camera model, and more. The unique feature of this service is that descriptions can be provided not only by the photo uploader but also by other users of the platform.

The goal of this project is to develop a reference photo search system where users can enter a textual description of a scene, and the system will display photos with similar scenes. This involves using machine learning models to represent images and text as vectors and then assessing the similarity between them. The PoC will help secure approval and funding for further development.

## Legal Restrictions

It's important to note that some countries where "With Sense" operates have legal restrictions on processing images, especially those involving children under the age of 16. The platform strictly adheres to these laws. In this PoC, we will not have the capability to handle this restriction, so we'll focus on data cleaning and displaying disclaimers when necessary.

## Data Description

The project involves working with several datasets:

- `train_dataset.csv`: Contains image file names, description IDs, and description text.
- `train_images`: Folder containing images for training the model.
- `CrowdAnnotations.tsv`: Contains image and description matching data obtained through crowdsourcing.
- `ExpertAnnotations.tsv`: Contains data on the correspondence between images and descriptions obtained from expert assessments.
- `test_queries.csv`: Contains information for testing, including query IDs, query text, and relevant images.
- `test_images`: Folder containing images for testing the model.

## Instructions for Completing the Project

This project consists of several steps:

### Step 1: Data Exploration

- Download and open the data files.
- Check the data for correctness and make necessary corrections.

### Step 2: Data Preparation

- Create a list of words that might be subject to legal restrictions.
- Exclude pairs from the training dataset that may fall under legal restrictions based on the list of words.
- Vectorize the text descriptions using TF-IDF, BERT, or word2vec.
- Vectorize images using the ResNet50 model from Keras or PyTorch.
- Describe the resulting pairs of vectors in terms of their dimensions.

### Step 3: Model Training

- Create a model that measures the proximity of two vectors.
- The model should take a concatenated vector of description and image vectors as input and predict expert assessments.
- Select a metric for comparing the accuracy of different models.
- Train several models and tune hyperparameters (consider linear regression and fully connected neural networks).

### Step 4: Testing and Demonstration

- Test the best model on the test data.
- Write a function that takes a text description as input, vectorizes it, and returns the image with the highest metric value.
- Ensure that the function displays a disclaimer if the request leads to legally harmful content.

### Step 5: Conclusion

- Draw a general conclusion about the best-performing model.
- Describe any mistakes the model makes when searching for photos based on text descriptions.
- Evaluate the feasibility of creating a service for searching photos by text description.

## Project Environment

This project should be completed in a Jupyter Notebook, with code cells for program code and markdown cells for text explanations. You may require a GPU for this project, so consider using Kaggle, Google Colab, DeepNote, Yandex Data Sphere, or local resources.

Thank you for your interest in the "With Sense Photo Search PoC" project. We hope this PoC will help demonstrate the potential of creating a photo search service based on text descriptions.