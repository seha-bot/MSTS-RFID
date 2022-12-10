import 'dart:async';
import 'dart:convert';
import 'dart:developer';
import 'dart:io';

import 'package:desktop/app/interface/constants.dart';
import 'package:desktop/app/interface/models.dart';
import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:get/get_connect/http/src/request/request.dart';

class AccountController extends GetxController {
  var correct = "123456";
  var password = "".obs;
  var hidden = true.obs;
  var logged = false.obs;
  var result = "".obs;

  TextEditingController ime = TextEditingController();
  TextEditingController prezime = TextEditingController();
  var tag = "".obs;

  void readTag() {
    final ft = File("${AppConstants.root}\\NEW");
    final fu = File("${AppConstants.root}\\USERS.json");
    Map<String, dynamic> json = jsonDecode(fu.readAsStringSync());

    try {
      tag.value = ft.readAsStringSync().trim();
    } catch (e) {
      tag.value = "";
    }
    for (var key in json.keys) {
      if (key == tag.value) {
        tag.value = "";
        return;
      }
    }
  }

  Rxn<Function()> createUser = Rxn();

  void submit() {
    if (password.value == correct) {
      logged.value = true;
    }
  }

  Future<void> addUser(User user) async {
    try {
      final connect = GetConnect();
      await connect.patch(
        "${AppConstants.baseUrl}${AppConstants.usersEndpoint}/${user.tag}${AppConstants.auth}",
        {
          "ime": user.ime,
          "prezime": user.prezime,
        },
      );
      result.value = "Korisnik uspjesno upisan";
    } catch (e) {
      log("$e");
      result.value = "";
    }
  }

  @override
  void onInit() {
    Timer.periodic(const Duration(milliseconds: 100), (c) {
      readTag();
      if (ime.text.isNotEmpty &&
          prezime.text.isNotEmpty &&
          tag.value.isNotEmpty &&
          result.isEmpty) {
        createUser.value = () {
          User user = User(tag.value, ime.text, prezime.text, false);
          addUser(user);
        };
      } else {
        createUser.value = null;
      }
    });
    super.onInit();
  }
}
