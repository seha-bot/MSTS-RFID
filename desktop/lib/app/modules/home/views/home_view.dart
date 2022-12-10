import 'package:desktop/app/routes/app_pages.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';
import 'package:get/get.dart';

import '../controllers/home_controller.dart';

class HomeView extends GetView<HomeController> {
  const HomeView({super.key});

  @override
  Widget build(BuildContext context) {
    return Focus(
      autofocus: true,
      onKeyEvent: (node, event) {
        if (event.physicalKey == PhysicalKeyboardKey.f12) {
          Get.toNamed(Routes.ACCOUNT);
          return KeyEventResult.handled;
        }
        return KeyEventResult.ignored;
      },
      child: Scaffold(
        body: Align(
          alignment: Alignment.topCenter,
          child: ConstrainedBox(
            constraints: const BoxConstraints(maxWidth: 700.0),
            child: SingleChildScrollView(
              controller: controller.scroll,
              child: Column(
                children: [
                  Container(
                    margin: const EdgeInsets.all(20.0),
                    child: const SearchBox(),
                  ),
                  Obx(
                    () => Visibility(
                      visible: controller.locked.value,
                      child: Container(
                        margin: const EdgeInsets.only(top: 10.0, bottom: 20.0),
                        child: Text(
                          controller.red.isEmpty
                              ? ""
                              : "${controller.red.first.isPresent ? "Doviđenja" : "Dobrodošao"} ${controller.red.first}",
                          style: const TextStyle(fontSize: 40),
                        ),
                      ),
                    ),
                  ),
                  Container(
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
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }
}

class SearchBox extends GetView<HomeController> {
  const SearchBox({
    Key? key,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      height: 48,
      child: TextField(
        onChanged: controller.search,
        cursorHeight: 20,
        decoration: InputDecoration(
          contentPadding: EdgeInsets.zero,
          hintText: "Pretraživanje",
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
          prefixIcon: const Icon(Icons.search),
        ),
      ),
    );
  }
}
