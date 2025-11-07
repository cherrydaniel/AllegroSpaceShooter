#ifndef _GAME_OBSERVER_H_
#define _GAME_OBSERVER_H_

#include <unordered_set>

template <typename EV>
class Observer
{
public:
    Observer(){}
    virtual ~Observer(){}
    virtual void onEvent(EV event, void* data) = 0;
};

template <typename EV>
class Subject
{
private:
    std::unordered_set<Observer<EV>*> observers;
public:
    Subject() = default;
    ~Subject(){}
    void addObserver(Observer<EV>* obs){ observers.insert(obs); }
    bool removeObserver(Observer<EV>* obs)
    {
        auto it = observers.find(obs);
        if (it==observers.end())
            return false;
        observers.erase(it);
        return true;
    }
    void notify(EV event, void* data)
    {
        for (auto it = observers.begin(); it!=observers.end(); it++)
            (*it)->onEvent(event, data);
    }
    void notify(EV event)
    {
        notify(event, nullptr);
    }
    void notify(EV event, void* data, void(*finalizer)(void*))
    {
        notify(event, data);
        finalizer(data);
    }
};

#endif
