#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <algorithm>

using namespace std;

enum class TaskStatus {
    NEW,
    IN_PROGRESS,
    TESTING,
    DONE
};

using TasksInfo = map<TaskStatus, int>;

class TeamTasks {
public:
    const TasksInfo& GetPersonTasksInfo(const string& person) const {
        return tasks_.at(person);
    }

    void AddNewTask(const string& person) {
        tasks_[person][TaskStatus::NEW]++;
    }

    tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person, int task_count) {
        if (tasks_.count(person) == 0) {
            return {TasksInfo(), TasksInfo()};
        }

        TasksInfo& person_tasks = tasks_[person];
        TasksInfo updated_tasks;
        TasksInfo untouched_tasks = person_tasks;

        for (TaskStatus status = TaskStatus::NEW; status != TaskStatus::DONE; status = Next(status)) {
            int tasks_to_move = min(task_count, person_tasks[status]);
            if (tasks_to_move == 0) continue;

            task_count -= tasks_to_move;

            updated_tasks[Next(status)] += tasks_to_move;
            untouched_tasks[status] -= tasks_to_move;

            person_tasks[status] -= tasks_to_move;
            person_tasks[Next(status)] += tasks_to_move;

            if (task_count == 0) {
                break;
            }
        }

        // Remove zero-count statuses from untouched_tasks
        for (auto it = untouched_tasks.begin(); it != untouched_tasks.end(); ) {
            if (it->second == 0) {
                it = untouched_tasks.erase(it);
            } else {
                ++it;
            }
        }

        // Remove the DONE status from untouched_tasks
        untouched_tasks.erase(TaskStatus::DONE);

        return {updated_tasks, untouched_tasks};
    }

private:
    static TaskStatus Next(TaskStatus status) {
        return static_cast<TaskStatus>(static_cast<int>(status) + 1);
    }

    map<string, TasksInfo> tasks_;
};

// Helper function for printing tasks info
void PrintTasksInfo(TasksInfo tasks_info) {
    cout << tasks_info[TaskStatus::NEW] << " new tasks, "
         << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress, "
         << tasks_info[TaskStatus::TESTING] << " tasks are being tested, "
         << tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
}

int main() {
    TeamTasks tasks;

    // Example 1
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

    // Example 2
    tasks.AddNewTask("Sasha");
    tasks.AddNewTask("Sasha");
    tasks.AddNewTask("Sasha");

    for (int i = 0; i != 4; ++i) {
        tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Sasha", 10);
        cout << "Updated Sasha's tasks: ";
        PrintTasksInfo(updated_tasks);
        cout << "Untouched Sasha's tasks: ";
        PrintTasksInfo(untouched_tasks);
    }

    return 0;
}