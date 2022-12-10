import 'dart:async';
import 'dart:collection';
import 'dart:convert';
import 'dart:io';
import 'dart:math';

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
  bool locked = false;
  List<User> oldUsers = [];
  RxList<User> users = RxList<User>();
  Queue<User> red = Queue();

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

  void findDiff() {
    for (var old in oldUsers) {
      for (var novi in users) {
        if (old.tag == novi.tag) {
          if (old.isPresent != novi.isPresent) {
            red.add(old);
            return;
          }
          break;
        }
      }
    }
  }

  @override
  void onInit() {
    getUsers();
    oldUsers = List.from(users);
    Timer.periodic(const Duration(milliseconds: 100), (c) async {
      if (locked) return;
      getUsers();
      findDiff();
      oldUsers = List.from(users);
      locked = true;
      while (red.isNotEmpty) {
        print("${red.first}");
        red.removeFirst();
      }
      locked = false;
    });
    super.onInit();
  }
}
