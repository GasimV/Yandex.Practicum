#include <iostream>
#include <memory>
#include <string>
#include <utility>

using namespace std;

class AnyStorageBase {
public:
    virtual ~AnyStorageBase() = default;
    virtual void Print(ostream& out) const = 0;
};

template <typename T>
class AnyStorage : public AnyStorageBase {
public:
    template <typename U>
    explicit AnyStorage(U&& value) : data_(std::forward<U>(value)) {}

    void Print(ostream& out) const override {
        out << data_;
    }

private:
    T data_;
};

class Any {
public:
    template <typename S>
    Any(S&& value) {
        using Initial = remove_reference_t<S>;
        ptr_ = make_unique<AnyStorage<Initial>>(std::forward<S>(value));
    }

    void Print(ostream& out) const {
        if (ptr_) {
            ptr_->Print(out);
        }
    }

private:
    unique_ptr<AnyStorageBase> ptr_;
};

class Dumper {
public:
    Dumper() {
        std::cout << "construct"sv << std::endl;
    }
    ~Dumper() {
        std::cout << "destruct"sv << std::endl;
    }
    Dumper(const Dumper&) {
        std::cout << "copy"sv << std::endl;
    }
    Dumper(Dumper&&) {
        std::cout << "move"sv << std::endl;
    }
    Dumper& operator=(const Dumper&) {
        std::cout << "= copy"sv << std::endl;
        return *this;
    }
    Dumper& operator=(Dumper&&) {
        std::cout << "= move"sv << std::endl;
        return *this;
    }
};

ostream& operator<<(ostream& out, const Any& arg) {
    arg.Print(out);
    return out;
}

ostream& operator<<(ostream& out, const Dumper&) {
    return out;
}

int main() {
    Any any_int(42);
    Any any_string("abc"s);

    // операция вывода Any в поток определена чуть выше в примере
    cout << any_int << endl << any_string << endl;

    Any any_dumper_temp{Dumper{}};

    Dumper auto_dumper;
    Any any_dumper_auto(auto_dumper);
}