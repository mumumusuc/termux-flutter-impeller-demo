import 'package:flutter/material.dart';
import 'package:vector_math/vector_math_64.dart' hide Colors;
import 'dart:math';
import 'camera.dart';
import 'scene.dart';

void main() {
  runApp(const MyHomePage());
}

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key});

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> with SingleTickerProviderStateMixin{
  late Animation<double> animation;
  late AnimationController controller;

  initState() {
    super.initState();
    controller = AnimationController(
        duration: Duration(seconds: 10),
        vsync: this
    );
    animation = Tween<double>(begin: 0, end: 2*pi).animate(controller);
    controller.repeat();
  }

  @override
  void dispose() {
    controller.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    var node = Node(
      children: [
        Node.asset("models/flutter_logo.glb"),
        Node.asset("models/sky_sphere.glb"),
      ],
    );

    return AnimatedBuilder(
      animation: animation,
      builder: (_, __) {
        var z = 4 * cos(animation.value);
        var x = 4 * sin(animation.value);
        return SceneBox(
          camera: Camera(position: Vector3(x, 0.4, -z)),
          root: node,
        );
      }
    );
  }
}
