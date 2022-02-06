#include "application.h"

namespace bl {

Application::Application()
{
    this->_application = bl_application_new();
}

Application::~Application()
{
    bl_application_free(this->_application);
}

} // namespace bl
