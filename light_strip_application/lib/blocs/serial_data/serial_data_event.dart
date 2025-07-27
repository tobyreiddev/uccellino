part of 'serial_data_bloc.dart';

@immutable
sealed class SerialDataEvent {}

class SerialDataInitialize extends SerialDataEvent {
  final SerialPort port;
  SerialDataInitialize({required this.port});
}

class SerialDataSend extends SerialDataEvent {
  final SerialPort port;
  final String data;

  SerialDataSend({
    required this.port,
    required this.data,
  });
}

class SerialDataStartListening extends SerialDataEvent {
  final SerialPort port;
  SerialDataStartListening(this.port);
}

class SerialDataStopListening extends SerialDataEvent {
  final SerialPort port;
  SerialDataStopListening(this.port);
}

class SerialDataReceived extends SerialDataEvent {
  final Uint8List data;
  SerialDataReceived(this.data);
}
