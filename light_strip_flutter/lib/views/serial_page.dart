import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:light_strip_application/blocs/serial_data/serial_data_bloc.dart';
import 'package:light_strip_application/blocs/serial/serial_bloc.dart';
import 'package:light_strip_application/cubits/theme_cubit.dart';

class SerialPage extends StatelessWidget {
  const SerialPage({super.key});

  @override
  Widget build(BuildContext context) {
    return BlocProvider(
      create: (context) => SerialBloc()..add(SerialInitialize()),
      child: const SerialView(),
    );
  }
}

class SerialView extends StatefulWidget {
  const SerialView({super.key});

  @override
  State<SerialView> createState() => _SerialViewState();
}

class _SerialViewState extends State<SerialView> {
  final TextEditingController _messageController = TextEditingController();
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Serial')),
      body: Column(
        children: [
          BlocListener<SerialBloc, SerialState>(
            listener: (context, state) {
              if (state is SerialError) {
                _dialogBuilder(
                  context: context,
                  error: state.message,
                  onOkayPressed: () =>
                      context.read<SerialBloc>().add(SerialInitialize()),
                );
              }
            },
            child: BlocBuilder<SerialBloc, SerialState>(
              builder: (context, state) {
                if (state is SerialInitial) {
                  return CircularProgressIndicator();
                } else if (state is SerialAvailable) {
                  return Column(
                    spacing: 12,
                    children: [
                      DropdownButton<String>(
                        value: state.selectedPort,
                        hint: Text('Select Port'),
                        items: state.availablePorts.map((port) {
                          return DropdownMenuItem(
                            value: port,
                            child: Text(port),
                          );
                        }).toList(),
                        onChanged: (port) {
                          if (port != null) {
                            context
                                .read<SerialBloc>()
                                .add(SerialSelectPort(port));
                          }
                        },
                      ),
                      Row(
                        mainAxisAlignment: MainAxisAlignment.center,
                        children: [
                          ElevatedButton(
                            onPressed: state.selectedPort != null
                                ? () => context
                                    .read<SerialBloc>()
                                    .add(SerialConnect(state.selectedPort!))
                                : null,
                            child: Text('Connect'),
                          ),
                          SizedBox(width: 8),
                          ElevatedButton(
                            onPressed: () => context
                                .read<SerialBloc>()
                                .add(SerialRefreshPorts()),
                            child: Text('Refresh'),
                          ),
                        ],
                      ),
                    ],
                  );
                } else if (state is SerialConnected) {
                  return Center(
                    child: Column(
                      spacing: 12,
                      crossAxisAlignment: CrossAxisAlignment.center,
                      children: [
                        Text(
                          style: TextStyle(fontSize: 16),
                          'Connected to: ${state.portName}',
                        ),
                        ElevatedButton(
                          onPressed: () => context
                              .read<SerialBloc>()
                              .add(SerialDisconnect()),
                          child: Text('Disconnect'),
                        ),
                        BlocProvider(
                          create: (context) => SerialDataBloc()
                            ..add(SerialDataInitialize(port: state.port)),
                          child: BlocBuilder<SerialDataBloc, SerialDataState>(
                            builder: (dataContext, dataState) {
                              if (dataState is SerialDataInitial) {
                                return CircularProgressIndicator();
                              } else if (dataState is SerialDataReady) {
                                return Center(
                                  child: Column(
                                    crossAxisAlignment:
                                        CrossAxisAlignment.center,
                                    spacing: 12,
                                    children: [
                                      Wrap(
                                        spacing: 12,
                                        runSpacing: 12,
                                        children: [
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data: 'CRAINBOW'))
                                                  },
                                              child: Text('CRAINBOW')),
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data: 'CBREATHING'))
                                                  },
                                              child: Text('CBREATHING')),
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data: 'CFIRE'))
                                                  },
                                              child: Text('CFIRE')),
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data: 'CWAVE'))
                                                  },
                                              child: Text('CWAVE')),
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data: 'CTWINKLE'))
                                                  },
                                              child: Text('CTWINKLE')),
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data: 'CCHASE'))
                                                  },
                                              child: Text('CCHASE')),
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data: 'STOP'))
                                                  },
                                              child: Text('STOP')),
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data: 'RAINBOW'))
                                                  },
                                              child: Text('RAINBOW')),
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data: 'RAINBOW'))
                                                  },
                                              child: Text('RAINBOW')),
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data:
                                                                'RGB,255,20,147'))
                                                  },
                                              child: Text('PINK')),
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data:
                                                                'RGB,100,0,0'))
                                                  },
                                              child: Text('RED')),
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data:
                                                                'RGB,75,0,226'))
                                                  },
                                              child: Text('PURPLE')),
                                          ElevatedButton(
                                              onPressed: () => {
                                                    dataContext
                                                        .read<SerialDataBloc>()
                                                        .add(SerialDataSend(
                                                            port: state.port,
                                                            data: 'OFF'))
                                                  },
                                              child: Text('OFF')),
                                        ],
                                      ),
                                      Row(
                                        mainAxisSize: MainAxisSize.max,
                                        crossAxisAlignment:
                                            CrossAxisAlignment.center,
                                        children: [
                                          SizedBox(
                                            width: 200,
                                            child: TextField(
                                              controller: _messageController,
                                              decoration: InputDecoration(
                                                hintText:
                                                    'Enter message to send',
                                              ),
                                            ),
                                          ),
                                          SizedBox(width: 8),
                                          ElevatedButton(
                                            onPressed: () {
                                              final message =
                                                  _messageController.text;
                                              if (message.isNotEmpty) {
                                                dataContext
                                                    .read<SerialDataBloc>()
                                                    .add(SerialDataSend(
                                                        port: state.port,
                                                        data: message));
                                                _messageController.clear();
                                              }
                                            },
                                            child: Text('Send'),
                                          ),
                                        ],
                                      ),
                                      Row(
                                        children: [
                                          ElevatedButton(
                                            onPressed: dataState.isListening
                                                ? () => dataContext
                                                    .read<SerialDataBloc>()
                                                    .add(
                                                        SerialDataStopListening(
                                                            state.port))
                                                : () => dataContext
                                                    .read<SerialDataBloc>()
                                                    .add(
                                                        SerialDataStartListening(
                                                            state.port)),
                                            child: Text(dataState.isListening
                                                ? 'Stop Listening'
                                                : 'Start Listening'),
                                          ),
                                        ],
                                      ),
                                      Row(
                                        spacing: 16,
                                        children: [
                                          Column(
                                            crossAxisAlignment:
                                                CrossAxisAlignment.start,
                                            children: [
                                              Text('Sent Messages:'),
                                              SizedBox(
                                                height: 200,
                                                width: 200,
                                                child: ListView.builder(
                                                  itemCount: dataState
                                                      .sentMessages.length,
                                                  itemBuilder:
                                                      (dataContext, index) {
                                                    return Text(
                                                        '> ${dataState.sentMessages[index]}');
                                                  },
                                                ),
                                              ),
                                            ],
                                          ),
                                          Column(
                                            crossAxisAlignment:
                                                CrossAxisAlignment.start,
                                            children: [
                                              Text('Received Messages:'),
                                              SizedBox(
                                                width: 200,
                                                height: 200,
                                                child: ListView.builder(
                                                  itemCount: dataState
                                                      .receivedMessages.length,
                                                  itemBuilder:
                                                      (dataContext, index) {
                                                    return Text(
                                                        '< ${dataState.receivedMessages[index]}');
                                                  },
                                                ),
                                              ),
                                            ],
                                          ),
                                        ],
                                      ),
                                    ],
                                  ),
                                );
                              } else if (dataState is SerialDataError) {
                                return Text('Data Error: ${dataState.message}');
                              } else {
                                return Container();
                              }
                            },
                          ),
                        ),
                      ],
                    ),
                  );
                } else {
                  return Container();
                }
              },
            ),
          ),
        ],
      ),
      floatingActionButton: Column(
        crossAxisAlignment: CrossAxisAlignment.end,
        mainAxisAlignment: MainAxisAlignment.end,
        children: <Widget>[
          FloatingActionButton(
            child: const Icon(Icons.brightness_6),
            onPressed: () {
              context.read<ThemeCubit>().toggleTheme();
            },
          ),
        ],
      ),
    );
  }
}

Future<void> _dialogBuilder(
    {required BuildContext context,
    required String error,
    required VoidCallback onOkayPressed}) {
  return showDialog<void>(
    context: context,
    builder: (BuildContext context) {
      return AlertDialog(
        title: const Text('Error'),
        content: Text(error),
        actions: <Widget>[
          TextButton(
            style: TextButton.styleFrom(
                textStyle: Theme.of(context).textTheme.labelLarge),
            child: const Text('Okay'),
            onPressed: () {
              Navigator.of(context).pop();
              onOkayPressed();
            },
          ),
        ],
      );
    },
  );
}
