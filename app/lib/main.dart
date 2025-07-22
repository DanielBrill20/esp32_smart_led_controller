import 'package:flutter/material.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ESP32 RGB LED Remote Control',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepOrange),
      ),
      home: const MyHomePage(),
    );
  }
}
class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key});

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  int selectedPageIndex = 0;
  Widget selectedPage = RemotePage();
  var commsSwitchSelected = 'ble';

  @override
  Widget build(BuildContext context) {
    switch (selectedPageIndex) {
      case 0:
        selectedPage = RemotePage();
        break;
      case 1:
        selectedPage = Placeholder();
        break;
      default:
        throw UnimplementedError('no widget for selectedPageIndex: $selectedPageIndex');
    }

    return Scaffold(
      appBar: AppBar(
        title: Row(
          children: [
            Expanded(child: Text('ESP32 LED Remote')),
            SegmentedButton(
              segments: [
                ButtonSegment(
                  value: 'ble',
                  icon: Icon(Icons.bluetooth),
                ),
                ButtonSegment(
                  value: 'wifi',
                  icon: Icon(Icons.wifi),
                ),
              ],
              selected: {commsSwitchSelected},
              onSelectionChanged: (value) {
                setState(() {
                  commsSwitchSelected=='ble' ? commsSwitchSelected='wifi' : commsSwitchSelected='ble';
                });
              },
              showSelectedIcon: false,
            ),
          ],
        ),
      ),
      body: selectedPage,
      bottomNavigationBar: SafeArea(
        child: BottomNavigationBar(
          items: [
            BottomNavigationBarItem(
              icon: Icon(Icons.lightbulb),
              label: 'Remote',
            ),
            BottomNavigationBarItem(
              icon: Icon(Icons.info_outline),
              label: 'Info',
            ),
          ],
          currentIndex: selectedPageIndex,
          onTap: (value) {
            setState(() {
              selectedPageIndex = value;
            });
          },
        ),
      ),
    );
  }
}

class RemotePage extends StatefulWidget {
  const RemotePage({super.key});

  @override
  State<RemotePage> createState() => _RemotePageState();
}

class _RemotePageState extends State<RemotePage> {
  var lightOn = false;
  int red = 0;
  int green = 0;
  int blue = 0;

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);

    return Padding(
      padding: const EdgeInsets.all(3),
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          // LIGHT
          Container(
            width: double.infinity,
            child: Card(
              color: theme.colorScheme.primary,
              child: Padding(
                padding: EdgeInsets.all(20),
                child: Row(
                  children: [
                    RotatedBox(
                      quarterTurns: 3,
                      child: Text(
                        'Light',
                        style: theme.textTheme.headlineMedium,
                      ),
                    ),
                    Expanded(
                      child: Center(
                        child: Switch(
                          value: lightOn,
                          onChanged: (value) {
                            setState(() {
                              lightOn = value;
                            });
                          },
                        ),
                      ),
                    ),
                  ],
                ),
              ),
            ),
          ),
          // BLINKY
          Container(
            width: double.infinity,
            child: Card(
              color: theme.colorScheme.primary,
              child: Padding(
                padding: EdgeInsets.all(20),
                child: Row(
                  children: [
                    Expanded(
                      flex: 2,
                      child: RotatedBox(
                        quarterTurns: 3,
                        child: Text(
                          'Blinky',
                          style: theme.textTheme.headlineMedium,
                        ),
                      ),
                    ),
                    Expanded(
                      flex: 5,
                      child: TextField(
                        keyboardType: TextInputType.number,
                        decoration: InputDecoration(
                          border: OutlineInputBorder(),
                          labelText: 'Duration',
                        ),
                      ),
                    ),
                    Expanded(flex: 1, child: Center(child: Text('ms'))),
                    ElevatedButton(
                      onPressed: () {},
                      child: Icon(Icons.send),
                    ),
                  ],
                ),
              ),
            ),
          ),
          // MORSE CODE
          Container(
            width: double.infinity,
            child: Card(
              color: theme.colorScheme.primary,
              child: Padding(
                padding: EdgeInsets.all(20),
                child: Row(
                  children: [
                    Expanded(
                      flex: 2,
                      child: RotatedBox(
                        quarterTurns: 3,
                        child: Column(
                          children: [
                            Text(
                              'Morse',
                              style: theme.textTheme.headlineMedium,
                            ),
                            Text(
                              'Code',
                              style: theme.textTheme.headlineMedium,
                            ),
                          ],
                        ),
                      ),
                    ),
                    Expanded(
                      flex: 5,
                      child: TextField(
                        keyboardType: TextInputType.text,
                        decoration: InputDecoration(
                          border: OutlineInputBorder(),
                          labelText: 'Message',
                        ),
                      ),
                    ),
                    Expanded(flex: 1, child: SizedBox()),
                    ElevatedButton(
                      onPressed: () {},
                      child: Icon(Icons.send),
                    ),
                  ],
                ),
              ),
            ),
          ),
          // COLOR
          Container(
            width: double.infinity,
            child: Card(
              color: theme.colorScheme.primary,
              child: Padding(
                padding: EdgeInsets.all(20),
                child: IntrinsicHeight(
                  child: Row(
                    crossAxisAlignment: CrossAxisAlignment.stretch,
                    children: [
                      RotatedBox(
                        quarterTurns: 3,
                        child: Text(
                          'Color',
                          style: theme.textTheme.headlineMedium,
                          textAlign: TextAlign.center,
                        ),
                      ),
                      Column(
                        children: [
                          RGBSlider(
                            color: Colors.red,
                            colorVal: red,
                            onChanged: (value) => setState(() => red = value),
                          ),
                          RGBSlider(
                            color: Colors.green,
                            colorVal: green,
                            onChanged: (value) => setState(() => green = value),
                          ),
                          RGBSlider(
                            color: Colors.blue,
                            colorVal: blue,
                            onChanged: (value) => setState(() => blue = value),
                          ),
                        ],
                      ),
                      Expanded(
                        child: Padding(
                          padding: const EdgeInsets.only(left: 8),
                          child: ColoredBox(color: Color.fromRGBO(red, green, blue, 1)),
                      )),
                    ],
                  ),
                ),
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class RGBSlider extends StatefulWidget {
  const RGBSlider({
    super.key,
    required this.color,
    required this.colorVal,
    required this.onChanged,
  });
  final Color color;
  final int colorVal;
  final ValueChanged<int> onChanged;

  @override
  State<RGBSlider> createState() => _RGBSliderState();
}

class _RGBSliderState extends State<RGBSlider> {
  var controller = TextEditingController();

  @override
  Widget build(BuildContext context) {
    controller.text = '${widget.colorVal}';

    return Padding(
      padding: const EdgeInsets.all(8.0),
      child: Row(
        children: [
          Slider(
            value: widget.colorVal.toDouble(),
            min: 0,
            max: 255,
            onChanged: (value) {
              widget.onChanged(value.round());
            },
            activeColor: widget.color,
          ),
          SizedBox(
            width: 55,
            child: TextField(
              keyboardType: TextInputType.number,
              decoration: InputDecoration(border: OutlineInputBorder()),
              controller: controller,
              onChanged: (value) {
                if (value == '') {
                  widget.onChanged(0);
                } else {
                  var inputVal = int.tryParse(value);
                  if (inputVal == null || inputVal < 0 || inputVal > 255) {
                    widget.onChanged(widget.colorVal);
                  } else {
                    widget.onChanged(inputVal);
                  }
                }
              },
            ),
          ),
        ],
      ),
    );
  }
}