part of 'serial_bloc.dart';

@immutable
sealed class SerialState {}

final class SerialInitial extends SerialState {}

class SerialAvailable extends SerialState {
  final List<String> availablePorts;
  final String? selectedPort;

  SerialAvailable({
    required this.availablePorts,
    this.selectedPort,
  });
}

class SerialConnected extends SerialState {
  final String portName;
  final SerialPort port;

  SerialConnected({
    required this.portName,
    required this.port,
  });
}

class SerialError extends SerialState {
  final String message;
  SerialError(this.message);
}
