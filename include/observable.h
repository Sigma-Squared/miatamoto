#ifndef OBSERVABLE_H
#define OBSERVABLE_H

template <typename T>
class Observable
{
    T value;
    std::vector<void (*)(T)> listeners;

public:
    Observable(T initialValue) : value(initialValue) {}

    T get() const { return value; }

    void set(T newValue)
    {
        if (value != newValue)
        {
            value = newValue;
            for (auto &listener : listeners)
            {
                listener(value);
            }
        }
    }

    void addListener(void (*observer)(T))
    {
        listeners.push_back(observer);
    }
};
#endif
