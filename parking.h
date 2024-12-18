#ifndef PARKING_H_INCLUDED
#define PARKING_H_INCLUDED
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdbool.h>
typedef struct 
{
    char id[10];
    char loc[20];
    char cap[10];
    char type[20];
	char has_charging[10];
	int slots;
	char location_category[20];
    char idAg[10];
} parking;

typedef struct
{
	char id[10];
	int status;
} agent;

typedef struct
{
	char name[50];
	char pass[50];
} user;

void sign_up_user(user u);
int sign_in_user(user u, const char *name, const char* pass); 

int is_id_unique(const char *filename, const char *id);

bool get_parking_by_id(const char *filename, const char *parking_id, parking *result);

int is_agent_assigned(const char *filename, const char *agent_id);

void update_agent_status(const char *agent_id, int status);

void refresh_parking_treeview(GtkWidget *parking_treeview);
void refresh_agent_treeview(GtkWidget *agent_treeview);

void setup_parking_treeview(GtkWidget *parking_treeview);
void setup_agent_treeview(GtkWidget *agent_treeview);

void save_parking_to_file(parking *p);

void edit_parking(const char *filename, const char *parking_id, parking *updated_parking);

int delete_parking(const char *filename, const char *parking_id, parking *deleted_parking);

gboolean are_all_user_fields_empty(GtkWidget *username_entry, GtkWidget *password_entry);

void set_label_message(GtkWidget *label_review, const char *message);

void parking_add_clicked(GtkButton *parking_add, gpointer user_data);
void parking_edit_clicked(GtkButton *parking_edit, gpointer user_data);
void parking_delete_clicked(GtkButton *parking_delete, gpointer user_data);
void refresh_clicked(GtkButton *refresh, gpointer user_data);

void sign_up_clicked(GtkButton *sign_up, gpointer user_data);
void sign_in_clicked(GtkButton *sign_in, gpointer user_data);
void sign_out_admin_clicked(GtkButton *sign_out_admin, gpointer user_data);
#endif

