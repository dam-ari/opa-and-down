#include <pthread.h>
#include <semaphore.h>

#include <mutex>
#include <queue>
#include <string>

class UnboundedQueue {
   private:
    std::queue<string> queue;
    mutex qutex;
    sem_t full;

    UnboundedQueue(const UnboundedQueue &) = delete;
    UnboundedQueue(UnboundedQueue &&) = delete;
    UnboundedQueue &operator=(const UnboundedQueue &) = delete;
    UnboundedQueue &operator=(UnboundedQueue &&) = delete;

    void logSem(string value) {
        // TODO: DELETE AFTER DEBUGGING
        int x;
        sem_getvalue(&full, &x);
        cout << "LOG: >>>>>>>>>>>>>>>> enqueue: " << value
             << " ; full in: " << x << endl;
    }

   public:
    UnboundedQueue() { sem_init(&full, 0, 0); };
    ~UnboundedQueue(){};
    void enqueue(string value) {
        // std::lock_guard<std::mutex> guard(qutex);
        qutex.lock();
        queue.push(value);
        qutex.unlock();
        sem_post(&full);

        // logSem(value);  // TODO: delete
    }

    string dequeue() {
        sem_wait(&full);
        qutex.lock();
        string value = queue.front();
        queue.pop();
        qutex.unlock();

        // logSem(value);  // TODO: delete

        return value;
    };

    int getSize() {
        std::unique_lock<std::mutex> lock(qutex);
        int size = queue.size();
        lock.unlock();
        // std::unique_lock<std::mutex> unlock(mutex);
        return size;
    }

    string getFront() {
        std::unique_lock<std::mutex> lock(qutex);
        string value = queue.front();
        lock.unlock();

        // std::unique_lock<std::mutex> unlock(mutex);
        return value;
    };

    string getRear() {
        std::unique_lock<std::mutex> lock(qutex);
        string value = queue.back();
        lock.unlock();

        // std::unique_lock<std::mutex> unlock(mutex);
        return value;
    };

    bool isEmpty() {
        std::unique_lock<std::mutex> lock(qutex);
        bool empty = queue.empty();
        lock.unlock();

        // std::unique_lock<std::mutex> unlock(mutex);
        return empty;
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
        cout << "◀≡‖ " << endl;
    };
};
