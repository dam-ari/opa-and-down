#include <pthread.h>
#include <semaphore.h>

#include <mutex>
#include <queue>
#include <string>

using namespace std;

class BoundedQueue {
   private:
    std::queue<string> queue;
    size_t capacity;
    mutex qutex;
    sem_t full;
    sem_t empty;

    BoundedQueue(const BoundedQueue &) = delete;
    BoundedQueue(BoundedQueue &&) = delete;
    BoundedQueue &operator=(const BoundedQueue &) = delete;
    BoundedQueue &operator=(BoundedQueue &&) = delete;

    void logSem(string value) {
        int x, y;
        sem_getvalue(&full, &x);
        sem_getvalue(&empty, &y);
        cout << "LOG: >>>>>>>>>>>>>>>> enqueue: " << value
             << " ; full in: " << x << " ; empty in: " << y << endl;
    }

   public:
    BoundedQueue(size_t size) : capacity(size) {
        sem_init(&full, 0, 0);
        sem_init(&empty, 0, size);
    };
    ~BoundedQueue(){};
    void enqueue(string value) {
        sem_wait(&empty);
        std::lock_guard<std::mutex> guard(qutex);
        queue.push(value);
        sem_post(&full);
    }

    string dequeue() {
        sem_wait(&full);
        qutex.lock();
        string value = queue.front();
        queue.pop();
        qutex.unlock();
        sem_post(&empty);

        return value;
    };

    int getSize() {
        std::unique_lock<std::mutex> lock(qutex);
        int size = queue.size();
        lock.unlock();
        return size;
    }

    string getFront() {
        std::unique_lock<std::mutex> lock(qutex);
        string value = queue.front();
        lock.unlock();

        return value;
    };

    string getRear() {
        std::unique_lock<std::mutex> lock(qutex);
        string value = queue.back();
        lock.unlock();
        return value;
    };

    bool isEmpty() {
        std::unique_lock<std::mutex> lock(qutex);
        bool empty = queue.empty();
        lock.unlock();

        return empty;
    };

    bool isFull() {
        std::unique_lock<std::mutex> lock(qutex);
        bool full = queue.size() == capacity;
        lock.unlock();
        return full;
    };

    void print() {
        std::unique_lock<std::mutex> lock(qutex);
        std::queue<string> copy(queue);
        lock.unlock();

        cout << "◀◀ ";

        while (!copy.empty()) {
            cout << copy.front() << " • ";
            copy.pop();
        }
        cout << "◀◀‖ " << endl;
    };
};
