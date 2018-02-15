#ifndef SHMEM_H
#define SHMEM_H

#include <QDebug>
#include <QSharedMemory>

class shmem
{
private:
    QSharedMemory mem;

public:
    shmem();

    ~shmem() {
        if (mem.isAttached()) {
            mem.detach();
        }
    }

    int size() {
        return mem.size();
    }

    char operator[] (int index) {
        if (mem.isAttached() && index < mem.size()) {
            char temp;
            char* p_temp = (char*) mem.data();
            if (mem.lock()) // это приложение начинает работу с общей памятью
            {
                temp = p_temp[index];
                mem.unlock(); // заканчиваем работу с общей памятью
                return temp;
            }
        }
    }

    bool isMemOpen() {
        return mem.isAttached();
    }

    bool open(QString key) {
        mem.setKey(key);
        return mem.attach();
    }

    int create(QString key, int size_) { // создаем общую память
        mem.setKey(key);
        if(!mem.create(size_)) {
            if (!mem.attach()) {
                qDebug() << "memory with key: " << key << "isn't create!";
                return -1;
            }else {
                mem.detach();
                mem.setKey(key);
                if(!mem.create(size_)) {
                    qDebug() << "memory with key: " << key << "already exist";
                    return -2;
                }else {
                    this->clear();
                    return 0;
                }
            }
        } else {
            this->clear();
            return 0;
        }
    }

    bool clear() { // очищаем общую память
        if (mem.isAttached()) {
            char* temp = (char*) mem.data();
            if (mem.lock()) // это приложение начинает работу с общей памятью
            {
                for(int i = 0; i < mem.size(); i++) {
                    temp[i] = 0;
                }
                mem.unlock(); // заканчиваем работу с общей памятью
            }
        }
        return false;
    }

    char* data() {
        if (mem.isAttached()) {
            char* buffer = new char[mem.size()];
            char* temp = (char*) mem.data();
            if (mem.lock()) // это приложение начинает работу с общей памятью
            {
                for(int i = 0; i < mem.size(); i++) {
                    buffer[i] = temp[i];
                }
                mem.unlock(); // заканчиваем работу с общей памятью
                return buffer;
            }
        }
    }

    bool write(char* data, int size_, int pos = 0) {
        if (mem.isAttached() && mem.size() <= (size_ - pos)) {
            char* temp = (char*) mem.data();
            if (mem.lock()) // это приложение начинает работу с общей памятью
            {
                for(int i = 0; i < size_; i++) {
                    temp[i + pos] = data[i];
                }
                mem.unlock(); // заканчиваем работу с общей памятью
                return true;
            }
        }
        return false;
    }
};

#endif // SHMEM_H
