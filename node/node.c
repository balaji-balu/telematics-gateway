#include <mosquitto.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <jansson.h>
#include <stdlib.h> // For random number generation (example)
#include <time.h>   // For seeding the random number generator
#include <locale.h>
//#include <uuid/uuid.h>

// ... (Mosquitto initialization and connection - same as before)
#define MQTT_BROKER "mosquitto" // Replace with your broker's address
#define MQTT_PORT 1883 // Default MQTT port
#define MQTT_TOPIC "telematics/data" // Your MQTT topic
#define MQTT_CLIENT_ID "telematics_node_client" // Unique client ID

// Function to simulate reading sensor data (replace with actual sensor reading)
float read_temperature_sensor() {
    // Replace this with your actual sensor reading code
    // This is just a simulation for testing
    // Seed the random number generator once
    static int seeded = 0;
    if (!seeded) {
        srand(time(NULL));
        seeded = 1;
    }
    return 20.0 + (float)rand() / RAND_MAX * 10.0; // Simulate temperature between 20 and 30
}

// Function to simulate reading GPS data (replace with actual GPS reading)
void read_gps_data(float *latitude, float *longitude) {
    // Replace this with your actual GPS reading code
    // Simulation
    *latitude = 37.77 + (float)rand() / RAND_MAX * 0.1;  // Simulate latitude
    *longitude = -122.41 + (float)rand() / RAND_MAX * 0.1; // Simulate longitude
}

void get_unique_node_id(char *node_id) {
    //uuid_t uuid;
    //uuid_generate(uuid);
    //uuid_unparse(uuid, node_id);
    int rand_value = 3777 + rand();
    snprintf(node_id, 50, "node%d", rand_value);
}

void publish_discovery_message(struct mosquitto *mosq, const char *node_id) {
    json_t *root = json_object();

    fprintf(stdout, "publish_discovery_message begin");

    // Node ID (Essential)
    json_object_set_new(root, "node_id", json_string(node_id));

    // Sensors (Example)
    json_t *sensors_array = json_array();
    json_array_append_new(sensors_array, json_string("temperature"));
    json_array_append_new(sensors_array, json_string("gps"));
    json_object_set_new(root, "sensors", sensors_array);


    // Capabilities (Example)
    json_t *caps_array = json_array();
    json_array_append_new(caps_array, json_string("ota_updates"));
    json_object_set_new(root, "capabilities", caps_array);

    // ... Add other data (location, version, etc.) as needed

    char *json_str = json_dumps(root, JSON_COMPACT);//JSON_ENCODE_ANY);

// ***DEBUGGING***
printf("JSON string length: %zu\n", strlen(json_str));
// for (size_t i = 0; i < strlen(json_str); i++) {
//     printf("char[%zu]: %c (0x%X) ", i, json_str[i], json_str[i]);
//     if (json_str[i] < 32 || json_str[i] > 126) {
//         printf(" <-- Potential problem\n");
//     } else {
//         printf("\n");
//     }
// }
// printf("JSON string: %s\n", json_str);
// ***END DEBUGGING***


    int rc = mosquitto_publish(mosq, NULL, "discovery/nodes", strlen(json_str), json_str, 0, false);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Discovery publish error: %s\n", mosquitto_strerror(rc));
    }

    free(json_str);
    json_decref(root);

    fprintf(stdout, "publish_discovery_message end");
}

int main() {
    // ... (Mosquitto initialization and connection - same as before)
    int rc;
    struct mosquitto *mosq = NULL;
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

    setlocale(LC_ALL, "en_US.UTF-8");

    char node_id[20];
    // ... (Get unique node ID - same as before)
    get_unique_node_id(node_id);
    fprintf(stdout, "node id: %s", node_id);
    
    publish_discovery_message(mosq, node_id);

    sleep(5);
 
    while (1) {

        // char status_topic[50];
        // snprintf(status_topic, sizeof(status_topic), "status/%s", "node3715");
        // char status_message[50];
        // snprintf(status_message, sizeof(status_message), "{\"Uptime\": 127}" ); // Example
        // const char *payload = "Hello, 🌍";
        // //rc = mosquitto_publish(mosq, NULL, status_topic, strlen(status_message), status_message, 0, false);
        // rc = mosquitto_publish(mosq, NULL, status_topic, strlen(payload), payload, 0, false);
        // if (rc != MOSQ_ERR_SUCCESS) {
        //     fprintf(stderr, "Status publish error: %s\n", mosquitto_strerror(rc));
        // }

       

        // 1. Read sensor data
        float temperature = read_temperature_sensor();
        float latitude, longitude;
        read_gps_data(&latitude, &longitude);

        // 2. Create JSON payload
        json_t *root = json_object();
        json_t *temp = json_real(temperature);
        json_t *lat = json_real(latitude);
        json_t *lon = json_real(longitude);
        json_object_set_new(root, "temperature", temp);
        json_object_set_new(root, "latitude", lat);
        json_object_set_new(root, "longitude", lon);

        char *json_str = json_dumps(root, JSON_COMPACT); //JSON_ENCODE_ANY);
        if (!json_str) {
            fprintf(stderr, "json_dumps failed!\n");
        }

        // 3. Publish telemetry data
        char telemetry_topic[50];
        snprintf(telemetry_topic, sizeof(telemetry_topic), "telemetry/%s", node_id);
        fprintf(stdout,"json_str:", json_str);
        int rc = mosquitto_publish(mosq, NULL, telemetry_topic, strlen(json_str), json_str, 0, false);
        if (rc != MOSQ_ERR_SUCCESS) {
            fprintf(stderr, "Telemetry publish error: %s\n", mosquitto_strerror(rc));
        }

        free(json_str);
        json_decref(root);


         // 4. Publish status data (example: uptime)
        // (You would need to calculate the actual uptime)
        char status_topic[50];
        snprintf(status_topic, sizeof(status_topic), "status/%s", node_id);
        char status_message[50];
        snprintf(status_message, sizeof(status_message), "Uptime: %d seconds", 120); // Example
        rc = mosquitto_publish(mosq, NULL, status_topic, strlen(status_message), status_message, 0, false);
        if (rc != MOSQ_ERR_SUCCESS) {
            fprintf(stderr, "Status publish error: %s\n", mosquitto_strerror(rc));
        }

        publish_discovery_message(mosq, node_id);
        
        // 5. Check for commands from the gateway
        mosquitto_loop(mosq, 100, -1); // Process incoming messages

        sleep(5); // Wait before the next reading
    }
    // ... (Mosquitto cleanup)
    mosquitto_disconnect(mosq); // Disconnect from the broker
    mosquitto_destroy(mosq); // Destroy the Mosquitto client instance
    mosquitto_lib_cleanup(); // Clean up Mosquitto

    return 0;
}