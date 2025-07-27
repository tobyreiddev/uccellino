part of 'serial_bloc.dart';

@immutable
sealed class SerialEvent {}

class SerialInitialize extends SerialEvent {}

class SerialRefreshPorts extends SerialEvent {}

class SerialSelectPort extends SerialEvent {
  final String portName;
  SerialSelectPort(this.portName);
}

class SerialConnect extends SerialEvent {
  final String portName;
  SerialConnect(this.portName);
}

class SerialDisconnect extends SerialEvent {}
