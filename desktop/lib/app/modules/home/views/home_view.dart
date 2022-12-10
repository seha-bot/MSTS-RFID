import 'package:flutter/material.dart';
import 'package:get/get.dart';

import '../controllers/home_controller.dart';

class HomeView extends GetView<HomeController> {
  const HomeView({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Align(
        alignment: Alignment.topCenter,
        child: Container(
          constraints: const BoxConstraints(maxWidth: 700.0),
          margin: const EdgeInsets.all(20.0),
          decoration: BoxDecoration(
            borderRadius: BorderRadius.circular(10.0),
            border: Border.all(width: 0.5, color: Colors.grey),
          ),
          child: Obx(
            () => Column(
              mainAxisSize: MainAxisSize.min,
              children: [
                for (int i = 0; i < controller.users.length; i++) ...[
                  Container(
                    padding: const EdgeInsets.all(15.0),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.center,
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Expanded(
                          child: Text(
                            "${i + 1}. ${controller.users[i].ime} ${controller.users[i].prezime}",
                          ),
                        ),
                        Text(
                          controller.users[i].isPresent
                              ? "Prisutan"
                              : "Odsutan",
                          style: TextStyle(
                            color: controller.users[i].isPresent
                                ? Colors.green
                                : Colors.red,
                          ),
                        ),
                      ],
                    ),
                  ),
                  if (i != controller.users.length - 1)
                    const Divider(
                      color: Colors.grey,
                      thickness: 0.5,
                      height: 0.0,
                    ),
                ],
              ],
            ),
          ),
        ),
      ),
    );
  }
}
