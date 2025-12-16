#pragma once
#include <gtkmm.h>
#include <functional>

class EditorView
{
private:
    Glib::RefPtr<Gtk::TextBuffer> buffer; //memory text
    Gtk::TextView* textView; //where we see text
    Gtk::ScrolledWindow* scrolled; //just for scroll...there is no magic here

public:
    EditorView()
    {
        // Create buffer
        buffer = Gtk::TextBuffer::create();
        
        // CreateTextView
        textView = Gtk::make_managed<Gtk::TextView>();
        textView->set_buffer(buffer);
        textView->set_wrap_mode(Gtk::WrapMode::WORD);
        
        // Create ScrolledWindow
        scrolled = Gtk::make_managed<Gtk::ScrolledWindow>();
        scrolled->set_child(*textView);
    }

    // Getters
    Gtk::Widget* GetWidget() { return scrolled; }
    Glib::RefPtr<Gtk::TextBuffer> GetBuffer() { return buffer; }
    
    // change signal conection
    void ConnectOnChanged(std::function<void()> callback)
    {
        buffer->signal_changed().connect(callback);
    }
};