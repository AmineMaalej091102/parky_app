#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include "service.h"

int is_service_assigned(const char *filename, const char *service_id) {
    char temp_service_id[6];
    int status;
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return 0; // If file doesn't exist, assume service is not assigned
    }

    // Read through the file line by line
    while (fscanf(file, "%5s %d", temp_service_id, &status) != EOF) {
        if (strcmp(temp_service_id, service_id) == 0 && status == 1) {
            fclose(file);
            return 1; // Service is already assigned
        }
    }

    fclose(file);
    return 0; // Service is not assigned
}

void refresh_iheb_service_treeview(GtkWidget *iheb_service_treeview) {
    static gboolean columns_added = FALSE;

    if (!columns_added) {
        // Set up the columns (only once)
        setup_iheb_service_treeview(iheb_service_treeview);
        columns_added = TRUE;
    }

    // Create a new ListStore and populate it with updated data
    GtkListStore *store = gtk_list_store_new(8, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
    FILE *file = fopen("iheb_service.txt", "r");
    if (!file) {
        perror("Failed to open iheb_service.txt");
        return;
    }

    char id[10], des[30], price[10], time[10], type[10], bonus[10], category[10];
    int staff;
    while (fscanf(file, "%9s\t%29s\t%9s\t%9s\t%9s\t%d\t%9s\t%9s", id, des, price, time, type, &staff, bonus, category) != EOF) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter); // Add a new row
        gtk_list_store_set(store, &iter,
                           0, id,
                           1, des,
                           2, price,                          
                           3, time,
                           4, type,
                           5, staff, 
                           6, bonus,
                           7, category,
                           -1);
    }
    fclose(file);

    // Attach the ListStore to the TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(iheb_service_treeview), GTK_TREE_MODEL(store));
    g_object_unref(store); // Release the ListStore reference
}

void setup_iheb_service_treeview(GtkWidget *iheb_service_treeview) {
    const char *titles[] = {"ID", "Description", "Price", "Time", "Type", "Staff", "Bonus", "Category"};
    for (int i = 0; i < 8; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(iheb_service_treeview), column);
    }
}

void save_service_to_file(iheb_service *s) {
    FILE *file = fopen("iheb_service.txt", "a");
    if (file == NULL) {
        printf("\nError opening iheb_service.txt for writing.\n");
        return;
    }

    fprintf(file, "%s\t%s\t%s\t%s\t%s\t%d\t%s\t%s\n",
            s->id, s->des, s->price, s->time, s->type, s->staff, s->bonus, s->category);

    fclose(file);
    printf("\n Service data saved successfully!\n");
}

void refresh_iheb_reservation_treeview(GtkWidget *iheb_reservation_treeview) {
    static gboolean columns_added = FALSE;

    if (!columns_added) {
        // Set up the columns (only once)
        setup_iheb_reservation_treeview(iheb_reservation_treeview);
        columns_added = TRUE;
    }

    // Create a ListStore for agent data
    GtkListStore *store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    FILE *file = fopen("iheb_reservation.txt", "r");
    if (!file) {
        perror("Failed to open iheb_reservation.txt");
        return;
    }

    char id[10], status[2];
    while (fscanf(file, "%9s %1s", id, status) != EOF) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter); // Add a new row
        gtk_list_store_set(store, &iter,
                           0, id,
                           1, (strcmp(status, "1") == 0) ? "Assigned" : "Not Assigned",
                           -1);
    }
    fclose(file);

    // Attach the ListStore to the TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(iheb_reservation_treeview), GTK_TREE_MODEL(store));
    g_object_unref(store); // Release the ListStore reference
}

void setup_iheb_reservation_treeview(GtkWidget *iheb_reservation_treeview) {
    const char *titles[] = {"ID", "Status"};
    for (int i = 0; i < 2; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(iheb_reservation_treeview), column);
    }
}

void update_service_status(const char *service_id, int status) {
    FILE *file = fopen("iheb_reservation.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    char id[10];
    int current_status;

    if (!file || !temp) {
        perror("Failed to open iheb_reservation.txt or temp.txt");
        return;
    }

    while (fscanf(file, "%9s %d", id, &current_status) != EOF) {
        if (strcmp(id, service_id) == 0) {
            fprintf(temp, "%s %d\n", id, status); // Update status
        } else {
            fprintf(temp, "%s %d\n", id, current_status); // Keep as is
        }
    }

    fclose(file);
    fclose(temp);

    // Replace original file with updated one
    remove("iheb_reservation.txt");
    rename("temp.txt", "iheb_reservation.txt");
}

void edit_service(const char *filename, const char *service_id, iheb_service *updated_service) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.txt", "w");
    iheb_service current_service;
    int found = 0;
  
    if (!file || !temp) {
        perror("Failed to open iheb_service.txt or temp.txt");
        return;
    }

    while (fscanf(file, "%9s %49s %9s %9s %9s %d %9s %9s", 
                  current_service.id, 
                  current_service.des, 
                  current_service.price, 
                  current_service.time,
                  current_service.type,
                  &current_service.staff,
                  current_service.bonus, 
                  current_service.category) != EOF) {
        
        if (strcmp(current_service.id, service_id) == 0) {
            found = 1;
         
            if (strlen(updated_service->des) > 0) strcpy(current_service.des, updated_service->des);
            if (strlen(updated_service->price) > 0) strcpy(current_service.price, updated_service->price);            
            if (strlen(updated_service->time) > 0) strcpy(current_service.time, updated_service->time);
            if (strlen(updated_service->type) > 0) strcpy(current_service.type, updated_service->type);
            if (updated_service->staff != -1)
            current_service.staff= updated_service->staff;
            if (strlen(updated_service->bonus) > 0) strcpy(current_service.bonus, updated_service->bonus);
            if (strlen(updated_service->category) > 0) strcpy(current_service.category, updated_service->category);
        }

        fprintf(temp, "%s\t%s\t%s\t%s\t%s\t%d\t%s\t%s\n", current_service.id, current_service.des, current_service.price, current_service.time, current_service.type, current_service.staff, current_service.bonus, current_service.category);
    }

    fclose(file);
    fclose(temp);

    // Replace the original file with the updated file
    remove(filename);
    rename("temp.txt", filename);
    
	// Update the label with a success message
    GtkWidget *iheb_review;
    if (found) {
        set_label_message(iheb_review, "Service edited successfully!");
    } else {
        set_label_message(iheb_review, "Edit failed! Service ID not found.");
    } 
}

int delete_service(const char *filename, const char *service_id, iheb_service *deleted_service) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.txt", "w");
    int found = 0;
    iheb_service s;

    if (!file || !temp) {
        perror("Failed to open file");
        return 0;
    }

    while (fscanf(file, "%s %s %s %s %s %d %s %s", s.id, s.des, s.price, s.time, s.type, &s.staff, s.bonus, s.category) == 8) {
        if (strcmp(s.id, service_id) == 0) {
            found = 1;
            *deleted_service = s; // Save the deleted parking data
        } else {
            fprintf(temp, "%s %s %s %s %s %d %s %s\n", s.id, s.des, s.price, s.time, s.type, s.staff, s.bonus, s.category);
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

bool get_service_by_id(const char *filename, const char *service_id, iheb_service *result) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return false;
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        iheb_service temp;
        // Assuming each line is formatted as: id des staff price idRes
        if (sscanf(line, "%9s %49s %9s %9s %9s %d %9s %9s", temp.id, temp.des, temp.price, temp.time, temp.type, &temp.staff, temp.bonus, temp.category) == 8) {
            if (strcmp(temp.id, service_id) == 0) {
                *result = temp;
                fclose(file);
                return true;
            }
        }
    }

    fclose(file);
    return false; // ID not found
}
