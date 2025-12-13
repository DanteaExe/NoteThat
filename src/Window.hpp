#pragma once
#include <gtkmm.h>

class Window
{
private:
    Gtk::ApplicationWindow* window = nullptr;
    Gtk::TextView* text_view = nullptr;
    Gtk::ScrolledWindow* scrolled = nullptr;

public:
    void OnActivate(const Glib::RefPtr<Gtk::Application>& app)
    {
        // Crear ventana
        window = Gtk::make_managed<Gtk::ApplicationWindow>(app);
        window->set_title("NoteThat");
        window->set_default_size(800, 600);

        // Text view
        text_view = Gtk::make_managed<Gtk::TextView>();
        text_view->set_wrap_mode(Gtk::WrapMode::WORD);

        // Scroll
        scrolled = Gtk::make_managed<Gtk::ScrolledWindow>();
        scrolled->set_child(*text_view);

        window->set_child(*scrolled);
        window->present();
    }
};
