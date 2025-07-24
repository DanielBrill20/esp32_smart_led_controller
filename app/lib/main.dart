import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

enum ColorEnum {red, green, blue}

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return ChangeNotifierProvider(
      create: (context) => LedState(),
      child: MaterialApp(
        title: 'ESP32 RGB LED Remote Control',
        theme: ThemeData(
          colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepOrange),
        ),
        home: const MyHomePage(),
      ),
    );
  }
}

class LedState extends ChangeNotifier {
  List<int> colorList = [0, 0, 0];
  bool lightOn = false;

  void setColor(ColorEnum color, int value) {
    colorList[color.index] = value;
    notifyListeners();
  }

  void setLightOn(bool value) {
    lightOn = value;
    notifyListeners();
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

class RemotePage extends StatelessWidget {
  const RemotePage({super.key});

  @override
  Widget build(BuildContext context) {
    final theme = Theme.of(context);
    var ledState = context.watch<LedState>();

    return Padding(
      padding: const EdgeInsets.all(3),
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          // LIGHT
          FeatureSection(
            theme: theme,
            ledState: ledState,
            title: 'Light',
            children: [
              Expanded(
                child: Center(
                  child: Switch(
                    value: ledState.lightOn,
                    onChanged: (value) {
                      ledState.setLightOn(value);
                    },
                  ),
                ),
              ),
              Expanded(
                flex: 2,
                child: SizedBox(),
              ),
            ],
          ),
          // BLINKY
          FeatureSection(
            theme: theme,
            ledState: ledState,
            title: 'Blinky',
            children: [
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
          // MORSE CODE
          FeatureSection(
            theme: theme,
            ledState: ledState,
            title: 'Morse Code',
            children: [
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
          // COLOR
          FeatureSection(
            theme: theme,
            ledState: ledState,
            title: 'Color', 
            children: [
              Column(
                children: [
                  RGBSlider(
                    rgbIndex: ColorEnum.red,
                  ),
                  RGBSlider(
                    rgbIndex: ColorEnum.green,
                  ),
                  RGBSlider(
                    rgbIndex: ColorEnum.blue,
                  ),
                ],
              ),
              Expanded(
                flex: 2,
                child: Padding(
                  padding: const EdgeInsets.only(left: 8),
                  child: SizedBox(
                    height: 200,
                    child: ColoredBox(color: Color.fromRGBO(
                      ledState.colorList[ColorEnum.red.index],
                      ledState.colorList[ColorEnum.green.index],
                      ledState.colorList[ColorEnum.blue.index], 1)
                    ),
                  ),
                ),
              ),
            ],
          ),
        ],
      ),
    );
  }
}

class FeatureSection extends StatelessWidget {
  const FeatureSection({
    super.key,
    required this.theme,
    required this.ledState,
    required this.title,
    required this.children,
  });

  final ThemeData theme;
  final LedState ledState;
  final String title;
  final List<Widget> children;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
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
                      for (var widget in WordsToTextWidgets(title, theme.textTheme.headlineMedium))
                        widget,
                    ],
                  ),
                ),
              ),
              for (var widget in children)
                widget,
            ],
          ),
        ),
      ),
    );
  }
}

class RGBSlider extends StatefulWidget {
  const RGBSlider({
    super.key,
    required this.rgbIndex,
  });
  final ColorEnum rgbIndex;

  @override
  State<RGBSlider> createState() => _RGBSliderState();
}

class _RGBSliderState extends State<RGBSlider> {
  late final TextEditingController _controller;

  @override
  void initState() {
    super.initState();
    final ledState = context.read<LedState>();
    _controller = TextEditingController(
      text: '${ledState.colorList[widget.rgbIndex.index]}'
    );
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
  }

  @override
  void didUpdateWidget(covariant RGBSlider oldWidget) {
    super.didUpdateWidget(oldWidget);
    final ledState = context.read<LedState>();
    final currentValue = _controller.text;
    final newValue = '${ledState.colorList[widget.rgbIndex.index]}';

    if (currentValue != newValue) {
      _controller.text = newValue;
    }
  }

  @override
  Widget build(BuildContext context) {
    var ledState = context.watch<LedState>();
    var colorValue = ledState.colorList[widget.rgbIndex.index];

    Color sliderColor;
    switch (widget.rgbIndex) {
      case ColorEnum.red:
        sliderColor = Colors.red;
        break;
      case ColorEnum.green:
        sliderColor = Colors.green;
        break;
      case ColorEnum.blue:
        sliderColor = Colors.blue;
        break;
    }

    return Padding(
      padding: const EdgeInsets.all(8.0),
      child: Row(
        children: [
          Slider(
            value: colorValue.toDouble(),
            min: 0,
            max: 255,
            onChanged: (value) {
              ledState.setColor(widget.rgbIndex, value.round());
            },
            activeColor: sliderColor,
          ),
          SizedBox(
            width: 55,
            child: TextField(
              keyboardType: TextInputType.number,
              decoration: InputDecoration(border: OutlineInputBorder()),
              controller: _controller,
              onChanged: (value) {
                if (value == '') {
                  ledState.setColor(widget.rgbIndex, 0);
                } else {
                  var inputVal = int.tryParse(value);
                  if (inputVal == null || inputVal < 0 || inputVal > 255) {
                    ledState.setColor(widget.rgbIndex, ledState.colorList[widget.rgbIndex.index]);
                  } else {
                    ledState.setColor(widget.rgbIndex, inputVal);
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

List<Text> WordsToTextWidgets(String words, TextStyle? style) {
  List<Text> textWidgets = [];
  for (var word in words.split(' ')) {
    Text text = Text(
      word,
      style: style
    );
    textWidgets.add(text);
  }
  return textWidgets;
}