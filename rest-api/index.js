const express = require('express');
const mqtt = require('mqtt');
const cors = require('cors');
const app = express();
const port = 3000;

// MQTT broker URL
const mqttBrokerUrl = 'mqtt://mqtt_broker';

// Create an MQTT client
const client = mqtt.connect(mqttBrokerUrl);

client.on('connect', () => {
  console.log('Connected to MQTT broker');
});

app.use(cors()); // Enable CORS - A cross-origin resource sharing (CORS)
app.use(express.json());

app.get('/', (req, res) => {
  res.send('Hello, World!');
});

// Endpoint to publish a message to an MQTT topic
app.post('/mqttpub', (req, res) => {
  const { topic, message } = req.body;

  if (!topic || !message) {
    // return res.status(400).send('Topic and message are required');
    return res.status(400).json({ error: 'Topic and message are required' });
  }

  client.publish(topic, message, (err) => {
    if (err) {
      // return res.status(500).send('Failed to publish message');
      return res.status(500).json({ error: 'Failed to publish message' });
    }
    // res.send('Message published successfully');
    res.json({ success: true, topic: topic, message: message });
  });
});

app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});
