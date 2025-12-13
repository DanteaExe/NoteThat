#pragma once
#include <gtkmm.h>

class Window
{
public:
    void OnActivate(const Glib::RefPtr<Gtk::Application>& app)
    {
        auto window = Gtk::make_managed<Gtk::ApplicationWindow>(app);
        window->set_title("NoteThat");
        window->set_default_size(800, 600);

        auto text_view = Gtk::make_managed<Gtk::TextView>();
        text_view->set_wrap_mode(Gtk::WrapMode::WORD);

        window->set_child(*text_view);
        window->present();
    }
};
