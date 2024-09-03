#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server)
    : requests_(), 
      no_result_count_(0), 
      current_time_(0),  
      search_server_(search_server) {}

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    std::vector<Document> results = search_server_.FindTopDocuments(raw_query, document_predicate);
    bool is_empty = results.empty();
    ++current_time_;
    requests_.push_back({is_empty, current_time_});
    if (is_empty) {
        ++no_result_count_;
    }
    while (requests_.size() > 0 && requests_.front().time <= current_time_ - min_in_day_) {
        if (requests_.front().is_empty) {
            --no_result_count_;
        }
        requests_.pop_front();
    }
    return results;
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    return AddFindRequest(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
        (void)document_id;
        (void)rating;
        return document_status == status;
    });
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    return AddFindRequest(raw_query, DocumentStatus::ACTUAL);
}

int RequestQueue::GetNoResultRequests() const {
    return no_result_count_;
}