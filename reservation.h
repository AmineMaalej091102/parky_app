#ifndef RESERVATION_H_INCLUDED
#define RESERVATION_H_INCLUDED
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct 
{
    char id[10];
    char idCit[10];
    char idPar[10];
    char service[30];
    char date[11];
    char duration[10];
    char status[10];
} salma_reservation;

typedef struct
{
	char free_slot[5];
	char id[10];
} salma_parking;

void set_label_message(GtkWidget *label_review, const char *message);

void save_reservation_to_file(salma_reservation *r);

void on_calendar_date_selected(GtkCalendar *calendar, salma_reservation *new_reservation);

bool get_reservation_by_id(const char *filename, const char *reservation_id, salma_reservation *result);

void refresh_salma_reservation_treeview(GtkWidget *salma_reservation_treeview);
void refresh_salma_parking_treeview(GtkWidget *salma_parking_treeview);

void setup_salma_reservation_treeview(GtkWidget *salma_reservation_treeview);
void setup_salma_parking_treeview(GtkWidget *salma_parking_treeview);

void edit_reservation(const char *filename, const char *reservation_id, salma_reservation *updated_reservation);

int delete_reservation(const char *filename, const char *reservation_id, salma_reservation *deleted_reservation);

void reservation_add_clicked(GtkButton *reservation_add, gpointer user_data);
void reservation_edit_clicked(GtkButton *reservation_edit, gpointer user_data);
void reservation_delete_clicked(GtkButton *reservation_delete, gpointer user_data);
void reservation_refresh_clicked(GtkButton *reservation_refresh, gpointer user_data);

#endif
