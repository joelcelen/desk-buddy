class Notification {
  private:
    const char* notificationMessage;         //notification message
    unsigned long intervalNotification;      //time interval to send notification to user (default: 3600000)
    unsigned long lastNotification;          //last notification send time

  public:
    // Constructors
    Notification();
    Notification(const char* message, unsigned long interval);

    // Getters
    const char* getMessage();
    unsigned long getInterval();
    unsigned long getLastNotification();

    // Setters
    void setMessage(const char* message);
    void setInterval(unsigned long interval);
    void setLastNotification(unsigned long lastNotification);
};