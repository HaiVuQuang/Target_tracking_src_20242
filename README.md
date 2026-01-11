# Target Tracking System for Firefighting Training

Hệ thống hỗ trợ huấn luyện chữa cháy trên môi trường ảo, bao gồm firmware ESP32 và server Django để định vị IPS sử dụng RSSI và dữ liệu IMU.

## Tổng quan

Dự án này phát triển một hệ thống định vị trong nhà (IPS) cho huấn luyện chữa cháy ảo. ESP32 thu thập dữ liệu từ các cảm biến (nút bấm, van, IMU) và gửi qua MQTT. Server xử lý dữ liệu, sử dụng mô hình CNN kết hợp với các bộ lọc (Kalman, WBO) để định vị người dùng.

## Cấu trúc dự án

```
Target_tracking_src_20242/
├── ESP32_firmware/          # Firmware cho ESP32
│   ├── platformio.ini       # Cấu hình PlatformIO
│   ├── include/             # Header files
│   ├── lib/                 # Thư viện
│   ├── src/                 # Source code ESP32
│   │   ├── main.cpp         # Main loop
│   │   ├── config.h         # Cấu hình
│   │   ├── button_handler.* # Xử lý nút bấm
│   │   ├── lcd_*.cpp        # Điều khiển LCD TFT
│   │   ├── mqtt_handler.*   # Giao tiếp MQTT
│   │   ├── read_imu_data.*  # Đọc dữ liệu IMU
│   │   └── valve_data_handle.* # Xử lý dữ liệu van
│   └── test/                # Test files
└── Server/                  # Server Django
    ├── requirements.txt     # Dependencies Python
    ├── config/              # Cấu hình Django
    │   ├── manage.py        # Django management
    │   ├── config/
    │   │   ├── settings.py  # Cài đặt Django
    │   │   ├── urls.py      # URL routing
    │   │   └── wsgi.py      # WSGI config
    │   └── target_tracking/ # App chính
    │       ├── models.py    # Database models
    │       ├── views.py     # Views
    │       ├── mqtt.py      # MQTT handling
    │       ├── kalmanfilter.py # Bộ lọc Kalman
    │       ├── wbo_filter.py # Bộ lọc WBO
    │       ├── ml_model.py  # Mô hình ML
    │       ├── live_plot.py # Vẽ biểu đồ real-time
    │       ├── consumers.py # WebSocket consumers
    │       ├── static/      # CSS, JS, images
    │       ├── templates/   # HTML templates
    │       ├── rssi_data/   # Dữ liệu RSSI
    │       ├── training_data/ # Dữ liệu training
    │       └── model/       # Mô hình đã train
    └── migrations/          # Database migrations
```

## Firmware ESP32

### Chức năng
- Điều khiển ngoại vi: nút bấm, van, IMU
- Hiển thị giao diện người dùng trên LCD TFT
- Thu thập dữ liệu RSSI và magnetic từ IMU
- Gửi dữ liệu qua MQTT đến server

### Setup và Build
1. Cài đặt PlatformIO
2. Mở folder `ESP32_firmware` trong VS Code
3. Build: `Ctrl+Shift+P` > "PlatformIO: Build"
4. Upload: `Ctrl+Shift+P` > "PlatformIO: Upload"

### Dependencies
- PubSubClient (MQTT)
- Adafruit ILI9341 (LCD)
- Adafruit GFX Library
- Adafruit BusIO

## Server

### Chức năng
- Dashboard quản lý: tạo map, setup bài tập, quản lý DB
- Xử lý thuật toán định vị: CNN + Kalman/WBO filter
- WebSocket cho real-time updates
- Thu thập và xử lý dữ liệu RSSI/IMU

### Setup
1. Cài đặt Python 3.8+
2. Tạo virtual environment:
   ```bash
   python -m venv venv
   venv\Scripts\activate  # Windows
   ```
3. Cài đặt dependencies:
   ```bash
   pip install -r requirements.txt
   ```
4. Chạy migrations:
   ```bash
   cd Server/config
   python manage.py migrate
   ```
5. Chạy server:
   ```bash
   python manage.py runserver
   ```

### Dependencies chính
- Django 5.0
- Channels (WebSocket)
- Keras/TensorFlow (ML)
- Paho-MQTT
- Matplotlib (plotting)
- Pandas (data processing)

## Luồng hệ thống

1. **ESP32 thu thập dữ liệu**: Đọc cảm biến, hiển thị UI, gửi MQTT
2. **Server nhận MQTT**: Lưu DB, xử lý real-time
3. **Định vị**: Sử dụng RSSI + IMU để predict vị trí qua CNN
4. **Lọc**: Áp dụng Kalman/WBO filter để smooth vị trí
5. **Dashboard**: Hiển thị vị trí, quản lý bài tập

## Cách sử dụng

1. Setup và chạy server
2. Build/upload firmware lên ESP32
3. Truy cập dashboard tại `http://localhost:8000`
4. Tạo map và bài tập huấn luyện
5. ESP32 kết nối MQTT và bắt đầu gửi dữ liệu

## Training Model

Dữ liệu RSSI được thu thập và preprocess trong `rssi_data/`. Model CNN được train với dữ liệu magnetic từ IMU để phân loại vị trí.

## Contributors

[Thêm thông tin contributors]

## License

[Thêm license nếu có]