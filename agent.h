#ifndef AGENT_H_INCLUDED
#define AGENT_H_INCLUDED
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct 
{
    char id[10];
    char cin[10];
    char tel[10];
    char mail[30];
    char gender[10];
    char seas[10];
    int  age;
    char exp[20];
} fedi_agent;

typedef struct
{
	char date[20];
	char  id[10];
} fedi_reservation;

void set_label_message(GtkWidget *label_review, const char *message);

bool get_agent_by_id(const char *filename, const char *agent_id, fedi_agent *result);

void refresh_fedi_agent_treeview(GtkWidget *fedi_agent_treeview);
void refresh_fedi_reservation_treeview(GtkWidget *fedi_reservation_treeview);

void setup_fedi_agent_treeview(GtkWidget *fedi_agent_treeview);
void setup_fedi_reservation_treeview(GtkWidget *fedi_reservation_treeview);

void save_agent_to_file(fedi_agent *a);

void edit_agent(const char *filename, const char *agent_id, fedi_agent *updated_agent);

int delete_agent(const char *filename, const char *agent_id, fedi_agent *deleted_agent);

void agent_add_clicked(GtkButton *agent_add, gpointer user_data);
void agent_edit_clicked(GtkButton *agent_edit, gpointer user_data);
void agent_delete_clicked(GtkButton *agent_delete, gpointer user_data);
void agent_refresh_clicked(GtkButton *agent_refresh, gpointer user_data);

#endif
