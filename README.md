<img width="2000" height="1414" alt="1" src="https://github.com/user-attachments/assets/f97d2891-f8b1-4932-b166-607de68cf8e9" />
# IoT-Based Predictive Maintenance System for Vehicle Condition Monitoring Using Machine Learning

## Project Overview

This project presents an IoT-based predictive maintenance system designed for real-time vehicle condition monitoring using machine learning techniques. It addresses the limitations of conventional maintenance approaches by enabling early fault detection and continuous monitoring of critical vehicle parameters. The system integrates embedded hardware, data processing, a Flask API, and a Node-RED dashboard to provide a comprehensive solution for automotive predictive maintenance.

## Features

- **Real-time Vehicle Condition Monitoring**: Continuously monitors important vehicle parameters such as engine temperature, vibration, and battery voltage using an ESP32 microcontroller and various sensors.

- **On-device Data Processing**: Extracts meaningful features from raw sensor data, including temperature rate of change, vibration RMS, smoothing filters, and voltage calibration, directly on the ESP32.

- **MQTT Communication**: Utilizes the MQTT protocol for efficient and lightweight data transmission from the ESP32 to a central platform.

- **Node-RED Platform**: Provides capabilities for data visualization, storage, and real-time analysis of incoming sensor data.

- **Flask API for Prediction**: Implements a Flask API for additional data processing and machine learning-based prediction of vehicle states (Normal, Warning, Fault).

- **Machine Learning Models**: Employs Random Forest and Decision Tree classifiers for vehicle state classification, with Random Forest achieving a high accuracy of 93% in experimental results.

- **Early Anomaly Detection**: Facilitates the detection of potential issues before they lead to critical failures, reducing maintenance costs and improving safety.

- **Interactive Dashboard**: Offers real-time visualization of vehicle conditions and predictive insights through a user-friendly dashboard.

## System Architecture

The system architecture is composed of several interconnected components:

1. **Hardware Layer**: ESP32 microcontroller interfaced with various sensors (e.g., temperature, vibration, voltage) for data acquisition.

2. **Communication Layer**: MQTT protocol for transmitting processed sensor data from the ESP32.

3. **Platform Layer**: Node-RED for data ingestion, visualization, and initial processing.

4. **Application Layer**: Flask API for advanced data processing, machine learning inference, and serving predictions.

5. **Database Layer**: For storing historical sensor data and prediction results.

6. **User Interface Layer**: A dashboard (likely built with Node-RED's dashboard capabilities) for presenting real-time vehicle status and predictive insights.

## Technologies Used

- **Microcontroller**: ESP32
- **Sensors**: Temperature, Vibration (MPU6050), Voltage
- **Communication Protocol**: MQTT
- **Data Processing & Visualization**: Node-RED
- **API Development**: Flask (Python)
- **Machine Learning**: Random Forest, Decision Tree (for classification)
- **Database**: (Implicit, but essential for data storage)
- **Version Control**: Git / GitHub

## Installation and Setup

To set up this project locally, you will need Docker and Docker Compose installed. The project is designed to be run in a containerized environment to ensure consistency and ease of deployment.

### 1. Clone the repository:

```bash
git clone https://github.com/YOUR_USERNAME/YOUR_REPOSITORY_NAME.git
cd YOUR_REPOSITORY_NAME
```

### 2. Configure Environment Variables

Create a `.env` file in the root directory of the project based on the `docker-compose.yml` requirements. This file will contain sensitive information like database credentials.

```
# Example .env content
DB_ROOT_PASSWORD=your_root_password
DB_NAME=your_database_name
DB_USER=your_db_user
DB_PASSWORD=your_db_password
FLASK_SECRET_KEY=a_strong_random_secret_key
```

### 3. Build and Run with Docker Compose:

```bash
docker compose build
docker compose up -d
```

This will build the Docker images for your Flask application, Node-RED, and database, and start them as detached services.

### 4. Node-RED Project Setup

Access the Node-RED interface (typically at `http://localhost:1880`). If Node-RED Projects are enabled, you can import your flows.json or manage them directly within the Node-RED editor, which integrates with Git.

## Usage

Once the Docker containers are running:

- **Flask API**: Accessible at `http://localhost:5000` (or the port you configured). You can interact with its endpoints to send data and receive predictions.

- **Node-RED Dashboard**: Accessible at `http://localhost:1880/ui` (assuming you have the dashboard nodes installed and configured in Node-RED). This will display real-time sensor data and vehicle condition status.

## Experimental Results

The system was validated using a dataset of 1,619 samples, including real measurements from an ESP32 under normal conditions and simulated data for warning and fault conditions. The Random Forest classifier achieved a 93% accuracy in classifying vehicle states, outperforming the Decision Tree model, particularly in fault detection. These results demonstrate the feasibility and promising potential of combining IoT, embedded systems, and machine learning for intelligent multi-sensor predictive maintenance applications.

## Project Structure

```
├── README.md                          # Project documentation
├── Esp32conenction code/              # ESP32 firmware and connection code
│   └── EDsp32read.ino                 # Arduino sketch for ESP32
├── flask api/                         # Flask API application
│   ├── api.py                         # Main Flask application
│   ├── car_condition_model.joblib     # Trained ML model
│   └── feature_columns.joblib         # Feature column metadata
└── ML creation for predection/        # Machine learning model development
    ├── car_health_predection.py       # ML model training script
    ├── car_dataset_cleaned_enhanced.csv    # Enhanced training dataset
    ├── final_car_dataset.csv          # Final processed dataset
    └── datarealcar.csv                # Real vehicle data
```

## Contributing

Contributions are welcome! Please feel free to fork the repository, create a new branch, and submit pull requests. For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## References

[1] Younsi, O. (2026). IoT-Based Predictive Maintenance System for Vehicle Condition Monitoring Using Machine Learning (Graduation Thesis). Beijing Polytechnic University, College of Automotive Engineering.
