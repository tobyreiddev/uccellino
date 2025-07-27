part of 'serial_data_bloc.dart';

@immutable
sealed class SerialDataState {}

final class SerialDataInitial extends SerialDataState {}

class SerialDataReady extends SerialDataState {
  final SerialPort selectedPort;
  final bool isListening;
  final List<String> sentMessages;
  final List<String> receivedMessages;

  SerialDataReady({
    required this.selectedPort,
    required this.isListening,
    required this.sentMessages,
    required this.receivedMessages,
  });

  SerialDataReady copyWith({
    SerialPort? selectedPort,
    bool? isListening,
    List<String>? sentMessages,
    List<String>? receivedMessages,
  }) {
    return SerialDataReady(
      selectedPort: selectedPort ?? this.selectedPort,
      isListening: isListening ?? this.isListening,
      sentMessages: sentMessages ?? this.sentMessages,
      receivedMessages: receivedMessages ?? this.receivedMessages,
    );
  }
}

class SerialDataError extends SerialDataState {
  final String message;
  SerialDataError(this.message);
}
