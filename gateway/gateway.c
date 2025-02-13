
#include <mosquitto.h>
#include <stdio.h>
#include <string.h>
#include <jansson.h>
#include <stdlib.h>
#include <unistd.h> // For sleep()

// ... (Mosquitto initialization and connection - same as before)
#define MQTT_BROKER "mosquitto" // Replace with your broker's address
#define MQTT_PORT 1883 // Default MQTT port
#define MQTT_TOPIC "telematics/data" // Your MQTT topic
#define MQTT_CLIENT_ID "telematics_gateway_client" // Unique client ID

// Structure to store information about discovered nodes
struct node_info {
    char node_id[20];
    char sensor_types[50];
    struct node_info *next;
};

struct node_info *head = NULL; // Head of the linked list of nodes

// Function to process discovery messages
void process_discovery_message(const char *message) {
    // ... (JSON parsing - same as before)
    // ... (Check if node exists, add or update - same as before)
    json_t *root, *node_id, *sensors;
    json_error_t error;

    root = json_loads(message, 0, &error);
    if (!root) {
        fprintf(stderr, "JSON parse error: %s\n", error.text);
        return;
    }

    node_id = json_object_get(root, "node_id");
    sensors = json_object_get(root, "sensors");

    if (!json_is_string(node_id) || !json_is_string(sensors)) {
        fprintf(stderr, "Invalid JSON format\n");
        json_decref(root);
        return;
    }

    // Check if the node is already in the list
    struct node_info *current = head;
    while (current != NULL) {
        if (strcmp(current->node_id, json_string_value(node_id)) == 0) {
            // Node already exists, update info if needed
            strcpy(current->sensor_types, json_string_value(sensors)); // Update sensor types
            json_decref(root);
            return;
        }
        current = current->next;
    }

    // Node is new, add it to the list
    struct node_info *new_node = (struct node_info *)malloc(sizeof(struct node_info));
    strcpy(new_node->node_id, json_string_value(node_id));
    strcpy(new_node->sensor_types, json_string_value(sensors));
    new_node->next = head;
    head = new_node;

    printf("New node discovered: %s (%s)\n", new_node->node_id, new_node->sensor_types);

    json_decref(root);


}

// Function to handle incoming MQTT messages
void on_message(struct mosquitto *client, void *userdata, const struct mosquitto_message *msg) {
    fprintf(stdout, "on_message: start");
    printf("on_message: start");
    if (strcmp(msg->topic, "discovery/nodes") == 0) {
        process_discovery_message(msg->payload);
    } else if (strncmp(msg->topic, "telemetry/", strlen("telemetry/")) == 0) {
        // Handle telemetry data from nodes
        printf("Telemetry from %s: %p\n", msg->topic, msg->payload);
        // Process or store the telemetry data as needed
    } else if (strncmp(msg->topic, "status/", strlen("status/")) == 0) {
        // Handle status updates from nodes
        printf("Status from %s: %p\n", msg->topic, msg->payload);
    } // ... handle other message types (commands, config, etc.)
}


// Function to publish commands to a specific node
void send_command(struct mosquitto *mosq, const char *node_id, const char *command) {
    char topic[50];
    snprintf(topic, sizeof(topic), "commands/%s", node_id); // e.g., "commands/node123"

    json_t *root = json_object();
    json_t *cmd = json_string(command);
    json_object_set_new(root, "command", cmd);
    char *json_str = json_dumps(root, JSON_ENCODE_ANY);

    int rc = mosquitto_publish(mosq, NULL, topic, strlen(json_str), json_str, 0, false);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Command publish error: %s\n", mosquitto_strerror(rc));
    }

    free(json_str);
    json_decref(root);
}


int main() {
    // ... (Mosquitto initialization and connection - same as before)
    int rc;
    struct mosquitto *mosq = NULL;

    fprintf(stderr, "Gateway: starting ....");

    mosquitto_lib_init(); // Initialize Mosquitto
    mosq = mosquitto_new(MQTT_CLIENT_ID, true, NULL); // Create a Mosquitto client instance
    if (!mosq) {
        fprintf(stderr, "Error: Out of memory.\n");
        return 1;
    }
    rc = mosquitto_connect(mosq, MQTT_BROKER, MQTT_PORT, 60); // Connect to the broker
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Connection failed: %s\n", mosquitto_strerror(rc));
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }

    mosquitto_subscribe(mosq, NULL, "discovery/nodes", 0); // Subscribe to discovery messages
    mosquitto_subscribe(mosq, NULL, "telemetry/#", 0);    // Subscribe to all telemetry messages
    mosquitto_subscribe(mosq, NULL, "status/#", 0);       // Subscribe to all status messages
    mosquitto_message_callback_set(mosq, on_message);      // Set the message callback function

    printf("starting the loop");
    while (1) {
        mosquitto_loop(mosq, 100, -1); // Process MQTT events

        // Example: Send a command to a node (replace with your logic)
        // (You'll need to get the node ID somehow, e.g., from user input or from the discovered nodes list)
        // char target_node_id[20] = "node123"; // Replace with the actual node ID
        // send_command(mosq, target_node_id, "get_temperature");

        sleep(5); // Example: Do other things in the gateway loop
    }

    // ... (Mosquitto cleanup and free the linked list of nodes - same as before)
    mosquitto_disconnect(mosq); // Disconnect from the broker
    mosquitto_destroy(mosq); // Destroy the Mosquitto client instance
    mosquitto_lib_cleanup(); // Clean up Mosquitto    
    return 0;
}