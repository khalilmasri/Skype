#ifndef EVENTS_H
#define EVENTS_H

#include <iostream>

class Events {

public:
    enum Target{ SERVER, CLIENT};

    Events(Events& other) = delete;
    void operator=(const Events &) = delete;

    static Events* get_instance(const Target t_target);

protected:
    static Events* m_events_;
    Target m_target;

    static std::string to_string (const Target t_target);

    Events(const Target t_target): m_target(t_target) {};
};



#endif