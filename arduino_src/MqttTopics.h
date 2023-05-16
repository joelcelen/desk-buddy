// Define MQTT topics for subscribing (TOPIC_SUB*) and publishing (TOPIC_PUB*)
#define TOPIC_PUBSUB "deskBuddy"                                // publish-subscribe to deskBuddy topic
#define TOPIC_SUB_TEMP "deskBuddy/app/setTemperature"           // subscribe to temperature preference
#define TOPIC_SUB_HUMID "deskBuddy/app/setHumidity"             // subscribe to humidity preference
#define TOPIC_SUB_LIGHT "deskBuddy/app/setLight"                // subscribe to light preference
#define TOPIC_SUB_MOTIVATION "deskBuddy/app/setMotivation"      // subscribe to motivational messages preference
#define TOPIC_SUB_NOTIFICATION "deskBuddy/app/setNotification"  // subscribe to notifications from app
#define TOPIC_SUB_STANDUP "deskBuddy/app/setStandUp"            // subscribe to standup messages from app
#define TOPIC_SUB_TIMING "deskBuddy/app/setTiming"              // subscribe to timing preference
#define TOPIC_PUB_TEMP "deskBuddy/temperature"                  // publish temperature reading
#define TOPIC_PUB_HUMID "deskBuddy/humidity"                    // publish humidity reading
#define TOPIC_PUB_LIGHT "deskBuddy/light"                       // publish light reading

/** Add your own MQTT topics here:
* #define TOPIC_PUB_NAME "deskBuddy/newFeature"                 // publish to new feature status
* #define TOPIC_SUB_NAME "deskBuddy/app/setNewFeature"          // subscribe to new feature preference
*/