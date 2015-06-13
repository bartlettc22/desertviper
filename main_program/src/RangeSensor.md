#Sensor Timing:
0.000010s (10 microseconds) Trigger Pulse<br>
0.000200s (200 microseconds) Send Pulse Burst (8 Pulses @ 40kHz)<br>
0.000060s (60 microseconds) Receive Response (2 cm away)<br>
0.012000s (12,000 microseconds) Receive Response (400cm away)<br>

##Shortest cases scenario (Object 2cm away)
Total sensor time: 270 microseconds (0.00027 seconds)

## Longest case scenario (Object 3m away)
Total sensor time: 12,2100 microseconds (0.01221 seconds)

## Conclusions
With this, we can set the timeout of the pulseIn function to about 15,000 milliseconds so we're not waiting up to 1 full second for a response when reading the sensor
