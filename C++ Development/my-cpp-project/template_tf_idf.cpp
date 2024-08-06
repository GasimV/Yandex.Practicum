#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

template <typename Documents, typename Term>
vector<double> ComputeTfIdfs(const Documents& documents, const Term& term) {
    vector<double> tf_idf_scores;
    int total_documents = documents.size();
    double document_frequency = 0;
    
    // Calculate Term Frequency (TF) and Document Frequency (DF)
    for (const auto& document : documents) {
        int term_count = 0;
        int total_terms = document.size();
        
        for (const auto& word : document) {
            if (word == term) {
                term_count += 1;
            }
        }
        
        double tf = static_cast<double>(term_count) / total_terms;
        tf_idf_scores.push_back(tf); // Append TF score to the result vector
        
        if (term_count > 0) {
            document_frequency += 1;
        }
    }
    
    // Calculate Inverse Document Frequency (IDF)
    double idf;
    if (document_frequency > 0) {
        idf = log(static_cast<double>(total_documents) / document_frequency);
    } else {
        idf = 0;
    }
    
    // Calculate TF-IDF for each document
    for (auto& score : tf_idf_scores) { // Update each TF score by multiplying with IDF
        score *= idf;
    }
    
    return tf_idf_scores; // Return the result vector
}

int main() {
    const vector<vector<string>> documents = {
        {"белый"s, "кот"s, "и"s, "модный"s, "ошейник"s},
        {"пушистый"s, "кот"s, "пушистый"s, "хвост"s},
        {"ухоженный"s, "пёс"s, "выразительные"s, "глаза"s},
    };
    const auto tf_idfs = ComputeTfIdfs(documents, "кот"s);
    for (const double tf_idf : tf_idfs) {
        cout << tf_idf << " "s;
    }
    cout << endl;
    return 0;
}