# ViewerBi5 — Dukascopy Tick Data Decoder (.bi5)

![C++](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Qt](https://img.shields.io/badge/framework-Qt6/Qt5-green.svg)
![License](https://img.shields.io/badge/license-MIT-lightgrey.svg)

Утилита на C++/Qt для распаковки и анализа файлов биржевых котировок формата `.bi5` (Dukascopy).

---

## 🌍 English Description

### Overview
**ViewerBi5** is a lightweight tool designed to handle binary tick data. Since `.bi5` files are compressed using LZMA, this application integrates the LZMA SDK to provide a human-readable view of the market data.

### Key Features
* **LZMA Decompression**: Direct integration of LZMA SDK for high-performance extraction.
* **Dukascopy Format Parsing**: Decodes the 20-byte binary structure of each tick.
* **Data Visualization**: Displays Tick #, Time (ms), Ask, Bid, and Volumes in a formatted list.
* **Export Capability**: Save the processed data into `.txt` or `.csv` files.
* **Clean GUI**: Built with Qt, featuring an intuitive layout and responsive design.

### Technical Details
* **Time**: Milliseconds from the beginning of the hour.
* **Price**: Decoded using `price / 100000.0`.
* **Volume**: Represents liquidity in millions.

---

## 🇷🇺 Описание на русском

### Обзор
**ViewerBi5** — это легковесный инструмент для работы с тиковыми данными. Так как файлы `.bi5` сжаты алгоритмом LZMA, данное приложение использует LZMA SDK для преобразования бинарных данных в читаемый вид.

### Основные функции
* **Распаковка LZMA**: Прямая интеграция LZMA SDK для быстрой обработки данных.
* **Парсинг формата Dukascopy**: Декодирование 20-байтовой бинарной структуры каждого тика.
* **Визуализация**: Отображение номера тика, времени (мс), цен Ask/Bid и объемов в удобном списке.
* **Экспорт**: Сохранение расшифрованных данных в файлы `.txt` или `.csv`.
* **Интерфейс**: Чистый GUI на базе Qt с удобным расположением элементов.

---

## 🛠 Installation & Build / Сборка и установка

1.  **Clone the repository / Клонируйте репозиторий**:
    ```bash
    git clone [https://github.com/ВАШ_НИК/ViewerBi5.git](https://github.com/ВАШ_НИК/ViewerBi5.git)
    ```
2.  **Open in Qt Creator / Откройте в Qt Creator**:
    Select `CMakeLists.txt` and configure the project.
3.  **Build / Соберите проект**:
    Use Ninja or MinGW to compile the executable.

---
*Developed as a practice in C++ and Qt binary data processing.*
