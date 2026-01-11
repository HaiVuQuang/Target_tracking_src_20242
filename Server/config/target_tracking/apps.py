from django.apps import AppConfig
client = None

class Target_trackingConfig(AppConfig):
    default_auto_field = 'django.db.models.BigAutoField'
    name = 'target_tracking'

    def ready(self):
        global client
        # Import the MQTT client setup function
        from . import mqtt
        client = mqtt.connect_mqtt()
