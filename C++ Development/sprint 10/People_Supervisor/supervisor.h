#pragma once

#include "people.h"

/*
    Надзиратель за уровнем удовлетворённости.
    Способен наблюдать за состоянием человека
    Если уровень удовлетворённости человека опустится ниже минимального уровня, Надзиратель
    побуждает человека танцевать до тех пор, пока уровень уровень удовлетворённости
    не станет больше или равен максимальному значению
*/
class SatisfactionSupervisor : public PersonObserver {
private:
    int min_satisfaction_;
    int max_satisfaction_;

public:
    // Конструктор принимает значение нижнего и верхнего уровня удовлетворённости
    SatisfactionSupervisor(int min_satisfaction, int max_satisfaction) 
        : min_satisfaction_(min_satisfaction), max_satisfaction_(max_satisfaction) {}

    void OnSatisfactionChanged(Person& person, int old_value, int new_value) override {
        // Реагировать только на снижение удовлетворенности
        if (new_value < min_satisfaction_ && new_value < old_value) {
            int required_dance_count = (max_satisfaction_ - person.GetSatisfaction());
            if (required_dance_count > 0) {
                for (int i = 0; i < required_dance_count; ++i) {
                    person.Dance();
                    if (person.GetSatisfaction() >= max_satisfaction_) {
                        break;
                    }
                }
            }
        }
    }
};