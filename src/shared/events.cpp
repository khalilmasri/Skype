#include "events.hpp"
#include "logger.hpp"

#include <string>
#include <vector>

Events* Events::get_instance(const Target t_target) {
    
    if ( nullptr == m_events_ ) {
        m_events_ = new Events(t_target);
    }

    std::cout << "Instance created for " << to_string(t_target)  << std::endl;
    return m_events_;
}

std::string Events::to_string(const Target t_target){
    
    std::vector<std::string> name = {"Server", "Client"};
    
    return name.at(static_cast<int>(t_target));
}