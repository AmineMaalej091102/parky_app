#include <string.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include "agent.h"

void refresh_fedi_agent_treeview(GtkWidget *fedi_agent_treeview) {
    static gboolean columns_added = FALSE;

    if (!columns_added) {
        // Set up the columns (only once)
        setup_fedi_agent_treeview(fedi_agent_treeview);
        columns_added = TRUE;
    }

    // Create a new ListStore and populate it with updated data
    GtkListStore *store = gtk_list_store_new(8, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);
    FILE *file = fopen("fedi_agent.txt", "r");
    if (!file) {
        perror("Failed to open fedi_agent.txt");
        return;
    }

    char id[10], cin[10], tel[10], mail[30], gender[10], seas[10], exp[20];
    int age;
    while (fscanf(file, "%9s %9s %9s %29s %9s %d %9s %19s", id, cin, tel, mail, gender, &age, seas, exp) != EOF) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter); // Add a new row
        gtk_list_store_set(store, &iter,
                           0, id,
                           1, cin,
                           2, tel,                          
                           3, mail,
                           4, gender,
                           5, age,
                           6, seas, 
                           7, exp,
                           -1);
    }
    fclose(file);

    // Attach the ListStore to the TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(fedi_agent_treeview), GTK_TREE_MODEL(store));
    g_object_unref(store); // Release the ListStore reference
}

void setup_fedi_agent_treeview(GtkWidget *fedi_agent_treeview) {
    const char *titles[] = {"ID", "CIN", "Phone", "Mail", "Gender", "Age", "Seasonal",  "Experience"};
    for (int i = 0; i < 8; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(fedi_agent_treeview), column);
    }
}

void refresh_fedi_reservation_treeview(GtkWidget *fedi_reservation_treeview) {
    static gboolean columns_added = FALSE;

    if (!columns_added) {
        // Set up the columns (only once)
        setup_fedi_reservation_treeview(fedi_reservation_treeview);
        columns_added = TRUE;
    }

    // Create a ListStore for agent data
    GtkListStore *store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    FILE *file = fopen("fedi_reservation.txt", "r");
    if (!file) {
        perror("Failed to open fedi_reservation.txt");
        return;
    }

    char date[20], id[10];
    while (fscanf(file, "%19s %9s", date, id) != EOF) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter); // Add a new row
        gtk_list_store_set(store, &iter,
                           0, date,
                           1, id,
                           -1);
    }
    fclose(file);

    // Attach the ListStore to the TreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(fedi_reservation_treeview), GTK_TREE_MODEL(store));
    g_object_unref(store); // Release the ListStore reference
}

void setup_fedi_reservation_treeview(GtkWidget *fedi_reservation_treeview) {
    const char *titles[] = {"Date", "Reservation ID"};
    for (int i = 0; i < 2; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(fedi_reservation_treeview), column);
    }
}

void edit_agent(const char *filename, const char *agent_id, fedi_agent *updated_agent) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.txt", "w");
    fedi_agent current_agent;
    int found = 0;
  
    if (!file || !temp) {
        perror("Failed to open fedi_agent.txt or temp.txt");
        return;
    }

    while (fscanf(file, "%9s %9s %9s %19s %9s %d %9s %19s", 
                  current_agent.id, 
                  current_agent.cin, 
                  current_agent.tel, 
                  current_agent.mail,
                  current_agent.gender,
                  &current_agent.age,
                  current_agent.seas, 
                  current_agent.exp) != EOF) {
        
        if (strcmp(current_agent.id, agent_id) == 0) {
            found = 1;
         
            if (strlen(updated_agent->cin) > 0) strcpy(current_agent.cin, updated_agent->cin);
            if (strlen(updated_agent->tel) > 0) strcpy(current_agent.tel, updated_agent->tel);            
            if (strlen(updated_agent->mail) > 0) strcpy(current_agent.mail, updated_agent->mail);
            if (strlen(updated_agent->gender) > 0) strcpy(current_agent.gender, updated_agent->gender);
            if (updated_agent->age != -1)
            current_agent.age= updated_agent->age;
            if (strlen(updated_agent->seas) > 0) strcpy(current_agent.seas, updated_agent->seas);
            if (strlen(updated_agent->exp) > 0) strcpy(current_agent.exp, updated_agent->exp);
        }

        fprintf(temp, "%s\t%s\t%s\t%s\t%s\t%d\t%s\t%s\n", current_agent.id, current_agent.cin, current_agent.tel, current_agent.mail, current_agent.gender, current_agent.age, current_agent.seas, current_agent.exp);
    }

    fclose(file);
    fclose(temp);

    // Replace the original file with the updated file
    remove(filename);
    rename("temp.txt", filename);
    
	// Update the label with a success message
    GtkWidget *fedi_review;
    if (found) {
        set_label_message(fedi_review, "Agent edited successfully!");
    } else {
        set_label_message(fedi_review, "Edit failed! Agent ID not found.");
    } 
}

int delete_agent(const char *filename, const char *agent_id, fedi_agent *deleted_agent) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.txt", "w");
    int found = 0;
    fedi_agent a;

    if (!file || !temp) {
        perror("Failed to open file");
        return 0;
    }

    while (fscanf(file, "%s %s %s %s %s %d %s %s", a.id, a.cin, a.tel, a.mail, a.gender, &a.age, a.seas, a.exp) == 8) {
        if (strcmp(a.id, agent_id) == 0) {
            found = 1;
            *deleted_agent = a; // Save the deleted parking data
        } else {
            fprintf(temp, "%s %s %s %s %s %d %s %s\n", a.id, a.cin, a.tel, a.mail, a.gender, a.age, a.seas, a.exp);
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

bool get_agent_by_id(const char *filename, const char *agent_id, fedi_agent *result) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return false;
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        fedi_agent temp;
        
        if (sscanf(line, "%9s %9s %9s %19s %9s %d %9s %19s", temp.id, temp.cin, temp.tel, temp.mail, temp.gender, &temp.age, temp.seas, temp.exp) == 8) {
            if (strcmp(temp.id, agent_id) == 0) {
                *result = temp;
                fclose(file);
                return true;
            }
        }
    }

    fclose(file);
    return false; // ID not found
}

void save_agent_to_file(fedi_agent *a) {
    FILE *file = fopen("fedi_agent.txt", "a");
    if (file == NULL) {
        printf("\nError opening fedi_agent.txt for writing.\n");
        return;
    }

    fprintf(file, "%s\t%s\t%s\t%s\t%s\t%d\t%s\t%s\n",
            a->id, a->cin, a->tel, a->mail, a->gender, a->age, a->seas, a->exp);

    fclose(file);
    printf("\n Agent data saved successfully!\n");
}
