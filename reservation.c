#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include "reservation.h"

void on_calendar_date_selected(GtkCalendar *calendar, salma_reservation *new_reservation) {
    guint year, month, day;
    char formatted_date[11]; 

    gtk_calendar_get_date(GTK_CALENDAR(calendar), &year, &month, &day);

    // Format date to "YYYY-MM-DD" (month + 1 since it is zero-based)
    sprintf(formatted_date, "%04u-%02u-%02u", year, month + 1, day);

    strcpy(new_reservation->date, formatted_date);

    printf("Selected Date: %s\n", new_reservation->date);
}


void save_reservation_to_file(salma_reservation *r) {
    FILE *file = fopen("salma_reservation.txt", "a");
    if (file == NULL) {
        printf("\nError opening parking.txt for writing.\n");
        return;
    }

    fprintf(file, "%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
            r->id, r->idCit, r->idPar, r->service, r->date, r->duration, r->status);

    fclose(file);
    printf("\n Reservation data saved successfully!\n");
}

void refresh_salma_reservation_treeview(GtkWidget *salma_reservation_treeview) {
    static gboolean columns_added = FALSE;

    if (!columns_added) {
        // Set up the columns (only once)
        setup_salma_reservation_treeview(salma_reservation_treeview);
        columns_added = TRUE;
    }

    // Create a new ListStore and populate it with updated data
    GtkListStore *store = gtk_list_store_new(7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    FILE *file = fopen("salma_reservation.txt", "r");
    if (!file) {
        perror("Failed to open salma_reservation.txt");
        return;
    }

    char id[10], idCit[10], idPar[10], service[30], date[11], duration[10], status[10];
    while (fscanf(file, "%9s\t%9s\t%9s\t%29s\t%10s\t%9s\t%9s", id, idCit, idPar, service, date, duration, status) != EOF) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter); // Add a new row
        gtk_list_store_set(store, &iter,
                           0, id,
                           1, idCit,
                           2, idPar,
                           3, service,
                           4, date,
                           5, duration,
                           6, status,
                           -1);
    }
    fclose(file);

    // Attach the ListStore to the TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(salma_reservation_treeview), GTK_TREE_MODEL(store));
    g_object_unref(store); // Release the ListStore reference
}

void setup_salma_reservation_treeview(GtkWidget *salma_reservation_treeview) {
    const char *titles[] = {"ID", "Citizen", "Parking", "Service", "Date", "Duration", "Status"};
    for (int i = 0; i < 7; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(salma_reservation_treeview), column);
    }
}

void refresh_salma_parking_treeview(GtkWidget *salma_parking_treeview) {
    static gboolean columns_added = FALSE;

    if (!columns_added) {
        // Set up the columns (only once)
        setup_salma_parking_treeview(salma_parking_treeview);
        columns_added = TRUE;
    }

    // Create a ListStore for agent data
    GtkListStore *store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    FILE *file = fopen("salma_parking.txt", "r");
    if (!file) {
        perror("Failed to open salma_parking.txt");
        return;
    }

    char free_slot[5], id[10];
    while (fscanf(file, "%4s %9s", free_slot, id) != EOF) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter); // Add a new row
        gtk_list_store_set(store, &iter,
                           0, free_slot,
                           1, id,
                           -1);
    }
    fclose(file);

    // Attach the ListStore to the TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(salma_parking_treeview), GTK_TREE_MODEL(store));
    g_object_unref(store); // Release the ListStore reference
}

void setup_salma_parking_treeview(GtkWidget *salma_parking_treeview) {
    const char *titles[] = {"Free Slots", "Parking"};
    for (int i = 0; i < 2; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(salma_parking_treeview), column);
    }
}

void edit_reservation(const char *filename, const char *reservation_id, salma_reservation *updated_reservation) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.txt", "w");
    salma_reservation current_reservation;
    int found = 0;
  
    if (!file || !temp) {
        perror("Failed to open salma_reservation.txt or temp.txt");
        return;
    }

    // Read the file and process each record
    while (fscanf(file, "%9s %9s %9s %49s %10s %9s %9s", 
                  current_reservation.id, 
                  current_reservation.idCit, 
                  current_reservation.idPar, 
                  current_reservation.service, 
                  current_reservation.date,
                  current_reservation.duration,
                  current_reservation.status) != EOF) {
        
        if (strcmp(current_reservation.id, reservation_id) == 0) {
            found = 1;
         
            // Update fields only if they are not empty
            if (strlen(updated_reservation->idCit) > 0) strcpy(current_reservation.idCit, updated_reservation->idCit);
            if (strlen(updated_reservation->idPar) > 0) strcpy(current_reservation.idPar, updated_reservation->idPar);
            if (strlen(updated_reservation->service) > 0) strcpy(current_reservation.service, updated_reservation->service);
            if (strlen(updated_reservation->date) > 0) strcpy(current_reservation.date, updated_reservation->date);
            if (strlen(updated_reservation->duration) > 0) strcpy(current_reservation.duration, updated_reservation->duration);
            if (strlen(updated_reservation->status) > 0) strcpy(current_reservation.status, updated_reservation->status);
        }

        // Write the (potentially updated) service record back to the temp file
        fprintf(temp, "%9s\t%9s\t%9s\t%49s\t%10s\t%9s\t%9s\n", current_reservation.id, current_reservation.idCit, current_reservation.idPar, current_reservation.service, current_reservation.date, current_reservation.duration, current_reservation.status);
    }

    fclose(file);
    fclose(temp);

    // Replace the original file with the updated file
    remove(filename);
    rename("temp.txt", filename);
    
	// Update the label with a success message
    GtkWidget *salma_review;
    if (found) {
        set_label_message(salma_review, "Reservation edited successfully!");
    } else {
        set_label_message(salma_review, "Edit failed! Reservation ID not found.");
    } 
}

int delete_reservation(const char *filename, const char *reservation_id, salma_reservation *deleted_reservation) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.txt", "w");
    int found = 0;
    salma_reservation r;

    if (!file || !temp) {
        perror("Failed to open file");
        return 0;
    }

    while (fscanf(file, "%s %s %s %s %s %s %s", r.id, r.idCit, r.idPar, r.service, r.date, r.duration, r.status) == 7) {
        if (strcmp(r.id, reservation_id) == 0) {
            found = 1;
            *deleted_reservation = r; // Save the deleted parking data
        } else {
            fprintf(temp, "%s %s %s %s %s %s %s\n", r.id, r.idCit, r.idPar, r.service, r.date, r.duration, r.status);
        }
    }

    fclose(file);
    fclose(temp);

    if (found) {
        remove(filename);
        rename("temp.txt", filename);
    } else {
        remove("temp.txt");
    }

    return found;
}

bool get_reservation_by_id(const char *filename, const char *reservation_id, salma_reservation *result) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return false;
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        salma_reservation temp;
        // Assuming each line is formatted as: id idCit idPar idSer period
        if (sscanf(line, "%9s %9s %9s %49s %10s %9s %9s", temp.id, temp.idCit, temp.idPar, temp.service, temp.date, temp.duration, temp.status) == 7) {
            if (strcmp(temp.id, reservation_id) == 0) {
                *result = temp;
                fclose(file);
                return true;
            }
        }
    }

    fclose(file);
    return false; // ID not found
}

