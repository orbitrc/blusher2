#ifndef _BL_RESOURCE_H
#define _BL_RESOURCE_H

#include <stdint.h>

#include <vector>

namespace bl {

class Resource
{
public:
    class Data {
    public:
        Data(const char *path, const unsigned char *data, uint64_t size);

        inline const char* path() const
        {
            return this->_path;
        }

        inline const unsigned char* data() const
        {
            return this->_data;
        }

        inline uint64_t size() const
        {
            return this->_size;
        }

    private:
        const char *_path;
        const unsigned char *_data;
        uint64_t _size;
    };
public:
    Resource();

    void add_data(Data *data);

    const Data* data(const char *path) const;

private:
    std::vector<Data*> _data;
};

} // namespace bl

#endif /* _BL_RESOURCE_H */
