#include <blusher/resource.h>

#include <string.h>

namespace bl {

//===================
// Resource::Data

Resource::Data::Data(const char *path, const unsigned char *data, uint64_t size)
{
    this->_path = path;
    this->_data = data;
    this->_size = size;
}


//==================
// Resource

Resource::Resource()
{
}

void Resource::add_data(Data *data)
{
    this->_data.push_back(data);
}

const Resource::Data* Resource::data(const char *path) const
{
    Resource::Data *data = nullptr;

    auto end = this->_data.end();
    for (auto it = this->_data.begin(); it != end; ++it) {
        Resource::Data *d = *it;
        if (strcmp(d->path(), path) == 0) {
            data = d;
            break;
        }
    }

    return data;
}

} // namespace bl
