#include <blusher/application.h>

#include "application-impl.h"

#include <blusher/resource.h>

namespace bl {

Application *app = nullptr;

Application::Application(int argc, char *argv[])
{
    this->_impl = new ApplicationImpl(argc, argv);

    this->_resource = new Resource();

    this->_event_dispatcher =
        std::make_shared<EventDispatcher>(EventDispatcher(this));

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

std::shared_ptr<EventDispatcher> Application::event_dispatcher()
{
    return this->_event_dispatcher;
}

int Application::exec()
{
    return this->_impl->exec();
}

std::shared_ptr<WlCompositor> Application::wl_compositor()
{
    return this->_impl->compositor();
}

std::shared_ptr<XdgWmBase> Application::xdg_wm_base()
{
    return this->_impl->xdgWmBase();
}

} // namespace bl
