#pragma once

#include <string>
#include <iostream>
#include <optional>

class Person;

// Наблюдатель за состоянием человека.
class PersonObserver {
public:
    // Этот метод вызывается, когда меняется состояние удовлетворённости человека
    virtual void OnSatisfactionChanged(Person& person, int old_value, int new_value) {
        // Реализация метода базового класса ничего не делает
        (void)person;
        (void)old_value;
        (void)new_value;
    }

protected:
    // Класс PersonObserver не предназначен для удаления напрямую
    virtual ~PersonObserver() = default;
};

/*
    Человек.
    При изменении уровня удовлетворённости уведомляет
    связанного с ним наблюдателя
*/
class Person {
public:
    Person(const std::string& name, int age) 
        : dance_count_(0), name_(name), age_(age), satisfaction_(100), observer_(nullptr) {}

    virtual ~Person() = default;
    
    int GetSatisfaction() const {
        return satisfaction_;
    }

    const std::string& GetName() const {
        return name_;
    }

    int GetAge() const {
        return age_;
    }

    // «Привязывает» наблюдателя к человеку. Привязанный наблюдатель
    // уведомляется об изменении уровня удовлетворённости человека
    // Новый наблюдатель заменяет собой ранее привязанного
    // Если передать nullptr в качестве наблюдателя, это эквивалентно отсутствию наблюдателя
    void SetObserver(PersonObserver* observer) {
        observer_ = observer;
    }

    // Увеличивает на 1 количество походов на танцы
    // Увеличивает удовлетворённость на 1
    virtual void Dance() {
        int old_satisfaction_ = satisfaction_;
        satisfaction_ += 1;
        ++dance_count_;
        NotifyObserver(old_satisfaction_, satisfaction_);
    }

    int GetDanceCount() const {
        return dance_count_;
    }

    // Прожить день. Реализация в базовом классе ничего не делает
    virtual void LiveADay() {
        // Подклассы могут переопределить этот метод
    }

protected:
    void ChangeSatisfaction(int delta) {
        int old_satisfaction_ = satisfaction_;
        satisfaction_ += delta;
        NotifyObserver(old_satisfaction_, satisfaction_);
    }

    int dance_count_;

private:
    void NotifyObserver(int old_value, int new_value) {
        if (observer_) {
            observer_->OnSatisfactionChanged(*this, old_value, new_value);
        }
    }
    
    std::string name_;
    int age_;
    int satisfaction_ = 100;
    PersonObserver* observer_ = nullptr;
};

// Рабочий.
// День рабочего проходит за работой
class Worker : public Person {
private:
    int work_done_;

public:
    Worker(const std::string& name, int age) : Person(name, age), work_done_(0) {}

    // Рабочий старше 30 лет и младше 40 за танец получает 2 единицы удовлетворённости вместо 1

    // День рабочего проходит за работой

    // Увеличивает счётчик сделанной работы на 1, уменьшает удовлетворённость на 5
    void Work() {
        ChangeSatisfaction(-5);
        ++work_done_;
    }

    // Возвращает значение счётчика сделанной работы
    int GetWorkDone() const {
        return work_done_;
    }

    void Dance() override {
        // int old_satisfaction = GetSatisfaction();
        int satisfaction_increase = (GetAge() > 30 && GetAge() < 40) ? 2 : 1;
        ChangeSatisfaction(satisfaction_increase);
        ++dance_count_;
    }

    void LiveADay() override { Work(); }
};

// Студент.
// День студента проходит за учёбой
class Student : public Person {
private:
    int knowledge_level_;

public:
    Student(const std::string& name, int age) : Person(name, age), knowledge_level_(0) {}

    // День студента проходит за учёбой

    // Учёба увеличивает уровень знаний на 1, уменьшает уровень удовлетворённости на 3
    void Study() {
        ChangeSatisfaction(-3);
        ++knowledge_level_;
    }

    // Возвращает уровень знаний
    int GetKnowledgeLevel() const {
        return knowledge_level_;
    }

    void LiveADay() override { Study(); }
};