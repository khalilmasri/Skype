#ifndef DATA_H
#define DATA_H

#include <cstddef>

class Data{

  public:
    enum type {Audio, Video, Text};

    virtual type get_type() const = 0;
    virtual std::byte* get_data() const = 0;
    virtual bool empty() const = 0;
    virtual std::size_t size() const = 0;


    virtual ~Data() {};
};

#endif // !DATA_H
