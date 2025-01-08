import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Weather Station',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: WeatherStationScreen(),
    );
  }
}

class WeatherStationScreen extends StatefulWidget {
  @override
  _WeatherStationScreenState createState() => _WeatherStationScreenState();
}

class _WeatherStationScreenState extends State<WeatherStationScreen> {
  double temperature = 0.0;
  double humidity = 0.0;
  double pressure = 0.0;
  bool isLoading = false;

  final String username = 'abc';
  final String aioKey = 'xyz';

Future<double> fetchData(String feed) async {
  final url = 'https://io.adafruit.com/api/v2/$username/feeds/$feed/data/last';
  final response = await http.get(
    Uri.parse(url),
    headers: {'X-AIO-Key': aioKey},
  );

  if (response.statusCode == 200) {
    final data = json.decode(response.body);
    return double.parse(data['value']);
  } else {
    throw Exception('Failed to load data');
  }
}


  Future<void> fetchWeatherData() async {
    setState(() {
      isLoading = true;
    });

    try {
      temperature = await fetchData('temperature');
      humidity = await fetchData('humidity');
      pressure = await fetchData('pressure');
    } catch (error) {
      print('Error fetching data: $error');
    } finally {
      setState(() {
        isLoading = false;
      });
    }
  }

  @override
  void initState() {
    super.initState();
    fetchWeatherData();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Weather Station'),
      ),
      body: Container(
        decoration: BoxDecoration(
          gradient: LinearGradient(
            colors: [Colors.blue.shade100, Colors.blue.shade400],
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
          ),
        ),
        child: Center(
          child: Padding(
            padding: const EdgeInsets.all(16.0),
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                buildDataCard(
                  icon: Icons.thermostat,
                  label: 'Temperature',
                  value: '$temperature °C',
                  color: Colors.redAccent,
                ),
                SizedBox(height: 16),
                buildDataCard(
                  icon: Icons.water_drop,
                  label: 'Humidity',
                  value: '$humidity %',
                  color: Colors.blueAccent,
                ),
                SizedBox(height: 16),
                buildDataCard(
                  icon: Icons.compress,
                  label: 'Pressure',
                  value: '$pressure hPa',
                  color: Colors.green,
                ),
                SizedBox(height: 32),
                ElevatedButton.icon(
                  onPressed: fetchWeatherData,
                  icon: isLoading
                      ? SizedBox(
                          height: 24,
                          width: 24,
                          child: CircularProgressIndicator(
                            color: Colors.white,
                            strokeWidth: 2,
                          ),
                        )
                      : Icon(Icons.refresh),
                  label: Text('Refresh Data'),
                  style: ElevatedButton.styleFrom(
                    padding:
                        EdgeInsets.symmetric(horizontal: 24, vertical: 12),
                    textStyle: TextStyle(fontSize: 18),
                  ),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }

  Widget buildDataCard(
      {required IconData icon, required String label, required String value, required Color color}) {
    return Card(
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(15),
      ),
      elevation: 4,
      child: Padding(
        padding: const EdgeInsets.symmetric(vertical: 16.0, horizontal: 24.0),
        child: Row(
          children: [
            Icon(icon, size: 40, color: color),
            SizedBox(width: 16),
            Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  label,
                  style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold),
                ),
                SizedBox(height: 8),
                Text(
                  value,
                  style: TextStyle(fontSize: 20),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}
