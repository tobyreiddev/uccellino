import 'package:bloc/bloc.dart';
import 'package:flutter_libserialport/flutter_libserialport.dart';
import 'package:meta/meta.dart';

part 'serial_event.dart';
part 'serial_state.dart';

// BLoC
class SerialBloc extends Bloc<SerialEvent, SerialState> {
  SerialBloc() : super(SerialInitial()) {
    on<SerialInitialize>(_onInitialize);
    on<SerialRefreshPorts>(_onRefreshPorts);
    on<SerialSelectPort>(_onSelectPort);
    on<SerialConnect>(_onConnect);
    on<SerialDisconnect>(_onDisconnect);
  }

// Extract common logic
  Future<void> _refreshAvailablePorts(Emitter<SerialState> emit) async {
    try {
      final ports = SerialPort.availablePorts;
      emit(SerialAvailable(availablePorts: ports));
    } catch (e) {
      emit(SerialError('Failed to refresh ports: $e'));
    }
  }

  Future<void> _onInitialize(
      SerialInitialize event, Emitter<SerialState> emit) async {
    try {
      _refreshAvailablePorts(emit);
    } catch (e) {
      emit(SerialError('Failed to initialize: $e'));
    }
  }

  Future<void> _onRefreshPorts(
      SerialRefreshPorts event, Emitter<SerialState> emit) async {
    try {
      _refreshAvailablePorts(emit);
    } catch (e) {
      emit(SerialError('Failed to refresh ports: $e'));
    }
  }

  Future<void> _onSelectPort(
      SerialSelectPort event, Emitter<SerialState> emit) async {
    try {
      final selectedPortName = event.portName;
      final ports = (state as SerialAvailable).availablePorts;
      emit(SerialAvailable(
        availablePorts: ports,
        selectedPort: selectedPortName,
      ));
    } catch (e) {
      emit(SerialError('Failed to select port: $e'));
    }
  }

  Future<void> _onConnect(
      SerialConnect event, Emitter<SerialState> emit) async {
    if ((state as SerialAvailable).selectedPort == null) {
      emit(SerialError('No port selected'));
      return;
    }

    try {
      final portName = event.portName;
      final currentPort = SerialPort(portName);

      // Configure the port
      final config = SerialPortConfig();
      config.baudRate = 9600;
      config.bits = 8;
      config.parity = SerialPortParity.none;
      config.stopBits = 1;
      config.setFlowControl(SerialPortFlowControl.none);

      currentPort.config = config;

      if (currentPort.openReadWrite()) {
        emit(SerialConnected(
          portName: portName,
          port: currentPort,
        ));
      } else {
        emit(SerialError('Failed to open port: $currentPort'));
      }
    } catch (e) {
      emit(SerialError('Failed to connect: $e'));
    }
  }

  Future<void> _onDisconnect(
      SerialDisconnect event, Emitter<SerialState> emit) async {
    try {
      if (state is SerialConnected) {
        (state as SerialConnected).port.close();
      }
      _refreshAvailablePorts(emit);
    } catch (e) {
      emit(SerialError('Failed to disconnect: $e'));
    }
  }
}
