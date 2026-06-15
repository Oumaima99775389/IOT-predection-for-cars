from flask import Flask, request, jsonify
import joblib
import pandas as pd

app = Flask(__name__)

# 1. Load the model and feature names we saved earlier
model = joblib.load('car_condition_model.joblib')
feature_cols = joblib.load('feature_columns.joblib')

@app.route('/predict', methods=['POST'])
def predict():
    try:
        # 2. Get JSON data from Node-RED
        data = request.get_json()
        
        # 3. Convert to DataFrame (ensures correct feature order)
        # Expected keys: 'temperature', 'temp_rate', 'vibration_g', 'voltage_real'
        input_data = pd.DataFrame([data])
        
        # 4. Make Prediction
        prediction = model.predict(input_data[feature_cols])[0]
        
        # Mapping for readability
        status_map = {0: "Normal", 1: "Warning", 2: "Fault"}
        result = status_map.get(prediction, "Unknown")

        # 5. Send response back to Node-RED
        return jsonify({
            'prediction': int(prediction),
            'status': result,
            'confidence': "High" # Optional: add probability if needed
        })

    except Exception as e:
        return jsonify({'error': str(e)}), 400

if __name__ == '__main__':
    # Run on port 5000
    app.run(debug=True, port=5000)