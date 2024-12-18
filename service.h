#ifndef SERVICE_H_INCLUDED
#define SERVICE_H_INCLUDED
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct 
{
    char id[10];
    char des[30];
    char price[10];
    char time[10];
	char type[10];
	int staff;
	char bonus[10];
    char category[10];
} iheb_service;

typedef struct
{
	char id[10];
	int status;
} iheb_reservation;

void save_service_to_file(iheb_service *s);

bool get_service_by_id(const char *filename, const char *service_id, iheb_service *result);

int is_service_assigned(const char *filename, const char *service_id);
void update_service_status(const char *service_id, int status);

void refresh_iheb_service_treeview(GtkWidget *iheb_service_treeview);
void refresh_iheb_reservation_treeview(GtkWidget *iheb_reservation_treeview);

void setup_iheb_service_treeview(GtkWidget *iheb_service_treeview);
void setup_iheb_reservation_treeview(GtkWidget *iheb_reservation_treeview);

void edit_service(const char *filename, const char *service_id, iheb_service *updated_service);

int delete_service(const char *filename, const char *service_id, iheb_service *deleted_service);

void service_add_clicked(GtkButton *service_add, gpointer user_data);
void service_edit_clicked(GtkButton *service_edit, gpointer user_data);
void service_delete_clicked(GtkButton *service_delete, gpointer user_data);
void service_refresh_clicked(GtkButton *service_refresh, gpointer user_data);

#endif
