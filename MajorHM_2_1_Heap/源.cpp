#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <ctime>

using namespace std;

struct HeapNode {
    string name;
    int id;
    int priority;
    time_t timestamp;
    bool deleted;

    HeapNode(string n, int i, int p, time_t t)
        : name(n), id(i), priority(p), timestamp(t), deleted(false) {}
};

struct HeapCompare {
    bool operator()(const HeapNode& a, const HeapNode& b) {
        if (a.priority != b.priority)
            return a.priority < b.priority;
        return a.timestamp > b.timestamp;
    }
};

class HeapPriorityQueue {
private:
    vector<HeapNode> heap;
    unordered_map<int, int> idToIndex;
    HeapCompare comparator;

    void siftUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (comparator(heap[idx], heap[parent]))
                break;
            swap(heap[idx], heap[parent]);
            idToIndex[heap[idx].id] = idx;
            idToIndex[heap[parent].id] = parent;
            idx = parent;
        }
    }
    void siftDown(int idx) {
        int n = heap.size();
        while (true) {
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;
            int largest = idx;

            if (left < n && !comparator(heap[left], heap[largest]))
                largest = left;
            if (right < n && !comparator(heap[right], heap[largest]))
                largest = right;

            if (largest == idx)
                break;

            swap(heap[idx], heap[largest]);
            idToIndex[heap[idx].id] = idx;
            idToIndex[heap[largest].id] = largest;
            idx = largest;
        }
    }

    int findTop() {
        int n = heap.size();
        for (int i = 0; i < n; ++i) {
            if (!heap[i].deleted)
                return i;
        }
        return -1; 
    }

public:
    void enqueue(string name, int id, int priority) {
        time_t now = time(nullptr);
        heap.emplace_back(name, id, priority, now);
        int idx = heap.size() - 1;
        idToIndex[id] = idx;
        siftUp(idx);
    }
    pair<string, int> dequeue() {
        int topIdx = findTop();
        if (topIdx == -1)
            return { "", -1 };

        HeapNode& top = heap[topIdx];
        top.deleted = true;
        idToIndex.erase(top.id);

        int lastIdx = heap.size() - 1;
        swap(heap[topIdx], heap[lastIdx]);
        idToIndex[heap[topIdx].id] = topIdx;
        heap.pop_back();
        siftDown(topIdx);

        return { top.name, top.id };
    }

    bool updatePriority(int id, int newPriority) {
        if (idToIndex.find(id) == idToIndex.end())
            return false;

        int idx = idToIndex[id];
        HeapNode& node = heap[idx];
        node.deleted = true;
        idToIndex.erase(id);
        enqueue(node.name, id, newPriority);
        return true;
    }
    bool remove(int id) {
        if (idToIndex.find(id) == idToIndex.end())
            return false; 

        int idx = idToIndex[id];
        heap[idx].deleted = true;
        idToIndex.erase(id);
        return true;
    }

    void print() {
        vector<HeapNode> validNodes;
        for (const auto& node : heap) {
            if (!node.deleted)
                validNodes.push_back(node);
        }

        sort(validNodes.begin(), validNodes.end(), [](const HeapNode& a, const HeapNode& b) {
            if (a.priority != b.priority)
                return a.priority > b.priority;
            return a.timestamp < b.timestamp;
            });

        cout << "Heap-based Priority Queue:" << endl;
        for (const auto& node : validNodes) {
            cout << "Name: " << node.name << ", ID: " << node.id << ", Priority: " << node.priority << endl;
        }
    }
};
//下为ai生成的测试函数
int main() {
    // 初始化优先级队列
    HeapPriorityQueue pq;
    cout << "=== 测试1：初始入队操作 ===" << endl;
    // 入队5个元素（优先级不同，测试优先级排序）
    pq.enqueue("Alice", 101, 3);
    pq.enqueue("Bob", 102, 5);  // 最高优先级
    pq.enqueue("Charlie", 103, 2);
    pq.enqueue("David", 104, 5);  // 与Bob优先级相同，测试时间戳排序
    pq.enqueue("Eve", 105, 4);
    pq.print();
    cout << endl;

    // 测试2：出队操作（验证最高优先级元素先出队）
    cout << "=== 测试2：出队操作 ===" << endl;
    auto top1 = pq.dequeue();
    cout << "出队元素：Name: " << top1.first << ", ID: " << top1.second << endl;  // 预期Bob（ID102，优先级5，更早入队）
    auto top2 = pq.dequeue();
    cout << "出队元素：Name: " << top2.first << ", ID: " << top2.second << endl;  // 预期David（ID104，优先级5，晚于Bob）
    pq.print();  // 剩余元素：Eve(4)、Alice(3)、Charlie(2)
    cout << endl;

    // 测试3：更新优先级操作
    cout << "=== 测试3：更新优先级 ===" << endl;
    // 情况1：更新存在的元素（Alice ID101，优先级3→6）
    bool updateOk1 = pq.updatePriority(101, 6);
    cout << "更新ID101的优先级为6：" << (updateOk1 ? "成功" : "失败") << endl;
    // 情况2：更新不存在的元素（ID200）
    bool updateOk2 = pq.updatePriority(200, 4);
    cout << "更新ID200的优先级为4：" << (updateOk2 ? "成功" : "失败") << endl;
    pq.print();  // 预期Alice优先级6成为最高，排在最前
    cout << endl;

    // 测试4：删除元素操作
    cout << "=== 测试4：删除元素 ===" << endl;
    // 情况1：删除存在的元素（Eve ID105）
    bool removeOk1 = pq.remove(105);
    cout << "删除ID105：" << (removeOk1 ? "成功" : "失败") << endl;
    // 情况2：删除不存在的元素（ID300）
    bool removeOk2 = pq.remove(300);
    cout << "删除ID300：" << (removeOk2 ? "成功" : "失败") << endl;
    pq.print();  // 剩余元素：Alice(6)、Charlie(2)
    cout << endl;

    // 测试5：继续入队+出队，验证动态调整
    cout << "=== 测试5：动态入队+出队 ===" << endl;
    // 新增两个元素
    pq.enqueue("Frank", 106, 5);
    pq.enqueue("Grace", 107, 3);
    pq.print();  // 当前队列：Alice(6)、Frank(5)、Grace(3)、Charlie(2)
    // 连续出队3次
    auto top3 = pq.dequeue();
    cout << "出队元素：Name: " << top3.first << ", ID: " << top3.second << endl;  // Alice(6)
    auto top4 = pq.dequeue();
    cout << "出队元素：Name: " << top4.first << ", ID: " << top4.second << endl;  // Frank(5)
    auto top5 = pq.dequeue();
    cout << "出队元素：Name: " << top5.first << ", ID: " << top5.second << endl;  // Grace(3)
    pq.print();  // 剩余：Charlie(2)
    cout << endl;

    // 测试6：队列为空时出队
    cout << "=== 测试6：空队列出队 ===" << endl;
    pq.dequeue();  // 出队最后一个元素Charlie
    auto emptyTop = pq.dequeue();
    cout << "空队列出队结果：Name: " << emptyTop.first << ", ID: " << emptyTop.second << endl;  // 预期("", -1)
    pq.print();  // 打印空队列
    cout << endl;

    // 测试7：边界情况（相同优先级连续入队）
    cout << "=== 测试7：相同优先级批量入队 ===" << endl;
    pq.enqueue("Harry", 108, 4);
    pq.enqueue("Ivy", 109, 4);
    pq.enqueue("Jack", 110, 4);
    pq.print();  // 预期按入队顺序排列：Harry→Ivy→Jack
    cout << endl;

    return 0;
}