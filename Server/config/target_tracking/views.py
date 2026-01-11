from django.shortcuts import render
from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
import json
import numpy as np
from .models import Map_Infor, Fires_Information
from .collect_rssi import collect_data
from django.core.cache import cache
from .apps import client

def web(request):
    return render(request, 'chat/web.html')

@csrf_exempt
def home(request):
    maps = Map_Infor.objects.all()
    if request.method == 'POST':
        try:
            data = json.loads(request.body)
            id_map = int(data.get('map_id'))
            map_obj = Map_Infor.objects.get(map_infor_id=id_map)
            cache.set('map_obj', map_obj, timeout=None)
        except Exception as e:  
            return JsonResponse({'status': 'error', 'message': str(e)})        
        return JsonResponse({'status': 'ok'})
    return render(request, 'chat/home.html', {'maps': maps})

@csrf_exempt
def settingmap(request):
    if request.method == 'POST':
        try:
            map_obj = cache.get('map_obj')
            data = json.loads(request.body)
            total_units = int(data.get('total_units'))
            area_of_one_unit = float(data.get('area_of_one_unit'))
            walkable_area = data.get('walkable_area')
            router_number = int(data.get('router_number'))
            router_location = list(map(int, data.get('router_location').split(',')))
            if len(router_location) != router_number:
                return JsonResponse({'status': 'error', 'message': 'Invalid router location'})
            Map_Infor.objects.create(total_units=total_units, area_of_one_unit=area_of_one_unit,walkable_area = walkable_area, router_number=router_number, router_location=router_location)
            map_obj = Map_Infor.objects.latest('map_infor_id')
            cache.set('map_obj', map_obj, timeout=None)
            client.publish("map_infor", walkable_area)
        except Exception as e:  
            return JsonResponse({'status': 'error', 'message': str(e)})        
        return JsonResponse({'status': 'ok'})
    return render(request, 'chat/settingmap.html')

@csrf_exempt
def collectdata(request):
    if request.method == 'POST':
        try:
            map_obj = cache.get('map_obj')
            data = json.loads(request.body)
            location = int(data.get('location'))
            axis_x = float(data.get('axis_x'))
            axis_y = float(data.get('axis_y'))
            number_of_rssi = int(data.get('number_of_rssi'))
            save = collect_data(location, axis_x, axis_y, number_of_rssi, map_obj)
            while True:
                if save.check() == number_of_rssi:
                    break
            return JsonResponse({'status': 'ok'})
        except Exception as e:
            return JsonResponse({'status': 'error', 'message': str(e)})
    return render(request, 'chat/collectdata.html')

def training (request):
    return render(request, 'chat/training.html')

@csrf_exempt
def setupexercise(request):
    map_obj = cache.get('map_obj')
    fires_info = Fires_Information.objects.filter(map_infor=map_obj).values('task_number').distinct().order_by('task_number')
    last_record = Fires_Information.objects.filter(map_infor=map_obj).values('task_number').last()
    if last_record:
        next_task_number = last_record['task_number'] + 1
    else:
        next_task_number = 1
    context = {
        'unwalkable_area': map_obj.walkable_area,
        'fires_info': fires_info,
        'next_task_number': next_task_number
    }
    if request.method == 'POST':
        try:
            data = json.loads(request.body)
            if 'task_number' in data:
                # Xử lý POST từ submitTaskSelection()
                fires_size = []
                fires_id = []
                task_number = data.get('task_number')
                values = Fires_Information.objects.filter(map_infor=map_obj, task_number=task_number).values('fires_axis_x', 'fires_axis_y', 'fires_size')
                for value in values:
                    fire_axis_x = value['fires_axis_x']
                    fire_axis_y = value['fires_axis_y']
                    fire_size = value['fires_size']
                    fire_id = 10 * (fire_axis_y + 0.5) - (10 - (fire_axis_x + 0.5))
                    fires_size.append(fire_size)
                    fires_id.append(fire_id)
                return JsonResponse({'fires_size': fires_size, 'fires_id': fires_id})

            else:     
                fire_locations = np.array(list(map(int, data.get('fire_locations').split(','))))
                fire_size = np.array(list(map(int, data.get('fire_size').split(','))))
                task_number = int(data.get('task'))
                Fires_Information.objects.filter(map_infor=map_obj, task_number=task_number).delete()  # Xóa các bản ghi cũ trước khi thêm mới
                for i in range(fire_locations.shape[0]):
                    fire_axis_y = (fire_locations[i]-1) // 10 + 0.5
                    fire_axis_x = (fire_locations[i]-1) % 10 + 0.5
                    Fires_Information.objects.create(map_infor=map_obj, fires_axis_x=fire_axis_x, fires_axis_y=fire_axis_y, fires_size=fire_size[i], task_number=task_number)
        except Exception as e:  
            return JsonResponse({'status': 'error', 'message': str(e)})
        return JsonResponse({'status': 'ok'})
    return render(request, 'chat/setupexercise.html', context)

def monitoring(request):
    map_obj = cache.get('map_obj')
    fires_info = Fires_Information.objects.filter(map_infor=map_obj).values('task_number').distinct().order_by('task_number')
    return render(request, 'chat/monitoring.html', {'fires_info': fires_info})