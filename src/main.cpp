#include "Window.hpp"

int main()
{
    auto app = Gtk::Application::create("com.notethat.app");

    Window win;

    app->signal_activate().connect(
        sigc::bind(sigc::mem_fun(win, &Window::OnActivate), app)
    );

    return app->run();
}
