#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string>

using namespace std;

struct Nucleotide {
    char symbol;         // A, T, G, C
    size_t position;     // 0..3,3 млрд
    int chromosome_num;  // 1..46 (хотя пример использует 48)
    int gene_num;        // 0..25000
    bool is_marked;
    char service_info;   // любой символ
};

struct CompactNucleotide {
    // 32 бита под позицию
    uint32_t position;

    // 32 бита, разбитые на битовые поля
    uint32_t gene_num         : 15; // до 25 000
    uint32_t chromosome_num   : 6;  // до 64
    uint32_t symbol           : 2;  // 4 варианта (A,T,G,C)
    uint32_t is_marked        : 1;  // 1 бит
    uint32_t service_info     : 8;  // любой символ
};

// Утилита для преобразования символа нуклеотида (A,T,G,C) в 2-битное число
static uint32_t SymbolToBits(char symbol) {
    // Можно выбрать любую удобную для нас раскладку, лишь бы она была согласована с Decompress
    switch (symbol) {
        case 'A': return 0;
        case 'T': return 1;
        case 'G': return 2;
        case 'C': return 3;
        default:
            return 0;
    }
}

// Обратное преобразование 2-битного числа в символ нуклеотида
static char BitsToSymbol(uint32_t bits) {
    switch (bits) {
        case 0: return 'A';
        case 1: return 'T';
        case 2: return 'G';
        case 3: return 'C';
        default:
            // Теоретически сюда никогда не попадём
            return 'A';
    }
}

CompactNucleotide Compress(const Nucleotide& n) {
    CompactNucleotide cn;

    // Укладываем 32 бита позиции
    cn.position = static_cast<uint32_t>(n.position);

    // Теперь упаковываем битовые поля во вторые 32 бита
    cn.gene_num       = static_cast<uint32_t>(n.gene_num);
    cn.chromosome_num = static_cast<uint32_t>(n.chromosome_num);
    cn.symbol         = SymbolToBits(n.symbol);
    cn.is_marked      = n.is_marked ? 1 : 0;
    // Преобразуем любой символ к uint32_t
    cn.service_info   = static_cast<uint8_t>(n.service_info);

    return cn;
}

Nucleotide Decompress(const CompactNucleotide& cn) {
    Nucleotide n;

    // Восстанавливаем 32 бита позиции
    n.position = cn.position;

    // Распаковываем оставшиеся битовые поля
    n.gene_num        = static_cast<int>(cn.gene_num);
    n.chromosome_num  = static_cast<int>(cn.chromosome_num);
    n.symbol          = BitsToSymbol(cn.symbol);
    n.is_marked       = (cn.is_marked != 0);
    n.service_info    = static_cast<char>(cn.service_info);

    return n;
}

// Проверка на размер и отсутствие выравнивания с помощью pragma pack
static_assert(sizeof(CompactNucleotide) <= 8, "Your CompactNucleotide is not compact enough");
static_assert(alignof(CompactNucleotide) == 4, "Don't use '#pragma pack'!");

bool operator==(const Nucleotide& lhs, const Nucleotide& rhs) {
    return (lhs.symbol == rhs.symbol)
        && (lhs.position == rhs.position)
        && (lhs.chromosome_num == rhs.chromosome_num)
        && (lhs.gene_num == rhs.gene_num)
        && (lhs.is_marked == rhs.is_marked)
        && (lhs.service_info == rhs.service_info);
}

void TestSize() {
    assert(sizeof(CompactNucleotide) <= 8);
}

void TestCompressDecompress() {
    Nucleotide source;
    source.symbol = 'T';
    source.position = 1'000'000'000;
    source.chromosome_num = 48;  // хотя в задаче сказано 1..46, в тесте 48 :)
    source.gene_num = 1'000;
    source.is_marked = true;
    source.service_info = '!';

    CompactNucleotide compressedSource = Compress(source);
    Nucleotide decompressedSource = Decompress(compressedSource);

    assert(source == decompressedSource);
}

int main() {
    TestSize();
    TestCompressDecompress();
}