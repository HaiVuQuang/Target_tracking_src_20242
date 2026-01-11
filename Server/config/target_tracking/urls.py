from django.urls import path

from . import views

urlpatterns = [
    path('', views.web, name='web'),
    path('home/', views.home, name='home'),
    path('settingmap/', views.settingmap, name='settingmap'),
    path('collectdata/', views.collectdata, name='collectdata'),
    path('training/', views.training, name='training'),
    path('setupexercise/', views.setupexercise, name='setupexercise'),
    path('monitoring/', views.monitoring, name='monitoring'),
]