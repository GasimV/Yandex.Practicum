#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>
 
using namespace std;
 
// Перечислимый тип для статуса задачи
enum class TaskStatus {
    NEW,          // новая
    IN_PROGRESS,  // в разработке
    TESTING,      // на тестировании
    DONE          // завершена
};
 
// Объявляем тип-синоним для map<TaskStatus, int>,
// позволяющего хранить количество задач каждого статуса
using TasksInfo = map<TaskStatus, int>;
 
class TeamTasks {
public:
    // Получить статистику по статусам задач конкретного разработчика
    const TasksInfo& GetPersonTasksInfo(const string& person) const {
        return person_tasks_.at(person);
    }
 
    // Добавить новую задачу (в статусе NEW) для конкретного разработчика
    void AddNewTask(const string& person) {
        ++person_tasks_[person][TaskStatus::NEW];
    }
 
    // Обновить статусы по данному количеству задач конкретного разработчика
    tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person, int task_count) {
        TasksInfo updated_tasks, untouched_tasks;
        TasksInfo& tasks = person_tasks_[person];
 
        for (TaskStatus status = TaskStatus::NEW; status != TaskStatus::DONE; status = Next(status)) {
            const int can_update = tasks[status] - updated_tasks[status];
            const int to_update = min(task_count, can_update);
            
            updated_tasks[Next(status)] = to_update;
            tasks[Next(status)] += to_update;
            
            tasks[status] -= to_update;
            task_count -= to_update;
            untouched_tasks[status] = can_update - to_update;
        }

        // Гарантируем, что нетронутые задачи не имеют статуса DONE
        untouched_tasks.erase(TaskStatus::DONE);
 
        return make_tuple(updated_tasks, untouched_tasks);
    }
 
private:
    map<string, TasksInfo> person_tasks_;
 
    static TaskStatus Next(TaskStatus task_status) {
        return static_cast<TaskStatus>(static_cast<int>(task_status) + 1);
    }
};
 
// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь
void PrintTasksInfo(TasksInfo tasks_info) {
    cout << tasks_info[TaskStatus::NEW] << " new tasks, "
         << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress, "
         << tasks_info[TaskStatus::TESTING] << " tasks are being tested, "
         << tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
}
 
int main() {
    TeamTasks tasks;
    tasks.AddNewTask("Ilia");
    for (int i = 0; i < 3; ++i) {
        tasks.AddNewTask("Ivan");
    }
    cout << "Ilia's tasks: ";
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"));
    cout << "Ivan's tasks: ";
    PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));
 
    TasksInfo updated_tasks, untouched_tasks;
 
    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan", 2);
    cout << "Updated Ivan's tasks: ";
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: ";
    PrintTasksInfo(untouched_tasks);
 
    tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan", 2);
    cout << "Updated Ivan's tasks: ";
    PrintTasksInfo(updated_tasks);
    cout << "Untouched Ivan's tasks: ";
    PrintTasksInfo(untouched_tasks);
}