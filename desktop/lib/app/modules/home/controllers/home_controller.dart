import 'dart:async';
import 'dart:io';

import 'package:get/get.dart';

class HomeController extends GetxController {
  var fajl = "reading...".obs;

  File get _localFile {
    const path = "C:\\test.txt";
    return File(path);
  }

  Future<String> readContent() async {
    try {
      final file = _localFile;
      return await file.readAsString();
    } catch (e) {
      return "ERROR";
    }
  }

  @override
  void onInit() {
    Timer.periodic(const Duration(milliseconds: 100), (c) async {
      fajl.value = await readContent();
    });
    super.onInit();
  }
}
