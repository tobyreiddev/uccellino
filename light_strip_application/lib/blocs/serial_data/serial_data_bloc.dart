import 'dart:typed_data';

import 'package:bloc/bloc.dart';
import 'package:flutter_libserialport/flutter_libserialport.dart';
import 'package:meta/meta.dart';

part 'serial_data_event.dart';
part 'serial_data_state.dart';

class SerialDataBloc extends Bloc<SerialDataEvent, SerialDataState> {
  SerialDataBloc() : super(SerialDataInitial()) {
    on<SerialDataInitialize>(_onInitialize);
    on<SerialDataSend>(_onSend);
    on<SerialDataStartListening>(_onStartListening);
    on<SerialDataStopListening>(_onStopListening);
    on<SerialDataReceived>(_onDataReceived);
  }

  Future<void> _onInitialize(
    SerialDataInitialize event,
    Emitter<SerialDataState> emit,
  ) async {
    emit(
      SerialDataReady(
        isListening: false,
        receivedMessages: [],
        selectedPort: event.port,
        sentMessages: [],
      ),
    );
  }

  Future<void> _onSend(
    SerialDataSend event,
    Emitter<SerialDataState> emit,
  ) async {
    try {
      final port = event.port;
      final data = event.data;
      final List<String> sentMessages =
          List.from((state as SerialDataReady).sentMessages);

      final bytes = Uint8List.fromList(('$data\n').codeUnits);

      final bytesWritten = port.write(bytes);

      if (bytesWritten > 0) {
        sentMessages.add(data);

        if (state is SerialDataReady) {
          final currentState = state as SerialDataReady;
          emit(currentState.copyWith(
            sentMessages: sentMessages,
          ));
        }
      } else {
        emit(SerialDataError('Failed to send data: $port'));
      }
    } catch (e) {
      emit(SerialDataError('Failed to send data: $e'));
    }
  }

  Future<void> _onStartListening(
    SerialDataStartListening event,
    Emitter<SerialDataState> emit,
  ) async {
    try {
      final port = event.port;
      final reader = SerialPortReader(port);
      final isListening = true;

      reader.stream.listen(
        (data) {
          add(SerialDataReceived(data));
        },
        onError: (error) {
          add(SerialDataStopListening(port));
        },
      );

      if (state is SerialDataReady) {
        final currentState = state as SerialDataReady;
        emit(currentState.copyWith(isListening: isListening));
      }
    } catch (e) {
      emit(SerialDataError('Failed to start listening: $e'));
    }
  }

  Future<void> _onStopListening(
    SerialDataStopListening event,
    Emitter<SerialDataState> emit,
  ) async {
    try {
      final port = event.port;
      final reader = SerialPortReader(port);
      reader.close();
      final isListening = false;

      if (state is SerialDataReady) {
        final currentState = state as SerialDataReady;
        emit(currentState.copyWith(isListening: isListening));
      }
    } catch (e) {
      emit(SerialDataError('Failed to stop listening: $e'));
    }
  }

  Future<void> _onDataReceived(
    SerialDataReceived event,
    Emitter<SerialDataState> emit,
  ) async {
    try {
      final List<String> receivedMessages = List.from(
        (state as SerialDataReady).receivedMessages,
      );
      final dataString = String.fromCharCodes(event.data);
      receivedMessages.add(dataString);

      if (state is SerialDataReady) {
        final currentState = state as SerialDataReady;
        emit(
          currentState.copyWith(
            receivedMessages: receivedMessages,
          ),
        );
      }
    } catch (e) {
      emit(SerialDataError('Failed to process received data: $e'));
    }
  }
}
