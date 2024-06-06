#include <pthread.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <thread>
#include <utility>
#include <vector>

#include "boundedQueue.h"
#include "omp.h"
#include "unboundedQueue.h"

#define PRODUCER "PRODUCER"
#define COEDITOR "Co-Editor"
#define COEDITOR_COUNT 3
#define DONE "done"
#define FINANCE_SEC 0
#define NEWS_SEC 1
#define TECH_SEC 2
#define DIVIDER " // "

using namespace std;

struct Producer {
    int id;
    int numOfProducts;
    int queueBound;
};

vector<Producer> producers;
int coEditorBound = 0;
vector<BoundedQueue *> proQueues;
vector<UnboundedQueue *> dispatcherQueues;
UnboundedQueue newsSec;
UnboundedQueue financeSec;
UnboundedQueue techSec;
// UnboundedQueue dispatcherQueues[3]
BoundedQueue *sharedQueue;
// ***********************
void testBq();
void testUbq();
void testConfig();
void testProQueues();
void testSharedQueue();
// ***********************

string getSectorNameById(int i) {
    switch (i) {
        case 0:
            return "Finance";
        case 1:
            return "News";
        case 2:
            return "Tech";
        default:
            return "";
    }
}

int getIdBySectorName(string s) {
    if (s == "Finance") {
        return 0;
    } else if (s == "News") {
        return 1;
    } else if (s == "Tech") {
        return 2;
    } else {
        return -1;
    }
}

void logConfErr(exception e) {
    cout << "‼️Error: " << e.what() << endl;
    cout << "❣️Config file is not as excepted" << endl;
}

void getConfig(string filename) {
    ifstream in(filename);
    string line;
    int numOfEditors = 0;
    Producer pro;
    bool done = false;
    while (!done) {
        getline(in, line);
        if (line.find(PRODUCER) != string::npos) {
            pro.id = ++numOfEditors;
            getline(in, line);
            try {
                pro.numOfProducts = stoi(line);
            } catch (exception &e) {
                logConfErr(e);
                pro.numOfProducts = 0;
            }
            getline(in, line);
            try {
                pro.queueBound = stoi(line.substr(line.find("=") + 1).c_str());
            } catch (exception &e) {
                logConfErr(e);
                pro.queueBound = 0;
            }
            producers.push_back(pro);
        }
        if (line.find(COEDITOR) != string::npos) {
            try {
                coEditorBound = stoi(line.substr(line.find("=") + 1).c_str());
            } catch (exception &e) {
                logConfErr(e);
                coEditorBound = 0;
            }
            done = true;
        }
        if (line == "" && in.eof()) {
            done = true;
        }
    }
}

void initBoundedQueues() {
    // producers queues
    for (size_t i = 0; i < producers.size(); i++) {
        proQueues.push_back(new BoundedQueue(producers[i].queueBound));
    };
    for (size_t i = 0; i < COEDITOR_COUNT; i++) {
        dispatcherQueues.push_back(new UnboundedQueue());
    }
    // co-editors queue
    sharedQueue = new BoundedQueue(coEditorBound);
}

// ***********************
void producer(size_t i) {
    BoundedQueue *bq = proQueues.at(i);

    size_t prod = producers[i].numOfProducts;
    // cout << " num of prod: " << prod << endl;
    for (size_t j = 1; j <= prod; j++) {
        // really produce something
        string value = "producer " + to_string(producers[i].id) + DIVIDER +
                       getSectorNameById(j % 3) + DIVIDER + to_string(j);
        bq->enqueue(value);

        usleep(5000);
    }
    bq->enqueue(DONE);

    // cout << "📎producer DONE!📎left for edit:";
    // bq->print();
}

void dispatcher() {
    size_t countDone = 0;
    bool done = false;

    while (!done) {
        // circular loop for pro queues
        for (size_t i = 0; i < proQueues.size(); i++) {
            BoundedQueue *bq = proQueues.at(i);
            if (!bq->isEmpty()) {
                string value = bq->dequeue();
                // if value equal done, then countDone++
                if (value == DONE) {
                    countDone++;
                    proQueues.erase(proQueues.begin() + i);

                } else {
                    // dispatch value to the right co-editors queue by the
                    // value
                    int sp = value.find(DIVIDER) + 4;
                    string sector =
                        value.substr(sp, value.find(DIVIDER, sp) - sp);
                    cout << "📦 dispatching to sector " << sector;
                    int sectorId = getIdBySectorName(sector);
                    cout << "; sector id " << sectorId << endl;
                    dispatcherQueues[sectorId]->enqueue(value);
                    usleep(5000);
                }

            } else {
            }

            // circulate vector
            if (proQueues.size() && i == proQueues.size() - 1) {
                i = 0;
            }

            // if countDone == producers.size() then done
            if (countDone == producers.size()) {
                done = true;
                cout << "📦📦📦📦📦 Dispatcher is DONE 📦📦📦📦📦\n" << endl;
                for_each(dispatcherQueues.begin(), dispatcherQueues.end(),
                         [](UnboundedQueue *q) { q->enqueue(DONE); });
            }
            // if all pro queues are empty, then done
            if (proQueues.size() == 0) {
                done = true;
            }
        }
    }
}

void coEditor(size_t i) {
    // cout << "I am co-editor " << i << endl;
    UnboundedQueue *ubq = dispatcherQueues[i];
    bool done = false;
    while (!done) {
        string value = ubq->dequeue();
        if (value == DONE) {
            done = true;
        } else {
            sharedQueue->enqueue(value + " ✒️edited ");
        }
        usleep(5000);
    }
}

int main(int argc, char **argv) {
    if (argc == 1) {
        cout << "❌Error: No config file provided" << endl;
        return 0;
    }

    string config = argv[1];
    getConfig(config);
    // testConfig();
    initBoundedQueues();
    cout << ">>>>> 🗞️Start produce!🗞️" << endl;
    mutex m;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < producers.size(); i++) {
        threads.emplace_back([&] { producer(i); });
        usleep(10000);
    }

    std::thread dt(dispatcher);
    std::thread fiCt(coEditor, FINANCE_SEC);
    std::thread nsCt(coEditor, NEWS_SEC);
    std::thread tcCt(coEditor, TECH_SEC);

    std::for_each(threads.begin(), threads.end(),
                  [](std::thread &x) { x.join(); });
    dt.join();
    fiCt.join();
    nsCt.join();
    tcCt.join();

    sharedQueue->print();
    cout << "DONE!" << endl;
    // testProQueues();
    // testSharedQueue();
    return 0;
}

//==========================================================

void testSharedQueue() {
    // cout << "shared size: " << sharedQueue->getSize() << endl;
    sharedQueue->print();
}

void testProQueues() {
    // cout << "pro size: " << proQueues.size() << endl;
    for (size_t i = 0; i < proQueues.size(); i++) {
        proQueues[i]->print();
    }
}

void testConfig() {
    cout << "📜 CONFIG: 📜" << endl;
    for (size_t i = 0; i < producers.size(); i++) {
        cout << "       Producer " << producers[i].id << ": "
             << producers[i].numOfProducts << " products, "
             << producers[i].queueBound << " queue bound" << endl;
    }
    cout << "       co-editor: " << coEditorBound << endl;
    cout << "===================================================" << endl;
}

void testUbq() {
    UnboundedQueue bq;
    bq.enqueue("Hello");
    bq.enqueue("World");
    bq.enqueue("!");
    cout << "LOG>>>> size is: " << bq.getSize() << " queue should not be empty "
         << bq.isEmpty() << " and neither should be full " << endl;
    bq.enqueue("1");
    bq.enqueue("2");
    cout << "LOG>>>> size is: " << bq.getSize() << " queue should not be empty "
         << bq.isEmpty() << " and neither should be full " << endl;
    cout << "LOG>>>> size is: " << bq.getSize() << " queue should not be empty "
         << bq.isEmpty() << " and neither should be full " << endl;
    cout << " now out: " << bq.dequeue();
    cout << " now in front " << bq.getFront() << endl;
    cout << " now in rear " << bq.getRear() << endl;

    bq.enqueue("3");
    cout << " now in rear " << bq.getRear() << endl;
    bq.enqueue("3");
    bq.enqueue("3");

    bq.print();
}

void testBq() {
    BoundedQueue bq(5);
    bq.enqueue("Hello");
    bq.enqueue("World");
    bq.enqueue("!");
    cout << "LOG>>>> size is: " << bq.getSize() << " queue should not be empty "
         << bq.isEmpty() << " and neither should be full " << bq.isFull()
         << endl;
    bq.enqueue("1");
    bq.enqueue("2");
    cout << "LOG>>>> size is: " << bq.getSize() << " queue should not be empty "
         << bq.isEmpty() << " but should be full " << bq.isFull() << endl;
    cout << "LOG>>>> size is: " << bq.getSize() << " queue should not be empty "
         << bq.isEmpty() << " but should be full " << bq.isFull() << endl;
    cout << " now out: " << bq.dequeue();
    cout << " now in front " << bq.getFront() << endl;
    cout << " now in rear " << bq.getRear() << endl;

    bq.enqueue("3");
    cout << " now in rear " << bq.getRear() << endl;

    bq.print();
}