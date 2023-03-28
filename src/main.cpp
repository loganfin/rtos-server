#include <Arduino.h>
#include <WiFi.h>

void vServer(void* parameters);

void setup()
{
    Serial.begin(115200);
    xTaskCreate(vServer, "Web Server", 4096, nullptr, 1, nullptr);
}

void loop() {}

void vServer(void* parameters)
{
    const char* ssid = "";
    const char* password = "";
    WiFiServer server(80);
    WiFiClient client;
    String header;
    String current_line = "";
    char c;

    const uint64_t timeout_ms = 2000;
    uint64_t current_time = millis();
    uint64_t previous_time = 0;


    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();

    while(true) {
        client = server.available();

        if (client) {
            current_time = millis();
            previous_time = current_time;
            Serial.println("New Client.");

            while (client.connected() && current_time - previous_time <= timeout_ms) {
                current_time = millis();

                if (client.available()) {
                    c = client.read();
                    Serial.write(c);
                    header += c;

                    if (c == '\n') {
                        if (current_line.length() == 0) {
                            // http header
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-type:text/html");
                            client.println("Connection: close");
                            client.println();

                            // http page content
                            client.println(
                                    "<!DOCTYPE html>"
                                    "<html lang='en'>"
                                    "   <head>"
                                    "       <link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/css/bootstrap.min.css' integrity='sha384-GLhlTQ8iRABdZLl6O3oVMWSktQOp6b7In1Zl3/Jr59b6EGGoI1aFkw7cmDA6j6gD' crossorigin='anonymous'>"
                                    "   </head>"
                                    "   <body>"
                                    "       <div class='mt-3 text-center'>"
                                    "           <h1>Logan Finley</h1>"
                                    "           <h1>March 27th, 2023</h1>"
                                    "           <button class='btn btn-outline-primary'>"
                                    "               hello"
                                    "           </button>"
                                    "       </div>"
                                    "   </body>"
                                    "</html>"
                                    );
                            client.println();

                            break;
                        } else {
                            current_line = "";
                        }
                    } else if (c != '\r') {
                        current_line += c;
                    }
                }
            }
            header = "";

            client.stop();
            Serial.println("Client disconnected.");
            Serial.println("");
        }
    }
}
