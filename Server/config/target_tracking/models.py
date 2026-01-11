from django.db import models
from django.contrib.auth.models import AbstractBaseUser, BaseUserManager
from django.contrib.auth.models import PermissionsMixin
from django.utils import timezone

class UserManager(BaseUserManager):
    def create_user(self, email, password=None, **extra_fields):
        if not email:
            raise ValueError('The Email field must be set')
        email = self.normalize_email(email)
        user = self.model(email=email, **extra_fields)
        user.set_password(password)
        user.save(using=self._db)
        return user

    def create_superuser(self, email, password=None, **extra_fields):
        extra_fields.setdefault('is_staff', True)
        extra_fields.setdefault('is_superuser', True)

        if extra_fields.get('is_staff') is not True:
            raise ValueError('Superuser must have is_staff=True.')
        if extra_fields.get('is_superuser') is not True:
            raise ValueError('Superuser must have is_superuser=True.')

        return self.create_user(email, password, **extra_fields)

class User(AbstractBaseUser, PermissionsMixin):
    id = models.AutoField(primary_key=True)
    email = models.EmailField(unique=True)
    is_active = models.BooleanField(default=True)
    is_trainee = models.BooleanField(default=False)
    is_superuser = models.BooleanField(default=False)
    map_infor = models.ForeignKey('Map_Infor', on_delete=models.CASCADE, null=True, blank=True)
    score = models.IntegerField(default=0)
    training_time = models.TimeField(default='00:00:00')
    objects = UserManager()
    USERNAME_FIELD = 'email'
    REQUIRED_FIELDS = []

    def __str__(self):
        return self.email

class Device(models.Model):
    device_id = models.AutoField(primary_key=True)
    user = models.ManyToManyField('User', related_name='devices')
    device_name = models.CharField(max_length=100)
    date_created = models.DateTimeField(default=timezone.now)
    date_modified = models.DateTimeField(auto_now=True)

    def __str__(self):
        return self.device_name

class DeviceInfor(models.Model):
    device_infor_id = models.AutoField(primary_key=True)
    # device = models.ForeignKey(Device, on_delete=models.CASCADE)
    rssi1 = models.FloatField()
    rssi2 = models.FloatField()
    rssi3 = models.FloatField()
    rssi4 = models.FloatField()
    accx = models.FloatField()
    accy = models.FloatField()
    accz = models.FloatField()
    magx = models.FloatField()
    magy = models.FloatField()
    magz = models.FloatField()
    gyrox = models.FloatField()
    gyroy = models.FloatField()
    gyroz = models.FloatField()
    eulerx = models.FloatField()
    eulery = models.FloatField()
    eulerz = models.FloatField()
    date_created = models.DateTimeField(default=timezone.now)

    def __str__(self):
        return f"TimeStamp: {self.date_created} --- ID: {self.device_infor_id}"
  
class Map_Infor(models.Model):
    map_infor_id = models.AutoField(primary_key=True)
    total_units = models.IntegerField()
    area_of_one_unit = models.FloatField()
    walkable_area = models.CharField(max_length=200)
    router_number = models.IntegerField()
    router_location = models.JSONField()
    date_created = models.DateTimeField(default=timezone.now)

    def __str__(self):
        return f"Map ID: {self.map_infor_id} - Total Units: {self.total_units}"
    
class Trainee_Location(models.Model):
    trainee_location_id = models.AutoField(primary_key=True)
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    map_infor = models.ForeignKey(Map_Infor, on_delete=models.CASCADE)
    axis_x = models.FloatField()
    axis_y = models.FloatField()
    date_created = models.DateTimeField(default=timezone.now)

    def __str__(self):
        return f"TimeStamp: {self.date_created} --- ID: {self.trainee_location_id}"
    
class Fires_Information(models.Model):
    fires_location_id = models.AutoField(primary_key=True)
    map_infor = models.ForeignKey(Map_Infor, on_delete=models.CASCADE)
    fires_axis_x = models.FloatField()
    fires_axis_y = models.FloatField()
    fires_size = models.IntegerField()
    task_number = models.IntegerField()
    date_created = models.DateTimeField(default=timezone.now)

    def __str__(self):
        return f"TimeStamp: {self.date_created} --- ID: {self.fires_location_id}"
    
class Data_for_Training(models.Model):
    rssi_for_training_id = models.AutoField(primary_key=True)
    map_infor = models.ForeignKey(Map_Infor, on_delete=models.CASCADE)
    rssi1 = models.FloatField()
    rssi2 = models.FloatField()
    rssi3 = models.FloatField()
    rssi4 = models.FloatField()
    magy = models.FloatField()
    magz = models.FloatField()
    location = models.IntegerField()
    axis_x = models.FloatField()
    axis_y = models.FloatField()
    date_created = models.DateTimeField(default=timezone.now)

    def __str__(self):
        return f"TimeStamp: {self.date_created} --- ID: {self.rssi_for_training_id}"