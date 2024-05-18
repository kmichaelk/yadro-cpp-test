#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <list>

template<typename Subject>
class observer {
public:
    using subject_type = Subject;

    virtual ~observer() = default;

    virtual void accept(Subject &subject) = 0;
};

template<typename Subject>
class observable {
private:
    std::list<observer<Subject>*> observers;
protected:
    void emit(Subject &subject);
public:
    using subject_type = Subject;

    virtual ~observable() = default;

    void subscribe(observer<Subject>* observer);
    void unsubscribe(observer<Subject>* observer);
};

template<typename Subject>
void observable<Subject>::emit(Subject &subject) {
    for (const auto &observer: observers) {
        observer->accept(subject);
    }
}

template<typename Subject>
void observable<Subject>::subscribe(observer<Subject> *observer) {
    observers.push_back(observer);
}

template<typename Subject>
void observable<Subject>::unsubscribe(observer<Subject> *observer) {
    observers.remove(observer);
}

#endif // OBSERVABLE_H
