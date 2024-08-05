#include <iostream>
#include <vector>
#include <map>
#include <string>

void updateTasks(std::map<std::string, int>& tasks, int task_count) {
    std::vector<std::string> statuses = {"NEW", "IN_PROGRESS", "TESTING", "DONE"};

    for (const auto& status : statuses) {
        if (task_count == 0) break;
        int to_update = std::min(tasks[status], task_count);
        tasks[status] -= to_update;
        task_count -= to_update;
        
        // Move tasks to the next status if possible
        if (status != "DONE") {
            tasks[statuses[&status - &statuses[0] + 1]] += to_update;
        }
    }
}

void printTaskDistribution(const std::map<std::string, int>& tasks) {
    for (const auto& task : tasks) {
        std::cout << task.first << ": " << task.second << " tasks\n";
    }
}

int main() {
    // Initial task distribution
    std::map<std::string, int> tasks = {
        {"NEW", 3},
        {"IN_PROGRESS", 2},
        {"TESTING", 4},
        {"DONE", 1}
    };

    // Number of tasks to update
    int task_count = 4;

    std::cout << "Initial Task Distribution:\n";
    printTaskDistribution(tasks);

    // Update tasks
    updateTasks(tasks, task_count);

    std::cout << "\nTask Distribution After Updates:\n";
    printTaskDistribution(tasks);

    return 0;
}