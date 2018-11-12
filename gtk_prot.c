#include<stdio.h>
#include<stdlib.h>
#include<gtk/gtk.h>
#include"joystick.h"

GtkWidget** toggle_vector;
GtkWidget** label_vector;
GtkWidget* val;

static gboolean delete_event(GtkWidget* widget,
				GdkEvent* event,
				gpointer data){

	gtk_main_quit();
	return FALSE;
}

void* gtk_main_thread(){
	gtk_main();
}

void register_callback(
                char * device,
                char * name,
                char number_of_axes,
                char number_of_buttons,
                int driver_version
                ){
	int i;
	//Instaciação da janela	
	GtkWidget* window;
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Controler Monitoring");
	//Definição dos callbacks da janela
	g_signal_connect(window, "delete_event", G_CALLBACK(delete_event), NULL);

	GtkWidget* label;	
	GtkWidget* toggle;
	GtkWidget* box;
	GtkWidget* box2;
	GtkWidget* box3;
	box = gtk_vbox_new(TRUE, 0);
	box2 = gtk_hbox_new(FALSE, 0);
	toggle_vector = (GtkWidget**)malloc((unsigned int)number_of_buttons*sizeof(GtkWidget*));
	char* string;
	string = (char*)malloc(12 * sizeof(char));
	for(i = 0; i < (unsigned int)number_of_buttons; i++){
		sprintf(string, "Button %d", i);	
		toggle = gtk_check_button_new_with_label(string);
		toggle_vector[i] = toggle;
		gtk_box_pack_start(GTK_BOX(box), toggle, TRUE, TRUE, 0);
		gtk_widget_show(toggle);
	}
	gtk_widget_show(box);
	gtk_box_pack_start(GTK_BOX(box2), box, TRUE, TRUE, 0);
	box = gtk_vbox_new(TRUE, 0);
	label_vector = (GtkWidget**)malloc((unsigned int)number_of_axes*sizeof(GtkWidget*));
	for(i = 0; i < (unsigned int)number_of_axes; i++){
		box3 = gtk_hbox_new(FALSE, 0);
		sprintf(string, "Axis %d", i);
		label = gtk_label_new(string);
		gtk_box_pack_start(GTK_BOX(box3), label, FALSE, TRUE, 0);
		gtk_widget_show(label);
		label = gtk_label_new("0");
		label_vector[i] = label;
		gtk_box_pack_start(GTK_BOX(box3), label, FALSE, TRUE, 10);
		gtk_widget_show(label);
		gtk_widget_show(box3);
		gtk_box_pack_start(GTK_BOX(box), box3, FALSE, TRUE, 0);	
	}
	gtk_widget_show(box);
	gtk_box_pack_start(GTK_BOX(box2), box, FALSE, TRUE, 0);
	gtk_widget_show(box2);
	free(string);
	
	gtk_container_add(GTK_CONTAINER(window), box2);	
	gtk_widget_show(window);
	pthread_t tid;
	pthread_create(&tid, NULL, gtk_main_thread, NULL);
}

void joystick_callback(t_mosaic_button_event* event){
	if(event->type == JS_EVENT_BUTTON){
		if(event->value){
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_vector[event->id]), TRUE);
		}else{
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_vector[event->id]), FALSE);
		}
	}
	if(event->type == JS_EVENT_AXIS){
		char val[12];
		sprintf(val, "%d", event->value);
		gtk_label_set_text(GTK_LABEL(label_vector[event->id]), val); 
	}
}
				
int main(int argc, char** argv){
	gtk_init(&argc, &argv);
	joystick_inicialize("/dev/input/js0", &joystick_callback, &register_callback);
	while(1) usleep(10000);
	return 0;
}
