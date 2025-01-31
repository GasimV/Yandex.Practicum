#pragma once

#include <memory>
#include <type_traits>

struct IndexSegment {
    size_t left;
    size_t right;

    size_t length() const {
        return right - left;
    }

    bool empty() const {
        return length() == 0;
    }

    bool Contains(IndexSegment other) const {
        return left <= other.left && other.right <= right;
    }

    static bool AreIntersected(IndexSegment lhs, IndexSegment rhs) {
        return !(lhs.right <= rhs.left || rhs.right <= lhs.left);
    }
};

//
// Шаблонное дерево отрезков, параметризованное:
//   Data — тип данных (у нас это DayState),
//   BulkOperation — тип "ленивой" операции (BulkLinearUpdater).
//
template <typename Data, typename BulkOperation>
class SummingSegmentTree {
public:
    explicit SummingSegmentTree(size_t size)
        : root_(Build({0, size})) {
    }

    // Вернуть суммарный Data на заданном отрезке
    Data ComputeSum(IndexSegment segment) const {
        return TraverseWithQuery(root_, segment, ComputeSumVisitor{});
    }

    // Добавить (или применить) операцию на заданном отрезке
    void AddBulkOperation(IndexSegment segment, const BulkOperation& operation) {
        TraverseWithQuery(root_, segment, AddBulkOperationVisitor{operation});
    }

private:
    struct Node;
    using NodeHolder = std::unique_ptr<Node>;

    struct Node {
        NodeHolder left;
        NodeHolder right;
        IndexSegment segment;
        Data data;
        BulkOperation postponed_bulk_operation;
    };

    static NodeHolder Build(IndexSegment segment) {
        if (segment.empty()) {
            return nullptr;
        }
        if (segment.length() == 1) {
            return std::make_unique<Node>(Node{
                {},
                {},
                segment,
                Data{},
                BulkOperation{}
            });
        } else {
            const size_t mid = segment.left + segment.length() / 2;
            return std::make_unique<Node>(Node{
                Build({segment.left, mid}),
                Build({mid, segment.right}),
                segment,
                Data{},
                BulkOperation{}
            });
        }
    }

    template <typename Visitor>
    static typename Visitor::ResultType
    TraverseWithQuery(const NodeHolder& node, IndexSegment query_segment, Visitor visitor) {
        if (!node || !IndexSegment::AreIntersected(node->segment, query_segment)) {
            return visitor.ProcessEmpty(node);
        }
        // Убедимся, что уже отложенные операции протолкнуты ниже
        PropagateBulkOperation(node);

        if (query_segment.Contains(node->segment)) {
            return visitor.ProcessFull(node);
        } else {
            if constexpr (std::is_void_v<typename Visitor::ResultType>) {
                TraverseWithQuery(node->left, query_segment, visitor);
                TraverseWithQuery(node->right, query_segment, visitor);
                return visitor.ProcessPartial(node, query_segment);
            } else {
                auto left_res = TraverseWithQuery(node->left, query_segment, visitor);
                auto right_res = TraverseWithQuery(node->right, query_segment, visitor);
                return visitor.ProcessPartial(node, query_segment, left_res, right_res);
            }
        }
    }

    // "Протолкнуть" отложенную операцию из node к его детям
    static void PropagateBulkOperation(const NodeHolder& node) {
        for (auto* child_ptr : {node->left.get(), node->right.get()}) {
            if (child_ptr) {
                child_ptr->postponed_bulk_operation.CombineWith(node->postponed_bulk_operation);
                child_ptr->data = node->postponed_bulk_operation.Collapse(child_ptr->data, child_ptr->segment);
            }
        }
        // Сброс отложенной операции в текущем узле
        node->postponed_bulk_operation = BulkOperation();
    }

private:
    //
    // Вспомогательные классы для "обхода" узлов дерева
    //
    class ComputeSumVisitor {
    public:
        using ResultType = Data;

        Data ProcessEmpty(const NodeHolder&) const {
            return {};
        }

        Data ProcessFull(const NodeHolder& node) const {
            return node->data;
        }

        Data ProcessPartial(const NodeHolder&, IndexSegment,
                            const Data& left_res, const Data& right_res) const {
            return left_res + right_res;
        }
    };

    class AddBulkOperationVisitor {
    public:
        using ResultType = void;

        explicit AddBulkOperationVisitor(const BulkOperation& operation)
            : operation_(operation) {
        }

        void ProcessEmpty(const NodeHolder&) const {
            // ничего не делаем
        }

        void ProcessFull(const NodeHolder& node) const {
            // "Накатить" операцию прямо на узел
            node->postponed_bulk_operation.CombineWith(operation_);
            node->data = operation_.Collapse(node->data, node->segment);
        }

        void ProcessPartial(const NodeHolder& node, IndexSegment) const {
            // Пересобрать data = сумма детей, т.к. часть узла обновилась
            node->data = (node->left ? node->left->data : Data{})
                       + (node->right ? node->right->data : Data{});
        }

    private:
        const BulkOperation& operation_;
    };

    NodeHolder root_;
};
