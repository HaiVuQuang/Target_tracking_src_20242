from django.urls import re_path

from . import consumers

websocket_urlpatterns = [
    re_path(r'ws/web/$', consumers.ChatConsumer.as_asgi()),
    re_path(r'ws/web/training/$', consumers.Trainingconsumer.as_asgi()),
    re_path(r'ws/web/monitoring/$', consumers.Monitoringconsumer.as_asgi()),
]