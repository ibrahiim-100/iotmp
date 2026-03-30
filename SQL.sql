CREATE TABLE safety_data (
  id INT AUTO_INCREMENT PRIMARY KEY,
  temperature FLOAT,
  smoke VARCHAR(10),
  fire VARCHAR(10),
  light_status VARCHAR(10),
  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);