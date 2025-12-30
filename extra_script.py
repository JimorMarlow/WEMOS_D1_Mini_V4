import os
import shutil
import json
from datetime import datetime

Import("env")

def before_upload(source, target, env):
    print("\n=== Preparing files for LittleFS ===")
    
    # Путь к папке data
    data_dir = "data"
    
    # Проверяем существование папки
    if not os.path.exists(data_dir):
        print(f"Creating {data_dir} directory...")
        os.makedirs(data_dir)
        
        # Создаем базовую структуру
        os.makedirs(os.path.join(data_dir, "css"), exist_ok=True)
        os.makedirs(os.path.join(data_dir, "js"), exist_ok=True)
        os.makedirs(os.path.join(data_dir, "images"), exist_ok=True)
        os.makedirs(os.path.join(data_dir, "config"), exist_ok=True)
        
        print("Created default directory structure")
    
    # Создаем файл версии
    version_info = {
        "build_time": datetime.now().isoformat(),
        "project": env["PIOENV"],
        "framework": env["PIOFRAMEWORK"],
        "board": env["BOARD"]
    }
    
    version_path = os.path.join(data_dir, "config", "version.json")
    with open(version_path, "w") as f:
        json.dump(version_info, f, indent=2)
    
    print(f"Created version file: {version_path}")
    
    # Подсчет файлов
    file_count = 0
    total_size = 0
    
    for root, dirs, files in os.walk(data_dir):
        for file in files:
            file_path = os.path.join(root, file)
            file_size = os.path.getsize(file_path)
            total_size += file_size
            file_count += 1
    
    print(f"Total files: {file_count}")
    print(f"Total size: {total_size} bytes")
    print("===============================\n")