#include <blusher/application.h>

#include "application-impl.h"

#include <blusher/resource.h>

namespace bl {

Application *app = nullptr;

Application::Application(int argc, char *argv[])
{
    this->_impl = new ApplicationImpl(argc, argv);

    this->_resource = new Resource();

    bl::app = this;
}

Application::~Application()
{
    delete this->_impl;

    delete this->_resource;
}

//==================
// Public Methods
//==================

Resource* Application::resource()
{
    return this->_resource;
}

int Application::exec()
{
    return this->_impl->exec();
}

} // namespace bl
