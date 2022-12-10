import 'dart:async';
import 'dart:convert';
import 'dart:io';

import 'package:get/get.dart';

class User {
  User(this.tag, this.ime, this.prezime, this.isPresent);

  final String tag;
  final String ime;
  final String prezime;
  bool isPresent;

  @override
  String toString() {
    return "$ime $prezime";
  }
}

class HomeController extends GetxController {
  var fajl = "reading...".obs;

  final root = "C:\\db";
  RxList<User> users = RxList<User>();

  void getUsers() async {
    final f = File("$root\\USERS.json");
    Map<String, dynamic> json = jsonDecode(f.readAsStringSync());
    users.clear();
    json.forEach((key, value) {
      if (value["ime"] == "uprava") return;
      users.add(User(
        key,
        value["ime"],
        value["prezime"],
        value["is_present"],
      ));
    });
  }

  @override
  void onInit() {
    getUsers();
    Timer.periodic(const Duration(milliseconds: 100), (c) {
      getUsers();
    });
    super.onInit();
  }
}
