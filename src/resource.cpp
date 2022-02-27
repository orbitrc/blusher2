#include <blusher/resource.h>

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

} // namespace bl
