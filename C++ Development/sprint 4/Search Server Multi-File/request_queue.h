#pragma once

#include <vector>
#include <deque>
#include "document.h"
#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);
    int GetNoResultRequests() const;

private:
    struct QueryResult {
        bool is_empty;
        int time;
    };
    std::deque<QueryResult> requests_;
    static const int min_in_day_ = 1440;
    int no_result_count_;
    int current_time_;
    const SearchServer& search_server_;
};