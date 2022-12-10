import 'package:flutter/material.dart';

import 'package:get/get.dart';

import '../controllers/account_controller.dart';

class AccountView extends GetView<AccountController> {
  const AccountView({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text("Dodavanje osoblja"),
        centerTitle: true,
      ),
      body: Center(
        child: ConstrainedBox(
          constraints: const BoxConstraints(maxWidth: 500.0),
          child: Obx(
            () => controller.logged.value
                ? Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      Text(
                        controller.tag.isEmpty
                            ? "Molimo skenirajte novu karticu"
                            : controller.tag.value,
                        style: const TextStyle(fontSize: 30),
                      ),
                      const Padding(padding: EdgeInsets.all(10.0)),
                      CustomInput(
                        hint: "Ime",
                        controller: controller.ime,
                      ),
                      const Padding(padding: EdgeInsets.all(10.0)),
                      CustomInput(
                        hint: "Prezime",
                        controller: controller.prezime,
                      ),
                      const Padding(padding: EdgeInsets.all(10.0)),
                      ElevatedButton(
                        onPressed: controller.createUser.value,
                        child: const Text("Spasi"),
                      ),
                      if (controller.result.isNotEmpty)
                        Text(
                          controller.result.value,
                          style: const TextStyle(
                            fontSize: 30,
                            color: Colors.green,
                          ),
                        ),
                    ],
                  )
                : Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      Container(
                        height: 48,
                        margin: const EdgeInsets.only(bottom: 20.0),
                        child: TextField(
                          onChanged: controller.password,
                          cursorHeight: 20,
                          obscureText: controller.hidden.value,
                          decoration: InputDecoration(
                            contentPadding: const EdgeInsets.all(15.0),
                            hintText: "Lozinka",
                            enabledBorder: OutlineInputBorder(
                              borderRadius: BorderRadius.circular(8),
                              borderSide: const BorderSide(
                                color: Colors.grey,
                                width: 0.5,
                              ),
                            ),
                            focusedBorder: OutlineInputBorder(
                              borderRadius: BorderRadius.circular(8),
                              borderSide: BorderSide(
                                color: Colors.grey.shade500,
                              ),
                            ),
                            suffixIcon: InkWell(
                              borderRadius: BorderRadius.circular(60.0),
                              onTap: controller.hidden.toggle,
                              child: const Icon(Icons.remove_red_eye),
                            ),
                          ),
                        ),
                      ),
                      ElevatedButton(
                          onPressed: controller.submit,
                          child: const Text("Dalje")),
                    ],
                  ),
          ),
        ),
      ),
    );
  }
}

class CustomInput extends StatelessWidget {
  const CustomInput({
    Key? key,
    required this.hint,
    required this.controller,
  }) : super(key: key);

  final String hint;
  final TextEditingController controller;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      height: 48,
      child: TextField(
        controller: controller,
        cursorHeight: 20,
        decoration: InputDecoration(
          contentPadding: const EdgeInsets.all(15.0),
          hintText: hint,
          enabledBorder: OutlineInputBorder(
            borderRadius: BorderRadius.circular(8),
            borderSide: const BorderSide(
              color: Colors.grey,
              width: 0.5,
            ),
          ),
          focusedBorder: OutlineInputBorder(
            borderRadius: BorderRadius.circular(8),
            borderSide: BorderSide(
              color: Colors.grey.shade500,
            ),
          ),
        ),
      ),
    );
  }
}
