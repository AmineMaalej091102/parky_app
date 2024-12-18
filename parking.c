#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include "parking.h"

int is_id_unique(const char *filename, const char *id) {
    char temp_id[6];
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return 1; 
    }

    while (fscanf(file, "%5s", temp_id) != EOF) {
        if (strcmp(temp_id, id) == 0) {
            fclose(file);
            return 0; 
        }
    }

    fclose(file);
    return 1; 
}

int is_agent_assigned(const char *filename, const char *agent_id) {
    char temp_agent_id[6];
    int status;
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return 0; // If file doesn't exist, assume agent is not assigned
    }

    // Read through the file line by line
    while (fscanf(file, "%5s %d", temp_agent_id, &status) != EOF) {
        if (strcmp(temp_agent_id, agent_id) == 0 && status == 1) {
            fclose(file);
            return 1; // Agent is already assigned
        }
    }

    fclose(file);
    return 0; // Agent is not assigned
}


void update_agent_status(const char *agent_id, int status) {
    FILE *file = fopen("agent.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    char id[10];
    int current_status;

    if (!file || !temp) {
        perror("Failed to open agent.txt or temp.txt");
        return;
    }

    while (fscanf(file, "%9s %d", id, &current_status) != EOF) {
        if (strcmp(id, agent_id) == 0) {
            fprintf(temp, "%s %d\n", id, status); // Update status
        } else {
            fprintf(temp, "%s %d\n", id, current_status); // Keep as is
        }
    }

    fclose(file);
    fclose(temp);

    // Replace original file with updated one
    remove("agent.txt");
    rename("temp.txt", "agent.txt");
}

void refresh_parking_treeview(GtkWidget *parking_treeview) {
    static gboolean columns_added = FALSE;

    if (!columns_added) {
        // Set up the columns (only once)
        setup_parking_treeview(parking_treeview);
        columns_added = TRUE;
    }

    // Create a new ListStore and populate it with updated data
    GtkListStore *store = gtk_list_store_new(8, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
    FILE *file = fopen("parking.txt", "r");
    if (!file) {
        perror("Failed to open parking.txt");
        return;
    }

    char id[10], loc[20], cap[10], type[20], has_charging[10], location_category[20], idAg[10];
    int slots; 
    while (fscanf(file, "%9s\t%19s\t%9s\t%19s\t%9s\t%d\t%19s\t%9s", id, loc, cap, type, has_charging, &slots, location_category, idAg) != EOF) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter); // Add a new row
        gtk_list_store_set(store, &iter,
                           0, id,
                           1, loc,
                           2, cap,                          
                           3, type,
                           4, has_charging,
                           5, slots, 
                           6, location_category,
                           7, idAg,
                           -1);
    }
    fclose(file);

    // Attach the ListStore to the TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(parking_treeview), GTK_TREE_MODEL(store));
    g_object_unref(store); // Release the ListStore reference
}

void setup_parking_treeview(GtkWidget *parking_treeview) {
    const char *titles[] = {"ID", "Location", "Capacity", "Type", "Charging", "Slots", "Category", "Agent ID"};
    for (int i = 0; i < 8; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(parking_treeview), column);
    }
}


void refresh_agent_treeview(GtkWidget *agent_treeview) {
    static gboolean columns_added = FALSE;

    if (!columns_added) {
        // Set up the columns (only once)
        setup_agent_treeview(agent_treeview);
        columns_added = TRUE;
    }

    // Create a ListStore for agent data
    GtkListStore *store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    FILE *file = fopen("agent.txt", "r");
    if (!file) {
        perror("Failed to open agent.txt");
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
    gtk_tree_view_set_model(GTK_TREE_VIEW(agent_treeview), GTK_TREE_MODEL(store));
    g_object_unref(store); // Release the ListStore reference
}

void setup_agent_treeview(GtkWidget *agent_treeview) {
    const char *titles[] = {"Agent ID", "Status"};
    for (int i = 0; i < 2; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(agent_treeview), column);
    }
}


void set_label_message(GtkWidget *label_review, const char *message) {
    gtk_label_set_text(GTK_LABEL(label_review), message);
}

void save_parking_to_file(parking *p) {
    FILE *file = fopen("parking.txt", "a");
    if (file == NULL) {
        printf("\nError opening parking.txt for writing.\n");
        return;
    }

    fprintf(file, "%s\t%s\t%s\t%s\t%s\t%d\t%s\t%s\n",
            p->id, p->loc, p->cap, p->type, p->has_charging, p->slots, p->location_category, p->idAg);

    fclose(file);
    printf("\nParking data saved successfully!\n");
}

void edit_parking(const char *filename, const char *parking_id, parking *updated_parking) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.txt", "w");
    parking current_parking;
    int found = 0;
    char old_agent_id[10] = ""; // To store the old agent ID

    if (!file || !temp) {
        perror("Failed to open parking.txt or temp.txt");
        return;
    }

    // Read the file and process each record
    while (fscanf(file, "%9s %19s %9s %19s %9s %d %19s %9s", 
                  current_parking.id, 
                  current_parking.loc, 
                  current_parking.cap, 
                  current_parking.type,
                  current_parking.has_charging,
                  &current_parking.slots,
                  current_parking.location_category, 
                  current_parking.idAg) != EOF) {
        
        if (strcmp(current_parking.id, parking_id) == 0) {
            found = 1;
            // Store old agent ID for later status update
            strcpy(old_agent_id, current_parking.idAg);

            // Update fields only if they are not empty
            if (strlen(updated_parking->loc) > 0) strcpy(current_parking.loc, updated_parking->loc);
            if (strlen(updated_parking->cap) > 0) strcpy(current_parking.cap, updated_parking->cap);            
            if (strlen(updated_parking->type) > 0) strcpy(current_parking.type, updated_parking->type);
            if (strlen(updated_parking->has_charging) > 0) strcpy(current_parking.has_charging, updated_parking->has_charging);
            if (updated_parking->slots != -1)
            current_parking.slots= updated_parking->slots;
            if (strlen(updated_parking->location_category) > 0) strcpy(current_parking.location_category, updated_parking->location_category);
            if (strlen(updated_parking->idAg) > 0) strcpy(current_parking.idAg, updated_parking->idAg);
        }

        // Write the (potentially updated) parking record back to the temp file
        fprintf(temp, "%s\t%s\t%s\t%s\t%s\t%d\t%s\t%s\n", current_parking.id, current_parking.loc, current_parking.cap, current_parking.type, current_parking.has_charging, current_parking.slots, current_parking.location_category, current_parking.idAg);
    }

    fclose(file);
    fclose(temp);

    // Replace the original file with the updated file
    remove(filename);
    rename("temp.txt", filename);
    
	// Update the label with a success message
    GtkWidget *label_review;
    if (found) {
        gtk_label_set_text(GTK_LABEL(label_review), "Parking edited successfully!");
    } else {
        gtk_label_set_text(GTK_LABEL(label_review), "Edit failed! Parking ID not found.");
    }
    
    // Update agent statuses if an agent ID was changed
    if (found) {
        if (strlen(updated_parking->idAg) > 0) {
            update_agent_status(updated_parking->idAg, 1); // Mark new agent as assigned
        }

        if (strlen(old_agent_id) > 0 && strcmp(old_agent_id, updated_parking->idAg) != 0) {
            update_agent_status(old_agent_id, 0); // Mark old agent as not assigned
        }
    } else {
        printf("Edit failed: Parking ID not found.\n");
    }
}


int delete_parking(const char *filename, const char *parking_id, parking *deleted_parking) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.txt", "w");
    int found = 0;
    parking p;

    if (!file || !temp) {
        perror("Failed to open file");
        return 0;
    }

    while (fscanf(file, "%s %s %s %s %s %d %s %s", p.id, p.loc, p.cap, p.type, p.has_charging, &p.slots, p.location_category, p.idAg) == 8) {
        if (strcmp(p.id, parking_id) == 0) {
            found = 1;
            *deleted_parking = p; // Save the deleted parking data
        } else {
            fprintf(temp, "%s %s %s %s %s %d %s %s\n", p.id, p.loc, p.cap, p.type, p.has_charging, p.slots, p.location_category, p.idAg);
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


bool get_parking_by_id(const char *filename, const char *parking_id, parking *result) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return false;
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        parking temp;
        // Assuming each line is formatted as: id loc cap fees type has_charging slots location_category idAg
        if (sscanf(line, "%9s %49s %9s %19s %9s %d %19s %9s", temp.id, temp.loc, temp.cap, temp.type, temp.has_charging, &temp.slots, temp.location_category, temp.idAg) == 8) {
            if (strcmp(temp.id, parking_id) == 0) {
                *result = temp;
                fclose(file);
                return true;
            }
        }
    }

    fclose(file);
    return false; // ID not found
}


gboolean are_all_user_fields_empty(GtkWidget *username_entry, GtkWidget *password_entry) {
    const char *name = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *pass = gtk_entry_get_text(GTK_ENTRY(password_entry));
   
    return (strlen(name) == 0 && strlen(pass) == 0);
}

void sign_up_user(user u)
{
	FILE *f;
	f=fopen("user.txt","a+");
	if(f!=NULL)
	{
	fprintf(f,"%s\t%s\n",u.name,u.pass);
	fclose(f);
	}
}

int sign_in_user(user u, const char *name, const char *pass)
{
	FILE *f;
	f=fopen("user.txt","r");
	if(f!=NULL)
	{
	while(fscanf(f,"%49s %49s",u.name,u.pass)!=EOF) {
	if(strcmp(u.name,name)==0 && strcmp(u.pass,pass)==0) {
	return 1;
	}
	}
	fclose(f);
	}
}
