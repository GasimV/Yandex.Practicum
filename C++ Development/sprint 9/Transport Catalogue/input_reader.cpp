#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <iostream>

namespace transport_catalogue_app::io {

using transport_catalogue_app::core::TransportCatalogue;
using transport_catalogue_app::detail::Coordinates;

/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        // Cyclic route
        return Split(route, '>');
    }

    // Non-cyclic route
    return Split(route, '-');
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto command = std::string(line.substr(0, line.find(' ')));
    auto id = std::string(Trim(line.substr(command.size() + 1, colon_pos - command.size() - 1)));
    auto description = std::string(Trim(line.substr(colon_pos + 1)));

    return {command, id, description};
}

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

std::vector<std::pair<std::string_view, int>> ParseDistances(std::string_view distances_part) {
    std::vector<std::pair<std::string_view, int>> distances;

    size_t pos = 0;
    while (pos < distances_part.size()) {
        auto m_pos = distances_part.find("m to ", pos);
        if (m_pos == distances_part.npos) {
            break;
        }

        int distance = std::stoi(std::string(distances_part.substr(pos, m_pos - pos)));

        auto stop_pos = m_pos + 5; // "m to " is 5 characters long
        auto stop_end = distances_part.find_first_of(',', stop_pos);
        if (stop_end == distances_part.npos) {
            stop_end = distances_part.size();
        }

        std::string_view stop_name = distances_part.substr(stop_pos, stop_end - stop_pos);
        distances.emplace_back(stop_name, distance);

        pos = stop_end + 1;
    }

    return distances;
}

void InputReader::ApplyCommands(TransportCatalogue& catalogue) const {
    // First pass: Add stops and their coordinates
    for (const auto& command : commands_) {
        if (command.command == "Stop") {
            auto colon_pos = command.description.find(',');
            auto coords = ParseCoordinates(command.description.substr(0, command.description.find(',', command.description.find(',') + 1)));
            
            catalogue.AddStop(command.id, coords);
        }
    }

    // Second pass: Add distances for stops
    for (const auto& command : commands_) {
        if (command.command == "Stop") {
            auto colon_pos = command.description.find(',');
            auto distances_part = command.description.substr(command.description.find(',', command.description.find(',') + 1) + 1);

            const auto* from_stop = catalogue.GetStopInfo(command.id);
            if (!from_stop) {
                continue;
            }

            auto distances = ParseDistances(distances_part);
            for (const auto& [to_stop_name, distance] : distances) {
                const auto* to_stop = catalogue.GetStopInfo(to_stop_name);
                if (to_stop) {
                    catalogue.SetDistance(from_stop, to_stop, distance);

                    if (catalogue.GetDistance(to_stop, from_stop) == 0) {
                        catalogue.SetDistance(to_stop, from_stop, distance);
                    }
                } else {
                    std::cout << "[ApplyCommands] Destination Stop not found: \"" 
                              << to_stop_name << "\"\n";
                }
            }
        }
    }

    // Third pass: Add bus routes
    for (const auto& command : commands_) {
        if (command.command == "Bus") {
            bool is_cyclic = command.description.find('>') != std::string::npos;
            auto stops = ParseRoute(command.description);
            catalogue.AddRoute(command.id, stops, is_cyclic);
        }
    }
}

} // namespace transport_catalogue_app::io