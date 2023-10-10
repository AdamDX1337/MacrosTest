#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

std::mutex mtx;
std::atomic<bool> shouldStop(false);

static void DontUse() {
    bool keyPressed = false;
    int i = 0;
    if (GetAsyncKeyState(VK_CONTROL) & 0x8000 ? !keyPressed : keyPressed) {

        keyPressed = !keyPressed;

        if (keyPressed) {
            i++;
            std::cout << "Space Key Pressed " << i << " times\n";
            keybd_event(VK_SPACE, 0, KEYEVENTF_EXTENDEDKEY, 0);
            Sleep(1);
            keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
        }


    }
}

static void TimerThread(int* track);
static void TestClick();




static void TurboSpace(INPUT input);
static void TurboLeftMouse(INPUT input, int delay);

int main()
{
    int clickTrack = 0;
    int spaceTrack = 0;
    int delay;
    INPUT a = { 0 };
    std::cout << "(Anything below 1ms gets weird)\nEnter Delay Value in ms for autoclicker:";
    std::cin >> delay;


    std::cout << "Hold Numpad 1 to autoclick\nHold Control to BHOP\n\n\nTESTING ONLY, DO NOT USE!\nNumpad 5 to test click speed\n";

    while (true)
    {
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
            TurboSpace(a);
            spaceTrack++;
        }
        else if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000) {
            TurboLeftMouse(a, delay);
            clickTrack++;
        }
        else if (GetAsyncKeyState(VK_NUMPAD5) & 0x8000) {
            TestClick();
            Sleep(10000);
            std::cout << "Done";
        }

        if (clickTrack == 1000) {
            std::cout << "Clicked 1000x\n";
            clickTrack = 0;
        }
        if (spaceTrack == 1000) {
            std::cout << "Pressed Space 1000x\n";
            spaceTrack = 0;
        }

    }

}

static void TurboSpace(INPUT input) {
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = VkKeyScanA(' ');
    SendInput(1, &input, sizeof(input));
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(input));

}

static void TurboLeftMouse(INPUT input, int delay) {
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(input));
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(input));
    Sleep(delay);
}


static void TimerThread(int* track) {
    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    {
        std::lock_guard<std::mutex> lock(mtx);
        shouldStop.store(true);
    }
}

static void TestClick() {
    INPUT input = { 0 };
    int track = 0;
    std::thread timer(TimerThread, &track);

    while (true) {
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &input, sizeof(input));
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(input));
        {
            std::lock_guard<std::mutex> lock(mtx);
            track++;
        }


        {
            std::lock_guard<std::mutex> lock(mtx);
            if (shouldStop.load()) {
                break;
            }
        }
    }

    timer.join();
    int test;
    double cps = track / 5;
    std::cout << "Clicks Per Second:" << cps << std::endl;
    std::cout << "Total Clicks: " << track << std::endl;
    std::cin >> test;
}
