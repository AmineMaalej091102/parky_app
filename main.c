#include <gtk/gtk.h>
#include <webkit/webkit.h>

int main(int argc, char *argv[]) {
	GtkWidget *window;
	GtkWidget *web_view;
	
	gtk_init(&argc, &argv);
	
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Google Maps in GTK2");
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
	
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	
	web_view=webkit_web_view_new();
	
	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), "https://www.google.com/maps");
	
	gtk_container_add(GTK_CONTAINER(window), web_view);
	
	gtk_widget_show_all(window);
	
	gtk_main();
	
	return 0;
}
