#pragma once
#include <gtkmm.h>
#include "DocumentManager.hpp"
#include "EditorView.hpp"

class Window
{
private:
    // Widgets
    Gtk::ApplicationWindow* window = nullptr;
    // Components
    std::unique_ptr<EditorView> editorView;  // We need an editor to manage text
    std::unique_ptr<DocumentManager> docManager;  // We need to handle files
    
private:
    void SetupMenu()
    {
        auto menu = Gio::Menu::create();
        menu->append("Open", "app.open");
        menu->append("Save", "app.save");
        menu->append("Save As", "app.save_as");
        menu->append("Quit", "app.quit");
        
        auto header = Gtk::make_managed<Gtk::HeaderBar>();
        window->set_titlebar(*header);
        
        auto menu_button = Gtk::make_managed<Gtk::MenuButton>();
        menu_button->set_label("File");
        menu_button->set_menu_model(menu);
        header->pack_start(*menu_button);
    }
public:
    void OnActivate(const Glib::RefPtr<Gtk::Application>& app)
    {
        // ---- Window ----
        window = Gtk::make_managed<Gtk::ApplicationWindow>(app);
        window->set_title("NoteThat");
        window->set_default_size(800, 600);
        
        // ---- Editor View ----
        editorView = std::make_unique<EditorView>();
        
        // ---- Document Manager ----
        docManager = std::make_unique<DocumentManager>(editorView->GetBuffer());
        
        // Connect signal modification
        editorView->ConnectOnChanged([this]() {
            // If there is modifications on text
            docManager->SetModified(true);
        });
        
        // Callback to update title on save/open
        docManager->SetOnSaveCallback([this](const std::string& path) {
            // If file is saved or opened
            window->set_title("NoteThat - " + path);
        });
        
        // ---- Set editor as child ----
        window->set_child(*editorView->GetWidget());
        
        // ---- App actions ----
        app->add_action("save", [this]() { 
            docManager->Save(*window); 
        });
        
        app->add_action("save_as", [this]() { 
            docManager->SaveAs(*window); 
        });
        
        app->add_action("open", [this]() { 
            docManager->Open(*window); 
        });
        
        app->add_action("quit", [app]() { 
            app->quit(); 
        });
        
        // ---- Menu ----
        SetupMenu();
        
        // ---- Show window (LAST) ----
        window->present();
    }
};