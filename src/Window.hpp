#pragma once
#include <gtkmm.h>

class Window
{
private:
    Gtk::ApplicationWindow* window = nullptr;
    Gtk::TextView* text_view = nullptr;
    Gtk::ScrolledWindow* scrolled = nullptr;
    // Document buffer (source of truth)
    Glib::RefPtr<Gtk::TextBuffer> buffer;

public:
    void OnActivate(const Glib::RefPtr<Gtk::Application>& app)
    {
        // window
        window = Gtk::make_managed<Gtk::ApplicationWindow>(app);
        window->set_title("NoteThat");
        window->set_default_size(800, 600);

        // Buffer (text lives here)
        buffer = Gtk::TextBuffer::create();

        // text view
        text_view = Gtk::make_managed<Gtk::TextView>();
        text_view->set_buffer(buffer);
        text_view->set_wrap_mode(Gtk::WrapMode::WORD);

        // Scroll
        scrolled = Gtk::make_managed<Gtk::ScrolledWindow>();
        scrolled->set_child(*text_view);

        window->set_child(*scrolled);
        window->present();
    }
};
