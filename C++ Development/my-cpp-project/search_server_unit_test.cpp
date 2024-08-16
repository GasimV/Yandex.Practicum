#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <iostream>

using namespace std;

/* Подставьте вашу реализацию класса SearchServer сюда */
const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }

    return words;
}

struct Document {
    int id;
    double relevance;
    int rating;
};

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};

// Overload the << operator for DocumentStatus
ostream& operator<<(ostream& os, DocumentStatus status) {
    switch (status) {
        case DocumentStatus::ACTUAL:
            os << "ACTUAL";
            break;
        case DocumentStatus::IRRELEVANT:
            os << "IRRELEVANT";
            break;
        case DocumentStatus::BANNED:
            os << "BANNED";
            break;
        case DocumentStatus::REMOVED:
            os << "REMOVED";
            break;
    }
    return os;
}

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document, DocumentStatus status,
                     const vector<int>& ratings) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        const double inv_word_count = 1.0 / words.size();
        for (const string& word : words) {
            word_to_document_freqs_[word][document_id] += inv_word_count;
        }
        documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
    }

    // Новый перегруженный метод FindTopDocuments с DocumentStatus
    vector<Document> FindTopDocuments(const string& raw_query, DocumentStatus status) const {
        auto lambda = [status](int /*document_id*/, DocumentStatus doc_status, int /*rating*/) { return doc_status == status; };
        return FindTopDocuments(raw_query, lambda);
    }
    
    // Перегруженный FindTopDocuments со статусом ACTUAL по умолчанию
    vector<Document> FindTopDocuments(const string& raw_query) const {
        return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
    }
    
    // Предикатная версия FindTopDocuments
    template <typename Predicate>
    vector<Document> FindTopDocuments(const string& raw_query, Predicate predicate) const {
        const Query query = ParseQuery(raw_query);
        auto matched_documents = FindAllDocuments(query, predicate);

        sort(matched_documents.begin(), matched_documents.end(),
             [](const Document& lhs, const Document& rhs) {
                 if (abs(lhs.relevance - rhs.relevance) < 1e-6) {
                     return lhs.rating > rhs.rating;
                 } else {
                     return lhs.relevance > rhs.relevance;
                 }
             });
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

    int GetDocumentCount() const {
        return documents_.size();
    }

    tuple<vector<string>, DocumentStatus> MatchDocument(const string& raw_query,
                                                        int document_id) const {
        const Query query = ParseQuery(raw_query);
        vector<string> matched_words;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.push_back(word);
            }
        }
        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            if (word_to_document_freqs_.at(word).count(document_id)) {
                matched_words.clear();
                break;
            }
        }
        return {matched_words, documents_.at(document_id).status};
    }

private:
    struct DocumentData {
        int rating;
        DocumentStatus status;
    };

    set<string> stop_words_;
    map<string, map<int, double>> word_to_document_freqs_;
    map<int, DocumentData> documents_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    static int ComputeAverageRating(const vector<int>& ratings) {
        if (ratings.empty()) {
            return 0;
        }
        int rating_sum = 0;
        for (const int rating : ratings) {
            rating_sum += rating;
        }
        return rating_sum / static_cast<int>(ratings.size());
    }

    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };

    QueryWord ParseQueryWord(string text) const {
        bool is_minus = false;
        // Word shouldn't be empty
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return {text, is_minus, IsStopWord(text)};
    }

    struct Query {
        set<string> plus_words;
        set<string> minus_words;
    };

    Query ParseQuery(const string& text) const {
        Query query;
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus_words.insert(query_word.data);
                } else {
                    query.plus_words.insert(query_word.data);
                }
            }
        }
        return query;
    }

    // Existence required
    double ComputeWordInverseDocumentFreq(const string& word) const {
        return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
    }

    template <typename Predicate>
    vector<Document> FindAllDocuments(const Query& query, Predicate predicate) const {
        map<int, double> document_to_relevance;
        for (const string& word : query.plus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);
            for (const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
                const auto& document_data = documents_.at(document_id);
                if (predicate(document_id, document_data.status, document_data.rating)) {
                    document_to_relevance[document_id] += term_freq * inverse_document_freq;
                }
            }
        }

        for (const string& word : query.minus_words) {
            if (word_to_document_freqs_.count(word) == 0) {
                continue;
            }
            for (const auto &[document_id, _] : word_to_document_freqs_.at(word)) {
                document_to_relevance.erase(document_id);
            }
        }

        vector<Document> matched_documents;
        for (const auto &[document_id, relevance] : document_to_relevance) {
            matched_documents.push_back(
                {document_id, relevance, documents_.at(document_id).rating});
        }
        return matched_documents;
    }
};

/*
   Подставьте сюда вашу реализацию макросов
   ASSERT, ASSERT_EQUAL, ASSERT_EQUAL_HINT, ASSERT_HINT и RUN_TEST
*/
template <typename T>
ostream& operator<<(ostream& output, const vector<T>& items) {
    output << "["s;
    bool first_item = true;
    for (const T& item : items) {
        if (!first_item) {
            output << ", "s;
        }
        output << item;
        first_item = false;
    }
    output << "]"s;
    return output;
}
 
template <typename T>
ostream& operator<<(ostream& output, const set<T>& items) {
    output << "{"s;
    bool first_item = true;
    for (const T& item : items) {
        if (!first_item) {
            output << ", "s;
        }
        output << item;
        first_item = false;
    }
    output << "}"s;
    return output;
}
 
template <typename K, typename V>
ostream& operator<<(ostream& output, const map<K, V>& items) {
    output << "{"s;
    bool first_item = true;
    for (const auto& [key, value] : items) {
        if (!first_item) {
            output << ", "s;
        }
        output << key << ": "s << value;
        first_item = false;
    }
    output << "}"s;
    return output;
}
 
/*
   Подставьте сюда вашу реализацию макросов
   ASSERT, ASSERT_EQUAL, ASSERT_EQUAL_HINT, ASSERT_HINT и RUN_TEST
*/
 
void AssertImpl(bool value, const string& expr_str, const string& file, const string& func, unsigned line,
                const string& hint) {
    if (!value) {
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "Assert("s << expr_str << ") failed."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}
 
template <typename T, typename U>
void AssertEqualImpl(const T& t, const U& u, const string& t_str, const string& u_str, const string& file,
                     const string& func, unsigned line, const string& hint) {
    
    using CommonType = typename common_type<T, U>::type;

    if (static_cast<CommonType>(t) != static_cast<CommonType>(u)) {
        cerr << boolalpha;
        cerr << file << "("s << line << "): "s << func << ": "s;
        cerr << "ASSERT_EQUAL("s << t_str << ", "s << u_str << ") failed: "s;
        cerr << t << " != "s << u << "."s;
        if (!hint.empty()) {
            cerr << " Hint: "s << hint;
        }
        cerr << endl;
        abort();
    }
}
 
template <typename TestFunc>
void RunTestImpl(const TestFunc& func, const string& test_name) {
    func();
    cerr << test_name << " OK"s << endl;
}
 
#define ASSERT(a) AssertImpl((a), #a, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_HINT(a, hint) AssertImpl((a), #a, __FILE__, __FUNCTION__, __LINE__, (hint))
#define ASSERT_EQUAL(a, b) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, ""s)
#define ASSERT_EQUAL_HINT(a, b, hint) AssertEqualImpl((a), (b), #a, #b, __FILE__, __FUNCTION__, __LINE__, (hint))
#define RUN_TEST(func) RunTestImpl(func, #func)

// -------- Начало модульных тестов поисковой системы ----------

// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = {1, 2, 3};
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT_EQUAL(found_docs.size(), 1u);
        const Document& doc0 = found_docs[0];
        ASSERT_EQUAL(doc0.id, doc_id);
    }

    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT_HINT(server.FindTopDocuments("in"s).empty(),
                    "Stop words must be excluded from documents"s);
    }
}

/*
Разместите код остальных тестов здесь
*/

// Функция для проверки добавления документов в SearchServer
void TestAddingDocuments() {
    // Шаг 1: Создание экземпляра SearchServer
    SearchServer server;

    // Шаг 2: Определение идентификатора и содержания документа
    const int doc_id = 1;
    const string document_content = "cat sat on the mat";
    const vector<int> document_ratings = {5, 3, 4};

    server.AddDocument(doc_id, document_content, DocumentStatus::ACTUAL, document_ratings);

    // Шаг 4: Поиск документа с помощью запроса, соответствующего содержанию
    string search_query = "cat";
    const auto result = server.FindTopDocuments(search_query);

    // Шаг 5: Подтверждаем (assert), что результат содержит ровно один документ
    ASSERT_EQUAL(result.size(), 1u);

    // Шаг 6. Подтверждаем, что возвращенный идентификатор документа соответствует идентификатору добавленного документа
    const Document& doc0 = result[0];
    ASSERT_EQUAL(doc0.id, doc_id);
}

// Функция для проверки исключения стоп-слов в SearchServer
void TestExcludeStopWords() {
    // Шаг 1: Создание экземпляра SearchServer
    SearchServer server;

    // Шаг 2: Установка стоп-слов
    server.SetStopWords("on the");

    // Шаг 3: Добавить документ на сервер
    const int doc_id = 2;
    const string content = "dog runs on the street";
    const vector<int> ratings = {1, 2, 3};
    server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);

    // Шаг 4: Поиск с помощью стоп-слова
    const string search_query = "on";
    const auto result = server.FindTopDocuments(search_query);

    // Шаг 5: Утверждаем, что результат пустой (поскольку стоп-слова должны быть исключены)
    ASSERT_EQUAL(result.size(), 0);
}

// Функция для проверки минус-слов в SearchServer
void TestMinusWords() {
    // Шаг 1: Создание экземпляра SearchServer
    SearchServer server;

    // Шаг 2: Добавление документа на сервер
    const int document_id = 3;
    const string document_content = "car drives fast";
    const vector<int> document_ratings = {4, 5, 2};
    server.AddDocument(document_id, document_content, DocumentStatus::ACTUAL, document_ratings);

    // Шаг 3: Поиск с минус-словом
    string search_query = "car -fast";
    const auto result = server.FindTopDocuments(search_query);

    // Шаг 4: Утверждение, что результат пустой (так как минус-слово "fast" есть в документе)
    ASSERT_EQUAL(result.size(), 0);
}

// Функция для проверки расчета рейтинга в SearchServer
void TestRatingCalculation() {
    // Шаг 1: Создание экземпляра SearchServer
    SearchServer server;

    // Шаг 2: Добавление документа с определенными рейтингами
    const int document_id = 7;
    const string document_content = "slow turtle";
    const vector<int> document_ratings = {3, 3, 3};
    server.AddDocument(document_id, document_content, DocumentStatus::ACTUAL, document_ratings);

    // Шаг 3: Поиск и извлечение документа
    const string search_query = "turtle";
    const auto result = server.FindTopDocuments(search_query);

    // Шаг 4: Подтверждение того, что рейтинг документа рассчитан правильно
    int sum_ratings = 0;
    for (int rating : document_ratings) {
        sum_ratings += rating;
    }

    int expected_rating = sum_ratings / static_cast<int>(document_ratings.size());
    ASSERT_EQUAL(result[0].rating, expected_rating);
}

// Функция для проверки соответствия документов поисковому запросу
void TestMatchingDocumentsToQuery() {
    // Шаг 1: Создание экземпляра SearchServer
    SearchServer server;

    // Шаг 2: Добавление документа, содержащего как положительные, так и отрицательные слова
    const int document_id = 1;
    const string document_content = "happy cat and sad dog";
    const vector<int> document_ratings = {1, 2, 3};
    server.AddDocument(document_id, document_content, DocumentStatus::ACTUAL, document_ratings);

    // Шаг 3: Сопоставление запроса как с положительными, так и отрицательными словами
    {
        const string query_with_minus_word = "happy -sad";
        auto [matched_words, status] = server.MatchDocument(query_with_minus_word, document_id);

        // Шаг 4: Утверждение, что список совпавших слов пуст (потому что "sad" — это отрицательное слово, присутствующее в документе)
        ASSERT_EQUAL(matched_words.size(), 0u);
    }

    // Шаг 5: Сопоставление запроса только с положительными словами
    {
        const string query_positive_words = "happy cat";
        auto [matched_words, status] = server.MatchDocument(query_positive_words, document_id);

        // Шаг 6: Утверждаем, что список совпавших слов содержит "happy" и "cat"
        ASSERT_HINT(count(matched_words.begin(), matched_words.end(), "happy") > 0, "Expected 'happy' in matched words");
        ASSERT_HINT(count(matched_words.begin(), matched_words.end(), "cat") > 0, "Expected 'cat' in matched words");
        ASSERT_EQUAL(matched_words.size(), 2u);
    }
}

// Функция для проверки фильтрации результатов поиска с использованием предиката
void TestFilteringWithPredicate() {
    // Шаг 1: Создание экземпляра SearchServer
    SearchServer server;

    // Шаг 2: Добавление нескольких документов с различным содержимым и рейтингами
    server.AddDocument(1, "dog cat", DocumentStatus::ACTUAL, {3, 4, 5});
    server.AddDocument(2, "dog fox", DocumentStatus::ACTUAL, {1, 1, 1});
    server.AddDocument(3, "cat bird", DocumentStatus::BANNED, {5, 5, 5});

    // Шаг 3: Определение предиката, который допускает только документы со средним рейтингом выше 3
    auto predicate = [](int document_id, DocumentStatus status, int rating) {
        (void)document_id; // Suppress unused parameter warning
        (void)status;      // Suppress unused parameter warning

        // cout << "Document ID: " << document_id << ", Status: " << status << ", Rating: " << rating << endl;       
        return rating >= 3;
    };

    // Шаг 4: Поиск с предикатом
    const string search_query = "dog";
    const auto result = server.FindTopDocuments(search_query, predicate);

    // Шаг 5: Утверждаем, что возвращаются только документы с рейтингом больше 3
    ASSERT_EQUAL(result.size(), 1u); // Только один документ соответствует условию предиката
    ASSERT_EQUAL(result[0].id, 1); // Документ 1 должен быть единственным возвращаемым
}

// Функция для проверки поиска документов с указанным статусом
void TestSearchByStatus() {
    // Шаг 1: Создание экземпляра SearchServer
    SearchServer server;

    // Шаг 2: Добавление нескольких документов с разными статусами
    server.AddDocument(1, "happy cat", DocumentStatus::ACTUAL, {1, 2, 3});
    server.AddDocument(2, "sad dog", DocumentStatus::BANNED, {4, 5, 6});
    server.AddDocument(3, "angry bird", DocumentStatus::REMOVED, {7, 8, 9});

    // Шаг 3: Поиск документов с ACTUAL статусом
    const string search_query = "cat";
    const auto result = server.FindTopDocuments(search_query, DocumentStatus::ACTUAL);

    // Шаг 4: Утверждаем, что возвращаются только документы со статусом ACTUAL
    ASSERT_EQUAL(result.size(), 1u); // Только один документ со статусом ACTUAL
    ASSERT_EQUAL(result[0].id, 1); // Документ 1 должен быть единственным возвращаемым
}

// Функция для проверки правильности расчета релевантности
void TestCorrectRelevanceCalculation() {
    // Шаг 1: Создание экземпляра SearchServer
    SearchServer server;

    // Шаг 2: Добавление документов с определенным содержимым
    server.AddDocument(1, "quick brown fox", DocumentStatus::ACTUAL, {1, 2, 3});
    server.AddDocument(2, "lazy dog", DocumentStatus::ACTUAL, {4, 5, 6});

    // Шаг 3: Поиск с запросом, который соответствует нескольким документам
    const string search_query = "quick fox";
    const auto result = server.FindTopDocuments(search_query);

    // Шаг 4: Расчет ожидаемой релевантности вручную
    vector<double> expected_relevance;

    // Рассчитаем ожидаемую релевантность для документа 1
    double tf_quick_doc1 = 1.0 / 3; // "quick" встречается один раз в 3 словах
    double tf_fox_doc1 = 1.0 / 3; // "fox" встречается один раз в 3 словах
    double idf_quick = log(2.0 / 1); // "quick" встречается в 1 из 2 документов
    double idf_fox = log(2.0 / 1); // "fox" встречается в 1 из 2 документов
    
    expected_relevance.push_back((tf_quick_doc1 * idf_quick) + (tf_fox_doc1 * idf_fox));

    // Рассчитаем ожидаемую релевантность для документа 2
    double tf_quick_doc2 = 0; // "quick" не встречается в документе 2
    double tf_fox_doc2 = 0; // "fox" не встречается в документе 2
    
    expected_relevance.push_back((tf_quick_doc2 * idf_quick) + (tf_fox_doc2 * idf_fox)); 

    // Шаг 5: Утверждаем, что релевантность каждого документа в результате соответствует ожидаемой релевантности
    for (size_t i = 0; i < result.size() - 1; ++i) {
        ASSERT_EQUAL(result[i].relevance, expected_relevance[i]);
    }
}

// Функция для проверки правильного возвращения статуса документов в MatchDocument
void TestMatchDocumentReturnsCorrectStatus() {
    // Шаг 1: Создание экземпляра SearchServer
    SearchServer server;

    // Шаг 2: Добавление нескольких документов с разными статусами
    server.AddDocument(1, "happy cat", DocumentStatus::ACTUAL, {1, 2, 3});
    server.AddDocument(2, "sad dog", DocumentStatus::BANNED, {4, 5, 6});
    server.AddDocument(3, "angry bird", DocumentStatus::REMOVED, {7, 8, 9});

    // Шаг 3: Сопоставление запроса с каждым документом и проверка статуса

    // Test for document with ACTUAL status
    {
        const string query = "happy";
        auto [matched_words, status] = server.MatchDocument(query, 1);
        ASSERT_EQUAL(status, DocumentStatus::ACTUAL); // Должен вернуть ACTUAL
    }

    // Test for document with BANNED status
    {
        const string query = "sad";
        auto [matched_words, status] = server.MatchDocument(query, 2);
        ASSERT_EQUAL(status, DocumentStatus::BANNED); // Должен вернуть BANNED
    }

    // Test for document with REMOVED status
    {
        const string query = "angry";
        auto [matched_words, status] = server.MatchDocument(query, 3);
        ASSERT_EQUAL(status, DocumentStatus::REMOVED); // Должен вернуть REMOVED
    }
}

// Функция для проверки фильтрации документов по статусу с использованием лямбда-функции
void TestDocumentStatusFiltering() {
    // Шаг 1: Создание экземпляра SearchServer
    SearchServer server;

    // Шаг 2: Добавление нескольких документов с разными статусами
    server.AddDocument(1, "happy cat", DocumentStatus::ACTUAL, {1, 2, 3});
    server.AddDocument(2, "sad dog", DocumentStatus::BANNED, {4, 5, 6});
    server.AddDocument(3, "angry bird", DocumentStatus::REMOVED, {7, 8, 9});

    // Шаг 3: Определение предиката, который допускает только документы со статусом ACTUAL
    auto predicate = [](int document_id, DocumentStatus status, int rating) {
        (void)document_id;
        (void)rating;
        return status == DocumentStatus::ACTUAL;
    };

    // Шаг 4: Поиск документов с предикатом
    const string search_query = "cat";
    const auto result = server.FindTopDocuments(search_query, predicate);

    // Шаг 5: Утверждаем, что возвращаются только документы со статусом ACTUAL
    ASSERT_EQUAL(result.size(), 1u); // Должен быть только один документ
    ASSERT_EQUAL(result[0].id, 1);   // Документ 1 должен быть единственным возвращаемым

    // Дополнительный поиск документов с другим статусом
    const auto result_banned = server.FindTopDocuments(search_query, [](int, DocumentStatus status, int) {
        return status == DocumentStatus::BANNED;
    });

    ASSERT_EQUAL(result_banned.size(), 0u); // Должен быть пустой результат

    const auto result_removed = server.FindTopDocuments(search_query, [](int, DocumentStatus status, int) {
        return status == DocumentStatus::REMOVED;
    });

    ASSERT_EQUAL(result_removed.size(), 0u); // Должен быть пустой результат
}

// Функция для проверки правильности сортировки по релевантности
void TestRelevanceSorting() {
    // Шаг 1: Создание экземпляра SearchServer
    SearchServer server;

    // Шаг 2: Добавление документов с разным содержанием и рейтингами
    server.AddDocument(1, "quick brown fox", DocumentStatus::ACTUAL, {1, 2, 3});
    server.AddDocument(2, "lazy dog", DocumentStatus::ACTUAL, {4, 5, 6});
    server.AddDocument(3, "brown fox", DocumentStatus::ACTUAL, {7, 8, 9});
    server.AddDocument(4, "quick brown fox jumps", DocumentStatus::ACTUAL, {2, 3, 4});
    server.AddDocument(5, "quick dog", DocumentStatus::ACTUAL, {5, 6});

    // Шаг 3: Поиск с запросом, который соответствует нескольким документам
    const string search_query = "quick brown fox";
    const auto result = server.FindTopDocuments(search_query);

    // Шаг 4: Утверждаем, что документы отсортированы по релевантности
    ASSERT_EQUAL(result.size(), 4u); // Должны быть найдены 4 документа

    // Документ 3 должен быть первым (он содержит оба слова запроса)
    ASSERT_EQUAL(result[0].id, 3);

    // Документ 1 должен быть вторым (он содержит все слова запроса, но с меньшим рейтингом)
    ASSERT_EQUAL(result[1].id, 1);

    // Документ 4 должен быть третьим (он содержит все слова запроса, но с ещё меньшим рейтингом)
    ASSERT_EQUAL(result[2].id, 4);

    // Документ 5 должен быть четвертым (он содержит только одно слово запроса)
    ASSERT_EQUAL(result[3].id, 5);

    // Шаг 5: Утверждение, что результат отсортирован по релевантности (по убыванию)
    for (size_t i = 0; i < result.size() - 1; ++i) {
        ASSERT(result[i].relevance >= result[i + 1].relevance);
    }
}

// Функция TestSearchServer является точкой входа для запуска тестов
void TestSearchServer() {
    RUN_TEST(TestExcludeStopWordsFromAddedDocumentContent);
    // Не забудьте вызывать остальные тесты здесь
    RUN_TEST(TestAddingDocuments);
    RUN_TEST(TestExcludeStopWords);
    RUN_TEST(TestMinusWords);
    RUN_TEST(TestRatingCalculation);
    RUN_TEST(TestMatchingDocumentsToQuery);
    RUN_TEST(TestFilteringWithPredicate);
    RUN_TEST(TestSearchByStatus);
    RUN_TEST(TestCorrectRelevanceCalculation);
    RUN_TEST(TestMatchDocumentReturnsCorrectStatus);
    RUN_TEST(TestDocumentStatusFiltering);
    RUN_TEST(TestRelevanceSorting);
}

// --------- Окончание модульных тестов поисковой системы -----------

int main() {
    TestSearchServer();
    // Если вы видите эту строку, значит все тесты прошли успешно
    cout << "Search server testing finished"s << endl;
}