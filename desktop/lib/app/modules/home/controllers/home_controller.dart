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
  var texts = <Text>[].obs;
  var locked = false.obs;

  List<User> oldUsers = [];
  RxList<User> users = RxList<User>();
  Queue<User> red = Queue();

  List<TextSpan> generateSpan(String kveri, String text) {
    List<String> svi = kveri.toLowerCase().split(" ");
    String origin = text;
    for (String jedan in svi) {
      origin = origin.toLowerCase().replaceAll(RegExp(jedan), "|$jedan|");
    }
    bool open = false;

    List<TextSpan> spans = [];
    int offset = 0;
    for (int i = 0; i < origin.length; i++) {
      if (origin[i] != '|') {
        spans.add(
          TextSpan(
            text: text[i - offset],
            style: TextStyle(
              fontWeight: open ? FontWeight.bold : FontWeight.normal,
            ),
          ),
        );
      } else {
        open = !open;
        offset++;
      }
    }
    return spans;
  }

  void getUsers() {
    final f = File("${AppConstants.root}USERS.json");
    Map<String, dynamic> json = jsonDecode(f.readAsStringSync());
    users.clear();
    texts.clear();
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
      bool prod = kveri == "prisutan" && value["is_present"] ||
          kveri == "odsutan" && !value["is_present"];
      if (prod) kveri = "";
      if (prod || any) {
        users.add(User(
          key,
          value["ime"],
          value["prezime"],
          value["is_present"],
        ));

        texts.add(
          Text.rich(
            TextSpan(
              children: [
                TextSpan(text: "${users.length}. "),
                TextSpan(
                    children:
                        generateSpan(search.value.trim(), users.last.ime)),
                const TextSpan(text: " "),
                TextSpan(
                    children:
                        generateSpan(search.value.trim(), users.last.prezime)),
              ],
            ),
          ),
        );
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
