#include "json_reader.h"
#include "transport_catalogue.h"
#include "json.h"
#include <iostream>

using namespace std;
using namespace transport_catalogue_app::core;

int main() {
    // Считываем JSON из входного потока
    json::Document doc = json::Load(cin);

    // Создаем транспортный справочник
    TransportCatalogue catalogue;

    // Обрабатываем запросы для создания базы данных транспортного справочника
    json_reader::ProcessBaseRequests(catalogue, doc.GetRoot().AsMap().at("base_requests"));

    // Обрабатываем запросы для получения статистики
    json::Node responses = json_reader::ProcessStatRequests(catalogue, doc.GetRoot().AsMap().at("stat_requests"));

    // Выводим результаты в виде JSON
    json::Print(json::Document{responses}, cout);

    return 0;
}