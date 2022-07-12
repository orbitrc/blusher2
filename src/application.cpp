#include <blusher/application.h>

#include "application-impl.h"

#include <blusher/resource.h>

#ifdef __cplusplus
extern "C" {
#endif

static void register_resources()
{
    #include "../resources/resources.h"
    BL_REGISTER_RESOURCE()
}

#ifdef __cplusplus
}
#endif

namespace bl {

Application *app = nullptr;

Application::Application(int argc, char *argv[])
{
    this->_impl = new ApplicationImpl(argc, argv);

    this->_resource = new Resource();

    this->_event_dispatcher =
        std::make_shared<EventDispatcher>(this);

    bl::app = this;

    // Load resources.
    register_resources();
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
    this->_event_dispatcher->start_loop();

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
