#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

using namespace std;

// Объявляем Sentence<Token> для произвольного типа Token
// синонимом vector<Token>.
// Благодаря этому в качестве возвращаемого значения
// функции можно указать не малопонятный вектор векторов,
// а вектор предложений — vector<Sentence<Token>>.
template <typename Token>
using Sentence = vector<Token>;

template <typename TokenForwardIt>
TokenForwardIt FindSentenceEnd(TokenForwardIt tokens_begin, TokenForwardIt tokens_end) {
    const TokenForwardIt before_sentence_end
        = adjacent_find(tokens_begin, tokens_end, [](const auto& left_token, const auto& right_token) {
              return left_token.IsEndSentencePunctuation() && !right_token.IsEndSentencePunctuation();
          });
    return before_sentence_end == tokens_end ? tokens_end : next(before_sentence_end);
}

// Класс Token имеет метод bool IsEndSentencePunctuation() const
template <typename Token>
vector<Sentence<Token>> SplitIntoSentences(vector<Token> tokens) {
    vector<Sentence<Token>> sentences;
    auto sentence_begin = tokens.begin();

    while (sentence_begin != tokens.end()) {
        auto sentence_end = FindSentenceEnd(sentence_begin, tokens.end());

        Sentence<Token> sentence;
        sentence.reserve(distance(sentence_begin, sentence_end)); // Оптимизация: резервируем память
        move(sentence_begin, sentence_end, back_inserter(sentence));

        sentences.push_back(move(sentence));
        sentence_begin = sentence_end;
    }
    return sentences;
}

struct TestToken {
    string data;
    bool is_end_sentence_punctuation = false;

    bool IsEndSentencePunctuation() const {
        return is_end_sentence_punctuation;
    }
    bool operator==(const TestToken& other) const {
        return data == other.data && is_end_sentence_punctuation == other.is_end_sentence_punctuation;
    }
};

ostream& operator<<(ostream& stream, const TestToken& token) {
    return stream << token.data;
}

// Тест содержит копирования объектов класса TestToken.
// Для проверки отсутствия копирований в функции SplitIntoSentences
// необходимо написать отдельный тест.
void TestSplitting() {
    assert(SplitIntoSentences(vector<TestToken>({{"Split"s}, {"into"s}, {"sentences"s}, {"!"s}}))
           == vector<Sentence<TestToken>>({{{"Split"s}, {"into"s}, {"sentences"s}, {"!"s}}}));

    assert(SplitIntoSentences(vector<TestToken>({{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}}))
           == vector<Sentence<TestToken>>({{{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}}}));

    assert(SplitIntoSentences(vector<TestToken>(
               {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}, {"!"s, true}, {"Without"s}, {"copies"s}, {"."s, true}}))
           == vector<Sentence<TestToken>>({
               {{"Split"s}, {"into"s}, {"sentences"s}, {"!"s, true}, {"!"s, true}},
               {{"Without"s}, {"copies"s}, {"."s, true}},
           }));
}

struct NoncopyableToken {
    string data;
    bool is_end_sentence_punctuation = false;

    NoncopyableToken(const NoncopyableToken&) = delete; // Запрещаем копирование
    NoncopyableToken& operator=(const NoncopyableToken&) = delete;

    NoncopyableToken(NoncopyableToken&&) = default; // Разрешаем перемещение
    NoncopyableToken& operator=(NoncopyableToken&&) = default;

    bool IsEndSentencePunctuation() const {
        return is_end_sentence_punctuation;
    }

    bool operator==(const NoncopyableToken& other) const {
        return data == other.data && is_end_sentence_punctuation == other.is_end_sentence_punctuation;
    }
};

void TestNoncopyableSplitting() {
    vector<NoncopyableToken> tokens;
    tokens.push_back(NoncopyableToken{"Hello"s});
    tokens.push_back(NoncopyableToken{"world"s});
    tokens.push_back(NoncopyableToken{"!"s, true});
    tokens.push_back(NoncopyableToken{"This"s});
    tokens.push_back(NoncopyableToken{"is"s});
    tokens.push_back(NoncopyableToken{"test"s});
    tokens.push_back(NoncopyableToken{"."s, true});

    auto sentences = SplitIntoSentences(move(tokens));

    vector<Sentence<NoncopyableToken>> expected;
    expected.push_back(Sentence<NoncopyableToken>());
    expected.back().push_back(NoncopyableToken{"Hello"s});
    expected.back().push_back(NoncopyableToken{"world"s});
    expected.back().push_back(NoncopyableToken{"!"s, true});

    expected.push_back(Sentence<NoncopyableToken>());
    expected.back().push_back(NoncopyableToken{"This"s});
    expected.back().push_back(NoncopyableToken{"is"s});
    expected.back().push_back(NoncopyableToken{"test"s});
    expected.back().push_back(NoncopyableToken{"."s, true});

    assert(sentences == expected);
}

int main() {
    TestSplitting();
    TestNoncopyableSplitting();
}