#include "gmsk.h"
#include "oqpsk.h"

#include <iostream>

template<class T>
static void vClassTask(void *pvParameters) {
    (static_cast<T *>(pvParameters))->execute();
}

extern "C" void main_cpp(){

    //xTaskCreate(vClassTask<DummyTask>, "Dummy", 2000, nullptr, tskIDLE_PRIORITY + 1, NULL);
    //vTaskStartScheduler();
    return;
}
