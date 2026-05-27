#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <climits>
using namespace std;

struct ListNode {
    string name;
    int id;
    int priority;
    ListNode* prev;
    ListNode* next;

    ListNode(string n, int i, int p)
        : name(n), id(i), priority(p), prev(nullptr), next(nullptr) {}
};

class LinkedListQueue {
public:
    ListNode* head;
    ListNode* tail;
    int size;

    LinkedListQueue() : head(nullptr), tail(nullptr), size(0) {}

    ~LinkedListQueue() {
        ListNode* curr = head;
        while (curr) {
            ListNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }

    void enqueue(string name, int id, int priority) {
        ListNode* newNode = new ListNode(name, id, priority);
        if (size == 0) {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        size++;
    }


    ListNode* dequeue() {
        if (size == 0) {
            return nullptr;
        }
        ListNode* front = head;
        head = head->next;
        if (head) {
            head->prev = nullptr;
        }
        else {
            tail = nullptr;
        }
        size--;
        front->next = nullptr;
        return front;
    }

    ListNode* removeAndGet(int id) { 
        ListNode* curr = head;
        while (curr) {
            if (curr->id == id) {
                if (curr->prev) {
                    curr->prev->next = curr->next;
                }
                else {
                    head = curr->next;
                }
                if (curr->next) {
                    curr->next->prev = curr->prev;
                }
                else {
                    tail = curr->prev;
                }
                curr->prev = curr->next = nullptr;
                size--;
                return curr;
            }
            curr = curr->next;
        }
        return nullptr;
    }

    bool remove(int id) {
        ListNode* node = removeAndGet(id);
        if (node) {
            delete node;
            return true;
        }
        return false;
    }

    bool isEmpty() const {
        return size == 0;
    }
};

class MultiListPriorityQueue {
private:
    unordered_map<int, LinkedListQueue*> priorityQueues;
    unordered_map<int, ListNode*> idToNode;
    int maxPriority;
    int minPriority;
    void updateMaxMinPriority() {
        if (priorityQueues.empty()) {
            maxPriority = -1;
            minPriority = INT_MAX;
            return;
        }
        maxPriority = INT_MIN;
        minPriority = INT_MAX;
        for (auto& pair : priorityQueues) {
            int p = pair.first;
            maxPriority = max(maxPriority, p);
            minPriority = min(minPriority, p);
        }
    }

public:
    MultiListPriorityQueue() : maxPriority(-1), minPriority(INT_MAX) {}

    ~MultiListPriorityQueue() {
        for (auto& pair : priorityQueues) {
            delete pair.second;
        }
    }
    void enqueue(string name, int id, int priority) {
        if (idToNode.count(id)) {
            cout << "错误：患者ID " << id << " 已存在！" << endl;
            return;
        }
        if (!priorityQueues.count(priority)) {
            priorityQueues[priority] = new LinkedListQueue();
        }
        priorityQueues[priority]->enqueue(name, id, priority);
        ListNode* newNode = priorityQueues[priority]->tail;
        idToNode[id] = newNode;
        maxPriority = max(maxPriority, priority);
        minPriority = min(minPriority, priority);
        cout << "成功入队：" << name << " (ID: " << id << ", 优先级: " << priority << ")" << endl;
    }

    pair<string, int> dequeue() {
        for (int p = maxPriority; p >= minPriority; p--) {
            if (priorityQueues.count(p) && !priorityQueues[p]->isEmpty()) {
                ListNode* front = priorityQueues[p]->dequeue();
                idToNode.erase(front->id);
                string name = front->name;
                int id = front->id;
                cout << "出队患者：" << name << " (ID: " << id << ", 优先级: " << p << ")" << endl;
                delete front;
                if (priorityQueues[p]->isEmpty()) {
                    delete priorityQueues[p];
                    priorityQueues.erase(p);
                    updateMaxMinPriority();
                }
                return { name, id };
            }
        }
        cout << "队列为空，无患者可出队！" << endl;
        return { "", -1 }; 
    }

    void remove(int id) {
        if (!idToNode.count(id)) {
            cout << "错误：患者ID " << id << " 不存在！" << endl;
            return;
        }
        ListNode* node = idToNode[id];
        int p = node->priority;
        string name = node->name;
        priorityQueues[p]->remove(id);
        idToNode.erase(id);
        cout << "患者离开：" << name << " (ID: " << id << ")" << endl;

        if (priorityQueues[p]->isEmpty()) {
            delete priorityQueues[p];
            priorityQueues.erase(p);
            updateMaxMinPriority();
        }
    }

    void switchQueue(int id, int newPriority) {
        if (!idToNode.count(id)) {
            cout << "错误：患者ID " << id << " 不存在！" << endl;
            return;
        }
        ListNode* node = idToNode[id];
        int oldPriority = node->priority;
        if (oldPriority == newPriority) {
            cout << "提示：患者ID " << id << " 优先级未变化！" << endl;
            return;
        }
        string name = node->name;
        ListNode* removedNode = priorityQueues[oldPriority]->removeAndGet(id);
        idToNode.erase(id);
        if (priorityQueues[oldPriority]->isEmpty()) {
            delete priorityQueues[oldPriority];
            priorityQueues.erase(oldPriority);
            updateMaxMinPriority();
        }
        enqueue(name, id, newPriority);
        delete removedNode; 
        cout << "更新优先级：" << name << " (ID: " << id << ") 从 " << oldPriority << " 到 " << newPriority << endl;
    }

    void print() {
        cout << "\n=== 多重链表队列当前状态 ===" << endl;
        if (priorityQueues.empty()) {
            cout << "队列为空！" << endl;
            cout << "===========================" << endl;
            return;
        }

        vector<int> priorities;
        for (auto& pair : priorityQueues) {
            priorities.push_back(pair.first);
        }
        sort(priorities.rbegin(), priorities.rend());

        int count = 1;
        for (int p : priorities) {
            LinkedListQueue* q = priorityQueues[p];
            ListNode* curr = q->head;
            while (curr) {
                cout << count++ << ". " << curr->name << " (ID: " << curr->id
                    << ", 优先级: " << curr->priority << ")" << endl;
                curr = curr->next;
            }
        }
        cout << "===========================" << endl;
    }
};

// 多重链表队列的测试主函数
int main() {
    MultiListPriorityQueue listPQ;

    cout << "===== 多重链表队列优先级队列测试 =====" << endl << endl;

    // 测试入队
    listPQ.enqueue("Tom", 123, 4);
    listPQ.enqueue("Jerry", 456, 2);
    listPQ.enqueue("Alice", 789, 5);
    listPQ.enqueue("Bob", 111, 3);
    listPQ.print();

    // 测试出队（优先级最高的Alice）
    listPQ.dequeue();
    listPQ.print();

    // 测试更新优先级
    listPQ.switchQueue(123, 5);  // Tom优先级提升到5
    listPQ.print();

    // 测试患者离开
    listPQ.remove(456);  // Jerry离开
    listPQ.print();

    // 测试连续出队
    listPQ.dequeue();
    listPQ.dequeue();
    listPQ.dequeue();  // 此时队列为空
    listPQ.print();

    return 0;
}