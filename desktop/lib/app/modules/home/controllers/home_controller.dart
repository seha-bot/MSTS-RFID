import 'dart:async';
import 'dart:collection';
import 'dart:convert';
import 'dart:io';

import 'package:desktop/app/interface/constants.dart';
import 'package:desktop/app/interface/models.dart';
import 'package:flutter/material.dart';
import 'package:get/get.dart';

class HomeController extends GetxController {
  ScrollController scroll = ScrollController();
  var search = "".obs;
  var locked = false.obs;

  List<User> oldUsers = [];
  RxList<User> users = RxList<User>();
  Queue<User> red = Queue();

  void getUsers() {
    final f = File("${AppConstants.root}\\USERS.json");
    Map<String, dynamic> json = jsonDecode(f.readAsStringSync());
    users.clear();
    json.forEach((key, value) {
      if (value["ime"] == "uprava") return;
      String kveri = search.value.toLowerCase().trim();
      bool any = false;
      kveri.split(" ").forEach((combination) {
        if (value["ime"].toLowerCase().contains(combination) ||
            value["prezime"].toLowerCase().contains(combination)) {
          any = true;
        }
      });
      if (kveri == "prisutan" && value["is_present"] ||
          kveri == "odsutan" && !value["is_present"] ||
          any) {
        users.add(User(
          key,
          value["ime"],
          value["prezime"],
          value["is_present"],
        ));
      }
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
      if (locked.value) return;
      getUsers();
      findDiff();
      oldUsers = List.from(users);
      locked.value = true;
      while (red.isNotEmpty) {
        scroll.animateTo(0.0,
            duration: const Duration(milliseconds: 300), curve: Curves.easeOut);
        await Future.delayed(const Duration(seconds: 1, milliseconds: 500));
        red.removeFirst();
      }
      locked.value = false;
    });
    super.onInit();
  }
}
